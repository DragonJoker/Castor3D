/*
See LICENSE file in root folder
*/
#ifndef ___GLSL_KEYWORDS_H___
#define ___GLSL_KEYWORDS_H___

#include "GlslUbo.hpp"
#include "GlslSsbo.hpp"

namespace glsl
{
	class KeywordsBase
	{
	public:
		GlslWriter_API static std::unique_ptr< glsl::KeywordsBase > get( GlslWriterConfig const & config );
		GlslWriter_API virtual castor::String getUboLayout( Ubo::Layout layout, uint32_t index, uint32_t set )const;
		GlslWriter_API castor::String getSsboLayout( Ssbo::Layout layout, uint32_t index, uint32_t set )const;
		GlslWriter_API castor::String getTextureLayout( uint32_t index, uint32_t set )const;

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

		GlslWriter_API virtual castor::String getLayout( uint32_t index )const = 0;
		GlslWriter_API virtual castor::String getAttribute( uint32_t index )const;
		GlslWriter_API virtual bool hasNamedFragData()const = 0;
		GlslWriter_API virtual castor::String getFragData( uint32_t index )const = 0;

	protected:
		castor::String m_uboLayout;
		castor::String m_uboSet;
		castor::String m_uboBinding;
		castor::String m_ssboLayout;
		castor::String m_ssboSet;
		castor::String m_ssboBinding;
		castor::String m_textureSet;
		castor::String m_textureBinding;
		castor::String m_textureLayout;
		castor::String m_version;
		castor::String m_attribute;
		castor::String m_in;
		castor::String m_out;
		castor::String m_texture1D;
		castor::String m_texture1DLod;
		castor::String m_texture1DOffset;
		castor::String m_texture1DOffsetLod;
		castor::String m_texture1DLodOffset;
		castor::String m_texture2D;
		castor::String m_texture2DLod;
		castor::String m_texture2DOffset;
		castor::String m_texture2DOffsetLod;
		castor::String m_texture2DLodOffset;
		castor::String m_texture3D;
		castor::String m_texture3DLod;
		castor::String m_texture3DOffset;
		castor::String m_texture3DOffsetLod;
		castor::String m_texture3DLodOffset;
		castor::String m_textureCube;
		castor::String m_textureCubeLod;
		castor::String m_texture1DArray;
		castor::String m_texture1DArrayLod;
		castor::String m_texture1DArrayOffset;
		castor::String m_texture1DArrayOffsetLod;
		castor::String m_texture1DArrayLodOffset;
		castor::String m_texture2DArray;
		castor::String m_texture2DArrayLod;
		castor::String m_texture2DArrayOffset;
		castor::String m_texture2DArrayOffsetLod;
		castor::String m_texture2DArrayLodOffset;
		castor::String m_textureCubeArray;
		castor::String m_textureCubeArrayLod;
		castor::String m_texture1DShadow;
		castor::String m_texture1DShadowLod;
		castor::String m_texture1DShadowOffset;
		castor::String m_texture1DShadowOffsetLod;
		castor::String m_texture1DShadowLodOffset;
		castor::String m_texture2DShadow;
		castor::String m_texture2DShadowLod;
		castor::String m_texture2DShadowOffset;
		castor::String m_texture2DShadowOffsetLod;
		castor::String m_texture2DShadowLodOffset;
		castor::String m_textureCubeShadow;
		castor::String m_textureCubeShadowLod;
		castor::String m_texture1DArrayShadow;
		castor::String m_texture1DArrayShadowLod;
		castor::String m_texture1DArrayShadowOffset;
		castor::String m_texture1DArrayShadowOffsetLod;
		castor::String m_texture1DArrayShadowLodOffset;
		castor::String m_texture2DArrayShadow;
		castor::String m_texture2DArrayShadowLod;
		castor::String m_texture2DArrayShadowOffset;
		castor::String m_texture2DArrayShadowOffsetLod;
		castor::String m_texture2DArrayShadowLodOffset;
		castor::String m_textureCubeArrayShadow;
		castor::String m_textureCubeArrayShadowLod;
		castor::String m_texelFetchBuffer;
		castor::String m_texelFetch1D;
		castor::String m_texelFetch2D;
		castor::String m_texelFetch3D;
	};
}

#include "GlslKeywords.inl"

#endif
