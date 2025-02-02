#include "GltfImporter/GltfImporterFile.hpp"

#include "GltfImporter/GltfAnimationImporter.hpp"
#include "GltfImporter/GltfCameraImporter.hpp"
#include "GltfImporter/GltfLightImporter.hpp"
#include "GltfImporter/GltfMaterialImporter.hpp"
#include "GltfImporter/GltfMeshImporter.hpp"
#include "GltfImporter/GltfSceneNodeImporter.hpp"
#include "GltfImporter/GltfSkeletonImporter.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Model/Mesh/Mesh.hpp>
#include <Castor3D/Model/Skeleton/Skeleton.hpp>
#include <Castor3D/Scene/Camera.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/SceneNode.hpp>
#include <Castor3D/Scene/Light/Light.hpp>

#include <CastorUtils/Design/ArrayView.hpp>

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <meshoptimizer.h>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

namespace c3d_gltf
{
	//*********************************************************************************************

	namespace file
	{
		static fastgltf::Expected< fastgltf::Asset > loadScene( castor::Path const & filePath )
		{
			fastgltf::Parser parser{ fastgltf::Extensions::KHR_texture_transform
				| fastgltf::Extensions::MSFT_texture_dds
				| fastgltf::Extensions::KHR_mesh_quantization
				| fastgltf::Extensions::EXT_mesh_gpu_instancing
				| fastgltf::Extensions::EXT_meshopt_compression
				| fastgltf::Extensions::KHR_texture_basisu
				| fastgltf::Extensions::EXT_texture_webp
				| fastgltf::Extensions::KHR_lights_punctual
				| fastgltf::Extensions::KHR_materials_specular
				| fastgltf::Extensions::KHR_materials_pbrSpecularGlossiness
				| fastgltf::Extensions::KHR_materials_ior
				| fastgltf::Extensions::KHR_materials_iridescence
				| fastgltf::Extensions::KHR_materials_volume
				| fastgltf::Extensions::KHR_materials_transmission
				| fastgltf::Extensions::KHR_materials_clearcoat
				| fastgltf::Extensions::KHR_materials_emissive_strength
				| fastgltf::Extensions::KHR_materials_sheen
				| fastgltf::Extensions::KHR_materials_unlit
				| fastgltf::Extensions::KHR_materials_anisotropy
				| fastgltf::Extensions::KHR_materials_dispersion
				| fastgltf::Extensions::KHR_materials_diffuse_transmission };
			auto path = castor::makePath( filePath );

			constexpr auto gltfOptions = fastgltf::Options::DontRequireValidAssetMember
				| fastgltf::Options::AllowDouble
				| fastgltf::Options::LoadExternalBuffers
				| fastgltf::Options::DecomposeNodeMatrices
				| fastgltf::Options::LoadExternalImages;

			auto dataResult = fastgltf::GltfDataBuffer::FromPath( path );

			if ( !dataResult )
			{
				castor3d::log::error << "Failed to to load glTF buffer" << std::endl;
				return fastgltf::Expected< fastgltf::Asset >( dataResult.error() );
			}

			auto & data = dataResult.get();

			if ( auto type = fastgltf::determineGltfFileType( data );
				type != fastgltf::GltfType::glTF && type != fastgltf::GltfType::GLB )
			{
				castor3d::log::error << "Failed to determine glTF container" << std::endl;
				return fastgltf::Expected< fastgltf::Asset >( fastgltf::Error::InvalidPath );
			}

			auto result = parser.loadGltf( data, path.parent_path(), gltfOptions );

			if ( result.error() != fastgltf::Error::None )
			{
				castor3d::log::error << "Failed to load glTF: " << castor::makeString( fastgltf::getErrorMessage( result.error() ) ) << std::endl;
			}

			return result;
		}

		static void parseNodesRec( fastgltf::pmr::MaybeSmallVector< size_t > const & nodes
			, size_t parentNodeIndex
			, castor::Vector< fastgltf::Node > const & allNodes
			, castor::Function< std::tuple< size_t, bool, bool >( fastgltf::Node const &, size_t, size_t, size_t, bool ) > const & func
			, castor::Set< size_t > & parsed
			, size_t instanceCount
			, bool skeletonNode )
		{
			for ( auto & nodeIndex : nodes )
			{
				if ( parsed.insert( nodeIndex ).second )
				{
					fastgltf::Node const & node = allNodes[nodeIndex];
					auto [curInstanceCount, carryOn, isSkel] = func( node, nodeIndex, parentNodeIndex, instanceCount, skeletonNode );

					if ( carryOn )
					{
						parseNodesRec( node.children, nodeIndex, allNodes, func, parsed, curInstanceCount, isSkel );
					}
				}
			}
		}

		static void parseNodes( fastgltf::pmr::MaybeSmallVector< size_t > const & nodes
			, castor::Vector< fastgltf::Node > const & allNodes
			, castor::Function< std::tuple< size_t, bool, bool >( fastgltf::Node const &, size_t, size_t, size_t, bool ) > const & func )
		{
			castor::Set< size_t > parsed;
			size_t parent = ~0u;
			parseNodesRec( nodes, parent, allNodes, func, parsed, 1u, false );
		}

		static castor::Vector< castor3d::NodeTransform > listInstances( fastgltf::Asset const & impAsset
			, CompressedBufferDataAdapter const & adapter
			, GltfNodeData const & nodeData )
		{
			auto const & impNode = *nodeData.node;
			castor::Point3fArray translations;
			castor::QuaternionArray rotations;
			castor::Point3fArray scalings;
			auto tit = impNode.findInstancingAttribute( "TRANSLATION" );
			auto rit = impNode.findInstancingAttribute( "ROTATION" );
			auto sit = impNode.findInstancingAttribute( "SCALE" );

			if ( tit != impNode.instancingAttributes.end() )
			{
				iterateAccessor< castor::Point3f >( impAsset
					, impAsset.accessors[tit->accessorIndex]
					, [&translations]( castor::Point3f value )
					{
						translations.push_back( castor::move( value ) );
					}
					, adapter );
			}

			if ( rit != impNode.instancingAttributes.end() )
			{
				iterateAccessor< castor::Point4f >( impAsset
					, impAsset.accessors[rit->accessorIndex]
					, [&rotations]( castor::Point4f const & value )
					{
						rotations.emplace_back( value );
					}
					, adapter );
			}

			if ( sit != impNode.instancingAttributes.end() )
			{
				iterateAccessor< castor::Point3f >( impAsset
					, impAsset.accessors[sit->accessorIndex]
					, [&scalings]( castor::Point3f value )
					{
						scalings.push_back( castor::move( value ) );
					}
					, adapter );
			}

			size_t instanceCount = std::max( translations.size(), std::max( rotations.size(), scalings.size() ) );

			if ( instanceCount )
			{
				if ( translations.empty() )
				{
					translations.resize( instanceCount, castor::Point3f{} );
				}

				if ( rotations.empty() )
				{
					rotations.resize( instanceCount, castor::Quaternion::identity() );
				}

				if ( scalings.empty() )
				{
					scalings.resize( instanceCount, castor::Point3f{ 1.0f, 1.0f, 1.0f } );
				}
			}

			castor::Vector< castor3d::NodeTransform > result;
			result.reserve( instanceCount );

			for ( size_t i = 0u; i < instanceCount; ++i )
			{
				result.emplace_back( castor3d::NodeTransform{ translations[i], scalings[i], rotations[i] } );
			}

			return result;
		}

		static bool hasChildNode( fastgltf::Asset const & impAsset
			, size_t rootIndex
			, size_t lookupIndex )
		{
			bool result{};
			parseNodes( impAsset.nodes[rootIndex].children
				, impAsset.nodes
				, [&result, &lookupIndex]( fastgltf::Node const & /*node*/, size_t nodeIndex, size_t /*parentIndex*/, size_t /*parentInstanceCount*/, bool )
				{
					if ( nodeIndex == lookupIndex )
					{
						result = true;
						return std::make_tuple( size_t( 1u ), false, false );
					}

					return std::make_tuple( size_t( 1u ), true, false );
				} );
			return result;
		}

		static bool isSkeletonNode( fastgltf::Asset const & impAsset
			, std::vector< fastgltf::Skin > const & skeletons
			, size_t nodeIndex
			, castor::Vector< size_t > const & skinsRootNodes )
		{
			return skinsRootNodes.end() != std::find( skinsRootNodes.begin(), skinsRootNodes.end(), nodeIndex )
				|| std::any_of( skeletons.begin()
					, skeletons.end()
					, [&impAsset, nodeIndex]( fastgltf::Skin const & lookup )
					{
						return lookup.joints.end() != std::find_if( lookup.joints.begin()
							, lookup.joints.end()
							, [&impAsset, nodeIndex]( size_t lookupIndex )
							{
								return lookupIndex == nodeIndex
									|| hasChildNode( impAsset, lookupIndex, nodeIndex );
							} );
					} );
		}

		static auto findNodeMesh( size_t meshIndex
			, castor::StringMap< GltfMeshData > const & meshes )
		{
			return std::find_if( meshes.begin()
				, meshes.end()
				, [&meshIndex]( castor::StringMap< GltfMeshData >::value_type const & lookup )
				{
					return lookup.second.submeshes.end() != std::find_if( lookup.second.submeshes.begin()
						, lookup.second.submeshes.end()
						, [&meshIndex]( GltfSubmeshData const & submesh )
						{
							return submesh.meshIndex == meshIndex;
						} );
				} );
		}

		static castor::String getElementName( auto const & elements
			, size_t index
			, castor::StringView baseName )
		{
			castor::String result = castor::makeString( elements[index].name );

			if ( result.empty() )
			{
				result += baseName;
				result += cuT( "-" ) + castor::string::toString( index );
			}

			return result;
		}

		static castor::String getElementName( auto const & elements
			, size_t index
			, castor::StringView baseName
			, NameContainer & names )
		{
			if ( auto it = names.namesByIndex.find( index );
				it != names.namesByIndex.end() )
			{
				return it->second;
			}

			auto result = castor::makeString( elements[index].name );

			if ( result.empty() )
			{
				result = baseName;
			}

			if ( auto it = names.names.find( result );
				it != names.names.end() )
			{
				result += cuT( "-" ) + castor::string::toString( index );
			}

			names.namesByIndex.emplace( index, result );
			names.names.emplace( result );
			return result;
		}

		static castor::String getLongestCommonSubstring( castor::String const & a, castor::String const & b )
		{
			auto result = castor::string::getLongestCommonSubstring( a, b );
			return castor::string::trim( result
				, true
				, true
				, castor::StringView{ cuT( " \r\t-_/\\|*$<>[](){}" ) } );
		}

		template< typename IterT, typename TypeT >
		static castor::Pair< IterT, castor::String > replaceIter( castor::String const & name
			, IterT iter
			, castor::StringMap< TypeT > & map )
		{
			auto common = getLongestCommonSubstring( name, iter->first );

			if ( !common.empty() && common != iter->first )
			{
				auto data = iter->second;
				map.erase( iter );
				iter = map.emplace( common, data ).first;
			}

			return { iter, common };
		}

		static bool isAnimationTarget( fastgltf::Asset const & /*asset*/
			, fastgltf::AnimationChannel const & channel
			, GltfNodeData const & nodeData )
		{
			return channel.nodeIndex == nodeData.index
				&& ( channel.path == fastgltf::AnimationPath::Rotation
					|| channel.path == fastgltf::AnimationPath::Scale
					|| channel.path == fastgltf::AnimationPath::Translation );
		}

		static bool isAnimationTarget( fastgltf::Asset const & asset
			, fastgltf::AnimationChannel const & channel
			, GltfSubmeshData const & submeshData )
		{
			return channel.nodeIndex
				&& ( *asset.nodes[*channel.nodeIndex].meshIndex == submeshData.meshIndex )
				&& channel.path == fastgltf::AnimationPath::Weights
				&& asset.nodes[*channel.nodeIndex].meshIndex;
		}

		template< typename DataT >
		static void listDataAnimations( GltfImporterFile const & file
			, DataT & data )
		{
			auto & asset = file.getAsset();
			size_t animIndex{};

			for ( auto & animation : asset.animations )
			{
				for ( auto & channel : animation.channels )
				{
					if ( isAnimationTarget( asset, channel, data ) )
					{
						auto & channelSamplers = data.anims.emplace( file.getAnimationName( animIndex ), AnimationChannelSamplers{} ).first->second;
						auto & nodeSamplers = channelSamplers.emplace( channel.path, NodeAnimationChannelSampler{} ).first->second;
						nodeSamplers.emplace_back( channel, animation.samplers[channel.samplerIndex] );
					}
				}

				++animIndex;
			}
		}

		static castor::StringMap< GltfMeshData >::iterator mergeMeshes( GltfImporterFile const & file
			, size_t meshIndex
			, castor::String const & meshName
			, castor::StringMap< GltfMeshData > & meshes
			, size_t & skinIndex
			, fastgltf::Skin const *& skin )
		{
			auto & asset = file.getAsset();
			// Merge meshes that use the same skeleton
			auto it = std::find_if( asset.nodes.begin()
				, asset.nodes.end()
				, [meshIndex]( fastgltf::Node const & lookup )
				{
					return lookup.meshIndex
						&& *lookup.meshIndex == meshIndex
						&& lookup.skinIndex;
				} );
			auto result = meshes.end();

			if ( it != asset.nodes.end() )
			{
				skinIndex = *it->skinIndex;
				skin = &asset.skins[skinIndex];
				result = std::find_if( meshes.begin()
					, meshes.end()
					, [skin]( castor::StringMap< GltfMeshData >::value_type const & lookup )
					{
						return skin == lookup.second.skin;
					} );

				if ( result != meshes.end() )
				{
					result = file::replaceIter( meshName, result, meshes ).first;
				}
			}

			return result;
		}

		static void listNodeMeshes( castor::Vector< castor::Matrix4x4f > const & cumulativeTransforms
			, castor::StringMap< GltfMeshData > const & meshes
			, size_t meshIndex
			, castor::Matrix4x4f const & matrix
			, castor::Map< GltfMeshData const *, castor::Vector< size_t > > & processedMeshes
			, GltfNodeData & nodeData )
		{
			auto it = file::findNodeMesh( meshIndex, meshes );

			if ( it != meshes.end() )
			{
				if ( nodeData.meshes.end() == std::find( nodeData.meshes.begin()
					, nodeData.meshes.end()
					, &it->second ) )
				{
					// Don't add the mesh if it has already been added to a node with the same transform.
					auto & nodeArray = processedMeshes.try_emplace( &it->second ).first->second;
					auto nodeIt = std::find_if( nodeArray.begin()
						, nodeArray.end()
						, [&cumulativeTransforms, &matrix]( size_t lookup )
						{
							return cumulativeTransforms[lookup] == matrix;
						} );

					if ( nodeIt == nodeArray.end() )
					{
						nodeArray.push_back( nodeData.index );
						nodeData.meshes.push_back( &it->second );
					}
				}
			}
			else
			{
				CU_Failure( "Could not find node's mesh ?" );
			}
		}

		static bool hasNonSkinnedData( GltfSceneData const & sceneData
			, GltfNodeData const & nodeData )
		{
			if ( nodeData.isCamera || nodeData.node->lightIndex || !nodeData.meshes.empty() )
			{
				return true;
			}

			return std::any_of( nodeData.node->children.begin()
				, nodeData.node->children.end()
				, [&sceneData]( size_t lookup )
				{
					return hasNonSkinnedData( sceneData, sceneData.nodes[lookup] );
				} );
		}
	}

	//*********************************************************************************************

	castor::Point3f convert( fastgltf::math::fvec3 const & value )
	{
		return castor::Point3f{ value[0], value[1], value[2] };
	}

	castor::Quaternion convert( fastgltf::math::fquat const & value )
	{
		return castor::Quaternion::fromComponents( value[0], value[1], value[2], value[3] );
	}

	castor3d::NodeTransform convert( std::variant< fastgltf::TRS, fastgltf::math::fmat4x4 > const & transform )
	{
		if ( transform.index() == 0u )
		{
			fastgltf::TRS const & trs = std::get< 0 >( transform );
			return { convert( trs.translation )
				, convert( trs.scale )
				, convert( trs.rotation ) };
		}

		fastgltf::math::fvec3 translation;
		fastgltf::math::fvec3 scale;
		fastgltf::math::fquat rotation;
		fastgltf::math::decomposeTransformMatrix( std::get< 1 >( transform ), scale, rotation, translation );
		return { convert( translation )
			, convert( scale )
			, convert( rotation ) };
	}

	//*********************************************************************************************

	[[nodiscard]]
	fastgltf::span< std::byte const > CompressedBufferDataAdapter::getData( fastgltf::Buffer const & buffer
		, std::size_t byteOffset,
		std::size_t byteLength )
	{
		using namespace fastgltf;
		return std::visit( visitor{ []( auto & ) -> span< std::byte const >
					{
						assert( false && "Tried accessing a buffer with no data, likely because no buffers were loaded. Perhaps you forgot to specify the LoadExternalBuffers option?" );
						return {};
					}
				, []( const sources::Fallback & ) -> span< std::byte const >
					{
						assert( false && "Tried accessing data of a fallback buffer." );
						return {};
					}
				, [byteOffset, byteLength]( const sources::Array & array )
					{
						return span( array.bytes.data(), array.bytes.size_bytes() ).subspan( byteOffset, byteLength );
					}
				, [byteOffset, byteLength]( const sources::Vector & vec )
					{
						return span( vec.bytes.data(), vec.bytes.size() ).subspan( byteOffset, byteLength );
					}
				, [byteOffset, byteLength]( const sources::ByteView & bv )
					{
						return bv.bytes.subspan( byteOffset, byteLength );
					} }
			, buffer.data );
	}

	/** Decompress all buffer views and store them in this adapter */
	bool CompressedBufferDataAdapter::decompress( const fastgltf::Asset & asset )
	{
		using namespace fastgltf;

		decompressedBuffers.reserve( asset.bufferViews.size() );
		for ( auto & bufferView : asset.bufferViews )
		{
			if ( !bufferView.meshoptCompression )
			{
				decompressedBuffers.emplace_back( std::nullopt );
				continue;
			}

			// This is a compressed buffer view.
			// For the original implementation, see https://github.com/jkuhlmann/cgltf/pull/129#issue-739550034
			auto const & mc = *bufferView.meshoptCompression;
			fastgltf::StaticVector< std::byte > result( mc.count * mc.byteStride );

			// Get the data span from the compressed buffer.
			auto data = getData( asset.buffers[mc.bufferIndex], mc.byteOffset, mc.byteLength );

			int rc = -1;
			switch ( mc.mode )
			{
			case MeshoptCompressionMode::Attributes:
				{
					rc = meshopt_decodeVertexBuffer( result.data()
						, mc.count
						, mc.byteStride
						, reinterpret_cast< const unsigned char * >( data.data() )
						, mc.byteLength );
					break;
				}
			case MeshoptCompressionMode::Triangles:
				{
					rc = meshopt_decodeIndexBuffer( result.data()
						, mc.count
						, mc.byteStride
						, reinterpret_cast< const unsigned char * >( data.data() )
						, mc.byteLength );
					break;
				}
			case MeshoptCompressionMode::Indices:
				{
					rc = meshopt_decodeIndexSequence( result.data()
						, mc.count
						, mc.byteStride
						, reinterpret_cast< const unsigned char * >( data.data() )
						, mc.byteLength );
					break;
				}
			}

			if ( rc != 0 )
				return false;

			switch ( mc.filter )
			{
			case MeshoptCompressionFilter::None:
				break;
			case MeshoptCompressionFilter::Octahedral:
				{
					meshopt_decodeFilterOct( result.data(), mc.count, mc.byteStride );
					break;
				}
			case MeshoptCompressionFilter::Quaternion:
				{
					meshopt_decodeFilterQuat( result.data(), mc.count, mc.byteStride );
					break;
				}
			case MeshoptCompressionFilter::Exponential:
				{
					meshopt_decodeFilterExp( result.data(), mc.count, mc.byteStride );
					break;
				}
			}

			decompressedBuffers.emplace_back( std::move( result ) );
		}

		return true;
	}

	fastgltf::span< std::byte const > CompressedBufferDataAdapter::operator()( const fastgltf::Asset & asset, std::size_t bufferViewIdx ) const
	{
		using namespace fastgltf;

		auto & bufferView = asset.bufferViews[bufferViewIdx];
		if ( bufferView.meshoptCompression )
		{
			assert( decompressedBuffers.size() == asset.bufferViews.size() );

			assert( decompressedBuffers[bufferViewIdx].has_value() );
			return span( decompressedBuffers[bufferViewIdx]->data(), decompressedBuffers[bufferViewIdx]->size_bytes() );
		}

		return getData( asset.buffers[bufferView.bufferIndex], bufferView.byteOffset, bufferView.byteLength );
	}
	//*********************************************************************************************

	castor::MbString const GltfImporterFile::Name = "GLTF Importer";

	GltfImporterFile::GltfImporterFile( castor3d::Engine & engine
		, castor3d::Scene * scene
		, castor::Path const & path
		, castor3d::Parameters const & parameters
		, castor3d::ProgressBar * progress )
		: castor3d::ImporterFile{ engine, scene, path, parameters, progress }
		, m_expAsset{ file::loadScene( getFileName() ) }
	{
		if ( isValid() )
		{
			m_asset = &m_expAsset.get< 1 >();
			m_adapter.decompress( *m_asset );

			if ( auto sceneIndex = getParameters().get< uint32_t >( cuT( "sceneIndex" ) ) )
			{
				m_sceneIndices.push_back( sceneIndex );
			}
			else if ( m_asset->defaultScene )
			{
				m_sceneIndices.push_back( *m_asset->defaultScene );
			}
			else
			{
				m_sceneIndices.resize( m_asset->scenes.size() );
				std::iota( m_sceneIndices.begin(), m_sceneIndices.end(), 0u );
			}

			engine.getMaterialCache().forEach( [this]( castor3d::Material const & element )
				{
					m_materialNames.namesByIndex.try_emplace( 0xFFFFFFFF00000000ULL + m_materialNames.namesByIndex.size(), element.getName() );
					m_materialNames.names.emplace( element.getName() );
				} );
			engine.getSamplerCache().forEach( [this]( castor3d::Sampler const & element )
				{
					m_samplerNames.namesByIndex.try_emplace( 0xFFFFFFFF00000000ULL + m_samplerNames.namesByIndex.size(), element.getName() );
					m_samplerNames.names.emplace( element.getName() );
				} );

			if ( scene )
			{
				scene->getMeshCache().forEach( [this]( castor3d::Mesh const & element )
					{
						m_meshNames.namesByIndex.try_emplace( 0xFFFFFFFF00000000ULL + m_meshNames.namesByIndex.size(), element.getName() );
						m_meshNames.names.emplace( element.getName() );

						if ( auto skeleton = element.getSkeleton() )
						{
							m_skinNames.namesByIndex.try_emplace( 0xFFFFFFFF00000000ULL + m_skinNames.namesByIndex.size(), skeleton->getName() );
							m_skinNames.names.emplace( skeleton->getName() );
						}
					} );
				scene->getSceneNodeCache().forEach( [this]( castor3d::SceneNode const & element )
					{
						m_nodeNames.namesByIndex.try_emplace( 0xFFFFFFFF00000000ULL + m_nodeNames.namesByIndex.size(), element.getName() );
						m_nodeNames.names.emplace( element.getName() );
					} );
				scene->getLightCache().forEach( [this]( castor3d::Light const & element )
					{
						m_lightNames.namesByIndex.try_emplace( 0xFFFFFFFF00000000ULL + m_lightNames.namesByIndex.size(), element.getName() );
						m_lightNames.names.emplace( element.getName() );
					} );
				scene->getCameraCache().forEach( [this]( castor3d::Camera const & element )
					{
						m_cameraNames.namesByIndex.try_emplace( 0xFFFFFFFF00000000ULL + m_cameraNames.namesByIndex.size(), element.getName() );
						m_cameraNames.names.emplace( element.getName() );
					} );
			}

			doPrelistMeshes();
			doPrelistNodes();
		}
	}

	castor::String GltfImporterFile::getMaterialName( size_t index )const
	{
		return getInternalName( file::getElementName( m_asset->materials, index, getName(), m_materialNames ) );
	}

	castor::String GltfImporterFile::getMeshName( size_t index )const
	{
		return getInternalName( file::getElementName( m_asset->meshes, index, getName(), m_meshNames ) );
	}

	castor::String GltfImporterFile::getNodeName( size_t index, size_t instance )const
	{
		auto result = file::getElementName( m_asset->nodes, index, getName(), m_nodeNames );

		if ( instance )
		{
			result += cuT( "_" ) + castor::string::toString( instance );
		}

		return getInternalName( result );
	}

	castor::String GltfImporterFile::getSkinName( size_t index )const
	{
		return getInternalName( file::getElementName( m_asset->skins, index, getName(), m_skinNames ) );
	}

	castor::String GltfImporterFile::getLightName( size_t index )const
	{
		return getInternalName( file::getElementName( m_asset->lights, index, getName(), m_lightNames ) );
	}

	castor::String GltfImporterFile::getCameraName( size_t index )const
	{
		return getInternalName( file::getElementName( m_asset->cameras, index, getName(), m_cameraNames ) );
	}

	castor::String GltfImporterFile::getSamplerName( fastgltf::Sampler const & impSampler )const
	{
		auto const & engine = *getOwner();
		auto const & defaultSampler = *engine.getDefaultSampler();
		return castor3d::getSamplerName( VK_COMPARE_OP_NEVER
			, impSampler.minFilter ? convert( *impSampler.minFilter ) : defaultSampler.getMinFilter()
			, impSampler.magFilter ? convert( *impSampler.magFilter ) : defaultSampler.getMagFilter()
			, impSampler.minFilter ? getMipFilter( *impSampler.minFilter ) : defaultSampler.getMipFilter()
			, convert( impSampler.wrapS )
			, convert( impSampler.wrapT )
			, defaultSampler.getWrapR() );
	}

	castor::String GltfImporterFile::getGeometryName( size_t nodeIndex, size_t meshIndex, size_t instance )const
	{
		auto nodeName = file::getElementName( m_asset->nodes, nodeIndex, getName(), m_nodeNames );

		if ( instance )
		{
			nodeName += cuT( "_" ) + castor::string::toString( instance );
		}

		auto meshName = file::getElementName( m_asset->meshes, meshIndex, getName(), m_meshNames );
		castor::String result;

		if ( nodeName == meshName )
		{
			result += nodeName;
		}
		else
		{
			result += nodeName;
			result += cuT( "-" ) + meshName;
		}

		return getInternalName( result );
	}

	castor::String GltfImporterFile::getAnimationName( size_t index )const
	{
		return getInternalName( file::getElementName( m_asset->animations, index, getName() ) );
	}

	size_t GltfImporterFile::getNodeIndex( castor::String const & name )const
	{
		auto it = std::find_if( m_sceneData.nodes.begin()
			, m_sceneData.nodes.end()
			, [&name]( GltfNodeData const & lookup )
			{
				return lookup.instances.end() != std::find_if( lookup.instances.begin()
					, lookup.instances.end()
					, [&name]( auto const & nodeAndTransform )
					{
						return nodeAndTransform.first.name == name;
					} );
			} );
		CU_Require( it != m_sceneData.nodes.end() );
		return it->index;
	}

	size_t GltfImporterFile::getSkeletonNodeIndex( castor::String const & name )const
	{
		auto it = std::find_if( m_sceneData.skeletonNodes.begin()
			, m_sceneData.skeletonNodes.end()
			, [&name]( GltfNodeData const * lookup )
			{
				return lookup->instances.end() != std::find_if( lookup->instances.begin()
					, lookup->instances.end()
					, [&name]( auto const & nodeAndTransform )
					{
						return nodeAndTransform.first.name == name;
					} );
			} );
		CU_Require( it != m_sceneData.skeletonNodes.end() );
		return ( *it )->index;
	}

	size_t GltfImporterFile::getMeshIndex( castor::String const & name, uint32_t submeshIndex )const
	{
		auto mit = m_sceneData.meshes.find( name );
		CU_Require( mit != m_sceneData.meshes.end() );
		CU_Require( submeshIndex < mit->second.submeshes.size() );
		return mit->second.submeshes[submeshIndex].meshIndex;
	}

	Animations GltfImporterFile::getMeshAnimations( castor3d::Mesh const & mesh, uint32_t submeshIndex )const
	{
		if ( auto it = m_sceneData.meshes.find( mesh.getName() );
			it != m_sceneData.meshes.end()
				&& submeshIndex < it->second.submeshes.size() )
		{
			return it->second.submeshes[submeshIndex].anims;
		}

		static Animations const dummy;
		return dummy;
	}

	Animations GltfImporterFile::getSkinAnimations( castor3d::Skeleton const & skeleton )const
	{
		Animations result;
		size_t index{};

		for ( auto & animation : m_asset->animations )
		{
			for ( auto & channel : animation.channels )
			{
				if ( ( channel.path == fastgltf::AnimationPath::Rotation
						|| channel.path == fastgltf::AnimationPath::Scale
						|| channel.path == fastgltf::AnimationPath::Translation )
					&& channel.nodeIndex
					&& isSkeletonNode( *channel.nodeIndex )
					&& skeleton.findNode( getNodeName( *channel.nodeIndex, 0u ) ) != nullptr )
				{
					auto & channelSamplers = result.try_emplace( getAnimationName( index ) ).first->second;
					auto & nodeSamplers = channelSamplers.try_emplace( channel.path ).first->second;
					nodeSamplers.emplace_back( channel, animation.samplers[channel.samplerIndex] );
				}
			}

			++index;
		}

		return result;
	}

	Animations GltfImporterFile::getNodeAnimations( castor3d::SceneNode const & node )const
	{
		if ( auto it = std::find_if( m_sceneData.nodes.begin()
			, m_sceneData.nodes.end()
			, [&node]( GltfNodeData const & lookup )
			{
				return lookup.instances.end() != std::find_if( lookup.instances.begin()
					, lookup.instances.end()
					, [&node]( auto const & nodeAndTransform )
					{
						return nodeAndTransform.first.name == node.getName();
					} );
			} );
			it != m_sceneData.nodes.end() )
		{
			return it->anims;
		}

		static Animations const dummy;
		return dummy;
	}

	bool GltfImporterFile::isSkeletonNode( size_t nodeIndex )const
	{
		return std::any_of( m_sceneData.skeletonNodes.begin()
			, m_sceneData.skeletonNodes.end()
			, [nodeIndex]( GltfNodeData const * lookup )
			{
				return lookup->index == nodeIndex;
			} );
	}

	castor::StringArray GltfImporterFile::listMaterials()
	{
		castor::StringArray result;

		if ( isValid() )
		{
			for ( size_t i = 0u; i < m_asset->materials.size(); ++i )
			{
				result.emplace_back( getMaterialName( i ) );
			}
		}

		return result;
	}

	castor::Vector< castor3d::ImporterFile::MeshData > GltfImporterFile::listMeshes()
	{
		castor::Vector< MeshData > result;

		for ( auto const & [name, data] : m_sceneData.meshes )
		{
			result.emplace_back( name
				, ( data.skin
					? getSkinName( data.skinIndex )
					: castor::String{} ) );
		}

		return result;
	}

	castor::StringArray GltfImporterFile::listSkeletons()
	{
		castor::StringArray result;

		if ( isValid() )
		{
			for ( size_t i = 0u; i < m_asset->skins.size(); ++i )
			{
				result.emplace_back( getSkinName( i ) );
			}
		}

		return result;
	}

	castor::Vector< castor3d::ImporterFile::NodeData > GltfImporterFile::listSceneNodes()
	{
		castor::Vector< NodeData > result;

		if ( isValid() )
		{
			for ( auto const * nodeData : m_sceneData.sortedNodes )
			{
				if ( file::hasNonSkinnedData( m_sceneData, *nodeData ) )
				{
					for ( auto const & [instance, _] : nodeData->instances )
					{
						result.emplace_back( instance );
					}
				}
			}
		}

		return result;
	}

	castor::Vector< castor3d::ImporterFile::LightData > GltfImporterFile::listLights()
	{
		castor::Vector< LightData > result;

		for ( auto & light : m_sceneData.lights )
		{
			result.emplace_back( light.name, light.type );
		}

		return result;
	}

	castor::Vector< castor3d::ImporterFile::LightGroupData > GltfImporterFile::listLightGroups()
	{
		castor::Vector< LightGroupData > result;

		for ( auto & [_, light] : m_sceneData.lightGroups )
		{
			result.emplace_back( light.name, light.type );
		}

		return result;
	}

	castor::Vector< castor3d::ImporterFile::GeometryData > GltfImporterFile::listGeometries()
	{
		castor::Vector< GeometryData > result;

		if ( isValid() )
		{
			for ( auto & nodeData : m_sceneData.nodes )
			{
				for ( auto & meshData : nodeData.meshes )
				{
					auto it = std::find_if( m_sceneData.meshes.begin()
						, m_sceneData.meshes.end()
						, [meshData]( castor::StringMap< GltfMeshData >::value_type const & lookup )
						{
							return meshData == &lookup.second;
						} );
					CU_Require( it != m_sceneData.meshes.end() );
					auto meshName = it->first;

					for ( auto & [nodeInstance, transform] : nodeData.instances )
					{
						auto name = nodeInstance.name == meshName
							? nodeInstance.name
							: nodeInstance.name + cuT( "_" ) + meshName;
						result.emplace_back( name, nodeInstance.name, it->first );
					}
				}
			}
		}

		return result;
	}

	castor::Vector< castor3d::ImporterFile::CameraData > GltfImporterFile::listCameras()
	{
		castor::Vector< CameraData > result;

		if ( isValid() )
		{
			size_t idx{};

			for ( auto & camera : m_asset->cameras )
			{
				result.emplace_back( getCameraName( idx )
					, ( camera.camera.index() == 1u
						? castor3d::ViewportType::eOrtho
						: ( std::get< 0u >( camera.camera ).zfar
							? castor3d::ViewportType::ePerspective
							: castor3d::ViewportType::eInfinitePerspective ) ) );
				++idx;
			}
		}

		return result;
	}

	castor::StringArray GltfImporterFile::listMeshAnimations( castor3d::Mesh const & mesh )
	{
		castor::Set< castor::String > result;

		if ( auto it = m_sceneData.meshes.find( mesh.getName() );
			it != m_sceneData.meshes.end() )
		{
			for ( auto const & submesh : it->second.submeshes )
			{
				for ( auto const & [name, _] : submesh.anims )
				{
					result.insert( name );
				}
			}
		}

		return castor::StringArray{ result.begin()
			, result.end() };
	}

	castor::StringArray GltfImporterFile::listSkeletonAnimations( castor3d::Skeleton const & skeleton )
	{
		castor::Set< castor::String > result;

		if ( isValid() )
		{
			size_t index{};

			for ( auto & animation : m_asset->animations )
			{
				for ( auto & channel : animation.channels )
				{
					if ( ( channel.path == fastgltf::AnimationPath::Rotation
							|| channel.path == fastgltf::AnimationPath::Scale
							|| channel.path == fastgltf::AnimationPath::Translation )
						&& channel.nodeIndex
						&& isSkeletonNode( *channel.nodeIndex ) )
					{
						result.insert( getAnimationName( index ) );
					}
				}

				++index;
			}
		}

		return castor::StringArray{ result.begin()
			, result.end() };
	}

	castor::StringArray GltfImporterFile::listSceneNodeAnimations( castor3d::SceneNode const & node )
	{
		castor::StringArray result;

		if ( auto it = std::find_if( m_sceneData.nodes.begin()
			, m_sceneData.nodes.end()
			, [&node]( GltfNodeData const & lookup )
			{
				return lookup.instances.end() != std::find_if( lookup.instances.begin()
					, lookup.instances.end()
					, [&node]( auto const & nodeAndTransform )
					{
						return nodeAndTransform.first.name == node.getName();
					} );
			} );
			it != m_sceneData.nodes.end() )
		{
			for ( auto const & [name, _] : it->anims )
			{
				result.push_back( name );
			}
		}

		return result;
	}

	castor::Vector< uint32_t > GltfImporterFile::listTextureAnimations( castor3d::Material const & material
		, uint32_t pass )
	{
		castor::Vector< uint32_t > result;
		return result;
	}

	uint32_t GltfImporterFile::countAllMeshAnimations()const
	{
		uint32_t result{};

		for ( auto & [_, mesh] : m_sceneData.meshes )
		{
			for ( auto & submesh : mesh.submeshes )
			{
				result += uint32_t( submesh.anims.size() );
			}
		}

		return result;
	}

	uint32_t GltfImporterFile::countAllSkeletonAnimations()const
	{
		castor::Set< castor::String > result;

		if ( isValid() )
		{
			size_t index{};

			for ( auto & animation : m_asset->animations )
			{
				for ( auto & channel : animation.channels )
				{
					if ( ( channel.path == fastgltf::AnimationPath::Rotation
						|| channel.path == fastgltf::AnimationPath::Scale
						|| channel.path == fastgltf::AnimationPath::Translation )
						&& channel.nodeIndex
						&& isSkeletonNode( *channel.nodeIndex ) )
					{
						result.insert( getAnimationName( index ) );
					}
				}

				++index;
			}
		}

		return uint32_t( result.size() );
	}

	uint32_t GltfImporterFile::countAllSceneNodeAnimations()const
	{
		uint32_t result{};

		for ( auto & node : m_sceneData.nodes )
		{
			result += uint32_t( node.anims.size() );
		}

		return result;
	}

	uint32_t GltfImporterFile::countAllTextureAnimations()const
	{
		return 0u;
	}

	castor3d::MaterialImporterUPtr GltfImporterFile::createMaterialImporter()
	{
		return castor::makeUniqueDerived< castor3d::MaterialImporter, GltfMaterialImporter >( *getOwner() );
	}

	castor3d::AnimationImporterUPtr GltfImporterFile::createAnimationImporter()
	{
		return castor::makeUniqueDerived< castor3d::AnimationImporter, GltfAnimationImporter >( *getOwner() );
	}

	castor3d::SkeletonImporterUPtr GltfImporterFile::createSkeletonImporter()
	{
		return castor::makeUniqueDerived< castor3d::SkeletonImporter, GltfSkeletonImporter >( *getOwner() );
	}

	castor3d::MeshImporterUPtr GltfImporterFile::createMeshImporter()
	{
		return castor::makeUniqueDerived< castor3d::MeshImporter, GltfMeshImporter >( *getOwner() );
	}

	castor3d::SceneNodeImporterUPtr GltfImporterFile::createSceneNodeImporter()
	{
		return castor::makeUniqueDerived< castor3d::SceneNodeImporter, GltfSceneNodeImporter >( *getOwner() );
	}

	castor3d::LightImporterUPtr GltfImporterFile::createLightImporter()
	{
		return castor::makeUniqueDerived< castor3d::LightImporter, GltfLightImporter >( *getOwner() );
	}

	castor3d::CameraImporterUPtr GltfImporterFile::createCameraImporter()
	{
		return castor::makeUniqueDerived< castor3d::CameraImporter, GltfCameraImporter >( *getOwner() );
	}

	castor3d::ImporterFileUPtr GltfImporterFile::create( castor3d::Engine & engine
		, castor3d::Scene * scene
		, castor::Path const & path
		, castor3d::Parameters const & parameters
		, castor3d::ProgressBar * progress )
	{
		return castor::makeUniqueDerived< castor3d::ImporterFile, GltfImporterFile >( engine, scene, path, parameters, progress );
	}

	void GltfImporterFile::doPrelistNodes()
	{
		castor::Vector< castor::Matrix4x4f > cumulativeTransforms;
		castor::Vector< size_t > skinsRootNodes;

		for ( auto & skin : m_asset->skins )
		{
			auto skinRootNodes = findSkinRootNodes( *this, skin );
			skinsRootNodes.insert( skinsRootNodes.end(), skinRootNodes.begin(), skinRootNodes.end() );
		}

		// First, list all nodes, with their own transforms and instances
		m_sceneData.nodes.reserve( m_asset->nodes.size() );
		cumulativeTransforms.resize( m_asset->nodes.size() );
		size_t nodeIndex{};
		for ( auto & node : m_asset->nodes )
		{
			auto transform = convert( node.transform );

			if ( node.cameraIndex )
			{
				transform.rotate *= castor::Quaternion::fromAxisAngle( castor::Point3f{ 0.0f, 1.0f, 0.0f }, castor::Angle::fromDegrees( 180.0f ) );
			}

			auto & nodeData = m_sceneData.nodes.emplace_back( node.cameraIndex.has_value()
				, file::isSkeletonNode( *m_asset, m_asset->skins, nodeIndex, skinsRootNodes )
				, nodeIndex
				, &node );
			castor::matrix::setTransform( cumulativeTransforms[nodeIndex]
				, transform.translate, transform.scale, transform.rotate );
			nodeData.instances.emplace_back( NodeData{ castor::String{}, getNodeName( nodeIndex, 0u ), nodeData.isCamera }, transform );
			auto instances = file::listInstances( *m_asset, m_adapter, nodeData );

			// List this node's instances.
			size_t instanceIndex{ 1u };
			for ( auto & instanceTransform : instances )
			{
				nodeData.instances.emplace_back( NodeData{ castor::String{}
						, getNodeName( nodeIndex, instanceIndex )
						, nodeData.isCamera }
					, castor::move( instanceTransform ) );
				++instanceIndex;
			}

			++nodeIndex;
		}

		// Then build the hierarchy, updating instances when needed
		for ( auto const & sceneIndex : m_sceneIndices )
		{
			castor::Vector< size_t > work;
			for ( auto index : m_asset->scenes[sceneIndex].nodeIndices )
			{
				work.emplace_back( index );
				m_sceneData.sortedNodes.emplace_back( &m_sceneData.nodes[index] );
			}

			while ( !work.empty() )
			{
				auto parentNodeIndex = work.back();
				work.pop_back();
				auto const & parentNodeData = m_sceneData.nodes[parentNodeIndex];

				for ( auto childNodeIndex : m_asset->nodes[parentNodeIndex].children )
				{
					work.emplace_back( childNodeIndex );
					m_sceneData.sortedNodes.emplace_back( &m_sceneData.nodes[childNodeIndex] );

					auto & childNodeData = m_sceneData.nodes[childNodeIndex];
					childNodeData.isSkeleton = childNodeData.isSkeleton || parentNodeData.isSkeleton;
					cumulativeTransforms[childNodeIndex] = cumulativeTransforms[parentNodeIndex] * cumulativeTransforms[childNodeIndex];

					// Build child instances
					size_t parentInstanceIndex{};
					auto childInstances = childNodeData.instances;
					childNodeData.instances.clear();
					for ( size_t i = 0u; i < parentNodeData.instances.size(); ++i )
					{
						size_t childInstanceIndex{};
						for ( auto const & [childInstanceData, childInstanceTransform] : childInstances )
						{
							childNodeData.instances.emplace_back( NodeData{ getNodeName( parentNodeIndex, parentInstanceIndex )
									, getNodeName( childNodeIndex, childInstanceIndex )
									, childInstanceData.isCamera }
								, childInstanceTransform );
							++childInstanceIndex;
						}
						++parentInstanceIndex;
					}
				}
			}
		}

		// List their attached objects
		castor::Map< GltfMeshData const *, castor::Vector< size_t > > processedMeshes;
		for ( auto nodeData : m_sceneData.sortedNodes )
		{
			auto & node = *nodeData->node;

			//
			if ( node.meshIndex )
			{
				file::listNodeMeshes( cumulativeTransforms, m_sceneData.meshes, *node.meshIndex, cumulativeTransforms[nodeIndex]
					, processedMeshes, *nodeData );
			}

			// Check for light
			if ( node.lightIndex )
			{
				if ( auto lightIndex = *node.lightIndex;
					lightIndex < m_asset->lights.size() )
				{
					auto light = m_asset->lights[lightIndex];
					auto lightName = getLightName( lightIndex );
					auto & lightGroup = m_sceneData.lightGroups.try_emplace( lightName
						, lightName
						, ( light.type == fastgltf::LightType::Directional
							? castor3d::LightType::eDirectional
							: ( light.type == fastgltf::LightType::Point
								? castor3d::LightType::ePoint
								: castor3d::LightType::eSpot ) )
						, uint32_t( lightIndex ) ).first->second;

					for ( auto const & [nodeInstanceData, _] : nodeData->instances )
					{
						lightGroup.nodeNames.push_back( nodeInstanceData.name );
					}
				}
			}

			if ( nodeData->isSkeleton )
			{
				m_sceneData.skeletonNodes.emplace_back( nodeData );
			}
			else
			{
				file::listDataAnimations( *this, *nodeData );
			}

			++nodeIndex;
		}

		// Fill helper containers.
		for ( auto nodeData : m_sceneData.sortedNodes )
		{
			for ( auto & [nodeInstance, transform]: nodeData->instances )
			{
				m_nodes.try_emplace( nodeInstance.name, &transform );
			}
		}

		// Replace light groups with a single node by lights
		auto it = m_sceneData.lightGroups.begin();
		while ( it != m_sceneData.lightGroups.end() )
		{
			auto & lightGroup = it->second;

			if ( lightGroup.nodeNames.size() == 1U )
			{
				m_sceneData.lights.emplace_back( lightGroup.name
					, lightGroup.type
					, lightGroup.lightIndex
					, lightGroup.nodeNames.front() );
				it = m_sceneData.lightGroups.erase( it );
			}
			else
			{
				++it;
			}
		}
	}

	void GltfImporterFile::doPrelistMeshes()
	{
		uint32_t meshIndex = 0u;

		for ( auto & impMesh : m_asset->meshes )
		{
			auto meshName = getMeshName( meshIndex );

			auto regIt = m_sceneData.meshes.find( meshName );
			fastgltf::Skin const * skin{};
			size_t skinIndex{};

			if ( regIt != m_sceneData.meshes.end() )
			{
				meshName += castor::string::toString( meshIndex );
				regIt = m_sceneData.meshes.find( meshName );
			}

			if ( regIt == m_sceneData.meshes.end() )
			{
				regIt = file::mergeMeshes( *this, meshIndex, meshName
					, m_sceneData.meshes, skinIndex, skin );
			}

			if ( regIt == m_sceneData.meshes.end() )
			{
				regIt = m_sceneData.meshes.try_emplace( meshName, skin, skinIndex ).first;
			}

			file::listDataAnimations( *this
				, regIt->second.submeshes.emplace_back( &impMesh, meshIndex ) );

			++meshIndex;
		}
	}

	//*********************************************************************************************
}
