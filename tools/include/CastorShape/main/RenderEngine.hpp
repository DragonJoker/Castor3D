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
#ifndef ___CS_RenderEngine___
#define ___CS_RenderEngine___

namespace CastorShape
{
	//! The render engine
	/*!
	Used to render scene in an image. To be implemented
	\author Sylvain DOREMUS
	\date 25/08/2010
	*/
	class RenderEngine
	{
	protected:
		Castor3D::SceneSPtr m_pScene;		//!< The scene to render
		Castor::String m_strFileName;	//!< The file to write in

	public:
		/**
		 * Constructor
		 *\param[in]	p_strFileName	The file path
		 *\param[in]	p_pScene	The scene to render
		 */
		RenderEngine( Castor::String const & p_strFileName, Castor3D::SceneSPtr p_pScene);
		/**
		 * Destructor
		 */
		~RenderEngine();
		/**
		 * Renders the scene
		 *\return	true if successful, false if not
		 */
		bool Draw();
	};
}

#endif