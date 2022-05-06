#pragma GCC diagnostic ignored "-Woverloaded-virtual"
#pragma GCC diagnostic ignored "-Werror=overloaded-virtual"

#include "AssimpImporter/AssimpImporter.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Model/Mesh/Mesh.hpp>
#include <Castor3D/Model/Skeleton/Skeleton.hpp>

namespace c3d_assimp
{
	castor::String const AssimpImporter::Name = cuT( "ASSIMP Importer" );

	AssimpImporter::AssimpImporter( castor3d::Engine & engine )
		: castor3d::MeshImporter{ engine }
		, m_materialsImp{ *this }
		, m_skeletonsImp{ *this }
		, m_meshesImp{ *this, m_materialsImp, m_skeletonsImp, m_meshes }
		, m_scenesImp{ *this, m_skeletonsImp, m_nodes, m_geometries }
	{
	}

	castor3d::MeshImporterUPtr AssimpImporter::create( castor3d::Engine & engine )
	{
		return std::make_unique< AssimpImporter >( engine );
	}

	bool AssimpImporter::doImportSkeleton( castor3d::Skeleton & skeleton )
	{
		bool result{ false };

		if ( auto aiScene = doLoadScene() )
		{
			if ( aiScene->HasMeshes() )
			{
				m_skeletonsImp.import( m_fileName
					, *aiScene
					, *skeleton.getScene()
					, !m_animsOnly );
				result = true;
			}
		}

		return result;
	}

	bool AssimpImporter::doImportMesh( castor3d::Mesh & mesh )
	{
		bool result{ false };

		if ( auto aiScene = doLoadScene() )
		{
			if ( aiScene->HasMeshes() )
			{
				if ( m_animsOnly )
				{
					m_skeletonsImp.import( m_fileName
						, *aiScene
						, *mesh.getScene()
						, false );
					result = true;
				}
				else
				{
					m_materialsImp.import( m_fileName
						, m_textureRemaps
						, *aiScene
						, *mesh.getScene() );
					m_skeletonsImp.import( m_fileName
						, *aiScene
						, *mesh.getScene()
						, true );
					auto indices = m_meshesImp.import( m_fileName
						, *aiScene
						, mesh );
					result = !indices.empty();

					if ( result )
					{
						m_scenesImp.import( *aiScene
							, indices );
					}
				}
			}

			m_importer.FreeScene();
		}

		return result;
	}

	bool AssimpImporter::doImportScene( castor3d::Scene & scene )
	{
		bool result{ false };

		if ( auto aiScene = doLoadScene() )
		{
			if ( m_animsOnly )
			{
				m_skeletonsImp.import( m_fileName
					, *aiScene
					, scene
					, false );

				if ( m_skeletonsImp.needsAnimsReparse() )
				{
					m_scenesImp.importAnims();
				}
			}
			else
			{
				m_materialsImp.import( m_fileName
					, m_textureRemaps
					, *aiScene
					, scene );
				m_skeletonsImp.import( m_fileName
					, *aiScene
					, scene
					, true );
				auto meshes = m_meshesImp.import( m_fileName
					, *aiScene
					, scene );
				m_scenesImp.import( *aiScene
					, scene
					, meshes );
			}

			result = true;
		}
		else
		{
			// The import failed, report it
			castor3d::log::error << "Scene import failed : " << m_importer.GetErrorString() << std::endl;
		}

		return result;
	}

	aiScene const * AssimpImporter::doLoadScene()
	{
		bool noOptim = false;
		auto found = m_parameters.get( "no_optimisations", noOptim );
		uint32_t importFlags{ aiProcess_ValidateDataStructure };

		if ( !found || !noOptim )
		{
			importFlags |= aiProcess_Triangulate
				| aiProcess_JoinIdenticalVertices
				| aiProcess_OptimizeMeshes
				| aiProcess_OptimizeGraph
				| aiProcess_FixInfacingNormals
				| aiProcess_LimitBoneWeights;
		}

		bool tangentSpace = false;
		castor::String normals;
		castor::String prefix;

		if ( m_parameters.get( cuT( "prefix" ), prefix ) )
		{
			m_prefix = prefix + "-";
		}

		if ( m_parameters.get( cuT( "normals" ), normals ) )
		{
			if ( normals == cuT( "smooth" ) )
			{
				importFlags |= aiProcess_GenSmoothNormals;
			}
		}

		if ( m_parameters.get( cuT( "tangent_space" ), tangentSpace ) && tangentSpace )
		{
			importFlags |= aiProcess_CalcTangentSpace;
		}

		// And have it read the given file with some postprocessing
		aiScene const * aiScene;

		try
		{
			aiScene = m_importer.ReadFile( castor::string::stringCast< char >( m_fileName ), importFlags );
		}
		catch ( std::exception & exc )
		{
			castor3d::log::error << exc.what() << std::endl;
			return nullptr;
		}

		return aiScene;
	}
}
