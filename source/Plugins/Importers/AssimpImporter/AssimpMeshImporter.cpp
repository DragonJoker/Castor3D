#include "AssimpImporter/AssimpMeshImporter.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Material/MaterialImporter.hpp>
#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Model/Mesh/Mesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/BaseDataComponent.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/DefaultRenderComponent.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/SkinComponent.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/MorphComponent.hpp>
#include <Castor3D/Model/Skeleton/BoneNode.hpp>
#include <Castor3D/Model/Skeleton/Skeleton.hpp>
#include <Castor3D/Scene/Scene.hpp>

namespace c3d_assimp
{
	namespace meshes
	{
		static castor::Matrix4x4f getTranslation( aiMatrix4x4 const & transform )
		{
			aiQuaternion quat;
			aiVector3D tran;
			transform.DecomposeNoScaling( quat, tran );

			castor::Matrix4x4f result;
			castor::matrix::setTranslate( result, fromAssimp( tran ) );
			return result;
		}

		static castor3d::SkeletonRPtr findSkeletonForMesh( AssimpImporterFile const & file
			, castor3d::Scene & scene
			, aiNode const & sceneRootNode
			, aiNode const & skelRootNode
			, castor::String skelName )
		{
			auto skelIt = std::find_if( scene.getSkeletonCache().begin()
				, scene.getSkeletonCache().end()
				, [&file, &skelName]( auto const & lookup )
				{
					return file.getExternalName( lookup.second->getRootNode()->getName() ) == skelName;
				} );

			if ( skelIt != scene.getSkeletonCache().end() )
			{
				return skelIt->second.get();
			}

			for ( auto & skeleton : scene.getSkeletonCache() )
			{
				auto skelRootNodeName = castor::toUtf8( file.getExternalName( skeleton.second->getRootNode()->getName() ) );

				if ( toUtf8( sceneRootNode.mName ) == skelRootNodeName )
				{
					auto children = castor::makeArrayView( sceneRootNode.mChildren
						, sceneRootNode.mNumChildren );
					auto it = std::find_if( children.begin()
						, children.end()
						, [&skelRootNode, &skelRootNodeName]( aiNode const * lookup )
						{
							auto node = lookup->FindNode( skelRootNodeName.c_str() );
							return node != nullptr && node == &skelRootNode;
						} );

					if ( it != children.end() )
					{
						return skeleton.second.get();
					}
				}
				else if ( auto node = sceneRootNode.FindNode( skelRootNodeName.c_str() );
					&skelRootNode == node )
				{
					return skeleton.second.get();
				}
			}

			return nullptr;
		}
	}

	AssimpMeshImporter::AssimpMeshImporter( castor3d::Engine & engine )
		: castor3d::MeshImporter{ engine }
	{
	}

	bool AssimpMeshImporter::doImportMesh( castor3d::Mesh & mesh )
	{
		auto & file = static_cast< AssimpImporterFile const & >( *m_file );

		if ( file.getListedMeshes().empty() )
		{
			doImportSingleMesh( mesh );
			return true;
		}

		return doImportSceneMesh( mesh );
	}

	void AssimpMeshImporter::doImportSingleMesh( castor3d::Mesh & mesh )
	{
		auto & file = static_cast< AssimpImporterFile & >( *m_file );
		auto & aiScene = file.getAiScene();
		auto & scene = *mesh.getScene();
		uint32_t meshIndex{};

		for ( auto aiMesh : castor::makeArrayView( aiScene.mMeshes, aiScene.mNumMeshes ) )
		{
			if ( isValidMesh( *aiMesh ) )
			{
				auto matName = file.getMaterialName( aiMesh->mMaterialIndex );
				auto materialRes = scene.tryFindMaterial( matName );

				if ( !materialRes )
				{
					if ( auto importer = file.createMaterialImporter() )
					{
						auto mat = getOwner()->createMaterial( matName
							, *getOwner()
							, getOwner()->getDefaultLightingModel() );

						if ( importer->import( *mat
								, &file
								, castor3d::Parameters{}
							, castor::Map< castor3d::PassComponentTextureFlag, castor3d::TextureConfiguration >{} ) )
						{
							scene.getMaterialView().add( matName, mat, true );
						}
					}
				}

				doProcessMesh( aiScene
					, *aiMesh
					, meshIndex
					, mesh
					, *mesh.createSubmesh() );
			}

			++meshIndex;
		}

		doTransformMesh( *aiScene.mRootNode
			, mesh );
	}

	bool AssimpMeshImporter::doImportSceneMesh( castor3d::Mesh & mesh )
	{
		auto & file = static_cast< AssimpImporterFile const & >( *m_file );
		auto name = mesh.getName();
		auto it = file.getMeshes().find( name );

		if ( it == file.getMeshes().end() )
		{
			return false;
		}

		auto & aiScene = file.getAiScene();

		for ( auto submesh : it->second.submeshes )
		{
			doProcessMesh( aiScene
				, *submesh.mesh
				, submesh.meshIndex
				, mesh
				, *mesh.createSubmesh() );
		}

		return true;
	}

	void AssimpMeshImporter::doProcessMesh( aiScene const & aiScene
		, aiMesh const & aiMesh
		, uint32_t aiMeshIndex
		, castor3d::Mesh & mesh
		, castor3d::Submesh & submesh )
	{
		auto & file = static_cast< AssimpImporterFile & >( *m_file );
		auto & scene = *mesh.getScene();
		auto materialRes = scene.tryFindMaterial( file.getMaterialName( aiMesh.mMaterialIndex ) );
		castor3d::MaterialObs material{};

		if ( !materialRes )
		{
			material = scene.getEngine()->getDefaultMaterial();
		}
		else
		{
			material = materialRes;
		}

		castor3d::log::info << cuT( "  Mesh found: [" ) << file.getInternalName( aiMesh.mName ) << cuT( "]" ) << std::endl;
		submesh.setDefaultMaterial( material );
		submesh.createComponent< castor3d::DefaultRenderComponent >();

		auto positions = submesh.createComponent< castor3d::PositionsComponent >();
		auto normals = submesh.createComponent< castor3d::NormalsComponent >();
		castor::Point4fArray tan;
		castor::Point3fArray bit;
		castor::Point3fArray tex0;
		castor::Point3fArray tex1;
		castor::Point3fArray tex2;
		castor::Point3fArray tex3;
		castor::Point3fArray col;
		castor::Point4fArray * tangents = &tan;
		castor::Point3fArray * bitangents = &bit;
		castor::Point3fArray * texcoords0 = &tex0;
		castor::Point3fArray * texcoords1 = &tex1;
		castor::Point3fArray * texcoords2 = &tex2;
		castor::Point3fArray * texcoords3 = &tex3;
		castor::Point3fArray * colours = &col;

		if ( aiMesh.HasTextureCoords( 0u ) )
		{
			auto tanComp = submesh.createComponent< castor3d::TangentsComponent >();
			auto texComp = submesh.createComponent< castor3d::Texcoords0Component >();
			tangents = &tanComp->getData().getData();
			texcoords0 = &texComp->getData().getData();
		}

		if ( aiMesh.HasTextureCoords( 1u ) )
		{
			auto texComp = submesh.createComponent< castor3d::Texcoords1Component >();
			texcoords1 = &texComp->getData().getData();
		}

		if ( aiMesh.HasTextureCoords( 2u ) )
		{
			auto texComp = submesh.createComponent< castor3d::Texcoords2Component >();
			texcoords2 = &texComp->getData().getData();
		}

		if ( aiMesh.HasTextureCoords( 3u ) )
		{
			auto texComp = submesh.createComponent< castor3d::Texcoords3Component >();
			texcoords3 = &texComp->getData().getData();
		}

		if ( aiMesh.HasVertexColors( 0u )
			&& !file.getParameters().get< bool >( "ignore_vertex_colour" ) )
		{
			auto colComp = submesh.createComponent< castor3d::ColoursComponent >();
			colours = &colComp->getData().getData();
		}

		if ( aiMesh.HasTangentsAndBitangents() )
		{
			auto bitComp = submesh.createComponent< castor3d::BitangentsComponent >();
			bitangents = &bitComp->getData().getData();
		}

		createVertexBuffer( aiMesh
			, positions->getData().getData()
			, normals->getData().getData()
			, *tangents
			, *bitangents
			, *texcoords0
			, *texcoords1
			, *texcoords2
			, *texcoords3
			, *colours );
		auto animBuffers = gatherMeshAnimBuffers( positions->getData().getData()
			, normals->getData().getData()
			, *tangents
			, *bitangents
			, *texcoords0
			, *texcoords1
			, *texcoords2
			, *texcoords3
			, *colours
			, castor::makeArrayView( aiMesh.mAnimMeshes, aiMesh.mNumAnimMeshes ) );

		if ( !animBuffers.empty() )
		{
			castor3d::log::debug << cuT( "    Morph targets found: [" ) << uint32_t( animBuffers.size() ) << cuT( "]" ) << std::endl;
			auto component = submesh.hasComponent( castor3d::MorphComponent::TypeName )
				? submesh.getComponent< castor3d::MorphComponent >()
				: submesh.createComponent< castor3d::MorphComponent >();

			for ( auto & animBuffer : animBuffers )
			{
				component->getData().addMorphTarget( animBuffer );
			}
		}

		if ( aiMesh.HasBones() )
		{
			castor::Vector< castor3d::VertexBoneData > bonesData( aiMesh.mNumVertices );
			auto meshNode = findMeshNode( aiMeshIndex, *aiScene.mRootNode );
			auto rootNode = findRootSkeletonNode( *aiScene.mRootNode
				, castor::makeArrayView( aiMesh.mBones, aiMesh.mNumBones )
				, meshNode );
			auto skelName = file.getInternalName( findSkeletonName( file.getBonesNodes(), *rootNode ) );
			auto skeleton = meshes::findSkeletonForMesh( file
				, scene
				, *file.getAiScene().mRootNode
				, *rootNode
				, skelName );

			if ( skeleton )
			{
				for ( auto aiBone : castor::makeArrayView( aiMesh.mBones, aiMesh.mNumBones ) )
				{
					castor::String boneName = file.getInternalName( aiBone->mName );
					auto node = skeleton->findNode( boneName );
					CU_Require( node && node->getType() == castor3d::SkeletonNodeType::eBone );
					auto bone = &static_cast< castor3d::BoneNode & >( *node );

					for ( auto weight : castor::makeArrayView( aiBone->mWeights, aiBone->mNumWeights ) )
					{
						bonesData[weight.mVertexId].addBoneData( bone->getId(), weight.mWeight );
					}
				}

				auto bones = submesh.createComponent< castor3d::SkinComponent >();
				bones->getData().addDatas( bonesData );
			}
		}

		auto mapping = submesh.createComponent< castor3d::TriFaceMapping >();
		auto faces = castor::makeArrayView( aiMesh.mFaces, aiMesh.mNumFaces );

		for ( auto face : faces )
		{
			if ( face.mNumIndices == 3 )
			{
				mapping->getData().addFace( face.mIndices[0], face.mIndices[2], face.mIndices[1] );
			}
			else if ( face.mNumIndices == 4 )
			{
				mapping->getData().addQuadFace( face.mIndices[0], face.mIndices[2], face.mIndices[1], face.mIndices[2] );
			}
		}

		if ( !aiMesh.HasNormals() )
		{
			mapping->computeNormals();
			mapping->computeTangents();
		}
		else if ( !aiMesh.HasTangentsAndBitangents() )
		{
			mapping->computeTangents();
		}
	}

	void AssimpMeshImporter::doTransformMesh( aiNode const & aiNode
		, castor3d::Mesh & mesh
		, aiMatrix4x4 transformAcc )
	{
		transformAcc = transformAcc * aiNode.mTransformation;

		for ( auto aiMeshIndex : castor::makeArrayView( aiNode.mMeshes, aiNode.mNumMeshes ) )
		{
			if ( aiMeshIndex < mesh.getSubmeshCount() )
			{
				auto submesh = mesh.getSubmesh( aiMeshIndex );
				auto transform = ( submesh->hasComponent( castor3d::SkinComponent::TypeName )
					? meshes::getTranslation( transformAcc )
					: fromAssimp( transformAcc ) );

				for ( auto & vertex : submesh->getPositions() )
				{
					vertex = transform * vertex;
				}

				auto indexMapping = submesh->getIndexMapping();
				indexMapping->computeNormals();
				indexMapping->computeTangents();
			}
		}

		for ( auto child : castor::makeArrayView( aiNode.mChildren, aiNode.mNumChildren ) )
		{
			doTransformMesh( *child, mesh, transformAcc );
		}
	}
}
