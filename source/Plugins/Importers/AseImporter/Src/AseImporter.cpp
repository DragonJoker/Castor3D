#include "AseImporter.hpp"
#include "AseFileParser.hpp"

#include <Engine.hpp>
#include <Event/Frame/InitialiseEvent.hpp>
#include <Mesh/Mesh.hpp>
#include <Mesh/Submesh.hpp>
#include <Scene/Geometry.hpp>
#include <Scene/Scene.hpp>

using namespace castor3d;
using namespace castor;

namespace Ase
{
	AseImporter::AseImporter( Engine & p_engine )
		: Importer{ p_engine }
	{
	}

	AseImporter::~AseImporter()
	{
	}

	ImporterUPtr AseImporter::create( Engine & p_engine )
	{
		return std::make_unique< AseImporter >( p_engine );
	}

	bool AseImporter::doImportScene( Scene & p_scene )
	{
		return AseFileParser{ *this, p_scene }.parseFile( m_fileName );
	}

	bool AseImporter::doImportMesh( Mesh & p_mesh )
	{
		return AseFileParser{ *this, *p_mesh.getScene(), p_mesh }.parseFile( m_fileName );
	}
}
