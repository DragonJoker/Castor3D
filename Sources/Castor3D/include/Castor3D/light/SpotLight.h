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
#ifndef ___C3D_SpotLight___
#define ___C3D_SpotLight___

#include "Light.h"


#include "../render_system/Module_Render.h"

namespace Castor3D
{
	//! Class which represents a SpotLight
	/*!
	A spot light is a light which enlights from a point in a given direction with a conic shape
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class CS3D_API SpotLight : public Light
	{
	private:
		friend class Scene;
		//! The actual orientation of the spot
		Quaternion m_orientation;
		//! The rotation matrix associated to the orientation
		float * m_matrix;
		//! The attenuation variables : constant, linear and quadratic
		Point3D<float> m_attenuation;
		//! The light exponent, id est how much the light is focused
		float m_exponent;
		//! The angle of the cone
		float m_cutOff;
		/**
		* Constructor, not to be used by the user, use Scene::CreateLight function instead
		*@param p_renderer : A light renderer, may be OpenGL or Direct3D
		*@param p_name : The light's name, default is void
		*@param p_index : The light's index (max 8 active lights at a time)
		*/
		SpotLight( LightRenderer * p_renderer, const String & p_name=C3DEmptyString);

	public:
		/**
		 * Destructor
		 */
		~SpotLight();
		/**
		 * Renders the light in the current window
		 */
		void Render();

	private:
		/**
		 * Initialisation function, makes renderer apply attenuations and colour values
		 */
		virtual void _initialise();

	public:
		/**
		 * Rotates the light around Y Axis
		 *@param p_angle : the rotation angle
		 */
		void Yaw( float p_angle);
		/**
		 * Rotates the light around X Axis
		 *@param p_angle : the rotation angle
		 */
		void Pitch( float p_angle);
		/**
		 * Rotates the light around Z Axis
		 *@param p_angle : the rotation angle
		 */
		void Roll( float p_angle);
		/**
		 * Rotates the light
		 *@param p_quat : The rotation we want to add to the current rotation
		 */
		void Rotate( const Quaternion & p_quat);
		/**
		 * Sets attenuation variables from an array of float
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
		 * Sets attenuation variables from a Vector3f
		 *@param p_attenuation : a Vector3f
		 */
		void SetAttenuation	( const Vector3f & p_attenuation);
		/**
		 * Sets the light exponent
		 *@param p_exponent : the new exponent value
		 */
		void SetExponent( float p_exponent);
		/**
		 * Sets the light cutoff
		 *@param p_cutOff : the new cutoff value
		 */
		void SetCutOff( float p_cutOff);
		/**
		 *@return The rotation matrix from the current orientation
		 */
		float * Get4x4RotationMatrix();
		/**
		 * Writes the spot light in a file
		 *@param p_file : a pointer to file to write in
		 */
		virtual bool Write( General::Utils::File & p_file)const;
		/**
		 * Reads the spot light from a file
		 *@param p_file : a pointer to file to read from
		*@param p_scene : [in] The scene to find the node to attach this light to
		*@param p_index : [in] The index of this light
		 */
		virtual bool Read( General::Utils::File & p_file, Scene * p_scene);

	public:
		/**
		 *@return The light type (Spot for this one), used for polymorphism
		 */
		virtual inline eTYPE	GetLightType()const { return eSpot; }
		/**
		 *@return The attenuation values, three floats : constant, linear and quadratic attenuation
		 */
		inline const float *	GetAttenuation	()const { return m_attenuation.const_ptr(); }
		/**
		 *@return The exponent value
		 */
		inline float			GetExponent		()const { return m_exponent; }
		/**
		 *@return The cutoff value
		 */
		inline float			GetCutOff		()const { return m_cutOff; }
	};
}

#endif

