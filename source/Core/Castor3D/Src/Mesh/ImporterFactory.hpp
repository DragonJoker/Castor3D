/*
See LICENSE file in root folder
*/
#ifndef ___C3D_IMPORTER_FACTORY_H___
#define ___C3D_IMPORTER_FACTORY_H___

#include "Importer.hpp"

#include <Design/Factory.hpp>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date		22/07/2016
	\~english
	\brief		The importer factory.
	\~french
	\brief		La fabrique d'importeurs.
	*/
	class ImporterFactory
		: public castor::Factory< Importer, castor::String, ImporterUPtr, std::function< ImporterUPtr( Engine & ) > >
	{
	public:
		/**
		*\~english
		*\brief		Constructor
		*\~french
		*\brief		Constructeur
		*/
		C3D_API ImporterFactory();
		/**
		*\~english
		*\brief		Destructor
		*\~french
		*\brief		Destructeur
		*/
		C3D_API ~ImporterFactory();
	};
}

#endif
