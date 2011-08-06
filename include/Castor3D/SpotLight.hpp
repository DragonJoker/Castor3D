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
#ifndef ___C3D_SpotLight___
#define ___C3D_SpotLight___

#include "Light.hpp"

namespace Castor
{	namespace Resources
{
	//! SpotLight loader
	/*!
	Loads and saves lights from/into a file
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	template <> class C3D_API Loader<Castor3D::SpotLight>
	{
	public:
		/**
		 * Writes a light into a text file
		 *@param p_file : [in] the file to write the light in
		 *@param p_light : [in] the p_light to write
		 */
		static bool Write( const Castor3D::SpotLight & p_light, Utils::File & p_file);
	};
}
}

namespace Castor3D
{
	//! Class which represents a SpotLight
	/*!
	A spot light is a light which enlights from a point in a given direction with a conic shape
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class C3D_API SpotLight : public LightCategory
	{
	private:
		friend class Scene;
		//! The attenuation variables : constant, linear and quadratic
		Point3f m_attenuation;
		//! The light exponent, id est how much the light is focused
		float m_exponent;
		//! The angle of the cone
		float m_cutOff;

	public:
		/**
		* Constructor, not to be used by the user, use Scene::CreateLight function instead
		*/
		SpotLight();
		/**
		 * Destructor
		 */
		~SpotLight();
		/**
		 * Clone function, used by the Factory
		 */
		virtual LightCategoryPtr Clone();
		/**
		 * Renders the light => Applies it's position
		 *@param p_displayMode : [in] The display mode
		 */
		virtual void Render( LightRendererPtr p_pRenderer, ePRIMITIVE_TYPE p_displayMode);
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

		/**@name Accessors */
		//@{
		inline const Point3f &	GetAttenuation	()const { return m_attenuation; }
		inline Point3f &		GetAttenuation	()		{ return m_attenuation; }
		inline float			GetExponent		()const { return m_exponent; }
		inline float			GetCutOff		()const { return m_cutOff; }
		//@}
		DECLARE_SERIALISE_MAP()
	};
}

#endif

