/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MeshImporterFactory_H___
#define ___C3D_MeshImporterFactory_H___

#include "MeshModule.hpp"

#include <CastorUtils/Design/Factory.hpp>

namespace castor3d
{
	class MeshImporterFactory
		: public castor::Factory< MeshImporter, castor::String, MeshImporterUPtr, std::function< MeshImporterUPtr( Engine & ) > >
	{
	public:
		/**
		*\~english
		*\brief		Constructor
		*\~french
		*\brief		Constructeur
		*/
		C3D_API MeshImporterFactory();
		/**
		*\~english
		*\brief		Destructor
		*\~french
		*\brief		Destructeur
		*/
		C3D_API ~MeshImporterFactory();
	};
}

#endif
