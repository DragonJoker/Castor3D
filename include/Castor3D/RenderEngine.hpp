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
#ifndef ___C3D_RenderEngine___
#define ___C3D_RenderEngine___

#include "Prerequisites.hpp"

namespace Castor3D
{
	//! The render engine
	/*!
	Used to render scene in an image. To be implemented
	\author Sylvain DOREMUS
	\date 25/08/2010
	*/
	class C3D_API RenderEngine : public MemoryTraced<RenderEngine>
	{
	protected:
		ScenePtr m_pScene;		//!< The scene to render
		String m_strFileName;	//!< The file to write in

	public:
		/**
		 * Constructor
		 *@param p_strFileName : [in] The file path
		 *@param p_pScene : [in] The scene to render
		 */
		RenderEngine( String const & p_strFileName, ScenePtr p_pScene);
		/**
		 * Destructor
		 */
		~RenderEngine();
		/**
		 * Renders the scene
		 *@return true if successful, false if not
		 */
		bool Draw();
	};
}

#endif