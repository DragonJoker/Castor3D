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
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_Plugin___
#define ___C3D_Plugin___

#include "Module_Main.h"

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <CastorUtils/DynamicLibrary.h>

namespace Castor3D
{
	//! Plugin class
	/*!
	Manages the plugin functions, allows plugins to check versions and to register themselves
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class CS3D_API Plugin 
	{
	private:
		typedef void fnGetRequiredVersion( int &);		//!< Signature for the plugin's version checking function
		typedef void fnRegisterPlugin( Root &);			//!< Signature for the plugin's registration function
	
		size_t * m_pDLLRefCount;						//!< Counts the number of occurences of the plugin
		fnGetRequiredVersion * m_pfnGetRequiredVersion;	//!< The plugin's version checking function
		fnRegisterPlugin * m_pfnRegisterPlugin;			//!< The plugin's registration function

		DynamicLibrary * m_library;

	public:
		/**
		 * Constructor
		 *@param p_fileName : [in] The plugin name
		 */
		Plugin( const String & p_fileName);
		/**
		 * Copy constructor
		 */
		Plugin( const Plugin & p_plugin);
		/**
		 * Destructor
		 */
		~Plugin();

	private:
		/**
		 * Check equality between two plugin
		 */
		Plugin & operator =( const Plugin & p_plugin);

	public:
		/**
		 * Retrieves required version
		 */
		void GetRequiredVersion( int & p_version){ m_pfnGetRequiredVersion( p_version); }
		/**
		 * Register the plugin to a root system
		 */
		void RegisterPlugin( Root & p_root){ m_pfnRegisterPlugin( p_root); }

	};
}

#endif

