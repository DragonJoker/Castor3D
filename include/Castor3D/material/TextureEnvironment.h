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
	class C3D_API TextureEnvironment : public Textable, public Serialisable, public Renderable<TextureEnvironment, TextureEnvironmentRenderer>
	{
	public:
		//! Environment modes enumerator
		/*!
		The different environment modes for materials : replace, Modulate, Blend, Add and Combine
		*/
		typedef enum
		{
			eModeReplace	= 1,	//!< replace the previous pixel
			eModeModulate	= 2,	//!< modulate with the previous pixel
			eModeBlend		= 3,	//!< blend with the previous pixel
			eModeAdd		= 4,	//!< add to the previous pixel
			eModeCombine	= 5,	//!< Combine with the previous pixel, implies more configuration
			eNbEnvModes
		}
		eMODE;
		//! RGB Combinations enumerator
		/*!
		The different RGB combination modes : none, replace, modulate, add, add signed, dot 3 RGB, dot 3 RGBA, interpolate
		*/
		typedef enum
		{
			eRgbCombiNone			= 0,	//!< None
			eRgbCombiReplace		= 1,	//!< Arg0
			eRgbCombiModulate		= 2,	//!< Arg0 * Arg1
			eRgbCombiAdd			= 3,	//!< Arg0 + Arg1
			eRgbCombiAddSigned		= 4,	//!< Arg0 + Arg1 - 0.5
			eRgbCombiDot3Rgb		= 5,	//!< 4 * ((Arg0r - 0.5) * (Arg1r - 0.5) + (Arg0g - 0.5) * (Arg1g - 0.5) + (Arg0b - 0.5) * (Arg1b - 0.5))
			eRgbCombiDot3Rgba		= 6,	//!< 4 * ((Arg0r - 0.5) * (Arg1r - 0.5) + (Arg0g - 0.5) * (Arg1g - 0.5) + (Arg0b - 0.5) * (Arg1b - 0.5) + (Arg0a - 0.5) * (Arg1a - 0.5))
			eRgbCombiInterpolate	= 7,	//!< Arg0 * Arg2 + Arg1 * (1 - Arg2)
			eNbRgbCombinations
		}
		eRGB_COMBINATION;

		//! RGB Operands enumerator
		/*!
		The different RGB operands that can be used in textures environment
		*/
		typedef enum
		{
			eRgbOpeSrcColour			= 0,	//!< Uses source colour
			eRgbOpeOneMinusSrcColour	= 1,	//!< Uses 1 - source colour
			eRgbOpeSrcAlpha				= 2,	//!< Uses source alpha
			eRgbOpeOneMinusSrcAlpha		= 3,	//!< Uses 1 - source alpha
			eNbRgbOperands
		}
		eRGB_OPERAND;
		//! Alpha Combinations enumerator
		/*!
		The different Alpha combination modes : none, replace, modulate, add, add signed, interpolate
		*/
		typedef enum
		{
			eAlphaCombiNone			= 0,	//!< None
			eAlphaCombiReplace		= 1,	//!< Arg0
			eAlphaCombiModulate		= 2,	//!< Arg0 * Arg1
			eAlphaCombiAdd			= 3,	//!< Arg0 + Arg1
			eAlphaCombiAddSigned	= 4,	//!< Arg0 + Arg1 - 0.5
			eAlphaCombiInterpolate	= 5,	//!< Arg0 * Arg2 + Arg1 * (1 - Arg2)
			eNbAlphaCombinations
		}
		eALPHA_COMBINATION;
		//! Alpha Operands enumerator
		/*!
		The different Alpha operands that can be used in textures environment
		*/
		typedef enum
		{
			eAlphaOpeSrcAlpha			= 0,	//!< Uses source alpha
			eAlphaOpeOneMinusSrcAlpha	= 1,	//!< Uses 1 - source alpha
			eNbAlphaOperands
		}
		eALPHA_OPERAND;
		//! Combination sources enumerator
		/*!
		The different combination sources are texture, current texture, constant, primary colour and previous
		*/
		typedef enum
		{
			eCombiSrcTexture			= 0,	//!< The n-th texture unit is the source
			eCombiSrcCurrentTexture		= 1,	//!< The current texture unit is the source
			eCombiSrcConstant			= 2,	//!< The source is the defined constant colour
			eCombiSrcPrimaryColour		= 3,	//!< The source is the fragment colour before texturing
			eCombiSrcPrevious			= 4,	//!< The source is the fragment colour before the texture
			eNbCombinationSources
		}
		eCOMBINATION_SOURCE;

		static const String StringMode[eNbEnvModes];
		static const String StringRgbCombinations[eNbRgbCombinations];
		static const String StringRgbOperands[eNbRgbOperands];
		static const String StringAlphaCombinations[eNbAlphaCombinations];
		static const String StringAlphaOperands[eNbAlphaOperands];
		static const String StringCombinationSources[eNbCombinationSources];

		static std::map <String, eMODE> MapModes;
		static std::map <String, eRGB_COMBINATION> MapRgbCombinations;
		static std::map <String, eRGB_OPERAND> MapRgbOperands;
		static std::map <String, eALPHA_COMBINATION> MapAlphaCombinations;
		static std::map <String, eALPHA_OPERAND> MapAlphaOperands;
		static std::map <String, eCOMBINATION_SOURCE> MapCombinationSources;

	private:
		eMODE				m_eMode;						//!< The general environment mode

		bool				m_bCombineRgb;					//!< Tells whether or not the environment mode combines RGB
		eRGB_COMBINATION	m_eRgbCombination;				//!< The RGB combination mode
		Point3i				m_ptRgbCombinationSources;		//!< The three RGB combination sources (max three are used, look at the RGB combination modes descriptions to learn when)
		int					m_iRgbTextureSourceIndex;		//!< The current RGB source index
		Point3i				m_ptRgbOperands;				//!< The three RGB operands (max three are used, look at the RGB operand descriptions to learn when)
		real				m_rRgbScale;					//!< The current RGB scale

		bool				m_bCombineAlpha;				//!< Tells whether or not the environment mode combines Alpha
		eALPHA_COMBINATION	m_eAlphaCombination;			//!< The Alpha combination mode
		Point3i				m_ptAlphaCombinationSources;	//!< The three Alpha combination sources (max three are used, look at the Alpha combination modes descriptions to learn when)
		int					m_iAlphaTextureSourceIndex;		//!< The current Alpha source index
		Point3i				m_ptAlphaOperands;				//!< The three Alpha operands (max three are used, look at the Alpha operand descriptions to learn when)
		real				m_rAlphaScale;					//!< The current Alpha scale

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
		virtual void Render( ePRIMITIVE_TYPE p_displayMode);
		/**
		 * Sets RGB source for given index of given texture index
		 *@param p_index : The index of the source
		 *@param p_source : The source value
		 *@param p_textureIndex : The texture unit
		 */
		void SetRgbSource			( unsigned int p_index, eCOMBINATION_SOURCE p_source, int p_textureIndex=0);
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
		void SetRgbOperand			( unsigned int p_index, eRGB_OPERAND p_operand);
		/**
		 * Sets Alpha operand for given index
		 *@param p_index : The index of the operand
		 *@param p_operand : The operand value
		 */
		void SetAlphaOperand		( unsigned int p_index, eALPHA_OPERAND p_operand);
		/**
		 * Retrieves RGB source at given index of given texture index
		 */
		eCOMBINATION_SOURCE	GetRgbSource		( unsigned int p_index, int & p_textureIndex)const;
		/**
		 * Retrieves Alpha source at given index of given texture index
		 */
		eCOMBINATION_SOURCE	GetAlphaSource		( unsigned int p_index, int & p_textureIndex)const;
		/**
		 * Retrieves RGB operand at given index
		 */
		eRGB_OPERAND		GetRgbOperand		( unsigned int p_index)const;
		/**
		 * Retrieves Alpha operand at given index
		 */
		eALPHA_OPERAND		GetAlphaOperand		( unsigned int p_index)const;

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
		inline void SetMode					( eMODE p_mode)						{ m_eMode = p_mode; }
		inline void SetRgbCombination		( eRGB_COMBINATION p_combination)	{ m_eRgbCombination = p_combination; }
		inline void SetAlphaCombination		( eALPHA_COMBINATION p_combination)	{ m_eAlphaCombination = p_combination; }
		inline void SetRgbScale				( real p_scale) 					{ m_rRgbScale = p_scale; }
		inline void SetAlphaScale			( real p_scale) 					{ m_rAlphaScale = p_scale; }
		inline eMODE				GetMode						()const { return m_eMode; }
		inline eRGB_COMBINATION		GetRgbCombination			()const { return m_eRgbCombination; }
		inline eALPHA_COMBINATION	GetAlphaCombination			()const { return m_eAlphaCombination; }
		inline real					GetRgbScale					()const { return m_rRgbScale; }
		inline real					GetAlphaScale				()const { return m_rAlphaScale; }
		inline int					GetRgbTextureSourceIndex	()const { return m_iRgbTextureSourceIndex; }
		inline int					GetAlphaTextureSourceIndex	()const { return m_iAlphaTextureSourceIndex; }
		inline const Point3i &		GetRgbSources				()const { return m_ptRgbCombinationSources; }
		inline const Point3i &		GetAlphaSources				()const { return m_ptAlphaCombinationSources; }
		inline const Point3i &		GetRgbOperands				()const { return m_ptRgbOperands; }
		inline const Point3i &		GetAlphaOperands			()const { return m_ptAlphaOperands; }
		inline Point3i &			GetRgbSources				()		{ return m_ptRgbCombinationSources; }
		inline Point3i &			GetAlphaSources				()		{ return m_ptAlphaCombinationSources; }
		inline Point3i &			GetRgbOperands				()		{ return m_ptRgbOperands; }
		inline Point3i &			GetAlphaOperands			()		{ return m_ptAlphaOperands; }
		//@}

		private:
			void _initialiseMaps();
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
