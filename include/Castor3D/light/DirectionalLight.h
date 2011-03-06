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
#ifndef ___C3D_DirectionalLight___
#define ___C3D_DirectionalLight___

#include "Light.h"

namespace Castor3D
{
	//! Class which represents a Directional Light
	/*!
	A directionam light is a light which enlights from an infinite point in a given direction
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class C3D_API DirectionalLight : public Light
	{
	private:
		friend class Scene;

	public:
		/**@name Construction / Destruction */
		//@{
		/**
		 * Constructor, not to be used by the user, use Scene::CreateLight function instead
		 *@param p_renderer : A light renderer, may be OpenGL or Direct3D
		 *@param p_name : The light's name, default is void
		 *@param p_index : The light's index (max 8 active lights at a time)
		 */
		DirectionalLight( Scene * p_pScene, SceneNodePtr p_pNode, const String & p_name=C3DEmptyString);
		/**
		 * Destructor
		 */
		~DirectionalLight();
		//@}

		/**@name Inherited methods from Textable */
		//@{
		virtual bool Write( File & p_file)const;
		virtual bool Read( File & p_file) { return false; }
		//@}

		/**@name Inherited methods from Serialisable */
		//@{
		virtual bool Save( File & p_file)const;
		virtual bool Load( File & p_file);
		//@}

	private:
		/**
		 * Initialisation function, makes renderer apply colour values
		 */
		virtual void _initialise();
	};
}

#endif

