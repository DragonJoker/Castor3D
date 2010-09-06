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
#ifndef ___C3D_RendererServer___
#define ___C3D_RendererServer___

#include "../render_system/Module_Render.h"
#include "../camera/Module_Camera.h"

namespace Castor3D
{
	//! Renderer driver
	/*!
	 * Used to introduce OpenGL and Direct3D drivers
	\author Sylvain DOREMUS
	\date 25/08/2010
	*/
	class RendererDriver
	{
		public:
			/**
			 * Destructor
			 */ 
			virtual ~RendererDriver() {}
			/**
			 * Get name of the graphics driver
			 */ 
			virtual const String & GetName()const = 0;
			/**
			 * Create a renderer
			 */ 
			virtual RenderSystem * CreateRenderSystem() = 0;
	};
	//! Renderer server
	/**
	 * Registers available RendererDriver (OpenGL or Direct3D)
	 */
	class RendererServer
	{
		private:
			RendererDriverPtrArray m_rendererDrivers;	//!< The renderer drivers array

	public:
		/**
		 * Destructor
		 */ 
		CS3D_API ~RendererServer();
		/**
		 * Allows plugins to add new graphics drivers
		 *@param p_driver : [in] The driver to register
		 */ 
		CS3D_API void AddRendererDriver( RendererDriver * p_driver);
		/**
		 * Get the total number of registered graphics drivers
		 *@return The number of registered drivers
		 */
		CS3D_API size_t GetDriverCount()const;
		/**
		 * Access a driver by its index
		 *@param Index : [in] The index where to look for the driver 
		 *@return The driver at the given index, NULL if not found
		 */
		CS3D_API RendererDriver * GetDriver( size_t Index);
	};

}

#endif
