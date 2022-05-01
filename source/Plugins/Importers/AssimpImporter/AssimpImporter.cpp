#pragma GCC diagnostic ignored "-Woverloaded-virtual"
#pragma GCC diagnostic ignored "-Werror=overloaded-virtual"

#include "AssimpImporter/AssimpImporter.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Model/Mesh/Mesh.hpp>

namespace c3d_assimp
{
	castor::String const AssimpImporter::Name = cuT( "ASSIMP Importer" );

	AssimpImporter::AssimpImporter( castor3d::Engine & engine )
		: castor3d::MeshImporter{ engine }
		, m_materialsImp{ *this }
		, m_skeletonsImp{ *this }
		, m_meshesImp{ *this, m_materialsImp, m_skeletonsImp, m_meshes }
		, m_scenesImp{ *this, m_skeletonsImp, m_meshesImp, m_nodes, m_geometries }
	{
	}

	castor3d::MeshImporterUPtr AssimpImporter::create( castor3d::Engine & engine )
	{
		return std::make_unique< AssimpImporter >( engine );
	}

	bool AssimpImporter::doImportMesh( castor3d::Mesh & mesh )
	{
		bool result{ false };

		if ( auto aiScene = doLoadScene() )
		{
			if ( aiScene->HasMeshes() )
			{
				m_materialsImp.import( m_prefix
					, m_fileName
					, m_textureRemaps
					, *aiScene
					, *mesh.getScene() );
				m_skeletonsImp.import( m_prefix
					, m_fileName
					, m_importFlags
					, *aiScene
					, *mesh.getScene() );
				auto indices = m_meshesImp.import( m_prefix
					, m_fileName
					, *aiScene
					, mesh );
				result = !indices.empty();

				if ( result )
				{
					m_scenesImp.import( m_prefix
						, m_fileName
						, *aiScene
						, indices );
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
			m_materialsImp.import( m_prefix
				, m_fileName
				, m_textureRemaps
				, *aiScene
				, scene );
			m_skeletonsImp.import( m_prefix
				, m_fileName
				, m_importFlags
				, *aiScene
				, scene );
			auto meshes = m_meshesImp.import( m_prefix
				, m_fileName
				, *aiScene
				, scene );
			m_scenesImp.import( m_prefix
				, m_fileName
				, *aiScene
				, scene
				, meshes );
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

		if ( !found || !noOptim )
		{
			m_importFlags = aiProcess_Triangulate
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
				m_importFlags |= aiProcess_GenSmoothNormals;
			}
		}

		if ( m_parameters.get( cuT( "tangent_space" ), tangentSpace ) && tangentSpace )
		{
			m_importFlags |= aiProcess_CalcTangentSpace;
		}

		// And have it read the given file with some postprocessing
		aiScene const * aiScene;

		try
		{
			aiScene = m_importer.ReadFile( castor::string::stringCast< char >( m_fileName ), m_importFlags );
		}
		catch ( std::exception & exc )
		{
			castor3d::log::error << exc.what() << std::endl;
			return nullptr;
		}

		return aiScene;
	}
}
