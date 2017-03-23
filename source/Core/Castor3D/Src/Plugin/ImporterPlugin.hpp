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
	\brief		Classe de plug-in d'import
	*/
	class ImporterPlugin
		: public Plugin
	{
	public:
		typedef std::pair< Castor::String, Castor::String > Extension;
		DECLARE_VECTOR( Extension, Extension );

	private:
		friend class Plugin;
		friend class Engine;
		typedef ExtensionArray GetExtensionFunction( Engine * p_engine );

		typedef GetExtensionFunction * PGetExtensionFunction;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_library	The shared library holding the plug-in
		 *\param[in]	p_engine	The engine
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_library	La bibliothèque partagée contenant le plug-in
		 *\param[in]	p_engine	Le moteur
		 */
		C3D_API ImporterPlugin( Castor::DynamicLibrarySPtr p_library, Engine * p_engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~ImporterPlugin();
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
		PGetExtensionFunction m_pfnGetExtension;
	};
}

#endif
