#include "AssimpImporter/AssimpMeshImporter.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Model/Mesh/Mesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/BaseDataComponent.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/BonesComponent.hpp>
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
		auto & aiScene = file.getScene();

		for ( auto aiMesh : castor::makeArrayView( aiScene.mMeshes, aiScene.mNumMeshes ) )
		{
			auto faces = castor::makeArrayView( aiMesh->mFaces, aiMesh->mNumFaces );
			auto count = uint32_t( std::count_if( faces.begin()
				, faces.end()
				, []( aiFace const & face )
				{
					return face.mNumIndices == 3
						|| face.mNumIndices == 4;
				} ) );

			if ( count > 0 && aiMesh->HasPositions() )
			{
				doProcessMesh( aiScene
					, *aiMesh
					, mesh
					, *mesh.createSubmesh() );
			}
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

		auto & aiScene = file.getScene();

		for ( auto submesh : it->second.submeshes )
		{
			doProcessMesh( aiScene
				, *submesh.mesh
				, mesh
				, *mesh.createSubmesh() );
		}

		return true;
	}

	void AssimpMeshImporter::doProcessMesh( aiScene const & aiScene
		, aiMesh const & aiMesh
		, castor3d::Mesh & mesh
		, castor3d::Submesh & submesh )
	{
		auto & file = static_cast< AssimpImporterFile & >( *m_file );
		auto & scene = *mesh.getScene();
		auto materialRes = scene.tryFindMaterial( file.getMaterialName( aiMesh.mMaterialIndex ) );
		castor3d::MaterialRPtr material{};

		if ( !materialRes.lock() )
		{
			material = scene.getEngine()->getDefaultMaterial();
		}
		else
		{
			material = &( *materialRes.lock() );
		}

		castor3d::log::info << cuT( "Mesh found: [" ) << file.getInternalName( aiMesh.mName ) << cuT( "]" ) << std::endl;
		submesh.setDefaultMaterial( material );

		auto positions = submesh.createComponent< castor3d::PositionsComponent >();
		auto normals = submesh.createComponent< castor3d::NormalsComponent >();
		castor::Point3fArray tan;
		castor::Point3fArray tex0;
		castor::Point3fArray tex1;
		castor::Point3fArray tex2;
		castor::Point3fArray tex3;
		castor::Point3fArray * tangents = &tan;
		castor::Point3fArray * texcoords0 = &tex0;
		castor::Point3fArray * texcoords1 = &tex1;
		castor::Point3fArray * texcoords2 = &tex2;
		castor::Point3fArray * texcoords3 = &tex3;

		if ( aiMesh.HasTextureCoords( 0u ) )
		{
			auto tanComp = submesh.createComponent< castor3d::TangentsComponent >();
			auto texComp = submesh.createComponent< castor3d::Texcoords0Component >();
			tangents = &tanComp->getData();
			texcoords0 = &texComp->getData();
		}

		if ( aiMesh.HasTextureCoords( 1u ) )
		{
			auto texComp = submesh.createComponent< castor3d::Texcoords1Component >();
			texcoords1 = &texComp->getData();
		}

		if ( aiMesh.HasTextureCoords( 2u ) )
		{
			auto texComp = submesh.createComponent< castor3d::Texcoords2Component >();
			texcoords2 = &texComp->getData();
		}

		if ( aiMesh.HasTextureCoords( 3u ) )
		{
			auto texComp = submesh.createComponent< castor3d::Texcoords3Component >();
			texcoords3 = &texComp->getData();
		}

		createVertexBuffer( aiMesh
			, positions->getData()
			, normals->getData()
			, *tangents
			, *texcoords0
			, *texcoords1
			, *texcoords2
			, *texcoords3 );
		auto & animBuffers = file.addMeshAnimBuffers( &aiMesh
			, gatherMeshAnimBuffers( positions->getData()
				, normals->getData()
				, *tangents
				, *texcoords0
				, *texcoords1
				, *texcoords2
				, *texcoords3
				, castor::makeArrayView( aiMesh.mAnimMeshes, aiMesh.mNumAnimMeshes ) ) );
		auto index = 0u;

		for ( auto & animBuffer : animBuffers )
		{
			applyMorphTarget( float( aiMesh.mAnimMeshes[index]->mWeight )
				, animBuffer
				, positions->getData()
				, normals->getData()
				, *tangents
				, *texcoords0
				, *texcoords1
				, *texcoords2
				, *texcoords3 );
			++index;
		}

		if ( aiMesh.HasBones() )
		{
			std::vector< castor3d::VertexBoneData > bonesData( aiMesh.mNumVertices );
			uint32_t boneIndex{};

			for ( auto aiBone : castor::makeArrayView( aiMesh.mBones, aiMesh.mNumBones ) )
			{
				for ( auto weight : castor::makeArrayView( aiBone->mWeights, aiBone->mNumWeights ) )
				{
					bonesData[weight.mVertexId].addBoneData( boneIndex, weight.mWeight );
				}

				++boneIndex;
			}

			auto bones = submesh.createComponent< castor3d::BonesComponent >();
			bones->addBoneDatas( bonesData );
		}

		auto mapping = submesh.createComponent< castor3d::TriFaceMapping >();
		auto faces = castor::makeArrayView( aiMesh.mFaces, aiMesh.mNumFaces );

		for ( auto face : faces )
		{
			if ( face.mNumIndices == 3 )
			{
				mapping->addFace( face.mIndices[0], face.mIndices[2], face.mIndices[1] );
			}
			else if ( face.mNumIndices == 4 )
			{
				mapping->addQuadFace( face.mIndices[0], face.mIndices[2], face.mIndices[1], face.mIndices[2] );
			}
		}

		if ( !aiMesh.HasNormals() )
		{
			mapping->computeNormals( true );
		}
		else if ( !aiMesh.HasTangentsAndBitangents() )
		{
			mapping->computeTangentsFromNormals();
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
				auto transform = ( submesh->hasComponent( castor3d::BonesComponent::Name )
					? meshes::getTranslation( transformAcc )
					: fromAssimp( transformAcc ) );

				for ( auto & vertex : submesh->getPositions() )
				{
					vertex = transform * vertex;
				}

				auto indexMapping = submesh->getIndexMapping();
				indexMapping->computeNormals( true );
			}
		}

		for ( auto child : castor::makeArrayView( aiNode.mChildren, aiNode.mNumChildren ) )
		{
			doTransformMesh( *child, mesh, transformAcc );
		}
	}
}
