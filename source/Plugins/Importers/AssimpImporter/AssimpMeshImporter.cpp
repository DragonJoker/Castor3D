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
		static c3d::Matrix4x4f getTranslation( aiMatrix4x4 const & transform )
		{
			aiQuaternion quat;
			aiVector3D tran;
			transform.DecomposeNoScaling( quat, tran );

			c3d::Matrix4x4f result;
			c3d::matrix::setTranslate( result, fromAssimp( tran ) );
			return result;
		}

		static c3d::SkeletonRPtr findSkeletonForMesh( AssimpImporterFile const & file
			, c3d::Scene & scene
			, aiNode const & sceneRootNode
			, aiNode const & skelRootNode
			, c3d::String const & skelName )
		{

			if ( auto skelIt = std::find_if( scene.getSkeletonCache().begin()
				, scene.getSkeletonCache().end()
				, [&file, &skelName]( auto const & lookup )
				{
					return file.getExternalName( lookup.second->getRootNode()->getName() ) == skelName;
				} );
				skelIt != scene.getSkeletonCache().end() )
			{
				return skelIt->second.get();
			}

			for ( auto const & [_, skeleton] : scene.getSkeletonCache() )
			{
				auto skelRootNodeName = c3d::toUtf8( file.getExternalName( skeleton->getRootNode()->getName() ) );

				if ( toUtf8( sceneRootNode.mName ) == skelRootNodeName )
				{
					auto children = c3d::makeArrayView( sceneRootNode.mChildren
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
						return skeleton.get();
					}
				}
				else if ( auto node = sceneRootNode.FindNode( skelRootNodeName.c_str() );
					&skelRootNode == node )
				{
					return skeleton.get();
				}
			}

			return nullptr;
		}
	}

	AssimpMeshImporter::AssimpMeshImporter( c3d::Engine & engine )
		: c3d::MeshImporter{ engine, cuT( "Assimp" ) }
	{
	}

	bool AssimpMeshImporter::doImportMesh( c3d::Mesh & mesh, uint32_t submeshIndex )
	{
		if ( auto & file = static_cast< AssimpImporterFile const & >( *m_file );
			file.getListedMeshes().empty() )
		{
			doImportSingleMesh( mesh, submeshIndex );
			return true;
		}

		return doImportSceneMesh( mesh, submeshIndex );
	}

	void AssimpMeshImporter::doImportSingleMesh( c3d::Mesh & mesh, uint32_t submeshIndex )
	{
		auto & file = static_cast< AssimpImporterFile & >( *m_file );
		auto & aiScene = file.getAiScene();
		auto & scene = *mesh.getScene();
		uint32_t meshIndex{};

		for ( auto aiMesh : c3d::makeArrayView( aiScene.mMeshes, aiScene.mNumMeshes ) )
		{
			if ( isValidMesh( *aiMesh )
				&& ( submeshIndex == meshIndex || submeshIndex == 0xFFFFFFFFu ) )
			{
				auto matName = file.getMaterialName( aiMesh->mMaterialIndex );

				if ( auto materialRes = scene.tryFindMaterial( matName );
					!materialRes )
				{
					if ( auto importer = file.createMaterialImporter() )
					{
						auto mat = getOwner()->createMaterial( matName
							, *getOwner()
							, getOwner()->getDefaultLightingModel() );

						if ( importer->importData( *mat
								, &file
								, c3d::Parameters{}
							, c3d::Map< c3d::PassComponentTextureFlag, c3d::TextureConfiguration >{} ) )
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

	bool AssimpMeshImporter::doImportSceneMesh( c3d::Mesh & mesh, uint32_t submeshIndex )const
	{
		auto & file = static_cast< AssimpImporterFile const & >( *m_file );
		auto name = mesh.getName();
		auto it = file.getMeshes().find( name );

		if ( it == file.getMeshes().end() )
		{
			return false;
		}

		auto & aiScene = file.getAiScene();

		for ( auto const & submesh : it->second.submeshes )
		{
			if ( submeshIndex == submesh.meshIndex || submeshIndex == 0xFFFFFFFFu )
			{
				doProcessMesh( aiScene
					, *submesh.mesh
					, submesh.meshIndex
					, mesh
					, *mesh.createSubmesh() );
			}
		}

		return true;
	}

	void AssimpMeshImporter::doProcessMesh( aiScene const & aiScene
		, aiMesh const & aiMesh
		, uint32_t aiMeshIndex
		, c3d::Mesh const & mesh
		, c3d::Submesh & submesh )const
	{
		auto const & file = static_cast< AssimpImporterFile & >( *m_file );
		auto & scene = *mesh.getScene();
		c3d::MaterialObs material{};

		if ( auto materialRes = scene.tryFindMaterial( file.getMaterialName( aiMesh.mMaterialIndex ) ) )
			material = materialRes;
		else
			material = scene.getEngine()->getDefaultMaterial();

		submesh.setDefaultMaterial( material );
		submesh.createComponent< c3d::DefaultRenderComponent >();

		auto positions = submesh.createComponent< c3d::PositionsComponent >();
		auto normals = submesh.createComponent< c3d::NormalsComponent >();
		c3d::Point4fArray tan;
		c3d::Point3fArray bit;
		c3d::Point3fArray tex0;
		c3d::Point3fArray tex1;
		c3d::Point3fArray tex2;
		c3d::Point3fArray tex3;
		c3d::Point3fArray col;
		c3d::Point4fArray * tangents = &tan;
		c3d::Point3fArray * bitangents = &bit;
		c3d::Point3fArray * texcoords0 = &tex0;
		c3d::Point3fArray * texcoords1 = &tex1;
		c3d::Point3fArray * texcoords2 = &tex2;
		c3d::Point3fArray * texcoords3 = &tex3;
		c3d::Point3fArray * colours = &col;

		if ( aiMesh.HasTextureCoords( 0u ) )
		{
			auto tanComp = submesh.createComponent< c3d::TangentsComponent >();
			auto texComp = submesh.createComponent< c3d::Texcoords0Component >();
			tangents = &tanComp->getData().getData();
			texcoords0 = &texComp->getData().getData();
		}

		if ( aiMesh.HasTextureCoords( 1u ) )
		{
			auto texComp = submesh.createComponent< c3d::Texcoords1Component >();
			texcoords1 = &texComp->getData().getData();
		}

		if ( aiMesh.HasTextureCoords( 2u ) )
		{
			auto texComp = submesh.createComponent< c3d::Texcoords2Component >();
			texcoords2 = &texComp->getData().getData();
		}

		if ( aiMesh.HasTextureCoords( 3u ) )
		{
			auto texComp = submesh.createComponent< c3d::Texcoords3Component >();
			texcoords3 = &texComp->getData().getData();
		}

		if ( aiMesh.HasVertexColors( 0u )
			&& !file.getParameters().get< bool >( "ignore_vertex_colour" ) )
		{
			auto colComp = submesh.createComponent< c3d::ColoursComponent >();
			colours = &colComp->getData().getData();
		}

		if ( aiMesh.HasTangentsAndBitangents() )
		{
			auto bitComp = submesh.createComponent< c3d::BitangentsComponent >();
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

		if ( auto animBuffers = gatherMeshAnimBuffers( positions->getData().getData()
				, normals->getData().getData()
				, *tangents
				, *bitangents
				, *texcoords0
				, *texcoords1
				, *texcoords2
				, *texcoords3
				, *colours
				, c3d::makeArrayView( aiMesh.mAnimMeshes, aiMesh.mNumAnimMeshes ) );
			!animBuffers.empty() )
		{
			c3d::log::debug << cuT( "    Morph targets found: [" ) << uint32_t( animBuffers.size() ) << cuT( "]" ) << std::endl;
			auto component = submesh.hasComponent( c3d::MorphComponent::TypeName )
				? submesh.getComponent< c3d::MorphComponent >()
				: submesh.createComponent< c3d::MorphComponent >();

			for ( auto const & animBuffer : animBuffers )
			{
				component->getData().addMorphTarget( animBuffer );
			}
		}

		if ( aiMesh.HasBones() )
		{
			c3d::Vector< c3d::VertexBoneData > bonesData( aiMesh.mNumVertices );
			auto meshNode = findMeshNode( aiMeshIndex, *aiScene.mRootNode );
			auto rootNode = findRootSkeletonNode( *aiScene.mRootNode
				, c3d::makeArrayView( aiMesh.mBones, aiMesh.mNumBones )
				, meshNode );
			auto skelName = file.getInternalName( findSkeletonName( file.getBonesNodes(), *rootNode ) );
			auto skeleton = meshes::findSkeletonForMesh( file
				, scene
				, *file.getAiScene().mRootNode
				, *rootNode
				, skelName );

			if ( skeleton )
			{
				for ( auto aiBone : c3d::makeArrayView( aiMesh.mBones, aiMesh.mNumBones ) )
				{
					c3d::String boneName = file.getInternalName( aiBone->mName );
					auto node = skeleton->findNode( boneName );
					CU_Require( node && node->getType() == c3d::SkeletonNodeType::eBone );
					auto bone = &static_cast< c3d::BoneNode & >( *node );

					for ( auto weight : c3d::makeArrayView( aiBone->mWeights, aiBone->mNumWeights ) )
					{
						bonesData[weight.mVertexId].addBoneData( bone->getId(), weight.mWeight );
					}
				}

				auto bones = submesh.createComponent< c3d::SkinComponent >();
				bones->getData().addDatas( bonesData );
			}
		}

		auto mapping = submesh.createComponent< c3d::TriFaceMapping >();
		auto faces = c3d::makeArrayView( aiMesh.mFaces, aiMesh.mNumFaces );

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
		, c3d::Mesh & mesh
		, aiMatrix4x4 transformAcc )
	{
		transformAcc = transformAcc * aiNode.mTransformation;

		for ( auto aiMeshIndex : c3d::makeArrayView( aiNode.mMeshes, aiNode.mNumMeshes ) )
		{
			if ( aiMeshIndex < mesh.getSubmeshCount() )
			{
				auto submesh = mesh.getSubmesh( aiMeshIndex );
				auto transform = ( submesh->hasComponent( c3d::SkinComponent::TypeName )
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

		for ( auto child : c3d::makeArrayView( aiNode.mChildren, aiNode.mNumChildren ) )
		{
			doTransformMesh( *child, mesh, transformAcc );
		}
	}
}
