#include <Exception/Assertion.hpp>

namespace GLSL
{
	inline Castor::String KeywordsBase::GetStdLayout( int p_index )const
	{
		REQUIRE( !m_strIn.empty() );
		return m_strStdLayout + cuT( "( std" ) + Castor::string::to_string( p_index ) + cuT( " ) " );
	}
	inline Castor::String const & KeywordsBase::GetVersion()const
	{
		return m_version;
	}
	inline Castor::String const & KeywordsBase::GetIn()const
	{
		REQUIRE( !m_strIn.empty() );
		return m_strIn;
	}
	inline Castor::String const & KeywordsBase::GetOut()const
	{
		REQUIRE( !m_strOut.empty() );
		return m_strOut;
	}
	inline Castor::String const & KeywordsBase::GetTexture1D()const
	{
		REQUIRE( !m_strTexture1D.empty() );
		return m_strTexture1D;
	}
	inline Castor::String const & KeywordsBase::GetTexture1DLod()const
	{
		REQUIRE( !m_strTexture1DLod.empty() );
		return m_strTexture1DLod;
	}
	inline Castor::String const & KeywordsBase::GetTexture1DOffset()const
	{
		REQUIRE( !m_strTexture1DOffset.empty() );
		return m_strTexture1DOffset;
	}
	inline Castor::String const & KeywordsBase::GetTexture1DOffsetLod()const
	{
		REQUIRE( !m_strTexture1DOffsetLod.empty() );
		return m_strTexture1DOffsetLod;
	}
	inline Castor::String const & KeywordsBase::GetTexture1DLodOffset()const
	{
		REQUIRE( !m_strTexture1DLodOffset.empty() );
		return m_strTexture1DLodOffset;
	}
	inline Castor::String const & KeywordsBase::GetTexture2D()const
	{
		REQUIRE( !m_strTexture2D.empty() );
		return m_strTexture2D;
	}
	inline Castor::String const & KeywordsBase::GetTexture2DLod()const
	{
		REQUIRE( !m_strTexture2DLod.empty() );
		return m_strTexture2DLod;
	}
	inline Castor::String const & KeywordsBase::GetTexture2DOffset()const
	{
		REQUIRE( !m_strTexture2DOffset.empty() );
		return m_strTexture2DOffset;
	}
	inline Castor::String const & KeywordsBase::GetTexture2DOffsetLod()const
	{
		REQUIRE( !m_strTexture2DOffsetLod.empty() );
		return m_strTexture2DOffsetLod;
	}
	inline Castor::String const & KeywordsBase::GetTexture2DLodOffset()const
	{
		REQUIRE( !m_strTexture2DLodOffset.empty() );
		return m_strTexture2DLodOffset;
	}
	inline Castor::String const & KeywordsBase::GetTexture3D()const
	{
		REQUIRE( !m_strTexture3D.empty() );
		return m_strTexture3D;
	}
	inline Castor::String const & KeywordsBase::GetTexture3DLod()const
	{
		REQUIRE( !m_strTexture3DLod.empty() );
		return m_strTexture3DLod;
	}
	inline Castor::String const & KeywordsBase::GetTexture3DOffset()const
	{
		REQUIRE( !m_strTexture3DOffset.empty() );
		return m_strTexture3DOffset;
	}
	inline Castor::String const & KeywordsBase::GetTexture3DOffsetLod()const
	{
		REQUIRE( !m_strTexture3DOffsetLod.empty() );
		return m_strTexture3DOffsetLod;
	}
	inline Castor::String const & KeywordsBase::GetTexture3DLodOffset()const
	{
		REQUIRE( !m_strTexture3DLodOffset.empty() );
		return m_strTexture3DLodOffset;
	}
	inline Castor::String const & KeywordsBase::GetTextureCube()const
	{
		REQUIRE( !m_strTextureCube.empty() );
		return m_strTextureCube;
	}
	inline Castor::String const & KeywordsBase::GetTextureCubeLod()const
	{
		REQUIRE( !m_strTextureCubeLod.empty() );
		return m_strTextureCubeLod;
	}
	inline Castor::String const & KeywordsBase::GetTexture1DShadow()const
	{
		REQUIRE( !m_strTexture1DShadow.empty() );
		return m_strTexture1DShadow;
	}
	inline Castor::String const & KeywordsBase::GetTexture1DShadowLod()const
	{
		REQUIRE( !m_strTexture1DShadowLod.empty() );
		return m_strTexture1DShadowLod;
	}
	inline Castor::String const & KeywordsBase::GetTexture1DShadowOffset()const
	{
		REQUIRE( !m_strTexture1DShadowOffset.empty() );
		return m_strTexture1DShadowOffset;
	}
	inline Castor::String const & KeywordsBase::GetTexture1DShadowOffsetLod()const
	{
		REQUIRE( !m_strTexture1DShadowOffsetLod.empty() );
		return m_strTexture1DShadowOffsetLod;
	}
	inline Castor::String const & KeywordsBase::GetTexture1DShadowLodOffset()const
	{
		REQUIRE( !m_strTexture1DShadowLodOffset.empty() );
		return m_strTexture1DShadowLodOffset;
	}
	inline Castor::String const & KeywordsBase::GetTexture2DShadow()const
	{
		REQUIRE( !m_strTexture2DShadow.empty() );
		return m_strTexture2DShadow;
	}
	inline Castor::String const & KeywordsBase::GetTexture2DShadowLod()const
	{
		REQUIRE( !m_strTexture2DShadowLod.empty() );
		return m_strTexture2DShadowLod;
	}
	inline Castor::String const & KeywordsBase::GetTexture2DShadowOffset()const
	{
		REQUIRE( !m_strTexture2DShadowOffset.empty() );
		return m_strTexture2DShadowOffset;
	}
	inline Castor::String const & KeywordsBase::GetTexture2DShadowOffsetLod()const
	{
		REQUIRE( !m_strTexture2DShadowOffsetLod.empty() );
		return m_strTexture2DShadowOffsetLod;
	}
	inline Castor::String const & KeywordsBase::GetTexture2DShadowLodOffset()const
	{
		REQUIRE( !m_strTexture2DShadowLodOffset.empty() );
		return m_strTexture2DShadowLodOffset;
	}
	inline Castor::String const & KeywordsBase::GetTextureCubeShadow()const
	{
		REQUIRE( !m_strTextureCubeShadow.empty() );
		return m_strTextureCubeShadow;
	}
	inline Castor::String const & KeywordsBase::GetTextureCubeShadowLod()const
	{
		REQUIRE( !m_strTextureCubeShadowLod.empty() );
		return m_strTextureCubeShadowLod;
	}
	inline Castor::String const & KeywordsBase::GetTexelFetchBuffer()const
	{
		REQUIRE( !m_strTexelFetchBuffer.empty() );
		return m_strTexelFetchBuffer;
	}
	inline Castor::String const & KeywordsBase::GetTexelFetch1D()const
	{
		REQUIRE( !m_strTexelFetch1D.empty() );
		return m_strTexelFetch1D;
	}
	inline Castor::String const & KeywordsBase::GetTexelFetch2D()const
	{
		REQUIRE( !m_strTexelFetch2D.empty() );
		return m_strTexelFetch2D;
	}
	inline Castor::String const & KeywordsBase::GetTexelFetch3D()const
	{
		REQUIRE( !m_strTexelFetch3D.empty() );
		return m_strTexelFetch3D;
	}
	inline Castor::String const & KeywordsBase::GetPixelOut()const
	{
		REQUIRE( !m_strPixelOut.empty() );
		return m_strPixelOut;
	}
	inline Castor::String const & KeywordsBase::GetPixelOutputName()const
	{
		REQUIRE( !m_strPixelOutputName.empty() );
		return m_strPixelOutputName;
	}
	inline Castor::String const & KeywordsBase::GetGSOutPositionName()const
	{
		REQUIRE( !m_strGSOutPositionName.empty() );
		return m_strGSOutPositionName;
	}
	inline Castor::String const & KeywordsBase::GetGSOutNormalName()const
	{
		REQUIRE( !m_strGSOutNormalName.empty() );
		return m_strGSOutNormalName;
	}
	inline Castor::String const & KeywordsBase::GetGSOutTangentName()const
	{
		REQUIRE( !m_strGSOutTangentName.empty() );
		return m_strGSOutTangentName;
	}
	inline Castor::String const & KeywordsBase::GetGSOutBitangentName()const
	{
		REQUIRE( !m_strGSOutBitangentName.empty() );
		return m_strGSOutBitangentName;
	}
	inline Castor::String const & KeywordsBase::GetGSOutDiffuseName()const
	{
		REQUIRE( !m_strGSOutDiffuseName.empty() );
		return m_strGSOutDiffuseName;
	}
	inline Castor::String const & KeywordsBase::GetGSOutSpecularName()const
	{
		REQUIRE( !m_strGSOutSpecularName.empty() );
		return m_strGSOutSpecularName;
	}
	inline Castor::String const & KeywordsBase::GetGSOutEmissiveName()const
	{
		REQUIRE( !m_strGSOutEmissiveName.empty() );
		return m_strGSOutEmissiveName;
	}
	inline Castor::String const & KeywordsBase::GetGSOutPositionDecl()const
	{
		REQUIRE( !m_strGSOutPositionDecl.empty() );
		return m_strGSOutPositionDecl;
	}
	inline Castor::String const & KeywordsBase::GetGSOutNormalDecl()const
	{
		REQUIRE( !m_strGSOutNormalDecl.empty() );
		return m_strGSOutNormalDecl;
	}
	inline Castor::String const & KeywordsBase::GetGSOutTangentDecl()const
	{
		REQUIRE( !m_strGSOutTangentDecl.empty() );
		return m_strGSOutTangentDecl;
	}
	inline Castor::String const & KeywordsBase::GetGSOutBitangentDecl()const
	{
		REQUIRE( !m_strGSOutBitangentDecl.empty() );
		return m_strGSOutBitangentDecl;
	}
	inline Castor::String const & KeywordsBase::GetGSOutDiffuseDecl()const
	{
		REQUIRE( !m_strGSOutDiffuseDecl.empty() );
		return m_strGSOutDiffuseDecl;
	}
	inline Castor::String const & KeywordsBase::GetGSOutSpecularDecl()const
	{
		REQUIRE( !m_strGSOutSpecularDecl.empty() );
		return m_strGSOutSpecularDecl;
	}
	inline Castor::String const & KeywordsBase::GetGSOutEmissiveDecl()const
	{
		REQUIRE( !m_strGSOutEmissiveDecl.empty() );
		return m_strGSOutEmissiveDecl;
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
			m_strTexture1DShadow = cuT( "texture1DShadow" );
			m_strTexture1DShadowLod = cuT( "texture1DShadowLod" );
			m_strTexture2DShadow = cuT( "texture2DShadow" );
			m_strTexture2DShadowLod = cuT( "texture2DShadowLod" );
			m_strTextureCubeShadow = cuT( "textureCubeShadow" );
			m_strTextureCubeShadowLod = cuT( "textureCubeShadowLod" );
			m_strTexelFetch1D = cuT( "texture1D" );
			m_strTexelFetch2D = cuT( "texture2D" );
			m_strTexelFetch3D = cuT( "texture3D" );
			m_strTexelFetch3D = cuT( "texture3D" );
			m_strPixelOutputName = cuT( "gl_FragColor" );
			m_strGSOutPositionName = cuT( "gl_FragData[0]" );
			m_strGSOutNormalName = cuT( "gl_FragData[1]" );
			m_strGSOutTangentName = cuT( "gl_FragData[2]" );
			m_strGSOutBitangentName = cuT( "gl_FragData[3]" );
			m_strGSOutDiffuseName = cuT( "gl_FragData[4]" );
			m_strGSOutSpecularName = cuT( "gl_FragData[5]" );
			m_strGSOutEmissiveName = cuT( "gl_FragData[6]" );
		}

		virtual Castor::String GetLayout( uint32_t CU_PARAM_UNUSED( p_index ) )const
		{
			return cuT( "" );
		}

		virtual Castor::String GetFragData( uint32_t p_index )const
		{
			Castor::StringStream l_stream;
			l_stream << cuT( "gl_FragData[" ) << p_index << cuT( "]" );
			return l_stream.str();
		}

		virtual bool HasNamedFragData()const
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
			m_version = cuT( "#version " ) + Castor::string::to_string( Version );
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
			m_strTexelFetchBuffer = cuT( "texelFetch" );
			m_strTexelFetch1D = cuT( "texelFetch" );
			m_strTexelFetch2D = cuT( "texelFetch" );
			m_strTexelFetch3D = cuT( "texelFetch" );
			m_strPixelOut = cuT( "out vec4 pxl_v4FragColor;" );
			m_strPixelOutputName = cuT( "pxl_v4FragColor" );
			m_strGSOutPositionName = cuT( "gl_FragData[0]" );
			m_strGSOutDiffuseName = cuT( "gl_FragData[1]" );
			m_strGSOutNormalName = cuT( "gl_FragData[2]" );
			m_strGSOutTangentName = cuT( "gl_FragData[3]" );
			m_strGSOutBitangentName = cuT( "gl_FragData[4]" );
			m_strGSOutSpecularName = cuT( "gl_FragData[5]" );
			m_strGSOutEmissiveName = cuT( "gl_FragData[6]" );
		}

		virtual Castor::String GetLayout( uint32_t CU_PARAM_UNUSED( p_index ) )const
		{
			return cuT( "" );
		}

		virtual Castor::String GetFragData( uint32_t p_index )const
		{
			Castor::StringStream l_stream;
			l_stream << cuT( "gl_FragData[" ) << p_index << cuT( "]" );
			return l_stream.str();
		}

		virtual bool HasNamedFragData()const
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
			m_strStdLayout = cuT( "layout" );
			m_version = cuT( "#version " ) + Castor::string::to_string( Version );
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
			m_strTexelFetchBuffer = cuT( "texelFetch" );
			m_strTexelFetch1D = cuT( "texelFetch" );
			m_strTexelFetch2D = cuT( "texelFetch" );
			m_strTexelFetch3D = cuT( "texelFetch" );
			m_strPixelOut = cuT( "out vec4 pxl_v4FragColor;" );
			m_strPixelOutputName = cuT( "pxl_v4FragColor" );
			m_strGSOutPositionName = cuT( "out_c3dPosition" );
			m_strGSOutNormalName = cuT( "out_c3dNormals" );
			m_strGSOutTangentName = cuT( "out_c3dTangent" );
			m_strGSOutBitangentName = cuT( "out_c3dBitangent" );
			m_strGSOutDiffuseName = cuT( "out_c3dDiffuse" );
			m_strGSOutSpecularName = cuT( "out_c3dSpecular" );
			m_strGSOutEmissiveName = cuT( "out_c3dEmissive" );
			m_strGSOutPositionDecl = GetLayout( 0 ) + m_strOut + cuT( " vec4 " ) + m_strGSOutPositionName + cuT( ";" );
			m_strGSOutDiffuseDecl = GetLayout( 1 ) + m_strOut + cuT( " vec4 " ) + m_strGSOutDiffuseName + cuT( ";" );
			m_strGSOutNormalDecl = GetLayout( 2 ) + m_strOut + cuT( " vec4 " ) + m_strGSOutNormalName + cuT( ";" );
			m_strGSOutTangentDecl = GetLayout( 3 ) + m_strOut + cuT( " vec4 " ) + m_strGSOutTangentName + cuT( ";" );
			m_strGSOutBitangentDecl = GetLayout( 4 ) + m_strOut + cuT( " vec4 " ) + m_strGSOutBitangentName + cuT( ";" );
			m_strGSOutSpecularDecl = GetLayout( 5 ) + m_strOut + cuT( " vec4 " ) + m_strGSOutSpecularName + cuT( ";" );
			m_strGSOutEmissiveDecl = GetLayout( 6 ) + m_strOut + cuT( " vec4 " ) + m_strGSOutEmissiveName + cuT( ";" );
		}

		virtual Castor::String GetLayout( uint32_t CU_PARAM_UNUSED( p_index ) )const
		{
			return cuT( "" );
		}

		virtual Castor::String GetFragData( uint32_t CU_PARAM_UNUSED( p_index ) )const
		{
			return cuT( "" );
		}

		virtual bool HasNamedFragData()const
		{
			return true;
		}
	};

	template< int Version >
	class Keywords< Version, typename std::enable_if< ( Version >= 330 ) >::type >
		: public KeywordsBase
	{
	public:
		Keywords()
		{
			m_strStdLayout = cuT( "layout" );
			m_version = cuT( "#version " ) + Castor::string::to_string( Version );
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
			m_strTexelFetchBuffer = cuT( "texelFetch" );
			m_strTexelFetch1D = cuT( "texelFetch" );
			m_strTexelFetch2D = cuT( "texelFetch" );
			m_strTexelFetch3D = cuT( "texelFetch" );
			m_strPixelOut = cuT( "out vec4 pxl_v4FragColor;" );
			m_strPixelOutputName = cuT( "pxl_v4FragColor" );
			m_strGSOutPositionName = cuT( "out_c3dPosition" );
			m_strGSOutNormalName = cuT( "out_c3dNormals" );
			m_strGSOutTangentName = cuT( "out_c3dTangent" );
			m_strGSOutBitangentName = cuT( "out_c3dBitangent" );
			m_strGSOutDiffuseName = cuT( "out_c3dDiffuse" );
			m_strGSOutSpecularName = cuT( "out_c3dSpecular" );
			m_strGSOutEmissiveName = cuT( "out_c3dEmissive" );
			m_strGSOutPositionDecl = GetLayout( 0 ) + m_strOut + cuT( " vec4 " ) + m_strGSOutPositionName + cuT( ";" );
			m_strGSOutDiffuseDecl = GetLayout( 1 ) + m_strOut + cuT( " vec4 " ) + m_strGSOutDiffuseName + cuT( ";" );
			m_strGSOutNormalDecl = GetLayout( 2 ) + m_strOut + cuT( " vec4 " ) + m_strGSOutNormalName + cuT( ";" );
			m_strGSOutTangentDecl = GetLayout( 3 ) + m_strOut + cuT( " vec4 " ) + m_strGSOutTangentName + cuT( ";" );
			m_strGSOutBitangentDecl = GetLayout( 4 ) + m_strOut + cuT( " vec4 " ) + m_strGSOutBitangentName + cuT( ";" );
			m_strGSOutSpecularDecl = GetLayout( 5 ) + m_strOut + cuT( " vec4 " ) + m_strGSOutSpecularName + cuT( ";" );
			m_strGSOutEmissiveDecl = GetLayout( 6 ) + m_strOut + cuT( " vec4 " ) + m_strGSOutEmissiveName + cuT( ";" );
		}

		virtual Castor::String GetLayout( uint32_t p_index )const
		{
			return cuT( "layout( location=" ) + Castor::string::to_string( p_index ) + cuT( " ) " );
		}

		virtual Castor::String GetFragData( uint32_t CU_PARAM_UNUSED( p_index ) )const
		{
			return cuT( "" );
		}

		virtual bool HasNamedFragData()const
		{
			return true;
		}
	};
}
