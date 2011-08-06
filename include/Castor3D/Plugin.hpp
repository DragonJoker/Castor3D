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
#ifndef ___C3D_Plugin___
#define ___C3D_Plugin___

#include "Prerequisites.hpp"

#ifdef WIN32
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#endif

#pragma warning( push)
#pragma warning( disable:4290)

namespace Castor3D
{
	//!Plugin loading exception
	/*!
	Exception thrown when a Plugin fails to load its functions
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 01/04/2011
	*/
	class C3D_API PluginException : public Utils::Exception
	{
	public:
		PluginException( String const & p_description, String const & p_file, String const & p_function, unsigned int p_line);
		~PluginException()throw();
	};
	//! helper macro to ease the use of PluginException
#	define CASTOR_PLUGIN_EXCEPTION( x) throw PluginException( x, __FILE__, __FUNCTION__, __LINE__)

	//! Plugin types enumerator
	/*!
	There are 3 plugins types defined yet : renderer, importer and divider
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	*/
	typedef enum
	{	ePLUGIN_TYPE_RENDERER		//!< Renderer plugin (OpenGl2, 3 or Direct3D)
	,	ePLUGIN_TYPE_IMPORTER		//!< Importer plugin
	,	ePLUGIN_TYPE_DIVIDER		//!< Mesh Divider plugin
	,	ePLUGIN_TYPE_GENERATOR		//!< Procedural generator
	,	ePLUGIN_TYPE_COUNT
	}	ePLUGIN_TYPE;
	//! Enumerates all supported renderers
	/*!
	2 renderers are supported yet : OpenGL and Direct3D9
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 01/04/2011
	*/
	typedef enum
	{	eRENDERER_TYPE_UNDEFINED	= -1
	,	eRENDERER_TYPE_OPENGL		= 0		//!< OpenGl Renderer
	,	eRENDERER_TYPE_DIRECT3D		= 1		//!< Direct3D9 Renderer
	,	eRENDERER_TYPE_COUNT		= 2
	}	eRENDERER_TYPE;
	//! Plugin Base class
	/*!
	Manages the base plugin functions, allows plugins to check versions and to register themselves
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class C3D_API PluginBase
	{
	private:
		typedef ePLUGIN_TYPE	GetTypeFunction();									//!< Signature for the plugin's type retrieval function
		typedef void			GetRequiredVersionFunction( Version & p_version);	//!< Signature for the plugin's version checking function
		typedef String			GetNameFunction();									//!< Signature for the plugin's name retrieval function

	public:
		typedef GetTypeFunction				*	PGetTypeFunction;
		typedef GetRequiredVersionFunction	*	PGetRequiredVersionFunction;
		typedef GetNameFunction				*	PGetNameFunction;

	protected:
		PGetRequiredVersionFunction	m_pfnGetRequiredVersion;	//!< The plugin's version checking function
		PGetNameFunction			m_pfnGetName;				//!< The plugin's name retrieval function
		ePLUGIN_TYPE				m_eType;					//!< The plugin type
		/**
		 * Constructor
		 *@param p_fileName : [in] The plugin name
		 */
		PluginBase( ePLUGIN_TYPE p_eType, Utils::DynamicLibraryPtr p_pLibrary)throw( PluginException);

	public:
		/**
		 * Destructor
		 */
		virtual ~PluginBase() = 0;

		void GetRequiredVersion( Version & p_version)const;
		String GetName()const;

		inline ePLUGIN_TYPE GetType()const { return m_eType; }

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
		friend class PluginBase;
		friend class Root;

		typedef RenderSystem * CreateRenderSystemFunction();
		typedef eRENDERER_TYPE GetRendererTypeFunction();

		typedef CreateRenderSystemFunction	*	PCreateRenderSystemFunction;
		typedef GetRendererTypeFunction		*	PGetRendererTypeFunction;

		PCreateRenderSystemFunction	m_pfnCreateRenderSystem;
		PGetRendererTypeFunction	m_pfnGetRendererType;
		/**
		 * Constructor
		 *@param p_fileName : [in] The plugin name
		 */
		RendererPlugin( DynamicLibraryPtr p_pLibrary)throw( PluginException);

	public:
		/**
		 * Destructor
		 */
		virtual ~RendererPlugin();

		RenderSystem * CreateRenderSystem();
		eRENDERER_TYPE GetRendererType();

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
	//! Importer Plugin class
	/*!
	Manages the plugin functions, allows plugins to check versions and to register themselves
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class C3D_API ImporterPlugin : public PluginBase, public MemoryTraced<ImporterPlugin>
	{
	private:
		friend class PluginBase;
		friend class Root;
		typedef Importer * CreateImporterFunction();
		typedef String GetExtensionFunction();

		typedef CreateImporterFunction	*	PCreateImporterFunction;
		typedef GetExtensionFunction	*	PGetExtensionFunction;

		PCreateImporterFunction m_pfnCreateImporter;
		PGetExtensionFunction	m_pfnGetExtension;
		/**
		 * Constructor
		 *@param p_fileName : [in] The plugin name
		 */
		ImporterPlugin( DynamicLibraryPtr p_pLibrary)throw( PluginException);

	public:
		/**
		 * Destructor
		 */
		virtual ~ImporterPlugin();

		Importer * CreateImporter();
		String GetExtension();

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
	//! Divider Plugin class
	/*!
	Manages the plugin functions, allows plugins to check versions and to register themselves
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class C3D_API DividerPlugin : public PluginBase, public MemoryTraced<DividerPlugin>
	{
	private:
		friend class PluginBase;
		friend class Root;
		typedef Subdivider * CreateDividerFunction( Submesh * p_pSubmesh);

		typedef CreateDividerFunction		*	PCreateDividerFunction;

		PCreateDividerFunction		m_pfnCreateDivider;
		/**
		 * Constructor
		 *@param p_fileName : [in] The plugin name
		 */
		DividerPlugin( DynamicLibraryPtr p_pLibrary)throw( PluginException);

	public:
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
	//! Generator Plugin class
	/*!
	Manages the plugin functions, allows plugins to check versions and to register themselves
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class C3D_API GeneratorPlugin : public PluginBase, public MemoryTraced<GeneratorPlugin>
	{
	private:
		friend class PluginBase;
		friend class Root;
		typedef Subdivider * CreateGeneratorFunction( TextureUnit * p_pTexture);

		typedef CreateGeneratorFunction		*	PCreateGeneratorFunction;

		PCreateGeneratorFunction		m_pfnCreateGenerator;
		/**
		 * Constructor
		 *@param p_fileName : [in] The plugin name
		 */
		GeneratorPlugin( DynamicLibraryPtr p_pLibrary)throw( PluginException);

	public:
		/**
		 * Destructor
		 */
		virtual ~GeneratorPlugin();

		Subdivider * CreateGenerator( TextureUnit * p_pSubmesh);

	private:
		/**
		 * Copy constructor
		 */
		GeneratorPlugin( const GeneratorPlugin & p_plugin);
		/**
		 * Assignment operator
		 */
		GeneratorPlugin & operator =( const GeneratorPlugin & p_plugin);
	};
}

#pragma warning( pop)

#endif

