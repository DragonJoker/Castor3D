#include "Castor3D/Model/Mesh/MeshImporter.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/ImporterFile.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/MeshPreparer.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/SubmeshUtils.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/BaseDataComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/SkinComponent.hpp"
#include "Castor3D/Scene/Scene.hpp"

CU_ImplementSmartPtr( c3d, MeshImporter )

namespace c3d
{
	namespace meshimp
	{
		static bool constexpr displaySkinningDetail = false;

		static void transformMesh( Matrix4x4f const & transform
			, Mesh const & mesh )
		{
			for ( auto const & submesh : mesh )
			{
				for ( auto & vertex : submesh->getPositions() )
					vertex = transform * vertex;

				SubmeshUtils::computeNormals( submesh->getPositions()
					, submesh->getNormals()
					, static_cast< TriFaceMapping const & >( *submesh->getIndexMapping() ).getData().getFaces() );

				static Point4fArray tan;
				static Point3fArray tex;
				Point4fArray * tangents = &tan;
				Point3fArray const * texcoords = &tex;

				if ( auto tanComp = submesh->getComponent< TangentsComponent >() )
					tangents = &tanComp->getData().getData();
				if ( auto texComp = submesh->getComponent< Texcoords0Component >() )
					texcoords = &texComp->getData().getData();

				SubmeshUtils::computeTangentsFromNormals( submesh->getPositions()
					, *texcoords
					, submesh->getNormals()
					, *tangents
					, static_cast< TriFaceMapping const & >( *submesh->getIndexMapping() ).getData().getFaces() );
			}
		}
	}

	MeshImporter::MeshImporter( Engine & engine
		, String const & prefix
		, MeshImportConfig const & config )
		: OwnedBy< Engine >{ engine }
		, m_prefix{ prefix + cuT( " - " ) }
		, m_config{ config }
	{
	}

	MeshRes MeshImporter::importData( String const & name
		, Scene & scene
		, ImporterFile * file
		, bool forceImport )
	{
		if ( !m_file )
			m_file = file;

		auto result = doCreateMesh( name, scene );
		if ( !result
			|| !importData( *result, file, forceImport ) )
		{
			return nullptr;
		}

		return result;
	}

	bool MeshImporter::importData( Mesh & mesh
		, ImporterFile * file
		, bool forceImport )
	{
		m_file = file;
		bool result = true;

		if ( !mesh.getSubmeshCount() || forceImport )
		{
			Point3f scale{ 1.0f, 1.0f, 1.0f };
			Quaternion orientation{ Quaternion::identity() };
			bool needsTransform = parseImportParameters( m_file->getConfig(), scale, orientation);
			log::info << getPrefix() << cuT( "Loading Mesh [" ) << mesh.getName() << cuT( "]" ) << std::endl;
			result = doImportMesh( mesh, m_config.submeshIndex );

			if ( result )
			{
				if ( needsTransform )
				{
					Matrix4x4f transform;
					matrix::setRotate( transform, orientation );
					matrix::scale( transform, scale );
					meshimp::transformMesh( transform, mesh );
				}

				if ( m_config.invertNormals )
				{
					for ( auto const & submesh : mesh )
					{
						for ( auto & n : submesh->getNormals() )
							n = -n;
					}
				}

				if ( !m_file->getConfig().disableOptimisations )
					MeshPreparer::prepare( mesh, m_file->getConfig().parameters );

				mesh.computeContainers();
				log::info << getPrefix() << cuT( "Loaded Mesh [" ) << mesh.getName() << cuT( "]" )
					<< cuT( " AABB (" ) << print( mesh.getBoundingBox() ) << cuT( ")" )
					<< cuT( ", " ) << mesh.getVertexCount() << cuT( " vertices" )
					<< cuT( ", " ) << mesh.getFaceCount() << cuT( " faces" )
					<< cuT( ", " ) << mesh.getSubmeshCount() << cuT( " submeshes" ) << std::endl;

				if constexpr ( meshimp::displaySkinningDetail )
				{
					log::debug << cuT( "Mesh [" ) << mesh.getName() << cuT( "]" )
						<< cuT( " AABB (" ) << print( mesh.getBoundingBox() ) << cuT( ")" )
						<< cuT( ", " ) << mesh.getVertexCount() << cuT( " vertices" )
						<< cuT( ", " ) << mesh.getFaceCount() << cuT( " faces" )
						<< cuT( ", " ) << mesh.getSubmeshCount() << cuT( " submeshes" ) << std::endl;

					for ( auto const & submesh : mesh )
					{
						log::debug << "  Submesh " << submesh->getId() << std::endl;

						if ( auto skin = submesh->getComponent< SkinComponent >() )
						{
							for ( auto const & s : skin->getData().getData() )
								log::debug << "    " << s.m_ids[0] << " " << s.m_ids[1] << " " << s.m_ids[2] << " " << s.m_ids[3]
									<< "    " << s.m_weights[0] << " " << s.m_weights[1] << " " << s.m_weights[2] << " " << s.m_weights[3]
									<< std::endl;
						}
					}
				}
			}
			else
			{
				log::info << getPrefix() << cuT( "Couldn't load Mesh [" ) << mesh.getName() << cuT( "]" ) << std::endl;
			}
		}
		else
		{
			for ( auto const & submesh : mesh )
				submesh->instantiate( {}, submesh->getDefaultMaterial(), false );
		}

		return result;
	}

	bool MeshImporter::importData( Mesh & mesh
		, Path const & path
		, ImportConfig const & config
		, bool forceImport )
	{
		auto & engine = *mesh.getEngine();
		auto extension = string::lowerCase( path.getExtension() );

		if ( !engine.getImporterFileFactory().isTypeRegistered( extension ) )
		{
			log::error << cuT( "Importer for [" ) << extension << cuT( "] files is not registered, make sure you've got the matching plug-in installed." );
			return false;
		}

		auto file = engine.getImporterFileFactory().create( extension
			, engine
			, path
			, config );

		if ( auto importer = file->createMeshImporter() )
		{
			return importer->importData( mesh
				, file.get()
				, forceImport );
		}

		return false;
	}

	MeshRes MeshImporter::doCreateMesh( String const & name, Scene & scene )
	{
		return scene.createMesh( name, scene );
	}
}
