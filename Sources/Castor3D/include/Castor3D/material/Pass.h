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
#ifndef ___C3D_Pass___
#define ___C3D_Pass___

#include "../shader/Module_Shader.h"
#include "../geometry/Module_Geometry.h"
#include "../render_system/Module_Render.h"

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
	class CS3D_API Pass
	{
	protected:
		friend class Material;
		Colour m_diffuse;				//!< Diffuse material colour
		Colour m_ambient;				//!< Ambient material colour
		Colour m_specular;				//!< Specular material colour
		Colour m_emissive;				//!< Emissive material colour
		float m_shininess;						//!< The shininess value
		float m_fAlpha;							//!< The alpha value
		Colour m_texBaseColour;		//!< Texture basic colour, used for environment modes
		bool m_doubleFace;

		ShaderProgram * m_shaderProgram;

		TextureUnitPtrArray m_textureUnits;		//!< Texture units

		Material * m_parent;

		PassRenderer * m_renderer;

		/**
		 * Constructor, used by Material, don't use it.
		 */
		Pass( PassRenderer * p_renderer, Material * p_parent);

	public:
		/**
		 * Destructor
		 */
		~Pass();
		/**
		 * Initialises this pass and all it's texture units
		 */
		void Initialise();
		/**
		 * Applies this pass
		 */
		void Apply( Submesh * p_submesh, DrawType p_displayMode);
		/**
		 * Applies this pass
		 */
		void Apply( DrawType p_displayMode);
		/**
		 * Removes this pass (to avoid it from interfering with other passes)
		 */
		void Remove();
		/**
		 * Adds a TextureUnit to this pass, set this unit's base colour to mine
		 *@param p_texUnit : [in] The TextureUnit to add
		 */
		void AddTextureUnit( TextureUnit * p_texUnit);
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
		TextureUnit * GetTextureUnit( unsigned int p_index);
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
		 * Writes this material in a file
		 *@param p_file : [in] The file to write in
		 *@return true if successful, false if not
		 */
		bool Write( General::Utils::FileIO * p_file)const;
		/**
		 * Reads this material from a file
		 *@param p_file : [in] The file to read from
		 *@return true if successful, false if not
		 */
		bool Read( General::Utils::FileIO & p_file);

	public:
		inline void SetDiffuse			( float p_r, float p_g, float p_b, float p_a)	{ m_diffuse.x = p_r;m_diffuse.y = p_g;m_diffuse.z = p_b;m_diffuse.w = p_a; }
		inline void SetDiffuse			( const Colour & p_cColour)						{ m_diffuse = p_cColour; }
		inline void SetDiffuse			( float * p_diffuse)							{ m_diffuse.x = p_diffuse[0];m_diffuse.y = p_diffuse[1];m_diffuse.z = p_diffuse[2]; }
		inline void SetAmbient			( float p_r, float p_g, float p_b, float p_a)	{ m_ambient.x = p_r;m_ambient.y = p_g;m_ambient.z = p_b;m_ambient.w = p_a; }
		inline void SetAmbient			( const Colour & p_cColour)						{ m_ambient = p_cColour; }
		inline void SetAmbient			( float * p_ambient)							{ m_ambient.x = p_ambient[0];m_ambient.y = p_ambient[1];m_ambient.z = p_ambient[2]; }
		inline void SetSpecular			( float p_r, float p_g, float p_b, float p_a)	{ m_specular.x = p_r;m_specular.y = p_g;m_specular.z = p_b;m_specular.w = p_a; }
		inline void SetSpecular			( const Colour & p_cColour)						{ m_specular = p_cColour; }
		inline void SetSpecular			( float * p_specular)							{ m_specular.x = p_specular[0];m_specular.y = p_specular[1];m_specular.z = p_specular[2]; }
		inline void SetEmissive			( float p_r, float p_g, float p_b, float p_a)	{ m_emissive.x = p_r;m_emissive.y = p_g;m_emissive.z = p_b;m_emissive.w = p_a; }
		inline void SetEmissive			( const Colour & p_cColour)						{ m_emissive = p_cColour; }
		inline void SetEmissive			( float * p_emissive)							{ m_emissive.x = p_emissive[0];m_emissive.y = p_emissive[1];m_emissive.z = p_emissive[2]; }
		inline void SetShininess		( float p_shininess)							{ m_shininess = p_shininess; }
		inline void SetDoubleFace		( bool p_double)								{ m_doubleFace = p_double; }
		inline void SetShader			( ShaderProgram * p_program)					{ m_shaderProgram = p_program; }
		inline void SetAlpha			( float p_fAlpha)								{ m_fAlpha = p_fAlpha;m_diffuse.w = p_fAlpha;m_ambient.w = p_fAlpha;m_specular.w = p_fAlpha;m_emissive.w = p_fAlpha; }

		inline const float *		GetDiffuse			()const { return m_diffuse.const_ptr(); }
		inline const float *		GetAmbient			()const { return m_ambient.const_ptr(); }
		inline const float *		GetSpecular			()const { return m_specular.const_ptr(); }
		inline const float *		GetEmissive			()const { return m_emissive.const_ptr(); }
		inline float				GetShininess		()const { return m_shininess; }
		inline unsigned int			GetNbTexUnits		()const { return static_cast <unsigned int>( m_textureUnits.size()); }
		inline const float *		GetTexBaseColour	()const { return m_texBaseColour.const_ptr(); }
		inline TextureUnitPtrArray	GetTextureUnits		()const { return m_textureUnits; }
		inline bool					IsDoubleFace		()const { return m_doubleFace; }
		inline ShaderProgram *		GetShader			()const { return m_shaderProgram; }
		inline Material *			GetParent			()const { return m_parent; }
		inline PassRenderer *		GetRenderer			()const { return m_renderer; }
		inline float				GetAlpha			()const { return m_fAlpha; }
	};
}

#endif