#include "CmshImporter.hpp"

#include "Engine.hpp"

#include "Mesh/Mesh.hpp"
#include "Scene/Scene.hpp"

using namespace castor;

namespace castor3d
{
	String const CmshImporter::Type = cuT( "cmsh" );

	CmshImporter::CmshImporter( Engine & engine )
		: Importer{ engine }
	{
	}

	ImporterUPtr CmshImporter::create( Engine & engine )
	{
		return std::make_unique< CmshImporter >( engine );
	}

	bool CmshImporter::doImportScene( Scene & p_scene )
	{
		CASTOR_EXCEPTION( "WTF are you trying to do? There is no scene, in a cmsh file." );
	}

	bool CmshImporter::doImportMesh( Mesh & p_mesh )
	{
		BinaryFile file{ m_fileName, File::OpenMode::eRead };
		return BinaryParser< Mesh >{}.parse( p_mesh, file );
	}
}
