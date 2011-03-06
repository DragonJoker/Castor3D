/*
This source file is part of Castor3D (http://dragonjoker.co.cc

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
#ifndef ___C3D_Plugin___
#define ___C3D_Plugin___

#include "../Prerequisites.h"

#ifdef WIN32
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#endif

namespace Castor3D
{
	//! Plugin Base class
	/*!
	Manages the base plugin functions, allows plugins to check versions and to register themselves
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class C3D_API PluginBase
	{
	public:
		//! Plugin types enumerator
		/*!
		There are 3 plugins types defined yet : renderer, importer and divider
		\author Sylvain DOREMUS
		\version 0.6.1.0
		\date 03/01/2011
		*/
		typedef enum
		{
			ePluginRenderer,	//!< Renderer plugin (OpenGl2, 3 or Direct3D)
			ePluginImporter,	//!< Importer plugin
			ePluginDivider,		//!< Mesh Divider plugin
		}
		eTYPE;

	protected:
		typedef void GetRequiredVersionFunction( Version & p_version);	//!< Signature for the plugin's version checking function

		typedef GetRequiredVersionFunction	*	PGetRequiredVersionFunction;

		PGetRequiredVersionFunction m_pfnGetRequiredVersion;	//!< The plugin's version checking function

		size_t * m_pDLLRefCount;				//!< Counts the number of occurences of the plugin
		Utils::DynamicLibrary * m_library;		//!< The library holding the plugin
		eTYPE m_eType;							//!< The plugin type

	public:
		/**
		 * Constructor
		 *@param p_fileName : [in] The plugin name
		 */
		PluginBase( const String & p_fileName);
		/**
		 * Destructor
		 */
		virtual ~PluginBase() = 0;

		void GetRequiredVersion( Version & p_version);

		inline const eTYPE &	GetType	()const { return m_eType; }

	protected:
		/**
		 * Copy constructor
		 */
		PluginBase( const PluginBase & p_plugin);
		/**
		 * Assignment operator, protected so nobody uses it
		 */
		PluginBase & operator =( const PluginBase & p_plugin);

	};
	//! Renderer Plugin class
	/*!
	Manages the plugin functions
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class C3D_API RendererPlugin : public PluginBase, public MemoryTraced<RendererPlugin>
	{
	private:
		typedef void RegisterPluginFunction( Root &);

		typedef RegisterPluginFunction		*	PRegisterPluginFunction;

		PRegisterPluginFunction		m_pfnRegisterPlugin;

	public:
		/**
		 * Constructor
		 *@param p_fileName : [in] The plugin name
		 */
		RendererPlugin( const String & p_fileName);
		/**
		 * Destructor
		 */
		virtual ~RendererPlugin();

		void RegisterPlugin( Root & p_root);

	private:
		/**
		 * Copy constructor
		 */
		RendererPlugin( const RendererPlugin & p_plugin);
		/**
		 * Assignment operator
		 */
		RendererPlugin & operator =( const RendererPlugin & p_plugin);

	};
	//! Plugin class
	/*!
	Manages the plugin functions, allows plugins to check versions and to register themselves
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class C3D_API ImporterPlugin : public PluginBase, public MemoryTraced<ImporterPlugin>
	{
	private:
		typedef Importer * CreateImporterFunction( SceneManager * p_pManager);

		typedef CreateImporterFunction	*	PCreateImporterFunction;

		PCreateImporterFunction m_pfnCreateImporterFunction;

	public:
		/**
		 * Constructor
		 *@param p_fileName : [in] The plugin name
		 */
		ImporterPlugin( const String & p_fileName);
		/**
		 * Destructor
		 */
		virtual ~ImporterPlugin();

		Importer * CreateImporter( SceneManager * p_pManager);

	private:
		/**
		 * Copy constructor
		 */
		ImporterPlugin( const ImporterPlugin & p_plugin);
		/**
		 * Assignment operator
		 */
		ImporterPlugin & operator =( const ImporterPlugin & p_plugin);

	};
	//! Plugin class
	/*!
	Manages the plugin functions, allows plugins to check versions and to register themselves
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class C3D_API DividerPlugin : public PluginBase, public MemoryTraced<DividerPlugin>
	{
	private:
		typedef Subdivider * CreateDividerFunction( Submesh * p_pSubmesh);

		typedef CreateDividerFunction	*	PCreateDividerFunction;

		PCreateDividerFunction m_pfnCreateDividerFunction;

	public:
		/**
		 * Constructor
		 *@param p_fileName : [in] The plugin name
		 */
		DividerPlugin( const String & p_fileName);
		/**
		 * Destructor
		 */
		virtual ~DividerPlugin();

		Subdivider * CreateDivider( Submesh * p_pSubmesh);

	private:
		/**
		 * Copy constructor
		 */
		DividerPlugin( const DividerPlugin & p_plugin);
		/**
		 * Assignment operator
		 */
		DividerPlugin & operator =( const DividerPlugin & p_plugin);

	};
}

#endif

