#include "ImporterFactory.hpp"

#include "CmshImporter.hpp"

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
