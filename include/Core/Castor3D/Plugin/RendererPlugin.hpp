/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RENDERER_PLUGIN_H___
#define ___C3D_RENDERER_PLUGIN_H___

#include "Castor3D/Plugin/Plugin.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Renderer Plugin class
	\~french
	\brief		Classe de plug-in de rendu
	*/
	class RendererPlugin
		: public Plugin
	{
	private:
		friend class Plugin;
		friend class Engine;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	library	The shared library holding the plug-in
		 *\param[in]	engine	The engine
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	library	La bibliothèque partagée contenant le plug-in
		 *\param[in]	engine	Le moteur
		 */
		C3D_API RendererPlugin( castor::DynamicLibrarySPtr library
			, Engine * engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~RendererPlugin();
		/**
		 *\~english
		 *\brief		Retrieves the renderer type
		 *\return		The type
		 *\~french
		 *\brief		Récupère le type de renderer
		 *\return		Le type
		 */
		C3D_API castor::String const & getRendererType();
		/**
		 *\~english
		 *\brief		Defines the renderer type.
		 *\param[in]	type	The type.
		 *\~french
		 *\brief		Définit le type de renderer.
		 *\param[in]	type	Le type.
		 */
		C3D_API void setRendererType( castor::String const & type );

	private:
		castor::String m_type;
	};
}

#endif

