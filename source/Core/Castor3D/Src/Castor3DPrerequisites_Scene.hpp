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
#ifndef ___C3D_PREREQUISITES_SCENE_H___
#define ___C3D_PREREQUISITES_SCENE_H___

namespace Castor3D
{
	/**@name Scene */
	//@{

	class Scene;
	class SceneLoader;
	class SceneNode;
	class SceneFileContext;
	class SceneFileParser;

	DECLARE_SMART_PTR( SceneNode );
	DECLARE_SMART_PTR( Scene );
	DECLARE_SMART_PTR( SceneFileContext );
	DECLARE_SMART_PTR( SceneFileParser );

	//! SceneNode pointer array
	DECLARE_VECTOR( SceneNodeSPtr, SceneNodePtr );
	//! Scene pointer map, sorted by name
	DECLARE_MAP( Castor::String, SceneSPtr, ScenePtrStr );
	//! SceneNode pointer map, sorted by name
	DECLARE_MAP( Castor::String, SceneNodeSPtr, SceneNodePtrStr );

	//@}
}

#endif
