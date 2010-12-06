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
	class C3D_API SpotLight : public Light
	{
	private:
		friend class Scene;
		//! The attenuation variables : constant, linear and quadratic
		Point3f m_attenuation;
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
		SpotLight( LightNodePtr p_pNode, const String & p_name=C3DEmptyString);

	public:
		/**
		 * Destructor
		 */
		~SpotLight();
		/**
		 * Renders the light in the current window
		 */
		virtual void Render( eDRAW_TYPE p_displayMode);

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
		 * Writes the spot light in a file
		 *@param p_pFile : a pointer to file to write in
		 */
		virtual bool Write( Castor::Utils::File & p_pFile)const;

	public:
		/**
		 *@return The light type (Spot, here), used for polymorphism
		 */
		virtual eTYPE			GetLightType	()const { return eSpot; }
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

