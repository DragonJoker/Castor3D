/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PLUGIN_H___
#define ___C3D_PLUGIN_H___

#include "Castor3DPrerequisites.hpp"
#include "PluginException.hpp"

#include <Design/OwnedBy.hpp>

namespace castor3d
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
	class Plugin
		: public castor::OwnedBy< Engine >
	{
	private:
		//!< Signature for the plug-in's loading function
		typedef void OnLoadFunction( Engine *, Plugin * );
		//!< Signature for the plug-in's unloading function
		typedef void OnUnloadFunction( Engine * );
		//!< Signature for the plug-in's type retrieval function
		typedef void GetTypeFunction( PluginType * );
		//!< Signature for the plug-in's version checking function
		typedef void GetRequiredVersionFunction( Version * p_version );
		//!< Signature for the plug-in's name retrieval function
		typedef void GetNameFunction( char const ** );

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
		 *\param[in]	engine	The engine
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_type		Le type du plug-in
		 *\param[in]	p_library	La bibliothèque partagée contenant le plug-in
		 *\param[in]	engine	Le moteur
		 */
		C3D_API Plugin( PluginType p_type, castor::DynamicLibrarySPtr p_library, Engine & engine );

	public:
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~Plugin() = 0;
		/**
		 *\~english
		 *\brief		Retrieves the required version for the plug-in to work correctly
		 *\return		The version
		 *\~french
		 *\brief		Récupère la version nécessaire au bon fonctionnement du plug-in
		 *\return		La version
		 */
		C3D_API void getRequiredVersion( Version & p_version )const;
		/**
		 *\~english
		 *\brief		Retrieves the plug-in name
		 *\return		The name
		 *\~french
		 *\brief		Récupère le nom du plug-in
		 *\return		Le nom
		 */
		C3D_API castor::String getName()const;
		/**
		 *\~english
		 *\brief		Retrieves the plug-in type
		 *\return		The type
		 *\~french
		 *\brief		Récupère le type du plug-in
		 *\return		Le type
		 */
		inline PluginType getType()const
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
		PluginType m_type;
	};
}

#endif

