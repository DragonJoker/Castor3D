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
#ifndef ___C3D_Light___
#define ___C3D_Light___

#include "MovableObject.hpp"
#include "Renderable.hpp"

namespace Castor
{	namespace Resources
{
	//! Light loader
	/*!
	Loads and saves lights from/into a file
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	template <> class C3D_API Loader<Castor3D::LightCategory>
	{
	public:
		/**
		 * Writes a light into a text file
		 *@param p_file : [in] the file to write the light in
		 *@param p_light : [in] the p_light to write
		 */
		static bool Write( const Castor3D::LightCategory & p_light, Utils::File & p_file);
	};
}
}

namespace Castor3D
{
	class LightCategory : public Serialisable, public Clonable<LightCategory>
	{
	private:
		eLIGHT_TYPE	m_eLightType;		//!< The light type
		Light		*	m_pLight;
		Colour		m_ambient;			//!< The ambient colour values (RGBA)
		Colour		m_diffuse;			//!< The diffuse colour values (RGBA)
		Colour		m_specular;			//!< The specular colour values (RGBA)
		Point4r		m_ptPositionType;	//!< The position and type of the light (type is in 4th coordinate)

	public:
		LightCategory( eLIGHT_TYPE p_eLightType);
		virtual ~LightCategory();
		/**
		 * Renders the light => Applies it's position
		 *@param p_displayMode : [in] The display mode
		 */
		virtual void Render( LightRendererPtr p_pRenderer, ePRIMITIVE_TYPE p_displayMode)=0;
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
		void SetAmbient( Colour const & p_ambient);
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
		void SetDiffuse( Colour const & p_diffuse);
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
		void SetSpecular( Colour const & p_specular);

		inline eLIGHT_TYPE		GetLightType	()const	{ return m_eLightType; }
		inline Colour const &	GetAmbient		()const	{ return m_ambient; }
		inline Colour const &	GetDiffuse		()const	{ return m_diffuse; }
		inline Colour const &	GetSpecular		()const	{ return m_specular; }
		inline Colour &			GetAmbient		()		{ return m_ambient; }
		inline Colour &			GetDiffuse		()		{ return m_diffuse; }
		inline Colour &			GetSpecular		()		{ return m_specular; }
		inline Point4r const &	GetPositionType	()const	{ return m_ptPositionType; }
		inline Point4r &		GetPositionType	()		{ return m_ptPositionType; }
		inline Light *			GetLight		()const { return m_pLight; }

		inline void SetLight		( Light * val)					{ m_pLight = val; }
		inline void SetLightType	( eLIGHT_TYPE val)				{ m_eLightType = val; }
		inline void	SetPositionType	( Point4r const & p_ptPosition)	{ m_ptPositionType = p_ptPosition; }

		DECLARE_SERIALISE_MAP()
	};
	//! Virtual class which represents a Light
	/*!
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	\todo Show only the 8 nearest lights, hide the other ones
	*/
	class C3D_API Light : public MovableObject, public Renderable<Light, LightRenderer>
	{
	protected:
		friend class Scene;
		bool 				m_enabled;			//!< Tells if the light is enabled (for the 8 active lights law)
		LightCategoryPtr	m_pCategory;

	public:
		/**
		 * Constructor
		 */
		Light( eLIGHT_TYPE p_eLightType);
		/**
		 * Constructor, not to be used by the user, use Scene::CreateLight function instead
		 *@param p_pNode : [in] The light's parent node
		 *@param p_name : [in] The light's name, default is void
		 */
		Light( Scene * p_pScene, SceneNodePtr p_pNode, String const & p_name, eLIGHT_TYPE p_eLightType);
		/**
		 * Destructor	
		 */
		~Light();
		/**
		 * Registers each light type
		 */
		static void Register();
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
		 *@param p_displayMode : [in] The display mode
		 */
		void Render( ePRIMITIVE_TYPE p_displayMode);
		/**
		 * Renders the light => Applies it's position
		 */
		void EndRender();
		/**
		 * Attaches this light to a Node
		 *@param p_pNode : [in] The new light's parent node
		 */
		virtual void AttachTo( SceneNode * p_pNode);

		/**@name Accessors */
		//@{
		inline eLIGHT_TYPE		GetLightType	()const	{ return m_pCategory->GetLightType(); }
		inline bool				IsEnabled		()const	{ return m_enabled; }
		inline Colour const &	GetAmbient		()const	{ return m_pCategory->GetAmbient(); }
		inline Colour const &	GetDiffuse		()const	{ return m_pCategory->GetDiffuse(); }
		inline Colour const &	GetSpecular		()const	{ return m_pCategory->GetSpecular(); }
		inline Colour &			GetAmbient		()		{ return m_pCategory->GetAmbient(); }
		inline Colour &			GetDiffuse		()		{ return m_pCategory->GetDiffuse(); }
		inline Colour &			GetSpecular		()		{ return m_pCategory->GetSpecular(); }
		inline Point4r const &	GetPositionType	()const	{ return m_pCategory->GetPositionType(); }
		inline Point4r &		GetPositionType	()		{ return m_pCategory->GetPositionType(); }
		inline LightCategoryPtr	GetLightCategory()const	{ return m_pCategory; }

		inline void SetAmbient	( float * p_ambient)			{ m_pCategory->SetAmbient( p_ambient); }
		inline void SetAmbient	( float r, float g, float b)	{ m_pCategory->SetAmbient( r, g, b); }
		inline void SetAmbient	( Colour const & p_ambient)		{ m_pCategory->SetAmbient( p_ambient); }
		inline void SetDiffuse	( float * p_diffuse)			{ m_pCategory->SetDiffuse( p_diffuse); }
		inline void SetDiffuse	( float r, float g, float b)	{ m_pCategory->SetDiffuse( r, g, b); }
		inline void SetDiffuse	( Colour const & p_diffuse)		{ m_pCategory->SetDiffuse( p_diffuse); }
		inline void SetSpecular	( float * p_specular)			{ m_pCategory->SetSpecular( p_specular); }
		inline void SetSpecular	( float r, float g, float b)	{ m_pCategory->SetSpecular( r, g, b); }
		inline void SetSpecular	( Colour const & p_specular)	{ m_pCategory->SetSpecular( p_specular); }
		inline void SetEnabled	( bool p_enabled)				{ m_enabled = p_enabled; }
		//@}
		
	protected:
		DECLARE_SERIALISE_MAP()
		DECLARE_PRE_SERIALISE()
		DECLARE_POST_UNSERIALISE()
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
		LightRenderer()
			:	Renderer<Light, LightRenderer>()
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
