#include "AseImporter.hpp"
#include "AseFileParser.hpp"

#include <Engine.hpp>
#include <MeshManager.hpp>
#include <SceneManager.hpp>

#include <Mesh/Submesh.hpp>
#include <Scene/Geometry.hpp>

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
		m_pFileParser = new AseFileParser( GetEngine(), *this, *l_scene );
		m_pFileParser->ParseFile( m_fileName );
		MeshSPtr l_mesh;

		for ( auto && l_it : m_geometries )
		{
			l_mesh = l_it.second->GetMesh();
			l_mesh->ComputeContainers();

			for ( auto && l_submesh : *l_mesh )
			{
				GetEngine()->PostEvent( MakeInitialiseEvent( *l_submesh ) );
			}

			l_mesh->ComputeNormals();
		}

		delete m_pFileParser;
		return l_scene;
	}

	MeshSPtr AseImporter::DoImportMesh( Castor3D::Scene & p_scene )
	{
		m_pFileParser = new AseFileParser( GetEngine(), *this, p_scene );
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
