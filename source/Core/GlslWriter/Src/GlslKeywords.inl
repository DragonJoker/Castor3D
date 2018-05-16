#include <Exception/Assertion.hpp>

namespace glsl
{
	inline castor::String const & KeywordsBase::getVersion()const
	{
		return m_version;
	}

	inline castor::String const & KeywordsBase::getIn()const
	{
		REQUIRE( !m_in.empty() );
		return m_in;
	}

	inline castor::String const & KeywordsBase::getOut()const
	{
		REQUIRE( !m_out.empty() );
		return m_out;
	}

	inline castor::String const & KeywordsBase::getTexture1D()const
	{
		REQUIRE( !m_texture1D.empty() );
		return m_texture1D;
	}

	inline castor::String const & KeywordsBase::getTexture1DLod()const
	{
		REQUIRE( !m_texture1DLod.empty() );
		return m_texture1DLod;
	}

	inline castor::String const & KeywordsBase::getTexture1DOffset()const
	{
		REQUIRE( !m_texture1DOffset.empty() );
		return m_texture1DOffset;
	}

	inline castor::String const & KeywordsBase::getTexture1DOffsetLod()const
	{
		REQUIRE( !m_texture1DOffsetLod.empty() );
		return m_texture1DOffsetLod;
	}

	inline castor::String const & KeywordsBase::getTexture1DLodOffset()const
	{
		REQUIRE( !m_texture1DLodOffset.empty() );
		return m_texture1DLodOffset;
	}

	inline castor::String const & KeywordsBase::getTexture2D()const
	{
		REQUIRE( !m_texture2D.empty() );
		return m_texture2D;
	}

	inline castor::String const & KeywordsBase::getTexture2DLod()const
	{
		REQUIRE( !m_texture2DLod.empty() );
		return m_texture2DLod;
	}

	inline castor::String const & KeywordsBase::getTexture2DOffset()const
	{
		REQUIRE( !m_texture2DOffset.empty() );
		return m_texture2DOffset;
	}

	inline castor::String const & KeywordsBase::getTexture2DOffsetLod()const
	{
		REQUIRE( !m_texture2DOffsetLod.empty() );
		return m_texture2DOffsetLod;
	}

	inline castor::String const & KeywordsBase::getTexture2DLodOffset()const
	{
		REQUIRE( !m_texture2DLodOffset.empty() );
		return m_texture2DLodOffset;
	}

	inline castor::String const & KeywordsBase::getTexture3D()const
	{
		REQUIRE( !m_texture3D.empty() );
		return m_texture3D;
	}

	inline castor::String const & KeywordsBase::getTexture3DLod()const
	{
		REQUIRE( !m_texture3DLod.empty() );
		return m_texture3DLod;
	}

	inline castor::String const & KeywordsBase::getTexture3DOffset()const
	{
		REQUIRE( !m_texture3DOffset.empty() );
		return m_texture3DOffset;
	}

	inline castor::String const & KeywordsBase::getTexture3DOffsetLod()const
	{
		REQUIRE( !m_texture3DOffsetLod.empty() );
		return m_texture3DOffsetLod;
	}

	inline castor::String const & KeywordsBase::getTexture3DLodOffset()const
	{
		REQUIRE( !m_texture3DLodOffset.empty() );
		return m_texture3DLodOffset;
	}

	inline castor::String const & KeywordsBase::getTextureCube()const
	{
		REQUIRE( !m_textureCube.empty() );
		return m_textureCube;
	}

	inline castor::String const & KeywordsBase::getTextureCubeLod()const
	{
		REQUIRE( !m_textureCubeLod.empty() );
		return m_textureCubeLod;
	}

	inline castor::String const & KeywordsBase::getTexture1DArray()const
	{
		REQUIRE( !m_texture1DArray.empty() );
		return m_texture1DArray;
	}

	inline castor::String const & KeywordsBase::getTexture1DArrayLod()const
	{
		REQUIRE( !m_texture1DArrayLod.empty() );
		return m_texture1DArrayLod;
	}

	inline castor::String const & KeywordsBase::getTexture1DArrayOffset()const
	{
		REQUIRE( !m_texture1DArrayOffset.empty() );
		return m_texture1DArrayOffset;
	}

	inline castor::String const & KeywordsBase::getTexture1DArrayOffsetLod()const
	{
		REQUIRE( !m_texture1DArrayOffsetLod.empty() );
		return m_texture1DArrayOffsetLod;
	}

	inline castor::String const & KeywordsBase::getTexture1DArrayLodOffset()const
	{
		REQUIRE( !m_texture1DArrayLodOffset.empty() );
		return m_texture1DArrayLodOffset;
	}

	inline castor::String const & KeywordsBase::getTexture2DArray()const
	{
		REQUIRE( !m_texture2DArray.empty() );
		return m_texture2DArray;
	}

	inline castor::String const & KeywordsBase::getTexture2DArrayLod()const
	{
		REQUIRE( !m_texture2DArrayLod.empty() );
		return m_texture2DArrayLod;
	}

	inline castor::String const & KeywordsBase::getTexture2DArrayOffset()const
	{
		REQUIRE( !m_texture2DArrayOffset.empty() );
		return m_texture2DArrayOffset;
	}

	inline castor::String const & KeywordsBase::getTexture2DArrayOffsetLod()const
	{
		REQUIRE( !m_texture2DArrayOffsetLod.empty() );
		return m_texture2DArrayOffsetLod;
	}

	inline castor::String const & KeywordsBase::getTexture2DArrayLodOffset()const
	{
		REQUIRE( !m_texture2DArrayLodOffset.empty() );
		return m_texture2DArrayLodOffset;
	}

	inline castor::String const & KeywordsBase::getTextureCubeArray()const
	{
		REQUIRE( !m_textureCubeArray.empty() );
		return m_textureCubeArray;
	}

	inline castor::String const & KeywordsBase::getTextureCubeArrayLod()const
	{
		REQUIRE( !m_textureCubeShadowLod.empty() );
		return m_textureCubeShadowLod;
	}

	inline castor::String const & KeywordsBase::getTexture1DShadow()const
	{
		REQUIRE( !m_texture1DShadow.empty() );
		return m_texture1DShadow;
	}

	inline castor::String const & KeywordsBase::getTexture1DShadowLod()const
	{
		REQUIRE( !m_texture1DShadowLod.empty() );
		return m_texture1DShadowLod;
	}

	inline castor::String const & KeywordsBase::getTexture1DShadowOffset()const
	{
		REQUIRE( !m_texture1DShadowOffset.empty() );
		return m_texture1DShadowOffset;
	}

	inline castor::String const & KeywordsBase::getTexture1DShadowOffsetLod()const
	{
		REQUIRE( !m_texture1DShadowOffsetLod.empty() );
		return m_texture1DShadowOffsetLod;
	}

	inline castor::String const & KeywordsBase::getTexture1DShadowLodOffset()const
	{
		REQUIRE( !m_texture1DShadowLodOffset.empty() );
		return m_texture1DShadowLodOffset;
	}

	inline castor::String const & KeywordsBase::getTexture2DShadow()const
	{
		REQUIRE( !m_texture2DShadow.empty() );
		return m_texture2DShadow;
	}

	inline castor::String const & KeywordsBase::getTexture2DShadowLod()const
	{
		REQUIRE( !m_texture2DShadowLod.empty() );
		return m_texture2DShadowLod;
	}

	inline castor::String const & KeywordsBase::getTexture2DShadowOffset()const
	{
		REQUIRE( !m_texture2DShadowOffset.empty() );
		return m_texture2DShadowOffset;
	}

	inline castor::String const & KeywordsBase::getTexture2DShadowOffsetLod()const
	{
		REQUIRE( !m_texture2DShadowOffsetLod.empty() );
		return m_texture2DShadowOffsetLod;
	}

	inline castor::String const & KeywordsBase::getTexture2DShadowLodOffset()const
	{
		REQUIRE( !m_texture2DShadowLodOffset.empty() );
		return m_texture2DShadowLodOffset;
	}

	inline castor::String const & KeywordsBase::getTextureCubeShadow()const
	{
		REQUIRE( !m_textureCubeShadow.empty() );
		return m_textureCubeShadow;
	}

	inline castor::String const & KeywordsBase::getTextureCubeShadowLod()const
	{
		REQUIRE( !m_textureCubeShadowLod.empty() );
		return m_textureCubeShadowLod;
	}

	inline castor::String const & KeywordsBase::getTexture1DArrayShadow()const
	{
		REQUIRE( !m_texture1DArrayShadow.empty() );
		return m_texture1DArrayShadow;
	}

	inline castor::String const & KeywordsBase::getTexture1DArrayShadowLod()const
	{
		REQUIRE( !m_texture1DArrayShadowLod.empty() );
		return m_texture1DArrayShadowLod;
	}

	inline castor::String const & KeywordsBase::getTexture1DArrayShadowOffset()const
	{
		REQUIRE( !m_texture1DArrayShadowOffset.empty() );
		return m_texture1DArrayShadowOffset;
	}

	inline castor::String const & KeywordsBase::getTexture1DArrayShadowOffsetLod()const
	{
		REQUIRE( !m_texture1DArrayShadowOffsetLod.empty() );
		return m_texture1DArrayShadowOffsetLod;
	}

	inline castor::String const & KeywordsBase::getTexture1DArrayShadowLodOffset()const
	{
		REQUIRE( !m_texture1DArrayShadowLodOffset.empty() );
		return m_texture1DArrayShadowLodOffset;
	}

	inline castor::String const & KeywordsBase::getTexture2DArrayShadow()const
	{
		REQUIRE( !m_texture2DArrayShadow.empty() );
		return m_texture2DArrayShadow;
	}

	inline castor::String const & KeywordsBase::getTexture2DArrayShadowLod()const
	{
		REQUIRE( !m_texture2DArrayShadowLod.empty() );
		return m_texture2DArrayShadowLod;
	}

	inline castor::String const & KeywordsBase::getTexture2DArrayShadowOffset()const
	{
		REQUIRE( !m_texture2DArrayShadowOffset.empty() );
		return m_texture2DArrayShadowOffset;
	}

	inline castor::String const & KeywordsBase::getTexture2DArrayShadowOffsetLod()const
	{
		REQUIRE( !m_texture2DArrayShadowOffsetLod.empty() );
		return m_texture2DArrayShadowOffsetLod;
	}

	inline castor::String const & KeywordsBase::getTexture2DArrayShadowLodOffset()const
	{
		REQUIRE( !m_texture2DArrayShadowLodOffset.empty() );
		return m_texture2DArrayShadowLodOffset;
	}

	inline castor::String const & KeywordsBase::getTextureCubeArrayShadow()const
	{
		REQUIRE( !m_textureCubeArrayShadow.empty() );
		return m_textureCubeArrayShadow;
	}

	inline castor::String const & KeywordsBase::getTextureCubeArrayShadowLod()const
	{
		REQUIRE( !m_textureCubeArrayShadowLod.empty() );
		return m_textureCubeArrayShadowLod;
	}

	inline castor::String const & KeywordsBase::getTexelFetchBuffer()const
	{
		REQUIRE( !m_texelFetchBuffer.empty() );
		return m_texelFetchBuffer;
	}

	inline castor::String const & KeywordsBase::getTexelFetch1D()const
	{
		REQUIRE( !m_texelFetch1D.empty() );
		return m_texelFetch1D;
	}

	inline castor::String const & KeywordsBase::getTexelFetch2D()const
	{
		REQUIRE( !m_texelFetch2D.empty() );
		return m_texelFetch2D;
	}

	inline castor::String const & KeywordsBase::getTexelFetch3D()const
	{
		REQUIRE( !m_texelFetch3D.empty() );
		return m_texelFetch3D;
	}

	template< int Version, bool IsVulkan >
	class Keywords< Version, IsVulkan, typename std::enable_if< ( Version <= 120 ) && !IsVulkan >::type >
		: public KeywordsBase
	{
	public:
		Keywords()
		{
			m_attribute = cuT( "attribute" );
			m_in = cuT( "varying" );
			m_out = cuT( "varying" );
			m_texture1D = cuT( "texture1D" );
			m_texture1DLod = cuT( "texture1DLod" );
			m_texture2D = cuT( "texture2D" );
			m_texture2DLod = cuT( "texture2DLod" );
			m_texture3D = cuT( "texture3D" );
			m_texture3DLod = cuT( "texture3DLod" );
			m_textureCube = cuT( "textureCube" );
			m_textureCubeLod = cuT( "textureCubeLod" );
			m_texture1DArray = cuT( "texture1D" );
			m_texture1DArrayLod = cuT( "texture1D" );
			m_texture2DArray = cuT( "texture2D" );
			m_texture2DArrayLod = cuT( "texture2D" );
			m_textureCubeArray = cuT( "textureCube" );
			m_textureCubeArrayLod = cuT( "textureCubeLod" );
			m_texelFetch1D = cuT( "texture1D" );
			m_texelFetch2D = cuT( "texture2D" );
			m_texelFetch3D = cuT( "texture3D" );
			m_texelFetch3D = cuT( "texture3D" );
		}

		castor::String getLayout( uint32_t CU_PARAM_UNUSED( index ) )const override
		{
			return cuT( "" );
		}

		castor::String getFragData( uint32_t index )const override
		{
			castor::StringStream stream{ makeStringStream() };
			stream << cuT( "gl_FragData[" ) << index << cuT( "]" );
			return stream.str();
		}

		bool hasNamedFragData()const override
		{
			return false;
		}
	};

	template< int Version, bool IsVulkan >
	class Keywords< Version, IsVulkan, typename std::enable_if < ( Version > 120 ) && ( Version < 140 ) && !IsVulkan >::type >
		: public KeywordsBase
	{
	public:
		Keywords()
		{
			m_version = cuT( "#version " ) + castor::string::toString( Version, 10, Expr::getLocale() );
			m_attribute = cuT( "in" );
			m_in = cuT( "in" );
			m_out = cuT( "out" );
			m_texture1D = cuT( "texture" );
			m_texture1DLod = cuT( "textureLod" );
			m_texture1DOffset = cuT( "textureOffset" );
			m_texture1DOffsetLod = cuT( "textureLodOffset" );
			m_texture1DLodOffset = cuT( "textureLodOffset" );
			m_texture2D = cuT( "texture" );
			m_texture2DLod = cuT( "textureLod" );
			m_texture2DOffset = cuT( "textureOffset" );
			m_texture2DOffsetLod = cuT( "textureLodOffset" );
			m_texture2DLodOffset = cuT( "textureLodOffset" );
			m_texture3D = cuT( "texture" );
			m_texture3DLod = cuT( "textureLod" );
			m_texture3DOffset = cuT( "textureOffset" );
			m_texture3DOffsetLod = cuT( "textureLodOffset" );
			m_texture3DLodOffset = cuT( "textureLodOffset" );
			m_textureCube = cuT( "texture" );
			m_textureCubeLod = cuT( "textureLod" );
			m_texture1DArray = cuT( "texture" );
			m_texture1DArrayLod = cuT( "textureLod" );
			m_texture1DArrayOffset = cuT( "textureOffset" );
			m_texture1DArrayOffsetLod = cuT( "textureLodOffset" );
			m_texture1DArrayLodOffset = cuT( "textureLodOffset" );
			m_texture2DArray = cuT( "texture" );
			m_texture2DArrayLod = cuT( "textureLod" );
			m_texture2DArrayOffset = cuT( "textureOffset" );
			m_texture2DArrayOffsetLod = cuT( "textureLodOffset" );
			m_texture2DArrayLodOffset = cuT( "textureLodOffset" );
			m_textureCubeArray = cuT( "texture" );
			m_textureCubeArrayLod = cuT( "textureLod" );
			m_texture1DShadow = cuT( "texture" );
			m_texture1DShadowLod = cuT( "textureLod" );
			m_texture1DShadowOffset = cuT( "textureOffset" );
			m_texture1DShadowOffsetLod = cuT( "textureLodOffset" );
			m_texture1DShadowLodOffset = cuT( "textureLodOffset" );
			m_texture2DShadow = cuT( "texture" );
			m_texture2DShadowLod = cuT( "textureLod" );
			m_texture2DShadowOffset = cuT( "textureOffset" );
			m_texture2DShadowOffsetLod = cuT( "textureLodOffset" );
			m_texture2DShadowLodOffset = cuT( "textureLodOffset" );
			m_textureCubeShadow = cuT( "texture" );
			m_textureCubeShadowLod = cuT( "textureLod" );
			m_texture1DArrayShadow = cuT( "texture" );
			m_texture1DArrayShadowLod = cuT( "textureLod" );
			m_texture1DArrayShadowOffset = cuT( "textureOffset" );
			m_texture1DArrayShadowOffsetLod = cuT( "textureLodOffset" );
			m_texture1DArrayShadowLodOffset = cuT( "textureLodOffset" );
			m_texture2DArrayShadow = cuT( "texture" );
			m_texture2DArrayShadowLod = cuT( "textureLod" );
			m_texture2DArrayShadowOffset = cuT( "textureOffset" );
			m_texture2DArrayShadowOffsetLod = cuT( "textureLodOffset" );
			m_texture2DArrayShadowLodOffset = cuT( "textureLodOffset" );
			m_textureCubeArrayShadow = cuT( "texture" );
			m_textureCubeArrayShadowLod = cuT( "textureLod" );
			m_textureCubeShadow = cuT( "texture" );
			m_textureCubeShadowLod = cuT( "textureLod" );
			m_texelFetchBuffer = cuT( "texelFetch" );
			m_texelFetch1D = cuT( "texelFetch" );
			m_texelFetch2D = cuT( "texelFetch" );
			m_texelFetch3D = cuT( "texelFetch" );
		}

		castor::String getLayout( uint32_t CU_PARAM_UNUSED( index ) )const override
		{
			return cuT( "" );
		}

		castor::String getFragData( uint32_t index )const override
		{
			castor::StringStream stream{ makeStringStream() };
			stream << cuT( "gl_FragData[" ) << index << cuT( "]" );
			return stream.str();
		}

		bool hasNamedFragData()const override
		{
			return false;
		}
	};

	template< int Version, bool IsVulkan >
	class Keywords< Version, IsVulkan, typename std::enable_if < ( Version >= 140 ) && ( Version < 150 ) && !IsVulkan >::type >
		: public KeywordsBase
	{
	public:
		Keywords()
		{
			m_uboLayout = cuT( "layout" );
			m_version = cuT( "#version " ) + castor::string::toString( Version, 10, Expr::getLocale() );
			m_attribute = cuT( "in" );
			m_in = cuT( "in" );
			m_out = cuT( "out" );
			m_texture1D = cuT( "texture" );
			m_texture1DLod = cuT( "texture" );
			m_texture1DOffset = cuT( "textureOffset" );
			m_texture1DOffsetLod = cuT( "textureOffset" );
			m_texture1DLodOffset = cuT( "textureLodOffset" );
			m_texture2D = cuT( "texture" );
			m_texture2DLod = cuT( "texture" );
			m_texture2DOffset = cuT( "textureOffset" );
			m_texture2DOffsetLod = cuT( "textureOffset" );
			m_texture2DLodOffset = cuT( "textureLodOffset" );
			m_texture3D = cuT( "texture" );
			m_texture3DLod = cuT( "texture" );
			m_texture3DOffset = cuT( "textureOffset" );
			m_texture3DOffsetLod = cuT( "textureOffset" );
			m_texture3DLodOffset = cuT( "textureLodOffset" );
			m_textureCube = cuT( "texture" );
			m_textureCubeLod = cuT( "texture" );
			m_texture1DArray = cuT( "texture" );
			m_texture1DArrayLod = cuT( "texture" );
			m_texture1DArrayOffset = cuT( "textureOffset" );
			m_texture1DArrayOffsetLod = cuT( "textureOffset" );
			m_texture1DArrayLodOffset = cuT( "textureLodOffset" );
			m_texture2DArray = cuT( "texture" );
			m_texture2DArrayLod = cuT( "texture" );
			m_texture2DArrayOffset = cuT( "textureOffset" );
			m_texture2DArrayOffsetLod = cuT( "textureOffset" );
			m_texture2DArrayLodOffset = cuT( "textureLodOffset" );
			m_textureCubeArray = cuT( "texture" );
			m_textureCubeArrayLod = cuT( "texture" );
			m_texture1DShadow = cuT( "texture" );
			m_texture1DShadowLod = cuT( "texture" );
			m_texture1DShadowOffset = cuT( "textureOffset" );
			m_texture1DShadowOffsetLod = cuT( "textureOffset" );
			m_texture1DShadowLodOffset = cuT( "textureLodOffset" );
			m_texture2DShadow = cuT( "texture" );
			m_texture2DShadowLod = cuT( "texture" );
			m_texture2DShadowOffset = cuT( "textureOffset" );
			m_texture2DShadowOffsetLod = cuT( "textureOffset" );
			m_texture2DShadowLodOffset = cuT( "textureLodOffset" );
			m_textureCubeShadow = cuT( "texture" );
			m_textureCubeShadowLod = cuT( "texture" );
			m_texture1DArrayShadow = cuT( "texture" );
			m_texture1DArrayShadowLod = cuT( "texture" );
			m_texture1DArrayShadowOffset = cuT( "textureOffset" );
			m_texture1DArrayShadowOffsetLod = cuT( "textureOffset" );
			m_texture1DArrayShadowLodOffset = cuT( "textureLodOffset" );
			m_texture2DArrayShadow = cuT( "texture" );
			m_texture2DArrayShadowLod = cuT( "texture" );
			m_texture2DArrayShadowOffset = cuT( "textureOffset" );
			m_texture2DArrayShadowOffsetLod = cuT( "textureOffset" );
			m_texture2DArrayShadowLodOffset = cuT( "textureLodOffset" );
			m_textureCubeArrayShadow = cuT( "texture" );
			m_textureCubeArrayShadowLod = cuT( "texture" );
			m_texelFetchBuffer = cuT( "texelFetch" );
			m_texelFetch1D = cuT( "texelFetch" );
			m_texelFetch2D = cuT( "texelFetch" );
			m_texelFetch3D = cuT( "texelFetch" );
		}

		castor::String getLayout( uint32_t CU_PARAM_UNUSED( index ) )const override
		{
			return cuT( "" );
		}

		castor::String getFragData( uint32_t CU_PARAM_UNUSED( index ) )const override
		{
			return cuT( "" );
		}

		bool hasNamedFragData()const override
		{
			return true;
		}
	};

	template< int Version, bool IsVulkan >
	class Keywords< Version, IsVulkan, typename std::enable_if< ( Version >= 150 ) && ( Version < 330 ) && !IsVulkan >::type >
		: public KeywordsBase
	{
	public:
		Keywords()
		{
			m_uboLayout = cuT( "layout" );
			m_uboSet = cuT( "set" );
			m_uboBinding = cuT( "binding" );
			m_ssboLayout = cuT( "layout" );
			m_ssboBinding = cuT( "binding" );
			m_textureLayout = cuT( "layout" );
			m_textureBinding = cuT( "binding" );
			m_uniformLayout = cuT( "layout" );
			m_inputLayout = cuT( "layout" );
			m_outputLayout = cuT( "layout" );
			m_version = cuT( "#version " ) + castor::string::toString( Version, 10, Expr::getLocale() );
			m_attribute = cuT( "in" );
			m_in = cuT( "in" );
			m_out = cuT( "out" );
			m_texture1D = cuT( "texture" );
			m_texture1DLod = cuT( "textureLod" );
			m_texture1DOffset = cuT( "textureOffset" );
			m_texture1DOffsetLod = cuT( "textureOffset" );
			m_texture1DLodOffset = cuT( "textureLodOffset" );
			m_texture2D = cuT( "texture" );
			m_texture2DLod = cuT( "textureLod" );
			m_texture2DOffset = cuT( "textureOffset" );
			m_texture2DOffsetLod = cuT( "textureOffset" );
			m_texture2DLodOffset = cuT( "textureLodOffset" );
			m_texture3D = cuT( "texture" );
			m_texture3DLod = cuT( "textureLod" );
			m_texture3DOffset = cuT( "textureOffset" );
			m_texture3DOffsetLod = cuT( "textureOffset" );
			m_texture3DLodOffset = cuT( "textureLodOffset" );
			m_textureCube = cuT( "texture" );
			m_textureCubeLod = cuT( "textureLod" );
			m_texture1DArray = cuT( "texture" );
			m_texture1DArrayLod = cuT( "textureLod" );
			m_texture1DArrayOffset = cuT( "textureOffset" );
			m_texture1DArrayOffsetLod = cuT( "textureOffset" );
			m_texture1DArrayLodOffset = cuT( "textureLodOffset" );
			m_texture2DArray = cuT( "texture" );
			m_texture2DArrayLod = cuT( "textureLod" );
			m_texture2DArrayOffset = cuT( "textureOffset" );
			m_texture2DArrayOffsetLod = cuT( "textureOffset" );
			m_texture2DArrayLodOffset = cuT( "textureLodOffset" );
			m_textureCubeArray = cuT( "texture" );
			m_textureCubeArrayLod = cuT( "textureLod" );
			m_texture1DShadow = cuT( "texture" );
			m_texture1DShadowLod = cuT( "textureLod" );
			m_texture1DShadowOffset = cuT( "textureOffset" );
			m_texture1DShadowOffsetLod = cuT( "textureOffset" );
			m_texture1DShadowLodOffset = cuT( "textureLodOffset" );
			m_texture2DShadow = cuT( "texture" );
			m_texture2DShadowLod = cuT( "textureLod" );
			m_texture2DShadowOffset = cuT( "textureOffset" );
			m_texture2DShadowOffsetLod = cuT( "textureOffset" );
			m_texture2DShadowLodOffset = cuT( "textureLodOffset" );
			m_textureCubeShadow = cuT( "texture" );
			m_textureCubeShadowLod = cuT( "texture" );
			m_texture1DArrayShadow = cuT( "texture" );
			m_texture1DArrayShadowLod = cuT( "textureLod" );
			m_texture1DArrayShadowOffset = cuT( "textureOffset" );
			m_texture1DArrayShadowOffsetLod = cuT( "textureOffset" );
			m_texture1DArrayShadowLodOffset = cuT( "textureLodOffset" );
			m_texture2DArrayShadow = cuT( "texture" );
			m_texture2DArrayShadowLod = cuT( "textureLod" );
			m_texture2DArrayShadowOffset = cuT( "textureOffset" );
			m_texture2DArrayShadowOffsetLod = cuT( "textureOffset" );
			m_texture2DArrayShadowLodOffset = cuT( "textureLodOffset" );
			m_textureCubeArrayShadow = cuT( "texture" );
			m_textureCubeArrayShadowLod = cuT( "textureLod" );
			m_texelFetchBuffer = cuT( "texelFetch" );
			m_texelFetch1D = cuT( "texelFetch" );
			m_texelFetch2D = cuT( "texelFetch" );
			m_texelFetch3D = cuT( "texelFetch" );
		}

		castor::String getLayout( uint32_t index )const override
		{
			return cuT( "layout( location=" ) + castor::string::toString( index, 10, Expr::getLocale() ) + cuT( " ) " );
		}

		castor::String getFragData( uint32_t CU_PARAM_UNUSED( index ) )const override
		{
			return cuT( "" );
		}

		bool hasNamedFragData()const override
		{
			return true;
		}
	};

	template< int Version, bool IsVulkan >
	class Keywords< Version, IsVulkan, typename std::enable_if< ( Version >= 330 ) && ( Version < 420 ) && !IsVulkan >::type >
		: public KeywordsBase
	{
	public:
		Keywords()
		{
			m_uboLayout = cuT( "layout" );
			m_uboSet = cuT( "set" );
			m_uboBinding = cuT( "binding" );
			m_ssboLayout = cuT( "layout" );
			m_ssboBinding = cuT( "binding" );
			m_textureLayout = cuT( "layout" );
			m_textureBinding = cuT( "binding" );
			m_uniformLayout = cuT( "layout" );
			m_inputLayout = cuT( "layout" );
			m_outputLayout = cuT( "layout" );
			m_version = cuT( "#version " ) + castor::string::toString( Version, 10, Expr::getLocale() );
			m_attribute = cuT( "in" );
			m_in = cuT( "in" );
			m_out = cuT( "out" );
			m_texture1D = cuT( "texture" );
			m_texture1DLod = cuT( "textureLod" );
			m_texture1DOffset = cuT( "textureOffset" );
			m_texture1DOffsetLod = cuT( "textureOffset" );
			m_texture1DLodOffset = cuT( "textureLodOffset" );
			m_texture2D = cuT( "texture" );
			m_texture2DLod = cuT( "textureLod" );
			m_texture2DOffset = cuT( "textureOffset" );
			m_texture2DOffsetLod = cuT( "textureOffset" );
			m_texture2DLodOffset = cuT( "textureLodOffset" );
			m_texture3D = cuT( "texture" );
			m_texture3DLod = cuT( "textureLod" );
			m_texture3DOffset = cuT( "textureOffset" );
			m_texture3DOffsetLod = cuT( "textureOffset" );
			m_texture3DLodOffset = cuT( "textureLodOffset" );
			m_textureCube = cuT( "texture" );
			m_textureCubeLod = cuT( "textureLod" );
			m_texture1DArray = cuT( "texture" );
			m_texture1DArrayLod = cuT( "textureLod" );
			m_texture1DArrayOffset = cuT( "textureOffset" );
			m_texture1DArrayOffsetLod = cuT( "textureOffset" );
			m_texture1DArrayLodOffset = cuT( "textureLodOffset" );
			m_texture2DArray = cuT( "texture" );
			m_texture2DArrayLod = cuT( "textureLod" );
			m_texture2DArrayOffset = cuT( "textureOffset" );
			m_texture2DArrayOffsetLod = cuT( "textureOffset" );
			m_texture2DArrayLodOffset = cuT( "textureLodOffset" );
			m_textureCubeArray = cuT( "texture" );
			m_textureCubeArrayLod = cuT( "textureLod" );
			m_texture1DShadow = cuT( "texture" );
			m_texture1DShadowLod = cuT( "textureLod" );
			m_texture1DShadowOffset = cuT( "textureOffset" );
			m_texture1DShadowOffsetLod = cuT( "textureOffset" );
			m_texture1DShadowLodOffset = cuT( "textureLodOffset" );
			m_texture2DShadow = cuT( "texture" );
			m_texture2DShadowLod = cuT( "textureLod" );
			m_texture2DShadowOffset = cuT( "textureOffset" );
			m_texture2DShadowOffsetLod = cuT( "textureOffset" );
			m_texture2DShadowLodOffset = cuT( "textureLodOffset" );
			m_textureCubeShadow = cuT( "texture" );
			m_textureCubeShadowLod = cuT( "texture" );
			m_texture1DArrayShadow = cuT( "texture" );
			m_texture1DArrayShadowLod = cuT( "textureLod" );
			m_texture1DArrayShadowOffset = cuT( "textureOffset" );
			m_texture1DArrayShadowOffsetLod = cuT( "textureOffset" );
			m_texture1DArrayShadowLodOffset = cuT( "textureLodOffset" );
			m_texture2DArrayShadow = cuT( "texture" );
			m_texture2DArrayShadowLod = cuT( "textureLod" );
			m_texture2DArrayShadowOffset = cuT( "textureOffset" );
			m_texture2DArrayShadowOffsetLod = cuT( "textureOffset" );
			m_texture2DArrayShadowLodOffset = cuT( "textureLodOffset" );
			m_textureCubeArrayShadow = cuT( "texture" );
			m_textureCubeArrayShadowLod = cuT( "textureLod" );
			m_texelFetchBuffer = cuT( "texelFetch" );
			m_texelFetch1D = cuT( "texelFetch" );
			m_texelFetch2D = cuT( "texelFetch" );
			m_texelFetch3D = cuT( "texelFetch" );
		}

		castor::String getLayout( uint32_t index )const override
		{
			return cuT( "layout( location=" ) + castor::string::toString( index, 10, Expr::getLocale() ) + cuT( " ) " );
		}

		castor::String getFragData( uint32_t CU_PARAM_UNUSED( index ) )const override
		{
			return cuT( "" );
		}

		bool hasNamedFragData()const override
		{
			return true;
		}
	};

	template< int Version, bool IsVulkan >
	class Keywords< Version, IsVulkan, typename std::enable_if< ( Version >= 420 ) && !IsVulkan >::type >
		: public KeywordsBase
	{
	public:
		Keywords()
		{
			m_uboLayout = cuT( "layout" );
			m_uboSet = cuT( "set" );
			m_uboBinding = cuT( "binding" );
			m_ssboLayout = cuT( "layout" );
			m_ssboBinding = cuT( "binding" );
			m_textureLayout = cuT( "layout" );
			m_textureBinding = cuT( "binding" );
			m_uniformLayout = cuT( "layout" );
			m_inputLayout = cuT( "layout" );
			m_outputLayout = cuT( "layout" );
			m_version = cuT( "#version " ) + castor::string::toString( Version, 10, Expr::getLocale() );
			m_attribute = cuT( "in" );
			m_in = cuT( "in" );
			m_out = cuT( "out" );
			m_texture1D = cuT( "texture" );
			m_texture1DLod = cuT( "textureLod" );
			m_texture1DOffset = cuT( "textureOffset" );
			m_texture1DOffsetLod = cuT( "textureOffset" );
			m_texture1DLodOffset = cuT( "textureLodOffset" );
			m_texture2D = cuT( "texture" );
			m_texture2DLod = cuT( "textureLod" );
			m_texture2DOffset = cuT( "textureOffset" );
			m_texture2DOffsetLod = cuT( "textureOffset" );
			m_texture2DLodOffset = cuT( "textureLodOffset" );
			m_texture3D = cuT( "texture" );
			m_texture3DLod = cuT( "textureLod" );
			m_texture3DOffset = cuT( "textureOffset" );
			m_texture3DOffsetLod = cuT( "textureOffset" );
			m_texture3DLodOffset = cuT( "textureLodOffset" );
			m_textureCube = cuT( "texture" );
			m_textureCubeLod = cuT( "textureLod" );
			m_texture1DArray = cuT( "texture" );
			m_texture1DArrayLod = cuT( "texture" );
			m_texture1DArrayOffset = cuT( "textureOffset" );
			m_texture1DArrayOffsetLod = cuT( "textureOffset" );
			m_texture1DArrayLodOffset = cuT( "textureLodOffset" );
			m_texture2DArray = cuT( "texture" );
			m_texture2DArrayLod = cuT( "textureLod" );
			m_texture2DArrayOffset = cuT( "textureOffset" );
			m_texture2DArrayOffsetLod = cuT( "textureOffset" );
			m_texture2DArrayLodOffset = cuT( "textureLodOffset" );
			m_textureCubeArray = cuT( "texture" );
			m_textureCubeArrayLod = cuT( "texture" );
			m_texture1DShadow = cuT( "texture" );
			m_texture1DShadowLod = cuT( "textureLod" );
			m_texture1DShadowOffset = cuT( "textureOffset" );
			m_texture1DShadowOffsetLod = cuT( "textureOffset" );
			m_texture1DShadowLodOffset = cuT( "textureLodOffset" );
			m_texture2DShadow = cuT( "texture" );
			m_texture2DShadowLod = cuT( "textureLod" );
			m_texture2DShadowOffset = cuT( "textureOffset" );
			m_texture2DShadowOffsetLod = cuT( "textureOffset" );
			m_texture2DShadowLodOffset = cuT( "textureLodOffset" );
			m_textureCubeShadow = cuT( "texture" );
			m_textureCubeShadowLod = cuT( "textureLod" );
			m_texture1DArrayShadow = cuT( "texture" );
			m_texture1DArrayShadowLod = cuT( "textureLod" );
			m_texture1DArrayShadowOffset = cuT( "textureOffset" );
			m_texture1DArrayShadowOffsetLod = cuT( "textureOffset" );
			m_texture1DArrayShadowLodOffset = cuT( "textureLodOffset" );
			m_texture2DArrayShadow = cuT( "texture" );
			m_texture2DArrayShadowLod = cuT( "textureLod" );
			m_texture2DArrayShadowOffset = cuT( "textureOffset" );
			m_texture2DArrayShadowOffsetLod = cuT( "textureOffset" );
			m_texture2DArrayShadowLodOffset = cuT( "textureLodOffset" );
			m_textureCubeArrayShadow = cuT( "texture" );
			m_textureCubeArrayShadowLod = cuT( "textureLod" );
			m_texelFetchBuffer = cuT( "texelFetch" );
			m_texelFetch1D = cuT( "texelFetch" );
			m_texelFetch2D = cuT( "texelFetch" );
			m_texelFetch3D = cuT( "texelFetch" );
		}

		castor::String getLayout( uint32_t index )const override
		{
			return cuT( "layout( location=" ) + castor::string::toString( index, 10, Expr::getLocale() ) + cuT( " ) " );
		}

		castor::String getFragData( uint32_t CU_PARAM_UNUSED( index ) )const override
		{
			return cuT( "" );
		}

		bool hasNamedFragData()const override
		{
			return true;
		}
	};

	template< int Version, bool IsVulkan >
	class Keywords< Version, IsVulkan, typename std::enable_if< ( Version >= 140 ) && IsVulkan >::type >
		: public KeywordsBase
	{
	public:
		Keywords()
		{
			m_uboLayout = cuT( "layout" );
			m_uboSet = cuT( "set" );
			m_uboBinding = cuT( "binding" );
			m_ssboLayout = cuT( "layout" );
			m_ssboSet = cuT( "set" );
			m_ssboBinding = cuT( "binding" );
			m_textureLayout = cuT( "layout" );
			m_textureSet = cuT( "set" );
			m_textureBinding = cuT( "binding" );
			m_uniformLayout = cuT( "layout" );
			m_inputLayout = cuT( "layout" );
			m_outputLayout = cuT( "layout" );
			m_version = cuT( "#version " ) + castor::string::toString( Version, 10, Expr::getLocale() ) + cuT( "\n#extension GL_KHR_vulkan_glsl : enable\n" );
			m_attribute = cuT( "in" );
			m_in = cuT( "in" );
			m_out = cuT( "out" );
			m_texture1D = cuT( "texture" );
			m_texture1DLod = cuT( "textureLod" );
			m_texture1DOffset = cuT( "textureOffset" );
			m_texture1DOffsetLod = cuT( "textureOffset" );
			m_texture1DLodOffset = cuT( "textureLodOffset" );
			m_texture2D = cuT( "texture" );
			m_texture2DLod = cuT( "textureLod" );
			m_texture2DOffset = cuT( "textureOffset" );
			m_texture2DOffsetLod = cuT( "textureOffset" );
			m_texture2DLodOffset = cuT( "textureLodOffset" );
			m_texture3D = cuT( "texture" );
			m_texture3DLod = cuT( "textureLod" );
			m_texture3DOffset = cuT( "textureOffset" );
			m_texture3DOffsetLod = cuT( "textureOffset" );
			m_texture3DLodOffset = cuT( "textureLodOffset" );
			m_textureCube = cuT( "texture" );
			m_textureCubeLod = cuT( "textureLod" );
			m_texture1DArray = cuT( "texture" );
			m_texture1DArrayLod = cuT( "texture" );
			m_texture1DArrayOffset = cuT( "textureOffset" );
			m_texture1DArrayOffsetLod = cuT( "textureOffset" );
			m_texture1DArrayLodOffset = cuT( "textureLodOffset" );
			m_texture2DArray = cuT( "texture" );
			m_texture2DArrayLod = cuT( "textureLod" );
			m_texture2DArrayOffset = cuT( "textureOffset" );
			m_texture2DArrayOffsetLod = cuT( "textureOffset" );
			m_texture2DArrayLodOffset = cuT( "textureLodOffset" );
			m_textureCubeArray = cuT( "texture" );
			m_textureCubeArrayLod = cuT( "texture" );
			m_texture1DShadow = cuT( "texture" );
			m_texture1DShadowLod = cuT( "textureLod" );
			m_texture1DShadowOffset = cuT( "textureOffset" );
			m_texture1DShadowOffsetLod = cuT( "textureOffset" );
			m_texture1DShadowLodOffset = cuT( "textureLodOffset" );
			m_texture2DShadow = cuT( "texture" );
			m_texture2DShadowLod = cuT( "textureLod" );
			m_texture2DShadowOffset = cuT( "textureOffset" );
			m_texture2DShadowOffsetLod = cuT( "textureOffset" );
			m_texture2DShadowLodOffset = cuT( "textureLodOffset" );
			m_textureCubeShadow = cuT( "texture" );
			m_textureCubeShadowLod = cuT( "textureLod" );
			m_texture1DArrayShadow = cuT( "texture" );
			m_texture1DArrayShadowLod = cuT( "textureLod" );
			m_texture1DArrayShadowOffset = cuT( "textureOffset" );
			m_texture1DArrayShadowOffsetLod = cuT( "textureOffset" );
			m_texture1DArrayShadowLodOffset = cuT( "textureLodOffset" );
			m_texture2DArrayShadow = cuT( "texture" );
			m_texture2DArrayShadowLod = cuT( "textureLod" );
			m_texture2DArrayShadowOffset = cuT( "textureOffset" );
			m_texture2DArrayShadowOffsetLod = cuT( "textureOffset" );
			m_texture2DArrayShadowLodOffset = cuT( "textureLodOffset" );
			m_textureCubeArrayShadow = cuT( "texture" );
			m_textureCubeArrayShadowLod = cuT( "textureLod" );
			m_texelFetchBuffer = cuT( "texelFetch" );
			m_texelFetch1D = cuT( "texelFetch" );
			m_texelFetch2D = cuT( "texelFetch" );
			m_texelFetch3D = cuT( "texelFetch" );
		}

		castor::String getUboLayout( Ubo::Layout layout, uint32_t index, uint32_t set )const override
		{
			static std::map< Ubo::Layout, castor::String > LayoutName
			{
				{ Ubo::Layout::eStd140, cuT( "std140" ) },
				{ Ubo::Layout::eShared, cuT( "shared" ) },
				{ Ubo::Layout::ePacked, cuT( "packed" ) },
			};

			castor::StringStream result{ makeStringStream() };

			if ( !m_uboLayout.empty() )
			{
				result << m_uboLayout << cuT( "( " ) << LayoutName[layout];

				if ( !m_uboBinding.empty() )
				{
					result << cuT( ", " ) << m_uboBinding << cuT( " = " ) << index;
				}

				if ( !m_uboSet.empty() )
				{
					result << cuT( ", " ) << m_uboSet << cuT( " = " ) << set;
				}

				result << cuT( " ) " );
			}

			return result.str();
		}

		castor::String getLayout( uint32_t index )const override
		{
			return cuT( "layout( location=" ) + castor::string::toString( index, 10, Expr::getLocale() ) + cuT( " ) " );
		}

		castor::String getFragData( uint32_t CU_PARAM_UNUSED( index ) )const override
		{
			return cuT( "" );
		}

		bool hasNamedFragData()const override
		{
			return true;
		}
	};
}
