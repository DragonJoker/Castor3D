/*
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
	class ImporterPlugin
		: public PluginBase
	{
	public:
		typedef std::pair< Castor::String, Castor::String > Extension;
		DECLARE_VECTOR( Extension, Extension );

	private:
		friend class PluginBase;
		friend class Engine;
		typedef void CreateImporterFunction( Engine * p_engine, ImporterPlugin * p_pPlugin );
		typedef void DestroyImporterFunction( ImporterPlugin * p_pPlugin );
		typedef ExtensionArray GetExtensionFunction( Engine * p_engine );

		typedef CreateImporterFunction * PCreateImporterFunction;
		typedef DestroyImporterFunction * PDestroyImporterFunction;
		typedef GetExtensionFunction * PGetExtensionFunction;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pLibrary	The shared library holding the plugin
		 *\param[in]	p_engine	The engine
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pLibrary	La librairie partagée contenant le plugin
		 *\param[in]	p_engine	Le moteur
		 */
		C3D_API ImporterPlugin( Castor::DynamicLibrarySPtr p_pLibrary, Engine * p_engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~ImporterPlugin();
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
		C3D_API ExtensionArray GetExtensions();

	private:
		PCreateImporterFunction m_pfnCreateImporter;
		PDestroyImporterFunction m_pfnDestroyImporter;
		PGetExtensionFunction m_pfnGetExtension;
		ImporterSPtr m_pImporter;
	};
}

#endif
