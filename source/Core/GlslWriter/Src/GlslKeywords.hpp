/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___GLSL_KEYWORDS_H___
#define ___GLSL_KEYWORDS_H___

#include "GlslWriterPrerequisites.hpp"

#include <Stream/StreamIndentManipulators.hpp>

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

namespace GLSL
{
	class KeywordsBase
	{
	public:
		GlslWriter_API static std::unique_ptr< GLSL::KeywordsBase > Get( GlslWriterConfig const & p_rs );
		inline Castor::String GetStdLayout( int p_index )const;
		inline Castor::String const & GetVersion()const;
		inline Castor::String const & GetIn()const;
		inline Castor::String const & GetOut()const;
		inline Castor::String const & GetTexture1D()const;
		inline Castor::String const & GetTexture2D()const;
		inline Castor::String const & GetTexture3D()const;
		inline Castor::String const & GetTextureCube()const;
		inline Castor::String const & GetTexelFetchBuffer()const;
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

		GlslWriter_API virtual Castor::String GetLayout( uint32_t p_index )const = 0;
		GlslWriter_API virtual Castor::String GetAttribute( uint32_t p_index )const;
		GlslWriter_API virtual bool HasNamedFragData()const = 0;
		GlslWriter_API virtual Castor::String GetFragData( uint32_t p_index )const = 0;

	protected:
		Castor::String m_strStdLayout;
		Castor::String m_version;
		Castor::String m_strAttribute;
		Castor::String m_strIn;
		Castor::String m_strOut;
		Castor::String m_strTexture1D;
		Castor::String m_strTexture2D;
		Castor::String m_strTexture3D;
		Castor::String m_strTextureCube;
		Castor::String m_strTexelFetchBuffer;
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
		int m_index;
	};
	struct Uniform
	{
	};
	struct StdLayout
	{
		int m_index;
	};
}

#include "GlslKeywords.inl"

#endif
