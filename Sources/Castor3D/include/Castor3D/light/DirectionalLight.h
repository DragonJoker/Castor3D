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
	class CS3D_API DirectionalLight : public Light
	{
	private:
		friend class Scene;
		/**
		 * Constructor, not to be used by the user, use Scene::CreateLight function instead
		 *@param p_renderer : A light renderer, may be OpenGL or Direct3D
		 *@param p_name : The light's name, default is void
		 *@param p_index : The light's index (max 8 active lights at a time)
		 */
		DirectionalLight( LightRenderer * p_renderer, const String & p_name=C3DEmptyString);
	public:
		/**
		 * Destructor
		 */
		~DirectionalLight();

	private:
		/**
		 * Initialisation function, makes renderer apply colour values
		 */
		virtual void _initialise();

	public:
		/**
		 * Writes the directional light in a file
		 *@param p_file : a pointer to file to write in
		 */
		virtual bool Write( General::Utils::File & p_file)const;
		/**
		 * Reads the directional light from a file
		 *@param p_file : a pointer to file to read from
		 *@param p_scene : [in] The scene to find the node to attach this light to
		 *@param p_index : [in] The index of this light
		 */
		virtual bool Read( General::Utils::File & p_file, Scene * p_scene);

	public:
		/**
		 *@return The light type (Directional for this one), used for polymorphism
		 */
		virtual inline eTYPE	GetLightType()const {return eDirectional;}
	};
}

#endif

