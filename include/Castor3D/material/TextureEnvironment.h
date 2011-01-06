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

#include "../Prerequisites.h"
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
	class C3D_API TextureEnvironment : public Renderable<TextureEnvironment, TextureEnvironmentRenderer>
	{
	public:
		//! Environment modes enumerator
		/*!
		The different environment modes for materials : Replace, Modulate, Blend, Add and Combine
		*/
		typedef enum
		{
			eMReplace	= 0,	//!< replace the previous texture
			eMModulate	= 1,	//!< modulate with the previous texture
			eMBlend		= 2,	//!< blend with the previous texture
			eMAdd		= 3,	//!< add to the previous texture
			eMCombine	= 4		//!< Combine with the previous texture, implies more configuration
		}
		eMODE;

		//! RGB Combinations enumerator
		/*!
		The different RGB combination modes : none, replace, modulate, add, add signed, dot 3 RGB, dot 3 RGBA, interpolate
		*/
		typedef enum
		{
			eCCNone			= 0,	//!< None
			eCCReplace		= 1,	//!< Arg0
			eCCModulate		= 2,	//!< Arg0 * Arg1
			eCCAdd			= 3,	//!< Arg0 + Arg1
			eCCAddSigned	= 4,	//!< Arg0 + Arg1 - 0.5
			eCCDot3RGB		= 5,	//!< 4 * ((Arg0r - 0.5) * (Arg1r - 0.5) + (Arg0g - 0.5) * (Arg1g - 0.5) + (Arg0b - 0.5) * (Arg1b - 0.5))
			eCCDot3RGBA		= 6,	//!< 4 * ((Arg0r - 0.5) * (Arg1r - 0.5) + (Arg0g - 0.5) * (Arg1g - 0.5) + (Arg0b - 0.5) * (Arg1b - 0.5) + (Arg0a - 0.5) * (Arg1a - 0.5))
			eCCInterpolate	= 7		//!< Arg0 * Arg2 + Arg1 * (1 - Arg2)
		}
		eRGB_COMBINATION;

		//! RGB Operands enumerator
		/*!
		The different RGB operands that can be used in textures environment
		*/
		typedef enum
		{
			eCOSrcColour			= 0,	//!< Uses source colour
			eCOOneMinusSrcColour	= 1,	//!< Uses 1 - source colour
			eCOSrcAlpha				= 2,	//!< Uses source alpha
			eCOOneMinusSrcAlpha		= 3		//!< Uses 1 - source alpha
		}
		eRGB_OPERAND;

		//! Alpha Combinations enumerator
		/*!
		The different Alpha combination modes : none, replace, modulate, add, add signed, interpolate
		*/
		typedef enum
		{
			eACNone			= 0,	//!< None
			eACReplace		= 1,	//!< Arg0
			eACModulate		= 2,	//!< Arg0 * Arg1
			eACAdd			= 3,	//!< Arg0 + Arg1
			eACAddSigned	= 4,	//!< Arg0 + Arg1 - 0.5
			eACInterpolate	= 5		//!< Arg0 * Arg2 + Arg1 * (1 - Arg2)
		}
		eALPHA_COMBINATION;

		//! Alpha Operands enumerator
		/*!
		The different Alpha operands that can be used in textures environment
		*/
		typedef enum
		{
			eAOSrcAlpha			= 0,	//!< Uses source alpha
			eAOOneMinusSrcAlpha	= 1		//!< Uses 1 - source alpha
		}
		eALPHA_OPERAND;

		//! Combination sources enumerator
		/*!
		The different combination sources are texture, current texture, constant, primary colour and previous
		*/
		typedef enum
		{
			eSrcTexture			= 0,	//!< The n-th texture unit is the source
			eSrcCurrentTexture	= 1,	//!< The current texture unit is the source
			eSrcConstant		= 2,	//!< The source is the defined constant colour
			eSrcPrimaryColour	= 3,	//!< The source is the fragment colour before texturing
			eSrcPrevious		= 4		//!< The source is the fragment colour before the texture
		}
		eCOMBINATION_SOURCE;

	private:
		eMODE				m_mode;							//!< The general environment mode
		eRGB_COMBINATION	m_RGBCombination;				//!< The RGB combination mode
		eALPHA_COMBINATION	m_alphaCombination;				//!< The Alpha combination mode
		Point3i				m_RGBCombinationSources;		//!< The three RGB combination sources (max three are used, look at the RGB combination modes descriptions to learn when)
		int					m_RGBTextureSourceIndex;		//!< The current RGB source index
		Point3i				m_alphaCombinationSources;		//!< The three Alpha combination sources (max three are used, look at the Alpha combination modes descriptions to learn when)
		int					m_alphaTextureSourceIndex;		//!< The current Alpha source index
		Point3i				m_RGBOperands;					//!< The three RGB operands (max three are used, look at the RGB operand descriptions to learn when)
		Point3i				m_alphaOperands;				//!< The three Alpha operands (max three are used, look at the Alpha operand descriptions to learn when)
		real				m_RGBScale;						//!< The current RGB scale
		real				m_alphaScale;					//!< The current Alpha scale
		bool				m_combineRGB;					//!< Tells whether or not the environment mode combines RGB
		bool				m_combineAlpha;					//!< Tells whether or not the environment mode combines Alpha

	public:
		/**
		 * Constructor
		 */
		TextureEnvironment();
		/**
		 * Copy constructor
		 */
		TextureEnvironment( const TextureEnvironment & p_copy);
		/**
		 * Assignment operator
		 */
		TextureEnvironment & operator =( const TextureEnvironment & p_copy);
		/**
		 * Destructor
		 */
		~TextureEnvironment();
		/**
		 * Apply the environment
		 */
		virtual void Render( eDRAW_TYPE p_displayMode);
		/**
		 * Sets RGB source for given index of given texture index
		 *@param p_index : The index of the source
		 *@param p_source : The source value
		 *@param p_textureIndex : The texture unit
		 */
		void SetRGBSource			( unsigned int p_index, eCOMBINATION_SOURCE p_source, int p_textureIndex=0);
		/**
		 * Sets Alpha source for given index of given texture index
		 *@param p_index : The index of the source
		 *@param p_source : The source value
		 *@param p_textureIndex : The texture unit
		 */
		void SetAlphaSource			( unsigned int p_index, eCOMBINATION_SOURCE p_source, int p_textureIndex=0);
		/**
		 * Sets RGB operand for given index
		 *@param p_index : The index of the operand
		 *@param p_operand : The operand value
		 */
		void SetRGBOperand			( unsigned int p_index, eRGB_OPERAND p_operand);
		/**
		 * Sets Alpha operand for given index
		 *@param p_index : The index of the operand
		 *@param p_operand : The operand value
		 */
		void SetAlphaOperand		( unsigned int p_index, eALPHA_OPERAND p_operand);
		/**
		 * Retrieves RGB source at given index of given texture index
		 */
		eCOMBINATION_SOURCE	GetRGBSource		( unsigned int p_index, int & p_textureIndex)const;
		/**
		 * Retrieves Alpha source at given index of given texture index
		 */
		eCOMBINATION_SOURCE	GetAlphaSource		( unsigned int p_index, int & p_textureIndex)const;
		/**
		 * Retrieves RGB operand at given index
		 */
		eRGB_OPERAND		GetRGBOperand		( unsigned int p_index)const;
		/**
		 * Retrieves Alpha operand at given index
		 */
		eALPHA_OPERAND		GetAlphaOperand		( unsigned int p_index)const;
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
		/**@name Accessors */
		//@{
		inline void SetMode					( eMODE p_mode)						{ m_mode = p_mode; }
		inline void SetRGBCombination		( eRGB_COMBINATION p_combination)	{ m_RGBCombination = p_combination; }
		inline void SetAlphaCombination		( eALPHA_COMBINATION p_combination)	{ m_alphaCombination = p_combination; }
		inline void SetRGBScale				( real p_scale) 					{ m_RGBScale = p_scale; }
		inline void SetAlphaScale			( real p_scale) 					{ m_alphaScale = p_scale; }

		inline eMODE				GetMode						()const { return m_mode; }
		inline eRGB_COMBINATION		GetRGBCombination			()const { return m_RGBCombination; }
		inline eALPHA_COMBINATION	GetAlphaCombination			()const { return m_alphaCombination; }
		inline real					GetRGBScale					()const { return m_RGBScale; }
		inline real					GetAlphaScale				()const { return m_alphaScale; }
		inline int					GetRGBTextureSourceIndex	()const { return m_RGBTextureSourceIndex; }
		inline int					GetAlphaTextureSourceIndex	()const { return m_alphaTextureSourceIndex; }
		inline const Point3i &		GetRGBSources				()const { return m_RGBCombinationSources; }
		inline const Point3i &		GetAlphaSources				()const { return m_alphaCombinationSources; }
		inline const Point3i &		GetRGBOperands				()const { return m_RGBOperands; }
		inline const Point3i &		GetAlphaOperands			()const { return m_alphaOperands; }
		inline Point3i &			GetRGBSources				()		{ return m_RGBCombinationSources; }
		inline Point3i &			GetAlphaSources				()		{ return m_alphaCombinationSources; }
		inline Point3i &			GetRGBOperands				()		{ return m_RGBOperands; }
		inline Point3i &			GetAlphaOperands			()		{ return m_alphaOperands; }
		//@}
	};
	//! The TextureEnvironment renderer
	/*!
	Applies all the combination functions of a texture environment
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class C3D_API TextureEnvironmentRenderer : public Renderer<TextureEnvironment, TextureEnvironmentRenderer>
	{
	protected:
		/**
		 * Constructor, only RenderSystem can use it
		 */
		TextureEnvironmentRenderer( SceneManager * p_pManager)
			:	Renderer<TextureEnvironment, TextureEnvironmentRenderer>( p_pManager)
		{}

	public:
		/**
		 * Destructor
		 */
		virtual ~TextureEnvironmentRenderer(){}
		/**
		 * Applies the combination functions
		 */
		virtual void Render() = 0;
	};
}

#endif
