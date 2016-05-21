/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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

#include <OwnedBy.hpp>

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Plugin Base class
	\remark		Manages the base plug-in functions, allows plug-ins to check versions and to register themselves
	\~french
	\brief		Classe de base des plug-ins
	\remark		Gère les fonctions de base d'un plug-in, permet aux plug-ins de faire des vérifications de version et  de s'enregistrer auprès du moteur
	*/
	class PluginBase
		: public Castor::OwnedBy< Engine >
	{
	private:
		//!< Signature for the plug-in's loading function
		typedef void OnLoadFunction( Engine * );
		//!< Signature for the plug-in's unloading function
		typedef void OnUnloadFunction( Engine * );
		//!< Signature for the plug-in's type retrieval function
		typedef ePLUGIN_TYPE GetTypeFunction();
		//!< Signature for the plug-in's version checking function
		typedef void GetRequiredVersionFunction( Version & p_version );
		//!< Signature for the plug-in's name retrieval function
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
		 *\param[in]	p_type		The plug-in type
		 *\param[in]	p_library	The shared library holding the plug-in
		 *\param[in]	p_engine	The engine
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_type		Le type du plug-in
		 *\param[in]	p_library	La librairie partagée contenant le plug-in
		 *\param[in]	p_engine	Le moteur
		 */
		C3D_API PluginBase( ePLUGIN_TYPE p_type, Castor::DynamicLibrarySPtr p_library, Engine & p_engine );

	public:
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~PluginBase() = 0;
		/**
		 *\~english
		 *\brief		Retrieves the required version for the plug-in to work correctly
		 *\return		The version
		 *\~french
		 *\brief		Récupère la version nécessaire au bon fonctionnement du plug-in
		 *\return		La version
		 */
		C3D_API void GetRequiredVersion( Version & p_version )const;
		/**
		 *\~english
		 *\brief		Retrieves the plug-in name
		 *\return		The name
		 *\~french
		 *\brief		Récupère le nom du plug-in
		 *\return		Le nom
		 */
		C3D_API Castor::String GetName()const;
		/**
		 *\~english
		 *\brief		Retrieves the plug-in type
		 *\return		The type
		 *\~french
		 *\brief		Récupère le type du plug-in
		 *\return		Le type
		 */
		inline ePLUGIN_TYPE GetType()const
		{
			return m_type;
		}

	protected:
		//!\~english The plug-in's version checking function	\~french La fonction de récupération de la version requise
		PGetRequiredVersionFunction m_pfnGetRequiredVersion;
		//!\~english The plug-in's name retrieval function	\~french La fonction de récupération du nom du plug-in
		PGetNameFunction m_pfnGetName;
		//!\~english The plug-in's loading function	\~french La fonction de chargement du plug-in
		POnLoadFunction m_pfnOnLoad;
		//!\~english The plug-in's unloading function	\~french La fonction de déchargement du plug-in
		POnUnloadFunction m_pfnOnUnload;
		//!\~english The plug-in type	\~french Le type du plug-in
		ePLUGIN_TYPE m_type;
	};
}

#endif

