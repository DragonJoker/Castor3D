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

#include "Module_Material.h"
#include "../shader/Module_Shader.h"
#include "../geometry/Module_Geometry.h"
#include "../render_system/Module_Render.h"
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
		Colour m_diffuse;			//!< Diffuse material colour
		Colour m_ambient;			//!< Ambient material colour
		Colour m_specular;			//!< Specular material colour
		Colour m_emissive;			//!< Emissive material colour
		float m_shininess;			//!< The shininess value
		float m_fAlpha;				//!< The alpha value
		Colour m_texBaseColour;		//!< Texture basic colour, used for environment modes
		bool m_doubleFace;

		Templates::WeakPtr<ShaderProgram> m_shaderProgram;

		TextureUnitPtrArray m_textureUnits;		//!< Texture units

		Material * m_pParent;

		/**
		 * Constructor, used by Material, don't use it.
		 */
		Pass( Material * p_parent);

	public:
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
		virtual void Render( eDRAW_TYPE p_displayMode);
		/**
		 * Applies the pass
		 */
		virtual void Render2D( eDRAW_TYPE p_displayMode);
		/**
		 * Removes the pass (to avoid it from interfering with other passes)
		 */
		virtual void EndRender();
		/**
		 * Adds a TextureUnit to the pass, set the unit's base colour to mine
		 *@param p_texUnit : [in] The TextureUnit to add
		 */
		void AddTextureUnit( TextureUnitPtr p_texUnit);
		/**
		 * Destroys a TextureUnit at the given index
		 *@param p_index : [in] the index of the TextureUnit to destroy
		 */
		bool DestroyTextureUnit( size_t p_index);
		/**
		 * Retrieves and returns the TextureUnit at the given index, NULL if it doesn't exist
		 *@param p_index : [in] The index of the TextureUnit to retrieve
		 *@return The retrieved TextureUnit, NULL if none
		 */
		TextureUnitPtr GetTextureUnit( unsigned int p_index);
		/**
		 * Retrieves the image path of the TextureUnit at the given index
		 *@param p_index : [in] The index of the TextureUnit we want the image path
		 *@return The path, void if none
		 */
		String GetTexturePath( unsigned int p_index);
		/**
		 * Sets the base texture colour
		 */
		void SetTexBaseColour( float p_r, float p_g, float p_b, float p_a);
		/**
		 * Sets the base texture colour
		 */
		void SetTexBaseColour( const Colour & p_crColour);
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
		void SetShader( ShaderProgramPtr p_program);
		ShaderProgramPtr GetShader()const;
		bool HasShader()const;

	public:
		inline void SetDiffuse			( float p_r, float p_g, float p_b, float p_a)	{ m_diffuse[0] = p_r;m_diffuse[1] = p_g;m_diffuse[2] = p_b;m_diffuse[3] = p_a; }
		inline void SetDiffuse			( const Colour & p_cColour)						{ m_diffuse = p_cColour; }
		inline void SetDiffuse			( float * p_diffuse)							{ m_diffuse[0] = p_diffuse[0];m_diffuse[1] = p_diffuse[1];m_diffuse[2] = p_diffuse[2]; }
		inline void SetAmbient			( float p_r, float p_g, float p_b, float p_a)	{ m_ambient[0] = p_r;m_ambient[1] = p_g;m_ambient[2] = p_b;m_ambient[3] = p_a; }
		inline void SetAmbient			( const Colour & p_cColour)						{ m_ambient = p_cColour; }
		inline void SetAmbient			( float * p_ambient)							{ m_ambient[0] = p_ambient[0];m_ambient[1] = p_ambient[1];m_ambient[2] = p_ambient[2]; }
		inline void SetSpecular			( float p_r, float p_g, float p_b, float p_a)	{ m_specular[0] = p_r;m_specular[1] = p_g;m_specular[2] = p_b;m_specular[3] = p_a; }
		inline void SetSpecular			( const Colour & p_cColour)						{ m_specular = p_cColour; }
		inline void SetSpecular			( float * p_specular)							{ m_specular[0] = p_specular[0];m_specular[1] = p_specular[1];m_specular[2] = p_specular[2]; }
		inline void SetEmissive			( float p_r, float p_g, float p_b, float p_a)	{ m_emissive[0] = p_r;m_emissive[1] = p_g;m_emissive[2] = p_b;m_emissive[3] = p_a; }
		inline void SetEmissive			( const Colour & p_cColour)						{ m_emissive = p_cColour; }
		inline void SetEmissive			( float * p_emissive)							{ m_emissive[0] = p_emissive[0];m_emissive[1] = p_emissive[1];m_emissive[2] = p_emissive[2]; }
		inline void SetShininess		( float p_shininess)							{ m_shininess = p_shininess; }
		inline void SetDoubleFace		( bool p_double)								{ m_doubleFace = p_double; }
		inline void SetAlpha			( float p_fAlpha)								{ m_fAlpha = p_fAlpha;m_diffuse[3] = p_fAlpha;m_ambient[3] = p_fAlpha;m_specular[3] = p_fAlpha;m_emissive[3] = p_fAlpha; }

		inline const float *		GetDiffuse			()const { return m_diffuse.const_ptr(); }
		inline const float *		GetAmbient			()const { return m_ambient.const_ptr(); }
		inline const float *		GetSpecular			()const { return m_specular.const_ptr(); }
		inline const float *		GetEmissive			()const { return m_emissive.const_ptr(); }
		inline float				GetShininess		()const { return m_shininess; }
		inline unsigned int			GetNbTexUnits		()const { return static_cast <unsigned int>( m_textureUnits.size()); }
		inline const float *		GetTexBaseColour	()const { return m_texBaseColour.const_ptr(); }
		inline TextureUnitPtrArray	GetTextureUnits		()const { return m_textureUnits; }
		inline bool					IsDoubleFace		()const { return m_doubleFace; }
		inline Material *			GetParent			()const { return m_pParent; }
		inline float				GetAlpha			()const { return m_fAlpha; }
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
		PassRenderer()
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
		virtual void Render( eDRAW_TYPE p_displayMode) = 0;
		/**
		 * Applies the material
		 */
		virtual void Render2D( eDRAW_TYPE p_displayMode) = 0;
		/**
		 * Removes the material
		 */
		virtual void EndRender() = 0;
	};
}

#endif