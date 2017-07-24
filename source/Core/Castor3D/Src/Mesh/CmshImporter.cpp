#include "CmshImporter.hpp"

#include "Engine.hpp"

#include "Mesh/Mesh.hpp"
#include "Scene/Scene.hpp"

using namespace Castor;

namespace Castor3D
{
	String const CmshImporter::Type = cuT( "cmsh" );

	CmshImporter::CmshImporter( Engine & engine )
		: Importer{ engine }
	{
	}

	ImporterUPtr CmshImporter::Create( Engine & engine )
	{
		return std::make_unique< CmshImporter >( engine );
	}

	bool CmshImporter::DoImportScene( Scene & p_scene )
	{
		CASTOR_EXCEPTION( "WTF are you trying to do? There is no scene, in a cmsh file." );
	}

	bool CmshImporter::DoImportMesh( Mesh & p_mesh )
	{
		BinaryFile file{ m_fileName, File::OpenMode::eRead };
		return BinaryParser< Mesh >{}.Parse( p_mesh, file );
	}
}
