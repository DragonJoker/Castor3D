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
#ifndef ___C3D_Light___
#define ___C3D_Light___

#include "Module_Light.h"
#include "../render_system/Module_Render.h"
#include "../scene/Module_Scene.h"
#include "../render_system/Renderable.h"

namespace Castor3D
{
	//! Virtual class which represents a Light
	/*!
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	\todo Show only the 8 nearest lights, hide the other ones
	*/
	class CS3D_API Light : public Renderable<Light, LightRenderer>
	{
	public:
		//! The light types enumerator
		/*!
		Defines the three different light types : directional, point and spot
		*/
		typedef enum eTYPE
		{
			eDirectional,	//!< Directional light type
			ePoint,			//!< Point light type
			eSpot			//!< Spot light type
		} eTYPE;

	protected:
		friend class Scene;
		String m_name;					//! The light name
		bool m_enabled;					//! Tells if the light is enabled (for the 8 active lights law)
		Point<float, 4> m_position;		//! The light's position, 4 floats for OpenGL compatibility : x, y, z, and w. w is used to tell if the light is directional or point
		Colour m_ambient;				//! The ambient colour values (RGBA)
		Colour m_diffuse;				//! The diffuse colour values (RGBA)
		Colour m_specular;				//! The specular colour values (RGBA)

	protected:
		/**
		 * Constructor, not to be used by the user, use Scene::CreateLight function instead
		 *@param p_name : The light's name, default is void
		 */
		Light( const String & p_name);

	public:
		/**
		 * Destructor	
		 */
		virtual ~Light();
		/**
		 * Enables the light => Displays it
		 */
		virtual void Enable();
		/**
		 * Disables the light => Hides it
		 */
		virtual void Disable();
		/**
		 * Renders the light => Applies it's position
		 */
		virtual void Apply( eDRAW_TYPE p_displayMode);
		/**
		 * Sets the light's position from an array of 3 floats
		 *@param p_vertex : the 3 floats array
		 */
		void SetPosition( float * p_vertex);
		/**
		 * Sets the light's position from three floats
		 *@param x : [in] The X coordinate
		 *@param y : [in] The Y coordinate
		 *@param z : [in] The Z coordinate
		 */
		void SetPosition( float x, float y, float z);
		/**
		 * Sets the light's position from a Point3r
		 *@param p_vertex : The new position
		 */
		void SetPosition( const Point<float, 3> & p_vertex);
		/**
		 * Translates the light from an array of 3 floats
		 *@param p_vector : the 3 floats array
		 */
		void Translate( float * p_vector);
		/**
		 * Translates the light from three floats
		 *@param x : [in] The X translation value
		 *@param y : [in] The Y translation value
		 *@param z : [in] The Z translation value
		 */
		void Translate( float x, float y, float z);
		/**
		 * Translates the light from a Point3r
		 *@param p_vertex : The new position
		 */
		void Translate( const Point<float, 3> & p_vertex);
		/**
		 * Sets the light's ambient colour from an array of 3 floats
		 *@param p_ambient : the 3 floats array
		 */
		void SetAmbient( float * p_ambient);
		/**
		 * Sets the light's ambient colour from three floats
		 *@param  r : [in] The red value
		 *@param  g : [in] The green value
		 *@param  b : [in] The blue value
		 */
		void SetAmbient( float r, float g, float b);
		/**
		 * Sets the light's ambient colour from a Point3r
		 *@param p_ambient : The new position
		 */
		void SetAmbient( const Colour & p_ambient);
		/**
		 * Sets the light's diffuse colour from an array of 3 floats
		 *@param p_diffuse : the 3 floats array
		 */
		void SetDiffuse( float * p_diffuse);
		/**
		 * Sets the light's diffuse colour from three floats
		 *@param  r : [in] The red value
		 *@param  g : [in] The green value
		 *@param  b : [in] The blue value
		 */
		void SetDiffuse( float r, float g, float b);
		/**
		 * Sets the light's diffuse colour from a Point3r
		 *@param p_diffuse : The new position
		 */
		void SetDiffuse( const Colour & p_diffuse);
		/**
		 * Sets the light's specular colour from an array of 3 floats
		 *@param p_specular : the 3 floats array
		 */
		void SetSpecular( float * p_specular);
		/**
		 * Sets the light's specular colour from three floats
		 *@param  r : [in] The red value
		 *@param  g : [in] The green value
		 *@param  b : [in] The blue value
		 */
		void SetSpecular( float r, float g, float b);
		/**
		 * Sets the light's specular colour from a Point3r
		 *@param p_specular : The new position
		 */
		void SetSpecular( const Colour & p_specular);
		/**
		 * Writes the light in a file
		 *@param p_pFile : a pointer to file to write in
		 */
		virtual bool Write( Castor::Utils::File & p_pFile)const;
		/**
		 * Reads the light from a file
		 *@param p_file : a pointer to file to read from
		 */
		virtual bool Read( Castor::Utils::File & p_file);
		
	protected:
		/**
		 * Initialisation function, makes renderer apply colour values
		 */
		virtual void _initialise();

	private:
		/**
		 * Enables the light and applies it's position
		 */
		void _apply();
		/**
		 * Disables the light
		 */
		void _remove();

	public:
		/**
		 * Virtual function which returns the light type.
		 * Must be implemented for each new light type
		 */
		virtual eTYPE	GetLightType()const { return eTYPE( -1); }
		/**
		 *@return The light name
		 */
		inline String			GetName		()const { return m_name; }
		/**
		 *@returns The enable status
		 */
		inline bool				IsEnabled	()const { return m_enabled; }
		/**
		 *@return The light position
		 */
		inline const float *	GetPosition () { return m_position.ptr(); }
		/**
		 *@return The ambient colour value (RGBA)
		 */
		inline const float *	GetAmbient	() { return m_ambient.ptr(); }
		/**
		 *@return The diffuse colour value (RGBA)
		 */
		inline const float *	GetDiffuse	() { return m_diffuse.ptr(); }
		/**
		 *@return The specular colour value (RGBA)
		 */
		inline const float *	GetSpecular	() { return m_specular.ptr(); }
		/**
		 * Sets the enabled status, applies or remove the light
		 *@param p_enabled : The new enable status
		 */
		inline void SetEnabled( bool p_enabled){m_enabled = p_enabled;(m_enabled? _apply() : _remove());}
	};
	//! The Light renderer
	/*!
	Renders a light, applies it's colours values...
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class CS3D_API LightRenderer : public Renderer<Light, LightRenderer>
	{
	protected:
		/**
		 * Constructor
		 */
		LightRenderer()
		{}
	public:
		/**
		 * Destructor
		 */
		virtual ~LightRenderer(){ Cleanup(); }
		/**
		 * Enables (shows) the light
		 */
		virtual void Enable() = 0;
		/**
		 * Disables (hides) the light
		 */
		virtual void Disable() = 0;
		/**
		 * Cleans up the renderer
		 */
		virtual void Cleanup(){}
		/**
		 * Applies ambient colour
		 */
		virtual void ApplyAmbient( float * p_ambient) = 0;
		/**
		 * Applies diffuse colour
		 */
		virtual void ApplyDiffuse( float * p_diffuse) = 0;
		/**
		 * Applies specular colour
		 */
		virtual void ApplySpecular( float * p_specular) = 0;
		/**
		 * Applies position
		 */
		virtual void ApplyPosition( float * p_position) = 0;
		/**
		 * Applies rotation
		 */
		virtual void ApplyOrientation( real * p_matrix) = 0;
		/**
		 * Applies constant attenuation
		 */
		virtual void ApplyConstantAtt( float p_constant) = 0;
		/**
		 * Applies linear attenuation
		 */
		virtual void ApplyLinearAtt( float p_linear) = 0;
		/**
		 * Applies quadratic attenuation
		 */
		virtual void ApplyQuadraticAtt( float p_quadratic) = 0;
		/**
		 * Applies exponent
		 */
		virtual void ApplyExponent( float p_exponent) = 0;
		/**
		 * Applies cut off
		 */
		virtual void ApplyCutOff( float p_cutOff) = 0;
	};
}

#endif
