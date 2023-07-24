#include "GltfImporter/GltfImporterFile.hpp"

#include "GltfImporter/GltfAnimationImporter.hpp"
#include "GltfImporter/GltfLightImporter.hpp"
#include "GltfImporter/GltfMaterialImporter.hpp"
#include "GltfImporter/GltfMeshImporter.hpp"
#include "GltfImporter/GltfSceneNodeImporter.hpp"
#include "GltfImporter/GltfSkeletonImporter.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Model/Mesh/Mesh.hpp>
#include <Castor3D/Model/Skeleton/Skeleton.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/SceneNode.hpp>

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
			, std::function< std::pair< size_t, bool >( fastgltf::Node const &, size_t, size_t, size_t ) > func
			, std::set< size_t > & parsed
			, size_t instanceCount )
		{
			bool carryOn = true;

			for ( auto & nodeIndex : nodes )
			{
				if ( parsed.insert( nodeIndex ).second )
				{
					fastgltf::Node const & node = allNodes[nodeIndex];
					auto [curInstanceCount, isOk] = func( node, nodeIndex, parentNodeIndex, instanceCount );
					carryOn = isOk;

					if ( carryOn )
					{
						parseNodesRec( node.children, nodeIndex, allNodes, func, parsed, curInstanceCount );
					}
				}
			}
		}

		static void parseNodes( fastgltf::pmr::MaybeSmallVector< size_t > const & nodes
			, std::vector< fastgltf::Node > const & allNodes
			, std::function< std::pair< size_t, bool >( fastgltf::Node const &, size_t, size_t, size_t ) > func )
		{
			std::set< size_t > parsed;
			size_t parent = ~0u;
			parseNodesRec( nodes, parent, allNodes, func, parsed, 1u );
		}

		static std::vector< castor3d::NodeTransform > listInstances( fastgltf::Asset const & impAsset
			, fastgltf::Node const & impNode )
		{
			castor::Point3fArray translations;
			castor::QuaternionArray rotations;
			castor::Point3fArray scalings;
			auto tit = findAttribute( impNode.instancingAttributes, "TRANSLATION" );
			auto rit = findAttribute( impNode.instancingAttributes, "ROTATION" );
			auto sit = findAttribute( impNode.instancingAttributes, "SCALE" );

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

		bool hasChildNode( fastgltf::Asset const & impAsset
			, size_t rootIndex
			, size_t lookupIndex )
		{
			bool result{};
			parseNodes( impAsset.nodes[rootIndex].children
				, impAsset.nodes
				, [&result, &lookupIndex]( fastgltf::Node const & node, size_t nodeIndex, size_t parentIndex, size_t parentInstanceCount )
				{
					if ( nodeIndex == lookupIndex )
					{
						result = true;
						return std::make_pair( size_t( 1u ), false );
					}

					return std::make_pair( size_t( 1u ), true );
				} );
			return result;
		}

		bool isSkeletonNode( auto const & skeletons
			, size_t index )
		{
			return std::any_of( skeletons.begin()
				, skeletons.end()
				, [index]( fastgltf::Skin const & lookup )
				{
					return lookup.joints.end() != std::find( lookup.joints.begin()
						, lookup.joints.end()
						, index );
				} );
		}

		castor::String getElementName( auto const & elements
			, size_t index
			, castor::String const & baseName )
		{
			castor::String result = castor::String( elements[index].name );

			if ( result.empty() )
			{
				result += baseName;
			}

			return result + cuT( "-" ) + castor::string::toString( index );
		}
	}

	//*********************************************************************************************

	castor3d::NodeTransform convert( std::variant< fastgltf::Node::TRS, fastgltf::Node::TransformMatrix > const & transform )
	{
		if ( transform.index() == 0u )
		{
			fastgltf::Node::TRS const & trs = std::get< 0 >( transform );
			return { castor::Point3f{ trs.translation[0], trs.translation[1], trs.translation[2] }
			, castor::Point3f{ trs.scale[0], trs.scale[1], trs.scale[2] }
				, castor::Quaternion::fromComponents( trs.rotation[0], trs.rotation[1], trs.rotation[2], trs.rotation[3] ) };
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
		, m_asset{ file::loadScene( getFileName(), getParameters(), category ) }
	{
		uint32_t nodeIndex{};

		for ( auto & node : m_asset->nodes )
		{
			GltfNodeData nodeData{ castor::String{}
				, getNodeName( nodeIndex, 0u )
				, nodeIndex
				, 0u
				, convert( node.transform )
				, &node };

			if ( file::isSkeletonNode( m_asset->skins, nodeIndex ) )
			{
				m_skeletonNodes.push_back( std::move( nodeData ) );
			}

			++nodeIndex;
		}

		if ( isValid() )
		{
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
		}
	}

	castor::String GltfImporterFile::getMaterialName( size_t index )const
	{
		return file::getElementName( m_asset->materials, index, getName() );
	}

	castor::String GltfImporterFile::getMeshName( size_t index )const
	{
		return file::getElementName( m_asset->meshes, index, getName() );
	}

	castor::String GltfImporterFile::getNodeName( size_t index, size_t instance )const
	{
		auto result = file::getElementName( m_asset->nodes, index, getName() );

		if ( instance )
		{
			result += cuT( "_" ) + castor::string::toString( instance );
		}

		return result;
	}

	castor::String GltfImporterFile::getSkinName( size_t index )const
	{
		return file::getElementName( m_asset->skins, index, getName() );
	}

	castor::String GltfImporterFile::getLightName( size_t index )const
	{
		return file::getElementName( m_asset->lights, index, getName() );
	}

	castor::String GltfImporterFile::getSamplerName( size_t index )const
	{
		return file::getElementName( m_asset->samplers, index, getName() );
	}

	castor::String GltfImporterFile::getGeometryName( size_t nodeIndex, size_t meshIndex, size_t instance )const
	{
		castor::String result;
		auto nodeName = castor::String( m_asset->nodes[nodeIndex].name );
		auto meshName = castor::String( m_asset->meshes[meshIndex].name );

		if ( nodeName.empty() )
		{
			nodeName = "Node";
		}

		if ( meshName.empty() )
		{
			meshName = "Mesh";
		}

		if ( nodeName == meshName )
		{
			result += std::string( nodeName.c_str() ) + cuT( "-" ) + castor::string::toString( nodeIndex );
			result += castor::string::toString( meshIndex );
		}
		else
		{
			result += std::string( nodeName.c_str() ) + cuT( "-" ) + castor::string::toString( nodeIndex );
			result += cuT( "-" ) + std::string( meshName.c_str() ) + cuT( "-" ) + castor::string::toString( meshIndex );
		}

		if ( instance )
		{
			result += cuT( "_" ) + castor::string::toString( instance );
		}

		return result;
	}

	castor::String GltfImporterFile::getAnimationName( size_t index )const
	{
		return file::getElementName( m_asset->animations, index, getName() );
	}

	size_t GltfImporterFile::getNodeIndex( castor::String const & name )const
	{
		auto it = std::find_if( m_nodes.begin()
			, m_nodes.end()
			, [&name]( GltfNodeData const & lookup )
			{
				return lookup.name == name;
			} );
		CU_Require( it != m_nodes.end() );
		return it->index;
	}

	size_t GltfImporterFile::getSkeletonNodeIndex( castor::String const & name )const
	{
		auto it = std::find_if( m_skeletonNodes.begin()
			, m_skeletonNodes.end()
			, [&name]( GltfNodeData const & lookup )
			{
				return lookup.name == name;
			} );
		CU_Require( it != m_skeletonNodes.end() );
		return it->index;
	}

	size_t GltfImporterFile::getMeshIndex( castor::String const & name )const
	{
		size_t index{};
		auto it = std::find_if( m_asset->meshes.begin()
			, m_asset->meshes.end()
			, [this, &index, &name]( fastgltf::Mesh const & lookup )
			{
				return getMeshName( index++ ) == name;
			} );
		CU_Require( it != m_asset->meshes.end() );
		return size_t( std::distance( m_asset->meshes.begin(), it ) );
	}

	Animations GltfImporterFile::getMeshAnimations( castor3d::Mesh const & mesh )const
	{
		Animations result;
		size_t index{};

		for ( auto & animation : m_asset->animations )
		{
			for ( auto & channel : animation.channels )
			{
				if ( channel.path == fastgltf::AnimationPath::Weights
					&& m_asset->nodes[channel.nodeIndex].meshIndex
					&& getMeshName( *m_asset->nodes[channel.nodeIndex].meshIndex ) == mesh.getName() )
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
					&& file::isSkeletonNode( m_asset->skins, channel.nodeIndex )
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
		Animations result;
		size_t index{};

		for ( auto & animation : m_asset->animations )
		{
			for ( auto & channel : animation.channels )
			{
				if ( ( channel.path == fastgltf::AnimationPath::Rotation
						|| channel.path == fastgltf::AnimationPath::Scale
						|| channel.path == fastgltf::AnimationPath::Translation )
					&& !file::isSkeletonNode( m_asset->skins, channel.nodeIndex )
					&& getNodeName( channel.nodeIndex, 0u ) == node.getName() )
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

		if ( isValid() )
		{
			for ( size_t i = 0u; i < m_asset->meshes.size(); ++i )
			{
				castor::String skelName;

				for ( auto & node : m_asset->nodes )
				{
					if ( node.meshIndex
						&& node.skinIndex
						&& *node.meshIndex == i )
					{
						skelName = getSkinName( *node.skinIndex );
						break;
					}
				}

				result.emplace_back( getMeshName( i ), skelName );
			}
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
			for ( auto & sceneIndex : m_sceneIndices )
			{
				file::parseNodes( m_asset->scenes[sceneIndex].nodeIndices
					, m_asset->nodes
					, [this, &result]( fastgltf::Node const & node, size_t nodeIndex, size_t parentIndex, size_t parentInstanceCount )
					{
						if ( file::isSkeletonNode( m_asset->skins, nodeIndex ) )
						{
							return std::make_pair( parentInstanceCount, false );
						}

						auto transforms = file::listInstances( m_asset.get< 1 >(), node );

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
										m_nodes.emplace_back( parentIndex < m_asset->nodes.size() ? getNodeName( parentIndex, j + 1u ) : std::string{}
											, getNodeName( nodeIndex, index + 1u )
											, nodeIndex
											, index + 1u
											, transforms[i]
											, &node );
										result.push_back( m_nodes.back() );
										++index;
									}
								}
							}
							else
							{
								for ( size_t i = 0u; i < instanceCount; ++i )
								{
									m_nodes.emplace_back( parentIndex < m_asset->nodes.size() ? getNodeName( parentIndex, 0u ) : std::string{}
										, getNodeName( nodeIndex, i + 1u )
										, nodeIndex
										, i + 1u
										, std::move( transforms[i] )
										, &node );
									result.push_back( m_nodes.back() );
								}
							}

							parentInstanceCount *= instanceCount;
						}
						else
						{
							auto transform = convert( node.transform );

							if ( parentInstanceCount > 1u )
							{
								for ( size_t i = 0u; i < parentInstanceCount; ++i )
								{
									m_nodes.emplace_back( parentIndex < m_asset->nodes.size() ? getNodeName( parentIndex, i + 1u ) : std::string{}
										, getNodeName( nodeIndex, 0u )
										, nodeIndex
										, i + 1u
										, transform
										, &node );
									result.push_back( m_nodes.back() );
								}
							}
							else
							{
								m_nodes.emplace_back( parentIndex < m_asset->nodes.size() ? getNodeName( parentIndex, 0u ) : std::string{}
									, getNodeName( nodeIndex, 0u )
										, nodeIndex
										, 0u
										, std::move( transform )
										, &node );
								result.push_back( m_nodes.back() );
							}
						}

						return std::make_pair( parentInstanceCount, true );
					} );
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
			for ( auto & sceneIndex : m_sceneIndices )
			{
				file::parseNodes( m_asset->scenes[sceneIndex].nodeIndices
					, m_asset->nodes
					, [this, &result]( fastgltf::Node const & node, size_t nodeIndex, size_t parentIndex, size_t parentInstanceCount )
					{
						if ( node.meshIndex && *node.meshIndex < m_asset->meshes.size() )
						{
							bool processed{ false };

							for ( auto & nodeData : m_nodes )
							{
								if ( nodeData.instance != 0u
									&& nodeData.index == nodeIndex )
								{
									processed = true;
									result.emplace_back( GeometryData{ getGeometryName( nodeIndex, *node.meshIndex, nodeData.instance )
										, getNodeName( nodeIndex, nodeData.instance )
										, getMeshName( *node.meshIndex ) } );
								}
							}

							if ( !processed )
							{
								result.emplace_back( GeometryData{ getGeometryName( nodeIndex, *node.meshIndex, 0u )
									, getNodeName( nodeIndex, 0u )
									, getMeshName( *node.meshIndex ) } );
							}
						}

						return std::make_pair( parentInstanceCount, true );
					} );
			}
		}

		return result;
	}

	std::vector< castor::String > GltfImporterFile::listMeshAnimations( castor3d::Mesh const & mesh )
	{
		std::set< castor::String > result;

		if ( isValid() )
		{
			size_t index{};

			for ( auto & animation : m_asset->animations )
			{
				for ( auto & channel : animation.channels )
				{
					if ( channel.path == fastgltf::AnimationPath::Weights
						&& getMeshName( *m_asset->nodes[channel.nodeIndex].meshIndex ) == mesh.getName() )
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
						&& file::isSkeletonNode( m_asset->skins, channel.nodeIndex ) )
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
						&& !file::isSkeletonNode( m_asset->skins, channel.nodeIndex )
						&& getNodeName( channel.nodeIndex, 0u ) == node.getName() )
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

	castor3d::ImporterFileUPtr GltfImporterFile::create( castor3d::Engine & engine
		, castor3d::Scene * scene
		, castor::Path const & path
		, castor3d::Parameters const & parameters )
	{
		return castor::makeUniqueDerived< castor3d::ImporterFile, GltfImporterFile >( engine, scene, path, parameters );
	}

	//*********************************************************************************************
}
