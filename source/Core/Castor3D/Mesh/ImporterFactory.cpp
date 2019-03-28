#include "Castor3D/Mesh/ImporterFactory.hpp"

#include "Castor3D/Mesh/CmshImporter.hpp"

using namespace castor;

namespace castor3d
{
	ImporterFactory::ImporterFactory()
		: Factory< Importer, String, ImporterUPtr, std::function< ImporterUPtr( Engine & ) > >()
	{
		registerType( CmshImporter::Type, CmshImporter::create );
	}

	ImporterFactory::~ImporterFactory()
	{
	}
}
