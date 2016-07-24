#include "AseImporter.hpp"
#include "AseFileParser.hpp"

#include <Engine.hpp>
#include <Event/Frame/InitialiseEvent.hpp>
#include <Mesh/Mesh.hpp>
#include <Mesh/Submesh.hpp>
#include <Scene/Geometry.hpp>
#include <Scene/Scene.hpp>

using namespace Castor3D;
using namespace Castor;

namespace Ase
{
	AseImporter::AseImporter( Engine & p_engine )
		: Importer{ p_engine }
	{
	}

	AseImporter::~AseImporter()
	{
	}

	ImporterUPtr AseImporter::Create( Engine & p_engine )
	{
		return std::make_unique< AseImporter >( p_engine );
	}

	bool AseImporter::DoImportScene( Scene & p_scene )
	{
		return AseFileParser{ *this, p_scene }.ParseFile( m_fileName );
	}

	bool AseImporter::DoImportMesh( Mesh & p_mesh )
	{
		return AseFileParser{ *this, *p_mesh.GetScene(), p_mesh }.ParseFile( m_fileName );
	}
}
