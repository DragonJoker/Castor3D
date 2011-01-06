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
#ifndef ___C3D_Pass___
#define ___C3D_Pass___

#include "../Prerequisites.h"
#include "../render_system/Renderable.h"

namespace Castor3D
{
	//! Definition of a material pass
	/*!
	A pass is composed of : base colours (ambient, diffuse, specular, emissive), shininess, 
	base texturing colour, texture units and texture units indexes. Shader programs can also be applied.
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class C3D_API Pass : public Renderable<Pass, PassRenderer>
	{
	protected:
		friend class Material;
		Colour									m_clrDiffuse;			//!< Diffuse material colour
		Colour									m_clrAmbient;			//!< Ambient material colour
		Colour									m_clrSpecular;			//!< Specular material colour
		Colour									m_clrEmissive;			//!< Emissive material colour
		float									m_fShininess;			//!< The shininess value
		float									m_fAlpha;				//!< The alpha value
		Colour									m_clrTexBaseColour;		//!< Texture basic colour, used for environment modes
		bool									m_bDoubleFace;			//!< Tells if the pass is double face
		SmartPtr<ShaderProgram>::Weak			m_pShaderProgram;		//!< The shader program, if any
		TextureUnitPtrArray						m_arrayTextureUnits;	//!< Texture units
		Material							*	m_pParent;				//!< The parent material


	public:
		/**
		 * Constructor, used by Material, don't use it.
		 */
		Pass( Material * p_pParent = NULL);
		Pass( const Pass & p_copy);
		Pass & operator =( const Pass & p_copy);
		/**
		 * Destructor
		 */
		~Pass();
		/**
		 * Initialises the pass and all it's texture units
		 */
		void Initialise();
		/**
		 * Applies the pass
		 */
		virtual void Render( eDRAW_TYPE p_eDisplayMode);
		/**
		 * Applies the pass
		 */
		virtual void Render2D();
		/**
		 * Removes the pass (to avoid it from interfering with other passes)
		 */
		virtual void EndRender();
		/**
		 * Adds a TextureUnit to the pass, set the unit's base colour to mine
		 */
		TextureUnitPtr AddTextureUnit();
		/**
		 * Destroys a TextureUnit at the given index
		 *@param p_uiIndex : [in] the index of the TextureUnit to destroy
		 */
		bool DestroyTextureUnit( size_t p_uiIndex);
		/**
		 * Retrieves and returns the TextureUnit at the given index, NULL if it doesn't exist
		 *@param p_uiIndex : [in] The index of the TextureUnit to retrieve
		 *@return The retrieved TextureUnit, NULL if none
		 */
		TextureUnitPtr GetTextureUnit( size_t p_uiIndex);
		/**
		 * Retrieves the image path of the TextureUnit at the given index
		 *@param p_uiIndex : [in] The index of the TextureUnit we want the image path
		 *@return The path, void if none
		 */
		String GetTexturePath( size_t p_uiIndex);
		/**
		 * Sets the base texture colour
		 */
		void SetTexBaseColour( float p_fRed, float p_fGreen, float p_fBlue, float p_fAlpha);
		/**
		 * Sets the base texture colour
		 */
		void SetTexBaseColour( const Colour & p_clrColour);
		/**
		 * Writes the material in a file
		 *@param p_file : [in] The file to write in
		 *@return true if successful, false if not
		 */
		virtual bool Write( Castor::Utils::File & p_file)const;
		/**
		 * Reads the material from a file
		 *@param p_file : [in] The file to read from
		 *@return true if successful, false if not
		 */
		virtual bool Read( Castor::Utils::File & p_file);
		/**
		 * Defines the shader program
		 *@param p_pProgram : [in] The shader program
		 */
		void SetShader( ShaderProgramPtr p_pProgram);
		/**
		 * Gives the current shader program
		 *@return The shader program, NULL if none
		 */
		template <typename T>
		typename SmartPtr<T>::Shared GetShader()const
		{
			typename SmartPtr<T>::Shared l_pReturn;

			if ( ! m_pShaderProgram.expired())
			{
				l_pReturn = static_pointer_cast<T>( m_pShaderProgram.lock());
			}

			return l_pReturn;
		}
		/**
		 * Returns true if the pass has a shader program
		 *@return true if the shader program has been set, false if not
		 */
		bool HasShader()const;

	public:
		/**@name Accessors */
		//@{
		inline void SetDiffuse		( float p_fRed, float p_fGreen, float p_fBlue, float p_fAlpha)	{ m_clrDiffuse[0] = p_fRed;m_clrDiffuse[1] = p_fGreen;m_clrDiffuse[2] = p_fBlue;m_clrDiffuse[3] = p_fAlpha; }
		inline void SetDiffuse		( const Colour & p_clrColour)									{ m_clrDiffuse = p_clrColour; }
		inline void SetDiffuse		( float * p_pDiffuse)											{ m_clrDiffuse[0] = p_pDiffuse[0];m_clrDiffuse[1] = p_pDiffuse[1];m_clrDiffuse[2] = p_pDiffuse[2]; }
		inline void SetAmbient		( float p_fRed, float p_fGreen, float p_fBlue, float p_fAlpha)	{ m_clrAmbient[0] = p_fRed;m_clrAmbient[1] = p_fGreen;m_clrAmbient[2] = p_fBlue;m_clrAmbient[3] = p_fAlpha; }
		inline void SetAmbient		( const Colour & p_clrColour)									{ m_clrAmbient = p_clrColour; }
		inline void SetAmbient		( float * p_pAmbient)											{ m_clrAmbient[0] = p_pAmbient[0];m_clrAmbient[1] = p_pAmbient[1];m_clrAmbient[2] = p_pAmbient[2]; }
		inline void SetSpecular		( float p_fRed, float p_fGreen, float p_fBlue, float p_fAlpha)	{ m_clrSpecular[0] = p_fRed;m_clrSpecular[1] = p_fGreen;m_clrSpecular[2] = p_fBlue;m_clrSpecular[3] = p_fAlpha; }
		inline void SetSpecular		( const Colour & p_clrColour)									{ m_clrSpecular = p_clrColour; }
		inline void SetSpecular		( float * p_pSpecular)											{ m_clrSpecular[0] = p_pSpecular[0];m_clrSpecular[1] = p_pSpecular[1];m_clrSpecular[2] = p_pSpecular[2]; }
		inline void SetEmissive		( float p_fRed, float p_fGreen, float p_fBlue, float p_fAlpha)	{ m_clrEmissive[0] = p_fRed;m_clrEmissive[1] = p_fGreen;m_clrEmissive[2] = p_fBlue;m_clrEmissive[3] = p_fAlpha; }
		inline void SetEmissive		( const Colour & p_clrColour)									{ m_clrEmissive = p_clrColour; }
		inline void SetEmissive		( float * p_pEmissive)											{ m_clrEmissive[0] = p_pEmissive[0];m_clrEmissive[1] = p_pEmissive[1];m_clrEmissive[2] = p_pEmissive[2]; }
		inline void SetShininess	( float p_fShininess)											{ m_fShininess = p_fShininess; }
		inline void SetDoubleFace	( bool p_bDouble)												{ m_bDoubleFace = p_bDouble; }
		inline void SetAlpha		( float p_fAlpha)												{ m_fAlpha = p_fAlpha;m_clrDiffuse[3] = p_fAlpha;m_clrAmbient[3] = p_fAlpha;m_clrSpecular[3] = p_fAlpha;m_clrEmissive[3] = p_fAlpha; }

		inline float			GetShininess		()const { return m_fShininess; }
		inline size_t			GetNbTexUnits		()const { return m_arrayTextureUnits.size(); }
		inline bool				IsDoubleFace		()const { return m_bDoubleFace; }
		inline Material *		GetParent			()const { return m_pParent; }
		inline float			GetAlpha			()const { return m_fAlpha; }
		inline const Colour &	GetDiffuse			()const { return m_clrDiffuse; }
		inline const Colour &	GetAmbient			()const { return m_clrAmbient; }
		inline const Colour &	GetSpecular			()const { return m_clrSpecular; }
		inline const Colour &	GetEmissive			()const { return m_clrEmissive; }
		inline const Colour &	GetTexBaseColour	()const { return m_clrTexBaseColour; }
		inline Colour &			GetDiffuse			()		{ return m_clrDiffuse; }
		inline Colour &			GetAmbient			()		{ return m_clrAmbient; }
		inline Colour &			GetSpecular			()		{ return m_clrSpecular; }
		inline Colour &			GetEmissive			()		{ return m_clrEmissive; }
		inline Colour &			GetTexBaseColour	()		{ return m_clrTexBaseColour; }
		//@}
	};
	//! The Pass renderer
	/*!
	Initialises a pass, draws it, removes it
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class C3D_API PassRenderer : public Renderer<Pass, PassRenderer>
	{
	protected:
		/**
		 * Constructor, only RenderSystem can use it
		 */
		PassRenderer( SceneManager * p_pManager)
			:	Renderer<Pass, PassRenderer>( p_pManager)
		{}

	public:
		/**
		 * Destructor
		 */
		virtual ~PassRenderer(){ Cleanup(); }
		/**
		 * Cleans up the renderer
		 */
		virtual void Cleanup(){}
		/**
		 * Initialises the material
		 */
		virtual void Initialise() = 0;
		/**
		 * Applies the material
		 */
		virtual void Render( eDRAW_TYPE p_eDisplayMode) = 0;
		/**
		 * Applies the material
		 */
		virtual void Render2D() = 0;
		/**
		 * Removes the material
		 */
		virtual void EndRender() = 0;
	};
}

#endif