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

#include "GlslUbo.hpp"
#include "GlslSsbo.hpp"

namespace GLSL
{
	class KeywordsBase
	{
	public:
		GlslWriter_API static std::unique_ptr< GLSL::KeywordsBase > Get( GlslWriterConfig const & p_rs );
		GlslWriter_API Castor::String GetLayout (Ubo::Layout p_layout, uint32_t p_index)const;
		GlslWriter_API Castor::String GetLayout( Ssbo::Layout p_layout, uint32_t p_index )const;

		inline Castor::String const & GetVersion()const;
		inline Castor::String const & GetIn()const;
		inline Castor::String const & GetOut()const;
		inline Castor::String const & GetTexture1D()const;
		inline Castor::String const & GetTexture1DLod()const;
		inline Castor::String const & GetTexture1DOffset()const;
		inline Castor::String const & GetTexture1DOffsetLod()const;
		inline Castor::String const & GetTexture1DLodOffset()const;
		inline Castor::String const & GetTexture2D()const;
		inline Castor::String const & GetTexture2DLod()const;
		inline Castor::String const & GetTexture2DOffset()const;
		inline Castor::String const & GetTexture2DOffsetLod()const;
		inline Castor::String const & GetTexture2DLodOffset()const;
		inline Castor::String const & GetTexture3D()const;
		inline Castor::String const & GetTexture3DLod()const;
		inline Castor::String const & GetTexture3DOffset()const;
		inline Castor::String const & GetTexture3DOffsetLod()const;
		inline Castor::String const & GetTexture3DLodOffset()const;
		inline Castor::String const & GetTextureCube()const;
		inline Castor::String const & GetTextureCubeLod()const;
		inline Castor::String const & GetTexture1DArray()const;
		inline Castor::String const & GetTexture1DArrayLod()const;
		inline Castor::String const & GetTexture1DArrayOffset()const;
		inline Castor::String const & GetTexture1DArrayOffsetLod()const;
		inline Castor::String const & GetTexture1DArrayLodOffset()const;
		inline Castor::String const & GetTexture2DArray()const;
		inline Castor::String const & GetTexture2DArrayLod()const;
		inline Castor::String const & GetTexture2DArrayOffset()const;
		inline Castor::String const & GetTexture2DArrayOffsetLod()const;
		inline Castor::String const & GetTexture2DArrayLodOffset()const;
		inline Castor::String const & GetTextureCubeArray()const;
		inline Castor::String const & GetTextureCubeArrayLod()const;
		inline Castor::String const & GetTexture1DShadow()const;
		inline Castor::String const & GetTexture1DShadowLod()const;
		inline Castor::String const & GetTexture1DShadowOffset()const;
		inline Castor::String const & GetTexture1DShadowOffsetLod()const;
		inline Castor::String const & GetTexture1DShadowLodOffset()const;
		inline Castor::String const & GetTexture2DShadow()const;
		inline Castor::String const & GetTexture2DShadowLod()const;
		inline Castor::String const & GetTexture2DShadowOffset()const;
		inline Castor::String const & GetTexture2DShadowOffsetLod()const;
		inline Castor::String const & GetTexture2DShadowLodOffset()const;
		inline Castor::String const & GetTextureCubeShadow()const;
		inline Castor::String const & GetTextureCubeShadowLod()const;
		inline Castor::String const & GetTexture1DArrayShadow()const;
		inline Castor::String const & GetTexture1DArrayShadowLod()const;
		inline Castor::String const & GetTexture1DArrayShadowOffset()const;
		inline Castor::String const & GetTexture1DArrayShadowOffsetLod()const;
		inline Castor::String const & GetTexture1DArrayShadowLodOffset()const;
		inline Castor::String const & GetTexture2DArrayShadow()const;
		inline Castor::String const & GetTexture2DArrayShadowLod()const;
		inline Castor::String const & GetTexture2DArrayShadowOffset()const;
		inline Castor::String const & GetTexture2DArrayShadowOffsetLod()const;
		inline Castor::String const & GetTexture2DArrayShadowLodOffset()const;
		inline Castor::String const & GetTextureCubeArrayShadow()const;
		inline Castor::String const & GetTextureCubeArrayShadowLod()const;
		inline Castor::String const & GetTexelFetchBuffer()const;
		inline Castor::String const & GetTexelFetch1D()const;
		inline Castor::String const & GetTexelFetch2D()const;
		inline Castor::String const & GetTexelFetch3D()const;

		GlslWriter_API virtual Castor::String GetLayout( uint32_t p_index )const = 0;
		GlslWriter_API virtual Castor::String GetAttribute( uint32_t p_index )const;
		GlslWriter_API virtual bool HasNamedFragData()const = 0;
		GlslWriter_API virtual Castor::String GetFragData( uint32_t p_index )const = 0;

	protected:
		Castor::String m_strUboLayout;
		Castor::String m_strUboBinding;
		Castor::String m_strSsboLayout;
		Castor::String m_strSsboBinding;
		Castor::String m_version;
		Castor::String m_strAttribute;
		Castor::String m_strIn;
		Castor::String m_strOut;
		Castor::String m_strTexture1D;
		Castor::String m_strTexture1DLod;
		Castor::String m_strTexture1DOffset;
		Castor::String m_strTexture1DOffsetLod;
		Castor::String m_strTexture1DLodOffset;
		Castor::String m_strTexture2D;
		Castor::String m_strTexture2DLod;
		Castor::String m_strTexture2DOffset;
		Castor::String m_strTexture2DOffsetLod;
		Castor::String m_strTexture2DLodOffset;
		Castor::String m_strTexture3D;
		Castor::String m_strTexture3DLod;
		Castor::String m_strTexture3DOffset;
		Castor::String m_strTexture3DOffsetLod;
		Castor::String m_strTexture3DLodOffset;
		Castor::String m_strTextureCube;
		Castor::String m_strTextureCubeLod;
		Castor::String m_strTexture1DArray;
		Castor::String m_strTexture1DArrayLod;
		Castor::String m_strTexture1DArrayOffset;
		Castor::String m_strTexture1DArrayOffsetLod;
		Castor::String m_strTexture1DArrayLodOffset;
		Castor::String m_strTexture2DArray;
		Castor::String m_strTexture2DArrayLod;
		Castor::String m_strTexture2DArrayOffset;
		Castor::String m_strTexture2DArrayOffsetLod;
		Castor::String m_strTexture2DArrayLodOffset;
		Castor::String m_strTextureCubeArray;
		Castor::String m_strTextureCubeArrayLod;
		Castor::String m_strTexture1DShadow;
		Castor::String m_strTexture1DShadowLod;
		Castor::String m_strTexture1DShadowOffset;
		Castor::String m_strTexture1DShadowOffsetLod;
		Castor::String m_strTexture1DShadowLodOffset;
		Castor::String m_strTexture2DShadow;
		Castor::String m_strTexture2DShadowLod;
		Castor::String m_strTexture2DShadowOffset;
		Castor::String m_strTexture2DShadowOffsetLod;
		Castor::String m_strTexture2DShadowLodOffset;
		Castor::String m_strTextureCubeShadow;
		Castor::String m_strTextureCubeShadowLod;
		Castor::String m_strTexture1DArrayShadow;
		Castor::String m_strTexture1DArrayShadowLod;
		Castor::String m_strTexture1DArrayShadowOffset;
		Castor::String m_strTexture1DArrayShadowOffsetLod;
		Castor::String m_strTexture1DArrayShadowLodOffset;
		Castor::String m_strTexture2DArrayShadow;
		Castor::String m_strTexture2DArrayShadowLod;
		Castor::String m_strTexture2DArrayShadowOffset;
		Castor::String m_strTexture2DArrayShadowOffsetLod;
		Castor::String m_strTexture2DArrayShadowLodOffset;
		Castor::String m_strTextureCubeArrayShadow;
		Castor::String m_strTextureCubeArrayShadowLod;
		Castor::String m_strTexelFetchBuffer;
		Castor::String m_strTexelFetch1D;
		Castor::String m_strTexelFetch2D;
		Castor::String m_strTexelFetch3D;
	};
}

#include "GlslKeywords.inl"

#endif
