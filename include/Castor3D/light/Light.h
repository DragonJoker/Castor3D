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

#include "../main/MovableObject.h"
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
	class C3D_API Light : public MovableObject, public Renderable<Light, LightRenderer>
	{
	public:
		//! The light types enumerator
		/*!
		Defines the three different light types : directional, point and spot
		*/
		typedef enum
		{
			eDirectional,	//!< Directional light type
			ePoint,			//!< Point light type
			eSpot,			//!< Spot light type
			eNbLightTypes,
		}
		eLIGHT_TYPE;

	protected:
		friend class Scene;
		bool m_enabled;				//!< Tells if the light is enabled (for the 8 active lights law)
		Colour m_ambient;			//!< The ambient colour values (RGBA)
		Colour m_diffuse;			//!< The diffuse colour values (RGBA)
		Colour m_specular;			//!< The specular colour values (RGBA)
		Point4r m_ptPositionType;	//!< The position and type of the light (type is in 4th coordinate)
		eLIGHT_TYPE m_eLightType;	//!< The light type

	protected:
		/**
		 * Constructor, not to be used by the user, use Scene::CreateLight function instead
		 *@param p_pNode : [in] The light's parent node
		 *@param p_name : [in] The light's name, default is void
		 */
		Light( Scene * p_pScene, SceneNodePtr p_pNode, const String & p_name, eLIGHT_TYPE p_eLightType);

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
		 *@param p_displayMode : [in] The display mode
		 */
		virtual void Render( ePRIMITIVE_TYPE p_displayMode);
		/**
		 * Renders the light => Applies it's position
		 */
		virtual void EndRender();
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
		 * Attaches this light to a Node
		 *@param p_pNode : [in] The new light's parent node
		 */
		virtual void AttachTo( SceneNode * p_pNode);

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
		virtual eLIGHT_TYPE		GetLightType	()const { return m_eLightType; }
		inline bool				IsEnabled		()const { return m_enabled; }
		inline const Colour &	GetAmbient		()const { return m_ambient; }
		inline const Colour &	GetDiffuse		()const { return m_diffuse; }
		inline const Colour &	GetSpecular		()const { return m_specular; }
		inline const Point4r &	GetPositionType	()const { return m_ptPositionType; }
		inline void SetEnabled( bool p_enabled){m_enabled = p_enabled;(m_enabled? _apply() : _remove());}
		//@}
		
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
	};
	//! The Light renderer
	/*!
	Renders a light, applies it's colours values...
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class C3D_API LightRenderer : public Renderer<Light, LightRenderer>
	{
	protected:
		/**
		 * Constructor
		 */
		LightRenderer( SceneManager * p_pManager)
			:	Renderer<Light, LightRenderer>( p_pManager)
		{}
	public:
		/**
		 * Destructor
		 */
		virtual ~LightRenderer(){ Cleanup(); }

		virtual void Initialise(){}
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
		virtual void ApplyAmbient() = 0;
		/**
		 * Applies diffuse colour
		 */
		virtual void ApplyDiffuse() = 0;
		/**
		 * Applies specular colour
		 */
		virtual void ApplySpecular() = 0;
		/**
		 * Applies position
		 */
		virtual void ApplyPosition() = 0;
		/**
		 * Applies rotation
		 */
		virtual void ApplyOrientation() = 0;
		/**
		 * Applies constant attenuation
		 */
		virtual void ApplyConstantAtt( float p_fConstant) = 0;
		/**
		 * Applies linear attenuation
		 */
		virtual void ApplyLinearAtt( float p_fLinear) = 0;
		/**
		 * Applies quadratic attenuation
		 */
		virtual void ApplyQuadraticAtt( float p_fQuadratic) = 0;
		/**
		 * Applies exponent
		 */
		virtual void ApplyExponent( float p_fExponent) = 0;
		/**
		 * Applies cut off
		 */
		virtual void ApplyCutOff( float p_fCutOff) = 0;
	};
}

#endif
