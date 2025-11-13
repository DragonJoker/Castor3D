#include "GltfImporter/GltfImporterFile.hpp"

#include "GltfImporter/GltfAnimationImporter.hpp"
#include "GltfImporter/GltfCameraImporter.hpp"
#include "GltfImporter/GltfLightImporter.hpp"
#include "GltfImporter/GltfMeshImporter.hpp"
#include "GltfImporter/GltfSceneNodeImporter.hpp"
#include "GltfImporter/GltfSkeletonImporter.hpp"

#include <GltfMaterialImporter/GltfMaterialsFile.hpp>

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

#pragma optimize("", off)

namespace c3d_gltf
{
	//*********************************************************************************************

	namespace file
	{
		static void parseNodesRec( fastgltf::pmr::MaybeSmallVector< size_t > const & nodes
			, size_t parentNodeIndex
			, c3d::Vector< fastgltf::Node > const & allNodes
			, c3d::Function< std::tuple< size_t, bool, bool >( fastgltf::Node const &, size_t, size_t, size_t, bool ) > const & func
			, c3d::Set< size_t > & parsed
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
						parseNodesRec( node.children, nodeIndex, allNodes, func, parsed, curInstanceCount, isSkel );
				}
			}
		}

		static void parseNodes( fastgltf::pmr::MaybeSmallVector< size_t > const & nodes
			, c3d::Vector< fastgltf::Node > const & allNodes
			, c3d::Function< std::tuple< size_t, bool, bool >( fastgltf::Node const &, size_t, size_t, size_t, bool ) > const & func )
		{
			c3d::Set< size_t > parsed;
			size_t parent = ~0u;
			parseNodesRec( nodes, parent, allNodes, func, parsed, 1u, false );
		}

		static c3d::Vector< c3d::NodeTransform > listInstances( fastgltf::Asset const & impAsset
			, CompressedBufferDataAdapter const & adapter
			, GltfNodeData const & nodeData )
		{
			auto const & impNode = *nodeData.node;
			c3d::Point3fArray translations;
			c3d::QuaternionArray rotations;
			c3d::Point3fArray scalings;
			auto tit = impNode.findInstancingAttribute( "TRANSLATION" );
			auto rit = impNode.findInstancingAttribute( "ROTATION" );
			auto sit = impNode.findInstancingAttribute( "SCALE" );

			if ( tit != impNode.instancingAttributes.end() )
				iterateAccessor< c3d::Point3f >( impAsset
					, impAsset.accessors[tit->accessorIndex]
					, [&translations]( c3d::Point3f value )
					{
						translations.emplace_back( c3d::move( value ) );
					}
					, adapter );

			if ( rit != impNode.instancingAttributes.end() )
				iterateAccessor< c3d::Point4f >( impAsset
					, impAsset.accessors[rit->accessorIndex]
					, [&rotations]( c3d::Point4f const & value )
					{
						rotations.emplace_back( value );
					}
					, adapter );

			if ( sit != impNode.instancingAttributes.end() )
				iterateAccessor< c3d::Point3f >( impAsset
					, impAsset.accessors[sit->accessorIndex]
					, [&scalings]( c3d::Point3f value )
					{
						scalings.emplace_back( c3d::move( value ) );
					}
					, adapter );

			size_t instanceCount = std::max( translations.size(), std::max( rotations.size(), scalings.size() ) );

			if ( instanceCount )
			{
				if ( translations.empty() )
					translations.resize( instanceCount, c3d::Point3f{} );
				if ( rotations.empty() )
					rotations.resize( instanceCount, c3d::Quaternion::identity() );
				if ( scalings.empty() )
					scalings.resize( instanceCount, c3d::Point3f{ 1.0f, 1.0f, 1.0f } );
			}

			c3d::Vector< c3d::NodeTransform > result;
			result.reserve( instanceCount );
			for ( size_t i = 0u; i < instanceCount; ++i )
				result.emplace_back( translations[i], scalings[i], rotations[i] );
			return result;
		}

		static bool hasChildNode( fastgltf::Asset const & impAsset
			, size_t rootIndex
			, size_t lookupIndex )
		{
			bool result{};
			parseNodes( impAsset.nodes[rootIndex].children, impAsset.nodes
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
			, c3d::Vector< size_t > const & skinsRootNodes )
		{
			return skinsRootNodes.end() != std::find( skinsRootNodes.begin(), skinsRootNodes.end(), nodeIndex )
				|| std::any_of( skeletons.begin(), skeletons.end()
					, [&impAsset, nodeIndex]( fastgltf::Skin const & lookup )
					{
						return lookup.joints.end() != std::find_if( lookup.joints.begin(), lookup.joints.end()
							, [&impAsset, nodeIndex]( size_t lookupIndex )
							{
								return lookupIndex == nodeIndex
									|| hasChildNode( impAsset, lookupIndex, nodeIndex );
							} );
					} );
		}

		static c3d::String getLongestCommonSubstring( c3d::String const & a, c3d::String const & b )
		{
			auto result = c3d::string::getLongestCommonSubstring( a, b );
			return c3d::string::trim( result
				, true
				, true
				, c3d::StringView{ cuT( " \r\t-_/\\|*$<>[](){}" ) } );
		}

		template< typename IterT, typename TypeT >
		static c3d::Pair< IterT, c3d::String > replaceIter( c3d::String const & name
			, IterT iter
			, c3d::StringMap< TypeT > & map )
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

		static void listDataAnimations( GltfImporterFile const & file
			, GltfNodeData & nodeData )
		{
			auto & asset = file.getAsset();
			size_t animIndex{};

			for ( auto & animation : asset.animations )
			{
				for ( auto & channel : animation.channels )
				{
					if ( isAnimationTarget( asset, channel, nodeData ) )
					{
						auto & channelSamplers = nodeData.anims.try_emplace( file.getAnimationName( animIndex ) ).first->second;
						auto & nodeSamplers = channelSamplers.try_emplace( channel.path ).first->second;
						nodeSamplers.emplace_back( channel, animation.samplers[channel.samplerIndex] );
					}
				}

				++animIndex;
			}
		}

		static void listDataAnimations( GltfImporterFile const & file
			, GltfSubmeshData & submeshData )
		{
			auto & asset = file.getAsset();
			size_t animIndex{};

			for ( auto & animation : asset.animations )
			{
				for ( auto & channel : animation.channels )
				{
					if ( isAnimationTarget( asset, channel, submeshData ) )
					{
						for ( auto & primitiveData : submeshData.primitives )
						{
							auto & channelSamplers = primitiveData.anims.try_emplace( file.getAnimationName( animIndex ) ).first->second;
							auto & nodeSamplers = channelSamplers.try_emplace( channel.path ).first->second;
							nodeSamplers.emplace_back( channel, animation.samplers[channel.samplerIndex] );
						}
					}
				}

				++animIndex;
			}
		}

		static c3d::StringMap< GltfMeshData >::iterator mergeMeshes( GltfImporterFile const & file
			, size_t meshIndex
			, c3d::String const & meshName
			, c3d::StringMap< GltfMeshData > & meshes
			, size_t & skinIndex
			, fastgltf::Skin const *& skin )
		{
			auto & asset = file.getAsset();
			// Merge meshes that use the same skeleton
			auto it = std::find_if( asset.nodes.begin()
				, asset.nodes.end()
				, [meshIndex]( fastgltf::Node const & lookup )
				{
					return lookup.skinIndex && lookup.meshIndex
						&& *lookup.meshIndex == meshIndex;
				} );
			auto result = meshes.end();

			if ( it != asset.nodes.end() )
			{
				skinIndex = *it->skinIndex;
				skin = &asset.skins[skinIndex];
				result = std::find_if( meshes.begin()
					, meshes.end()
					, [skin]( c3d::StringMap< GltfMeshData >::value_type const & lookup )
					{
						return skin == lookup.second.skin;
					} );
				if ( result != meshes.end() )
					result = file::replaceIter( meshName, result, meshes ).first;
			}

			return result;
		}

		static auto findNodeMesh( size_t meshIndex
			, c3d::StringMap< GltfMeshData > const & meshes )
		{
			return std::find_if( meshes.begin(), meshes.end()
				, [&meshIndex]( c3d::StringMap< GltfMeshData >::value_type const & lookup )
				{
					return lookup.second.submeshes.end() != std::find_if( lookup.second.submeshes.begin(), lookup.second.submeshes.end()
						, [&meshIndex]( GltfSubmeshData const & submesh )
						{
							return submesh.meshIndex == meshIndex;
						} );
				} );
		}

		static void listNodeMeshes( c3d::Vector< c3d::Matrix4x4f > const & cumulativeTransforms
			, c3d::StringMap< GltfMeshData > const & meshes
			, size_t meshIndex
			, c3d::Matrix4x4f const & matrix
			, c3d::Map< GltfMeshData const *, c3d::Vector< size_t > > & processedMeshes
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
				return true;
			return std::any_of( nodeData.node->children.begin()
				, nodeData.node->children.end()
				, [&sceneData]( size_t lookup )
				{
					return hasNonSkinnedData( sceneData, sceneData.nodes[lookup] );
				} );
		}
	}

	//*********************************************************************************************

	c3d::Point3f convert( fastgltf::math::fvec3 const & value )
	{
		return c3d::Point3f{ value[0], value[1], value[2] };
	}

	c3d::Quaternion convert( fastgltf::math::fquat const & value )
	{
		return c3d::Quaternion::fromComponents( value[0], value[1], value[2], value[3] );
	}

	c3d::NodeTransform convert( std::variant< fastgltf::TRS, fastgltf::math::fmat4x4 > const & transform )
	{
		if ( transform.index() == 0u )
		{
			fastgltf::TRS const & trs = std::get< 0 >( transform );
			return c3d::NodeTransform{ convert( trs.translation )
				, convert( trs.scale )
				, convert( trs.rotation ) };
		}

		fastgltf::math::fvec3 translation;
		fastgltf::math::fvec3 scale;
		fastgltf::math::fquat rotation;
		fastgltf::math::decomposeTransformMatrix( std::get< 1 >( transform ), scale, rotation, translation );
		return c3d::NodeTransform{ convert( translation )
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
				rc = meshopt_decodeVertexBuffer( result.data()
					, mc.count
					, mc.byteStride
					, reinterpret_cast< const unsigned char * >( data.data() )
					, mc.byteLength );
				break;
			case MeshoptCompressionMode::Triangles:
				rc = meshopt_decodeIndexBuffer( result.data()
					, mc.count
					, mc.byteStride
					, reinterpret_cast< const unsigned char * >( data.data() )
					, mc.byteLength );
				break;
			case MeshoptCompressionMode::Indices:
				rc = meshopt_decodeIndexSequence( result.data()
					, mc.count
					, mc.byteStride
					, reinterpret_cast< const unsigned char * >( data.data() )
					, mc.byteLength );
				break;
			}

			if ( rc != 0 )
				return false;

			switch ( mc.filter )
			{
			case MeshoptCompressionFilter::None:
				break;
			case MeshoptCompressionFilter::Octahedral:
				meshopt_decodeFilterOct( result.data(), mc.count, mc.byteStride );
				break;
			case MeshoptCompressionFilter::Quaternion:
				meshopt_decodeFilterQuat( result.data(), mc.count, mc.byteStride );
				break;
			case MeshoptCompressionFilter::Exponential:
				meshopt_decodeFilterExp( result.data(), mc.count, mc.byteStride );
				break;
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

	c3d::MbString const GltfImporterFile::Name = "GLTF Importer";

	GltfImporterFile::GltfImporterFile( c3d::Engine & engine
		, c3d::Scene * scene
		, c3d::Path const & path
		, c3d::Parameters const & parameters
		, c3d::ProgressBar * progress )
		: c3d::ImporterFile{ engine, scene, path, parameters, progress }
		, m_materialsFile{ engine, path, parameters, getPrefix(), getName() }
	{
		if ( isValid() )
		{
			auto const & gltfAsset = getAsset();
			m_adapter.decompress( getAsset() );

			if ( auto sceneIndex = getParameters().get< uint32_t >( cuT( "sceneIndex" ) ) )
			{
				m_sceneIndices.push_back( sceneIndex );
			}
			else if ( gltfAsset.defaultScene )
			{
				m_sceneIndices.push_back( *gltfAsset.defaultScene );
			}
			else
			{
				m_sceneIndices.resize( gltfAsset.scenes.size() );
				std::iota( m_sceneIndices.begin(), m_sceneIndices.end(), 0u );
			}

			engine.getSamplerCache().forEach( [this]( c3d::Sampler const & element )
				{
					m_samplerNames.namesByIndex.try_emplace( 0xFFFFFFFF00000000ULL + m_samplerNames.namesByIndex.size(), element.getName() );
					m_samplerNames.names.emplace( element.getName() );
				} );

			if ( scene )
			{
				scene->getMeshCache().forEach( [this]( c3d::Mesh const & element )
					{
						m_meshNames.namesByIndex.try_emplace( 0xFFFFFFFF00000000ULL + m_meshNames.namesByIndex.size(), element.getName() );
						m_meshNames.names.emplace( element.getName() );

						if ( auto skeleton = element.getSkeleton() )
						{
							m_skinNames.namesByIndex.try_emplace( 0xFFFFFFFF00000000ULL + m_skinNames.namesByIndex.size(), skeleton->getName() );
							m_skinNames.names.emplace( skeleton->getName() );
						}
					} );
				scene->getSceneNodeCache().forEach( [this]( c3d::SceneNode const & element )
					{
						m_nodeNames.namesByIndex.try_emplace( 0xFFFFFFFF00000000ULL + m_nodeNames.namesByIndex.size(), element.getName() );
						m_nodeNames.names.emplace( element.getName() );
					} );
				scene->getLightCache().forEach( [this]( c3d::Light const & element )
					{
						m_lightNames.namesByIndex.try_emplace( 0xFFFFFFFF00000000ULL + m_lightNames.namesByIndex.size(), element.getName() );
						m_lightNames.names.emplace( element.getName() );
					} );
				scene->getCameraCache().forEach( [this]( c3d::Camera const & element )
					{
						m_cameraNames.namesByIndex.try_emplace( 0xFFFFFFFF00000000ULL + m_cameraNames.namesByIndex.size(), element.getName() );
						m_cameraNames.names.emplace( element.getName() );
					} );
			}

			doPrelistMeshes();
			doPrelistNodes();
		}
	}

	c3d::String GltfImporterFile::getMaterialName( size_t index )const
	{
		return m_materialsFile.getMaterialName( index );
	}

	c3d::String GltfImporterFile::getMeshName( size_t index )const
	{
		auto const & gltfAsset = getAsset();
		return getInternalName( getElementName( gltfAsset.meshes, index, getName(), m_meshNames ) );
	}

	c3d::String GltfImporterFile::getNodeName( size_t index, size_t instance )const
	{
		auto const & gltfAsset = getAsset();
		auto result = getElementName( gltfAsset.nodes, index, getName(), m_nodeNames );

		if ( instance )
		{
			result += cuT( "_" ) + c3d::string::toString( instance );
		}

		return getInternalName( result );
	}

	c3d::String GltfImporterFile::getSkinName( size_t index )const
	{
		auto const & gltfAsset = getAsset();
		return getInternalName( getElementName( gltfAsset.skins, index, getName(), m_skinNames ) );
	}

	c3d::String GltfImporterFile::getLightName( size_t index )const
	{
		auto const & gltfAsset = getAsset();
		return getInternalName( getElementName( gltfAsset.lights, index, getName(), m_lightNames ) );
	}

	c3d::String GltfImporterFile::getCameraName( size_t index )const
	{
		auto const & gltfAsset = getAsset();
		return getInternalName( getElementName( gltfAsset.cameras, index, getName(), m_cameraNames ) );
	}

	c3d::String GltfImporterFile::getSamplerName( fastgltf::Sampler const & impSampler )const
	{
		return m_materialsFile.getSamplerName( *getOwner(), impSampler );
	}

	c3d::String GltfImporterFile::getGeometryName( size_t nodeIndex, size_t meshIndex, size_t instance )const
	{
		auto const & gltfAsset = getAsset();
		auto nodeName = getElementName( gltfAsset.nodes, nodeIndex, getName(), m_nodeNames );
		if ( instance )
			nodeName += cuT( "_" ) + c3d::string::toString( instance );

		auto meshName = getElementName( gltfAsset.meshes, meshIndex, getName(), m_meshNames );
		c3d::String result;

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

	c3d::String GltfImporterFile::getAnimationName( size_t index )const
	{
		auto const & gltfAsset = getAsset();
		return getInternalName( getElementName( gltfAsset.animations, index, getName() ) );
	}

	size_t GltfImporterFile::getNodeIndex( c3d::String const & name )const
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

	size_t GltfImporterFile::getSkeletonNodeIndex( c3d::String const & name )const
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

	size_t GltfImporterFile::getMeshIndex( c3d::String const & name, c3d::Submesh const & submesh )const
	{
		auto mit = m_sceneData.meshes.find( name );
		CU_Require( mit != m_sceneData.meshes.end() );
		auto sit = mit->second.submeshes.begin();
		while ( sit != mit->second.submeshes.end() )
		{
			auto pit = sit->primitives.begin();
			while ( pit != sit->primitives.end() )
			{
				if ( &submesh == pit->submesh )
					return sit->meshIndex;
				++pit;
			}
			++sit;
		}
		return ~0u;
	}

	Animations GltfImporterFile::getMeshAnimations( c3d::Mesh const & mesh, c3d::Submesh const & submesh )const
	{
		if ( auto mit = m_sceneData.meshes.find( mesh.getName() );
			mit != m_sceneData.meshes.end() )
		{
			auto sit = mit->second.submeshes.begin();
			while ( sit != mit->second.submeshes.end() )
			{
				auto pit = sit->primitives.begin();
				while ( pit != sit->primitives.end() )
				{
					if ( &submesh == pit->submesh )
						return pit->anims;
					++pit;
				}
				++sit;
			}
		}

		static Animations const dummy;
		return dummy;
	}

	Animations GltfImporterFile::getSkinAnimations( c3d::Skeleton const & skeleton )const
	{
		Animations result;
		size_t index{};

		if ( !getParameters().get< bool >( "no_skeleton" ) )
		{
			auto const & gltfAsset = getAsset();
			for ( auto & animation : gltfAsset.animations )
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
		}

		return result;
	}

	Animations GltfImporterFile::getNodeAnimations( c3d::SceneNode const & node )const
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

	c3d::StringArray GltfImporterFile::listMaterials()
	{
		c3d::StringArray result;
		if ( isValid() )
		{
			auto const & gltfAsset = getAsset();
			for ( size_t i = 0u; i < gltfAsset.materials.size(); ++i )
				result.emplace_back( getMaterialName( i ) );
		}
		return result;
	}

	c3d::Vector< c3d::ImporterFile::MeshData > GltfImporterFile::listMeshes()
	{
		c3d::Vector< MeshData > result;
		for ( auto const & [name, data] : m_sceneData.meshes )
			result.emplace_back( name
				, ( data.skin ? getSkinName( data.skinIndex ) : c3d::String{} ) );
		return result;
	}

	c3d::StringArray GltfImporterFile::listSkeletons()
	{
		c3d::StringArray result;
		if ( isValid() )
		{
			auto const & gltfAsset = getAsset();
			for ( size_t i = 0u; i < gltfAsset.skins.size(); ++i )
				result.emplace_back( getSkinName( i ) );
		}
		return result;
	}

	c3d::Vector< c3d::ImporterFile::NodeData > GltfImporterFile::listSceneNodes()
	{
		c3d::Vector< NodeData > result;
		if ( isValid() )
			for ( auto const * nodeData : m_sceneData.sortedNodes )
				if ( file::hasNonSkinnedData( m_sceneData, *nodeData ) )
					for ( auto const & [instance, _] : nodeData->instances )
						result.emplace_back( instance );
		return result;
	}

	c3d::Vector< c3d::ImporterFile::LightData > GltfImporterFile::listLights()
	{
		c3d::Vector< LightData > result;
		for ( auto & light : m_sceneData.lights )
			result.emplace_back( light.name, light.type );
		return result;
	}

	c3d::Vector< c3d::ImporterFile::LightGroupData > GltfImporterFile::listLightGroups()
	{
		c3d::Vector< LightGroupData > result;
		for ( auto & [_, light] : m_sceneData.lightGroups )
			result.emplace_back( light.name, light.type );
		return result;
	}

	c3d::Vector< c3d::ImporterFile::GeometryData > GltfImporterFile::listGeometries()
	{
		c3d::Vector< GeometryData > result;

		if ( isValid() )
		{
			for ( auto & nodeData : m_sceneData.nodes )
			{
				for ( auto & meshData : nodeData.meshes )
				{
					auto it = std::find_if( m_sceneData.meshes.begin()
						, m_sceneData.meshes.end()
						, [meshData]( c3d::StringMap< GltfMeshData >::value_type const & lookup )
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

	c3d::Vector< c3d::ImporterFile::CameraData > GltfImporterFile::listCameras()
	{
		c3d::Vector< CameraData > result;

		if ( isValid() )
		{
			auto const & gltfAsset = getAsset();
			size_t idx{};

			for ( auto & camera : gltfAsset.cameras )
			{
				result.emplace_back( getCameraName( idx )
					, ( camera.camera.index() == 1u
						? c3d::ViewportType::eOrtho
						: ( std::get< 0u >( camera.camera ).zfar
							? c3d::ViewportType::ePerspective
							: c3d::ViewportType::eInfinitePerspective ) ) );
				++idx;
			}
		}

		return result;
	}

	c3d::StringArray GltfImporterFile::listMeshAnimations( c3d::Mesh const & mesh )
	{
		c3d::Set< c3d::String > result;
		if ( auto it = m_sceneData.meshes.find( mesh.getName() );
			it != m_sceneData.meshes.end() )
			for ( auto const & submesh : it->second.submeshes )
				for ( auto const & [name, _] : submesh.primitives.front().anims )
					result.insert( name );
		return c3d::StringArray{ result.begin()
			, result.end() };
	}

	c3d::StringArray GltfImporterFile::listSkeletonAnimations( c3d::Skeleton const & skeleton )
	{
		c3d::Set< c3d::String > result;

		if ( isValid()
			&& !getParameters().get< bool >( "no_skeleton" ) )
		{
			auto const & gltfAsset = getAsset();
			size_t index{};

			for ( auto & animation : gltfAsset.animations )
			{
				auto it = animation.channels.begin();
				while ( it != animation.channels.end() )
				{
					auto & channel = *it;
					if ( ( channel.path == fastgltf::AnimationPath::Rotation
							|| channel.path == fastgltf::AnimationPath::Scale
							|| channel.path == fastgltf::AnimationPath::Translation )
						&& channel.nodeIndex
						&& isSkeletonNode( *channel.nodeIndex ) )
					{
						result.insert( getAnimationName( index ) );
						it = animation.channels.end();
					}
					else
					{
						++it;
					}
				}

				++index;
			}
		}

		return c3d::StringArray{ result.begin()
			, result.end() };
	}

	c3d::StringArray GltfImporterFile::listSceneNodeAnimations( c3d::SceneNode const & node )
	{
		c3d::StringArray result;

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
				result.push_back( name );
		}

		return result;
	}

	c3d::Vector< uint32_t > GltfImporterFile::listTextureAnimations( c3d::Material const & material
		, uint32_t pass )
	{
		c3d::Vector< uint32_t > result;
		return result;
	}

	uint32_t GltfImporterFile::countAllMeshAnimations()const
	{
		uint32_t result{};
		for ( auto & [_, mesh] : m_sceneData.meshes )
			for ( auto & submesh : mesh.submeshes )
				result += uint32_t( submesh.primitives.front().anims.size() );
		return result;
	}

	uint32_t GltfImporterFile::countAllSkeletonAnimations()const
	{
		c3d::Set< c3d::String > result;

		if ( isValid()
			&& !getParameters().get< bool >( "no_skeleton" ) )
		{
			auto const & gltfAsset = getAsset();
			size_t index{};

			for ( auto & animation : gltfAsset.animations )
			{
				auto it = animation.channels.begin();
				while ( it != animation.channels.end() )
				{
					auto & channel = *it;
					if ( ( channel.path == fastgltf::AnimationPath::Rotation
						|| channel.path == fastgltf::AnimationPath::Scale
						|| channel.path == fastgltf::AnimationPath::Translation )
						&& channel.nodeIndex
						&& isSkeletonNode( *channel.nodeIndex ) )
					{
						result.insert( getAnimationName( index ) );
						it = animation.channels.end();
					}
					else
					{
						++it;
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
			result += uint32_t( node.anims.size() );
		return result;
	}

	uint32_t GltfImporterFile::countAllTextureAnimations()const
	{
		return 0u;
	}

	c3d::MaterialImporterUPtr GltfImporterFile::createMaterialImporter()
	{
		return m_materialsFile.createMaterialImporter( *getOwner() );
	}

	c3d::AnimationImporterUPtr GltfImporterFile::createAnimationImporter()
	{
		return c3d::makeUniqueDerived< c3d::AnimationImporter, GltfAnimationImporter >( *getOwner() );
	}

	c3d::SkeletonImporterUPtr GltfImporterFile::createSkeletonImporter()
	{
		return c3d::makeUniqueDerived< c3d::SkeletonImporter, GltfSkeletonImporter >( *getOwner() );
	}

	c3d::MeshImporterUPtr GltfImporterFile::createMeshImporter()
	{
		return c3d::makeUniqueDerived< c3d::MeshImporter, GltfMeshImporter >( *getOwner() );
	}

	c3d::SceneNodeImporterUPtr GltfImporterFile::createSceneNodeImporter()
	{
		return c3d::makeUniqueDerived< c3d::SceneNodeImporter, GltfSceneNodeImporter >( *getOwner() );
	}

	c3d::LightImporterUPtr GltfImporterFile::createLightImporter()
	{
		return c3d::makeUniqueDerived< c3d::LightImporter, GltfLightImporter >( *getOwner() );
	}

	c3d::CameraImporterUPtr GltfImporterFile::createCameraImporter()
	{
		return c3d::makeUniqueDerived< c3d::CameraImporter, GltfCameraImporter >( *getOwner() );
	}

	c3d::ImporterFileUPtr GltfImporterFile::create( c3d::Engine & engine
		, c3d::Scene * scene
		, c3d::Path const & path
		, c3d::Parameters const & parameters
		, c3d::ProgressBar * progress )
	{
		return c3d::makeUniqueDerived< c3d::ImporterFile, GltfImporterFile >( engine, scene, path, parameters, progress );
	}

	void GltfImporterFile::doPrelistNodes()
	{
		c3d::Vector< c3d::Matrix4x4f > cumulativeTransforms;
		c3d::Vector< size_t > skinsRootNodes;
		bool noSkeleton = getParameters().get< bool >( "no_skeleton" );
		auto const & gltfAsset = getAsset();

		if ( !noSkeleton )
		{
			for ( auto & skin : gltfAsset.skins )
			{
				auto skinRootNodes = findSkinRootNodes( *this, skin );
				skinsRootNodes.insert( skinsRootNodes.end(), skinRootNodes.begin(), skinRootNodes.end() );
			}
		}

		// First, list all nodes, with their own transforms and instances
		m_sceneData.nodes.reserve( gltfAsset.nodes.size() );
		cumulativeTransforms.resize( gltfAsset.nodes.size() );
		size_t nodeIndex{};
		for ( auto & node : gltfAsset.nodes )
		{
			auto transform = convert( node.transform );
			if ( node.cameraIndex )
				transform.rotate *= c3d::Quaternion::fromAxisAngle( c3d::Point3f{ 0.0f, 1.0f, 0.0f }, c3d::Angle::fromDegrees( 180.0f ) );

			bool isSkeletonNode = ( noSkeleton
				? false
				: file::isSkeletonNode( getAsset(), gltfAsset.skins, nodeIndex, skinsRootNodes ) );
			auto & nodeData = m_sceneData.nodes.emplace_back( node.cameraIndex.has_value()
				, isSkeletonNode
				, nodeIndex
				, &node );
			c3d::matrix::setTransform( cumulativeTransforms[nodeIndex]
				, transform.translate, transform.scale, transform.rotate );
			nodeData.instances.emplace_back( NodeData{ c3d::String{}, getNodeName( nodeIndex, 0u ), nodeData.isCamera }, transform );
			auto instances = file::listInstances( getAsset(), m_adapter, nodeData );

			// List this node's instances.
			size_t instanceIndex{ 1u };
			for ( auto & instanceTransform : instances )
			{
				nodeData.instances.emplace_back( NodeData{ c3d::String{}
						, getNodeName( nodeIndex, instanceIndex )
						, nodeData.isCamera }
					, c3d::move( instanceTransform ) );
				++instanceIndex;
			}

			++nodeIndex;
		}

		// Then build the hierarchy, updating instances when needed
		for ( auto const & sceneIndex : m_sceneIndices )
		{
			c3d::Vector< size_t > work;
			for ( auto index : gltfAsset.scenes[sceneIndex].nodeIndices )
			{
				work.emplace_back( index );
				m_sceneData.sortedNodes.emplace_back( &m_sceneData.nodes[index] );
			}

			while ( !work.empty() )
			{
				auto parentNodeIndex = work.back();
				work.pop_back();
				auto const & parentNodeData = m_sceneData.nodes[parentNodeIndex];

				for ( auto childNodeIndex : gltfAsset.nodes[parentNodeIndex].children )
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
		c3d::Map< GltfMeshData const *, c3d::Vector< size_t > > processedMeshes;
		for ( auto nodeData : m_sceneData.sortedNodes )
		{
			auto & node = *nodeData->node;

			//
			if ( node.meshIndex )
			{
				file::listNodeMeshes( cumulativeTransforms, m_sceneData.meshes, *node.meshIndex, cumulativeTransforms[nodeData->index]
					, processedMeshes, *nodeData );
			}

			// Check for light
			if ( node.lightIndex )
			{
				if ( auto lightIndex = *node.lightIndex;
					lightIndex < gltfAsset.lights.size() )
				{
					auto light = gltfAsset.lights[lightIndex];
					auto lightName = getLightName( lightIndex );
					auto & lightGroup = m_sceneData.lightGroups.try_emplace( lightName
						, lightName
						, ( light.type == fastgltf::LightType::Directional
							? c3d::LightType::eDirectional
							: ( light.type == fastgltf::LightType::Point
								? c3d::LightType::ePoint
								: c3d::LightType::eSpot ) )
						, uint32_t( lightIndex ) ).first->second;

					for ( auto const & [nodeInstanceData, _] : nodeData->instances )
					{
						lightGroup.nodeNames.push_back( nodeInstanceData.name );
					}
				}
			}

			if ( nodeData->isSkeleton )
				m_sceneData.skeletonNodes.emplace_back( nodeData );
			else
				file::listDataAnimations( *this, *nodeData );
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
		auto noMeshMerge = getParameters().get< bool >( "no_merge" );
		auto const & gltfAsset = getAsset();

		for ( auto & impMesh : gltfAsset.meshes )
		{
			auto meshName = getMeshName( meshIndex );

			auto regIt = m_sceneData.meshes.find( meshName );
			fastgltf::Skin const * skin{};
			size_t skinIndex{};

			if ( regIt != m_sceneData.meshes.end() )
			{
				meshName += c3d::string::toString( meshIndex );
				regIt = m_sceneData.meshes.find( meshName );
			}

			// Try to merge the mesh with other ones (if they share the same skin)
			if ( regIt == m_sceneData.meshes.end() && !noMeshMerge )
				regIt = file::mergeMeshes( *this, meshIndex, meshName
					, m_sceneData.meshes, skinIndex, skin );

			// Brand new mesh
			if ( regIt == m_sceneData.meshes.end() )
				regIt = m_sceneData.meshes.try_emplace( meshName, skin, skinIndex ).first;

			auto & submeshData = regIt->second.submeshes.emplace_back( &impMesh, meshIndex );

			// Now split submesh by primitive type.
			uint32_t primitiveIndex{};
			for ( auto & primitive : submeshData.mesh->primitives )
			{
				c3d::String material = primitive.materialIndex
					? getMaterialName( uint32_t( *primitive.materialIndex ) )
					: DefaultMaterial;
				submeshData.primitives.emplace_back( primitiveIndex, material, &primitive );
				++primitiveIndex;
			}

			file::listDataAnimations( *this, submeshData );

			++meshIndex;
		}
	}

	//*********************************************************************************************
}
