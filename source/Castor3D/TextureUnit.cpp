#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/TextureUnit.hpp"
#include "Castor3D/Texture.hpp"
#include "Castor3D/RenderTarget.hpp"
#include "Castor3D/Sampler.hpp"
#include "Castor3D/Pipeline.hpp"

using namespace Castor3D;
using namespace Castor;

//*********************************************************************************************

bool UnitBinaryParser :: DoParse( TextureUnit & p_unit, stCHUNK & p_chunk )
{
	bool l_bReturn = true;
	p_unit.Cleanup();
	String l_strFile;
	eTEXTURE_MAP_MODE l_eMode;
	eALPHA_FUNC l_eAlphaFn;
	eALPHA_BLEND_FUNC l_eAlphaBlend;
	eRGB_BLEND_FUNC l_eRgbBlend;
	Colour l_colour;
	eTEXTURE_CHANNEL l_eChannel;
	String l_strName;
	ePIXEL_FORMAT l_ePf;
	Coords2ui l_size;
	eBLEND_SOURCE l_eSrc;

	while( p_chunk.m_uiIndex < p_chunk.m_pData.size() )
	{
		stCHUNK l_chunk;
		l_bReturn = DoGetSubChunk( p_chunk, l_chunk );

		if( l_bReturn )
		{
			switch( l_chunk.m_eChunkType )
			{
			case eCHUNK_TYPE_TEXTURE_FILE:
				l_bReturn = DoParseValue( l_strFile, l_chunk );
				if( l_bReturn )
				{
					p_unit.LoadTexture( l_strFile );
				}
				break;

			case eCHUNK_TYPE_TEXTURE_DATA:
				l_bReturn = DoParseValue( l_ePf, l_chunk );
				if( l_bReturn )
				{
					l_bReturn = DoParsePoint( l_size, l_chunk );
				}

				if( l_bReturn )
				{
					uint8_t const * l_pBuffer = &l_chunk.m_pData[l_chunk.m_uiIndex];
					PxBufferBaseSPtr l_pPxBuffer = PxBufferBase::create( Size( l_size[0], l_size[1] ), l_ePf, l_pBuffer, l_ePf );
					DynamicTextureSPtr l_pTexture = p_unit.GetRenderer()->GetRenderSystem()->CreateDynamicTexture();
					l_pTexture->SetImage( l_pPxBuffer );
					p_unit.SetTexture( l_pTexture );
				}
				break;

			case eCHUNK_TYPE_TEXTURE_MAP:
				l_bReturn = DoParseValue( l_eMode, l_chunk );
				if( l_bReturn )
				{
					p_unit.SetMappingMode( l_eMode );
				}
				break;

			case eCHUNK_TYPE_TEXTURE_ALPHA_FUNC:
				l_bReturn = DoParseValue( l_eAlphaFn, l_chunk );
				if( l_bReturn )
				{
					p_unit.SetAlphaFunc( l_eAlphaFn );
				}
				break;

			case eCHUNK_TYPE_TEXTURE_ALPHA_BLEND:
				l_bReturn = DoParseValue( l_eAlphaBlend, l_chunk );
				if( l_bReturn )
				{
					p_unit.SetAlpFunction( l_eAlphaBlend );
				}
				break;

			case eCHUNK_TYPE_TEXTURE_ALPHA_BLEND0:
				l_bReturn = DoParseValue( l_eSrc, l_chunk );
				if( l_bReturn )
				{
					p_unit.SetAlpArgument( eBLEND_SRC_INDEX_0, l_eSrc );
				}
				break;

			case eCHUNK_TYPE_TEXTURE_ALPHA_BLEND1:
				l_bReturn = DoParseValue( l_eSrc, l_chunk );
				if( l_bReturn )
				{
					p_unit.SetAlpArgument( eBLEND_SRC_INDEX_1, l_eSrc );
				}
				break;

			case eCHUNK_TYPE_TEXTURE_ALPHA_BLEND2:
				l_bReturn = DoParseValue( l_eSrc, l_chunk );
				if( l_bReturn )
				{
					p_unit.SetAlpArgument( eBLEND_SRC_INDEX_2, l_eSrc );
				}
				break;

			case eCHUNK_TYPE_TEXTURE_RGB_BLEND:
				l_bReturn = DoParseValue( l_eRgbBlend, l_chunk );
				if( l_bReturn )
				{
					p_unit.SetRgbFunction( l_eRgbBlend );
				}
				break;

			case eCHUNK_TYPE_TEXTURE_RGB_BLEND0:
				l_bReturn = DoParseValue( l_eSrc, l_chunk );
				if( l_bReturn )
				{
					p_unit.SetRgbArgument( eBLEND_SRC_INDEX_0, l_eSrc );
				}
				break;

			case eCHUNK_TYPE_TEXTURE_RGB_BLEND1:
				l_bReturn = DoParseValue( l_eSrc, l_chunk );
				if( l_bReturn )
				{
					p_unit.SetRgbArgument( eBLEND_SRC_INDEX_1, l_eSrc );
				}
				break;

			case eCHUNK_TYPE_TEXTURE_RGB_BLEND2:
				l_bReturn = DoParseValue( l_eSrc, l_chunk );
				if( l_bReturn )
				{
					p_unit.SetRgbArgument( eBLEND_SRC_INDEX_2, l_eSrc );
				}
				break;

			case eCHUNK_TYPE_TEXTURE_COLOUR:
				l_bReturn = DoParseColour( l_colour, l_chunk );
				if( l_bReturn )
				{
					p_unit.SetBlendColour( l_colour );
				}
				break;

			case eCHUNK_TYPE_TEXTURE_CHANNEL:
				l_bReturn = DoParseValue( l_eChannel, l_chunk );
				if( l_bReturn )
				{
					p_unit.SetChannel( l_eChannel );
				}
				break;
			}
		}

		if( !l_bReturn )
		{
			p_chunk.m_uiIndex = uint32_t( p_chunk.m_pData.size() );
			l_bReturn = false;
		}
	}

	return l_bReturn;
}

bool UnitBinaryParser :: DoFill( TextureUnit const & p_unit, stCHUNK & p_chunk )
{
	bool l_bReturn = true;
	stCHUNK l_chunk;
	l_chunk.m_eChunkType = eCHUNK_TYPE_PASS_TEXTURE;
	TextureBaseSPtr l_pTexture = p_unit.GetTexture();

	if( l_bReturn )
	{
		String l_path = p_unit.GetTexturePath();

		if( !l_path.empty() )
		{
			l_bReturn = DoFillStringChunk( l_path, eCHUNK_TYPE_TEXTURE_FILE, l_chunk );
		}
		else if( l_pTexture )
		{
			stCHUNK l_chunkData;
			l_chunkData.m_eChunkType = eCHUNK_TYPE_TEXTURE_DATA;
			uint16_t l_uiPf = uint16_t( l_pTexture->GetPixelFormat() );
			uint32_t l_uiSize = sizeof( uint16_t ) + sizeof( uint32_t ) * 2 + uint32_t( l_pTexture->GetBuffer()->size() );
			l_chunkData.m_pData.reserve( l_uiSize );
			uint8_t const * l_pBuffer = reinterpret_cast< uint8_t const * >( &l_uiSize );
			l_chunkData.m_pData.insert( l_chunkData.m_pData.end(), l_pBuffer, l_pBuffer + sizeof( uint32_t ) );
			l_pBuffer = reinterpret_cast< uint8_t const * >( &l_uiPf );
			l_chunkData.m_pData.insert( l_chunkData.m_pData.end(), l_pBuffer, l_pBuffer + sizeof( uint16_t ) );
			l_pBuffer = reinterpret_cast< uint8_t const * >( l_pTexture->GetBuffer()->dimensions().const_ptr() );
			l_chunkData.m_pData.insert( l_chunkData.m_pData.end(), l_pBuffer, l_pBuffer + sizeof( uint32_t ) * 2 );
			l_pBuffer = p_unit.GetImageBuffer();
			l_chunkData.m_pData.insert( l_chunkData.m_pData.end(), l_pBuffer, l_pBuffer + l_pTexture->GetBuffer()->size() );
			l_bReturn = true;
		}
	}

	if( l_bReturn )
	{
		l_bReturn = DoFillValueChunk( p_unit.GetMappingMode(), eCHUNK_TYPE_TEXTURE_MAP, l_chunk );
	}

	if( l_bReturn )
	{
		l_bReturn = DoFillValueChunk( p_unit.GetAlphaFunc(), eCHUNK_TYPE_TEXTURE_ALPHA_FUNC, l_chunk );
	}

	if( l_bReturn )
	{
		l_bReturn = DoFillValueChunk( p_unit.GetAlpFunction(), eCHUNK_TYPE_TEXTURE_ALPHA_BLEND, l_chunk );
	}

	if( l_bReturn )
	{
		l_bReturn = DoFillValueChunk( p_unit.GetAlpArgument( eBLEND_SRC_INDEX_0 ), eCHUNK_TYPE_TEXTURE_ALPHA_BLEND0, l_chunk );
	}

	if( l_bReturn )
	{
		l_bReturn = DoFillValueChunk( p_unit.GetAlpArgument( eBLEND_SRC_INDEX_1 ), eCHUNK_TYPE_TEXTURE_ALPHA_BLEND1, l_chunk );
	}

	if( l_bReturn )
	{
		l_bReturn = DoFillValueChunk( p_unit.GetAlpArgument( eBLEND_SRC_INDEX_2 ), eCHUNK_TYPE_TEXTURE_ALPHA_BLEND2, l_chunk );
	}

	if( l_bReturn )
	{
		l_bReturn = DoFillValueChunk( p_unit.GetRgbFunction(), eCHUNK_TYPE_TEXTURE_RGB_BLEND, l_chunk );
	}

	if( l_bReturn )
	{
		l_bReturn = DoFillValueChunk( p_unit.GetRgbArgument( eBLEND_SRC_INDEX_0 ), eCHUNK_TYPE_TEXTURE_MAP, l_chunk );
	}

	if( l_bReturn )
	{
		l_bReturn = DoFillValueChunk( p_unit.GetRgbArgument( eBLEND_SRC_INDEX_1 ), eCHUNK_TYPE_TEXTURE_MAP, l_chunk );
	}

	if( l_bReturn )
	{
		l_bReturn = DoFillValueChunk( p_unit.GetRgbArgument( eBLEND_SRC_INDEX_2 ), eCHUNK_TYPE_TEXTURE_MAP, l_chunk );
	}

	if( l_bReturn )
	{
		l_bReturn = DoFillValueChunk( p_unit.GetBlendColour(), eCHUNK_TYPE_TEXTURE_COLOUR, l_chunk );
	}

	if( l_bReturn )
	{
		l_bReturn = DoFillValueChunk( p_unit.GetChannel(), eCHUNK_TYPE_TEXTURE_CHANNEL, l_chunk );
	}

	if( l_bReturn )
	{
		l_bReturn = DoAddSubChunk( l_chunk, p_chunk );
	}

	return l_bReturn;
}

//*************************************************************************************************

TextureUnit::TextLoader :: TextLoader( File::eENCODING_MODE p_eEncodingMode )
	:	Loader< TextureUnit, eFILE_TYPE_TEXT, TextFile >( File::eOPEN_MODE_DUMMY, p_eEncodingMode )
{
}

bool TextureUnit::TextLoader :: operator ()( TextureUnit const & p_unit, TextFile & p_file)
{
	static const String l_strTextureArguments		[eBLEND_SOURCE_COUNT		]	= { cuT( "diffuse"	),	cuT( "texture"		),	cuT( "previous"			),	cuT( "constant"		) };
	static const String l_strTextureAlphaFunctions	[eALPHA_BLEND_FUNC_COUNT	]	= { cuT( "none"		),	cuT( "first_arg"	),	cuT( "add"				),	cuT( "add_signed"	),	cuT( "modulate"		),	cuT( "interpolate"		),	cuT( "substract"	) };
	static const String l_strAlphaFuncs				[eALPHA_FUNC_COUNT			]	= { cuT( "always"	),	cuT( "less"			),	cuT( "less_or_equal"	),	cuT( "equal"		),	cuT( "not_equal"	),	cuT( "greater_or_equal"	),	cuT( "greater"		),	cuT( "never"	) };
	static const String l_strTextureRgbFunctions	[eRGB_BLEND_FUNC_COUNT		]	= { cuT( "none"		),	cuT( "first_arg"	),	cuT( "add"				),	cuT( "add_signed"	),	cuT( "modulate"		),	cuT( "interpolate"		),	cuT( "substract"	),	cuT( "dot3_rgb"	),	cuT( "dot3_rgba"	) };
	static const String l_strFormats				[ePIXEL_FORMAT_COUNT		]	= { cuT( "l4"		),	cuT( "a4l4"			),	cuT( "l8"				),	cuT( "a8l8"			),	cuT( "a1r5g5b5"		),	cuT( "a4r4g4b4"			),	cuT( "r8g8b8"		),	cuT( "a8r8g8b8"	),	cuT( "dxtc1"		),	cuT( "dxtc3"	),	cuT( "dxtc5"	),	cuT( "yuy2"	),	cuT( "depth16"	),	cuT( "depth24" ),	cuT( "depth32" ) };
	bool l_bReturn = true;

	if (p_unit.IsTextured())
	{
		if (l_bReturn)
		{
			l_bReturn = p_file.WriteText( cuT( "\t\ttexture_unit\n\t\t{\n")) > 0;
		}

		if (l_bReturn)
		{
			l_bReturn = p_file.WriteText( cuT( "\t\t\tformat ") + l_strFormats[p_unit.GetPixelFormat()] + cuT( "\n")) > 0;
		}

		if (l_bReturn)
		{
			l_bReturn = p_file.Print( 256, cuT( "\t\t\tcolour ")) > 0;
		}

		if (l_bReturn)
		{
			l_bReturn = Colour::TextLoader()( p_unit.GetBlendColour(), p_file);
		}

		if (l_bReturn)
		{
			l_bReturn = p_file.Print( 256, cuT( "\n\t\t\tmap_type %i\n"), p_unit.GetMappingMode()) > 0;
		}

		Path l_path = p_unit.GetTexturePath();
		l_bReturn = p_file.WriteText( cuT( "\t\t\timage ") + l_path + cuT( "\n")) > 0;

		if (l_bReturn)
		{
			l_bReturn = p_file.WriteText( cuT( "\t\t\talpha_func ") + l_strAlphaFuncs[p_unit.GetAlphaFunc()] + cuT( " " ) + str_utils::to_string( p_unit.GetAlphaValue() ) + cuT( "\n" ) ) > 0;
		}

		if (l_bReturn && p_unit.GetRgbFunction() != eRGB_BLEND_FUNC_NONE)
		{
			l_bReturn = p_file.WriteText( cuT( "\t\t\trgb_blend ") + l_strTextureRgbFunctions[p_unit.GetRgbFunction()] + cuT( " " ) + l_strTextureArguments[p_unit.GetRgbArgument( eBLEND_SRC_INDEX_0)] + cuT( " " ) + l_strTextureArguments[p_unit.GetRgbArgument( eBLEND_SRC_INDEX_1)] + cuT( "\n")) > 0;
		}

		if (l_bReturn && p_unit.GetAlpFunction() != eALPHA_BLEND_FUNC_NONE)
		{
			l_bReturn = p_file.WriteText( cuT( "\t\t\talpha_blend ") + l_strTextureAlphaFunctions[p_unit.GetAlpFunction()] + cuT( " " ) + l_strTextureArguments[p_unit.GetAlpArgument( eBLEND_SRC_INDEX_0)] + cuT( " ") + l_strTextureArguments[p_unit.GetAlpArgument( eBLEND_SRC_INDEX_1)] + cuT( "\n")) > 0;
		}

		if (l_bReturn)
		{
			l_bReturn = p_file.WriteText( cuT( "\t\t}\n")) > 0;
		}
	}

	return l_bReturn;
}

//*********************************************************************************************

TextureUnit :: TextureUnit( Engine * p_pEngine )
	:	Renderable< TextureUnit, TextureRenderer >	( p_pEngine							)
	,	m_uiIndex									( 0									)
	,	m_clrBlend									( Colour::from_rgba( 0xFFFFFFFF )	)
	,	m_eChannel									( eTEXTURE_CHANNEL_DIFFUSE			)
	,	m_eAlphaFunc								( eALPHA_FUNC_ALWAYS				)
	,	m_fAlphaValue								( 0									)
	,	m_eRgbFunction								( eRGB_BLEND_FUNC_NONE				)
	,	m_eAlpFunction								( eALPHA_BLEND_FUNC_NONE			)
	,	m_bAutoMipmaps								( false								)
	,	m_bChanged									( false								)
{
	m_mtxTransformations.set_identity();
	m_eRgbArguments[0] = eBLEND_SOURCE_COUNT;
	m_eRgbArguments[1] = eBLEND_SOURCE_COUNT;
	m_eAlpArguments[0] = eBLEND_SOURCE_COUNT;
	m_eAlpArguments[1] = eBLEND_SOURCE_COUNT;

	m_pSampler = m_pEngine->GetDefaultSampler();
}

TextureUnit :: ~TextureUnit()
{
	if( !m_pRenderTarget.expired() )
	{
		m_pEngine->RemoveRenderTarget( std::move( m_pRenderTarget.lock() ) );
	}
}

void TextureUnit :: SetTexture( TextureBaseSPtr p_pTexture )
{
	m_pTexture = p_pTexture;
	m_bChanged = true;
}

void TextureUnit :: Initialise()
{
	RenderTargetSPtr l_pTarget = m_pRenderTarget.lock();
	SamplerSPtr l_pSampler = m_pSampler.lock();

	if( l_pTarget )
	{
		l_pTarget->Initialise();
		m_pTexture = l_pTarget->GetTexture();

		if( l_pSampler && m_pTexture )
		{
			m_pTexture->SetSampler( l_pSampler );
		}
	}
	else if( m_pTexture )
	{
		m_pTexture->SetSampler( l_pSampler );
		m_pTexture->Create();
		m_pTexture->Initialise( m_uiIndex );
	}
}

void TextureUnit :: Cleanup()
{
	if( m_pTexture )
	{
		m_pTexture->Cleanup();
//		m_pTexture.reset();
	}

	m_clrBlend = Colour();
	RenderTargetSPtr l_pTarget = m_pRenderTarget.lock();

	if( l_pTarget )
	{
		l_pTarget->Cleanup();
	}

	m_fAlphaValue = 1.0f;
}

void TextureUnit :: Render()
{
// 	if( m_pRenderTarget )
// 	{
// 		m_pRenderTarget->Render( 0.0 );
// 	}

	if( m_pTexture && m_pTexture->IsInitialised() )
	{
		TextureRendererSPtr l_pRenderer = GetRenderer();
		Pipeline * l_pPipeline = l_pRenderer->GetRenderSystem()->GetPipeline();

		m_pTexture->Bind();

		if( m_bChanged && m_bAutoMipmaps )
		{
			m_pTexture->GenerateMipmaps();
			m_bChanged = false;
		}

		l_pRenderer->Render();
		m_ePrevMtxMode = l_pPipeline->MatrixMode( eMTXMODE( eMTXMODE_TEXTURE0 + GetIndex() ) );
		l_pPipeline->LoadIdentity();
		l_pPipeline->MultMatrix( m_mtxTransformations );
		l_pRenderer->GetRenderSystem()->GetPipeline()->MatrixMode( m_ePrevMtxMode );
	}
}

void TextureUnit :: EndRender()
{
	if( m_pTexture && m_pTexture->IsInitialised() )
	{
		m_pTexture->Unbind();
		GetRenderer()->EndRender();
	}
}

uint8_t const * TextureUnit :: GetImageBuffer()const
{
	uint8_t const * l_pReturn = nullptr;

	if( m_pTexture && m_pTexture->GetBuffer() )
	{
		l_pReturn = m_pTexture->GetBuffer()->const_ptr();
	}

	return l_pReturn;
}

uint8_t * TextureUnit :: GetImageBuffer()
{
	uint8_t * l_pReturn = nullptr;

	if( m_pTexture && m_pTexture->GetBuffer() )
	{
		l_pReturn = m_pTexture->GetBuffer()->ptr();
	}

	return l_pReturn;
}

PxBufferBaseSPtr const TextureUnit :: GetImagePixels()const
{
	if( m_pTexture )
	{
		return m_pTexture->GetBuffer();
	}

	return nullptr;
}

PxBufferBaseSPtr TextureUnit :: GetImagePixels()
{
	PxBufferBaseSPtr l_pReturn;

	if( m_pTexture )
	{
		l_pReturn = m_pTexture->GetBuffer();
	}

	return l_pReturn;
}

uint32_t TextureUnit :: GetImageSize()const
{
	uint32_t l_uiReturn = 0;

	if( m_pTexture )
	{
		l_uiReturn = GetWidth() * GetHeight() * PF::GetBytesPerPixel( GetPixelFormat() );
	}

	return l_uiReturn;
}

uint32_t TextureUnit :: GetWidth()const
{
	uint32_t l_uiReturn = 0;

	if( m_pTexture )
	{
		l_uiReturn = m_pTexture->GetWidth();
	}

	return l_uiReturn;
}

uint32_t TextureUnit :: GetHeight()const
{
	uint32_t l_uiReturn = 0;

	if( m_pTexture )
	{
		l_uiReturn = m_pTexture->GetHeight();
	}

	return l_uiReturn;
}

ePIXEL_FORMAT TextureUnit :: GetPixelFormat()const
{
	ePIXEL_FORMAT l_pfReturn = ePIXEL_FORMAT_COUNT;

	if( m_pTexture )
	{
		l_pfReturn = m_pTexture->GetPixelFormat();
	}

	return l_pfReturn;
}

void TextureUnit :: UploadImage( bool CU_PARAM_UNUSED( p_bSync ) )
{
	if( m_pTexture )
	{
		uint8_t * l_pImage = m_pTexture->Lock( eLOCK_FLAG_WRITE_ONLY );

		if( l_pImage && m_pTexture->GetBuffer() )
		{
			memcpy( l_pImage, m_pTexture->GetBuffer()->const_ptr(), m_pTexture->GetBuffer()->size() );
		}

		m_pTexture->Unlock( true );
	}
}

void TextureUnit :: DownloadImage( bool CU_PARAM_UNUSED( p_bSync ) )
{
	if( m_pTexture )
	{
		uint8_t * l_pImage = m_pTexture->Lock( eLOCK_FLAG_WRITE_ONLY );

		if( l_pImage && m_pTexture->GetBuffer() )
		{
			memcpy( m_pTexture->GetBuffer()->ptr(), l_pImage, m_pTexture->GetBuffer()->size() );
		}

		m_pTexture->Unlock( true );
	}
}

bool TextureUnit :: LoadTexture( Path const & p_pathFile )
{
	bool		l_bReturn = false;
	ImageSPtr	l_pImage;

	if( !p_pathFile.empty() )
	{
		l_pImage = m_pEngine->GetImageManager().find( p_pathFile.GetFileName() );

		if( !l_pImage && File::FileExists( p_pathFile ) )
		{
			l_pImage = std::make_shared< Image >( p_pathFile.GetFileName(), p_pathFile );
			m_pEngine->GetImageManager().insert( p_pathFile.GetFileName(), l_pImage );
		}
	}

	if( l_pImage )
	{
		StaticTextureSPtr l_pStaTexture = m_pEngine->GetRenderSystem()->CreateStaticTexture();
		l_pStaTexture->SetDimension( eTEXTURE_DIMENSION_2D );
		l_pStaTexture->SetImage( l_pImage->GetPixels() );
		SetTexture( l_pStaTexture );
		m_pathTexture = p_pathFile;
		m_bChanged = true;
		l_bReturn = true;
	}

	return l_bReturn;
}

bool TextureUnit :: IsTextureInitialised()const
{
	return m_pTexture && m_pTexture->IsInitialised();
}

eTEXTURE_DIMENSION TextureUnit :: GetDimension()const
{
	return m_pTexture->GetDimension();
}

eTEXTURE_MAP_MODE TextureUnit :: GetMappingMode()const
{
	return m_pTexture->GetMappingMode();
}

void TextureUnit :: SetMappingMode( eTEXTURE_MAP_MODE p_eMode )
{
	return m_pTexture->SetMappingMode( p_eMode );
}

//*********************************************************************************************

TextureRenderer :: TextureRenderer( RenderSystem * p_pRenderSystem )
	:	Renderer< TextureUnit, TextureRenderer >	( p_pRenderSystem	)
{
}

TextureRenderer :: ~TextureRenderer()
{
}

//*********************************************************************************************
