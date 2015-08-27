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
	{
	private:
		//!< Signature for the plugin's type retrieval function
		typedef ePLUGIN_TYPE	GetTypeFunction();
		//!< Signature for the plugin's version checking function
		typedef void			GetRequiredVersionFunction( Version & p_version );
		//!< Signature for the plugin's name retrieval function
		typedef Castor::String	GetNameFunction();

	public:
		typedef GetTypeFunction			*		PGetTypeFunction;
		typedef GetRequiredVersionFunction	*	PGetRequiredVersionFunction;
		typedef GetNameFunction			*		PGetNameFunction;

	protected:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_eType		The plugin type
		 *\param[in]	p_pLibrary	The shared library holding the plugin
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_eType		Le type du plugin
		 *\param[in]	p_pLibrary	La librairie partagée contenant le plugin
		 */
		PluginBase( ePLUGIN_TYPE p_eType, Castor::DynamicLibrarySPtr p_pLibrary );

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
			return m_eType;
		}

	protected:
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\param[in]	p_plugin	The Plugin object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_plugin	L'objet Plugin à copier
		 */
		PluginBase( PluginBase const & p_plugin );
		/**
		 *\~english
		 *\brief		Move constructor
		 *\param[in]	p_plugin	The Plugin object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_plugin	L'objet Plugin à déplacer
		 */
		PluginBase( PluginBase && p_plugin );
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
		PluginBase & operator =( PluginBase const & p_plugin );
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
		PluginBase & operator =( PluginBase && p_plugin );

	protected:
		//!< The plugin's version checking function
		PGetRequiredVersionFunction m_pfnGetRequiredVersion;
		//!< The plugin's name retrieval function
		PGetNameFunction m_pfnGetName;
		//!< The plugin type
		ePLUGIN_TYPE m_eType;
	};
}

#endif

