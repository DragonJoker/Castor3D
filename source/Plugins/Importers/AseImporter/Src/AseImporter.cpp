#include "AseImporter.hpp"
#include "AseFileParser.hpp"

#include <Engine.hpp>
#include <Geometry.hpp>
#include <Mesh.hpp>
#include <SceneManager.hpp>
#include <Submesh.hpp>

using namespace Castor3D;
using namespace Castor;

namespace Ase
{
	AseImporter::AseImporter( Engine & p_pEngine )
		:	Importer( p_pEngine )
	{
	}

	AseImporter::~AseImporter()
	{
	}

	SceneSPtr AseImporter::DoImportScene()
	{
		SceneSPtr l_scene = GetEngine()->GetSceneManager().Create( cuT( "Scene_ASE" ), *GetEngine() );
		m_pFileParser = new AseFileParser( GetEngine() );
		m_pFileParser->ParseFile( m_fileName, l_scene );
		MeshSPtr l_mesh;

		for ( auto && l_it : m_geometries )
		{
			l_mesh = l_it.second->GetMesh();
			l_mesh->ComputeContainers();

			for ( auto && l_submesh : *l_mesh )
			{
				l_submesh->GenerateBuffers();
			}

			l_mesh->ComputeNormals();
		}

		delete m_pFileParser;
		return l_scene;
	}

	MeshSPtr AseImporter::DoImportMesh()
	{
		m_pFileParser = new AseFileParser( GetEngine() );
		m_pFileParser->ParseFile( m_fileName );
		MeshSPtr l_mesh = m_pFileParser->GetMesh();

		if ( l_mesh )
		{
			l_mesh->ComputeContainers();
			l_mesh->ComputeNormals();
		}

		delete m_pFileParser;
		return l_mesh;
	}
}
