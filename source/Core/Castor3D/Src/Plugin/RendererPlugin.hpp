/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___C3D_RENDERER_PLUGIN_H___
#define ___C3D_RENDERER_PLUGIN_H___

#include "Plugin.hpp"

namespace Castor3D
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
		 *\param[in]	p_library	The shared library holding the plug-in
		 *\param[in]	engine	The engine
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_library	La bibliothèque partagée contenant le plug-in
		 *\param[in]	engine	Le moteur
		 */
		C3D_API RendererPlugin( Castor::DynamicLibrarySPtr p_library, Engine * engine );
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
		C3D_API Castor::String const & GetRendererType();
		/**
		 *\~english
		 *\brief		Defines the renderer type.
		 *\param[in]	p_type	The type.
		 *\~french
		 *\brief		Définit le type de renderer.
		 *\param[in]	p_type	Le type.
		 */
		C3D_API void SetRendererType( Castor::String const & p_type );

	private:
		Castor::String m_type;
	};
}

#endif

