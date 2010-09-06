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
#ifndef ___C3D_Light___
#define ___C3D_Light___

#include "../render_system/Module_Render.h"


namespace Castor3D
{
	//! Virtual class which represents a Light
	/*!
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	\todo Show only the 8 nearest lights, hide the other ones
	*/
	class CS3D_API Light
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
		//! The light name
		String m_name;
		//! Tells if the light is enabled (for the 8 active lights law)
		bool m_enabled;
		//! The light's position, 4 floats for OpenGL compatibility : x, y, z, and w. w is used to tell if the light is directional or point
		Point4D<float> m_position;
		//! The ambient colour values (RGBA)
		Colour m_ambient;
		//! The diffuse colour values (RGBA)
		Colour m_diffuse;
		//! The specular colour values (RGBA)
		Colour m_specular;
		//! This light's renderer
		LightRenderer * m_renderer;
		/**
		 * Constructor, not to be used by the user, use Scene::CreateLight function instead
		 *@param p_renderer : A light renderer, may be OpenGL or Direct3D
		 *@param p_name : The light's name, default is void
		 */
		Light( LightRenderer * p_renderer, const String & p_name);

	public:
		/**
		 * Destructor	
		 */
		~Light();
		/**
		 * Enables the light => Displays it
		 */
		void Enable();
		/**
		 * Disables the light => Hides it
		 */
		void Disable();
		/**
		 * Renders the light => Applies it's position
		 */
		virtual void Render();
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
		 * Sets the light's position from a Vector3f
		 *@param p_vertex : The new position
		 */
		void SetPosition( const Vector3f & p_vertex);
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
		 * Translates the light from a Vector3f
		 *@param p_vertex : The new position
		 */
		void Translate( const Vector3f & p_vertex);
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
		 * Sets the light's ambient colour from a Vector3f
		 *@param p_ambient : The new position
		 */
		void SetAmbient( const Vector3f & p_ambient);
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
		 * Sets the light's diffuse colour from a Vector3f
		 *@param p_diffuse : The new position
		 */
		void SetDiffuse( const Vector3f & p_diffuse);
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
		 * Sets the light's specular colour from a Vector3f
		 *@param p_specular : The new position
		 */
		void SetSpecular( const Vector3f & p_specular);
		/**
		 * Writes the light in a file
		 *@param p_pFile : a pointer to file to write in
		 */
		virtual bool Write( General::Utils::FileIO * p_pFile)const;
		/**
		 * Reads the light from a file
		 *@param p_file : a pointer to file to read from
		*@param p_scene : [in] The scene to find the node to attach this light to
		 */
		virtual bool Read( General::Utils::FileIO & p_file, Scene * p_scene);
		
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
		virtual inline eTYPE	GetLightType()const=0;
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
}

#endif
