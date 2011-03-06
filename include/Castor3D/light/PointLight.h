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
#ifndef ___C3D_PointLight___
#define ___C3D_PointLight___

#include "Light.h"

namespace Castor3D
{
	//! Class which represents a PointLight
	/*!
	A point light is a light which enlights from a point in all directions.
	Uses Light::Render function
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class C3D_API PointLight : public Light
	{
	private:
		friend class Scene;
		//! The attenuation variables : constant, linear and quadratic
		Point3f m_attenuation;
		/**
		 * Constructor, not to be used by the user, use Scene::CreateLight function instead
		 *@param p_renderer : The renderer for the light, may be OpenGL or Direct3D
		 *@param p_name : This light's name, default is void
		 *@param p_index : This light's index (max 8 active lights at a time)
		 */
		PointLight( Scene * p_pScene, SceneNodePtr p_pNode, const String & p_name=C3DEmptyString);

	public:
		/**
		 * Destructor
		 */
		~PointLight();

	private:
		/**
		 * Initialisation function, makes renderer apply attenuations and colour values
		 */
		virtual void _initialise();

	public:
		/**
		 * Sets attenuation variables from an array of real
		 *@param p_attenuation : an array of three floats
		 */
		void SetAttenuation	( float * p_attenuation);
		/**
		* Sets attenuation variables from the three values
		*@param p_const : the constant attenuation
		*@param p_linear : the linear attenuation
		*@param p_quadratic : the quadratic attenuation
		 */
		void SetAttenuation	( float p_const, float p_linear, float p_quadratic);
		/**
		 * Sets attenuation variables from a Point3r
		 *@param p_attenuation : a Point3r
		 */
		void SetAttenuation	( const Point3f & p_attenuation);

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

		/**@name Accessors */
		//@{
		inline float *		GetAttenuation	()		{ return m_attenuation.ptr(); }
		//@}
	};
}

#endif