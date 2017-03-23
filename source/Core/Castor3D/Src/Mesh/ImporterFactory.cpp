#include "ImporterFactory.hpp"

#include "CmshImporter.hpp"

using namespace Castor;

namespace Castor3D
{
	ImporterFactory::ImporterFactory()
		: Factory< Importer, String, ImporterUPtr, std::function< ImporterUPtr( Engine & ) > >()
	{
		Register( CmshImporter::Type, CmshImporter::Create );
	}

	ImporterFactory::~ImporterFactory()
	{
	}
}
