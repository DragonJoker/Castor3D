#include "AssimpImporter/AssimpImporterFile.hpp"

#include "AssimpImporter/AssimpAnimationImporter.hpp"
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

#pragma warning( push )
#pragma warning( disable: 4365 )
#pragma warning( disable: 4619 )
#include <assimp/material.h>
#include <assimp/postprocess.h>
#pragma warning( pop )

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
			auto found = parameters.get( "no_optimisations", noOptim );
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
				auto result = importer.ReadFile( castor::string::stringCast< char >( filePath ), importFlags );

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

		static castor::String getMaterialName( AssimpImporterFile const & file
			, aiMaterial const & aiMaterial
			, uint32_t materialIndex )
		{
			castor::String result;
			aiString name;

			if ( aiMaterial.Get( AI_MATKEY_NAME, name ) == aiReturn_SUCCESS )
			{
				result += makeString( name );
			}
			else
			{
				result += file.getName() + cuT( "-" ) + castor::string::toString( materialIndex );
			}

			return result;
		}

		static bool isSkeletonNode( aiNode const & node
			, castor::String const & aiNodeName
			, std::map< castor::String, castor::Matrix4x4f > const & bonesNodes
			, std::map< castor::String, SkeletonData > const & skeletons )
		{
			if ( bonesNodes.find( aiNodeName ) != bonesNodes.end() )
			{
				return true;
			}

			return skeletons.end() != std::find_if( skeletons.begin()
				, skeletons.end()
				, [&aiNodeName]( std::map< castor::String, SkeletonData >::value_type const & lookup )
				{
					return lookup.second.rootNode->FindNode( aiNodeName.c_str() ) != nullptr;
				} );
		}

		static std::map< aiAnimation const *, aiNodeAnim const * > findNodeAnims( aiNode const & aiNode
			, castor::ArrayView< aiAnimation * > const & animations )
		{
			std::map< aiAnimation const *, aiNodeAnim const * > result;

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
			, SkeletonData const & skeleton )
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

		static std::pair< SkeletonData *, castor3d::SkeletonRPtr > findSkeletonForAnim( castor3d::Scene * scene
			, aiNode const & rootNode
			, aiAnimation const & animation
			, SceneData & sceneData )
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
					if ( auto node = rootNode.FindNode( skeleton.second->getRootNode()->getName().c_str() ) )
					{
						auto & data = sceneData.skeletons.emplace( skeleton.first
							, SkeletonData{ node } ).first->second;

						if ( isAnimForSkeleton( animation, data ) )
						{
							return { &data, skeleton.second.get() };
						}
					}
				}
			}

			return { nullptr, nullptr };
		}

		static std::map< castor::String, aiMeshMorphAnim const * > findMorphAnims( uint32_t aiMeshIndex
			, uint32_t aiNumAnimMeshes
			, aiNode const & rootNode
			, castor::ArrayView< aiAnimation * > animations )
		{
			std::map< castor::String, aiMeshMorphAnim const * > result;

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
			, std::map< castor::String, MeshData > const & meshes )
		{
			return std::find_if( meshes.begin()
				, meshes.end()
				, [&meshIndex]( std::map< castor::String, MeshData >::value_type const & lookup )
				{
					return lookup.second.submeshes.end() != std::find_if( lookup.second.submeshes.begin()
						, lookup.second.submeshes.end()
						, [&meshIndex]( SubmeshData const & submesh )
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
			castor::String separators = " \t\r_$|/:\\*!?&#\"()[]{}@+.";
			auto split = castor::string::split( name, separators, ~( 0u ), false );
			std::set< int > numbers;
			std::set< castor::String > names;

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
				sep = "_";
			}

			for ( auto i : numbers )
			{
				result += sep + castor::string::toString( i );
				sep = "_";
			}

			return result;
		}

		static void accumulateTransformsRec( aiNode const * node
			, std::vector< NodeData > const & nodes
			, std::vector< castor::Matrix4x4f > & transforms )
		{
			if ( !node )
			{
				return;
			}

			auto it = std::find_if( nodes.begin()
				, nodes.end()
				, [&node]( NodeData const & lookup )
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
				transforms.push_back( it->transform );
			}
		}

		static castor::Matrix4x4f accumulateTransforms( AssimpImporterFile const & file
			, castor::String const & name
			, aiNode const & rootNode
			, std::vector< NodeData > const & nodes
			, castor::Matrix4x4f transform )
		{
			auto node = rootNode.FindNode( file.getExternalName( name ).c_str() );

			if ( node )
			{
				std::vector< castor::Matrix4x4f > transforms;
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

	castor::String const AssimpImporterFile::Name = cuT( "ASSIMP Importer" );

	AssimpImporterFile::AssimpImporterFile( castor3d::Engine & engine
		, castor3d::Scene * scene
		, castor::Path const & path
		, castor3d::Parameters const & parameters )
		: castor3d::ImporterFile{ engine, scene, path, parameters }
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
			std::map< MeshData const *, aiNodeArray > processed;
			std::map< aiNode const *, castor::Matrix4x4f > cumulativeTransforms;
			doPrelistSceneNodes( *m_aiScene->mRootNode, processed, cumulativeTransforms );
			doPrelistLights();
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
			, [&node]( NodeData const & lookup )
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

	std::vector< castor::String > AssimpImporterFile::listMaterials()
	{
		std::vector< castor::String > result;

		for ( auto materials : m_sceneData.materials )
		{
			result.emplace_back( materials.first );
		}

		return result;
	}

	std::vector< std::pair< castor::String, castor::String > > AssimpImporterFile::listMeshes()
	{
		m_listedMeshes.clear();
		std::vector< std::pair< castor::String, castor::String > > result;

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

	std::vector< castor::String > AssimpImporterFile::listSkeletons()
	{
		std::vector< castor::String > result;

		for ( auto it : m_sceneData.skeletons )
		{
			m_listedSkeletons.emplace_back( it.first );
			result.emplace_back( it.first );
		}

		return result;
	}

	std::vector< castor::String > AssimpImporterFile::listSceneNodes()
	{
		std::vector< castor::String > result;

		for ( auto & node : m_sceneData.nodes )
		{
			result.emplace_back( node.name );
		}

		return result;
	}

	std::vector< std::pair< castor::String, castor3d::LightType > > AssimpImporterFile::listLights()
	{
		std::vector< std::pair< castor::String, castor3d::LightType > > result;

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

	std::vector< AssimpImporterFile::GeometryData > AssimpImporterFile::listGeometries()
	{
		std::vector< GeometryData > result;

		for ( auto & node : m_sceneData.nodes )
		{
			for ( auto & mesh : node.meshes )
			{
				auto it = std::find_if( m_sceneData.meshes.begin()
					, m_sceneData.meshes.end()
					, [mesh]( std::map< castor::String, MeshData >::value_type const & lookup )
					{
						return mesh == &lookup.second;
					} );
				CU_Require( it != m_sceneData.meshes.end() );
				auto meshName = getInternalName( it->first );
				auto name = node.name == meshName
					? node.name
					: node.name + meshName;
				result.emplace_back( GeometryData{ name, node.name, it->first } );
			}
		}

		return result;
	}

	std::vector< castor::String > AssimpImporterFile::listMeshAnimations( castor3d::Mesh const & mesh )
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

	std::vector< castor::String > AssimpImporterFile::listSkeletonAnimations( castor3d::Skeleton const & skeleton )
	{
		auto name = skeleton.getName();

		if ( getListedMeshes().empty()
			&& !getSkeletons().empty() )
		{
			name = getSkeletons().begin()->first;
		}

		std::vector< castor::String > result;
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

	std::vector< castor::String > AssimpImporterFile::listSceneNodeAnimations( castor3d::SceneNode const & node )
	{
		std::vector< castor::String > result;
		auto it = std::find_if( m_sceneData.nodes.begin()
			, m_sceneData.nodes.end()
			, [&node]( NodeData const & lookup )
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

	castor3d::MaterialImporterUPtr AssimpImporterFile::createMaterialImporter()
	{
		return std::make_unique< AssimpMaterialImporter >( *getOwner() );
	}

	castor3d::AnimationImporterUPtr AssimpImporterFile::createAnimationImporter()
	{
		return std::make_unique< AssimpAnimationImporter >( *getOwner() );
	}

	castor3d::SkeletonImporterUPtr AssimpImporterFile::createSkeletonImporter()
	{
		return std::make_unique< AssimpSkeletonImporter >( *getOwner() );
	}

	castor3d::MeshImporterUPtr AssimpImporterFile::createMeshImporter()
	{
		return std::make_unique< AssimpMeshImporter >( *getOwner() );
	}

	castor3d::SceneNodeImporterUPtr AssimpImporterFile::createSceneNodeImporter()
	{
		return std::make_unique< AssimpSceneNodeImporter >( *getOwner() );
	}

	castor3d::LightImporterUPtr AssimpImporterFile::createLightImporter()
	{
		return std::make_unique< AssimpLightImporter >( *getOwner() );
	}

	castor3d::ImporterFileUPtr AssimpImporterFile::create( castor3d::Engine & engine
		, castor3d::Scene * scene
		, castor::Path const & path
		, castor3d::Parameters const & parameters )
	{
		return std::make_unique< AssimpImporterFile >( engine, scene, path, parameters );
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

	std::map< aiMesh const *, aiNode const * > AssimpImporterFile::doPrelistSkeletons()
	{
		std::map< aiMesh const *, aiNode const * > result;
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
					m_sceneData.skeletons.emplace( skelName, SkeletonData{ rootNode } );
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
				auto [frameCount, frameTicks] = getAnimationFrameTicks( *aiAnimation );

				if ( frameCount > 1 )
				{
					castor::String animName{ normalizeName( makeString( aiAnimation->mName ) ) };

					if ( animName.empty() )
					{
						animName = normalizeName( getName() );
					}

					if ( skeleton && skeleton->hasAnimation( animName ) )
					{
						animName += "_" + getName();
					}

					skeletonData->anims.emplace( animName, aiAnimation );
				}
			}
		}

		return result;
	}

	void AssimpImporterFile::doPrelistMeshes( std::map< aiMesh const *, aiNode const * > const & meshSkeletons )
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
							, [&skelNode]( std::map< castor::String, MeshData >::value_type const & lookup )
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
						, MeshData{ skelNode } ).first;
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
		, std::map< MeshData const *, aiNodeArray > & processedMeshes
		, std::map< aiNode const *, castor::Matrix4x4f > & cumulativeTransforms
		, castor::String parentName
		, castor::Matrix4x4f transform )
	{
		auto aiNodeName = makeString( node.mName );

		if ( m_bonesNodes.find( aiNodeName ) != m_bonesNodes.end() )
		{
			return;
		}

		auto nodeTransform = fromAssimp( node.mTransformation );
		transform *= nodeTransform;
		cumulativeTransforms.emplace( &node, transform );
		bool isSkeletonNode = file::isSkeletonNode( node, aiNodeName, m_bonesNodes, m_sceneData.skeletons );
		auto anims = ( node.mParent && isSkeletonNode )
			? std::map< aiAnimation const *, aiNodeAnim const * >{}
			: file::findNodeAnims( node
				, castor::makeArrayView( m_aiScene->mAnimations, m_aiScene->mNumAnimations ) );
		auto nodeName = getInternalName( aiNodeName );
		NodeData nodeData{ parentName
			, nodeName
			, &node
			, nodeTransform };

		if ( !isSkeletonNode )
		{
			for ( auto anim : anims )
			{
				auto [frameCount, frameTicks] = getNodeAnimFrameTicks( *anim.second );

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

		m_sceneData.nodes.emplace_back( std::move( nodeData ) );
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
					, [&name]( NodeData const & lookup )
					{
						return lookup.name == name;
					} );

				if ( it == m_sceneData.nodes.end() )
				{
					m_sceneData.nodes.push_back( NodeData{ castor::String{}
						, name
						, nullptr
						, file::accumulateTransforms( *this
							, name
							, *m_aiScene->mRootNode
							, m_sceneData.nodes
							, transform ) } );
				}
				else
				{
					it->transform = transform;
				}
			}
		}
	}

	//*********************************************************************************************
}
