﻿/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_IMPORTER_PLUGIN_H___
#define ___C3D_IMPORTER_PLUGIN_H___

#include "Plugin.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Importer Plugin class
	\~french
	\brief		Classe de plugin d'import
	*/
	class C3D_API ImporterPlugin
		: public PluginBase
	{
	public:
		typedef std::pair< Castor::String, Castor::String > Extension;
		DECLARE_VECTOR( Extension, Extension );

	private:
		friend class PluginBase;
		friend class Engine;
		typedef void			CreateImporterFunction( Engine * p_pEngine, ImporterPlugin * p_pPlugin );
		typedef void			DestroyImporterFunction( ImporterPlugin * p_pPlugin );
		typedef ExtensionArray	GetExtensionFunction();

		typedef CreateImporterFunction	*	PCreateImporterFunction;
		typedef DestroyImporterFunction	*	PDestroyImporterFunction;
		typedef GetExtensionFunction	*	PGetExtensionFunction;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pEngine		The core engine
		 *\param[in]	p_pLibrary		The shared library holding the plugin
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pEngine		Le moteur
		 *\param[in]	p_pLibrary		La librairie partagée contenant le plugin
		 */
		ImporterPlugin( Engine * p_pEngine, Castor::DynamicLibrarySPtr p_pLibrary );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~ImporterPlugin();
		/**
		 *\~english
		 *\brief		Attaches the plugin to the given Importer
		 *\param[in]	p_pImporter	The Importer
		 *\~french
		 *\brief		Attache le plugin à l'Importer donné
		 *\param[in]	p_pImporter	L'Importer
		 */
		inline void AttachImporter( ImporterSPtr p_pImporter )
		{
			m_pImporter = p_pImporter;
		}
		/**
		 *\~english
		 *\brief		Detaches the plugin from the attached Importer
		 *\return		The attached Importer
		 *\~french
		 *\brief		Detache le plugin de l'Importer attaché
		 *\return		L'Importer attaché
		 */
		inline void DetachImporter()
		{
			m_pImporter.reset();
		}
		/**
		 *\~english
		 *\brief		Retrieves the attached Importer
		 *\return		The attached Importer
		 *\~french
		 *\brief		Récupère l'Importer attaché
		 *\return		L'Importer attaché
		 */
		inline ImporterSPtr GetImporter()
		{
			return m_pImporter;
		}
		/**
		 *\~english
		 *\brief		Retrieves the supported file extensions array
		 *\return		The supported extensions array
		 *\~french
		 *\brief		Récupère le tableau des extensions supportées
		 *\return		Le tableau d'extensions supportées
		 */
		ExtensionArray GetExtensions();

	private:
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\param[in]	p_plugin	The Plugin object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_plugin	L'objet Plugin à copier
		 */
		ImporterPlugin( ImporterPlugin const & p_plugin );
		/**
		 *\~english
		 *\brief		Move constructor
		 *\param[in]	p_plugin	The Plugin object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_plugin	L'objet Plugin à déplacer
		 */
		ImporterPlugin( ImporterPlugin && p_plugin );
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_plugin	The Plugin object to copy
		 *\return		A reference to this Plugin object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_plugin	L'objet Plugin à copier
		 *\return		Une référence sur cet objet Plugin
		 */
		ImporterPlugin & operator =( ImporterPlugin const & p_plugin );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_plugin	The Plugin object to move
		 *\return		A reference to this Plugin object
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement
		 *\param[in]	p_plugin	L'objet Plugin à déplacer
		 *\return		Une référence sur cet objet Plugin
		 */
		ImporterPlugin & operator =( ImporterPlugin && p_plugin );

	private:
		PCreateImporterFunction m_pfnCreateImporter;
		PDestroyImporterFunction m_pfnDestroyImporter;
		PGetExtensionFunction m_pfnGetExtension;
		ImporterSPtr m_pImporter;
	};
}

#endif

