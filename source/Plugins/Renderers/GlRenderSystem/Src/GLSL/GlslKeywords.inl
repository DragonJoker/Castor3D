namespace GlRender
{
	namespace GLSL
	{
		inline Castor::String KeywordsBase::GetStdLayout( int p_index )const
		{
			return m_strStdLayout + cuT( "( std" ) + Castor::string::to_string( p_index ) + cuT( " ) " );
		}
		inline Castor::String const & KeywordsBase::GetVersion()const
		{
			return m_version;
		}
		inline Castor::String const & KeywordsBase::GetIn()const
		{
			return m_strIn;
		}
		inline Castor::String const & KeywordsBase::GetOut()const
		{
			return m_strOut;
		}
		inline Castor::String const & KeywordsBase::GetTexture1D()const
		{
			return m_strTexture1D;
		}
		inline Castor::String const & KeywordsBase::GetTexture2D()const
		{
			return m_strTexture2D;
		}
		inline Castor::String const & KeywordsBase::GetTexture3D()const
		{
			return m_strTexture3D;
		}
		inline Castor::String const & KeywordsBase::GetTexelFetchBuffer()const
		{
			return m_strTexelFetchBuffer;
		}
		inline Castor::String const & KeywordsBase::GetTexelFetch1D()const
		{
			return m_strTexelFetch1D;
		}
		inline Castor::String const & KeywordsBase::GetTexelFetch2D()const
		{
			return m_strTexelFetch2D;
		}
		inline Castor::String const & KeywordsBase::GetTexelFetch3D()const
		{
			return m_strTexelFetch3D;
		}
		inline Castor::String const & KeywordsBase::GetPixelOut()const
		{
			return m_strPixelOut;
		}
		inline Castor::String const & KeywordsBase::GetPixelOutputName()const
		{
			return m_strPixelOutputName;
		}
		inline Castor::String const & KeywordsBase::GetGSOutPositionName()const
		{
			return m_strGSOutPositionName;
		}
		inline Castor::String const & KeywordsBase::GetGSOutNormalName()const
		{
			return m_strGSOutNormalName;
		}
		inline Castor::String const & KeywordsBase::GetGSOutTangentName()const
		{
			return m_strGSOutTangentName;
		}
		inline Castor::String const & KeywordsBase::GetGSOutBitangentName()const
		{
			return m_strGSOutBitangentName;
		}
		inline Castor::String const & KeywordsBase::GetGSOutDiffuseName()const
		{
			return m_strGSOutDiffuseName;
		}
		inline Castor::String const & KeywordsBase::GetGSOutSpecularName()const
		{
			return m_strGSOutSpecularName;
		}
		inline Castor::String const & KeywordsBase::GetGSOutEmissiveName()const
		{
			return m_strGSOutEmissiveName;
		}
		inline Castor::String const & KeywordsBase::GetGSOutPositionDecl()const
		{
			return m_strGSOutPositionDecl;
		}
		inline Castor::String const & KeywordsBase::GetGSOutNormalDecl()const
		{
			return m_strGSOutNormalDecl;
		}
		inline Castor::String const & KeywordsBase::GetGSOutTangentDecl()const
		{
			return m_strGSOutTangentDecl;
		}
		inline Castor::String const & KeywordsBase::GetGSOutBitangentDecl()const
		{
			return m_strGSOutBitangentDecl;
		}
		inline Castor::String const & KeywordsBase::GetGSOutDiffuseDecl()const
		{
			return m_strGSOutDiffuseDecl;
		}
		inline Castor::String const & KeywordsBase::GetGSOutSpecularDecl()const
		{
			return m_strGSOutSpecularDecl;
		}
		inline Castor::String const & KeywordsBase::GetGSOutEmissiveDecl()const
		{
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
				m_strTexture2D = cuT( "texture2D" );
				m_strTexture3D = cuT( "texture3D" );
				m_strTexelFetch1D = cuT( "texture1D" );
				m_strTexelFetch2D = cuT( "texture2D" );
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
				m_strTexture1D = cuT( "texture1D" );
				m_strTexture2D = cuT( "texture2D" );
				m_strTexture3D = cuT( "texture3D" );
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
				m_strTexture2D = cuT( "texture" );
				m_strTexture3D = cuT( "texture" );
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
				m_strTexture2D = cuT( "texture" );
				m_strTexture3D = cuT( "texture" );
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
}
