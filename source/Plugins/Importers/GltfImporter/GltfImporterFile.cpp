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

namespace c3d_gltf
{
	//*********************************************************************************************

	namespace file
	{
		static fastgltf::Expected< fastgltf::Asset > loadScene( castor::Path const & filePath
			, castor3d::Parameters const & parameters
			, fastgltf::Category category )
		{
			fastgltf::Parser parser{ fastgltf::Extensions::KHR_texture_transform
				| fastgltf::Extensions::MSFT_texture_dds
				| fastgltf::Extensions::KHR_mesh_quantization
				| fastgltf::Extensions::EXT_mesh_gpu_instancing
				| fastgltf::Extensions::EXT_meshopt_compression
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
				| fastgltf::Extensions::KHR_materials_unlit };
			auto path = castor::makePath( filePath );

			constexpr auto gltfOptions = fastgltf::Options::DontRequireValidAssetMember
				| fastgltf::Options::AllowDouble
				| fastgltf::Options::LoadGLBBuffers
				| fastgltf::Options::LoadExternalBuffers
				| fastgltf::Options::DecomposeNodeMatrices
				| fastgltf::Options::LoadExternalImages;

			fastgltf::GltfDataBuffer data;

			if ( !data.loadFromFile( path ) )
			{
				castor3d::log::error << "Failed to to initialise glTF buffer" << std::endl;
				return fastgltf::Expected< fastgltf::Asset >( fastgltf::Error::InvalidPath );
			}

			auto type = fastgltf::determineGltfFileType( &data );

			if ( type != fastgltf::GltfType::glTF
				&& type != fastgltf::GltfType::GLB )
			{
				castor3d::log::error << "Failed to determine glTF container" << std::endl;
				return fastgltf::Expected< fastgltf::Asset >( fastgltf::Error::InvalidPath );
			}

			auto result = type == fastgltf::GltfType::glTF
				? parser.loadGLTF( &data, path.parent_path(), gltfOptions )
				: parser.loadBinaryGLTF( &data, path.parent_path(), gltfOptions );

			if ( result.error() != fastgltf::Error::None )
			{
				castor3d::log::error << "Failed to load glTF: " << fastgltf::getErrorMessage( result.error() ) << std::endl;
			}

			return result;
		}

		static void parseNodesRec( fastgltf::pmr::MaybeSmallVector< size_t > const & nodes
			, size_t parentNodeIndex
			, std::vector< fastgltf::Node > const & allNodes
			, std::function< std::tuple< size_t, bool, bool >( fastgltf::Node const &, size_t, size_t, size_t, bool ) > func
			, std::set< size_t > & parsed
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
			, std::vector< fastgltf::Node > const & allNodes
			, std::function< std::tuple< size_t, bool, bool >( fastgltf::Node const &, size_t, size_t, size_t, bool ) > func )
		{
			std::set< size_t > parsed;
			size_t parent = ~0u;
			parseNodesRec( nodes, parent, allNodes, func, parsed, 1u, false );
		}

		static std::vector< castor3d::NodeTransform > listInstances( fastgltf::Asset const & impAsset
			, fastgltf::Node const & impNode )
		{
			castor::Point3fArray translations;
			castor::QuaternionArray rotations;
			castor::Point3fArray scalings;
			auto tit = impNode.findInstancingAttribute( "TRANSLATION" );
			auto rit = impNode.findInstancingAttribute( "ROTATION" );
			auto sit = impNode.findInstancingAttribute( "SCALE" );

			if ( tit != impNode.instancingAttributes.end() )
			{
				fastgltf::iterateAccessor< castor::Point3f >( impAsset
					, impAsset.accessors[tit->second]
					, [&translations]( castor::Point3f value )
					{
						translations.push_back( std::move( value ) );
					} );
			}

			if ( rit != impNode.instancingAttributes.end() )
			{
				fastgltf::iterateAccessor< castor::Point4f >( impAsset
					, impAsset.accessors[rit->second]
					, [&rotations]( castor::Point4f const & value )
					{
						rotations.push_back( castor::Quaternion{ value } );
					} );
			}

			if ( sit != impNode.instancingAttributes.end() )
			{
				fastgltf::iterateAccessor< castor::Point3f >( impAsset
					, impAsset.accessors[sit->second]
					, [&scalings]( castor::Point3f value )
					{
						scalings.push_back( std::move( value ) );
					} );
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

			std::vector< castor3d::NodeTransform > result;
			result.reserve( instanceCount );

			for ( size_t i = 0u; i < instanceCount; ++i )
			{
				result.push_back( { translations[i], scalings[i], rotations[i] } );
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
				, [&result, &lookupIndex]( fastgltf::Node const & node, size_t nodeIndex, size_t parentIndex, size_t parentInstanceCount, bool )
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
			, auto const & skeletons
			, size_t nodeIndex )
		{
			return std::any_of( skeletons.begin()
				, skeletons.end()
				, [&impAsset, nodeIndex]( fastgltf::Skin const & lookup )
				{
					return /*( lookup.skeleton && *lookup.skeleton == nodeIndex )
						|| */lookup.joints.end() != std::find_if( lookup.joints.begin()
						, lookup.joints.end()
						, [&impAsset, nodeIndex]( size_t lookupIndex )
						{
							return lookupIndex == nodeIndex
								|| hasChildNode( impAsset, lookupIndex, nodeIndex );
						} );
				} );
		}

		static auto findNodeMesh( size_t meshIndex
			, std::map< castor::String, GltfMeshData > const & meshes )
		{
			return std::find_if( meshes.begin()
				, meshes.end()
				, [&meshIndex]( std::map< castor::String, GltfMeshData >::value_type const & lookup )
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
			, castor::String const & baseName )
		{
			castor::String result = castor::String( elements[index].name );

			if ( result.empty() )
			{
				result += baseName;
				result += cuT( "-" ) + castor::string::toString( index );
			}

			return result;
		}

		static castor::String getElementName( auto const & elements
			, size_t index
			, castor::String const & baseName
			, NameContainer & names )
		{
			auto it = std::find_if( names.begin(), names.end()
				, [index]( IndexName const & lookup )
				{
					return lookup.first == index;
				} );

			if ( it != names.end() )
			{
				return it->second;
			}

			castor::String result = castor::String( elements[index].name );

			if ( result.empty() )
			{
				result = baseName;
			}

			it = std::find_if( names.begin(), names.end()
				, [&result]( IndexName const & lookup )
				{
					return lookup.second == result;
				} );

			if ( it != names.end() )
			{
				result += cuT( "-" ) + castor::string::toString( index );
			}

			names.emplace_back( index, result );
			return result;
		}

		static std::string getLongestCommonSubstring( std::string const & a, std::string const & b )
		{
			auto result = castor::string::getLongestCommonSubstring( a, b );
			return castor::string::trim( result
				, true
				, true
				, " \r\t-_/\\|*$<>[](){}" );
		}

		template< typename IterT, typename TypeT >
		static std::pair< IterT, castor::String > replaceIter( castor::String const & name
			, IterT iter
			, std::map< castor::String, TypeT > & map )
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

		static bool isAnimationTarget( fastgltf::Asset const & asset
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
			return *asset.nodes[channel.nodeIndex].meshIndex == submeshData.meshIndex
				&& channel.path == fastgltf::AnimationPath::Weights
				&& asset.nodes[channel.nodeIndex].meshIndex;
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

		static std::map< castor::String, GltfMeshData >::iterator mergeMeshes( GltfImporterFile const & file
			, size_t meshIndex
			, castor::String const & meshName
			, std::map< castor::String, GltfMeshData > & meshes
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
					, [skin]( std::map< castor::String, GltfMeshData >::value_type const & lookup )
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

		static void listNodeMeshes( GltfImporterFile const & file
			, std::map< size_t, castor::Matrix4x4f > const & cumulativeTransforms
			, std::map< castor::String, GltfMeshData > const & meshes
			, size_t meshIndex
			, castor::Matrix4x4f const & matrix
			, std::map< GltfMeshData const *, std::vector< size_t > > & processedMeshes
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
					auto & nodeArray = processedMeshes.emplace( &it->second, std::vector< size_t >{} ).first->second;
					auto nodeIt = std::find_if( nodeArray.begin()
						, nodeArray.end()
						, [&cumulativeTransforms, &matrix]( size_t lookup )
						{
							auto lookupIt = cumulativeTransforms.find( lookup );
							return lookupIt->second == matrix;
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

		static void addNode( GltfImporterFile const & file
			, GltfNodeData nodeData
			, bool isSkeletonNode
			, size_t parentIndex
			, size_t & parentInstanceCount
			, std::vector< GltfNodeData > & nodes
			, std::vector< GltfNodeData > & skeletonNodes )
		{
			if ( isSkeletonNode )
			{
				parentInstanceCount = 1u;
				skeletonNodes.push_back( std::move( nodeData ) );
			}
			else
			{
				auto & asset = file.getAsset();
				auto transforms = file::listInstances( asset, *nodeData.node );

				if ( !transforms.empty() )
				{
					auto instanceCount = transforms.size();

					if ( parentInstanceCount > 1u )
					{
						size_t index{};

						for ( size_t i = 0u; i < instanceCount; ++i )
						{
							for ( size_t j = 0u; j < parentInstanceCount; ++j )
							{
								nodeData.parent = parentIndex < asset.nodes.size() ? file.getNodeName( parentIndex, j + 1u ) : std::string{};
								nodeData.name = file.getNodeName( nodeData.index, index + 1u );
								nodeData.instance = index + 1u;
								nodeData.instanceCount = parentInstanceCount * instanceCount;
								nodeData.transform = transforms[i];
								nodes.push_back( nodeData );
								++index;
							}
						}
					}
					else
					{
						for ( size_t i = 0u; i < instanceCount; ++i )
						{
							nodeData.parent = parentIndex < asset.nodes.size() ? file.getNodeName( parentIndex, 0u ) : std::string{};
							nodeData.name = file.getNodeName( nodeData.index, i + 1u );
							nodeData.instance = i + 1u;
							nodeData.instanceCount = instanceCount;
							nodeData.transform = std::move( transforms[i] );
							nodes.push_back( nodeData );
						}
					}

					parentInstanceCount *= instanceCount;
				}
				else
				{
					if ( parentInstanceCount > 1u )
					{
						for ( size_t i = 0u; i < parentInstanceCount; ++i )
						{
							nodeData.parent = parentIndex < asset.nodes.size() ? file.getNodeName( parentIndex, i + 1u ) : std::string{};
							nodeData.name = file.getNodeName( nodeData.index, i + 1u );
							nodeData.instance = i + 1u;
							nodeData.instanceCount = parentInstanceCount;
							nodes.push_back( nodeData );
						}
					}
					else
					{
						nodes.push_back( std::move( nodeData ) );
					}
				}
			}
		}
	}

	//*********************************************************************************************

	castor3d::NodeTransform convert( std::variant< fastgltf::Node::TRS, fastgltf::Node::TransformMatrix > const & transform )
	{
		if ( transform.index() == 0u )
		{
			fastgltf::Node::TRS const & trs = std::get< 0 >( transform );
			return { convert( trs.translation )
				, convert( trs.scale )
				, convert( trs.rotation ) };
		}

		std::array< float, 3u > translation;
		std::array< float, 3u > scale;
		std::array< float, 4u > rotation;
		fastgltf::decomposeTransformMatrix( std::get< 1 >( transform ), scale, rotation, translation );
		return { convert( translation )
			, convert( scale )
			, convert( rotation ) };
	}

	castor::Point3f convert( std::array< float, 3u > const & value )
	{
		return castor::Point3f{ value[0], value[1], value[2] };;
	}

	castor::Quaternion convert( std::array< float, 4u > const & value )
	{
		return castor::Quaternion::fromComponents( value[0], value[1], value[2], value[3] );
	}

	//*********************************************************************************************

	castor::String const GltfImporterFile::Name = cuT( "GLTF Importer" );

	GltfImporterFile::GltfImporterFile( castor3d::Engine & engine
		, castor3d::Scene * scene
		, castor::Path const & path
		, castor3d::Parameters const & parameters
		, fastgltf::Category category )
		: castor3d::ImporterFile{ engine, scene, path, parameters }
		, m_expAsset{ file::loadScene( getFileName(), getParameters(), category ) }
	{
		if ( isValid() )
		{
			m_asset = &m_expAsset.get< 1 >();
			uint32_t sceneIndex{};

			if ( getParameters().get( "sceneIndex", sceneIndex ) )
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
					m_materialNames.emplace_back( ~0ull, element.getName() );
				} );
			engine.getSamplerCache().forEach( [this]( castor3d::Sampler const & element )
				{
					m_samplerNames.emplace_back( ~0ull, element.getName() );
				} );

			if ( scene )
			{
				scene->getMeshCache().forEach( [this]( castor3d::Mesh const & element )
					{
						m_meshNames.emplace_back( ~0ull, element.getName() );

						if ( auto skeleton = element.getSkeleton() )
						{
							m_skinNames.emplace_back( ~0ull, skeleton->getName() );
						}
					} );
				scene->getSceneNodeCache().forEach( [this]( castor3d::SceneNode const & element )
					{
						m_nodeNames.emplace_back( ~0ull, element.getName() );
					} );
				scene->getLightCache().forEach( [this]( castor3d::Light const & element )
					{
						m_lightNames.emplace_back( ~0ull, element.getName() );
					} );
				scene->getCameraCache().forEach( [this]( castor3d::Camera const & element )
					{
						m_cameraNames.emplace_back( ~0ull, element.getName() );
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
		auto & engine = *getOwner();
		auto & defaultSampler = *engine.getDefaultSampler();
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
				return lookup.name == name;
			} );
		CU_Require( it != m_sceneData.nodes.end() );
		return it->index;
	}

	size_t GltfImporterFile::getSkeletonNodeIndex( castor::String const & name )const
	{
		auto it = std::find_if( m_sceneData.skeletonNodes.begin()
			, m_sceneData.skeletonNodes.end()
			, [&name]( GltfNodeData const & lookup )
			{
				return lookup.name == name;
			} );
		CU_Require( it != m_sceneData.skeletonNodes.end() );
		return it->index;
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
		auto it = m_sceneData.meshes.find( mesh.getName() );

		if ( it != m_sceneData.meshes.end()
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
					&& isSkeletonNode( channel.nodeIndex )
					&& skeleton.findNode( getNodeName( channel.nodeIndex, 0u ) ) != nullptr )
				{
					auto & channelSamplers = result.emplace( getAnimationName( index ), AnimationChannelSamplers{} ).first->second;
					auto & nodeSamplers = channelSamplers.emplace( channel.path, NodeAnimationChannelSampler{} ).first->second;
					nodeSamplers.emplace_back( channel, animation.samplers[channel.samplerIndex] );
				}
			}

			++index;
		}

		return result;
	}

	Animations GltfImporterFile::getNodeAnimations( castor3d::SceneNode const & node )const
	{
		auto it = std::find_if( m_sceneData.nodes.begin()
			, m_sceneData.nodes.end()
			, [&node]( GltfNodeData const & lookup )
			{
				return node.getName() == lookup.name;
			} );

		if ( it != m_sceneData.nodes.end() )
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
			, [nodeIndex]( GltfNodeData const & lookup )
			{
				return lookup.index == nodeIndex;
			} );
	}

	std::vector< castor::String > GltfImporterFile::listMaterials()
	{
		std::vector< castor::String > result;

		if ( isValid() )
		{
			for ( size_t i = 0u; i < m_asset->materials.size(); ++i )
			{
				result.emplace_back( getMaterialName( i ) );
			}
		}

		return result;
	}

	std::vector< castor3d::ImporterFile::MeshData > GltfImporterFile::listMeshes()
	{
		std::vector< MeshData > result;

		for ( auto it : m_sceneData.meshes )
		{
			result.emplace_back( it.first
				, ( it.second.skin
					? getSkinName( it.second.skinIndex )
					: castor::String{} ) );
		}

		return result;
	}

	std::vector< castor::String > GltfImporterFile::listSkeletons()
	{
		std::vector< castor::String > result;

		if ( isValid() )
		{
			for ( size_t i = 0u; i < m_asset->skins.size(); ++i )
			{
				result.emplace_back( getSkinName( i ) );
			}
		}

		return result;
	}

	std::vector< castor3d::ImporterFile::NodeData > GltfImporterFile::listSceneNodes()
	{
		std::vector< NodeData > result;

		if ( isValid() )
		{
			for ( auto & nodeData : m_sceneData.nodes )
			{
				result.push_back( nodeData );
			}
		}

		return result;
	}

	std::vector< castor3d::ImporterFile::LightData > GltfImporterFile::listLights()
	{
		std::vector< LightData > result;

		if ( isValid() )
		{
			size_t idx{};

			for ( auto & light : m_asset->lights )
			{
				result.emplace_back( getLightName( idx++ )
					, ( light.type == fastgltf::LightType::Directional
						? castor3d::LightType::eDirectional
						: ( light.type == fastgltf::LightType::Point
							? castor3d::LightType::ePoint
							: castor3d::LightType::eSpot ) ) );
			}
		}

		return result;
	}

	std::vector< castor3d::ImporterFile::GeometryData > GltfImporterFile::listGeometries()
	{
		std::vector< GeometryData > result;

		if ( isValid() )
		{
			for ( auto & nodeData : m_sceneData.nodes )
			{
				for ( auto & meshData : nodeData.meshes )
				{
					if ( nodeData.instanceCount <= 1u || nodeData.instance != 0u )
					{
						auto it = std::find_if( m_sceneData.meshes.begin()
							, m_sceneData.meshes.end()
							, [meshData]( std::map< castor::String, GltfMeshData >::value_type const & lookup )
							{
								return meshData == &lookup.second;
							} );
						CU_Require( it != m_sceneData.meshes.end() );
						auto meshName = it->first;
						auto name = nodeData.name == meshName
							? nodeData.name
							: nodeData.name + cuT( "_" ) + meshName;
						result.emplace_back( name, nodeData.name, it->first );
					}
				}
			}
		}

		return result;
	}

	std::vector< castor3d::ImporterFile::CameraData > GltfImporterFile::listCameras()
	{
		std::vector< CameraData > result;

		if ( isValid() )
		{
			size_t idx{};

			for ( auto & camera : m_asset->cameras )
			{
				result.emplace_back( getCameraName( idx++ )
					, ( camera.camera.index() == 1u
						? castor3d::ViewportType::eOrtho
						: ( std::get< 0u >( camera.camera ).zfar
							? castor3d::ViewportType::ePerspective
							: castor3d::ViewportType::eInfinitePerspective ) ) );
			}
		}

		return result;
	}

	std::vector< castor::String > GltfImporterFile::listMeshAnimations( castor3d::Mesh const & mesh )
	{
		std::set< castor::String > result;
		auto it = m_sceneData.meshes.find( mesh.getName() );

		if ( it != m_sceneData.meshes.end() )
		{
			for ( auto & submesh : it->second.submeshes )
			{
				for ( auto & anim : submesh.anims )
				{
					result.insert( anim.first );
				}
			}
		}

		return std::vector< castor::String >{ result.begin()
			, result.end() };
	}

	std::vector< castor::String > GltfImporterFile::listSkeletonAnimations( castor3d::Skeleton const & skeleton )
	{
		std::set< castor::String > result;

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
						&& isSkeletonNode( channel.nodeIndex ) )
					{
						result.insert( getAnimationName( index ) );
					}
				}

				++index;
			}
		}

		return std::vector< castor::String >{ result.begin()
			, result.end() };
	}

	std::vector< castor::String > GltfImporterFile::listSceneNodeAnimations( castor3d::SceneNode const & node )
	{
		std::vector< castor::String > result;
		auto it = std::find_if( m_sceneData.nodes.begin()
			, m_sceneData.nodes.end()
			, [&node]( GltfNodeData const & lookup )
			{
				return node.getName() == lookup.name;
			} );

		if ( it != m_sceneData.nodes.end() )
		{
			for ( auto & anim : it->anims )
			{
				result.push_back( anim.first );
			}
		}

		return result;
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
		, castor3d::Parameters const & parameters )
	{
		return castor::makeUniqueDerived< castor3d::ImporterFile, GltfImporterFile >( engine, scene, path, parameters );
	}

	void GltfImporterFile::doPrelistNodes()
	{
		std::map< GltfMeshData const *, std::vector< size_t > > processedMeshes;
		std::map< size_t, castor::Matrix4x4f > cumulativeTransforms;

		for ( auto & sceneIndex : m_sceneIndices )
		{
			file::parseNodes( m_asset->scenes[sceneIndex].nodeIndices
				, m_asset->nodes
				, [this, &processedMeshes, &cumulativeTransforms]( fastgltf::Node const & node, size_t nodeIndex, size_t parentIndex, size_t parentInstanceCount, bool isParentSkeletonNode )
				{
					auto transform = convert( node.transform );

					if ( node.cameraIndex )
					{
						transform.rotate *= castor::Quaternion::fromAxisAngle( castor::Point3f{ 0.0f, 1.0f, 0.0f }, castor::Angle::fromDegrees( 180.0f ) );
					}

					GltfNodeData nodeData{ parentIndex < m_asset->nodes.size() ? getNodeName( parentIndex, 0u ) : std::string{}
						, getNodeName( nodeIndex, 0u )
						, node.cameraIndex.has_value()
						, nodeIndex
						, 0u
						, 1u
						, transform
						, &node };
					auto itTransform = cumulativeTransforms.find( parentIndex );
					auto matrix = itTransform != cumulativeTransforms.end()
						? itTransform->second
						: castor::Matrix4x4f{ 1.0f };

					castor::matrix::transform( matrix, transform.translate, transform.scale, transform.rotate );

					cumulativeTransforms.emplace( nodeIndex, matrix );
					bool isSkeletonNode = isParentSkeletonNode
						|| file::isSkeletonNode( *m_asset, m_asset->skins, nodeData.index );

					// List scene node animations
					if ( !isSkeletonNode )
					{
						file::listDataAnimations( *this, nodeData );
					}

					// List scene node meshes
					if ( node.meshIndex )
					{
						file::listNodeMeshes( *this, cumulativeTransforms, m_sceneData.meshes, *node.meshIndex, matrix
							, processedMeshes, nodeData );
					}

					file::addNode( *this, std::move( nodeData ), isSkeletonNode, parentIndex
						, parentInstanceCount
						, m_sceneData.nodes, m_sceneData.skeletonNodes );
					return std::make_tuple( parentInstanceCount, true, isSkeletonNode );
				} );
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
				regIt = m_sceneData.meshes.emplace( meshName
					, GltfMeshData{ skin, skinIndex } ).first;
			}

			file::listDataAnimations( *this
				, regIt->second.submeshes.emplace_back( &impMesh, meshIndex ) );

			++meshIndex;
		}
	}

	//*********************************************************************************************
}
