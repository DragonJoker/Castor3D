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
			uint32_t importFlags{ aiProcess_Triangulate
				| aiProcess_FixInfacingNormals
				| aiProcess_LimitBoneWeights
				//| aiProcess_SplitByBoneCount
				| aiProcess_RemoveRedundantMaterials
				| aiProcess_FindDegenerates };
			if ( !parameters.get< bool >( cuT( "no_validation" ) ) )
				importFlags |= aiProcess_ValidateDataStructure
				| aiProcess_FindInvalidData;
			if ( !parameters.get< bool >( cuT( "no_optimisations" ) ) )
				importFlags |= aiProcess_JoinIdenticalVertices
				| aiProcess_OptimizeMeshes
				| aiProcess_OptimizeGraph
				| aiProcess_ImproveCacheLocality;
			if ( parameters.get< c3d::String >( cuT( "normals" ) ) == cuT( "smooth" ) )
				importFlags |= aiProcess_GenSmoothNormals;
			if ( parameters.get< bool >( cuT( "tangent_space" ) ) )
				importFlags |= aiProcess_CalcTangentSpace;

			importer.SetPropertyInteger( AI_CONFIG_PP_LBW_MAX_WEIGHTS, 8 );
			importer.SetPropertyBool( AI_CONFIG_IMPORT_NO_SKELETON_MESHES, true );
			importer.SetPropertyInteger( AI_CONFIG_IMPORT_TER_MAKE_UVS, 1 );
			importer.SetPropertyFloat( AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE, 80.0 );
			importer.SetPropertyInteger( AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, 0 ); //< Get rid of $AssimpFbx$_PreRotation nodes
			importer.SetPropertyInteger( AI_CONFIG_FBX_CONVERT_TO_M, 0 ); //< Convert FBX cm to m.

			try
			{
				auto result = importer.ReadFile( c3d::toUtf8( filePath ), importFlags );
				if ( !result )
					c3d::log::error << "Scene loading failed : " << importer.GetErrorString() << std::endl;
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

		static c3d::String getRawName( aiMaterial const & element )
		{
			c3d::String result;
			if ( aiString name;
				element.Get( AI_MATKEY_NAME, name ) == aiReturn_SUCCESS )
				result += makeString( name );
			return result;
		}

		static c3d::String getRawName( aiMesh const & element )
		{
			return makeString( element.mName );
		}

		static c3d::String getRawName( aiLight const & element )
		{
			return makeString( element.mName );
		}

		static c3d::String getRawName( aiCamera const & element )
		{
			return makeString( element.mName );
		}

		static c3d::String getRawName( aiSkeleton const & element )
		{
			return makeString( element.mName );
		}

		static c3d::String getRawName( aiAnimation const & element )
		{
			return makeString( element.mName );
		}

		template< typename aiElementT >
		static c3d::String getElementName( aiElementT const & element
			, uint32_t index
			, c3d::StringView baseName )
		{
			c3d::String result = getRawName( element );

			if ( result.empty() )
				result = c3d::String{ baseName } + cuT( "-" ) + c3d::string::toString( index );
			else
				result = c3d::ImporterFile::reworkName( result, baseName, index );

			return result;
		}

		template< typename aiElementT >
		static c3d::String getElementName( aiElementT const & element
			, uint32_t index
			, c3d::StringView baseName
			, NameContainer & names )
		{
			if ( auto it = names.namesByIndex.find( index );
				it != names.namesByIndex.end() )
				return it->second;

			auto rawName = getRawName( element );
			if ( auto it = names.namesByRawName.find( rawName );
				it != names.namesByRawName.end() )
				return it->second;

			auto result = rawName;
			if ( result.empty() )
				result = baseName;
			else
				result = c3d::ImporterFile::reworkName( result, baseName, index );

			if ( auto it = names.names.find( result );
				it != names.names.end() )
				result += cuT( "-" ) + c3d::string::toString( index );

			names.namesByRawName.try_emplace( rawName, result );
			names.namesByIndex.try_emplace( index, result );
			names.names.emplace( result );
			return result;
		}

		static bool isSkeletonNode( c3d::String const & aiNodeName
			, c3d::StringMap< c3d::Matrix4x4f > const & bonesNodes
			, c3d::StringMap< AssimpSkeletonData > const & skeletons )
		{
			if ( bonesNodes.find( aiNodeName ) != bonesNodes.end() )
				return true;

			auto mbName = c3d::toUtf8( aiNodeName );
			return skeletons.end() != std::find_if( skeletons.begin()
				, skeletons.end()
				, [&mbName]( c3d::StringMap< AssimpSkeletonData >::value_type const & lookup )
				{
					return lookup.second.rootNode->FindNode( mbName.c_str() ) != nullptr;
				} );
		}

		struct NodeAnimAndIndex
		{
			NodeAnimAndIndex( aiNodeAnim const * channel
				, uint32_t index )
				: channel{ channel }
				, index{ index }
			{
			}

			aiNodeAnim const * channel;
			uint32_t index;
		};

		static c3d::Map< aiAnimation const *, NodeAnimAndIndex > findNodeAnims( aiNode const & aiNode
			, c3d::ArrayView< aiAnimation * > const & animations )
		{
			c3d::Map< aiAnimation const *, NodeAnimAndIndex > result;
			uint32_t index{};

			for ( auto aiAnimation : animations )
			{
				auto channels = c3d::makeArrayView( aiAnimation->mChannels, aiAnimation->mNumChannels );
				if ( auto it = std::find_if( channels.begin(), channels.end(), [&aiNode]( aiNodeAnim const * lookup )
					{
						return lookup->mNodeName == aiNode.mName;
					} );
					it != channels.end() )
					result.try_emplace( aiAnimation, *it, index );
				++index;
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
					return { &skeleton, nullptr };
			}

			if ( scene )
			{
				for ( auto const & [name, skeleton] : scene->getSkeletonCache() )
				{
					if ( auto node = rootNode.FindNode( c3d::toUtf8( skeleton->getRootNode()->getName() ).c_str() ) )
					{
						auto & data = sceneData.skeletons.try_emplace( name, node ).first->second;
						if ( isAnimForSkeleton( animation, data ) )
							return { &data, skeleton.get() };
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
				if ( auto morphIt = std::find_if( morphChannels.begin()
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
							auto aiNodeMeshes = c3d::makeArrayView( node->mMeshes, node->mNumMeshes );
							res = aiNodeMeshes.end() != std::find( aiNodeMeshes.begin(), aiNodeMeshes.end(), aiMeshIndex );
						}

						return res;
					} );
				morphIt != morphChannels.end() )
				{
					result.try_emplace( makeString( anim->mName ), *morphIt );
				}
			}

			return result;
		}

		static c3d::Pair< AssimpMeshData const *, AssimpSubmeshData const * > findNodeMesh( uint32_t meshIndex
			, c3d::StringMap< AssimpMeshData > const & meshes )
		{
			auto meshIt = meshes.begin();

			while ( meshIt != meshes.end() )
			{
				auto const & meshData = meshIt->second;
				if ( auto submeshIt = std::find_if( meshData.submeshes.begin(), meshData.submeshes.end()
					, [&meshIndex]( AssimpSubmeshData const & submesh )
					{
						return submesh.meshIndex == meshIndex;
					} );
					submeshIt != meshData.submeshes.end() )
					return { &meshData, std::to_address( submeshIt ) };
				++meshIt;
			}

			return {};
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

		namespace node
		{
			static c3d::Matrix4x4f accumulateTransforms( c3d::String const & objectName
				, aiNode const & aiRootNode
				, c3d::Matrix4x4f transform )
			{
				if ( auto aiCurrentNode = aiRootNode.FindNode( objectName.c_str() ) )
					transform = computeCumulativeTransform( aiRootNode, *aiCurrentNode ) * transform;
				return transform;
			}

			aiNode const * findCommonNode( aiNode const & node, AssimpMeshData const & meshData )
			{
				if ( std::all_of( meshData.submeshes.begin(), meshData.submeshes.end()
					, [&node]( AssimpSubmeshData const & lookup )
					{
						// Check that the current node (or is children, recursively), has the submeshes in its mesh list
						return findMeshNode( lookup.meshIndex, node );
					} ) )
					return &node;

				if ( node.mParent )
					return findCommonNode( *node.mParent, meshData );

				c3d::log::error << "Couldn't find a common node for mesh." << std::endl;
				return nullptr;
			}

			static void addNodeMeshToProcessedMeshes( aiNode const & aiCurrentNode
				, c3d::Map< aiNode const *, c3d::Matrix4x4f > & cumulativeTransforms
				, c3d::Matrix4x4f const & transform
				, AssimpMeshData const & meshData
				, AssimpNodeData & nodeData
				, c3d::Map< AssimpMeshData const *, aiNodeArray > & processedMeshes )
			{
				// Don't add the mesh if it has already been added to a node with the same transform.
				auto & nodeArray = processedMeshes.try_emplace( &meshData ).first->second;
				if ( auto nodeIt = std::find_if( nodeArray.begin(), nodeArray.end()
					, [&cumulativeTransforms, &transform]( aiNode const * lookup )
					{
						auto lookupIt = cumulativeTransforms.find( lookup );
						return lookupIt->second == transform;
					} );
					nodeIt == nodeArray.end() )
				{
					nodeArray.push_back( &aiCurrentNode );
					nodeData.meshes.emplace_back( &meshData );
				}
			}
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
			if ( !getParameters().get< bool >( "no_skeleton" ) )
				for ( auto aiMesh : c3d::makeArrayView( m_aiScene->mMeshes, m_aiScene->mNumMeshes ) )
					for ( auto aiBone : c3d::makeArrayView( aiMesh->mBones, aiMesh->mNumBones ) )
						m_bonesNodes.try_emplace( makeString( aiBone->mName )
							, fromAssimp( aiBone->mOffsetMatrix ) );

			doPrelistMaterials();
			doPrelistMeshes( doPrelistSkeletons() );
			doPrelistSceneNodes();
			doPrelistLights();
			doPrelistCameras();

#if C3D_HasFbxMaterialImporter
			if ( c3d::string::lowerCase( path.getExtension() ) == cuT( "fbx" ) )
				m_fbxMaterials = c3d::makeRawUnique< c3d_fbx::FbxMaterialsFile >( path, parameters, m_materialNames.namesByRawName );
#endif
#if C3D_HasGltfMaterialImporter
			if ( c3d::string::lowerCase( path.getExtension() ) == cuT( "gltf" )
				|| c3d::string::lowerCase( path.getExtension() ) == cuT( "glb" ) )
				m_gltfMaterials = c3d::makeRawUnique< c3d_gltf::GltfMaterialsFile >( engine, path, parameters, getPrefix(), getName()
					, m_materialNames.namesByRawName );
#endif
#if C3D_HasPlyMeshImporter
			if ( c3d::string::lowerCase( path.getExtension() ) == cuT( "ply" ) )
				m_plyMesh = c3d::makeRawUnique< c3d_ply::PlyMeshFile >( path, m_meshNames.namesByRawName );
#endif
		}
	}

	c3d::String AssimpImporterFile::getMaterialName( c3d::u32 index )const
	{
		return getInternalName( file::getElementName( *m_aiScene->mMaterials[index], index, getName(), m_materialNames ) );
	}

	c3d::String AssimpImporterFile::getMeshName( c3d::u32 index )const
	{
		return getInternalName( file::getElementName( *m_aiScene->mMeshes[index], index, getName(), m_meshNames ) );
	}

	c3d::String AssimpImporterFile::getSkinName( c3d::u32 index )const
	{
		return getInternalName( file::getElementName( *m_aiScene->mSkeletons[index], index, getName(), m_skinNames ) );
	}

	c3d::String AssimpImporterFile::getLightName( c3d::u32 index )const
	{
		return getInternalName( file::getElementName( *m_aiScene->mLights[index], index, getName(), m_lightNames ) );
	}

	c3d::String AssimpImporterFile::getCameraName( c3d::u32 index )const
	{
		return getInternalName( file::getElementName( *m_aiScene->mCameras[index], index, getName(), m_cameraNames ) );
	}

	c3d::String AssimpImporterFile::getAnimationName( uint32_t index )const
	{
		return getInternalName( file::getElementName( *m_aiScene->mAnimations[index], index, getName() ) );
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
			name = getSkeletons().begin()->first;

		if ( auto it = m_sceneData.skeletons.find( name );
			it != m_sceneData.skeletons.end() )
			return it->second.anims;

		static SkeletonAnimations const dummy;
		return dummy;
	}

	MeshAnimations const & AssimpImporterFile::getMeshesAnimations( c3d::Mesh const & mesh
		, uint32_t submeshIndex )const
	{
		if ( auto it = m_sceneData.meshes.find( mesh.getName() );
			it != m_sceneData.meshes.end()
				&& submeshIndex < it->second.submeshes.size() )
			return it->second.submeshes[submeshIndex].anims;

		static MeshAnimations const dummy;
		return dummy;
	}

	c3d::StringArray AssimpImporterFile::listMaterials()
	{
		c3d::StringArray result;
		for ( auto const & [name, _] : m_sceneData.materials )
			result.emplace_back( name );
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
			result.emplace_back( node.parent, node.name, node.isCamera );
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

		for ( auto const & node : m_sceneData.nodes )
		{
			for ( auto meshData : node.meshes )
			{
				auto it = std::find_if( m_sceneData.meshes.begin()
					, m_sceneData.meshes.end()
					, [meshData]( c3d::StringMap< AssimpMeshData >::value_type const & lookup )
					{
						return meshData == &lookup.second;
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
				for ( auto & [name, _] : submesh.anims )
					result.insert( name );
		}

		return c3d::StringArray{ result.begin()
			, result.end() };
	}

	c3d::StringArray AssimpImporterFile::listSkeletonAnimations( c3d::Skeleton const & skeleton )
	{
		auto name = skeleton.getName();
		if ( getListedMeshes().empty()
			&& !getSkeletons().empty() )
			name = getSkeletons().begin()->first;

		c3d::StringArray result;

		if ( auto it = m_sceneData.skeletons.find( name );
			it != m_sceneData.skeletons.end() )
		{
			for ( auto const & [animName, _] : it->second.anims )
				result.push_back( animName );
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
				result.push_back( name );
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
			for ( auto & submesh : mesh.submeshes )
				result += uint32_t( submesh.anims.size() );
		return result;
	}

	uint32_t AssimpImporterFile::countAllSkeletonAnimations()const
	{
		uint32_t result{};
		for ( auto & [_, skeleton] : m_sceneData.skeletons )
			result += uint32_t( skeleton.anims.size() );
		return result;
	}

	uint32_t AssimpImporterFile::countAllSceneNodeAnimations()const
	{
		uint32_t result{};
		for ( auto & node : m_sceneData.nodes )
			result += uint32_t( node.anims.size() );
		return result;
	}

	uint32_t AssimpImporterFile::countAllTextureAnimations()const
	{
		return 0u;
	}

	c3d::MaterialImporterUPtr AssimpImporterFile::createMaterialImporter()
	{
#if C3D_HasFbxMaterialImporter
		if ( m_fbxMaterials )
			return m_fbxMaterials->createMaterialImporter( *getOwner() );
#endif
#if C3D_HasGltfMaterialImporter
		if ( m_gltfMaterials )
			return m_gltfMaterials->createMaterialImporter( *getOwner() );
#endif
		return c3d::makeUniqueDerived< c3d::MaterialImporter, AssimpMaterialImporter >( *getOwner(), getParameters() );
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
#if C3D_HasPlyMeshImporter
		if ( m_plyMesh )
			return m_plyMesh->createMeshImporter( *getOwner() );
#endif
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
			auto name = getMaterialName( materialIndex );
			m_sceneData.materials.try_emplace( name, aiMaterial );
			++materialIndex;
		}
	}

	c3d::Map< aiMesh const *, aiNode const * > AssimpImporterFile::doPrelistSkeletons()
	{
		if ( getParameters().get< bool >( "no_skeleton" ) )
			return {};

		c3d::Map< aiMesh const *, aiNode const * > result;
		c3d::Map< aiMesh const *, aiNode const * > rejected;
		uint32_t meshIndex = 0u;

		for ( auto aiMesh : c3d::makeArrayView( m_aiScene->mMeshes, m_aiScene->mNumMeshes ) )
		{
			if ( auto meshNode = findMeshNode( meshIndex, *m_aiScene->mRootNode );
				meshNode && aiMesh->HasBones() )
			{
				auto rootNode = findRootSkeletonNode( *m_aiScene->mRootNode
					, c3d::makeArrayView( aiMesh->mBones, aiMesh->mNumBones )
					, meshNode );
				auto skelName = getInternalName( findSkeletonName( m_bonesNodes
					, *rootNode ) );
				m_sceneData.skeletons.try_emplace( skelName, rootNode );
				result.emplace( aiMesh, rootNode );
			}
			else if ( meshNode )
			{
				rejected.emplace( aiMesh, meshNode );
			}

			++meshIndex;
		}

		for ( auto [aiMesh, meshNode] : rejected )
		{
			if ( auto it = std::find_if( result.begin(), result.end()
				, [meshNode]( auto const & lookup )
				{
					return lookup.second->FindNode( meshNode->mName ) != nullptr;
				} );
				it != result.end() )
			{
				auto rootNode = it->second;
				// The mesh node is contained in the children of a bone node, include this mesh the skeleton.
				result.emplace( aiMesh, rootNode );
			}
		}

		uint32_t animIndex{};
		for ( auto aiAnimation : c3d::makeArrayView( m_aiScene->mAnimations, m_aiScene->mNumAnimations ) )
		{
			if ( auto [skeletonData, skeleton] = file::findSkeletonForAnim( getScene(), *m_aiScene->mRootNode, *aiAnimation, m_sceneData );
				skeletonData )
			{
				auto [frameCount, minFrameTicks, maxFrameTicks] = getAnimationFrameTicks( *aiAnimation );
				if ( frameCount > 1 )
				{
					c3d::String animName{ getAnimationName( animIndex ) };
					if ( skeleton && skeleton->hasAnimation( animName ) )
						animName += cuT( "_" ) + getName();
					skeletonData->anims.try_emplace( animName, aiAnimation );
				}
			}

			++animIndex;
		}

		return result;
	}

	void AssimpImporterFile::doPrelistMeshes( c3d::Map< aiMesh const *, aiNode const * > const & meshSkeletons )
	{
		uint32_t meshIndex = 0u;
		auto noMeshMerge = getParameters().get< bool >( "no_merge" );

		for ( auto aiCurrentMesh : c3d::makeArrayView( m_aiScene->mMeshes, m_aiScene->mNumMeshes ) )
		{
			if ( auto aiMeshNode = findMeshNode( meshIndex, *m_aiScene->mRootNode );
				aiMeshNode && isValidMesh( *aiCurrentMesh ) )
			{
				auto meshName = getMeshName( meshIndex );
				if ( file::hasNodeAnim( *m_aiScene, meshIndex ) )
					meshName += c3d::string::toString( meshIndex );

				auto regIt = m_sceneData.meshes.find( meshName );
				if ( regIt != m_sceneData.meshes.end() )
				{
					meshName += c3d::string::toString( meshIndex );
					regIt = m_sceneData.meshes.find( meshName );
				}

				aiNode const * skelNode{};
				if ( regIt == m_sceneData.meshes.end()
					&& !noMeshMerge )
				{
					// Merge meshes that use the same skeleton
					if ( auto skelIt = meshSkeletons.find( aiCurrentMesh );
						skelIt != meshSkeletons.end() )
					{
						skelNode = skelIt->second;
						regIt = std::find_if( m_sceneData.meshes.begin(), m_sceneData.meshes.end()
							, [&skelNode]( c3d::StringMap< AssimpMeshData >::value_type const & lookup )
							{
								return skelNode == lookup.second.skelNode;
							} );
						if ( regIt != m_sceneData.meshes.end() )
							regIt = file::replaceIter( meshName, regIt, m_sceneData.meshes ).first;
					}
				}

				if ( regIt == m_sceneData.meshes.end() )
					regIt = m_sceneData.meshes.try_emplace( meshName, skelNode ).first;

				auto & submeshData = regIt->second.submeshes.emplace_back( aiCurrentMesh, aiMeshNode, meshIndex );

				// Parse mesh animations
				if ( aiCurrentMesh->mNumAnimMeshes )
				{
					auto anims = file::findMorphAnims( meshIndex
						, aiCurrentMesh->mNumAnimMeshes
						, *m_aiScene->mRootNode
						, c3d::makeArrayView( m_aiScene->mAnimations, m_aiScene->mNumAnimations ) );

					for ( auto const & [name, animData] : anims )
					{
						c3d::String animName{ normalizeName( name ) };
						submeshData.anims.try_emplace( animName, aiCurrentMesh, animData );
					}
				}
			}

			++meshIndex;
		}
	}

	void AssimpImporterFile::doPrelistSceneNodes()
	{
		c3d::Map< AssimpMeshData const *, aiNodeArray > processed;
		c3d::Map< aiNode const *, c3d::Matrix4x4f > cumulativeTransforms;
		c3d::Map< AssimpMeshData const *, aiNodeArray > postponedMeshes;
		doPrelistSceneNodesRec( *m_aiScene->mRootNode, processed, postponedMeshes, cumulativeTransforms );
	}

	void AssimpImporterFile::doPrelistSceneNodesRec( aiNode const & aiCurrentNode
		, c3d::Map< AssimpMeshData const *, aiNodeArray > & processedMeshes
		, c3d::Map< AssimpMeshData const *, aiNodeArray > & postponedMeshes
		, c3d::Map< aiNode const *, c3d::Matrix4x4f > & cumulativeTransforms
		, c3d::String parentName
		, c3d::Matrix4x4f transform )
	{
		auto aiNodeName = makeString( aiCurrentNode.mName );
		if ( m_bonesNodes.find( aiNodeName ) != m_bonesNodes.end() )
			return;

		aiVector3D translate;
		aiVector3D scale;
		aiQuaternion rotate;
		aiCurrentNode.mTransformation.Decompose( scale, rotate, translate );
		auto currentTransform = fromAssimp( aiCurrentNode.mTransformation );
		transform *= currentTransform;
		cumulativeTransforms.try_emplace( &aiCurrentNode, transform );
		bool isSkeletonNode = file::isSkeletonNode( aiNodeName, m_bonesNodes, m_sceneData.skeletons );
		auto nodeName = getInternalName( aiNodeName );
		AssimpNodeData nodeData{ parentName
			, nodeName
			, false
			, &aiCurrentNode
			, fromAssimp( translate )
			, fromAssimp( rotate )
			, fromAssimp( scale ) };

		if ( !isSkeletonNode )
		{
			auto anims = file::findNodeAnims( aiCurrentNode
				, c3d::makeArrayView( m_aiScene->mAnimations, m_aiScene->mNumAnimations ) );

			for ( auto const & [anim, channelIndex] : anims )
			{
				if ( auto [frameCount, minFrameTicks, maxFrameTicks] = getNodeAnimFrameTicks( *channelIndex.channel );
					frameCount > 1 )
					nodeData.anims.try_emplace( getAnimationName( channelIndex.index ), anim, channelIndex.channel );
			}
		}

		for ( auto meshIndex : c3d::makeArrayView( aiCurrentNode.mMeshes, aiCurrentNode.mNumMeshes ) )
		{
			if ( !file::isValidMesh( *m_aiScene, meshIndex ) )
				continue;

			if ( auto [meshData, submeshData] = file::findNodeMesh( meshIndex, m_sceneData.meshes );
				meshData && submeshData )
			{
				if ( nodeData.meshes.end() == std::find( nodeData.meshes.begin(), nodeData.meshes.end(), meshData ) )
				{
					if ( meshData->submeshes.size() > 1u )
					{
						// If the aiMesh is part of a more complex mesh,
						// we need to find an aiNode, from the current one, which fits for all
						// the mesh's submeshes.
						if ( aiNode const * aiCommonNode = file::node::findCommonNode( aiCurrentNode, *meshData ) )
						{
							if ( aiCommonNode != &aiCurrentNode )
							{
								// Since we're in a recursive call, forward to caller.
								auto & nodeArray = postponedMeshes.try_emplace( meshData ).first->second;
								nodeArray.push_back( aiCommonNode );
							}
							else
							{
								// No need to postpone, add it now
								file::node::addNodeMeshToProcessedMeshes( aiCurrentNode, cumulativeTransforms, transform
									, *meshData, nodeData, processedMeshes );
							}
						}
					}
					else
					{
						// If not, add the aiMesh to the processed ones.
						file::node::addNodeMeshToProcessedMeshes( aiCurrentNode, cumulativeTransforms, transform
							, *meshData, nodeData, processedMeshes );
					}
				}
			}
			else
			{
				CU_Failure( "Could not find node's mesh ?" );
			}
		}

		size_t index = m_sceneData.nodes.size();
		m_sceneData.nodes.emplace_back( c3d::move( nodeData ) );
		parentName = nodeName;

		// continue for all children nodes
		for ( auto aiChildNode : c3d::makeArrayView( aiCurrentNode.mChildren, aiCurrentNode.mNumChildren ) )
		{
			c3d::Map< AssimpMeshData const *, aiNodeArray > currentPostponedMeshes;
			doPrelistSceneNodesRec( *aiChildNode
				, processedMeshes
				, currentPostponedMeshes
				, cumulativeTransforms
				, parentName
				, transform );

			// Process the child postponed nodes that are for the current node
			for ( auto & [meshData, postponedNodeArray] : currentPostponedMeshes )
			{
				auto it = postponedNodeArray.begin();
				while ( it != postponedNodeArray.end() )
				{
					if ( auto aiCommonNode = *it;
						aiCommonNode == &aiCurrentNode )
					{
						// If the child postponed node is for aiCurrentNode, process its aiMesh
						file::node::addNodeMeshToProcessedMeshes( aiCurrentNode, cumulativeTransforms, transform
							, *meshData, m_sceneData.nodes[index], processedMeshes );
					}
					else
					{
						// If not, postpone again, completing the cumulative matrix.
						auto & dest = postponedMeshes.try_emplace( meshData ).first->second;
						dest.push_back( aiCommonNode );
					}
					++it;
				}
			}
		}
	}

	void AssimpImporterFile::doPrelistLights()
	{
		uint32_t lightIndex{};
		for ( auto aiLight : c3d::makeArrayView( m_aiScene->mLights, m_aiScene->mNumLights ) )
		{
			if ( aiLight->mType == aiLightSource_DIRECTIONAL
				|| aiLight->mType == aiLightSource_POINT
				|| aiLight->mType == aiLightSource_SPOT )
			{
				c3d::String name = getLightName( lightIndex );
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
					position = fromAssimp( aiLight->mPosition );

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
					file::node::accumulateTransforms( makeString( aiLight->mName )
						, *m_aiScene->mRootNode
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

			++lightIndex;
		}
	}

	void AssimpImporterFile::doPrelistCameras()
	{
		uint32_t cameraIndex{};
		for ( auto aiCamera : c3d::makeArrayView( m_aiScene->mCameras, m_aiScene->mNumCameras ) )
		{
			c3d::String name = getCameraName( cameraIndex );
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

			if ( auto it = std::find_if( m_sceneData.nodes.begin()
				, m_sceneData.nodes.end()
				, [&name]( AssimpNodeData const & lookup )
				{
					return lookup.name == name;
				} );
				it == m_sceneData.nodes.end() )
			{
				file::node::accumulateTransforms( makeString( aiCamera->mName )
					, *m_aiScene->mRootNode
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

			++cameraIndex;
		}
	}

	//*********************************************************************************************
}
