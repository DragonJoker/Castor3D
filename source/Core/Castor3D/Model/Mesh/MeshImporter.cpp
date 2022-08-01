#include "Castor3D/Model/Mesh/MeshImporter.hpp"

#include "Castor3D/Engine.hpp"

#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/MeshPreparer.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/SubmeshUtils.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/BaseDataComponent.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneImporter.hpp"

namespace castor3d
{
	namespace meshimp
	{
		static void transformMesh( castor::Matrix4x4f const & transform
			, Mesh & mesh )
		{
			for ( auto submesh : mesh )
			{
				for ( auto & vertex : submesh->getPositions() )
				{
					vertex = transform * vertex;
				}

				static castor::Point3fArray tan;
				static castor::Point3fArray tex;
				castor::Point3fArray * tangents = &tan;
				castor::Point3fArray const * texcoords = &tex;

				if ( auto tanComp = submesh->getComponent< TangentsComponent >() )
				{
					tangents = &tanComp->getData();
				}

				if ( auto texComp = submesh->getComponent< Texcoords0Component >() )
				{
					texcoords = &texComp->getData();
				}

				SubmeshUtils::computeNormals( submesh->getPositions()
					, *texcoords
					, submesh->getNormals()
					, *tangents
					, static_cast< TriFaceMapping const & >( *submesh->getIndexMapping() ).getFaces() );
			}
		}
	}

	MeshImporter::MeshImporter( Engine & engine )
		: OwnedBy< Engine >{ engine }
	{
	}

	bool MeshImporter::import( Mesh & mesh
		, ImporterFile * file
		, Parameters const & parameters
		, bool forceImport )
	{
		m_file = file;
		m_parameters = parameters;
		bool result = true;

		if ( !mesh.getSubmeshCount() || forceImport )
		{
			result = doImportMesh( mesh );

			if ( result )
			{
				castor::Point3f scale{ 1.0f, 1.0f, 1.0f };
				castor::Quaternion orientation{ castor::Quaternion::identity() };

				if ( parseImportParameters( m_parameters, scale, orientation ) )
				{
					castor::Matrix4x4f transform;
					castor::matrix::setRotate( transform, orientation );
					castor::matrix::scale( transform, scale );
					meshimp::transformMesh( transform, mesh );
				}

				bool noOptim = false;
				auto found = parameters.get( "no_optimisations", noOptim );

				if ( !found || !noOptim )
				{
					MeshPreparer::prepare( mesh, parameters );
				}

				mesh.computeContainers();
				log::info << "Loaded mesh [" << mesh.getName() << "]"
					<< " AABB (" << print( mesh.getBoundingBox() ) << ")"
					<< ", " << mesh.getVertexCount() << " vertices"
					<< ", " << mesh.getFaceCount() << " faces"
					<< ", " << mesh.getSubmeshCount() << " submeshes" << std::endl;
			}
		}
		else
		{
			for ( auto submesh : mesh )
			{
				submesh->instantiate( nullptr, {}, submesh->getDefaultMaterial(), false );
			}
		}

		return result;
	}

	bool MeshImporter::import( Mesh & mesh
		, castor::Path const & path
		, Parameters const & parameters
		, bool forceImport )
	{
		auto & engine = *mesh.getEngine();
		auto extension = castor::string::lowerCase( path.getExtension() );

		if ( !engine.getImporterFileFactory().isTypeRegistered( extension ) )
		{
			log::error << cuT( "Importer for [" ) << extension << cuT( "] files is not registered, make sure you've got the matching plug-in installed." );
			return false;
		}

		auto file = engine.getImporterFileFactory().create( extension
			, engine
			, path
			, parameters );

		if ( auto importer = file->createMeshImporter() )
		{
			return importer->import( mesh
				, file.get()
				, parameters
				, forceImport );
		}

		return false;
	}
}
