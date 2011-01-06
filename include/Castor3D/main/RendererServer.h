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
#ifndef ___C3D_RendererServer___
#define ___C3D_RendererServer___

#include "../Prerequisites.h"

namespace Castor3D
{
	//! Renderer driver
	/*!
	 * Used to introduce OpenGL and Direct3D drivers
	\author Sylvain DOREMUS
	\date 25/08/2010
	*/
	class RendererDriver : public MemoryTraced<RendererDriver>
	{
	public:
		typedef enum
		{
			eOpenGL2,		//!< OpenGL2 Renderer
			eOpenGL3,		//!< OpenGL3 Renderer
			eDirect3D,		//!< Direct3D Renderer
		}
		eDRIVER_TYPE;

	protected:
		eDRIVER_TYPE m_eType;

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
		virtual void CreateRenderSystem( SceneManager * p_pSceneManager)=0;
	};
	//! Renderer server
	/**
	 * Registers available RendererDriver (OpenGL or Direct3D)
	 */
	class RendererServer : public MemoryTraced<RendererServer>
	{
	private:
		RendererDriverPtrIntMap m_mapRenderers;	//!< The renderer drivers array
		StrIntMap m_mapNames;

	public:
		/**
		 * Constructor
		 */ 
		C3D_API RendererServer();
		/**
		 * Destructor
		 */ 
		C3D_API ~RendererServer();
		C3D_API void Cleanup();
		/**
		 * Allows plugins to add new graphics drivers
		 *@param p_eType : [in] The renderer type
		 *@param p_driver : [in] The driver to register
		 */ 
		C3D_API void AddRenderer( RendererDriver::eDRIVER_TYPE p_eType, RendererDriverPtr p_driver);
		/**
		 * Get the total number of registered graphics drivers
		 *@return The number of registered drivers
		 */
		C3D_API size_t GetRendererCount()const;
		/**
		 * Access a renderer by its type
		 *@param p_eType : [in] The renderer type
		 *@return The renderer at the given type, NULL if not found
		 */
		C3D_API RendererDriverPtr GetRenderer( RendererDriver::eDRIVER_TYPE p_eType);
		/**
		 * Access a renderer name by its type
		 *@param p_eType : [in] The renderer type
		 *@return The name of the renderer
		 */
		C3D_API String GetRendererName( RendererDriver::eDRIVER_TYPE p_eType);
	};

}

#endif
