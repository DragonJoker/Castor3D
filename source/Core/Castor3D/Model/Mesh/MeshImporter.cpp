#include "Castor3D/Model/Mesh/MeshImporter.hpp"

#include "Castor3D/Engine.hpp"

#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/MeshPreparer.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/SubmeshUtils.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/BaseDataComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/SkinComponent.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneImporter.hpp"

CU_ImplementSmartPtr( castor3d, MeshImporter )

namespace castor3d
{
	namespace meshimp
	{
		static bool constexpr displaySkinningDetail = false;

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

		static bool parseMeshImportParameters( Parameters const & parameters
			, castor::Point3f & scale
			, castor::Quaternion & orientation
			, uint32_t & submesh )
		{
			bool needsTransform = parseImportParameters( parameters, scale, orientation );

			if ( uint32_t index = 0;
				parameters.get( cuT( "submesh" ), index ) )
			{
				submesh = index;
			}
			else
			{
				submesh = 0xFFFFFFFFu;
			}

			return needsTransform;
		}
	}

	MeshImporter::MeshImporter( Engine & engine
		, castor::String const & prefix )
		: OwnedBy< Engine >{ engine }
		, m_prefix{ prefix + cuT( " - " ) }
	{
	}

	MeshRes MeshImporter::importData( castor::String const & name
		, Scene & scene
		, ImporterFile * file
		, Parameters const & parameters
		, bool forceImport )
	{
		if ( !m_file )
		{
			m_file = file;
		}

		auto result = doCreateMesh( name, scene );

		if ( !result
			|| !importData( *result, file, parameters, forceImport ) )
		{
			return nullptr;
		}

		return result;
	}

	bool MeshImporter::importData( Mesh & mesh
		, ImporterFile * file
		, Parameters const & parameters
		, bool forceImport )
	{
		m_file = file;
		m_parameters = parameters;
		bool result = true;

		if ( !mesh.getSubmeshCount() || forceImport )
		{
			castor::Point3f scale{ 1.0f, 1.0f, 1.0f };
			castor::Quaternion orientation{ castor::Quaternion::identity() };
			uint32_t submeshIndex{};
			auto needsTransform = meshimp::parseMeshImportParameters( m_parameters, scale, orientation, submeshIndex );

			log::info << getPrefix() << cuT( "Loading Mesh [" ) << mesh.getName() << cuT( "]" ) << std::endl;
			result = doImportMesh( mesh, submeshIndex );

			if ( result )
			{
				if ( needsTransform )
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

				if constexpr ( meshimp::displaySkinningDetail )
				{
					log::debug << cuT( "Mesh [" ) << mesh.getName() << cuT( "]" )
						<< cuT( " AABB (" ) << print( mesh.getBoundingBox() ) << cuT( ")" )
						<< cuT( ", " ) << mesh.getVertexCount() << cuT( " vertices" )
						<< cuT( ", " ) << mesh.getFaceCount() << cuT( " faces" )
						<< cuT( ", " ) << mesh.getSubmeshCount() << cuT( " submeshes" ) << std::endl;

					for ( auto & submesh : mesh )
					{
						log::debug << "  Submesh " << submesh->getId() << std::endl;

						if ( auto skin = submesh->getComponent< SkinComponent >() )
						{
							for ( auto & s : skin->getData().getData() )
							{
								log::debug << "    " << s.m_ids[0] << " " << s.m_ids[1] << " " << s.m_ids[2] << " " << s.m_ids[3]
									<< "    " << s.m_weights[0] << " " << s.m_weights[1] << " " << s.m_weights[2] << " " << s.m_weights[3]
									<< std::endl;
							}
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
			for ( auto & submesh : mesh )
			{
				submesh->instantiate( {}, submesh->getDefaultMaterial(), false );
			}
		}

		return result;
	}

	bool MeshImporter::importData( Mesh & mesh
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
			return importer->importData( mesh
				, file.get()
				, parameters
				, forceImport );
		}

		return false;
	}

	MeshRes MeshImporter::doCreateMesh( castor::String const & name, Scene & scene )
	{
		return scene.createMesh( name, scene );
	}
}
