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
		GlslWriter_API static std::unique_ptr< GLSL::KeywordsBase > get( GlslWriterConfig const & p_rs );
		GlslWriter_API castor::String getLayout (Ubo::Layout p_layout, uint32_t p_index)const;
		GlslWriter_API castor::String getLayout( Ssbo::Layout p_layout, uint32_t p_index )const;

		inline castor::String const & getVersion()const;
		inline castor::String const & getIn()const;
		inline castor::String const & getOut()const;
		inline castor::String const & getTexture1D()const;
		inline castor::String const & getTexture1DLod()const;
		inline castor::String const & getTexture1DOffset()const;
		inline castor::String const & getTexture1DOffsetLod()const;
		inline castor::String const & getTexture1DLodOffset()const;
		inline castor::String const & getTexture2D()const;
		inline castor::String const & getTexture2DLod()const;
		inline castor::String const & getTexture2DOffset()const;
		inline castor::String const & getTexture2DOffsetLod()const;
		inline castor::String const & getTexture2DLodOffset()const;
		inline castor::String const & getTexture3D()const;
		inline castor::String const & getTexture3DLod()const;
		inline castor::String const & getTexture3DOffset()const;
		inline castor::String const & getTexture3DOffsetLod()const;
		inline castor::String const & getTexture3DLodOffset()const;
		inline castor::String const & getTextureCube()const;
		inline castor::String const & getTextureCubeLod()const;
		inline castor::String const & getTexture1DArray()const;
		inline castor::String const & getTexture1DArrayLod()const;
		inline castor::String const & getTexture1DArrayOffset()const;
		inline castor::String const & getTexture1DArrayOffsetLod()const;
		inline castor::String const & getTexture1DArrayLodOffset()const;
		inline castor::String const & getTexture2DArray()const;
		inline castor::String const & getTexture2DArrayLod()const;
		inline castor::String const & getTexture2DArrayOffset()const;
		inline castor::String const & getTexture2DArrayOffsetLod()const;
		inline castor::String const & getTexture2DArrayLodOffset()const;
		inline castor::String const & getTextureCubeArray()const;
		inline castor::String const & getTextureCubeArrayLod()const;
		inline castor::String const & getTexture1DShadow()const;
		inline castor::String const & getTexture1DShadowLod()const;
		inline castor::String const & getTexture1DShadowOffset()const;
		inline castor::String const & getTexture1DShadowOffsetLod()const;
		inline castor::String const & getTexture1DShadowLodOffset()const;
		inline castor::String const & getTexture2DShadow()const;
		inline castor::String const & getTexture2DShadowLod()const;
		inline castor::String const & getTexture2DShadowOffset()const;
		inline castor::String const & getTexture2DShadowOffsetLod()const;
		inline castor::String const & getTexture2DShadowLodOffset()const;
		inline castor::String const & getTextureCubeShadow()const;
		inline castor::String const & getTextureCubeShadowLod()const;
		inline castor::String const & getTexture1DArrayShadow()const;
		inline castor::String const & getTexture1DArrayShadowLod()const;
		inline castor::String const & getTexture1DArrayShadowOffset()const;
		inline castor::String const & getTexture1DArrayShadowOffsetLod()const;
		inline castor::String const & getTexture1DArrayShadowLodOffset()const;
		inline castor::String const & getTexture2DArrayShadow()const;
		inline castor::String const & getTexture2DArrayShadowLod()const;
		inline castor::String const & getTexture2DArrayShadowOffset()const;
		inline castor::String const & getTexture2DArrayShadowOffsetLod()const;
		inline castor::String const & getTexture2DArrayShadowLodOffset()const;
		inline castor::String const & getTextureCubeArrayShadow()const;
		inline castor::String const & getTextureCubeArrayShadowLod()const;
		inline castor::String const & getTexelFetchBuffer()const;
		inline castor::String const & getTexelFetch1D()const;
		inline castor::String const & getTexelFetch2D()const;
		inline castor::String const & getTexelFetch3D()const;

		GlslWriter_API virtual castor::String getLayout( uint32_t p_index )const = 0;
		GlslWriter_API virtual castor::String getAttribute( uint32_t p_index )const;
		GlslWriter_API virtual bool hasNamedFragData()const = 0;
		GlslWriter_API virtual castor::String getFragData( uint32_t p_index )const = 0;

	protected:
		castor::String m_strUboLayout;
		castor::String m_strUboBinding;
		castor::String m_strSsboLayout;
		castor::String m_strSsboBinding;
		castor::String m_version;
		castor::String m_strAttribute;
		castor::String m_strIn;
		castor::String m_strOut;
		castor::String m_strTexture1D;
		castor::String m_strTexture1DLod;
		castor::String m_strTexture1DOffset;
		castor::String m_strTexture1DOffsetLod;
		castor::String m_strTexture1DLodOffset;
		castor::String m_strTexture2D;
		castor::String m_strTexture2DLod;
		castor::String m_strTexture2DOffset;
		castor::String m_strTexture2DOffsetLod;
		castor::String m_strTexture2DLodOffset;
		castor::String m_strTexture3D;
		castor::String m_strTexture3DLod;
		castor::String m_strTexture3DOffset;
		castor::String m_strTexture3DOffsetLod;
		castor::String m_strTexture3DLodOffset;
		castor::String m_strTextureCube;
		castor::String m_strTextureCubeLod;
		castor::String m_strTexture1DArray;
		castor::String m_strTexture1DArrayLod;
		castor::String m_strTexture1DArrayOffset;
		castor::String m_strTexture1DArrayOffsetLod;
		castor::String m_strTexture1DArrayLodOffset;
		castor::String m_strTexture2DArray;
		castor::String m_strTexture2DArrayLod;
		castor::String m_strTexture2DArrayOffset;
		castor::String m_strTexture2DArrayOffsetLod;
		castor::String m_strTexture2DArrayLodOffset;
		castor::String m_strTextureCubeArray;
		castor::String m_strTextureCubeArrayLod;
		castor::String m_strTexture1DShadow;
		castor::String m_strTexture1DShadowLod;
		castor::String m_strTexture1DShadowOffset;
		castor::String m_strTexture1DShadowOffsetLod;
		castor::String m_strTexture1DShadowLodOffset;
		castor::String m_strTexture2DShadow;
		castor::String m_strTexture2DShadowLod;
		castor::String m_strTexture2DShadowOffset;
		castor::String m_strTexture2DShadowOffsetLod;
		castor::String m_strTexture2DShadowLodOffset;
		castor::String m_strTextureCubeShadow;
		castor::String m_strTextureCubeShadowLod;
		castor::String m_strTexture1DArrayShadow;
		castor::String m_strTexture1DArrayShadowLod;
		castor::String m_strTexture1DArrayShadowOffset;
		castor::String m_strTexture1DArrayShadowOffsetLod;
		castor::String m_strTexture1DArrayShadowLodOffset;
		castor::String m_strTexture2DArrayShadow;
		castor::String m_strTexture2DArrayShadowLod;
		castor::String m_strTexture2DArrayShadowOffset;
		castor::String m_strTexture2DArrayShadowOffsetLod;
		castor::String m_strTexture2DArrayShadowLodOffset;
		castor::String m_strTextureCubeArrayShadow;
		castor::String m_strTextureCubeArrayShadowLod;
		castor::String m_strTexelFetchBuffer;
		castor::String m_strTexelFetch1D;
		castor::String m_strTexelFetch2D;
		castor::String m_strTexelFetch3D;
	};
}

#include "GlslKeywords.inl"

#endif
