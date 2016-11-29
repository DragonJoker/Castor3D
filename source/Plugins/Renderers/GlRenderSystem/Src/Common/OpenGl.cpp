#include "Common/OpenGl.hpp"

#include "Render/GlRenderSystem.hpp"

#include <Graphics/Rectangle.hpp>
#include <Log/Logger.hpp>
#include <Miscellaneous/Utils.hpp>

#include <GlslWriter.hpp>

#include <iomanip>

using namespace Castor3D;
using namespace Castor;

#ifndef GL_INVALID_FRAMEBUFFER_OPERATION
#	define GL_INVALID_FRAMEBUFFER_OPERATION 0x506
#endif

#define DEF_USE_DIRECT_STATE_ACCESS 0

namespace GlRender
{
	template< typename T >
	inline void GetFunction( T & p_function, String const & p_name, String const & p_extension )
	{
		if ( !gl_api::GetFunction( p_name, p_function ) )
		{
			if ( !gl_api::GetFunction( p_name + p_extension, p_function ) )
			{
				Logger::LogWarning( cuT( "Unable to retrieve function " ) + p_name );
			}
		}
	}

	//*************************************************************************************************

	TexFunctionsBase::TexFunctionsBase( OpenGl & p_gl )
		: Holder( p_gl )
	{
	}

	//*************************************************************************************************

	TexFunctions::TexFunctions( OpenGl & p_gl )
		: TexFunctionsBase( p_gl )
	{
		m_pfnBindTexture = &glBindTexture;
		m_pfnTexSubImage1D = &glTexSubImage1D;
		m_pfnTexSubImage2D = &glTexSubImage2D;
		m_pfnTexImage1D = &glTexImage1D;
		m_pfnTexImage2D = &glTexImage2D;
		m_pfnGetTexImage = &glGetTexImage;
		GetFunction( m_pfnTexImage3D, cuT( "glTexImage3D" ), cuT( "EXT" ) );
		GetFunction( m_pfnTexSubImage3D, cuT( "glTexSubImage3D" ), cuT( "EXT" ) );
		GetFunction( m_pfnGenerateMipmap, cuT( "glGenerateMipmap" ), cuT( "EXT" ) );
	}

	//*************************************************************************************************

	TexFunctionsDSA::TexFunctionsDSA( OpenGl & p_gl )
		: TexFunctionsBase( p_gl )
		, m_uiTexture( 0 )
		, m_pfnTextureSubImage1D()
		, m_pfnTextureSubImage2D()
		, m_pfnTextureSubImage3D()
		, m_pfnTextureImage1D()
		, m_pfnTextureImage2D()
		, m_pfnTextureImage3D()
	{
		GetFunction( m_pfnTextureSubImage1D, cuT( "glTextureSubImage1D" ), cuT( "EXT" ) );
		GetFunction( m_pfnTextureSubImage2D, cuT( "glTextureSubImage2D" ), cuT( "EXT" ) );
		GetFunction( m_pfnTextureSubImage3D, cuT( "glTextureSubImage3D" ), cuT( "EXT" ) );
		GetFunction( m_pfnTextureImage1D, cuT( "glTextureImage1D" ), cuT( "EXT" ) );
		GetFunction( m_pfnTextureImage2D, cuT( "glTextureImage2D" ), cuT( "EXT" ) );
		GetFunction( m_pfnTextureImage3D, cuT( "glTextureImage3D" ), cuT( "EXT" ) );
		GetFunction( m_pfnGetTextureImage, cuT( "glGetTextureImage" ), cuT( "EXT" ) );
		GetFunction( m_pfnGenerateTextureMipmap, cuT( "glGenerateTextureMipmap" ), cuT( "EXT" ) );
	}

	//*************************************************************************************************

	BufFunctionsBase::BufFunctionsBase( OpenGl & p_gl )
		: Holder( p_gl )
	{
		if ( p_gl.HasExtension( NV_shader_buffer_load ) )
		{
			GetFunction( m_pfnMakeBufferResident, cuT( "glMakeBufferResident" ), cuT( "NV" ) );
			GetFunction( m_pfnMakeBufferNonResident, cuT( "glMakeBufferNonResident" ), cuT( "NV" ) );
			GetFunction( m_pfnIsBufferResident, cuT( "glIsBufferResident" ), cuT( "NV" ) );
			GetFunction( m_pfnMakeNamedBufferResident, cuT( "glMakeNamedBufferResident" ), cuT( "NV" ) );
			GetFunction( m_pfnMakeNamedBufferNonResident, cuT( "glMakeNamedBufferNonResident" ), cuT( "NV" ) );
			GetFunction( m_pfnIsNamedBufferResident, cuT( "glIsNamedBufferResident" ), cuT( "NV" ) );
			GetFunction( m_pfnGetBufferParameterui64v, cuT( "glGetBufferParameterui64v" ), cuT( "NV" ) );
			GetFunction( m_pfnGetNamedBufferParameterui64v, cuT( "glGetNamedBufferParameterui64v" ), cuT( "NV" ) );
		}

		if ( p_gl.HasExtension( NV_vertex_buffer_unified_memory ) )
		{
			GetFunction( m_pfnBufferAddressRange, cuT( "glBufferAddressRange" ), cuT( "NV" ) );
			GetFunction( m_pfnVertexFormat, cuT( "glVertexFormat" ), cuT( "NV" ) );
			GetFunction( m_pfnNormalFormat, cuT( "glNormalFormat" ), cuT( "NV" ) );
			GetFunction( m_pfnColorFormat, cuT( "glColorFormat" ), cuT( "NV" ) );
			GetFunction( m_pfnIndexFormat, cuT( "glIndexFormat" ), cuT( "NV" ) );
			GetFunction( m_pfnTexCoordFormat, cuT( "glTexCoordFormat" ), cuT( "NV" ) );
			GetFunction( m_pfnEdgeFlagFormat, cuT( "glEdgeFlagFormat" ), cuT( "NV" ) );
			GetFunction( m_pfnSecondaryColorFormat, cuT( "glSecondaryColorFormat" ), cuT( "NV" ) );
			GetFunction( m_pfnFogCoordFormat, cuT( "glFogCoordFormat" ), cuT( "NV" ) );
			GetFunction( m_pfnVertexAttribFormat, cuT( "glVertexAttribFormat" ), cuT( "NV" ) );
			GetFunction( m_pfnVertexAttribIFormat, cuT( "glVertexAttribIFormat" ), cuT( "NV" ) );
		}
	}

	//*************************************************************************************************

	BufFunctions::BufFunctions( OpenGl & p_gl )
		: BufFunctionsBase( p_gl )
	{
		GetFunction( m_pfnBindBuffer, cuT( "glBindBuffer" ), cuT( "ARB" ) );
		GetFunction( m_pfnBufferData, cuT( "glBufferData" ), cuT( "ARB" ) );
		GetFunction( m_pfnBufferSubData, cuT( "glBufferSubData" ), cuT( "ARB" ) );
		GetFunction( m_pfnCopyBufferSubData, cuT( "glCopyBufferSubData" ), cuT( "ARB" ) );
		GetFunction( m_pfnGetBufferParameteriv, cuT( "glGetBufferParameteriv" ), cuT( "ARB" ) );
		GetFunction( m_pfnMapBuffer, cuT( "glMapBuffer" ), cuT( "ARB" ) );
		GetFunction( m_pfnUnmapBuffer, cuT( "glUnmapBuffer" ), cuT( "ARB" ) );
		GetFunction( m_pfnMapBufferRange, cuT( "glMapBufferRange" ), cuT( "" ) );
		GetFunction( m_pfnFlushMappedBufferRange, cuT( "glFlushMappedBufferRange" ), cuT( "" ) );
	}

	//*************************************************************************************************

	BufFunctionsDSA::BufFunctionsDSA( OpenGl & p_gl )
		: BufFunctionsBase( p_gl )
		, m_uiBuffer( 0 )
	{
		GetFunction( m_pfnNamedBufferData, cuT( "glNamedBufferData" ), cuT( "EXT" ) );
		GetFunction( m_pfnNamedBufferSubData, cuT( "glNamedBufferSubData" ), cuT( "EXT" ) );
		GetFunction( m_pfnCopyNamedBufferSubData, cuT( "glCopyNamedBufferSubData" ), cuT( "ARB" ) );
		GetFunction( m_pfnGetNamedBufferParameteriv, cuT( "glGetNamedBufferParameteriv" ), cuT( "EXT" ) );
		GetFunction( m_pfnMapNamedBuffer, cuT( "glMapNamedBuffer" ), cuT( "EXT" ) );
		GetFunction( m_pfnUnmapNamedBuffer, cuT( "glUnmapNamedBuffer" ), cuT( "EXT" ) );
		GetFunction( m_pfnMapNamedBufferRange, cuT( "glMapNamedBufferRange" ), cuT( "EXT" ) );
		GetFunction( m_pfnFlushMappedNamedBufferRange, cuT( "glFlushMappedNamedBufferRange" ), cuT( "EXT" ) );
	}

	//*************************************************************************************************

#define CASTOR_DBG_WIN32 0

	OpenGl::OpenGl( GlRenderSystem & p_renderSystem )
		: m_pfnReadPixels()
		, m_pfnBlitFramebuffer()
		, m_pfnTexImage2DMultisample()
		, m_pfnGetActiveUniform()
		, m_pfnVertexAttribPointer()
		, m_renderSystem( p_renderSystem )
	{
		uint32_t l_index = 0;
		GlslStrings[l_index++] = cuT( "[e00] GLSL is not available!" );
		GlslStrings[l_index++] = cuT( "[e01] Not a valid program object!" );
		GlslStrings[l_index++] = cuT( "[e02] Not a valid object!" );
		GlslStrings[l_index++] = cuT( "[e03] Out of memory!" );
		GlslStrings[l_index++] = cuT( "[e04] Unknown compiler error!" );
		GlslStrings[l_index++] = cuT( "[e05] Linker log is not available!" );
		GlslStrings[l_index++] = cuT( "[e06] Compiler log is not available!" );
		GlslStrings[l_index] = cuT( "[Empty]" );

		PrimitiveTypes[uint32_t( Castor3D::Topology::ePoints )] = GlTopology::ePoints;
		PrimitiveTypes[uint32_t( Castor3D::Topology::eLines )] = GlTopology::eLines;
		PrimitiveTypes[uint32_t( Castor3D::Topology::eLineLoop )] = GlTopology::eLineLoop;
		PrimitiveTypes[uint32_t( Castor3D::Topology::eLineStrip )] = GlTopology::eLineStrip;
		PrimitiveTypes[uint32_t( Castor3D::Topology::eTriangles )] = GlTopology::eTriangles;
		PrimitiveTypes[uint32_t( Castor3D::Topology::eTriangleStrips )] = GlTopology::eTriangleStrip;
		PrimitiveTypes[uint32_t( Castor3D::Topology::eTriangleFan )] = GlTopology::eTriangleFan;
		PrimitiveTypes[uint32_t( Castor3D::Topology::eQuads )] = GlTopology::eQuads;
		PrimitiveTypes[uint32_t( Castor3D::Topology::eQuadStrips )] = GlTopology::eQuadStrip;
		PrimitiveTypes[uint32_t( Castor3D::Topology::ePolygon )] = GlTopology::ePolygon;

		TextureDimensions[uint32_t( Castor3D::TextureType::eBuffer )] = GlTexDim::eBuffer;
		TextureDimensions[uint32_t( Castor3D::TextureType::eOneDimension )] = GlTexDim::e1D;
		TextureDimensions[uint32_t( Castor3D::TextureType::eOneDimensionArray )] = GlTexDim::e1DArray;
		TextureDimensions[uint32_t( Castor3D::TextureType::eTwoDimensions )] = GlTexDim::e2D;
		TextureDimensions[uint32_t( Castor3D::TextureType::eTwoDimensionsArray )] = GlTexDim::e2DArray;
		TextureDimensions[uint32_t( Castor3D::TextureType::eTwoDimensionsMS )] = GlTexDim::e2DMS;
		TextureDimensions[uint32_t( Castor3D::TextureType::eTwoDimensionsMSArray )] = GlTexDim::e2DMSArray;
		TextureDimensions[uint32_t( Castor3D::TextureType::eThreeDimensions )] = GlTexDim::e3D;
		TextureDimensions[uint32_t( Castor3D::TextureType::eCube )] = GlTexDim::eCube;
		TextureDimensions[uint32_t( Castor3D::TextureType::eCubeArray )] = GlTexDim::eCubeArray;

		AlphaFuncs[uint32_t( Castor3D::ComparisonFunc::eAlways )] = GlComparator::eAlways;
		AlphaFuncs[uint32_t( Castor3D::ComparisonFunc::eLess )] = GlComparator::eLess;
		AlphaFuncs[uint32_t( Castor3D::ComparisonFunc::eLEqual )] = GlComparator::eLEqual;
		AlphaFuncs[uint32_t( Castor3D::ComparisonFunc::eEqual )] = GlComparator::eEqual;
		AlphaFuncs[uint32_t( Castor3D::ComparisonFunc::eNEqual )] = GlComparator::eNEqual;
		AlphaFuncs[uint32_t( Castor3D::ComparisonFunc::eGEqual )] = GlComparator::eGEqual;
		AlphaFuncs[uint32_t( Castor3D::ComparisonFunc::eGreater )] = GlComparator::eGreater;
		AlphaFuncs[uint32_t( Castor3D::ComparisonFunc::eNever )] = GlComparator::eNever;

		TextureWrapMode[uint32_t( Castor3D::WrapMode::eRepeat )] = GlWrapMode::eRepeat;
		TextureWrapMode[uint32_t( Castor3D::WrapMode::eMirroredRepeat )] = GlWrapMode::eMirroredRepeat;
		TextureWrapMode[uint32_t( Castor3D::WrapMode::eClampToBorder )] = GlWrapMode::eClampToBorder;
		TextureWrapMode[uint32_t( Castor3D::WrapMode::eClampToEdge )] = GlWrapMode::eClampToEdge;

		TextureInterpolation[uint32_t( Castor3D::InterpolationMode::eUndefined )] = GlInterpolationMode::eNearest;
		TextureInterpolation[uint32_t( Castor3D::InterpolationMode::eNearest )] = GlInterpolationMode::eNearest;
		TextureInterpolation[uint32_t( Castor3D::InterpolationMode::eLinear )] = GlInterpolationMode::eLinear;

		BlendFactors[uint32_t( Castor3D::BlendOperand::eZero )] = GlBlendFactor::eZero;
		BlendFactors[uint32_t( Castor3D::BlendOperand::eOne )] = GlBlendFactor::eOne;
		BlendFactors[uint32_t( Castor3D::BlendOperand::eSrcColour )] = GlBlendFactor::eSrcColour;
		BlendFactors[uint32_t( Castor3D::BlendOperand::eInvSrcColour )] = GlBlendFactor::eInvSrcColour;
		BlendFactors[uint32_t( Castor3D::BlendOperand::eDstColour )] = GlBlendFactor::eDstColour;
		BlendFactors[uint32_t( Castor3D::BlendOperand::eInvDstColour )] = GlBlendFactor::eInvDstColour;
		BlendFactors[uint32_t( Castor3D::BlendOperand::eSrcAlpha )] = GlBlendFactor::eSrcAlpha;
		BlendFactors[uint32_t( Castor3D::BlendOperand::eInvSrcAlpha )] = GlBlendFactor::eInvSrcAlpha;
		BlendFactors[uint32_t( Castor3D::BlendOperand::eDstAlpha )] = GlBlendFactor::eDstAlpha;
		BlendFactors[uint32_t( Castor3D::BlendOperand::eInvDstAlpha )] = GlBlendFactor::eInvDstAlpha;
		BlendFactors[uint32_t( Castor3D::BlendOperand::eConstant )] = GlBlendFactor::eConstant;
		BlendFactors[uint32_t( Castor3D::BlendOperand::eInvConstant )] = GlBlendFactor::eInvConstant;
		BlendFactors[uint32_t( Castor3D::BlendOperand::eSrcAlphaSaturate )] = GlBlendFactor::eSrcAlphaSaturate;
		BlendFactors[uint32_t( Castor3D::BlendOperand::eSrc1Colour )] = GlBlendFactor::eSrc1Colour;
		BlendFactors[uint32_t( Castor3D::BlendOperand::eInvSrc1Colour )] = GlBlendFactor::eInvSrc1Colour;
		BlendFactors[uint32_t( Castor3D::BlendOperand::eSrc1Alpha )] = GlBlendFactor::eSrc1Alpha;
		BlendFactors[uint32_t( Castor3D::BlendOperand::eInvSrc1Alpha )] = GlBlendFactor::eInvSrc1Alpha;

		TextureArguments[uint32_t( Castor3D::BlendSource::eTexture )] = GlBlendSource::eTexture;
		TextureArguments[uint32_t( Castor3D::BlendSource::eTexture0 )] = GlBlendSource::eTexture0;
		TextureArguments[uint32_t( Castor3D::BlendSource::eTexture1 )] = GlBlendSource::eTexture1;
		TextureArguments[uint32_t( Castor3D::BlendSource::eTexture2 )] = GlBlendSource::eTexture2;
		TextureArguments[uint32_t( Castor3D::BlendSource::eTexture3 )] = GlBlendSource::eTexture3;
		TextureArguments[uint32_t( Castor3D::BlendSource::eConstant )] = GlBlendSource::eConstant;
		TextureArguments[uint32_t( Castor3D::BlendSource::eDiffuse )] = GlBlendSource::eColour;
		TextureArguments[uint32_t( Castor3D::BlendSource::ePrevious )] = GlBlendSource::ePrevious;

		RgbBlendFuncs[uint32_t( Castor3D::ColourBlendFunc::eNoBlend )] = GlBlendFunc::eModulate;
		RgbBlendFuncs[uint32_t( Castor3D::ColourBlendFunc::eFirstArg )] = GlBlendFunc::eReplace;
		RgbBlendFuncs[uint32_t( Castor3D::ColourBlendFunc::eAdd )] = GlBlendFunc::eAdd;
		RgbBlendFuncs[uint32_t( Castor3D::ColourBlendFunc::eAddSigned )] = GlBlendFunc::eAddSigned;
		RgbBlendFuncs[uint32_t( Castor3D::ColourBlendFunc::eModulate )] = GlBlendFunc::eModulate;
		RgbBlendFuncs[uint32_t( Castor3D::ColourBlendFunc::eInterpolate )] = GlBlendFunc::eInterpolate;
		RgbBlendFuncs[uint32_t( Castor3D::ColourBlendFunc::eSubtract )] = GlBlendFunc::eSubtract;
		RgbBlendFuncs[uint32_t( Castor3D::ColourBlendFunc::eDot3RGB )] = GlBlendFunc::eDot3RGB;
		RgbBlendFuncs[uint32_t( Castor3D::ColourBlendFunc::eDot3RGBA )] = GlBlendFunc::eDot3RGBA;

		AlphaBlendFuncs[uint32_t( Castor3D::AlphaBlendFunc::eNoBlend )] = GlBlendFunc::eModulate;
		AlphaBlendFuncs[uint32_t( Castor3D::AlphaBlendFunc::eFirstArg )] = GlBlendFunc::eReplace;
		AlphaBlendFuncs[uint32_t( Castor3D::AlphaBlendFunc::eAdd )] = GlBlendFunc::eAdd;
		AlphaBlendFuncs[uint32_t( Castor3D::AlphaBlendFunc::eAddSigned )] = GlBlendFunc::eAddSigned;
		AlphaBlendFuncs[uint32_t( Castor3D::AlphaBlendFunc::eModulate )] = GlBlendFunc::eModulate;
		AlphaBlendFuncs[uint32_t( Castor3D::AlphaBlendFunc::eInterpolate )] = GlBlendFunc::eInterpolate;
		AlphaBlendFuncs[uint32_t( Castor3D::AlphaBlendFunc::eSubtract )] = GlBlendFunc::eSubtract;

		BlendOps[uint32_t( Castor3D::BlendOperation::eAdd )] = GlBlendOp::eAdd;
		BlendOps[uint32_t( Castor3D::BlendOperation::eSubtract )] = GlBlendOp::eSubtract;
		BlendOps[uint32_t( Castor3D::BlendOperation::eRevSubtract )] = GlBlendOp::eRevSubtract;
		BlendOps[uint32_t( Castor3D::BlendOperation::eMin )] = GlBlendOp::eMin;
		BlendOps[uint32_t( Castor3D::BlendOperation::eMax )] = GlBlendOp::eMax;

		PixelFormats[uint32_t( Castor::PixelFormat::eL8 )] = PixelFmt( GlFormat::eRed, GlInternal::eR8, GlType::eUnsignedByte );
		PixelFormats[uint32_t( Castor::PixelFormat::eL16F32F )] = PixelFmt( GlFormat::eRed, GlInternal::eR16F, GlType::eFloat );
		PixelFormats[uint32_t( Castor::PixelFormat::eL32F )] = PixelFmt( GlFormat::eRed, GlInternal::eR32F, GlType::eFloat );
		PixelFormats[uint32_t( Castor::PixelFormat::eA8L8 )] = PixelFmt( GlFormat::eRG, GlInternal::eRG16, GlType::eUnsignedByte );
		PixelFormats[uint32_t( Castor::PixelFormat::eAL16F32F )] = PixelFmt( GlFormat::eRG, GlInternal::eRG16F, GlType::eFloat );
		PixelFormats[uint32_t( Castor::PixelFormat::eAL32F )] = PixelFmt( GlFormat::eRG, GlInternal::eRG32F, GlType::eFloat );
		PixelFormats[uint32_t( Castor::PixelFormat::eA1R5G5B5 )] = PixelFmt( GlFormat::eRGBA, GlInternal::eRGB5A1, GlType::eUnsignedShort1555Rev );
		PixelFormats[uint32_t( Castor::PixelFormat::eA4R4G4B4 )] = PixelFmt( GlFormat::eRGBA, GlInternal::eRGBA4, GlType::eUnsignedShort4444Rev );
		PixelFormats[uint32_t( Castor::PixelFormat::eR5G6B5 )] = PixelFmt( GlFormat::eRGB, GlInternal::eRGB16, GlType::eUnsignedShort565Rev );
		PixelFormats[uint32_t( Castor::PixelFormat::eR8G8B8 )] = PixelFmt( GlFormat::eRGB, GlInternal::eRGB8, GlType::eUnsignedByte );
		PixelFormats[uint32_t( Castor::PixelFormat::eB8G8R8 )] = PixelFmt( GlFormat::eBGR, GlInternal::eRGB8, GlType::eUnsignedByte );
		PixelFormats[uint32_t( Castor::PixelFormat::eA8R8G8B8 )] = PixelFmt( GlFormat::eRGBA, GlInternal::eRGBA8, GlType::eUnsignedByte );
		PixelFormats[uint32_t( Castor::PixelFormat::eA8B8G8R8 )] = PixelFmt( GlFormat::eBGRA, GlInternal::eRGBA8, GlType::eUnsignedByte );
		PixelFormats[uint32_t( Castor::PixelFormat::eRGB16F )] = PixelFmt( GlFormat::eRGB, GlInternal::eRGB16F, GlType::eHalfFloat );
		PixelFormats[uint32_t( Castor::PixelFormat::eRGBA16F )] = PixelFmt( GlFormat::eRGBA, GlInternal::eRGBA16F, GlType::eHalfFloat );
		PixelFormats[uint32_t( Castor::PixelFormat::eRGB16F32F )] = PixelFmt( GlFormat::eRGB, GlInternal::eRGB16F, GlType::eFloat );
		PixelFormats[uint32_t( Castor::PixelFormat::eRGBA16F32F )] = PixelFmt( GlFormat::eRGBA, GlInternal::eRGBA16F, GlType::eFloat );
		PixelFormats[uint32_t( Castor::PixelFormat::eRGB32F )] = PixelFmt( GlFormat::eRGB, GlInternal::eRGB32F, GlType::eFloat );
		PixelFormats[uint32_t( Castor::PixelFormat::eRGBA32F )] = PixelFmt( GlFormat::eRGB, GlInternal::eRGBA32F, GlType::eFloat );
		PixelFormats[uint32_t( Castor::PixelFormat::eDXTC1 )] = PixelFmt( GlFormat::eRGB, GlInternal::eDXT1, GlType::eDefault );
		PixelFormats[uint32_t( Castor::PixelFormat::eDXTC3 )] = PixelFmt( GlFormat::eRGBA, GlInternal::eDXT3, GlType::eDefault );
		PixelFormats[uint32_t( Castor::PixelFormat::eDXTC5 )] = PixelFmt( GlFormat::eRGBA, GlInternal::eDXT5, GlType::eDefault );
		PixelFormats[uint32_t( Castor::PixelFormat::eYUY2 )] = PixelFmt( GlFormat( 0 ), GlInternal( 0 ), GlType::eDefault );
		PixelFormats[uint32_t( Castor::PixelFormat::eD16 )] = PixelFmt( GlFormat::eDepth, GlInternal::eD16, GlType::eUnsignedByte );
		PixelFormats[uint32_t( Castor::PixelFormat::eD24 )] = PixelFmt( GlFormat::eDepth, GlInternal::eD24, GlType::eUnsignedByte );
		PixelFormats[uint32_t( Castor::PixelFormat::eD24S8 )] = PixelFmt( GlFormat::eDepthStencil, GlInternal::eD24S8, GlType::eUnsignedInt248 );
		PixelFormats[uint32_t( Castor::PixelFormat::eD32 )] = PixelFmt( GlFormat::eDepth, GlInternal::eD32, GlType::eUnsignedByte );
		PixelFormats[uint32_t( Castor::PixelFormat::eD32F )] = PixelFmt( GlFormat::eDepth, GlInternal::eD32, GlType::eFloat );
		PixelFormats[uint32_t( Castor::PixelFormat::eS1 )] = PixelFmt( GlFormat::eStencil, GlInternal::eS1, GlType::eUnsignedByte );
		PixelFormats[uint32_t( Castor::PixelFormat::eS8 )] = PixelFmt( GlFormat::eStencil, GlInternal::eS8, GlType::eUnsignedByte );

		ShaderTypes[uint32_t( Castor3D::ShaderType::eVertex )] = GlShaderType::eVertex;
		ShaderTypes[uint32_t( Castor3D::ShaderType::eHull )] = GlShaderType::eTessControl;
		ShaderTypes[uint32_t( Castor3D::ShaderType::eDomain )] = GlShaderType::eTessEvaluation;
		ShaderTypes[uint32_t( Castor3D::ShaderType::eGeometry )] = GlShaderType::eGeometry;
		ShaderTypes[uint32_t( Castor3D::ShaderType::ePixel )] = GlShaderType::eFragment;
		ShaderTypes[uint32_t( Castor3D::ShaderType::eCompute )] = GlShaderType::eCompute;

		Internals[uint32_t( Castor::PixelFormat::eL8 )] = GlInternal::eR16;
		Internals[uint32_t( Castor::PixelFormat::eL16F32F )] = GlInternal::eR16F;
		Internals[uint32_t( Castor::PixelFormat::eL32F )] = GlInternal::eR32F;
		Internals[uint32_t( Castor::PixelFormat::eA8L8 )] = GlInternal::eRG16;
		Internals[uint32_t( Castor::PixelFormat::eAL16F32F )] = GlInternal::eRG16F;
		Internals[uint32_t( Castor::PixelFormat::eAL32F )] = GlInternal::eRG32F;
		Internals[uint32_t( Castor::PixelFormat::eA1R5G5B5 )] = GlInternal::eRGBA16UI;
		Internals[uint32_t( Castor::PixelFormat::eA4R4G4B4 )] = GlInternal::eRGBA16UI;
		Internals[uint32_t( Castor::PixelFormat::eR5G6B5 )] = GlInternal::eRGB16UI;
		Internals[uint32_t( Castor::PixelFormat::eR8G8B8 )] = GlInternal::eRGBA32UI;
		Internals[uint32_t( Castor::PixelFormat::eB8G8R8 )] = GlInternal::eRGBA32UI;
		Internals[uint32_t( Castor::PixelFormat::eA8R8G8B8 )] = GlInternal::eRGBA32UI;
		Internals[uint32_t( Castor::PixelFormat::eA8B8G8R8 )] = GlInternal::eRGBA32UI;
		Internals[uint32_t( Castor::PixelFormat::eRGB16F )] = GlInternal::eRGBA16F;
		Internals[uint32_t( Castor::PixelFormat::eRGBA16F )] = GlInternal::eRGBA16F;
		Internals[uint32_t( Castor::PixelFormat::eRGB16F32F )] = GlInternal::eRGBA16F;
		Internals[uint32_t( Castor::PixelFormat::eRGBA16F32F )] = GlInternal::eRGBA16F;
		Internals[uint32_t( Castor::PixelFormat::eRGB32F )] = GlInternal::eRGBA32F;
		Internals[uint32_t( Castor::PixelFormat::eRGBA32F )] = GlInternal::eRGBA32F;

		Attachments[uint32_t( Castor3D::AttachmentPoint::eNone )] = GlAttachmentPoint::eNone;
		Attachments[uint32_t( Castor3D::AttachmentPoint::eColour )] = GlAttachmentPoint::eColour0;
		Attachments[uint32_t( Castor3D::AttachmentPoint::eDepth )] = GlAttachmentPoint::eDepth;
		Attachments[uint32_t( Castor3D::AttachmentPoint::eStencil )] = GlAttachmentPoint::eStencil;

		FramebufferModes[uint32_t( Castor3D::FrameBufferTarget::eDraw )] = GlFrameBufferMode::eDraw;
		FramebufferModes[uint32_t( Castor3D::FrameBufferTarget::eRead )] = GlFrameBufferMode::eRead;
		FramebufferModes[uint32_t( Castor3D::FrameBufferTarget::eBoth )] = GlFrameBufferMode::eDefault;

		RboStorages[uint32_t( Castor::PixelFormat::eL8 )] = GlInternal::eL8;
		RboStorages[uint32_t( Castor::PixelFormat::eL16F32F )] = GlInternal::eR16F;
		RboStorages[uint32_t( Castor::PixelFormat::eL32F )] = GlInternal::eR32F;
		RboStorages[uint32_t( Castor::PixelFormat::eA8L8 )] = GlInternal::eL8A8;
		RboStorages[uint32_t( Castor::PixelFormat::eAL16F32F )] = GlInternal::eRG16F;
		RboStorages[uint32_t( Castor::PixelFormat::eAL32F )] = GlInternal::eRG32F;
		RboStorages[uint32_t( Castor::PixelFormat::eA1R5G5B5 )] = GlInternal::eRGB5A1;
		RboStorages[uint32_t( Castor::PixelFormat::eA4R4G4B4 )] = GlInternal::eRGBA4;
		RboStorages[uint32_t( Castor::PixelFormat::eR5G6B5 )] = GlInternal::eRGB565;
		RboStorages[uint32_t( Castor::PixelFormat::eR8G8B8 )] = GlInternal::eRGB8;
		RboStorages[uint32_t( Castor::PixelFormat::eB8G8R8 )] = GlInternal::eRGB8;
		RboStorages[uint32_t( Castor::PixelFormat::eA8R8G8B8 )] = GlInternal::eRGBA8;
		RboStorages[uint32_t( Castor::PixelFormat::eA8B8G8R8 )] = GlInternal::eRGBA8;
		RboStorages[uint32_t( Castor::PixelFormat::eRGB16F )] = GlInternal::eRGB16F;
		RboStorages[uint32_t( Castor::PixelFormat::eRGBA16F )] = GlInternal::eRGBA16F;
		RboStorages[uint32_t( Castor::PixelFormat::eRGB16F32F )] = GlInternal::eRGB16F;
		RboStorages[uint32_t( Castor::PixelFormat::eRGBA16F32F )] = GlInternal::eRGBA16F;
		RboStorages[uint32_t( Castor::PixelFormat::eRGB32F )] = GlInternal::eRGB32F;
		RboStorages[uint32_t( Castor::PixelFormat::eRGBA32F )] = GlInternal::eRGBA32F;
		RboStorages[uint32_t( Castor::PixelFormat::eDXTC1 )] = GlInternal::eDXT1;
		RboStorages[uint32_t( Castor::PixelFormat::eDXTC3 )] = GlInternal::eDXT3;
		RboStorages[uint32_t( Castor::PixelFormat::eDXTC5 )] = GlInternal::eDXT5;
		RboStorages[uint32_t( Castor::PixelFormat::eYUY2 )] = GlInternal( 0 );
		RboStorages[uint32_t( Castor::PixelFormat::eD16 )] = GlInternal::eD16;
		RboStorages[uint32_t( Castor::PixelFormat::eD24 )] = GlInternal::eD24;
		RboStorages[uint32_t( Castor::PixelFormat::eD24S8 )] = GlInternal::eD24;
		RboStorages[uint32_t( Castor::PixelFormat::eD32 )] = GlInternal::eD32;
		RboStorages[uint32_t( Castor::PixelFormat::eD32F )] = GlInternal::eD32F;
		RboStorages[uint32_t( Castor::PixelFormat::eS1 )] = GlInternal::eS1;
		RboStorages[uint32_t( Castor::PixelFormat::eS8 )] = GlInternal::eS8;

		Buffers[uint32_t( Castor3D::WindowBuffer::eNone )] = GlBufferBinding::eNone;
		Buffers[uint32_t( Castor3D::WindowBuffer::eFrontLeft )] = GlBufferBinding::eFrontLeft;
		Buffers[uint32_t( Castor3D::WindowBuffer::eFrontRight )] = GlBufferBinding::eFrontRight;
		Buffers[uint32_t( Castor3D::WindowBuffer::eBackLeft )] = GlBufferBinding::eBackLeft;
		Buffers[uint32_t( Castor3D::WindowBuffer::eBackRight )] = GlBufferBinding::eBackRight;
		Buffers[uint32_t( Castor3D::WindowBuffer::eFront )] = GlBufferBinding::eFront;
		Buffers[uint32_t( Castor3D::WindowBuffer::eBack )] = GlBufferBinding::eBack;
		Buffers[uint32_t( Castor3D::WindowBuffer::eLeft )] = GlBufferBinding::eLeft;
		Buffers[uint32_t( Castor3D::WindowBuffer::eRight )] = GlBufferBinding::eRight;
		Buffers[uint32_t( Castor3D::WindowBuffer::eFrontAndBack )] = GlBufferBinding::eFrontAndBack;

		Faces[uint32_t( Castor3D::Culling::eNone )] = GlFace( 0 );
		Faces[uint32_t( Castor3D::Culling::eFront )] = GlFace::eFront;
		Faces[uint32_t( Castor3D::Culling::eBack )] = GlFace::eBack;
		Faces[uint32_t( Castor3D::Culling::eFrontAndBack )] = GlFace::eBoth;

		FillModes[uint32_t( Castor3D::FillMode::ePoint )] = GlFillMode::ePoint;
		FillModes[uint32_t( Castor3D::FillMode::eLine )] = GlFillMode::eLine;
		FillModes[uint32_t( Castor3D::FillMode::eSolid )] = GlFillMode::eFill;

		DepthFuncs[uint32_t( Castor3D::DepthFunc::eNever )] = GlComparator::eNever;
		DepthFuncs[uint32_t( Castor3D::DepthFunc::eLess )] = GlComparator::eLess;
		DepthFuncs[uint32_t( Castor3D::DepthFunc::eEqual )] = GlComparator::eEqual;
		DepthFuncs[uint32_t( Castor3D::DepthFunc::eLEqual )] = GlComparator::eLEqual;
		DepthFuncs[uint32_t( Castor3D::DepthFunc::eGreater )] = GlComparator::eGreater;
		DepthFuncs[uint32_t( Castor3D::DepthFunc::eNEqual )] = GlComparator::eNEqual;
		DepthFuncs[uint32_t( Castor3D::DepthFunc::eGEqual )] = GlComparator::eGEqual;
		DepthFuncs[uint32_t( Castor3D::DepthFunc::eAlways )] = GlComparator::eAlways;

		WriteMasks[uint32_t( Castor3D::WritingMask::eZero )] = false;
		WriteMasks[uint32_t( Castor3D::WritingMask::eAll )] = true;

		StencilFuncs[uint32_t( Castor3D::StencilFunc::eNever )] = GlComparator::eNever;
		StencilFuncs[uint32_t( Castor3D::StencilFunc::eLess )] = GlComparator::eLess;
		StencilFuncs[uint32_t( Castor3D::StencilFunc::eEqual )] = GlComparator::eLEqual;
		StencilFuncs[uint32_t( Castor3D::StencilFunc::eLEqual )] = GlComparator::eEqual;
		StencilFuncs[uint32_t( Castor3D::StencilFunc::eGreater )] = GlComparator::eNEqual;
		StencilFuncs[uint32_t( Castor3D::StencilFunc::eNEqual )] = GlComparator::eGEqual;
		StencilFuncs[uint32_t( Castor3D::StencilFunc::eGEqual )] = GlComparator::eGreater;
		StencilFuncs[uint32_t( Castor3D::StencilFunc::eAlways )] = GlComparator::eAlways;

		StencilOps[uint32_t( Castor3D::StencilOp::eKeep )] = GlStencilOp::eKeep;
		StencilOps[uint32_t( Castor3D::StencilOp::eZero )] = GlStencilOp::eZero;
		StencilOps[uint32_t( Castor3D::StencilOp::eReplace )] = GlStencilOp::eReplace;
		StencilOps[uint32_t( Castor3D::StencilOp::eIncrement )] = GlStencilOp::eIncrement;
		StencilOps[uint32_t( Castor3D::StencilOp::eIncrWrap )] = GlStencilOp::eIncrementWrap;
		StencilOps[uint32_t( Castor3D::StencilOp::eDecrement )] = GlStencilOp::eDecrement;
		StencilOps[uint32_t( Castor3D::StencilOp::eDecrWrap )] = GlStencilOp::eDecrementWrap;
		StencilOps[uint32_t( Castor3D::StencilOp::eInvert )] = GlStencilOp::eInvert;

		BuffersTA[GlAttachmentPoint::eNone] = GlBufferBinding::eNone;
		BuffersTA[GlAttachmentPoint::eColour0] = GlBufferBinding::eColor0;
		BuffersTA[GlAttachmentPoint::eDepth] = GlBufferBinding::eDepth;
		BuffersTA[GlAttachmentPoint::eStencil] = GlBufferBinding::eStencil;

		Queries[uint32_t( Castor3D::QueryType::eTimeElapsed )] = GlQueryType::eTimeElapsed;
		Queries[uint32_t( Castor3D::QueryType::eSamplesPassed )] = GlQueryType::eSamplesPassed;
		Queries[uint32_t( Castor3D::QueryType::eAnySamplesPassed )] = GlQueryType::eAnySamplesPassed;
		Queries[uint32_t( Castor3D::QueryType::ePrimitivesGenerated )] = GlQueryType::ePrimitivesGenerated;
		Queries[uint32_t( Castor3D::QueryType::eTransformFeedbackPrimitivesWritten )] = GlQueryType::eTransformFeedbackPrimitivesWritten;
		Queries[uint32_t( Castor3D::QueryType::eAnySamplesPassedConservative )] = GlQueryType::eAnySamplesPassedConservative;
		Queries[uint32_t( Castor3D::QueryType::eTimestamp )] = GlQueryType::eTimestamp;

		QueryInfos[uint32_t( Castor3D::QueryInfo::eResult )] = GlQueryInfo::eResult;
		QueryInfos[uint32_t( Castor3D::QueryInfo::eResultAvailable )] = GlQueryInfo::eResultAvailable;
		QueryInfos[uint32_t( Castor3D::QueryInfo::eResultNoWait )] = GlQueryInfo::eResultNoWait;

		TextureStorages[uint32_t( Castor3D::TextureStorageType::eBuffer )] = GlTextureStorageType::eBuffer;
		TextureStorages[uint32_t( Castor3D::TextureStorageType::eOneDimension )] = GlTextureStorageType::e1D;
		TextureStorages[uint32_t( Castor3D::TextureStorageType::eOneDimensionArray )] = GlTextureStorageType::e2D;
		TextureStorages[uint32_t( Castor3D::TextureStorageType::eTwoDimensions )] = GlTextureStorageType::e2D;
		TextureStorages[uint32_t( Castor3D::TextureStorageType::eTwoDimensionsArray )] = GlTextureStorageType::e2DArray;
		TextureStorages[uint32_t( Castor3D::TextureStorageType::eTwoDimensionsMS )] = GlTextureStorageType::e2DMS;
		TextureStorages[uint32_t( Castor3D::TextureStorageType::eThreeDimensions )] = GlTextureStorageType::e3D;
		TextureStorages[uint32_t( Castor3D::TextureStorageType::eCubeMap )] = GlTextureStorageType::eCubeMap;
		TextureStorages[uint32_t( Castor3D::TextureStorageType::eCubeMapArray )] = GlTextureStorageType::eCubeMapArray;

		CubeMapFaces[uint32_t( Castor3D::CubeMapFace::ePositiveX )] = GlTexDim::ePositiveX;
		CubeMapFaces[uint32_t( Castor3D::CubeMapFace::eNegativeX )] = GlTexDim::eNegativeX;
		CubeMapFaces[uint32_t( Castor3D::CubeMapFace::ePositiveY )] = GlTexDim::ePositiveY;
		CubeMapFaces[uint32_t( Castor3D::CubeMapFace::eNegativeY )] = GlTexDim::eNegativeY;
		CubeMapFaces[uint32_t( Castor3D::CubeMapFace::ePositiveZ )] = GlTexDim::ePositiveZ;
		CubeMapFaces[uint32_t( Castor3D::CubeMapFace::eNegativeZ )] = GlTexDim::eNegativeZ;

		ComparisonModes[uint32_t( Castor3D::ComparisonMode::eNone )] = GlCompareMode::eNone;
		ComparisonModes[uint32_t( Castor3D::ComparisonMode::eRefToTexture )] = GlCompareMode::eRefToTexture;

		Cleanup();

#if defined( _WIN32 )

		m_pfnMakeCurrent = &wglMakeCurrent;
		m_pfnSwapBuffers = &::SwapBuffers;
		m_pfnCreateContext = &wglCreateContext;
		m_pfnDeleteContext = &wglDeleteContext;

#elif defined( __linux__ )

		m_pfnMakeCurrent = &glXMakeCurrent;
		m_pfnSwapBuffers = &glXSwapBuffers;
		m_pfnCreateContext = &glXCreateContext;
		m_pfnDeleteContext = &glXDestroyContext;

#else

#	error "Yet unsupported OS"

#endif
	}

	OpenGl::~OpenGl()
	{
	}

	bool OpenGl::PreInitialise( String const & p_strExtensions )
	{
		char const * l_extensions = ( char const * )glGetString( GL_EXTENSIONS );

		if ( l_extensions )
		{
			m_extensions = string::string_cast< xchar >( l_extensions ) + p_strExtensions;
		}
		else
		{
			m_extensions = p_strExtensions;
		}

		m_vendor = string::string_cast< xchar >( ( char const * )glGetString( GL_VENDOR ) );
		m_renderer = string::string_cast< xchar >( ( char const * )glGetString( GL_RENDERER ) );
		m_version = string::string_cast< xchar >( ( char const * )glGetString( GL_VERSION ) );

		auto l_vendor = string::lower_case( m_vendor );

		if ( l_vendor.find( "nvidia" ) != String::npos )
		{
			m_gpu = GlProvider::eNvidia;
		}
		else if ( l_vendor.find( "ati" ) != String::npos
				  || l_vendor.find( "amd" ) != String::npos )
		{
			m_gpu = GlProvider::eATI;
		}
		else if ( l_vendor.find( "intel" ) != String::npos )
		{
			m_gpu = GlProvider::eIntel;
		}

		double l_version{ 0u };
		StringStream l_stream( m_version );
		l_stream >> l_version;
		m_iVersion = int( l_version * 10 );

		if ( m_iVersion >= 33 )
		{
			m_iGlslVersion = m_iVersion * 10;
		}
		else if ( m_iVersion >= 32 )
		{
			m_iGlslVersion = 150;
		}
		else if ( m_iVersion >= 31 )
		{
			m_iGlslVersion = 140;
		}
		else if ( m_iVersion >= 30 )
		{
			m_iGlslVersion = 130;
		}
		else if ( m_iVersion >= 21 )
		{
			m_iGlslVersion = 120;
		}
		else if ( m_iVersion >= 20 )
		{
			m_iGlslVersion = 110;
		}

#if defined( _WIN32 )

		if ( HasExtension( ARB_create_context ) )
		{
			gl_api::GetFunction( cuT( "wglCreateContextAttribsARB" ), m_pfnCreateContextAttribs );
		}

#elif defined( __linux__ )

		if ( HasExtension( ARB_create_context ) )
		{
			gl_api::GetFunction( cuT( "glXCreateContextAttribsARB" ), m_pfnCreateContextAttribs );
		}
		else
		{
			gl_api::GetFunction( cuT( "glXCreateContextAttribsARB" ), m_pfnCreateContextAttribs );
		}

#else

#	error "Yet unsupported OS"

#endif

		return true;
	}

	bool OpenGl::Initialise()
	{
		m_pfnGetError = &glGetError;
		m_pfnClearColor = &glClearColor;
		m_pfnClearDepth = &glClearDepth;
		m_pfnViewport = &glViewport;
		m_pfnClear = &glClear;
		m_pfnDrawArrays = &glDrawArrays;
		m_pfnDrawElements = &glDrawElements;
		m_pfnEnable = &glEnable;
		m_pfnDisable = &glDisable;
		m_pfnGenTextures = &glGenTextures;
		m_pfnDeleteTextures = &glDeleteTextures;
		m_pfnBlendFunc = &glBlendFunc;
		m_pfnAlphaFunc = &glAlphaFunc;
		m_pfnIsTexture = &glIsTexture;
		m_pfnCullFace = &glCullFace;
		m_pfnFrontFace = &glFrontFace;
		m_pfnMaterialf = &glMaterialf;
		m_pfnMaterialfv = &glMaterialfv;
		m_pfnSelectBuffer = &glSelectBuffer;
		m_pfnGetIntegerv = &glGetIntegerv;
		m_pfnReadBuffer = &glReadBuffer;
		m_pfnReadPixels = &glReadPixels;
		m_pfnDrawBuffer = &glDrawBuffer;
		m_pfnDrawPixels = &glDrawPixels;
		m_pfnPixelStorei = &glPixelStorei;
		m_pfnPixelStoref = &glPixelStoref;
		m_pfnDepthFunc = &glDepthFunc;
		m_pfnDepthMask = &glDepthMask;
		m_pfnColorMask = &glColorMask;
		m_pfnPolygonMode = &glPolygonMode;
		m_pfnHint = &glHint;
		m_pfnPolygonOffset = &glPolygonOffset;
		m_pfnStencilOp = &glStencilOp;
		m_pfnStencilFunc = &glStencilFunc;
		m_pfnStencilMask = &glStencilMask;
		GetFunction( m_pfnBlendEquation, cuT( "glBlendEquation" ), cuT( "EXT" ) );
		GetFunction( m_pfnBlendFuncSeparate, cuT( "glBlendFuncSeparate" ), cuT( "EXT" ) );
		GetFunction( m_pfnBlendColor, cuT( "glBlendColor" ), cuT( "EXT" ) );
		GetFunction( m_pfnStencilOpSeparate, cuT( "glStencilOpSeparate" ), cuT( "ATI" ) );
		GetFunction( m_pfnStencilFuncSeparate, cuT( "glStencilFuncSeparate" ), cuT( "ATI" ) );
		GetFunction( m_pfnStencilMaskSeparate, cuT( "glStencilMaskSeparate" ), cuT( "" ) );
		GetFunction( m_pfnActiveTexture, cuT( "glActiveTexture" ), cuT( "ARB" ) );
		GetFunction( m_pfnClientActiveTexture, cuT( "glClientActiveTexture" ), cuT( "ARB" ) );

#if defined( _WIN32 )

		if ( HasExtension( ARB_create_context ) )
		{
			gl_api::GetFunction( cuT( "wglCreateContextAttribsARB" ), m_pfnCreateContextAttribs );
		}

		if ( HasExtension( EXT_swap_control ) )
		{
			gl_api::GetFunction( cuT( "wglSwapIntervalEXT" ), m_pfnSwapInterval );
		}

#elif defined( __linux__ )

		if ( HasExtension( ARB_create_context ) )
		{
			gl_api::GetFunction( cuT( "glXCreateContextAttribsARB" ), m_pfnCreateContextAttribs );
		}
		else
		{
			gl_api::GetFunction( cuT( "glXCreateContextAttribsARB" ), m_pfnCreateContextAttribs );
		}

		if ( HasExtension( EXT_swap_control ) )
		{
			gl_api::GetFunction( cuT( "glXSwapIntervalEXT" ), m_pfnSwapInterval );
		}
		else
		{
			gl_api::GetFunction( cuT( "glXSwapIntervalEXT" ), m_pfnSwapInterval );
		}

#else

#	error "Yet unsupported OS"

#endif
#if DEF_USE_DIRECT_STATE_ACCESS

		if ( HasExtension( EXT_direct_state_access ) )
		{
			m_pTexFunctions = new TexFunctionsDSA( *this );
			m_pBufFunctions = new BufFunctionsDSA( *this );
		}
		else
#endif
		{
			m_pTexFunctions = new TexFunctions( *this );
			m_pBufFunctions = new BufFunctions( *this );
		}

		if ( HasExtension( NV_shader_buffer_load ) )
		{
			GetFunction( m_pfnGetIntegerui64v, cuT( "glGetIntegerui64v" ), cuT( "NV" ) );
		}

		m_bBindVboToGpuAddress = HasExtension( NV_shader_buffer_load ) && HasExtension( NV_vertex_buffer_unified_memory );

		if ( m_iVersion >= 40 || HasExtension( ARB_draw_buffers_blend ) )
		{
			GetFunction( m_pfnBlendEquationi, cuT( "glBlendEquationi" ), cuT( "ARB" ) );
			GetFunction( m_pfnBlendFuncSeparatei, cuT( "glBlendFuncSeparatei" ), cuT( "ARB" ) );
		}
		else if ( HasExtension( AMD_draw_buffers_blend ) )
		{
			gl_api::GetFunction( cuT( "glBlendEquationIndexedAMD" ), m_pfnBlendEquationi );
			gl_api::GetFunction( cuT( "glBlendFuncIndexedAMD" ), m_pfnBlendFuncSeparatei );
		}

		m_bHasNonPowerOfTwoTextures = HasExtension( ARB_texture_non_power_of_two );

		if ( HasExtension( ARB_texture_multisample ) )
		{
			GetFunction( m_pfnTexImage2DMultisample, cuT( "glTexImage2DMultisample" ), cuT( "" ) );
			GetFunction( m_pfnSampleCoverage, cuT( "glSampleCoverage" ), cuT( "ARB" ) );
		}

		if ( HasExtension( ARB_imaging ) )
		{
		}

		if ( HasExtension( ARB_debug_output ) )
		{
			GetFunction( m_pfnDebugMessageCallback, cuT( "glDebugMessageCallback" ), cuT( "ARB" ) );
		}
		else if ( HasExtension( AMDX_debug_output ) )
		{
			GetFunction( m_pfnDebugMessageCallbackAMD, cuT( "glDebugMessageCallbackAMD" ), cuT( "" ) );
		}

		if ( HasExtension( ARB_vertex_buffer_object ) )
		{
			m_bHasVbo = true;
			GetFunction( m_pfnGenBuffers, cuT( "glGenBuffers" ), cuT( "ARB" ) );
			GetFunction( m_pfnDeleteBuffers, cuT( "glDeleteBuffers" ), cuT( "ARB" ) );
			GetFunction( m_pfnIsBuffer, cuT( "glIsBuffer" ), cuT( "ARB" ) );

			if ( HasExtension( ARB_pixel_buffer_object ) )
			{
				m_bHasPbo = true;
			}
		}

		if ( HasExtension( ARB_transform_feedback2 ) )
		{
			GetFunction( m_pfnGenTransformFeedbacks, cuT( "glGenTransformFeedbacks" ), cuT( "ARB" ) );
			GetFunction( m_pfnDeleteTransformFeedbacks, cuT( "glDeleteTransformFeedbacks" ), cuT( "ARB" ) );
			GetFunction( m_pfnBindTransformFeedback, cuT( "glBindTransformFeedback" ), cuT( "ARB" ) );
			GetFunction( m_pfnIsTransformFeedback, cuT( "glIsTransformFeedback" ), cuT( "ARB" ) );
			GetFunction( m_pfnBeginTransformFeedback, cuT( "glBeginTransformFeedback" ), cuT( "ARB" ) );
			GetFunction( m_pfnPauseTransformFeedback, cuT( "glPauseTransformFeedback" ), cuT( "ARB" ) );
			GetFunction( m_pfnResumeTransformFeedback, cuT( "glResumeTransformFeedback" ), cuT( "ARB" ) );
			GetFunction( m_pfnEndTransformFeedback, cuT( "glEndTransformFeedback" ), cuT( "ARB" ) );
			GetFunction( m_pfnTransformFeedbackVaryings, cuT( "glTransformFeedbackVaryings" ), cuT( "ARB" ) );
			GetFunction( m_pfnDrawTransformFeedback, cuT( "glDrawTransformFeedback" ), cuT( "ARB" ) );
		}

		if ( HasExtension( ARB_vertex_array_object ) )
		{
			m_bHasVao = true;
			GetFunction( m_pfnGenVertexArrays, cuT( "glGenVertexArrays" ), cuT( "ARB" ) );
			GetFunction( m_pfnBindVertexArray, cuT( "glBindVertexArray" ), cuT( "ARB" ) );
			GetFunction( m_pfnIsVertexArray, cuT( "glIsVertexArray" ), cuT( "ARB" ) );
			GetFunction( m_pfnDeleteVertexArrays, cuT( "glDeleteVertexArrays" ), cuT( "ARB" ) );
		}

		if ( HasExtension( ARB_texture_buffer_object ) )
		{
			m_bHasTbo = true;
			GetFunction( m_pfnTexBuffer, cuT( "glTexBuffer" ), cuT( "EXT" ) );
		}

		if ( HasExtension( ARB_framebuffer_object ) )
		{
			m_bHasFbo = true;
			GetFunction( m_pfnDrawBuffers, cuT( "glDrawBuffers" ), cuT( "ARB" ) );
			GetFunction( m_pfnBlitFramebuffer, cuT( "glBlitFramebuffer" ), cuT( "ARB" ) );
			GetFunction( m_pfnGenRenderbuffers, cuT( "glGenRenderbuffers" ), cuT( "ARB" ) );
			GetFunction( m_pfnDeleteRenderbuffers, cuT( "glDeleteRenderbuffers" ), cuT( "ARB" ) );
			GetFunction( m_pfnIsRenderbuffer, cuT( "glIsRenderbuffer" ), cuT( "ARB" ) );
			GetFunction( m_pfnBindRenderbuffer, cuT( "glBindRenderbuffer" ), cuT( "ARB" ) );
			GetFunction( m_pfnRenderbufferStorage, cuT( "glRenderbufferStorage" ), cuT( "ARB" ) );
			GetFunction( m_pfnRenderbufferStorageMultisample, cuT( "glRenderbufferStorageMultisample" ), cuT( "ARB" ) );
			GetFunction( m_pfnGetRenderbufferParameteriv, cuT( "glGetRenderbufferParameteriv" ), cuT( "ARB" ) );
			GetFunction( m_pfnFramebufferRenderbuffer, cuT( "glFramebufferRenderbuffer" ), cuT( "ARB" ) );
			GetFunction( m_pfnGenFramebuffers, cuT( "glGenFramebuffers" ), cuT( "ARB" ) );
			GetFunction( m_pfnDeleteFramebuffers, cuT( "glDeleteFramebuffers" ), cuT( "ARB" ) );
			GetFunction( m_pfnIsFramebuffer, cuT( "glIsFramebuffer" ), cuT( "ARB" ) );
			GetFunction( m_pfnBindFramebuffer, cuT( "glBindFramebuffer" ), cuT( "ARB" ) );
			GetFunction( m_pfnFramebufferTexture, cuT( "glFramebufferTexture" ), cuT( "ARB" ) );
			GetFunction( m_pfnFramebufferTexture1D, cuT( "glFramebufferTexture1D" ), cuT( "ARB" ) );
			GetFunction( m_pfnFramebufferTexture2D, cuT( "glFramebufferTexture2D" ), cuT( "ARB" ) );
			GetFunction( m_pfnFramebufferTexture3D, cuT( "glFramebufferTexture3D" ), cuT( "ARB" ) );
			GetFunction( m_pfnFramebufferTextureLayer, cuT( "glFramebufferTextureLayer" ), cuT( "ARB" ) );
			GetFunction( m_pfnCheckFramebufferStatus, cuT( "glCheckFramebufferStatus" ), cuT( "ARB" ) );
		}
		else if ( HasExtension( EXT_framebuffer_object ) )
		{
			m_bHasFbo = true;
			GetFunction( m_pfnDrawBuffers, cuT( "glDrawBuffers" ), cuT( "EXT" ) );
			GetFunction( m_pfnBlitFramebuffer, cuT( "glBlitFramebuffer" ), cuT( "EXT" ) );
			GetFunction( m_pfnGenRenderbuffers, cuT( "glGenRenderbuffers" ), cuT( "EXT" ) );
			GetFunction( m_pfnDeleteRenderbuffers, cuT( "glDeleteRenderbuffers" ), cuT( "EXT" ) );
			GetFunction( m_pfnIsRenderbuffer, cuT( "glIsRenderbuffer" ), cuT( "EXT" ) );
			GetFunction( m_pfnBindRenderbuffer, cuT( "glBindRenderbuffer" ), cuT( "EXT" ) );
			GetFunction( m_pfnRenderbufferStorage, cuT( "glRenderbufferStorage" ), cuT( "EXT" ) );
			GetFunction( m_pfnRenderbufferStorageMultisample, cuT( "glRenderbufferStorageMultisample" ), cuT( "EXT" ) );
			GetFunction( m_pfnGetRenderbufferParameteriv, cuT( "glGetRenderbufferParameteriv" ), cuT( "EXT" ) );
			GetFunction( m_pfnFramebufferRenderbuffer, cuT( "glFramebufferRenderbuffer" ), cuT( "EXT" ) );
			GetFunction( m_pfnGenFramebuffers, cuT( "glGenFramebuffers" ), cuT( "EXT" ) );
			GetFunction( m_pfnDeleteFramebuffers, cuT( "glDeleteFramebuffers" ), cuT( "EXT" ) );
			GetFunction( m_pfnIsFramebuffer, cuT( "glIsFramebuffer" ), cuT( "EXT" ) );
			GetFunction( m_pfnBindFramebuffer, cuT( "glBindFramebuffer" ), cuT( "EXT" ) );
			GetFunction( m_pfnFramebufferTexture, cuT( "glFramebufferTexture" ), cuT( "EXT" ) );
			GetFunction( m_pfnFramebufferTexture1D, cuT( "glFramebufferTexture1D" ), cuT( "EXT" ) );
			GetFunction( m_pfnFramebufferTexture2D, cuT( "glFramebufferTexture2D" ), cuT( "EXT" ) );
			GetFunction( m_pfnFramebufferTexture3D, cuT( "glFramebufferTexture3D" ), cuT( "EXT" ) );
			GetFunction( m_pfnFramebufferTextureLayer, cuT( "glFramebufferTextureLayer" ), cuT( "EXT" ) );
			GetFunction( m_pfnCheckFramebufferStatus, cuT( "glCheckFramebufferStatus" ), cuT( "EXT" ) );
		}

		if ( HasExtension( ARB_sampler_objects ) )
		{
			m_bHasSpl = true;
			GetFunction( m_pfnGenSamplers, cuT( "glGenSamplers" ), cuT( "ARB" ) );
			GetFunction( m_pfnDeleteSamplers, cuT( "glDeleteSamplers" ), cuT( "ARB" ) );
			GetFunction( m_pfnIsSampler, cuT( "glIsSampler" ), cuT( "ARB" ) );
			GetFunction( m_pfnBindSampler, cuT( "glBindSampler" ), cuT( "ARB" ) );
			GetFunction( m_pfnGetSamplerParameteruiv, cuT( "glGetSamplerParameterIuiv" ), cuT( "ARB" ) );
			GetFunction( m_pfnGetSamplerParameterfv, cuT( "glGetSamplerParameterfv" ), cuT( "ARB" ) );
			GetFunction( m_pfnGetSamplerParameteriv, cuT( "glGetSamplerParameteriv" ), cuT( "ARB" ) );
			GetFunction( m_pfnSamplerParameteruiv, cuT( "glSamplerParameterIuiv" ), cuT( "ARB" ) );
			GetFunction( m_pfnSamplerParameterf, cuT( "glSamplerParameterf" ), cuT( "ARB" ) );
			GetFunction( m_pfnSamplerParameterfv, cuT( "glSamplerParameterfv" ), cuT( "ARB" ) );
			GetFunction( m_pfnSamplerParameteri, cuT( "glSamplerParameteri" ), cuT( "ARB" ) );
			GetFunction( m_pfnSamplerParameteriv, cuT( "glSamplerParameteriv" ), cuT( "ARB" ) );

			if ( HasExtension( EXT_texture_filter_anisotropic ) )
			{
				m_bHasAnisotropic = true;
			}
		}
		else if ( HasExtension( EXT_sampler_objects ) )
		{
			m_bHasSpl = true;
			GetFunction( m_pfnGenSamplers, cuT( "glGenSamplers" ), cuT( "EXT" ) );
			GetFunction( m_pfnDeleteSamplers, cuT( "glDeleteSamplers" ), cuT( "EXT" ) );
			GetFunction( m_pfnIsSampler, cuT( "glIsSampler" ), cuT( "EXT" ) );
			GetFunction( m_pfnBindSampler, cuT( "glBindSampler" ), cuT( "EXT" ) );
			GetFunction( m_pfnGetSamplerParameteruiv, cuT( "glGetSamplerParameterIuiv" ), cuT( "EXT" ) );
			GetFunction( m_pfnGetSamplerParameterfv, cuT( "glGetSamplerParameterfv" ), cuT( "EXT" ) );
			GetFunction( m_pfnGetSamplerParameteriv, cuT( "glGetSamplerParameteriv" ), cuT( "EXT" ) );
			GetFunction( m_pfnSamplerParameteruiv, cuT( "glSamplerParameterIuiv" ), cuT( "EXT" ) );
			GetFunction( m_pfnSamplerParameterf, cuT( "glSamplerParameterf" ), cuT( "EXT" ) );
			GetFunction( m_pfnSamplerParameterfv, cuT( "glSamplerParameterfv" ), cuT( "EXT" ) );
			GetFunction( m_pfnSamplerParameteri, cuT( "glSamplerParameteri" ), cuT( "EXT" ) );
			GetFunction( m_pfnSamplerParameteriv, cuT( "glSamplerParameteriv" ), cuT( "EXT" ) );

			if ( HasExtension( EXT_texture_filter_anisotropic ) )
			{
				m_bHasAnisotropic = true;
			}
		}

		if ( HasExtension( ARB_draw_instanced ) )
		{
			m_bHasInstancedDraw = true;
			GetFunction( m_pfnDrawArraysInstanced, cuT( "glDrawArraysInstanced" ), cuT( "ARB" ) );
			GetFunction( m_pfnDrawElementsInstanced, cuT( "glDrawElementsInstanced" ), cuT( "ARB" ) );

			if ( HasExtension( ARB_instanced_arrays ) )
			{
				m_bHasInstancedArrays = true;
				GetFunction( m_pfnVertexAttribDivisor, cuT( "glVertexAttribDivisor" ), cuT( "ARB" ) );
			}
		}
		else if ( HasExtension( EXT_draw_instanced ) )
		{
			m_bHasInstancedDraw = true;
			GetFunction( m_pfnDrawArraysInstanced, cuT( "glDrawArraysInstanced" ), cuT( "EXT" ) );
			GetFunction( m_pfnDrawElementsInstanced, cuT( "glDrawElementsInstanced" ), cuT( "EXT" ) );

			if ( HasExtension( EXT_instanced_arrays ) )
			{
				m_bHasInstancedArrays = true;
				GetFunction( m_pfnVertexAttribDivisor, cuT( "glVertexAttribDivisor" ), cuT( "EXT" ) );
			}
		}

		if ( HasExtension( ARB_vertex_program ) )
		{
			m_bHasVSh = true;
			GetFunction( m_pfnCreateShader, cuT( "glCreateShader" ), cuT( "" ) );
			GetFunction( m_pfnDeleteShader, cuT( "glDeleteShader" ), cuT( "" ) );
			GetFunction( m_pfnIsShader, cuT( "glIsShader" ), cuT( "ARB" ) );
			GetFunction( m_pfnAttachShader, cuT( "glAttachShader" ), cuT( "" ) );
			GetFunction( m_pfnDetachShader, cuT( "glDetachShader" ), cuT( "" ) );
			GetFunction( m_pfnCompileShader, cuT( "glCompileShader" ), cuT( "ARB" ) );
			GetFunction( m_pfnGetShaderiv, cuT( "glGetShaderiv" ), cuT( "" ) );
			GetFunction( m_pfnGetShaderInfoLog, cuT( "glGetShaderInfoLog" ), cuT( "" ) );
			GetFunction( m_pfnShaderSource, cuT( "glShaderSource" ), cuT( "ARB" ) );
			GetFunction( m_pfnCreateProgram, cuT( "glCreateProgram" ), cuT( "" ) );
			GetFunction( m_pfnDeleteProgram, cuT( "glDeleteProgram" ), cuT( "" ) );
			GetFunction( m_pfnLinkProgram, cuT( "glLinkProgram" ), cuT( "ARB" ) );
			GetFunction( m_pfnUseProgram, cuT( "glUseProgram" ), cuT( "" ) );
			GetFunction( m_pfnGetProgramiv, cuT( "glGetProgramiv" ), cuT( "ARB" ) );
			GetFunction( m_pfnGetProgramInfoLog, cuT( "glGetProgramInfoLog" ), cuT( "" ) );
			GetFunction( m_pfnGetAttribLocation, cuT( "glGetAttribLocation" ), cuT( "ARB" ) );
			GetFunction( m_pfnIsProgram, cuT( "glIsProgram" ), cuT( "ARB" ) );
			GetFunction( m_pfnEnableVertexAttribArray, cuT( "glEnableVertexAttribArray" ), cuT( "ARB" ) );
			GetFunction( m_pfnVertexAttribPointer, cuT( "glVertexAttribPointer" ), cuT( "ARB" ) );
			GetFunction( m_pfnVertexAttribIPointer, cuT( "glVertexAttribIPointer" ), cuT( "ARB" ) );
			GetFunction( m_pfnDisableVertexAttribArray, cuT( "glDisableVertexAttribArray" ), cuT( "ARB" ) );
			GetFunction( m_pfnProgramParameteri, cuT( "glProgramParameteri" ), cuT( "ARB" ) );
			GetFunction( m_pfnGetUniformLocation, cuT( "glGetUniformLocation" ), cuT( "ARB" ) );
			GetFunction( m_pfnGetActiveAttrib, cuT( "glGetActiveAttrib" ), cuT( "ARB" ) );
			GetFunction( m_pfnGetActiveUniform, cuT( "glGetActiveUniform" ), cuT( "ARB" ) );

			if ( HasExtension( ARB_fragment_program ) )
			{
				m_bHasPSh = true;
				GetFunction( m_pfnUniform1i, cuT( "glUniform1i" ), cuT( "ARB" ) );
				GetFunction( m_pfnUniform2i, cuT( "glUniform2i" ), cuT( "ARB" ) );
				GetFunction( m_pfnUniform3i, cuT( "glUniform3i" ), cuT( "ARB" ) );
				GetFunction( m_pfnUniform4i, cuT( "glUniform4i" ), cuT( "ARB" ) );
				GetFunction( m_pfnUniform1iv, cuT( "glUniform1iv" ), cuT( "ARB" ) );
				GetFunction( m_pfnUniform2iv, cuT( "glUniform2iv" ), cuT( "ARB" ) );
				GetFunction( m_pfnUniform3iv, cuT( "glUniform3iv" ), cuT( "ARB" ) );
				GetFunction( m_pfnUniform4iv, cuT( "glUniform4iv" ), cuT( "ARB" ) );
				GetFunction( m_pfnUniform1ui, cuT( "glUniform1ui" ), cuT( "EXT" ) );
				GetFunction( m_pfnUniform2ui, cuT( "glUniform2ui" ), cuT( "EXT" ) );
				GetFunction( m_pfnUniform3ui, cuT( "glUniform3ui" ), cuT( "EXT" ) );
				GetFunction( m_pfnUniform4ui, cuT( "glUniform4ui" ), cuT( "EXT" ) );
				GetFunction( m_pfnUniform1uiv, cuT( "glUniform1uiv" ), cuT( "EXT" ) );
				GetFunction( m_pfnUniform2uiv, cuT( "glUniform2uiv" ), cuT( "EXT" ) );
				GetFunction( m_pfnUniform3uiv, cuT( "glUniform3uiv" ), cuT( "EXT" ) );
				GetFunction( m_pfnUniform4uiv, cuT( "glUniform4uiv" ), cuT( "EXT" ) );
				GetFunction( m_pfnUniform1f, cuT( "glUniform1f" ), cuT( "ARB" ) );
				GetFunction( m_pfnUniform2f, cuT( "glUniform2f" ), cuT( "ARB" ) );
				GetFunction( m_pfnUniform3f, cuT( "glUniform3f" ), cuT( "ARB" ) );
				GetFunction( m_pfnUniform4f, cuT( "glUniform4f" ), cuT( "ARB" ) );
				GetFunction( m_pfnUniform1fv, cuT( "glUniform1fv" ), cuT( "ARB" ) );
				GetFunction( m_pfnUniform2fv, cuT( "glUniform2fv" ), cuT( "ARB" ) );
				GetFunction( m_pfnUniform3fv, cuT( "glUniform3fv" ), cuT( "ARB" ) );
				GetFunction( m_pfnUniform4fv, cuT( "glUniform4fv" ), cuT( "ARB" ) );
				GetFunction( m_pfnUniform1d, cuT( "glUniform1d" ), cuT( "" ) );
				GetFunction( m_pfnUniform2d, cuT( "glUniform2d" ), cuT( "" ) );
				GetFunction( m_pfnUniform3d, cuT( "glUniform3d" ), cuT( "" ) );
				GetFunction( m_pfnUniform4d, cuT( "glUniform4d" ), cuT( "" ) );
				GetFunction( m_pfnUniform1dv, cuT( "glUniform1dv" ), cuT( "" ) );
				GetFunction( m_pfnUniform2dv, cuT( "glUniform2dv" ), cuT( "" ) );
				GetFunction( m_pfnUniform3dv, cuT( "glUniform3dv" ), cuT( "" ) );
				GetFunction( m_pfnUniform4dv, cuT( "glUniform4dv" ), cuT( "" ) );
				GetFunction( m_pfnUniformMatrix2fv, cuT( "glUniformMatrix2fv" ), cuT( "ARB" ) );
				GetFunction( m_pfnUniformMatrix2x3fv, cuT( "glUniformMatrix2x3fv" ), cuT( "" ) );
				GetFunction( m_pfnUniformMatrix2x4fv, cuT( "glUniformMatrix2x4fv" ), cuT( "" ) );
				GetFunction( m_pfnUniformMatrix3fv, cuT( "glUniformMatrix3fv" ), cuT( "ARB" ) );
				GetFunction( m_pfnUniformMatrix3x2fv, cuT( "glUniformMatrix3x2fv" ), cuT( "" ) );
				GetFunction( m_pfnUniformMatrix3x4fv, cuT( "glUniformMatrix3x4fv" ), cuT( "" ) );
				GetFunction( m_pfnUniformMatrix4fv, cuT( "glUniformMatrix4fv" ), cuT( "ARB" ) );
				GetFunction( m_pfnUniformMatrix4x2fv, cuT( "glUniformMatrix4x2fv" ), cuT( "" ) );
				GetFunction( m_pfnUniformMatrix4x3fv, cuT( "glUniformMatrix4x3fv" ), cuT( "" ) );
				GetFunction( m_pfnUniformMatrix2dv, cuT( "glUniformMatrix2dv" ), cuT( "" ) );
				GetFunction( m_pfnUniformMatrix2x3dv, cuT( "glUniformMatrix2x3dv" ), cuT( "" ) );
				GetFunction( m_pfnUniformMatrix2x4dv, cuT( "glUniformMatrix2x4dv" ), cuT( "" ) );
				GetFunction( m_pfnUniformMatrix3dv, cuT( "glUniformMatrix3dv" ), cuT( "" ) );
				GetFunction( m_pfnUniformMatrix3x2dv, cuT( "glUniformMatrix3x2dv" ), cuT( "" ) );
				GetFunction( m_pfnUniformMatrix3x4dv, cuT( "glUniformMatrix3x4dv" ), cuT( "" ) );
				GetFunction( m_pfnUniformMatrix4dv, cuT( "glUniformMatrix4dv" ), cuT( "" ) );
				GetFunction( m_pfnUniformMatrix4x2dv, cuT( "glUniformMatrix4x2dv" ), cuT( "" ) );
				GetFunction( m_pfnUniformMatrix4x3dv, cuT( "glUniformMatrix4x3dv" ), cuT( "" ) );

				if ( HasExtension( ARB_uniform_buffer_object ) )
				{
					m_bHasUbo = m_iGlslVersion >= 140;
					GetFunction( m_pfnGetUniformBlockIndex, cuT( "glGetUniformBlockIndex" ), cuT( "ARB" ) );
					GetFunction( m_pfnBindBufferBase, cuT( "glBindBufferBase" ), cuT( "ARB" ) );
					GetFunction( m_pfnUniformBlockBinding, cuT( "glUniformBlockBinding" ), cuT( "ARB" ) );
					GetFunction( m_pfnGetUniformIndices, cuT( "glGetUniformIndices" ), cuT( "ARB" ) );
					GetFunction( m_pfnGetActiveUniformsiv, cuT( "glGetActiveUniformsiv" ), cuT( "ARB" ) );
					GetFunction( m_pfnGetActiveUniformBlockiv, cuT( "glGetActiveUniformBlockiv" ), cuT( "ARB" ) );
				}
				else if ( HasExtension( EXT_uniform_buffer_object ) )
				{
					m_bHasUbo = m_iGlslVersion >= 140;
					GetFunction( m_pfnGetUniformBlockIndex, cuT( "glGetUniformBlockIndex" ), cuT( "EXT" ) );
					GetFunction( m_pfnBindBufferBase, cuT( "glBindBufferBase" ), cuT( "EXT" ) );
					GetFunction( m_pfnUniformBlockBinding, cuT( "glUniformBlockBinding" ), cuT( "EXT" ) );
					GetFunction( m_pfnGetUniformIndices, cuT( "glGetUniformIndices" ), cuT( "EXT" ) );
					GetFunction( m_pfnGetActiveUniformsiv, cuT( "glGetActiveUniformsiv" ), cuT( "EXT" ) );
					GetFunction( m_pfnGetActiveUniformBlockiv, cuT( "glGetActiveUniformBlockiv" ), cuT( "EXT" ) );
				}

				if ( HasExtension( ARB_geometry_shader4 ) || HasExtension( EXT_geometry_shader4 ) )
				{
					m_bHasGSh = true;
				}

				if ( HasExtension( ARB_tessellation_shader ) )
				{
					m_bHasTSh = true;
					GetFunction( m_pfnPatchParameteri, cuT( "glPatchParameteri" ), cuT( "ARB" ) );
				}

				if ( HasExtension( ARB_compute_shader ) )
				{
					m_bHasCSh = true;
				}
			}
		}

		HasExtension( ARB_timer_query );
		GetFunction( m_pfnGenQueries, cuT( "glGenQueries" ), cuT( "ARB" ) );
		GetFunction( m_pfnDeleteQueries, cuT( "glDeleteQueries" ), cuT( "ARB" ) );
		GetFunction( m_pfnIsQuery, cuT( "glIsQuery" ), cuT( "ARB" ) );
		GetFunction( m_pfnBeginQuery, cuT( "glBeginQuery" ), cuT( "ARB" ) );
		GetFunction( m_pfnEndQuery, cuT( "glEndQuery" ), cuT( "ARB" ) );
		GetFunction( m_pfnQueryCounter, cuT( "glQueryCounter" ), cuT( "ARB" ) );
		GetFunction( m_pfnGetQueryObjectiv, cuT( "glGetQueryObjectiv" ), cuT( "ARB" ) );
		GetFunction( m_pfnGetQueryObjectuiv, cuT( "glGetQueryObjectuiv" ), cuT( "ARB" ) );
		GetFunction( m_pfnGetQueryObjecti64v, cuT( "glGetQueryObjecti64v" ), cuT( "ARB" ) );
		GetFunction( m_pfnGetQueryObjectui64v, cuT( "glGetQueryObjectui64v" ), cuT( "ARB" ) );

		if ( HasExtension( ARB_program_interface_query ) )
		{
			GetFunction( m_pfnGetProgramInterfaceiv, cuT( "glGetProgramInterfaceiv" ), cuT( "ARB" ) );
			GetFunction( m_pfnGetProgramResourceIndex, cuT( "glGetProgramResourceIndex" ), cuT( "ARB" ) );
			GetFunction( m_pfnGetProgramResourceLocation, cuT( "glGetProgramResourceLocation" ), cuT( "ARB" ) );
			GetFunction( m_pfnGetProgramResourceLocationIndex, cuT( "glGetProgramResourceLocationIndex" ), cuT( "ARB" ) );
			GetFunction( m_pfnGetProgramResourceName, cuT( "glGetProgramResourceName" ), cuT( "ARB" ) );
			GetFunction( m_pfnGetProgramResourceiv, cuT( "glGetProgramResourceiv" ), cuT( "ARB" ) );
		}

		if ( HasExtension( ARB_texture_storage ) )
		{
			GetFunction( m_pfnTexStorage1D, cuT( "glTexStorage1D" ), cuT( "ARB" ) );
			GetFunction( m_pfnTexStorage2D, cuT( "glTexStorage2D" ), cuT( "ARB" ) );
			GetFunction( m_pfnTexStorage3D, cuT( "glTexStorage3D" ), cuT( "ARB" ) );
		}

		if ( HasExtension( ARB_texture_storage_multisample ) )
		{
			GetFunction( m_pfnTexStorage2DMultisample, cuT( "glTexStorage2DMultisample" ), cuT( "ARB" ) );
			GetFunction( m_pfnTexStorage3DMultisample, cuT( "glTexStorage3DMultisample" ), cuT( "ARB" ) );
		}

		if ( HasExtension( ARB_shader_storage_buffer_object ) )
		{
			m_bHasSsbo = true;
			GetFunction( m_pfnShaderStorageBlockBinding, cuT( "glShaderStorageBlockBinding" ), cuT( "ARB" ) );
		}

		return true;
	}

	void OpenGl::Cleanup()
	{
		delete m_pTexFunctions;
		m_pTexFunctions = nullptr;
		delete m_pBufFunctions;
		m_pBufFunctions = nullptr;
		m_bHasVao = false;
		m_bHasUbo = false;
		m_bHasPbo = false;
		m_bHasTbo = false;
		m_bHasFbo = false;
		m_bHasVbo = false;
		m_bHasVSh = false;
		m_bHasPSh = false;
		m_bHasGSh = false;
		m_bHasTSh = false;
		m_bHasCSh = false;
		m_bHasSpl = false;
		m_bHasSsbo = false;
		m_bHasNonPowerOfTwoTextures = false;
		m_bBindVboToGpuAddress = false;
		m_iGlslVersion = 0;
		m_iVersion = 0;
	}

#if !defined( NDEBUG )

	void OpenGl::Track( void * p_object, std::string const & p_name, std::string const & p_file, int p_line )const
	{
		m_renderSystem.Track( p_object, p_name, p_file, p_line );
	}

	void OpenGl::UnTrack( void * p_object )const
	{
		m_renderSystem.Untrack( p_object );
	}

#endif

	bool OpenGl::GlCheckError( std::string const & p_text )const
	{
		return DoGlCheckError( string::string_cast< xchar >( p_text ) );
	}

	bool OpenGl::GlCheckError( std::wstring const & p_text )const
	{
		return DoGlCheckError( string::string_cast< xchar >( p_text ) );
	}

	void OpenGl::StDebugLog( GlDebugSource source, GlDebugType type, uint32_t id, GlDebugSeverity severity, int length, const char * message, void * userParam )
	{
		reinterpret_cast< OpenGl * >( userParam )->DebugLog( source, type, id, severity, length, message );
	}

	void OpenGl::StDebugLogAMD( uint32_t id, GlDebugCategory category, GlDebugSeverity severity, int length, const char * message, void * userParam )
	{
		reinterpret_cast< OpenGl * >( userParam )->DebugLogAMD( id, category, severity, length, message );
	}

	bool OpenGl::DoGlCheckError( String const & p_text )const
	{
		static std::map< uint32_t, String > const Errors
		{
			{ GL_INVALID_ENUM, cuT( "Invalid Enum" ) },
			{ GL_INVALID_VALUE, cuT( "Invalid Value" ) },
			{ GL_INVALID_OPERATION, cuT( "Invalid Operation" ) },
			{ GL_STACK_OVERFLOW, cuT( "Stack Overflow" ) },
			{ GL_STACK_UNDERFLOW, cuT( "Stack Underflow" ) },
			{ GL_OUT_OF_MEMORY, cuT( "Out of memory" ) },
			{ GL_INVALID_FRAMEBUFFER_OPERATION, cuT( "Invalid frame buffer operation" ) },
		};

		bool l_return = true;
		uint32_t l_errorCode = GetError();

		if ( l_errorCode )
		{
			//auto l_it = Errors.find( l_errorCode );
			//StringStream l_error;
			//l_error << cuT( "OpenGL Error, on function: " ) << p_text << std::endl;
			//l_error << cuT( "  ID: 0x" ) << std::hex << l_errorCode << std::endl;

			//if ( l_it == Errors.end() )
			//{
			//	l_error << cuT( "  Message: Unknown error" ) << std::endl;
			//}
			//else
			//{
			//	l_error << cuT( "  Message: " ) << l_it->second << std::endl;
			//}

			//String l_sysError = System::GetLastErrorText();

			//if ( !l_sysError.empty() )
			//{
			//	l_error << cuT( "  System: " ) << l_sysError << std::endl;
			//}

			//l_error << Debug::Backtrace{ 20, 4 };
			//Logger::LogError( l_error );
			l_return = false;
			l_errorCode = GetError();
		}

		return l_return;
	}

	void OpenGl::DebugLog( GlDebugSource source, GlDebugType type, uint32_t id, GlDebugSeverity severity, int CU_PARAM_UNUSED( length ), const char * message )const
	{
		if ( id != 131185
			 && id != 131186
			 && id != 131154 )
		{
			bool l_error = false;
			StringStream l_toLog;
			l_toLog << cuT( "OpenGl Debug\n  Source: " );

			switch ( source )
			{
			case GlDebugSource::eAPI:
				l_toLog << cuT( "OpenGL" );
				break;

			case GlDebugSource::eWindowSystem:
				l_toLog << cuT( "Window System" );
				break;

			case GlDebugSource::eShaderCompiler:
				l_toLog << cuT( "Shader compiler" );
				break;

			case GlDebugSource::eThirdParty:
				l_toLog << cuT( "Third party" );
				break;

			case GlDebugSource::eApplication:
				l_toLog << cuT( "Application" );
				break;

			case GlDebugSource::eOther:
				l_toLog << cuT( "Other" );
				break;

			default:
				l_toLog << cuT( "Undefined" );
				break;
			}

			l_toLog << cuT( "\n  Type: " );

			switch ( type )
			{
			case GlDebugType::eError:
				l_toLog << cuT( "Error" );
				break;

			case GlDebugType::eDeprecatedBehavior:
				l_toLog << cuT( "Deprecated behavior" );
				break;

			case GlDebugType::eUndefinedBehavior:
				l_toLog << cuT( "Undefined behavior" );
				break;

			case GlDebugType::ePortability:
				l_toLog << cuT( "Portability" );
				break;

			case GlDebugType::ePerformance:
				l_toLog << cuT( "Performance" );
				break;

			case GlDebugType::eOther:
				l_toLog << cuT( "Other" );
				break;

			default:
				l_toLog << cuT( "Undefined" );
				break;
			}

			l_toLog << cuT( "\n  ID: " ) + string::to_string( id ) + cuT( "\n  Severity: " );

			switch ( severity )
			{
			case GlDebugSeverity::eHigh:
				l_error = true;
				l_toLog << cuT( "High" );
				break;

			case GlDebugSeverity::eMedium:
				l_toLog << cuT( "Medium" );
				break;

			case GlDebugSeverity::eLow:
				l_toLog << cuT( "Low" );
				break;

			default:
				l_toLog << cuT( "Undefined" );
				break;
			}

			l_toLog << cuT( "\n  Message: " ) << string::string_cast< xchar >( message );

			if ( l_error )
			{
				l_toLog << cuT( "\n  " ) << Debug::Backtrace{ 33, 8 };
				Logger::LogError( l_toLog );
			}
			else
			{
				Logger::LogWarning( l_toLog );
			}
		}
	}

	void OpenGl::DebugLogAMD( uint32_t id, GlDebugCategory category, GlDebugSeverity severity, int CU_PARAM_UNUSED( length ), const char * message )const
	{
		bool l_error = false;
		StringStream l_toLog;
		l_toLog << cuT( "OpenGl Debug\n  Category: " );

		switch ( category )
		{
		case GlDebugCategory::eAPIError:
			l_toLog << cuT( "OpenGL" );
			break;

		case GlDebugCategory::eWindowSystem:
			l_toLog << cuT( "Windows" );
			break;

		case GlDebugCategory::eDeprecation:
			l_toLog << cuT( "Deprecated behavior" );
			break;

		case GlDebugCategory::eUndefinedBehavior:
			l_toLog << cuT( "Undefined behavior" );
			break;

		case GlDebugCategory::ePerformance:
			l_toLog << cuT( "Performance" );
			break;

		case GlDebugCategory::eShaderCompiler:
			l_toLog << cuT( "Shader compiler" );
			break;

		case GlDebugCategory::eApplication:
			l_toLog << cuT( "Application" );
			break;

		case GlDebugCategory::eOther:
			l_toLog << cuT( "Other" );
			break;

		default:
			l_toLog << cuT( "Undefined" );
			break;
		}

		l_toLog << cuT( "\n  ID: " ) << string::to_string( id ) << cuT( "\n  Severity: " );

		switch ( severity )
		{
		case GlDebugSeverity::eHigh:
			l_error = true;
			l_toLog << cuT( "High" );
			break;

		case GlDebugSeverity::eMedium:
			l_toLog << cuT( "Medium" );
			break;

		case GlDebugSeverity::eLow:
			l_toLog << cuT( "Low" );
			break;

		default:
			l_toLog << cuT( "Undefined" );
			break;
		}

		l_toLog << cuT( "\n  Message: " ) << string::string_cast< xchar >( message );

		if ( l_error )
		{
			l_toLog << cuT( "\n  " ) << Debug::Backtrace{ 25, 2 };
			Logger::LogError( l_toLog );
		}
		else
		{
			Logger::LogWarning( l_toLog );
		}
	}

	void OpenGl::DisplayExtensions()const
	{
		auto l_array = string::split( m_extensions, cuT( " " ), 0xFFFFFFFF, false );
		std::sort( l_array.begin(), l_array.end() );

		for ( auto l_extension : l_array )
		{
			Logger::LogDebug( l_extension );
		}
	}

//*************************************************************************************************
}
