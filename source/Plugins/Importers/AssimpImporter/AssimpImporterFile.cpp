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
			, castor::Path const & filePath
			, castor3d::Parameters const & parameters )
		{
			bool noOptim = false;
			auto found = parameters.get( cuT( "no_optimisations" ), noOptim );
			uint32_t importFlags{ aiProcess_ValidateDataStructure
				| aiProcess_FindInvalidData
				| aiProcess_Triangulate
				| aiProcess_FixInfacingNormals
				| aiProcess_LimitBoneWeights };
			importer.SetPropertyInteger( AI_CONFIG_PP_LBW_MAX_WEIGHTS, 8 );
			importer.SetPropertyBool( AI_CONFIG_IMPORT_NO_SKELETON_MESHES, true );
			importer.SetPropertyInteger( AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, 0 ); //< Get rid of $AssimpFbx$_PreRotation nodes
			importer.SetPropertyInteger( AI_CONFIG_FBX_CONVERT_TO_M, 0 ); //< Convert FBX cm to m.

			if ( !found || !noOptim )
			{
				importFlags |= aiProcess_JoinIdenticalVertices
					| aiProcess_OptimizeMeshes
					| aiProcess_OptimizeGraph;
			}

			bool tangentSpace = false;
			castor::String normals;

			if ( parameters.get( cuT( "normals" ), normals ) )
			{
				if ( normals == cuT( "smooth" ) )
				{
					importFlags |= aiProcess_GenSmoothNormals;
				}
			}

			if ( parameters.get( cuT( "tangent_space" ), tangentSpace ) && tangentSpace )
			{
				importFlags |= aiProcess_CalcTangentSpace;
			}

			try
			{
				auto result = importer.ReadFile( castor::toUtf8( filePath ), importFlags );

				if ( !result )
				{
					castor3d::log::error << "Scene loading failed : " << importer.GetErrorString() << std::endl;
				}

				return result;
			}
			catch ( std::exception & exc )
			{
				castor3d::log::error << "Scene loading failed : " << exc.what() << std::endl;
			}

			return nullptr;
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

		static castor::String getMaterialName( AssimpImporterFile const & file
			, aiMaterial const & aiMaterial
			, uint32_t materialIndex )
		{
			castor::String result = file.getExtension() + cuT( "-" );
			aiString name;

			if ( aiMaterial.Get( AI_MATKEY_NAME, name ) == aiReturn_SUCCESS )
			{
				result += makeString( name ) + cuT( "-" ) + castor::string::toString( materialIndex );
			}
			else
			{
				result += file.getName() + cuT( "-" ) + castor::string::toString( materialIndex );
			}

			return result;
		}

		static bool isSkeletonNode( aiNode const & node
			, castor::String const & aiNodeName
			, castor::StringMap< castor::Matrix4x4f > const & bonesNodes
			, castor::StringMap< AssimpSkeletonData > const & skeletons )
		{
			if ( bonesNodes.find( aiNodeName ) != bonesNodes.end() )
			{
				return true;
			}

			auto mbName = castor::toUtf8( aiNodeName );
			return skeletons.end() != std::find_if( skeletons.begin()
				, skeletons.end()
				, [&mbName]( castor::StringMap< AssimpSkeletonData >::value_type const & lookup )
				{
					return lookup.second.rootNode->FindNode( mbName.c_str() ) != nullptr;
				} );
		}

		static castor::Map< aiAnimation const *, aiNodeAnim const * > findNodeAnims( aiNode const & aiNode
			, castor::ArrayView< aiAnimation * > const & animations )
		{
			castor::Map< aiAnimation const *, aiNodeAnim const * > result;

			for ( auto aiAnimation : animations )
			{
				auto channels = castor::makeArrayView( aiAnimation->mChannels, aiAnimation->mNumChannels );
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
			auto channels = castor::makeArrayView( animation.mChannels, animation.mNumChannels );
			return // The skeleton root node is in the animated channels
				( channels.end() != std::find_if( channels.begin()
					, channels.end()
					, [&skeleton]( aiNodeAnim const * animLookup )
					{
						return skeleton.rootNode->FindNode( animLookup->mNodeName ) != nullptr;
					} ) );
		}

		static castor::Pair< AssimpSkeletonData *, castor3d::SkeletonRPtr > findSkeletonForAnim( castor3d::Scene * scene
			, aiNode const & rootNode
			, aiAnimation const & animation
			, AssimpSceneData & sceneData )
		{
			for ( auto & skeleton : sceneData.skeletons )
			{
				if ( isAnimForSkeleton( animation, skeleton.second ) )
				{
					return { &skeleton.second, nullptr };
				}
			}

			if ( scene )
			{
				for ( auto & skeleton : scene->getSkeletonCache() )
				{
					if ( auto node = rootNode.FindNode( castor::toUtf8( skeleton.second->getRootNode()->getName() ).c_str() ) )
					{
						auto & data = sceneData.skeletons.emplace( skeleton.first
							, AssimpSkeletonData{ node } ).first->second;

						if ( isAnimForSkeleton( animation, data ) )
						{
							return { &data, skeleton.second.get() };
						}
					}
				}
			}

			return { nullptr, nullptr };
		}

		static castor::StringMap< aiMeshMorphAnim const * > findMorphAnims( uint32_t aiMeshIndex
			, uint32_t aiNumAnimMeshes
			, aiNode const & rootNode
			, castor::ArrayView< aiAnimation * > animations )
		{
			castor::StringMap< aiMeshMorphAnim const * > result;

			for ( auto anim : animations )
			{
				auto morphChannels = castor::makeArrayView( anim->mMorphMeshChannels, anim->mNumMorphMeshChannels );
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
							auto meshes = castor::makeArrayView( node->mMeshes, node->mNumMeshes );
							res = meshes.end() != std::find( meshes.begin()
								, meshes.end()
								, aiMeshIndex );
						}

						return res;
					} );

				if ( morphIt != morphChannels.end() )
				{
					result.emplace( makeString( anim->mName ), *morphIt );
				}
			}

			return result;
		}

		static auto findNodeMesh( uint32_t meshIndex
			, castor::StringMap< AssimpMeshData > const & meshes )
		{
			return std::find_if( meshes.begin()
				, meshes.end()
				, [&meshIndex]( castor::StringMap< AssimpMeshData >::value_type const & lookup )
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
			return !findNodeAnims( *node
				, castor::makeArrayView( scene.mAnimations, scene.mNumAnimations ) ).empty();
		}

		static bool isValidMesh( aiScene const & scene
			, uint32_t meshIndex )
		{
			return meshIndex < scene.mNumMeshes
				&& c3d_assimp::isValidMesh( *scene.mMeshes[meshIndex] );
		}

		static castor::String reworkMeshName( castor::String const & name
			, uint32_t meshIndex )
		{
			castor::StringView separators = cuT( " \t\r_$|/:\\*!?&#\"()[]{}@+." );
			auto split = castor::string::split( name, separators, ~( 0u ), false );
			castor::Set< int > numbers;
			castor::Set< castor::String > names;

			for ( auto s : split )
			{
				castor::string::trim( s, true, true, separators );

				if ( !s.empty() )
				{
					if ( castor::string::isInteger( s ) )
					{
						numbers.emplace( castor::string::toInt( s ) );
					}
					else
					{
						names.insert( s );
					}
				}
			}

			castor::String result;
			castor::String sep;

			for ( auto & s : names )
			{
				result += sep + s;
				sep = cuT( "_" );
			}

			for ( auto i : numbers )
			{
				result += sep + castor::string::toString( i );
				sep = cuT( "_" );
			}

			return result;
		}

		static void accumulateTransformsRec( aiNode const * node
			, castor::Vector< AssimpNodeData > const & nodes
			, castor::Vector< castor::Matrix4x4f > & transforms )
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
				castor::Matrix4x4f matrix;
				castor::matrix::setTransform( matrix, it->translate, it->scale, it->rotate );
				transforms.push_back( matrix );
			}
		}

		static castor::Matrix4x4f accumulateTransforms( AssimpImporterFile const & file
			, castor::String const & name
			, aiNode const & rootNode
			, castor::Vector< AssimpNodeData > const & nodes
			, castor::Matrix4x4f transform )
		{
			auto node = rootNode.FindNode( castor::toUtf8( file.getExternalName( name ) ).c_str() );

			if ( node )
			{
				castor::Vector< castor::Matrix4x4f > transforms;
				accumulateTransformsRec( node->mParent, nodes, transforms );
				std::reverse( transforms.begin(), transforms.end() );
				castor::Matrix4x4f cumulative{ 1.0f };

				for ( auto & t : transforms )
				{
					cumulative *= t;
				}

				transform = cumulative * transform;
			}

			return transform;
		}
	}

	//*********************************************************************************************

	castor::MbString const AssimpImporterFile::Name = "ASSIMP Importer";

	AssimpImporterFile::AssimpImporterFile( castor3d::Engine & engine
		, castor3d::Scene * scene
		, castor::Path const & path
		, castor3d::Parameters const & parameters
		, castor3d::ProgressBar * progress )
		: castor3d::ImporterFile{ engine, scene, path, parameters, progress }
		, m_aiScene{ file::loadScene( m_importer, getFileName(), getParameters() ) }
	{
		if ( m_aiScene )
		{
			for ( auto aiMesh : castor::makeArrayView( m_aiScene->mMeshes, m_aiScene->mNumMeshes ) )
			{
				for ( auto aiBone : castor::makeArrayView( aiMesh->mBones, aiMesh->mNumBones ) )
				{
					m_bonesNodes.emplace( makeString( aiBone->mName )
						, fromAssimp( aiBone->mOffsetMatrix ) );
				}
			}

			doPrelistMaterials();
			doPrelistMeshes( doPrelistSkeletons() );
			castor::Map< AssimpMeshData const *, aiNodeArray > processed;
			castor::Map< aiNode const *, castor::Matrix4x4f > cumulativeTransforms;
			doPrelistSceneNodes( *m_aiScene->mRootNode, processed, cumulativeTransforms );
			doPrelistLights();
			doPrelistCameras();
		}
	}

	castor::String AssimpImporterFile::getMaterialName( uint32_t materialIndex )const
	{
		return file::getMaterialName( *this
			, *m_aiScene->mMaterials[materialIndex]
			, materialIndex );
	}

	NodeAnimations const & AssimpImporterFile::getNodesAnimations( castor3d::SceneNode const & node )const
	{
		auto it = std::find_if( m_sceneData.nodes.begin()
			, m_sceneData.nodes.end()
			, [&node]( AssimpNodeData const & lookup )
			{
				return node.getName() == lookup.name;
			} );

		if ( it != m_sceneData.nodes.end() )
		{
			return it->anims;
		}

		static NodeAnimations const dummy;
		return dummy;
	}

	SkeletonAnimations const & AssimpImporterFile::getSkeletonsAnimations( castor3d::Skeleton const & skeleton )const
	{
		auto name = skeleton.getName();

		if ( getListedMeshes().empty()
			&& !getSkeletons().empty() )
		{
			name = getSkeletons().begin()->first;
		}

		auto it = m_sceneData.skeletons.find( name );

		if ( it != m_sceneData.skeletons.end() )
		{
			return it->second.anims;
		}

		static SkeletonAnimations const dummy;
		return dummy;
	}

	MeshAnimations const & AssimpImporterFile::getMeshesAnimations( castor3d::Mesh const & mesh
		, uint32_t submeshIndex )const
	{
		auto it = m_sceneData.meshes.find( mesh.getName() );

		if ( it != m_sceneData.meshes.end()
			&& submeshIndex < it->second.submeshes.size() )
		{
			return it->second.submeshes[submeshIndex].anims;
		}

		static MeshAnimations const dummy;
		return dummy;
	}

	castor::StringArray AssimpImporterFile::listMaterials()
	{
		castor::StringArray result;

		for ( auto materials : m_sceneData.materials )
		{
			result.emplace_back( materials.first );
		}

		return result;
	}

	castor::Vector< castor3d::ImporterFile::MeshData > AssimpImporterFile::listMeshes()
	{
		m_listedMeshes.clear();
		castor::Vector< MeshData > result;

		for ( auto it : m_sceneData.meshes )
		{
			m_listedMeshes.emplace_back( it.first );
			result.emplace_back( it.first
				, ( it.second.skelNode
					? getInternalName( findSkeletonName( m_bonesNodes, *it.second.skelNode ) )
					: castor::String{} ) );
		}

		return result;
	}

	castor::StringArray AssimpImporterFile::listSkeletons()
	{
		castor::StringArray result;

		for ( auto it : m_sceneData.skeletons )
		{
			m_listedSkeletons.emplace_back( it.first );
			result.emplace_back( it.first );
		}

		return result;
	}

	castor::Vector< castor3d::ImporterFile::NodeData > AssimpImporterFile::listSceneNodes()
	{
		castor::Vector< NodeData > result;

		for ( auto & node : m_sceneData.nodes )
		{
			result.emplace_back( node.parent, node.name, node.isCamera );
		}

		return result;
	}

	castor::Vector< castor3d::ImporterFile::LightData > AssimpImporterFile::listLights()
	{
		castor::Vector< LightData > result;

		for ( auto & light : m_sceneData.lights )
		{
			result.emplace_back( light.first
				, ( light.second->mType == aiLightSource_DIRECTIONAL
					? castor3d::LightType::eDirectional
					: ( light.second->mType == aiLightSource_POINT
						? castor3d::LightType::ePoint
						: castor3d::LightType::eSpot ) ) );
		}

		return result;
	}

	castor::Vector< castor3d::ImporterFile::GeometryData > AssimpImporterFile::listGeometries()
	{
		castor::Vector< GeometryData > result;

		for ( auto & node : m_sceneData.nodes )
		{
			for ( auto & mesh : node.meshes )
			{
				auto it = std::find_if( m_sceneData.meshes.begin()
					, m_sceneData.meshes.end()
					, [mesh]( castor::StringMap< AssimpMeshData >::value_type const & lookup )
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

	castor::Vector< castor3d::ImporterFile::CameraData > AssimpImporterFile::listCameras()
	{
		castor::Vector< CameraData > result;

		for ( auto & camera : m_sceneData.cameras )
		{
			result.emplace_back( camera.first
				, ( camera.second->mOrthographicWidth != 0.0f
					? castor3d::ViewportType::eOrtho
					: castor3d::ViewportType::ePerspective ) );
		}

		return result;
	}

	castor::StringArray AssimpImporterFile::listMeshAnimations( castor3d::Mesh const & mesh )
	{
		castor::Set< castor::String > result;
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

		return castor::StringArray{ result.begin()
			, result.end() };
	}

	castor::StringArray AssimpImporterFile::listSkeletonAnimations( castor3d::Skeleton const & skeleton )
	{
		auto name = skeleton.getName();

		if ( getListedMeshes().empty()
			&& !getSkeletons().empty() )
		{
			name = getSkeletons().begin()->first;
		}

		castor::StringArray result;
		auto it = m_sceneData.skeletons.find( name );

		if ( it != m_sceneData.skeletons.end() )
		{
			for ( auto & anim : it->second.anims )
			{
				result.push_back( anim.first );
			}
		}

		return result;
	}

	castor::StringArray AssimpImporterFile::listSceneNodeAnimations( castor3d::SceneNode const & node )
	{
		castor::StringArray result;
		auto it = std::find_if( m_sceneData.nodes.begin()
			, m_sceneData.nodes.end()
			, [&node]( AssimpNodeData const & lookup )
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

	castor::StringArray AssimpImporterFile::listAllMeshAnimations()
	{
		castor::StringArray result;

		for ( auto & [_, mesh] : m_sceneData.meshes )
		{
			for ( auto & submesh : mesh.submeshes )
			{
				for ( auto & anim : submesh.anims )
				{
					result.emplace_back( anim.first );
				}
			}
		}

		return result;
	}

	castor::StringArray AssimpImporterFile::listAllSkeletonAnimations()
	{
		castor::StringArray result;

		for ( auto & [_, skeleton] : m_sceneData.skeletons )
		{
			for ( auto & anim : skeleton.anims )
			{
				result.push_back( anim.first );
			}
		}

		return result;
	}

	castor::StringArray AssimpImporterFile::listAllSceneNodeAnimations()
	{
		castor::StringArray result;

		for ( auto & node : m_sceneData.nodes )
		{
			for ( auto & anim : node.anims )
			{
				result.push_back( anim.first );
			}
		}

		return result;
	}

	castor3d::MaterialImporterUPtr AssimpImporterFile::createMaterialImporter()
	{
		return castor::makeUniqueDerived< castor3d::MaterialImporter, AssimpMaterialImporter >( *getOwner() );
	}

	castor3d::AnimationImporterUPtr AssimpImporterFile::createAnimationImporter()
	{
		return castor::makeUniqueDerived< castor3d::AnimationImporter, AssimpAnimationImporter >( *getOwner() );
	}

	castor3d::SkeletonImporterUPtr AssimpImporterFile::createSkeletonImporter()
	{
		return castor::makeUniqueDerived< castor3d::SkeletonImporter, AssimpSkeletonImporter >( *getOwner() );
	}

	castor3d::MeshImporterUPtr AssimpImporterFile::createMeshImporter()
	{
		return castor::makeUniqueDerived< castor3d::MeshImporter, AssimpMeshImporter >( *getOwner() );
	}

	castor3d::SceneNodeImporterUPtr AssimpImporterFile::createSceneNodeImporter()
	{
		return castor::makeUniqueDerived< castor3d::SceneNodeImporter, AssimpSceneNodeImporter >( *getOwner() );
	}

	castor3d::LightImporterUPtr AssimpImporterFile::createLightImporter()
	{
		return castor::makeUniqueDerived< castor3d::LightImporter, AssimpLightImporter >( *getOwner() );
	}

	castor3d::CameraImporterUPtr AssimpImporterFile::createCameraImporter()
	{
		return castor::makeUniqueDerived< castor3d::CameraImporter, AssimpCameraImporter >( *getOwner() );
	}

	castor3d::ImporterFileUPtr AssimpImporterFile::create( castor3d::Engine & engine
		, castor3d::Scene * scene
		, castor::Path const & path
		, castor3d::Parameters const & parameters
		, castor3d::ProgressBar * progress )
	{
		return castor::makeUniqueDerived< castor3d::ImporterFile, AssimpImporterFile >( engine, scene, path, parameters, progress );
	}

	void AssimpImporterFile::doPrelistMaterials()
	{
		uint32_t materialIndex = 0u;

		for ( auto aiMaterial : castor::makeArrayView( m_aiScene->mMaterials, m_aiScene->mNumMaterials ) )
		{
			auto name = file::getMaterialName( *this, *aiMaterial, materialIndex );
			m_sceneData.materials.emplace( name, aiMaterial );
			++materialIndex;
		}
	}

	castor::Map< aiMesh const *, aiNode const * > AssimpImporterFile::doPrelistSkeletons()
	{
		castor::Map< aiMesh const *, aiNode const * > result;
		uint32_t meshIndex = 0u;

		for ( auto aiMesh : castor::makeArrayView( m_aiScene->mMeshes, m_aiScene->mNumMeshes ) )
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
						, castor::makeArrayView( aiMesh->mBones, aiMesh->mNumBones )
						, meshNode );
					auto skelName = getInternalName( findSkeletonName( m_bonesNodes
						, *rootNode ) );
					m_sceneData.skeletons.emplace( skelName, AssimpSkeletonData{ rootNode } );
					result.emplace( aiMesh, rootNode );
				}
			}

			++meshIndex;
		}

		for ( auto aiAnimation : castor::makeArrayView( m_aiScene->mAnimations, m_aiScene->mNumAnimations ) )
		{
			if ( auto [skeletonData, skeleton] = file::findSkeletonForAnim( getScene(), *m_aiScene->mRootNode, *aiAnimation, m_sceneData );
				skeletonData )
			{
				auto [frameCount, minFrameTicks, maxFrameTicks] = getAnimationFrameTicks( *aiAnimation );

				if ( frameCount > 1 )
				{
					castor::String animName{ normalizeName( makeString( aiAnimation->mName ) ) };

					if ( animName.empty() )
					{
						animName = normalizeName( getName() );
					}

					if ( skeleton && skeleton->hasAnimation( animName ) )
					{
						animName += cuT( "_" ) + getName();
					}

					skeletonData->anims.emplace( animName, aiAnimation );
				}
			}
		}

		return result;
	}

	void AssimpImporterFile::doPrelistMeshes( castor::Map< aiMesh const *, aiNode const * > const & meshSkeletons )
	{
		uint32_t meshIndex = 0u;

		for ( auto aiMesh : castor::makeArrayView( m_aiScene->mMeshes, m_aiScene->mNumMeshes ) )
		{
			if ( isValidMesh( *aiMesh ) )
			{
				auto meshName = normalizeName( getInternalName( file::reworkMeshName( makeString( aiMesh->mName ), meshIndex ) ) );

				if ( meshName.size() > 150u )
				{
					meshName = getInternalName( getName() ) + castor::string::toString( meshIndex );
				}
				
				if ( file::hasNodeAnim( *m_aiScene, meshIndex ) )
				{
					meshName += castor::string::toString( meshIndex );
				}

				auto regIt = m_sceneData.meshes.find( meshName );
				aiNode const * skelNode{};

				if ( regIt != m_sceneData.meshes.end() )
				{
					meshName += castor::string::toString( meshIndex );
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
							, [&skelNode]( castor::StringMap< AssimpMeshData >::value_type const & lookup )
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
					regIt = m_sceneData.meshes.emplace( meshName
						, AssimpMeshData{ skelNode } ).first;
				}

				auto & submeshData = regIt->second.submeshes.emplace_back( aiMesh, meshIndex );
				m_meshes.insert( meshIndex );

				if ( aiMesh->mNumAnimMeshes )
				{
					auto anims = file::findMorphAnims( meshIndex
						, aiMesh->mNumAnimMeshes
						, *m_aiScene->mRootNode
						, castor::makeArrayView( m_aiScene->mAnimations, m_aiScene->mNumAnimations ) );

					for ( auto anim : anims )
					{
						castor::String animName{ normalizeName( anim.first ) };
						submeshData.anims.emplace( animName
							, std::make_pair( aiMesh, anim.second ) );
					}
				}
			}

			++meshIndex;
		}
	}

	void AssimpImporterFile::doPrelistSceneNodes( aiNode const & node
		, castor::Map< AssimpMeshData const *, aiNodeArray > & processedMeshes
		, castor::Map< aiNode const *, castor::Matrix4x4f > & cumulativeTransforms
		, castor::String parentName
		, castor::Matrix4x4f transform )
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
		cumulativeTransforms.emplace( &node, transform );
		bool isSkeletonNode = file::isSkeletonNode( node, aiNodeName, m_bonesNodes, m_sceneData.skeletons );
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
				, castor::makeArrayView( m_aiScene->mAnimations, m_aiScene->mNumAnimations ) );

			for ( auto anim : anims )
			{
				auto [frameCount, minFrameTicks, maxFrameTicks] = getNodeAnimFrameTicks( *anim.second );

				if ( frameCount > 1 )
				{
					castor::String animName{ normalizeName( makeString( anim.first->mName ) ) };

					if ( animName.empty() )
					{
						animName = normalizeName( aiNodeName );
					}

					nodeData.anims.emplace( animName, anim );
				}
			}
		}

		for ( auto meshIndex : castor::makeArrayView( node.mMeshes, node.mNumMeshes ) )
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
					auto & nodeArray = processedMeshes.emplace( &it->second, aiNodeArray{} ).first->second;
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

		m_sceneData.nodes.emplace_back( castor::move( nodeData ) );
		parentName = nodeName;

		// continue for all child nodes
		for ( auto aiChild : castor::makeArrayView( node.mChildren, node.mNumChildren ) )
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
		for ( auto aiLight : castor::makeArrayView( m_aiScene->mLights, m_aiScene->mNumLights ) )
		{
			if ( aiLight->mType == aiLightSource_DIRECTIONAL
				|| aiLight->mType == aiLightSource_POINT
				|| aiLight->mType == aiLightSource_SPOT )
			{
				castor::String name = getInternalName( aiLight->mName );
				m_sceneData.lights.emplace( name, aiLight );

				auto position = castor::Point3f{};
				auto orientation = castor::Quaternion::identity();

				if ( aiLight->mType == aiLightSource_DIRECTIONAL
					|| aiLight->mType == aiLightSource_SPOT )
				{
					auto direction = castor::point::getNormalised( fromAssimp( aiLight->mDirection ) );
					auto up = castor::point::getNormalised( fromAssimp( aiLight->mUp ) );
					orientation = castor::Quaternion::fromMatrix( fromAssimp( direction, up ) );
				}

				if ( aiLight->mType != aiLightSource_DIRECTIONAL )
				{
					position = fromAssimp( aiLight->mPosition );
				}

				auto transform = castor::Matrix4x4f{ 1.0f };
				castor::matrix::setTransform( transform
					, position
					, castor::Point3f{ 1.0, 1.0, 1.0 }
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
					castor::Point3f translate;
					castor::Point3f scale;
					castor::Quaternion rotate;
					castor::matrix::decompose( transform, translate, scale, rotate );
					m_sceneData.nodes.push_back( AssimpNodeData{ castor::String{}
						, name
						, false
						, nullptr
						, translate
						, rotate
						, scale } );
				}
				else
				{
					castor::Matrix4x4f matrix;
					castor::matrix::setTransform( matrix, it->translate, it->scale, it->rotate );
					matrix *= transform;
					castor::matrix::decompose( matrix, it->translate, it->scale, it->rotate );
				}
			}
		}
	}

	void AssimpImporterFile::doPrelistCameras()
	{
		for ( auto aiCamera : castor::makeArrayView( m_aiScene->mCameras, m_aiScene->mNumCameras ) )
		{
			castor::String name = getInternalName( aiCamera->mName );
			m_sceneData.cameras.emplace( name, aiCamera );

			auto position = castor::Point3f{};
			auto orientation = castor::Quaternion::identity();
			auto direction = castor::point::getNormalised( fromAssimp( aiCamera->mLookAt ) );
			auto up = castor::point::getNormalised( fromAssimp( aiCamera->mUp ) );
			position = fromAssimp( aiCamera->mPosition );
			orientation = castor::Quaternion::fromMatrix( fromAssimp( direction, up ) );

			auto transform = castor::Matrix4x4f{ 1.0f };
			castor::matrix::setTransform( transform
				, position
				, castor::Point3f{ 1.0, 1.0, 1.0 }
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
				castor::Point3f translate;
				castor::Point3f scale;
				castor::Quaternion rotate;
				castor::matrix::decompose( transform, translate, scale, rotate );
				m_sceneData.nodes.push_back( AssimpNodeData{ castor::String{}
					, name
					, false
					, nullptr
					, translate
					, rotate
					, scale } );
			}
			else
			{
				castor::Matrix4x4f matrix;
				castor::matrix::setTransform( matrix, it->translate, it->scale, it->rotate );
				matrix *= transform;
				castor::matrix::decompose( matrix, it->translate, it->scale, it->rotate );
			}
		}
	}

	//*********************************************************************************************
}
