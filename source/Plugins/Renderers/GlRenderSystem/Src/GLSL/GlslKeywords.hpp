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
#ifndef ___GLSL_KEYWORDS_H___
#define ___GLSL_KEYWORDS_H___

#include "GlRenderSystemPrerequisites.hpp"

#include <StreamIndentManipulators.hpp>

#ifdef max
#	undef max
#	undef min
#	undef abs
#endif

#ifdef OUT
#	undef OUT
#endif

#ifdef IN
#	undef IN
#endif

namespace GlRender
{
	namespace GLSL
	{
		class KeywordsBase
		{
		public:
			C3D_Gl_API static std::unique_ptr< GLSL::KeywordsBase > Get( OpenGl const & p_gl );
			inline Castor::String GetStdLayout( int p_index )const;
			inline Castor::String const & GetVersion()const;
			inline Castor::String const & GetIn()const;
			inline Castor::String const & GetOut()const;
			inline Castor::String const & GetTexture1D()const;
			inline Castor::String const & GetTexture2D()const;
			inline Castor::String const & GetTexture3D()const;
			inline Castor::String const & GetTexelFetch1D()const;
			inline Castor::String const & GetTexelFetch2D()const;
			inline Castor::String const & GetTexelFetch3D()const;
			inline Castor::String const & GetPixelOut()const;
			inline Castor::String const & GetPixelOutputName()const;
			inline Castor::String const & GetGSOutPositionName()const;
			inline Castor::String const & GetGSOutNormalName()const;
			inline Castor::String const & GetGSOutTangentName()const;
			inline Castor::String const & GetGSOutBitangentName()const;
			inline Castor::String const & GetGSOutDiffuseName()const;
			inline Castor::String const & GetGSOutSpecularName()const;
			inline Castor::String const & GetGSOutEmissiveName()const;
			inline Castor::String const & GetGSOutPositionDecl()const;
			inline Castor::String const & GetGSOutNormalDecl()const;
			inline Castor::String const & GetGSOutTangentDecl()const;
			inline Castor::String const & GetGSOutBitangentDecl()const;
			inline Castor::String const & GetGSOutDiffuseDecl()const;
			inline Castor::String const & GetGSOutSpecularDecl()const;
			inline Castor::String const & GetGSOutEmissiveDecl()const;

			C3D_Gl_API virtual Castor::String GetLayout( uint32_t p_index )const = 0;
			C3D_Gl_API virtual Castor::String GetAttribute( uint32_t p_index )const;
			C3D_Gl_API virtual bool HasNamedFragData()const = 0;
			C3D_Gl_API virtual Castor::String GetFragData( uint32_t p_index )const = 0;

		protected:
			Castor::String m_strStdLayout;
			Castor::String m_version;
			Castor::String m_strAttribute;
			Castor::String m_strIn;
			Castor::String m_strOut;
			Castor::String m_strTexture1D;
			Castor::String m_strTexture2D;
			Castor::String m_strTexture3D;
			Castor::String m_strTexelFetch1D;
			Castor::String m_strTexelFetch2D;
			Castor::String m_strTexelFetch3D;
			Castor::String m_strPixelOut;
			Castor::String m_strPixelOutputName;
			Castor::String m_strGSOutPositionName;
			Castor::String m_strGSOutNormalName;
			Castor::String m_strGSOutTangentName;
			Castor::String m_strGSOutBitangentName;
			Castor::String m_strGSOutDiffuseName;
			Castor::String m_strGSOutSpecularName;
			Castor::String m_strGSOutEmissiveName;
			Castor::String m_strGSOutPositionDecl;
			Castor::String m_strGSOutNormalDecl;
			Castor::String m_strGSOutTangentDecl;
			Castor::String m_strGSOutBitangentDecl;
			Castor::String m_strGSOutDiffuseDecl;
			Castor::String m_strGSOutSpecularDecl;
			Castor::String m_strGSOutEmissiveDecl;
		};

		template< int Version, class Enable = void > class Keywords;

		//! Language keywords
		struct Version
		{
		};
		struct Attribute
		{
		};
		struct In
		{
		};
		struct Out
		{
		};
		struct Layout
		{
		};
		struct Uniform
		{
		};
		struct StdLayout
		{
			int m_index;
		};
	}
}

#include "GlslKeywords.inl"

#endif
