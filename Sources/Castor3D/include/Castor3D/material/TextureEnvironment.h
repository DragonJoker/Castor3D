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
#ifndef ___CU_TextureEnvironment___
#define ___CU_TextureEnvironment___

#include "Module_Material.h"
#include "../render_system/Module_Render.h"
#include "../render_system/Renderable.h"

namespace Castor3D
{
	//! Textures combinations representation
	/*!
	A texture environment is a combination of modes, operands and sources used to configure texture units and their interactions in multi-textured materials
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	\todo Make the use of texture environment mode more easy to understand and user friendly
	*/
	class CS3D_API TextureEnvironment : public Renderable<TextureEnvironment, TextureEnvironmentRenderer>
	{
	private:
		EnvironmentMode m_mode;								//!< The general environment mode
		RGBCombination m_RGBCombination;					//!< The RGB combination mode
		AlphaCombination m_alphaCombination;				//!< The Alpha combination mode
		CombinationSource m_RGBCombinationSources[3];		//!< The three RGB combination sources (max three are used, look at the RGB combination modes descriptions to learn when)
		int m_RGBTextureSourceIndex;						//!< The current RGB source index
		CombinationSource m_alphaCombinationSources[3];		//!< The three Alpha combination sources (max three are used, look at the Alpha combination modes descriptions to learn when)
		int m_alphaTextureSourceIndex;						//!< The current Alpha source index
		RGBOperand m_RGBOperands[3];						//!< The three RGB operands (max three are used, look at the RGB operand descriptions to learn when)
		AlphaOperand m_alphaOperands[3];					//!< The three Alpha operands (max three are used, look at the Alpha operand descriptions to learn when)
		real m_RGBScale;									//!< The current RGB scale
		real m_alphaScale;									//!< The current Alpha scale
		bool m_combineRGB;									//!< Tells whether or not the environment mode combines RGB
		bool m_combineAlpha;								//!< Tells whether or not the environment mode combines Alpha

	public:
		/**
		 * Constructor
		 */
		TextureEnvironment();
		/**
		 * Destructor
		 */
		~TextureEnvironment();
		/**
		 * Apply the environment
		 */
		virtual void Apply( eDRAW_TYPE p_displayMode);
		/**
		 * Sets RGB source for given index of given texture index
		 *@param p_index : The index of the source
		 *@param p_source : The source value
		 *@param p_textureIndex : The texture unit
		 */
		void SetRGBSource			( unsigned int p_index, CombinationSource p_source, int p_textureIndex=0);
		/**
		 * Sets Alpha source for given index of given texture index
		 *@param p_index : The index of the source
		 *@param p_source : The source value
		 *@param p_textureIndex : The texture unit
		 */
		void SetAlphaSource			( unsigned int p_index, CombinationSource p_source, int p_textureIndex=0);
		/**
		 * Sets RGB operand for given index
		 *@param p_index : The index of the operand
		 *@param p_operand : The operand value
		 */
		void SetRGBOperand			( unsigned int p_index, RGBOperand p_operand);
		/**
		 * Sets Alpha operand for given index
		 *@param p_index : The index of the operand
		 *@param p_operand : The operand value
		 */
		void SetAlphaOperand		( unsigned int p_index, AlphaOperand p_operand);
		/**
		 * Retrieves RGB source at given index of given texture index
		 */
		CombinationSource	GetRGBSource		( unsigned int p_index, int & p_textureIndex)const;
		/**
		 * Retrieves Alpha source at given index of given texture index
		 */
		CombinationSource	GetAlphaSource		( unsigned int p_index, int & p_textureIndex)const;
		/**
		 * Retrieves RGB operand at given index
		 */
		RGBOperand			GetRGBOperand		( unsigned int p_index)const;
		/**
		 * Retrieves Alpha operand at given index
		 */
		AlphaOperand		GetAlphaOperand		( unsigned int p_index)const;
		/**
		 * Writes the texture environment in a file
		 *@param p_file : [in] The file to write in
		 *@return true if successful, false if not
		 */
		virtual bool Write( Castor::Utils::File & p_file)const;
		/**
		 * Reads the texture environment from a file
		 *@param p_file : [in] The file to read from
		 *@return true if successful, false if not
		 */
		virtual bool Read( Castor::Utils::File & p_file);

	public:
		inline void SetMode					( EnvironmentMode p_mode)			{ m_mode = p_mode; }
		inline void SetRGBCombination		( RGBCombination p_combination)		{ m_RGBCombination = p_combination; }
		inline void SetAlphaCombination		( AlphaCombination p_combination)	{ m_alphaCombination = p_combination; }
		inline void SetRGBScale				( real p_scale) 					{ m_RGBScale = p_scale; }
		inline void SetAlphaScale			( real p_scale) 					{ m_alphaScale = p_scale; }

		inline EnvironmentMode				GetMode						()const { return m_mode; }
		inline RGBCombination				GetRGBCombination			()const { return m_RGBCombination; }
		inline AlphaCombination				GetAlphaCombination			()const { return m_alphaCombination; }
		inline real							GetRGBScale					()const { return m_RGBScale; }
		inline real							GetAlphaScale				()const { return m_alphaScale; }
		inline const CombinationSource *	GetRGBSources				()const { return m_RGBCombinationSources; }
		inline const CombinationSource *	GetAlphaSources				()const { return m_alphaCombinationSources; }
		inline const RGBOperand *			GetRGBOperands				()const { return m_RGBOperands; }
		inline const AlphaOperand *			GetAlphaOperands			()const { return m_alphaOperands; }
		inline int							GetRGBTextureSourceIndex	()const { return m_RGBTextureSourceIndex; }
		inline int							GetAlphaTextureSourceIndex	()const { return m_alphaTextureSourceIndex; }
	};
	//! The TextureEnvironment renderer
	/*!
	Applies all the combination functions of a texture environment
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class CS3D_API TextureEnvironmentRenderer : public Renderer<TextureEnvironment, TextureEnvironmentRenderer>
	{
	protected:
		/**
		 * Constructor, only RenderSystem can use it
		 */
		TextureEnvironmentRenderer()
		{}

	public:
		/**
		 * Destructor
		 */
		virtual ~TextureEnvironmentRenderer(){}
		/**
		 * Applies the combination functions
		 */
		virtual void Apply() = 0;
	};
}

#endif
