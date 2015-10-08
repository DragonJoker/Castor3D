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
#ifndef ___C3D_PLUGIN_H___
#define ___C3D_PLUGIN_H___

#include "Castor3DPrerequisites.hpp"
#include "PluginException.hpp"
#include <NonCopyable.hpp>

#include <OwnedBy.hpp>

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Plugin Base class
	\remark		Manages the base plugin functions, allows plugins to check versions and to register themselves
	\~french
	\brief		Classe de base des plugins
	\remark		Gère les fonctions de base d'un plugin, permet aux plugins de faire des vérifications de version et  de s'enregistrer auprès du moteur
	*/
	class C3D_API PluginBase
		: public Castor::NonCopyable
		, public Castor::OwnedBy< Engine >
	{
	private:
		//!< Signature for the plugin's loading function
		typedef void OnLoadFunction( Engine * );
		//!< Signature for the plugin's unloading function
		typedef void OnUnloadFunction( Engine * );
		//!< Signature for the plugin's type retrieval function
		typedef ePLUGIN_TYPE GetTypeFunction();
		//!< Signature for the plugin's version checking function
		typedef void GetRequiredVersionFunction( Version & p_version );
		//!< Signature for the plugin's name retrieval function
		typedef Castor::String GetNameFunction();

	public:
		typedef OnLoadFunction * POnLoadFunction;
		typedef OnUnloadFunction * POnUnloadFunction;
		typedef GetTypeFunction * PGetTypeFunction;
		typedef GetRequiredVersionFunction * PGetRequiredVersionFunction;
		typedef GetNameFunction * PGetNameFunction;

	protected:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_type		The plugin type
		 *\param[in]	p_pLibrary	The shared library holding the plugin
		 *\param[in]	p_engine	The engine
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_type		Le type du plugin
		 *\param[in]	p_pLibrary	La librairie partagée contenant le plugin
		 *\param[in]	p_engine	Le moteur
		 */
		PluginBase( ePLUGIN_TYPE p_type, Castor::DynamicLibrarySPtr p_pLibrary, Engine & p_engine );

	public:
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~PluginBase() = 0;
		/**
		 *\~english
		 *\brief		Retrieves the required version for the plugin to work correctly
		 *\return		The version
		 *\~french
		 *\brief		Récupère la version nécessaire au bon fonctionnement du plugin
		 *\return		La version
		 */
		void GetRequiredVersion( Version & p_version )const;
		/**
		 *\~english
		 *\brief		Retrieves the plugin name
		 *\return		The name
		 *\~french
		 *\brief		Récupère le nom du plugin
		 *\return		Le nom
		 */
		Castor::String GetName()const;
		/**
		 *\~english
		 *\brief		Retrieves the plugin type
		 *\return		The type
		 *\~french
		 *\brief		Récupère le type du plugin
		 *\return		Le type
		 */
		inline ePLUGIN_TYPE GetType()const
		{
			return m_type;
		}

	protected:
		//!\~english The plugin's version checking function	\~french La fonction de récupération de la version requise
		PGetRequiredVersionFunction m_pfnGetRequiredVersion;
		//!\~english The plugin's name retrieval function	\~french La fonction de récupération du nom du plugin
		PGetNameFunction m_pfnGetName;
		//!\~english The plugin's loading function	\~french La fonction de chargement du plugin
		POnLoadFunction m_pfnOnLoad;
		//!\~english The plugin's unloading function	\~french La fonction de déchargement du plugin
		POnUnloadFunction m_pfnOnUnload;
		//!\~english The plugin type	\~french Le type du plugin
		ePLUGIN_TYPE m_type;
	};
}

#endif

