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

#include "Scene.h"
#include <CastorUtils/UniqueManager.h>

namespace Castor3D
{
	//! The scene manager handler class
	/*!
	Manages all the scenes of a project. It is a singleton, so to have an access to it's function, you must call the SceneManager::GetSingletonPtr function
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class CS3D_API SceneManager : public Castor::Templates::UniqueManager<Scene, SceneManager>
	{
	private:
		friend class Castor::Templates::UniqueManager<Scene, SceneManager>;
	private:
		/**
		 * Constructor
		 */
		SceneManager();
		/**
		 * Destructor
		 */
		~SceneManager();

	public:
		static ScenePtr CreateElement( const String & p_elementName);
		static void ClearScenes();
	};
}

#endif

