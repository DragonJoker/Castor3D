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
#ifndef ___C3D_SceneManager___
#define ___C3D_SceneManager___

#include "../Prerequisites.h"
#include "Scene.h"

namespace Castor3D
{
	//! The scene manager handler class
	/*!
	Manages all the scenes of a project. It is a singleton, so to have an access to it's function, you must call the SceneManager::GetSingletonPtr function
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class C3D_API SceneManager : public Castor::Templates::Manager<Scene>
	{
	private:
		friend class Castor::Templates::Manager<Scene>;

		MeshManager		*	m_pMeshManager;
		MaterialManager	*	m_pMaterialManager;
		ImageManager	*	m_pImageManager;
		FontManager		*	m_pFontManager;

	public:
		/**
		 * Constructor
		 */
		SceneManager();
		/**
		 * Destructor
		 */
		~SceneManager();
		/**
		 * Clears all the scenes handled by the manager
		 */
		void ClearScenes();

		ScenePtr CreateScene( const String & p_strName);

		inline MeshManager		* 	GetMeshManager		()const { return m_pMeshManager; }
		inline MaterialManager	* 	GetMaterialManager	()const { return m_pMaterialManager; }
		inline ImageManager		*	GetImageManager		()const { return m_pImageManager; }
		inline FontManager		*	GetFontManager		()const { return m_pFontManager; }
	};
}

#endif

