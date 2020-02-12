#include "Castor3D/Model/Mesh/ImporterFactory.hpp"

#include "Castor3D/Binary/CmshImporter.hpp"

using namespace castor;

namespace castor3d
{
	MeshImporterFactory::MeshImporterFactory()
		: Factory< MeshImporter, String, MeshImporterUPtr, std::function< MeshImporterUPtr( Engine & ) > >()
	{
		registerType( CmshImporter::Type, CmshImporter::create );
	}

	MeshImporterFactory::~MeshImporterFactory()
	{
	}
}
