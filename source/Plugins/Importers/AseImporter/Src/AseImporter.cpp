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
		SceneSPtr l_pScene = GetOwner()->GetSceneManager().Create( cuT( "Scene_ASE" ), *GetOwner(), cuT( "Scene_ASE" ) );
		m_pFileParser = new AseFileParser( GetOwner() );
		m_pFileParser->ParseFile( m_fileName, l_pScene );
		MeshSPtr l_pMesh;

		for ( auto && l_it : m_geometries )
		{
			l_pMesh = l_it.second->GetMesh();
			l_pMesh->ComputeContainers();

			for ( auto && l_submesh : *l_pMesh )
			{
				l_submesh->GenerateBuffers();
			}

			l_pMesh->ComputeNormals();
		}

		delete m_pFileParser;
		return l_pScene;
	}

	MeshSPtr AseImporter::DoImportMesh()
	{
		m_pFileParser = new AseFileParser( GetOwner() );
		m_pFileParser->ParseFile( m_fileName );
		MeshSPtr l_pMesh = m_pFileParser->GetMesh();

		if ( l_pMesh )
		{
			l_pMesh->ComputeContainers();
			l_pMesh->ComputeNormals();
		}

		delete m_pFileParser;
		return l_pMesh;
	}
}
