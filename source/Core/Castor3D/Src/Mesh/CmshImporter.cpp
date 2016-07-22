#include "CmshImporter.hpp"

#include "Engine.hpp"
#include "MeshCache.hpp"

#include "Event/Frame/InitialiseEvent.hpp"
#include "Mesh/Mesh.hpp"
#include "Scene/Scene.hpp"

#include <Data/BinaryFile.hpp>

using namespace Castor;

namespace Castor3D
{
	CmshImporter::CmshImporter( Engine & p_engine )
		: Importer{ p_engine }
	{
	}

	SceneSPtr CmshImporter::DoImportScene()
	{
		CASTOR_EXCEPTION( "WTF are you trying to do? There is no scene, in a cmsh file." );
	}

	MeshSPtr CmshImporter::DoImportMesh( Scene & p_scene )
	{
		String l_name = m_fileName.GetFileName();
		String l_meshName = l_name.substr( 0, l_name.find_last_of( '.' ) );
		MeshSPtr l_mesh;

		if ( p_scene.GetMeshCache().Has( l_meshName ) )
		{
			l_mesh = p_scene.GetMeshCache().Find( l_meshName );
		}
		else
		{
			l_mesh = p_scene.GetMeshCache().Add( l_meshName, eMESH_TYPE_CUSTOM, UIntArray{}, RealArray{} );
		}

		if ( !l_mesh->GetSubmeshCount() )
		{
			BinaryFile l_file{ m_fileName, File::eOPEN_MODE_READ };

			if ( !BinaryParser< Mesh >{}.Parse( *l_mesh, l_file ) )
			{
				p_scene.GetMeshCache().Remove( l_meshName );
			}
		}

		return l_mesh;
	}
}
