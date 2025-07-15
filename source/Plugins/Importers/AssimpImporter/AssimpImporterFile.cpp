#include "AssimpImporter/AssimpImporterFile.hpp"

#include "AssimpImporter/AssimpAnimationImporter.hpp"
#include "AssimpImporter/AssimpCameraImporter.hpp"
#include "AssimpImporter/AssimpLightImporter.hpp"
#include "AssimpImporter/AssimpMaterialImporter.hpp"
#include "AssimpImporter/AssimpMeshImporter.hpp"
#include "AssimpImporter/AssimpSceneNodeImporter.hpp"
#include "AssimpImporter/AssimpSkeletonImporter.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Model/Mesh/Mesh.hpp>
#include <Castor3D/Model/Skeleton/Skeleton.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/SceneNode.hpp>

#include <CastorUtils/Design/ArrayView.hpp>

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <assimp/material.h>
#include <assimp/postprocess.h>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

namespace c3d_assimp
{
	//*********************************************************************************************

	namespace file
	{
		static aiScene const * loadScene( Assimp::Importer & importer
			, c3d::Path const & filePath
			, c3d::Parameters const & parameters )
		{
			bool noOptimisation = parameters.get< bool >( cuT( "no_optimisations" ) );
			bool noValidation = parameters.get< bool >( cuT( "no_validation" ) );
			uint32_t importFlags{ aiProcess_Triangulate
				| aiProcess_FixInfacingNormals
				| aiProcess_LimitBoneWeights };
			importer.SetPropertyInteger( AI_CONFIG_PP_LBW_MAX_WEIGHTS, 8 );
			importer.SetPropertyBool( AI_CONFIG_IMPORT_NO_SKELETON_MESHES, true );
			importer.SetPropertyInteger( AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, 0 ); //< Get rid of $AssimpFbx$_PreRotation nodes
			importer.SetPropertyInteger( AI_CONFIG_FBX_CONVERT_TO_M, 0 ); //< Convert FBX cm to m.

			if ( !noValidation )
			{
				importFlags |= aiProcess_ValidateDataStructure
					| aiProcess_FindInvalidData;
			}

			if ( !noOptimisation )
			{
				importFlags |= aiProcess_JoinIdenticalVertices
					| aiProcess_OptimizeMeshes
					| aiProcess_OptimizeGraph;
			}

			bool tangentSpace = false;

			if ( c3d::String normals;
				parameters.get( cuT( "normals" ), normals )
					&& normals == cuT( "smooth" ) )
			{
				importFlags |= aiProcess_GenSmoothNormals;
			}

			if ( parameters.get( cuT( "tangent_space" ), tangentSpace ) && tangentSpace )
			{
				importFlags |= aiProcess_CalcTangentSpace;
			}

			try
			{
				auto result = importer.ReadFile( c3d::toUtf8( filePath ), importFlags );

				if ( !result )
				{
					c3d::log::error << "Scene loading failed : " << importer.GetErrorString() << std::endl;
				}

				return result;
			}
			catch ( std::exception & exc )
			{
				c3d::log::error << "Scene loading failed : " << exc.what() << std::endl;
			}

			return nullptr;
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

		static c3d::String getMaterialName( AssimpImporterFile const & file
			, aiMaterial const & aiMaterial
			, uint32_t materialIndex )
		{
			c3d::String result = file.getExtension() + cuT( "-" );

			if ( aiString name;
				aiMaterial.Get( AI_MATKEY_NAME, name ) == aiReturn_SUCCESS )
			{
				result += makeString( name ) + cuT( "-" ) + c3d::string::toString( materialIndex );
			}
			else
			{
				result += file.getName() + cuT( "-" ) + c3d::string::toString( materialIndex );
			}

			return result;
		}

		static bool isSkeletonNode( c3d::String const & aiNodeName
			, c3d::StringMap< c3d::Matrix4x4f > const & bonesNodes
			, c3d::StringMap< AssimpSkeletonData > const & skeletons )
		{
			if ( bonesNodes.find( aiNodeName ) != bonesNodes.end() )
			{
				return true;
			}

			auto mbName = c3d::toUtf8( aiNodeName );
			return skeletons.end() != std::find_if( skeletons.begin()
				, skeletons.end()
				, [&mbName]( c3d::StringMap< AssimpSkeletonData >::value_type const & lookup )
				{
					return lookup.second.rootNode->FindNode( mbName.c_str() ) != nullptr;
				} );
		}

		static c3d::Map< aiAnimation const *, aiNodeAnim const * > findNodeAnims( aiNode const & aiNode
			, c3d::ArrayView< aiAnimation * > const & animations )
		{
			c3d::Map< aiAnimation const *, aiNodeAnim const * > result;

			for ( auto aiAnimation : animations )
			{
				auto channels = c3d::makeArrayView( aiAnimation->mChannels, aiAnimation->mNumChannels );
				auto it = std::find_if( channels.begin()
					, channels.end()
					, [&aiNode]( aiNodeAnim const * lookup )
					{
						return lookup->mNodeName == aiNode.mName;
					} );

				if ( it != channels.end() )
				{
					result.emplace( aiAnimation, *it );
				}
			}

			return result;
		}

		static bool isAnimForSkeleton( aiAnimation const & animation
			, AssimpSkeletonData const & skeleton )
		{
			auto channels = c3d::makeArrayView( animation.mChannels, animation.mNumChannels );
			return // The skeleton root node is in the animated channels
				( channels.end() != std::find_if( channels.begin()
					, channels.end()
					, [&skeleton]( aiNodeAnim const * animLookup )
					{
						return skeleton.rootNode->FindNode( animLookup->mNodeName ) != nullptr;
					} ) );
		}

		static c3d::Pair< AssimpSkeletonData *, c3d::SkeletonRPtr > findSkeletonForAnim( c3d::Scene * scene
			, aiNode const & rootNode
			, aiAnimation const & animation
			, AssimpSceneData & sceneData )
		{
			for ( auto & [_, skeleton] : sceneData.skeletons )
			{
				if ( isAnimForSkeleton( animation, skeleton ) )
				{
					return { &skeleton, nullptr };
				}
			}

			if ( scene )
			{
				for ( auto const & [name, skeleton] : scene->getSkeletonCache() )
				{
					if ( auto node = rootNode.FindNode( c3d::toUtf8( skeleton->getRootNode()->getName() ).c_str() ) )
					{
						auto & data = sceneData.skeletons.try_emplace( name, node ).first->second;

						if ( isAnimForSkeleton( animation, data ) )
						{
							return { &data, skeleton.get() };
						}
					}
				}
			}

			return { nullptr, nullptr };
		}

		static c3d::StringMap< aiMeshMorphAnim const * > findMorphAnims( uint32_t aiMeshIndex
			, uint32_t aiNumAnimMeshes
			, aiNode const & rootNode
			, c3d::ArrayView< aiAnimation * > animations )
		{
			c3d::StringMap< aiMeshMorphAnim const * > result;

			for ( auto anim : animations )
			{
				auto morphChannels = c3d::makeArrayView( anim->mMorphMeshChannels, anim->mNumMorphMeshChannels );
				auto morphIt = std::find_if( morphChannels.begin()
					, morphChannels.end()
					, [aiNumAnimMeshes, aiMeshIndex, &rootNode]( aiMeshMorphAnim const * morphChannel )
					{
						auto node = rootNode.FindNode( morphChannel->mName );
						bool res = node != nullptr
							&& morphChannel->mNumKeys > 0u
							&& morphChannel->mKeys->mNumValuesAndWeights == aiNumAnimMeshes
							&& morphChannel->mKeys[morphChannel->mNumKeys - 1u].mTime > 0.0;

						if ( res )
						{
							auto meshes = c3d::makeArrayView( node->mMeshes, node->mNumMeshes );
							res = meshes.end() != std::find( meshes.begin()
								, meshes.end()
								, aiMeshIndex );
						}

						return res;
					} );

				if ( morphIt != morphChannels.end() )
				{
					result.try_emplace( makeString( anim->mName ), *morphIt );
				}
			}

			return result;
		}

		static auto findNodeMesh( uint32_t meshIndex
			, c3d::StringMap< AssimpMeshData > const & meshes )
		{
			return std::find_if( meshes.begin()
				, meshes.end()
				, [&meshIndex]( c3d::StringMap< AssimpMeshData >::value_type const & lookup )
				{
					return lookup.second.submeshes.end() != std::find_if( lookup.second.submeshes.begin()
						, lookup.second.submeshes.end()
						, [&meshIndex]( AssimpSubmeshData const & submesh )
						{
							return submesh.meshIndex == meshIndex;
						} );
				} );
		}

		static bool hasNodeAnim( aiScene const & scene
			, uint32_t meshIndex )
		{
			auto node = findMeshNode( meshIndex, *scene.mRootNode );
			CU_Require( node );
			return node
				&& !findNodeAnims( *node
					, c3d::makeArrayView( scene.mAnimations, scene.mNumAnimations ) ).empty();
		}

		static bool isValidMesh( aiScene const & scene
			, uint32_t meshIndex )
		{
			return meshIndex < scene.mNumMeshes
				&& c3d_assimp::isValidMesh( *scene.mMeshes[meshIndex] );
		}

		static c3d::String reworkMeshName( c3d::String const & name
			, uint32_t meshIndex )
		{
			c3d::StringView separators = cuT( " \t\r_$|/:\\*!?&#\"()[]{}@+." );
			auto split = c3d::string::split( name, separators, ~0u, false );
			c3d::Set< int > numbers;
			c3d::Set< c3d::String > names;

			for ( auto s : split )
			{
				c3d::string::trim( s, true, true, separators );

				if ( !s.empty() )
				{
					if ( c3d::string::isInteger( s ) )
					{
						numbers.emplace( c3d::string::toInt( s ) );
					}
					else
					{
						names.insert( s );
					}
				}
			}

			c3d::String result;
			c3d::String sep;

			for ( auto & s : names )
			{
				result += sep + s;
				sep = cuT( "_" );
			}

			for ( auto i : numbers )
			{
				result += sep + c3d::string::toString( i );
				sep = cuT( "_" );
			}

			if ( result.empty() )
			{
				result = c3d::string::toString( meshIndex );
			}

			return result;
		}

		static void accumulateTransformsRec( aiNode const * node
			, c3d::Vector< AssimpNodeData > const & nodes
			, c3d::Vector< c3d::Matrix4x4f > & transforms )
		{
			if ( !node )
			{
				return;
			}

			auto it = std::find_if( nodes.begin()
				, nodes.end()
				, [&node]( AssimpNodeData const & lookup )
				{
					return node == lookup.node;
				} );

			if ( it == nodes.end() )
			{
				transforms.push_back( fromAssimp( node->mTransformation ) );
				accumulateTransformsRec( node->mParent, nodes, transforms );
			}
			else
			{
				c3d::Matrix4x4f matrix;
				c3d::matrix::setTransform( matrix, it->translate, it->scale, it->rotate );
				transforms.push_back( matrix );
			}
		}

		static c3d::Matrix4x4f accumulateTransforms( AssimpImporterFile const & file
			, c3d::String const & name
			, aiNode const & rootNode
			, c3d::Vector< AssimpNodeData > const & nodes
			, c3d::Matrix4x4f transform )
		{
			if ( auto node = rootNode.FindNode( c3d::toUtf8( file.getExternalName( name ) ).c_str() ) )
			{
				c3d::Vector< c3d::Matrix4x4f > transforms;
				accumulateTransformsRec( node->mParent, nodes, transforms );
				std::reverse( transforms.begin(), transforms.end() );
				c3d::Matrix4x4f cumulative{ 1.0f };

				for ( auto const & t : transforms )
				{
					cumulative *= t;
				}

				transform = cumulative * transform;
			}

			return transform;
		}
	}

	//*********************************************************************************************

	c3d::MbString const AssimpImporterFile::Name = "ASSIMP Importer";

	AssimpImporterFile::AssimpImporterFile( c3d::Engine & engine
		, c3d::Scene * scene
		, c3d::Path const & path
		, c3d::Parameters const & parameters
		, c3d::ProgressBar * progress )
		: c3d::ImporterFile{ engine, scene, path, parameters, progress }
		, m_aiScene{ file::loadScene( m_importer, getFileName(), getParameters() ) }
	{
		if ( m_aiScene )
		{
			for ( auto aiMesh : c3d::makeArrayView( m_aiScene->mMeshes, m_aiScene->mNumMeshes ) )
			{
				for ( auto aiBone : c3d::makeArrayView( aiMesh->mBones, aiMesh->mNumBones ) )
				{
					m_bonesNodes.try_emplace( makeString( aiBone->mName )
						, fromAssimp( aiBone->mOffsetMatrix ) );
				}
			}

			doPrelistMaterials();
			doPrelistMeshes( doPrelistSkeletons() );
			c3d::Map< AssimpMeshData const *, aiNodeArray > processed;
			c3d::Map< aiNode const *, c3d::Matrix4x4f > cumulativeTransforms;
			doPrelistSceneNodes( *m_aiScene->mRootNode, processed, cumulativeTransforms );
			doPrelistLights();
			doPrelistCameras();
		}
	}

	c3d::String AssimpImporterFile::getMaterialName( uint32_t materialIndex )const
	{
		return file::getMaterialName( *this
			, *m_aiScene->mMaterials[materialIndex]
			, materialIndex );
	}

	NodeAnimations const & AssimpImporterFile::getNodesAnimations( c3d::SceneNode const & node )const
	{
		if ( auto it = std::find_if( m_sceneData.nodes.begin()
			, m_sceneData.nodes.end()
			, [&node]( AssimpNodeData const & lookup )
			{
				return node.getName() == lookup.name;
			} );
			it != m_sceneData.nodes.end() )
		{
			return it->anims;
		}

		static NodeAnimations const dummy;
		return dummy;
	}

	SkeletonAnimations const & AssimpImporterFile::getSkeletonsAnimations( c3d::Skeleton const & skeleton )const
	{
		auto name = skeleton.getName();

		if ( getListedMeshes().empty()
			&& !getSkeletons().empty() )
		{
			name = getSkeletons().begin()->first;
		}

		if ( auto it = m_sceneData.skeletons.find( name );
			it != m_sceneData.skeletons.end() )
		{
			return it->second.anims;
		}

		static SkeletonAnimations const dummy;
		return dummy;
	}

	MeshAnimations const & AssimpImporterFile::getMeshesAnimations( c3d::Mesh const & mesh
		, uint32_t submeshIndex )const
	{
		if ( auto it = m_sceneData.meshes.find( mesh.getName() );
			it != m_sceneData.meshes.end()
				&& submeshIndex < it->second.submeshes.size() )
		{
			return it->second.submeshes[submeshIndex].anims;
		}

		static MeshAnimations const dummy;
		return dummy;
	}

	c3d::StringArray AssimpImporterFile::listMaterials()
	{
		c3d::StringArray result;

		for ( auto const & [name, _] : m_sceneData.materials )
		{
			result.emplace_back( name );
		}

		return result;
	}

	c3d::Vector< c3d::ImporterFile::MeshData > AssimpImporterFile::listMeshes()
	{
		m_listedMeshes.clear();
		c3d::Vector< MeshData > result;

		for ( auto const & [name, meshData] : m_sceneData.meshes )
		{
			m_listedMeshes.emplace_back( name );
			result.emplace_back( name
				, ( meshData.skelNode
					? getInternalName( findSkeletonName( m_bonesNodes, *meshData.skelNode ) )
					: c3d::String{} ) );
		}

		return result;
	}

	c3d::StringArray AssimpImporterFile::listSkeletons()
	{
		c3d::StringArray result;

		for ( auto const & [name, _] : m_sceneData.skeletons )
		{
			m_listedSkeletons.emplace_back( name );
			result.emplace_back( name );
		}

		return result;
	}

	c3d::Vector< c3d::ImporterFile::NodeData > AssimpImporterFile::listSceneNodes()
	{
		c3d::Vector< NodeData > result;

		for ( auto const & node : m_sceneData.nodes )
		{
			result.emplace_back( node.parent, node.name, node.isCamera );
		}

		return result;
	}

	c3d::Vector< c3d::ImporterFile::LightData > AssimpImporterFile::listLights()
	{
		c3d::Vector< LightData > result;

		for ( auto const & [name, lightData] : m_sceneData.lights )
		{
			result.emplace_back( name
				, ( lightData->mType == aiLightSource_DIRECTIONAL
					? c3d::LightType::eDirectional
					: ( lightData->mType == aiLightSource_POINT
						? c3d::LightType::ePoint
						: c3d::LightType::eSpot ) ) );
		}

		return result;
	}

	c3d::Vector< c3d::ImporterFile::LightGroupData > AssimpImporterFile::listLightGroups()
	{
		return c3d::Vector< c3d::ImporterFile::LightGroupData >{};
	}

	c3d::Vector< c3d::ImporterFile::GeometryData > AssimpImporterFile::listGeometries()
	{
		c3d::Vector< GeometryData > result;

		for ( auto & node : m_sceneData.nodes )
		{
			for ( auto & mesh : node.meshes )
			{
				auto it = std::find_if( m_sceneData.meshes.begin()
					, m_sceneData.meshes.end()
					, [mesh]( c3d::StringMap< AssimpMeshData >::value_type const & lookup )
					{
						return mesh == &lookup.second;
					} );
				CU_Require( it != m_sceneData.meshes.end() );
				auto meshName = getInternalName( it->first );
				auto name = node.name == meshName
					? node.name
					: node.name + meshName;
				result.emplace_back( name, node.name, it->first );
			}
		}

		return result;
	}

	c3d::Vector< c3d::ImporterFile::CameraData > AssimpImporterFile::listCameras()
	{
		c3d::Vector< CameraData > result;

		for ( auto const & [name, cameraData] : m_sceneData.cameras )
		{
			result.emplace_back( name
				, ( cameraData->mOrthographicWidth != 0.0f
					? c3d::ViewportType::eOrtho
					: c3d::ViewportType::ePerspective ) );
		}

		return result;
	}

	c3d::StringArray AssimpImporterFile::listMeshAnimations( c3d::Mesh const & mesh )
	{
		c3d::Set< c3d::String > result;

		if ( auto it = m_sceneData.meshes.find( mesh.getName() );
			it != m_sceneData.meshes.end() )
		{
			for ( auto const & submesh : it->second.submeshes )
			{
				for ( auto & [name, _] : submesh.anims )
				{
					result.insert( name );
				}
			}
		}

		return c3d::StringArray{ result.begin()
			, result.end() };
	}

	c3d::StringArray AssimpImporterFile::listSkeletonAnimations( c3d::Skeleton const & skeleton )
	{
		auto name = skeleton.getName();

		if ( getListedMeshes().empty()
			&& !getSkeletons().empty() )
		{
			name = getSkeletons().begin()->first;
		}

		c3d::StringArray result;

		if ( auto it = m_sceneData.skeletons.find( name );
			it != m_sceneData.skeletons.end() )
		{
			for ( auto const & [animName, _] : it->second.anims )
			{
				result.push_back( animName );
			}
		}

		return result;
	}

	c3d::StringArray AssimpImporterFile::listSceneNodeAnimations( c3d::SceneNode const & node )
	{
		c3d::StringArray result;

		if ( auto it = std::find_if( m_sceneData.nodes.begin()
			, m_sceneData.nodes.end()
			, [&node]( AssimpNodeData const & lookup )
			{
				return node.getName() == lookup.name;
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

	c3d::Vector< uint32_t > AssimpImporterFile::listTextureAnimations( c3d::Material const & material
		, uint32_t pass )
	{
		c3d::Vector< uint32_t > result;
		return result;
	}

	uint32_t AssimpImporterFile::countAllMeshAnimations()const
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

	uint32_t AssimpImporterFile::countAllSkeletonAnimations()const
	{
		uint32_t result{};

		for ( auto & [_, skeleton] : m_sceneData.skeletons )
		{
			result += uint32_t( skeleton.anims.size() );
		}

		return result;
	}

	uint32_t AssimpImporterFile::countAllSceneNodeAnimations()const
	{
		uint32_t result{};

		for ( auto & node : m_sceneData.nodes )
		{
			result += uint32_t( node.anims.size() );
		}

		return result;
	}

	uint32_t AssimpImporterFile::countAllTextureAnimations()const
	{
		return 0u;
	}

	c3d::MaterialImporterUPtr AssimpImporterFile::createMaterialImporter()
	{
		return c3d::makeUniqueDerived< c3d::MaterialImporter, AssimpMaterialImporter >( *getOwner() );
	}

	c3d::AnimationImporterUPtr AssimpImporterFile::createAnimationImporter()
	{
		return c3d::makeUniqueDerived< c3d::AnimationImporter, AssimpAnimationImporter >( *getOwner() );
	}

	c3d::SkeletonImporterUPtr AssimpImporterFile::createSkeletonImporter()
	{
		return c3d::makeUniqueDerived< c3d::SkeletonImporter, AssimpSkeletonImporter >( *getOwner() );
	}

	c3d::MeshImporterUPtr AssimpImporterFile::createMeshImporter()
	{
		return c3d::makeUniqueDerived< c3d::MeshImporter, AssimpMeshImporter >( *getOwner() );
	}

	c3d::SceneNodeImporterUPtr AssimpImporterFile::createSceneNodeImporter()
	{
		return c3d::makeUniqueDerived< c3d::SceneNodeImporter, AssimpSceneNodeImporter >( *getOwner() );
	}

	c3d::LightImporterUPtr AssimpImporterFile::createLightImporter()
	{
		return c3d::makeUniqueDerived< c3d::LightImporter, AssimpLightImporter >( *getOwner() );
	}

	c3d::CameraImporterUPtr AssimpImporterFile::createCameraImporter()
	{
		return c3d::makeUniqueDerived< c3d::CameraImporter, AssimpCameraImporter >( *getOwner() );
	}

	c3d::ImporterFileUPtr AssimpImporterFile::create( c3d::Engine & engine
		, c3d::Scene * scene
		, c3d::Path const & path
		, c3d::Parameters const & parameters
		, c3d::ProgressBar * progress )
	{
		return c3d::makeUniqueDerived< c3d::ImporterFile, AssimpImporterFile >( engine, scene, path, parameters, progress );
	}

	void AssimpImporterFile::doPrelistMaterials()
	{
		uint32_t materialIndex = 0u;

		for ( auto aiMaterial : c3d::makeArrayView( m_aiScene->mMaterials, m_aiScene->mNumMaterials ) )
		{
			auto name = file::getMaterialName( *this, *aiMaterial, materialIndex );
			m_sceneData.materials.try_emplace( name, aiMaterial );
			++materialIndex;
		}
	}

	c3d::Map< aiMesh const *, aiNode const * > AssimpImporterFile::doPrelistSkeletons()
	{
		c3d::Map< aiMesh const *, aiNode const * > result;
		uint32_t meshIndex = 0u;

		for ( auto aiMesh : c3d::makeArrayView( m_aiScene->mMeshes, m_aiScene->mNumMeshes ) )
		{
			if ( aiMesh->HasBones() )
			{
				auto meshNode = findMeshNode( meshIndex, *m_aiScene->mRootNode );

				if ( meshNode == nullptr )
				{
					CU_Failure( "Could not find mesh' node ?" );
				}
				else
				{
					auto rootNode = findRootSkeletonNode( *m_aiScene->mRootNode
						, c3d::makeArrayView( aiMesh->mBones, aiMesh->mNumBones )
						, meshNode );
					auto skelName = getInternalName( findSkeletonName( m_bonesNodes
						, *rootNode ) );
					m_sceneData.skeletons.try_emplace( skelName, rootNode );
					result.emplace( aiMesh, rootNode );
				}
			}

			++meshIndex;
		}

		for ( auto aiAnimation : c3d::makeArrayView( m_aiScene->mAnimations, m_aiScene->mNumAnimations ) )
		{
			if ( auto [skeletonData, skeleton] = file::findSkeletonForAnim( getScene(), *m_aiScene->mRootNode, *aiAnimation, m_sceneData );
				skeletonData )
			{
				auto [frameCount, minFrameTicks, maxFrameTicks] = getAnimationFrameTicks( *aiAnimation );

				if ( frameCount > 1 )
				{
					c3d::String animName{ normalizeName( makeString( aiAnimation->mName ) ) };

					if ( animName.empty() )
					{
						animName = normalizeName( getName() );
					}

					if ( skeleton && skeleton->hasAnimation( animName ) )
					{
						animName += cuT( "_" ) + getName();
					}

					skeletonData->anims.try_emplace( animName, aiAnimation );
				}
			}
		}

		return result;
	}

	void AssimpImporterFile::doPrelistMeshes( c3d::Map< aiMesh const *, aiNode const * > const & meshSkeletons )
	{
		uint32_t meshIndex = 0u;

		for ( auto aiMesh : c3d::makeArrayView( m_aiScene->mMeshes, m_aiScene->mNumMeshes ) )
		{
			if ( isValidMesh( *aiMesh ) )
			{
				auto meshName = normalizeName( getInternalName( file::reworkMeshName( makeString( aiMesh->mName ), meshIndex ) ) );

				if ( meshName.size() > 150u )
				{
					meshName = getInternalName( getName() ) + c3d::string::toString( meshIndex );
				}
				
				if ( file::hasNodeAnim( *m_aiScene, meshIndex ) )
				{
					meshName += c3d::string::toString( meshIndex );
				}

				auto regIt = m_sceneData.meshes.find( meshName );
				aiNode const * skelNode{};

				if ( regIt != m_sceneData.meshes.end() )
				{
					meshName += c3d::string::toString( meshIndex );
					regIt = m_sceneData.meshes.find( meshName );
				}

				if ( regIt == m_sceneData.meshes.end() )
				{
					// Merge meshes that use the same skeleton
					auto it = meshSkeletons.find( aiMesh );

					if ( it != meshSkeletons.end() )
					{
						skelNode = it->second;
						regIt = std::find_if( m_sceneData.meshes.begin()
							, m_sceneData.meshes.end()
							, [&skelNode]( c3d::StringMap< AssimpMeshData >::value_type const & lookup )
							{
								return skelNode == lookup.second.skelNode;
							} );

						if ( regIt != m_sceneData.meshes.end() )
						{
							regIt = file::replaceIter( meshName, regIt, m_sceneData.meshes ).first;
						}
					}
				}

				if ( regIt == m_sceneData.meshes.end() )
				{
					regIt = m_sceneData.meshes.try_emplace( meshName, skelNode ).first;
				}

				auto & submeshData = regIt->second.submeshes.emplace_back( aiMesh, meshIndex );
				m_meshes.insert( meshIndex );

				if ( aiMesh->mNumAnimMeshes )
				{
					auto anims = file::findMorphAnims( meshIndex
						, aiMesh->mNumAnimMeshes
						, *m_aiScene->mRootNode
						, c3d::makeArrayView( m_aiScene->mAnimations, m_aiScene->mNumAnimations ) );

					for ( auto const & [name, animData] : anims )
					{
						c3d::String animName{ normalizeName( name ) };
						submeshData.anims.try_emplace( animName, aiMesh, animData );
					}
				}
			}

			++meshIndex;
		}
	}

	void AssimpImporterFile::doPrelistSceneNodes( aiNode const & node
		, c3d::Map< AssimpMeshData const *, aiNodeArray > & processedMeshes
		, c3d::Map< aiNode const *, c3d::Matrix4x4f > & cumulativeTransforms
		, c3d::String parentName
		, c3d::Matrix4x4f transform )
	{
		auto aiNodeName = makeString( node.mName );

		if ( m_bonesNodes.find( aiNodeName ) != m_bonesNodes.end() )
		{
			return;
		}

		aiVector3D translate;
		aiVector3D scale;
		aiQuaternion rotate;
		node.mTransformation.Decompose( scale, rotate, translate );
		transform *= fromAssimp( node.mTransformation );
		cumulativeTransforms.try_emplace( &node, transform );
		bool isSkeletonNode = file::isSkeletonNode( aiNodeName, m_bonesNodes, m_sceneData.skeletons );
		auto nodeName = getInternalName( aiNodeName );
		AssimpNodeData nodeData{ parentName
			, nodeName
			, false
			, &node
			, fromAssimp( translate )
			, fromAssimp( rotate )
			, fromAssimp( scale ) };

		if ( !isSkeletonNode )
		{
			auto anims = file::findNodeAnims( node
				, c3d::makeArrayView( m_aiScene->mAnimations, m_aiScene->mNumAnimations ) );

			for ( auto anim : anims )
			{
				auto [frameCount, minFrameTicks, maxFrameTicks] = getNodeAnimFrameTicks( *anim.second );

				if ( frameCount > 1 )
				{
					c3d::String animName{ normalizeName( makeString( anim.first->mName ) ) };

					if ( animName.empty() )
					{
						animName = normalizeName( aiNodeName );
					}

					nodeData.anims.try_emplace( animName, anim );
				}
			}
		}

		for ( auto meshIndex : c3d::makeArrayView( node.mMeshes, node.mNumMeshes ) )
		{
			if ( !file::isValidMesh( *m_aiScene, meshIndex ) )
			{
				continue;
			}

			auto it = file::findNodeMesh( meshIndex, m_sceneData.meshes );

			if ( it != m_sceneData.meshes.end() )
			{
				if ( nodeData.meshes.end() == std::find( nodeData.meshes.begin()
					, nodeData.meshes.end()
					, &it->second ) )
				{
					// Don't add the mesh if it has already been added to a node with the same transform.
					auto & nodeArray = processedMeshes.try_emplace( &it->second ).first->second;
					auto nodeIt = std::find_if( nodeArray.begin()
						, nodeArray.end()
						, [&cumulativeTransforms, &transform]( aiNode const * lookup )
						{
							auto lookupIt = cumulativeTransforms.find( lookup );
							return lookupIt->second == transform;
						} );

					if ( nodeIt == nodeArray.end() )
					{
						nodeArray.push_back( &node );
						nodeData.meshes.push_back( &it->second );
					}
				}
			}
			else
			{
				CU_Failure( "Could not find node's mesh ?" );
			}
		}

		m_sceneData.nodes.emplace_back( c3d::move( nodeData ) );
		parentName = nodeName;

		// continue for all child nodes
		for ( auto aiChild : c3d::makeArrayView( node.mChildren, node.mNumChildren ) )
		{
			doPrelistSceneNodes( *aiChild
				, processedMeshes
				, cumulativeTransforms
				, parentName
				, transform );
		}
	}

	void AssimpImporterFile::doPrelistLights()
	{
		for ( auto aiLight : c3d::makeArrayView( m_aiScene->mLights, m_aiScene->mNumLights ) )
		{
			if ( aiLight->mType == aiLightSource_DIRECTIONAL
				|| aiLight->mType == aiLightSource_POINT
				|| aiLight->mType == aiLightSource_SPOT )
			{
				c3d::String name = getInternalName( aiLight->mName );
				m_sceneData.lights.try_emplace( name, aiLight );

				auto position = c3d::Point3f{};
				auto orientation = c3d::Quaternion::identity();

				if ( aiLight->mType == aiLightSource_DIRECTIONAL
					|| aiLight->mType == aiLightSource_SPOT )
				{
					auto direction = c3d::point::getNormalised( fromAssimp( aiLight->mDirection ) );
					auto up = c3d::point::getNormalised( fromAssimp( aiLight->mUp ) );
					orientation = c3d::Quaternion::fromMatrix( fromAssimp( direction, up ) );
				}

				if ( aiLight->mType != aiLightSource_DIRECTIONAL )
				{
					position = fromAssimp( aiLight->mPosition );
				}

				auto transform = c3d::Matrix4x4f{ 1.0f };
				c3d::matrix::setTransform( transform
					, position
					, c3d::Point3f{ 1.0, 1.0, 1.0 }
					, orientation );
				auto it = std::find_if( m_sceneData.nodes.begin()
					, m_sceneData.nodes.end()
					, [&name]( AssimpNodeData const & lookup )
					{
						return lookup.name == name;
					} );

				if ( it == m_sceneData.nodes.end() )
				{
					file::accumulateTransforms( *this
						, name
						, *m_aiScene->mRootNode
						, m_sceneData.nodes
						, transform );
					c3d::Point3f translate;
					c3d::Point3f scale;
					c3d::Quaternion rotate;
					c3d::matrix::decompose( transform, translate, scale, rotate );
					m_sceneData.nodes.emplace_back( c3d::String{}
						, name
						, false
						, nullptr
						, translate
						, rotate
						, scale );
				}
				else
				{
					c3d::Matrix4x4f matrix;
					c3d::matrix::setTransform( matrix, it->translate, it->scale, it->rotate );
					matrix *= transform;
					c3d::matrix::decompose( matrix, it->translate, it->scale, it->rotate );
				}
			}
		}
	}

	void AssimpImporterFile::doPrelistCameras()
	{
		for ( auto aiCamera : c3d::makeArrayView( m_aiScene->mCameras, m_aiScene->mNumCameras ) )
		{
			c3d::String name = getInternalName( aiCamera->mName );
			m_sceneData.cameras.try_emplace( name, aiCamera );

			auto position = c3d::Point3f{};
			auto orientation = c3d::Quaternion::identity();
			auto direction = c3d::point::getNormalised( fromAssimp( aiCamera->mLookAt ) );
			auto up = c3d::point::getNormalised( fromAssimp( aiCamera->mUp ) );
			position = fromAssimp( aiCamera->mPosition );
			orientation = c3d::Quaternion::fromMatrix( fromAssimp( direction, up ) );

			auto transform = c3d::Matrix4x4f{ 1.0f };
			c3d::matrix::setTransform( transform
				, position
				, c3d::Point3f{ 1.0, 1.0, 1.0 }
				, orientation );
			auto it = std::find_if( m_sceneData.nodes.begin()
				, m_sceneData.nodes.end()
				, [&name]( AssimpNodeData const & lookup )
				{
					return lookup.name == name;
				} );

			if ( it == m_sceneData.nodes.end() )
			{
				file::accumulateTransforms( *this
					, name
					, *m_aiScene->mRootNode
					, m_sceneData.nodes
					, transform );
				c3d::Point3f translate;
				c3d::Point3f scale;
				c3d::Quaternion rotate;
				c3d::matrix::decompose( transform, translate, scale, rotate );
				m_sceneData.nodes.emplace_back( c3d::String{}
					, name
					, false
					, nullptr
					, translate
					, rotate
					, scale );
			}
			else
			{
				c3d::Matrix4x4f matrix;
				c3d::matrix::setTransform( matrix, it->translate, it->scale, it->rotate );
				matrix *= transform;
				c3d::matrix::decompose( matrix, it->translate, it->scale, it->rotate );
			}
		}
	}

	//*********************************************************************************************
}
