#include <Exception/Assertion.hpp>

namespace glsl
{
	inline castor::String const & KeywordsBase::getVersion()const
	{
		return m_version;
	}
	inline castor::String const & KeywordsBase::getIn()const
	{
		REQUIRE( !m_strIn.empty() );
		return m_strIn;
	}
	inline castor::String const & KeywordsBase::getOut()const
	{
		REQUIRE( !m_strOut.empty() );
		return m_strOut;
	}
	inline castor::String const & KeywordsBase::getTexture1D()const
	{
		REQUIRE( !m_strTexture1D.empty() );
		return m_strTexture1D;
	}
	inline castor::String const & KeywordsBase::getTexture1DLod()const
	{
		REQUIRE( !m_strTexture1DLod.empty() );
		return m_strTexture1DLod;
	}
	inline castor::String const & KeywordsBase::getTexture1DOffset()const
	{
		REQUIRE( !m_strTexture1DOffset.empty() );
		return m_strTexture1DOffset;
	}
	inline castor::String const & KeywordsBase::getTexture1DOffsetLod()const
	{
		REQUIRE( !m_strTexture1DOffsetLod.empty() );
		return m_strTexture1DOffsetLod;
	}
	inline castor::String const & KeywordsBase::getTexture1DLodOffset()const
	{
		REQUIRE( !m_strTexture1DLodOffset.empty() );
		return m_strTexture1DLodOffset;
	}
	inline castor::String const & KeywordsBase::getTexture2D()const
	{
		REQUIRE( !m_strTexture2D.empty() );
		return m_strTexture2D;
	}
	inline castor::String const & KeywordsBase::getTexture2DLod()const
	{
		REQUIRE( !m_strTexture2DLod.empty() );
		return m_strTexture2DLod;
	}
	inline castor::String const & KeywordsBase::getTexture2DOffset()const
	{
		REQUIRE( !m_strTexture2DOffset.empty() );
		return m_strTexture2DOffset;
	}
	inline castor::String const & KeywordsBase::getTexture2DOffsetLod()const
	{
		REQUIRE( !m_strTexture2DOffsetLod.empty() );
		return m_strTexture2DOffsetLod;
	}
	inline castor::String const & KeywordsBase::getTexture2DLodOffset()const
	{
		REQUIRE( !m_strTexture2DLodOffset.empty() );
		return m_strTexture2DLodOffset;
	}
	inline castor::String const & KeywordsBase::getTexture3D()const
	{
		REQUIRE( !m_strTexture3D.empty() );
		return m_strTexture3D;
	}
	inline castor::String const & KeywordsBase::getTexture3DLod()const
	{
		REQUIRE( !m_strTexture3DLod.empty() );
		return m_strTexture3DLod;
	}
	inline castor::String const & KeywordsBase::getTexture3DOffset()const
	{
		REQUIRE( !m_strTexture3DOffset.empty() );
		return m_strTexture3DOffset;
	}
	inline castor::String const & KeywordsBase::getTexture3DOffsetLod()const
	{
		REQUIRE( !m_strTexture3DOffsetLod.empty() );
		return m_strTexture3DOffsetLod;
	}
	inline castor::String const & KeywordsBase::getTexture3DLodOffset()const
	{
		REQUIRE( !m_strTexture3DLodOffset.empty() );
		return m_strTexture3DLodOffset;
	}
	inline castor::String const & KeywordsBase::getTextureCube()const
	{
		REQUIRE( !m_strTextureCube.empty() );
		return m_strTextureCube;
	}
	inline castor::String const & KeywordsBase::getTextureCubeLod()const
	{
		REQUIRE( !m_strTextureCubeLod.empty() );
		return m_strTextureCubeLod;
	}
	inline castor::String const & KeywordsBase::getTexture1DArray()const
	{
		REQUIRE( !m_strTexture1DArray.empty() );
		return m_strTexture1DArray;
	}
	inline castor::String const & KeywordsBase::getTexture1DArrayLod()const
	{
		REQUIRE( !m_strTexture1DArrayLod.empty() );
		return m_strTexture1DArrayLod;
	}
	inline castor::String const & KeywordsBase::getTexture1DArrayOffset()const
	{
		REQUIRE( !m_strTexture1DArrayOffset.empty() );
		return m_strTexture1DArrayOffset;
	}
	inline castor::String const & KeywordsBase::getTexture1DArrayOffsetLod()const
	{
		REQUIRE( !m_strTexture1DArrayOffsetLod.empty() );
		return m_strTexture1DArrayOffsetLod;
	}
	inline castor::String const & KeywordsBase::getTexture1DArrayLodOffset()const
	{
		REQUIRE( !m_strTexture1DArrayLodOffset.empty() );
		return m_strTexture1DArrayLodOffset;
	}
	inline castor::String const & KeywordsBase::getTexture2DArray()const
	{
		REQUIRE( !m_strTexture2DArray.empty() );
		return m_strTexture2DArray;
	}
	inline castor::String const & KeywordsBase::getTexture2DArrayLod()const
	{
		REQUIRE( !m_strTexture2DArrayLod.empty() );
		return m_strTexture2DArrayLod;
	}
	inline castor::String const & KeywordsBase::getTexture2DArrayOffset()const
	{
		REQUIRE( !m_strTexture2DArrayOffset.empty() );
		return m_strTexture2DArrayOffset;
	}
	inline castor::String const & KeywordsBase::getTexture2DArrayOffsetLod()const
	{
		REQUIRE( !m_strTexture2DArrayOffsetLod.empty() );
		return m_strTexture2DArrayOffsetLod;
	}
	inline castor::String const & KeywordsBase::getTexture2DArrayLodOffset()const
	{
		REQUIRE( !m_strTexture2DArrayLodOffset.empty() );
		return m_strTexture2DArrayLodOffset;
	}
	inline castor::String const & KeywordsBase::getTextureCubeArray()const
	{
		REQUIRE( !m_strTextureCubeArray.empty() );
		return m_strTextureCubeArray;
	}
	inline castor::String const & KeywordsBase::getTextureCubeArrayLod()const
	{
		REQUIRE( !m_strTextureCubeShadowLod.empty() );
		return m_strTextureCubeShadowLod;
	}
	inline castor::String const & KeywordsBase::getTexture1DShadow()const
	{
		REQUIRE( !m_strTexture1DShadow.empty() );
		return m_strTexture1DShadow;
	}
	inline castor::String const & KeywordsBase::getTexture1DShadowLod()const
	{
		REQUIRE( !m_strTexture1DShadowLod.empty() );
		return m_strTexture1DShadowLod;
	}
	inline castor::String const & KeywordsBase::getTexture1DShadowOffset()const
	{
		REQUIRE( !m_strTexture1DShadowOffset.empty() );
		return m_strTexture1DShadowOffset;
	}
	inline castor::String const & KeywordsBase::getTexture1DShadowOffsetLod()const
	{
		REQUIRE( !m_strTexture1DShadowOffsetLod.empty() );
		return m_strTexture1DShadowOffsetLod;
	}
	inline castor::String const & KeywordsBase::getTexture1DShadowLodOffset()const
	{
		REQUIRE( !m_strTexture1DShadowLodOffset.empty() );
		return m_strTexture1DShadowLodOffset;
	}
	inline castor::String const & KeywordsBase::getTexture2DShadow()const
	{
		REQUIRE( !m_strTexture2DShadow.empty() );
		return m_strTexture2DShadow;
	}
	inline castor::String const & KeywordsBase::getTexture2DShadowLod()const
	{
		REQUIRE( !m_strTexture2DShadowLod.empty() );
		return m_strTexture2DShadowLod;
	}
	inline castor::String const & KeywordsBase::getTexture2DShadowOffset()const
	{
		REQUIRE( !m_strTexture2DShadowOffset.empty() );
		return m_strTexture2DShadowOffset;
	}
	inline castor::String const & KeywordsBase::getTexture2DShadowOffsetLod()const
	{
		REQUIRE( !m_strTexture2DShadowOffsetLod.empty() );
		return m_strTexture2DShadowOffsetLod;
	}
	inline castor::String const & KeywordsBase::getTexture2DShadowLodOffset()const
	{
		REQUIRE( !m_strTexture2DShadowLodOffset.empty() );
		return m_strTexture2DShadowLodOffset;
	}
	inline castor::String const & KeywordsBase::getTextureCubeShadow()const
	{
		REQUIRE( !m_strTextureCubeShadow.empty() );
		return m_strTextureCubeShadow;
	}
	inline castor::String const & KeywordsBase::getTextureCubeShadowLod()const
	{
		REQUIRE( !m_strTextureCubeShadowLod.empty() );
		return m_strTextureCubeShadowLod;
	}
	inline castor::String const & KeywordsBase::getTexture1DArrayShadow()const
	{
		REQUIRE( !m_strTexture1DArrayShadow.empty() );
		return m_strTexture1DArrayShadow;
	}
	inline castor::String const & KeywordsBase::getTexture1DArrayShadowLod()const
	{
		REQUIRE( !m_strTexture1DArrayShadowLod.empty() );
		return m_strTexture1DArrayShadowLod;
	}
	inline castor::String const & KeywordsBase::getTexture1DArrayShadowOffset()const
	{
		REQUIRE( !m_strTexture1DArrayShadowOffset.empty() );
		return m_strTexture1DArrayShadowOffset;
	}
	inline castor::String const & KeywordsBase::getTexture1DArrayShadowOffsetLod()const
	{
		REQUIRE( !m_strTexture1DArrayShadowOffsetLod.empty() );
		return m_strTexture1DArrayShadowOffsetLod;
	}
	inline castor::String const & KeywordsBase::getTexture1DArrayShadowLodOffset()const
	{
		REQUIRE( !m_strTexture1DArrayShadowLodOffset.empty() );
		return m_strTexture1DArrayShadowLodOffset;
	}
	inline castor::String const & KeywordsBase::getTexture2DArrayShadow()const
	{
		REQUIRE( !m_strTexture2DArrayShadow.empty() );
		return m_strTexture2DArrayShadow;
	}
	inline castor::String const & KeywordsBase::getTexture2DArrayShadowLod()const
	{
		REQUIRE( !m_strTexture2DArrayShadowLod.empty() );
		return m_strTexture2DArrayShadowLod;
	}
	inline castor::String const & KeywordsBase::getTexture2DArrayShadowOffset()const
	{
		REQUIRE( !m_strTexture2DArrayShadowOffset.empty() );
		return m_strTexture2DArrayShadowOffset;
	}
	inline castor::String const & KeywordsBase::getTexture2DArrayShadowOffsetLod()const
	{
		REQUIRE( !m_strTexture2DArrayShadowOffsetLod.empty() );
		return m_strTexture2DArrayShadowOffsetLod;
	}
	inline castor::String const & KeywordsBase::getTexture2DArrayShadowLodOffset()const
	{
		REQUIRE( !m_strTexture2DArrayShadowLodOffset.empty() );
		return m_strTexture2DArrayShadowLodOffset;
	}
	inline castor::String const & KeywordsBase::getTextureCubeArrayShadow()const
	{
		REQUIRE( !m_strTextureCubeArrayShadow.empty() );
		return m_strTextureCubeArrayShadow;
	}
	inline castor::String const & KeywordsBase::getTextureCubeArrayShadowLod()const
	{
		REQUIRE( !m_strTextureCubeArrayShadowLod.empty() );
		return m_strTextureCubeArrayShadowLod;
	}
	inline castor::String const & KeywordsBase::getTexelFetchBuffer()const
	{
		REQUIRE( !m_strTexelFetchBuffer.empty() );
		return m_strTexelFetchBuffer;
	}
	inline castor::String const & KeywordsBase::getTexelFetch1D()const
	{
		REQUIRE( !m_strTexelFetch1D.empty() );
		return m_strTexelFetch1D;
	}
	inline castor::String const & KeywordsBase::getTexelFetch2D()const
	{
		REQUIRE( !m_strTexelFetch2D.empty() );
		return m_strTexelFetch2D;
	}
	inline castor::String const & KeywordsBase::getTexelFetch3D()const
	{
		REQUIRE( !m_strTexelFetch3D.empty() );
		return m_strTexelFetch3D;
	}

	template< int Version >
	class Keywords < Version, typename std::enable_if< ( Version <= 120 ) >::type >
		: public KeywordsBase
	{
	public:
		Keywords()
		{
			m_strAttribute = cuT( "attribute" );
			m_strIn = cuT( "varying" );
			m_strOut = cuT( "varying" );
			m_strTexture1D = cuT( "texture1D" );
			m_strTexture1DLod = cuT( "texture1DLod" );
			m_strTexture2D = cuT( "texture2D" );
			m_strTexture2DLod = cuT( "texture2DLod" );
			m_strTexture3D = cuT( "texture3D" );
			m_strTexture3DLod = cuT( "texture3DLod" );
			m_strTextureCube = cuT( "textureCube" );
			m_strTextureCubeLod = cuT( "textureCubeLod" );
			m_strTexture1DArray = cuT( "texture1D" );
			m_strTexture1DArrayLod = cuT( "texture1D" );
			m_strTexture2DArray = cuT( "texture2D" );
			m_strTexture2DArrayLod = cuT( "texture2D" );
			m_strTextureCubeArray = cuT( "textureCube" );
			m_strTextureCubeArrayLod = cuT( "textureCubeLod" );
			m_strTexelFetch1D = cuT( "texture1D" );
			m_strTexelFetch2D = cuT( "texture2D" );
			m_strTexelFetch3D = cuT( "texture3D" );
			m_strTexelFetch3D = cuT( "texture3D" );
		}

		virtual castor::String getLayout( uint32_t CU_PARAM_UNUSED( p_index ) )const
		{
			return cuT( "" );
		}

		virtual castor::String getFragData( uint32_t p_index )const
		{
			castor::StringStream stream;
			stream << cuT( "gl_FragData[" ) << p_index << cuT( "]" );
			return stream.str();
		}

		virtual bool hasNamedFragData()const
		{
			return false;
		}
	};

	template< int Version >
	class Keywords < Version, typename std::enable_if < ( Version > 120 ) && ( Version < 140 ) >::type >
		: public KeywordsBase
	{
	public:
		Keywords()
		{
			m_version = cuT( "#version " ) + castor::string::toString( Version );
			m_strAttribute = cuT( "in" );
			m_strIn = cuT( "in" );
			m_strOut = cuT( "out" );
			m_strTexture1D = cuT( "texture" );
			m_strTexture1DLod = cuT( "textureLod" );
			m_strTexture1DOffset = cuT( "textureOffset" );
			m_strTexture1DOffsetLod = cuT( "textureLodOffset" );
			m_strTexture1DLodOffset = cuT( "textureLodOffset" );
			m_strTexture2D = cuT( "texture" );
			m_strTexture2DLod = cuT( "textureLod" );
			m_strTexture2DOffset = cuT( "textureOffset" );
			m_strTexture2DOffsetLod = cuT( "textureLodOffset" );
			m_strTexture2DLodOffset = cuT( "textureLodOffset" );
			m_strTexture3D = cuT( "texture" );
			m_strTexture3DLod = cuT( "textureLod" );
			m_strTexture3DOffset = cuT( "textureOffset" );
			m_strTexture3DOffsetLod = cuT( "textureLodOffset" );
			m_strTexture3DLodOffset = cuT( "textureLodOffset" );
			m_strTextureCube = cuT( "texture" );
			m_strTextureCubeLod = cuT( "textureLod" );
			m_strTexture1DArray = cuT( "texture" );
			m_strTexture1DArrayLod = cuT( "textureLod" );
			m_strTexture1DArrayOffset = cuT( "textureOffset" );
			m_strTexture1DArrayOffsetLod = cuT( "textureLodOffset" );
			m_strTexture1DArrayLodOffset = cuT( "textureLodOffset" );
			m_strTexture2DArray = cuT( "texture" );
			m_strTexture2DArrayLod = cuT( "textureLod" );
			m_strTexture2DArrayOffset = cuT( "textureOffset" );
			m_strTexture2DArrayOffsetLod = cuT( "textureLodOffset" );
			m_strTexture2DArrayLodOffset = cuT( "textureLodOffset" );
			m_strTextureCubeArray = cuT( "texture" );
			m_strTextureCubeArrayLod = cuT( "textureLod" );
			m_strTexture1DShadow = cuT( "texture" );
			m_strTexture1DShadowLod = cuT( "textureLod" );
			m_strTexture1DShadowOffset = cuT( "textureOffset" );
			m_strTexture1DShadowOffsetLod = cuT( "textureLodOffset" );
			m_strTexture1DShadowLodOffset = cuT( "textureLodOffset" );
			m_strTexture2DShadow = cuT( "texture" );
			m_strTexture2DShadowLod = cuT( "textureLod" );
			m_strTexture2DShadowOffset = cuT( "textureOffset" );
			m_strTexture2DShadowOffsetLod = cuT( "textureLodOffset" );
			m_strTexture2DShadowLodOffset = cuT( "textureLodOffset" );
			m_strTextureCubeShadow = cuT( "texture" );
			m_strTextureCubeShadowLod = cuT( "textureLod" );
			m_strTexture1DArrayShadow = cuT( "texture" );
			m_strTexture1DArrayShadowLod = cuT( "textureLod" );
			m_strTexture1DArrayShadowOffset = cuT( "textureOffset" );
			m_strTexture1DArrayShadowOffsetLod = cuT( "textureLodOffset" );
			m_strTexture1DArrayShadowLodOffset = cuT( "textureLodOffset" );
			m_strTexture2DArrayShadow = cuT( "texture" );
			m_strTexture2DArrayShadowLod = cuT( "textureLod" );
			m_strTexture2DArrayShadowOffset = cuT( "textureOffset" );
			m_strTexture2DArrayShadowOffsetLod = cuT( "textureLodOffset" );
			m_strTexture2DArrayShadowLodOffset = cuT( "textureLodOffset" );
			m_strTextureCubeArrayShadow = cuT( "texture" );
			m_strTextureCubeArrayShadowLod = cuT( "textureLod" );
			m_strTextureCubeShadow = cuT( "texture" );
			m_strTextureCubeShadowLod = cuT( "textureLod" );
			m_strTexelFetchBuffer = cuT( "texelFetch" );
			m_strTexelFetch1D = cuT( "texelFetch" );
			m_strTexelFetch2D = cuT( "texelFetch" );
			m_strTexelFetch3D = cuT( "texelFetch" );
		}

		virtual castor::String getLayout( uint32_t CU_PARAM_UNUSED( p_index ) )const
		{
			return cuT( "" );
		}

		virtual castor::String getFragData( uint32_t p_index )const
		{
			castor::StringStream stream;
			stream << cuT( "gl_FragData[" ) << p_index << cuT( "]" );
			return stream.str();
		}

		virtual bool hasNamedFragData()const
		{
			return false;
		}
	};

	template< int Version >
	class Keywords < Version, typename std::enable_if < ( Version >= 140 ) && ( Version < 330 ) >::type >
		: public KeywordsBase
	{
	public:
		Keywords()
		{
			m_strUboLayout = cuT( "layout" );
			m_version = cuT( "#version " ) + castor::string::toString( Version );
			m_strAttribute = cuT( "in" );
			m_strIn = cuT( "in" );
			m_strOut = cuT( "out" );
			m_strTexture1D = cuT( "texture" );
			m_strTexture1DLod = cuT( "texture" );
			m_strTexture1DOffset = cuT( "textureOffset" );
			m_strTexture1DOffsetLod = cuT( "textureOffset" );
			m_strTexture1DLodOffset = cuT( "textureLodOffset" );
			m_strTexture2D = cuT( "texture" );
			m_strTexture2DLod = cuT( "texture" );
			m_strTexture2DOffset = cuT( "textureOffset" );
			m_strTexture2DOffsetLod = cuT( "textureOffset" );
			m_strTexture2DLodOffset = cuT( "textureLodOffset" );
			m_strTexture3D = cuT( "texture" );
			m_strTexture3DLod = cuT( "texture" );
			m_strTexture3DOffset = cuT( "textureOffset" );
			m_strTexture3DOffsetLod = cuT( "textureOffset" );
			m_strTexture3DLodOffset = cuT( "textureLodOffset" );
			m_strTextureCube = cuT( "texture" );
			m_strTextureCubeLod = cuT( "texture" );
			m_strTexture1DArray = cuT( "texture" );
			m_strTexture1DArrayLod = cuT( "texture" );
			m_strTexture1DArrayOffset = cuT( "textureOffset" );
			m_strTexture1DArrayOffsetLod = cuT( "textureOffset" );
			m_strTexture1DArrayLodOffset = cuT( "textureLodOffset" );
			m_strTexture2DArray = cuT( "texture" );
			m_strTexture2DArrayLod = cuT( "texture" );
			m_strTexture2DArrayOffset = cuT( "textureOffset" );
			m_strTexture2DArrayOffsetLod = cuT( "textureOffset" );
			m_strTexture2DArrayLodOffset = cuT( "textureLodOffset" );
			m_strTextureCubeArray = cuT( "texture" );
			m_strTextureCubeArrayLod = cuT( "texture" );
			m_strTexture1DShadow = cuT( "texture" );
			m_strTexture1DShadowLod = cuT( "texture" );
			m_strTexture1DShadowOffset = cuT( "textureOffset" );
			m_strTexture1DShadowOffsetLod = cuT( "textureOffset" );
			m_strTexture1DShadowLodOffset = cuT( "textureLodOffset" );
			m_strTexture2DShadow = cuT( "texture" );
			m_strTexture2DShadowLod = cuT( "texture" );
			m_strTexture2DShadowOffset = cuT( "textureOffset" );
			m_strTexture2DShadowOffsetLod = cuT( "textureOffset" );
			m_strTexture2DShadowLodOffset = cuT( "textureLodOffset" );
			m_strTextureCubeShadow = cuT( "texture" );
			m_strTextureCubeShadowLod = cuT( "texture" );
			m_strTexture1DArrayShadow = cuT( "texture" );
			m_strTexture1DArrayShadowLod = cuT( "texture" );
			m_strTexture1DArrayShadowOffset = cuT( "textureOffset" );
			m_strTexture1DArrayShadowOffsetLod = cuT( "textureOffset" );
			m_strTexture1DArrayShadowLodOffset = cuT( "textureLodOffset" );
			m_strTexture2DArrayShadow = cuT( "texture" );
			m_strTexture2DArrayShadowLod = cuT( "texture" );
			m_strTexture2DArrayShadowOffset = cuT( "textureOffset" );
			m_strTexture2DArrayShadowOffsetLod = cuT( "textureOffset" );
			m_strTexture2DArrayShadowLodOffset = cuT( "textureLodOffset" );
			m_strTextureCubeArrayShadow = cuT( "texture" );
			m_strTextureCubeArrayShadowLod = cuT( "texture" );
			m_strTexelFetchBuffer = cuT( "texelFetch" );
			m_strTexelFetch1D = cuT( "texelFetch" );
			m_strTexelFetch2D = cuT( "texelFetch" );
			m_strTexelFetch3D = cuT( "texelFetch" );
		}

		virtual castor::String getLayout( uint32_t CU_PARAM_UNUSED( p_index ) )const
		{
			return cuT( "" );
		}

		virtual castor::String getFragData( uint32_t CU_PARAM_UNUSED( p_index ) )const
		{
			return cuT( "" );
		}

		virtual bool hasNamedFragData()const
		{
			return true;
		}
	};

	template< int Version >
	class Keywords< Version, typename std::enable_if< ( Version >= 330 ) && ( Version < 420 ) >::type >
		: public KeywordsBase
	{
	public:
		Keywords()
		{
			m_strUboLayout = cuT( "layout" );
			m_strSsboLayout = cuT( "layout" );
			m_version = cuT( "#version " ) + castor::string::toString( Version );
			m_strAttribute = cuT( "in" );
			m_strIn = cuT( "in" );
			m_strOut = cuT( "out" );
			m_strTexture1D = cuT( "texture" );
			m_strTexture1DLod = cuT( "textureLod" );
			m_strTexture1DOffset = cuT( "textureOffset" );
			m_strTexture1DOffsetLod = cuT( "textureOffset" );
			m_strTexture1DLodOffset = cuT( "textureLodOffset" );
			m_strTexture2D = cuT( "texture" );
			m_strTexture2DLod = cuT( "textureLod" );
			m_strTexture2DOffset = cuT( "textureOffset" );
			m_strTexture2DOffsetLod = cuT( "textureOffset" );
			m_strTexture2DLodOffset = cuT( "textureLodOffset" );
			m_strTexture3D = cuT( "texture" );
			m_strTexture3DLod = cuT( "textureLod" );
			m_strTexture3DOffset = cuT( "textureOffset" );
			m_strTexture3DOffsetLod = cuT( "textureOffset" );
			m_strTexture3DLodOffset = cuT( "textureLodOffset" );
			m_strTextureCube = cuT( "texture" );
			m_strTextureCubeLod = cuT( "textureLod" );
			m_strTexture1DArray = cuT( "texture" );
			m_strTexture1DArrayLod = cuT( "textureLod" );
			m_strTexture1DArrayOffset = cuT( "textureOffset" );
			m_strTexture1DArrayOffsetLod = cuT( "textureOffset" );
			m_strTexture1DArrayLodOffset = cuT( "textureLodOffset" );
			m_strTexture2DArray = cuT( "texture" );
			m_strTexture2DArrayLod = cuT( "textureLod" );
			m_strTexture2DArrayOffset = cuT( "textureOffset" );
			m_strTexture2DArrayOffsetLod = cuT( "textureOffset" );
			m_strTexture2DArrayLodOffset = cuT( "textureLodOffset" );
			m_strTextureCubeArray = cuT( "texture" );
			m_strTextureCubeArrayLod = cuT( "textureLod" );
			m_strTexture1DShadow = cuT( "texture" );
			m_strTexture1DShadowLod = cuT( "textureLod" );
			m_strTexture1DShadowOffset = cuT( "textureOffset" );
			m_strTexture1DShadowOffsetLod = cuT( "textureOffset" );
			m_strTexture1DShadowLodOffset = cuT( "textureLodOffset" );
			m_strTexture2DShadow = cuT( "texture" );
			m_strTexture2DShadowLod = cuT( "textureLod" );
			m_strTexture2DShadowOffset = cuT( "textureOffset" );
			m_strTexture2DShadowOffsetLod = cuT( "textureOffset" );
			m_strTexture2DShadowLodOffset = cuT( "textureLodOffset" );
			m_strTextureCubeShadow = cuT( "texture" );
			m_strTextureCubeShadowLod = cuT( "texture" );
			m_strTexture1DArrayShadow = cuT( "texture" );
			m_strTexture1DArrayShadowLod = cuT( "textureLod" );
			m_strTexture1DArrayShadowOffset = cuT( "textureOffset" );
			m_strTexture1DArrayShadowOffsetLod = cuT( "textureOffset" );
			m_strTexture1DArrayShadowLodOffset = cuT( "textureLodOffset" );
			m_strTexture2DArrayShadow = cuT( "texture" );
			m_strTexture2DArrayShadowLod = cuT( "textureLod" );
			m_strTexture2DArrayShadowOffset = cuT( "textureOffset" );
			m_strTexture2DArrayShadowOffsetLod = cuT( "textureOffset" );
			m_strTexture2DArrayShadowLodOffset = cuT( "textureLodOffset" );
			m_strTextureCubeArrayShadow = cuT( "texture" );
			m_strTextureCubeArrayShadowLod = cuT( "textureLod" );
			m_strTexelFetchBuffer = cuT( "texelFetch" );
			m_strTexelFetch1D = cuT( "texelFetch" );
			m_strTexelFetch2D = cuT( "texelFetch" );
			m_strTexelFetch3D = cuT( "texelFetch" );
		}

		virtual castor::String getLayout( uint32_t p_index )const
		{
			return cuT( "layout( location=" ) + castor::string::toString( p_index ) + cuT( " ) " );
		}

		virtual castor::String getFragData( uint32_t CU_PARAM_UNUSED( p_index ) )const
		{
			return cuT( "" );
		}

		virtual bool hasNamedFragData()const
		{
			return true;
		}
	};

	template< int Version >
	class Keywords< Version, typename std::enable_if< ( Version >= 420 ) >::type >
		: public KeywordsBase
	{
	public:
		Keywords()
		{
			m_strUboLayout = cuT( "layout" );
			m_strUboBinding = cuT( "binding" );
			m_strSsboLayout = cuT( "layout" );
			m_strSsboBinding = cuT( "binding" );
			m_version = cuT( "#version " ) + castor::string::toString( Version );
			m_strAttribute = cuT( "in" );
			m_strIn = cuT( "in" );
			m_strOut = cuT( "out" );
			m_strTexture1D = cuT( "texture" );
			m_strTexture1DLod = cuT( "textureLod" );
			m_strTexture1DOffset = cuT( "textureOffset" );
			m_strTexture1DOffsetLod = cuT( "textureOffset" );
			m_strTexture1DLodOffset = cuT( "textureLodOffset" );
			m_strTexture2D = cuT( "texture" );
			m_strTexture2DLod = cuT( "textureLod" );
			m_strTexture2DOffset = cuT( "textureOffset" );
			m_strTexture2DOffsetLod = cuT( "textureOffset" );
			m_strTexture2DLodOffset = cuT( "textureLodOffset" );
			m_strTexture3D = cuT( "texture" );
			m_strTexture3DLod = cuT( "textureLod" );
			m_strTexture3DOffset = cuT( "textureOffset" );
			m_strTexture3DOffsetLod = cuT( "textureOffset" );
			m_strTexture3DLodOffset = cuT( "textureLodOffset" );
			m_strTextureCube = cuT( "texture" );
			m_strTextureCubeLod = cuT( "textureLod" );
			m_strTexture1DArray = cuT( "texture" );
			m_strTexture1DArrayLod = cuT( "texture" );
			m_strTexture1DArrayOffset = cuT( "textureOffset" );
			m_strTexture1DArrayOffsetLod = cuT( "textureOffset" );
			m_strTexture1DArrayLodOffset = cuT( "textureLodOffset" );
			m_strTexture2DArray = cuT( "texture" );
			m_strTexture2DArrayLod = cuT( "textureLod" );
			m_strTexture2DArrayOffset = cuT( "textureOffset" );
			m_strTexture2DArrayOffsetLod = cuT( "textureOffset" );
			m_strTexture2DArrayLodOffset = cuT( "textureLodOffset" );
			m_strTextureCubeArray = cuT( "texture" );
			m_strTextureCubeArrayLod = cuT( "texture" );
			m_strTexture1DShadow = cuT( "texture" );
			m_strTexture1DShadowLod = cuT( "textureLod" );
			m_strTexture1DShadowOffset = cuT( "textureOffset" );
			m_strTexture1DShadowOffsetLod = cuT( "textureOffset" );
			m_strTexture1DShadowLodOffset = cuT( "textureLodOffset" );
			m_strTexture2DShadow = cuT( "texture" );
			m_strTexture2DShadowLod = cuT( "textureLod" );
			m_strTexture2DShadowOffset = cuT( "textureOffset" );
			m_strTexture2DShadowOffsetLod = cuT( "textureOffset" );
			m_strTexture2DShadowLodOffset = cuT( "textureLodOffset" );
			m_strTextureCubeShadow = cuT( "texture" );
			m_strTextureCubeShadowLod = cuT( "textureLod" );
			m_strTexture1DArrayShadow = cuT( "texture" );
			m_strTexture1DArrayShadowLod = cuT( "textureLod" );
			m_strTexture1DArrayShadowOffset = cuT( "textureOffset" );
			m_strTexture1DArrayShadowOffsetLod = cuT( "textureOffset" );
			m_strTexture1DArrayShadowLodOffset = cuT( "textureLodOffset" );
			m_strTexture2DArrayShadow = cuT( "texture" );
			m_strTexture2DArrayShadowLod = cuT( "textureLod" );
			m_strTexture2DArrayShadowOffset = cuT( "textureOffset" );
			m_strTexture2DArrayShadowOffsetLod = cuT( "textureOffset" );
			m_strTexture2DArrayShadowLodOffset = cuT( "textureLodOffset" );
			m_strTextureCubeArrayShadow = cuT( "texture" );
			m_strTextureCubeArrayShadowLod = cuT( "textureLod" );
			m_strTexelFetchBuffer = cuT( "texelFetch" );
			m_strTexelFetch1D = cuT( "texelFetch" );
			m_strTexelFetch2D = cuT( "texelFetch" );
			m_strTexelFetch3D = cuT( "texelFetch" );
		}

		virtual castor::String getLayout( uint32_t p_index )const
		{
			return cuT( "layout( location=" ) + castor::string::toString( p_index ) + cuT( " ) " );
		}

		virtual castor::String getFragData( uint32_t CU_PARAM_UNUSED( p_index ) )const
		{
			return cuT( "" );
		}

		virtual bool hasNamedFragData()const
		{
			return true;
		}
	};
}
