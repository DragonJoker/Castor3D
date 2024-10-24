#include "Castor3D/Model/Mesh/MeshImporter.hpp"

#include "Castor3D/Engine.hpp"

#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/MeshPreparer.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/SubmeshUtils.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/BaseDataComponent.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneImporter.hpp"

CU_ImplementSmartPtr( castor3d, MeshImporter )

namespace castor3d
{
	namespace meshimp
	{
		static void transformMesh( castor::Matrix4x4f const & transform
			, Mesh & mesh )
		{
			for ( auto & submesh : mesh )
			{
				for ( auto & vertex : submesh->getPositions() )
				{
					vertex = transform * vertex;
				}

				SubmeshUtils::computeNormals( submesh->getPositions()
					, submesh->getNormals()
					, static_cast< TriFaceMapping const & >( *submesh->getIndexMapping() ).getData().getFaces() );

				static castor::Point4fArray tan;
				static castor::Point3fArray tex;
				castor::Point4fArray * tangents = &tan;
				castor::Point3fArray const * texcoords = &tex;

				if ( auto tanComp = submesh->getComponent< TangentsComponent >() )
				{
					tangents = &tanComp->getData().getData();
				}

				if ( auto texComp = submesh->getComponent< Texcoords0Component >() )
				{
					texcoords = &texComp->getData().getData();
				}

				SubmeshUtils::computeTangentsFromNormals( submesh->getPositions()
					, *texcoords
					, submesh->getNormals()
					, *tangents
					, static_cast< TriFaceMapping const & >( *submesh->getIndexMapping() ).getData().getFaces() );
			}
		}
	}

	MeshImporter::MeshImporter( Engine & engine
		, castor::String const & prefix )
		: OwnedBy< Engine >{ engine }
		, m_prefix{ prefix + cuT( " - " ) }
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
			log::info << getPrefix() << cuT( "Loading Mesh [" ) << mesh.getName() << cuT( "]" ) << std::endl;
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

				if ( bool invertNormals{};
					parameters.get( cuT( "invert_normals" ), invertNormals ) && invertNormals )
				{
					for ( auto & submesh : mesh )
					{
						for ( auto & n : submesh->getNormals() )
						{
							n = -n;
						}
					}
				}

				bool noOptim = false;

				if ( auto found = parameters.get( cuT( "no_optimisations" ), noOptim );
					!found || !noOptim )
				{
					MeshPreparer::prepare( mesh, parameters );
				}

				mesh.computeContainers();
				log::info << getPrefix() << cuT( "Loaded Mesh [" ) << mesh.getName() << cuT( "]" )
					<< cuT( " AABB (" ) << print( mesh.getBoundingBox() ) << cuT( ")" )
					<< cuT( ", " ) << mesh.getVertexCount() << cuT( " vertices" )
					<< cuT( ", " ) << mesh.getFaceCount() << cuT( " faces" )
					<< cuT( ", " ) << mesh.getSubmeshCount() << cuT( " submeshes" ) << std::endl;
			}
			else
			{
				log::info << getPrefix() << cuT( "Couldn't load Mesh [" ) << mesh.getName() << cuT( "]" ) << std::endl;
			}
		}
		else
		{
			for ( auto & submesh : mesh )
			{
				submesh->instantiate( {}, submesh->getDefaultMaterial(), false );
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

		castor::String preferredImporter = cuT( "any" );
		parameters.get( cuT( "preferred_importer" ), preferredImporter );
		auto file = engine.getImporterFileFactory().create( extension
			, preferredImporter
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
