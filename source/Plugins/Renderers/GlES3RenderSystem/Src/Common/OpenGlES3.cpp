#include "Common/OpenGlES3.hpp"

#include "Render/GlES3RenderSystem.hpp"

#include <Graphics/Rectangle.hpp>
#include <Log/Logger.hpp>
#include <Miscellaneous/Utils.hpp>

#include <GlslWriter.hpp>

#include <iomanip>

using namespace Castor3D;
using namespace Castor;

#define DEF_USE_DIRECT_STATE_ACCESS 0

namespace GlES3Render
{
	//*************************************************************************************************

	TexFunctionsBase::TexFunctionsBase( OpenGlES3 & p_gl )
		: Holder( p_gl )
	{
	}

	//*************************************************************************************************

	TexFunctions::TexFunctions( OpenGlES3 & p_gl )
		: TexFunctionsBase( p_gl )
	{
		m_pfnBindTexture = &glBindTexture;
		m_pfnTexSubImage1D = &glTexSubImage1D;
		m_pfnTexSubImage2D = &glTexSubImage2D;
		m_pfnTexImage1D = &glTexImage1D;
		m_pfnTexImage2D = &glTexImage2D;
		m_pfnGetTexImage = &glGetTexImage;
		gl_api::GetFunction( m_pfnTexImage3D, cuT( "glTexImage3D" ), cuT( "EXT" ) );
		gl_api::GetFunction( m_pfnTexSubImage3D, cuT( "glTexSubImage3D" ), cuT( "EXT" ) );
		gl_api::GetFunction( m_pfnGenerateMipmap, cuT( "glGenerateMipmap" ), cuT( "EXT" ) );
	}

	//*************************************************************************************************

	TexFunctionsDSA::TexFunctionsDSA( OpenGlES3 & p_gl )
		: TexFunctionsBase( p_gl )
		, m_uiTexture( 0 )
		, m_pfnTextureSubImage1D()
		, m_pfnTextureSubImage2D()
		, m_pfnTextureSubImage3D()
		, m_pfnTextureImage1D()
		, m_pfnTextureImage2D()
		, m_pfnTextureImage3D()
	{
		gl_api::GetFunction( m_pfnTextureSubImage1D, cuT( "glTextureSubImage1D" ), cuT( "EXT" ) );
		gl_api::GetFunction( m_pfnTextureSubImage2D, cuT( "glTextureSubImage2D" ), cuT( "EXT" ) );
		gl_api::GetFunction( m_pfnTextureSubImage3D, cuT( "glTextureSubImage3D" ), cuT( "EXT" ) );
		gl_api::GetFunction( m_pfnTextureImage1D, cuT( "glTextureImage1D" ), cuT( "EXT" ) );
		gl_api::GetFunction( m_pfnTextureImage2D, cuT( "glTextureImage2D" ), cuT( "EXT" ) );
		gl_api::GetFunction( m_pfnTextureImage3D, cuT( "glTextureImage3D" ), cuT( "EXT" ) );
		gl_api::GetFunction( m_pfnGetTextureImage, cuT( "glGetTextureImage" ), cuT( "EXT" ) );
		gl_api::GetFunction( m_pfnGenerateMipmap, cuT( "glGenerateTextureMipmap" ), cuT( "EXT" ) );
	}

	//*************************************************************************************************

	BufFunctionsBase::BufFunctionsBase( OpenGlES3 & p_gl )
		: Holder( p_gl )
	{
		if ( p_gl.HasExtension( NV_shader_buffer_load ) )
		{
			gl_api::GetFunction( m_pfnMakeBufferResident, cuT( "glMakeBufferResident" ), cuT( "NV" ) );
			gl_api::GetFunction( m_pfnMakeBufferNonResident, cuT( "glMakeBufferNonResident" ), cuT( "NV" ) );
			gl_api::GetFunction( m_pfnIsBufferResident, cuT( "glIsBufferResident" ), cuT( "NV" ) );
			gl_api::GetFunction( m_pfnMakeNamedBufferResident, cuT( "glMakeNamedBufferResident" ), cuT( "NV" ) );
			gl_api::GetFunction( m_pfnMakeNamedBufferNonResident, cuT( "glMakeNamedBufferNonResident" ), cuT( "NV" ) );
			gl_api::GetFunction( m_pfnIsNamedBufferResident, cuT( "glIsNamedBufferResident" ), cuT( "NV" ) );
			gl_api::GetFunction( m_pfnGetBufferParameterui64v, cuT( "glGetBufferParameterui64v" ), cuT( "NV" ) );
			gl_api::GetFunction( m_pfnGetNamedBufferParameterui64v, cuT( "glGetNamedBufferParameterui64v" ), cuT( "NV" ) );
		}

		if ( p_gl.HasExtension( NV_vertex_buffer_unified_memory ) )
		{
			gl_api::GetFunction( m_pfnBufferAddressRange, cuT( "glBufferAddressRange" ), cuT( "NV" ) );
			gl_api::GetFunction( m_pfnVertexFormat, cuT( "glVertexFormat" ), cuT( "NV" ) );
			gl_api::GetFunction( m_pfnNormalFormat, cuT( "glNormalFormat" ), cuT( "NV" ) );
			gl_api::GetFunction( m_pfnColorFormat, cuT( "glColorFormat" ), cuT( "NV" ) );
			gl_api::GetFunction( m_pfnIndexFormat, cuT( "glIndexFormat" ), cuT( "NV" ) );
			gl_api::GetFunction( m_pfnTexCoordFormat, cuT( "glTexCoordFormat" ), cuT( "NV" ) );
			gl_api::GetFunction( m_pfnEdgeFlagFormat, cuT( "glEdgeFlagFormat" ), cuT( "NV" ) );
			gl_api::GetFunction( m_pfnSecondaryColorFormat, cuT( "glSecondaryColorFormat" ), cuT( "NV" ) );
			gl_api::GetFunction( m_pfnFogCoordFormat, cuT( "glFogCoordFormat" ), cuT( "NV" ) );
			gl_api::GetFunction( m_pfnVertexAttribFormat, cuT( "glVertexAttribFormat" ), cuT( "NV" ) );
			gl_api::GetFunction( m_pfnVertexAttribIFormat, cuT( "glVertexAttribIFormat" ), cuT( "NV" ) );
		}
	}

	//*************************************************************************************************

	BufFunctions::BufFunctions( OpenGlES3 & p_gl )
		: BufFunctionsBase( p_gl )
	{
		gl_api::GetFunction( m_pfnBindBuffer, cuT( "glBindBuffer" ), cuT( "ARB" ) );
		gl_api::GetFunction( m_pfnBufferData, cuT( "glBufferData" ), cuT( "ARB" ) );
		gl_api::GetFunction( m_pfnBufferSubData, cuT( "glBufferSubData" ), cuT( "ARB" ) );
		gl_api::GetFunction( m_pfnCopyBufferSubData, cuT( "glCopyBufferSubData" ), cuT( "ARB" ) );
		gl_api::GetFunction( m_pfnGetBufferParameteriv, cuT( "glGetBufferParameteriv" ), cuT( "ARB" ) );
		gl_api::GetFunction( m_pfnMapBuffer, cuT( "glMapBuffer" ), cuT( "ARB" ) );
		gl_api::GetFunction( m_pfnUnmapBuffer, cuT( "glUnmapBuffer" ), cuT( "ARB" ) );
		gl_api::GetFunction( m_pfnMapBufferRange, cuT( "glMapBufferRange" ), cuT( "" ) );
		gl_api::GetFunction( m_pfnFlushMappedBufferRange, cuT( "glFlushMappedBufferRange" ), cuT( "" ) );
	}

	//*************************************************************************************************

	BufFunctionsDSA::BufFunctionsDSA( OpenGlES3 & p_gl )
		: BufFunctionsBase( p_gl )
		, m_uiBuffer( 0 )
	{
		gl_api::GetFunction( m_pfnNamedBufferData, cuT( "glNamedBufferData" ), cuT( "EXT" ) );
		gl_api::GetFunction( m_pfnNamedBufferSubData, cuT( "glNamedBufferSubData" ), cuT( "EXT" ) );
		gl_api::GetFunction( m_pfnCopyNamedBufferSubData, cuT( "glCopyNamedBufferSubData" ), cuT( "ARB" ) );
		gl_api::GetFunction( m_pfnGetNamedBufferParameteriv, cuT( "glGetNamedBufferParameteriv" ), cuT( "EXT" ) );
		gl_api::GetFunction( m_pfnMapNamedBuffer, cuT( "glMapNamedBuffer" ), cuT( "EXT" ) );
		gl_api::GetFunction( m_pfnUnmapNamedBuffer, cuT( "glUnmapNamedBuffer" ), cuT( "EXT" ) );
		gl_api::GetFunction( m_pfnMapNamedBufferRange, cuT( "glMapNamedBufferRange" ), cuT( "EXT" ) );
		gl_api::GetFunction( m_pfnFlushMappedNamedBufferRange, cuT( "glFlushMappedNamedBufferRange" ), cuT( "EXT" ) );
	}

	//*************************************************************************************************

#define CASTOR_DBG_WIN32 0

	OpenGlES3::OpenGlES3( GlES3RenderSystem & p_renderSystem )
		: m_pfnReadPixels{}
		, m_pfnBlitFramebuffer{}
		, m_pfnTexImage2DMultisample{}
		, m_pfnGetActiveUniform{}
		, m_pfnVertexAttribPointer{}
		, m_renderSystem{ p_renderSystem }
		, m_debug{ p_renderSystem }
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

		PrimitiveTypes[uint32_t( Castor3D::Topology::ePoints )] = GlES3Topology::ePoints;
		PrimitiveTypes[uint32_t( Castor3D::Topology::eLines )] = GlES3Topology::eLines;
		PrimitiveTypes[uint32_t( Castor3D::Topology::eLineLoop )] = GlES3Topology::eLineLoop;
		PrimitiveTypes[uint32_t( Castor3D::Topology::eLineStrip )] = GlES3Topology::eLineStrip;
		PrimitiveTypes[uint32_t( Castor3D::Topology::eTriangles )] = GlES3Topology::eTriangles;
		PrimitiveTypes[uint32_t( Castor3D::Topology::eTriangleStrips )] = GlES3Topology::eTriangleStrip;
		PrimitiveTypes[uint32_t( Castor3D::Topology::eTriangleFan )] = GlES3Topology::eTriangleFan;
		PrimitiveTypes[uint32_t( Castor3D::Topology::eQuads )] = GlES3Topology::eQuads;
		PrimitiveTypes[uint32_t( Castor3D::Topology::eQuadStrips )] = GlES3Topology::eQuadStrip;
		PrimitiveTypes[uint32_t( Castor3D::Topology::ePolygon )] = GlES3Topology::ePolygon;

		TextureDimensions[uint32_t( Castor3D::TextureType::eBuffer )] = GlES3TexDim::eBuffer;
		TextureDimensions[uint32_t( Castor3D::TextureType::eOneDimension )] = GlES3TexDim::e1D;
		TextureDimensions[uint32_t( Castor3D::TextureType::eOneDimensionArray )] = GlES3TexDim::e1DArray;
		TextureDimensions[uint32_t( Castor3D::TextureType::eTwoDimensions )] = GlES3TexDim::e2D;
		TextureDimensions[uint32_t( Castor3D::TextureType::eTwoDimensionsArray )] = GlES3TexDim::e2DArray;
		TextureDimensions[uint32_t( Castor3D::TextureType::eTwoDimensionsMS )] = GlES3TexDim::e2DMS;
		TextureDimensions[uint32_t( Castor3D::TextureType::eTwoDimensionsMSArray )] = GlES3TexDim::e2DMSArray;
		TextureDimensions[uint32_t( Castor3D::TextureType::eThreeDimensions )] = GlES3TexDim::e3D;
		TextureDimensions[uint32_t( Castor3D::TextureType::eCube )] = GlES3TexDim::eCube;
		TextureDimensions[uint32_t( Castor3D::TextureType::eCubeArray )] = GlES3TexDim::eCubeArray;

		AlphaFuncs[uint32_t( Castor3D::ComparisonFunc::eAlways )] = GlES3Comparator::eAlways;
		AlphaFuncs[uint32_t( Castor3D::ComparisonFunc::eLess )] = GlES3Comparator::eLess;
		AlphaFuncs[uint32_t( Castor3D::ComparisonFunc::eLEqual )] = GlES3Comparator::eLEqual;
		AlphaFuncs[uint32_t( Castor3D::ComparisonFunc::eEqual )] = GlES3Comparator::eEqual;
		AlphaFuncs[uint32_t( Castor3D::ComparisonFunc::eNEqual )] = GlES3Comparator::eNEqual;
		AlphaFuncs[uint32_t( Castor3D::ComparisonFunc::eGEqual )] = GlES3Comparator::eGEqual;
		AlphaFuncs[uint32_t( Castor3D::ComparisonFunc::eGreater )] = GlES3Comparator::eGreater;
		AlphaFuncs[uint32_t( Castor3D::ComparisonFunc::eNever )] = GlES3Comparator::eNever;

		TextureWrapMode[uint32_t( Castor3D::WrapMode::eRepeat )] = GlES3WrapMode::eRepeat;
		TextureWrapMode[uint32_t( Castor3D::WrapMode::eMirroredRepeat )] = GlES3WrapMode::eMirroredRepeat;
		TextureWrapMode[uint32_t( Castor3D::WrapMode::eClampToBorder )] = GlES3WrapMode::eClampToBorder;
		TextureWrapMode[uint32_t( Castor3D::WrapMode::eClampToEdge )] = GlES3WrapMode::eClampToEdge;

		TextureInterpolation[uint32_t( Castor3D::InterpolationMode::eUndefined )] = GlES3InterpolationMode::eNearest;
		TextureInterpolation[uint32_t( Castor3D::InterpolationMode::eNearest )] = GlES3InterpolationMode::eNearest;
		TextureInterpolation[uint32_t( Castor3D::InterpolationMode::eLinear )] = GlES3InterpolationMode::eLinear;

		BlendFactors[uint32_t( Castor3D::BlendOperand::eZero )] = GlES3BlendFactor::eZero;
		BlendFactors[uint32_t( Castor3D::BlendOperand::eOne )] = GlES3BlendFactor::eOne;
		BlendFactors[uint32_t( Castor3D::BlendOperand::eSrcColour )] = GlES3BlendFactor::eSrcColour;
		BlendFactors[uint32_t( Castor3D::BlendOperand::eInvSrcColour )] = GlES3BlendFactor::eInvSrcColour;
		BlendFactors[uint32_t( Castor3D::BlendOperand::eDstColour )] = GlES3BlendFactor::eDstColour;
		BlendFactors[uint32_t( Castor3D::BlendOperand::eInvDstColour )] = GlES3BlendFactor::eInvDstColour;
		BlendFactors[uint32_t( Castor3D::BlendOperand::eSrcAlpha )] = GlES3BlendFactor::eSrcAlpha;
		BlendFactors[uint32_t( Castor3D::BlendOperand::eInvSrcAlpha )] = GlES3BlendFactor::eInvSrcAlpha;
		BlendFactors[uint32_t( Castor3D::BlendOperand::eDstAlpha )] = GlES3BlendFactor::eDstAlpha;
		BlendFactors[uint32_t( Castor3D::BlendOperand::eInvDstAlpha )] = GlES3BlendFactor::eInvDstAlpha;
		BlendFactors[uint32_t( Castor3D::BlendOperand::eConstant )] = GlES3BlendFactor::eConstant;
		BlendFactors[uint32_t( Castor3D::BlendOperand::eInvConstant )] = GlES3BlendFactor::eInvConstant;
		BlendFactors[uint32_t( Castor3D::BlendOperand::eSrcAlphaSaturate )] = GlES3BlendFactor::eSrcAlphaSaturate;
		BlendFactors[uint32_t( Castor3D::BlendOperand::eSrc1Colour )] = GlES3BlendFactor::eSrc1Colour;
		BlendFactors[uint32_t( Castor3D::BlendOperand::eInvSrc1Colour )] = GlES3BlendFactor::eInvSrc1Colour;
		BlendFactors[uint32_t( Castor3D::BlendOperand::eSrc1Alpha )] = GlES3BlendFactor::eSrc1Alpha;
		BlendFactors[uint32_t( Castor3D::BlendOperand::eInvSrc1Alpha )] = GlES3BlendFactor::eInvSrc1Alpha;

		TextureArguments[uint32_t( Castor3D::BlendSource::eTexture )] = GlES3BlendSource::eTexture;
		TextureArguments[uint32_t( Castor3D::BlendSource::eTexture0 )] = GlES3BlendSource::eTexture0;
		TextureArguments[uint32_t( Castor3D::BlendSource::eTexture1 )] = GlES3BlendSource::eTexture1;
		TextureArguments[uint32_t( Castor3D::BlendSource::eTexture2 )] = GlES3BlendSource::eTexture2;
		TextureArguments[uint32_t( Castor3D::BlendSource::eTexture3 )] = GlES3BlendSource::eTexture3;
		TextureArguments[uint32_t( Castor3D::BlendSource::eConstant )] = GlES3BlendSource::eConstant;
		TextureArguments[uint32_t( Castor3D::BlendSource::eDiffuse )] = GlES3BlendSource::eColour;
		TextureArguments[uint32_t( Castor3D::BlendSource::ePrevious )] = GlES3BlendSource::ePrevious;

		RgbBlendFuncs[uint32_t( Castor3D::ColourBlendFunc::eNoBlend )] = GlES3BlendFunc::eModulate;
		RgbBlendFuncs[uint32_t( Castor3D::ColourBlendFunc::eFirstArg )] = GlES3BlendFunc::eReplace;
		RgbBlendFuncs[uint32_t( Castor3D::ColourBlendFunc::eAdd )] = GlES3BlendFunc::eAdd;
		RgbBlendFuncs[uint32_t( Castor3D::ColourBlendFunc::eAddSigned )] = GlES3BlendFunc::eAddSigned;
		RgbBlendFuncs[uint32_t( Castor3D::ColourBlendFunc::eModulate )] = GlES3BlendFunc::eModulate;
		RgbBlendFuncs[uint32_t( Castor3D::ColourBlendFunc::eInterpolate )] = GlES3BlendFunc::eInterpolate;
		RgbBlendFuncs[uint32_t( Castor3D::ColourBlendFunc::eSubtract )] = GlES3BlendFunc::eSubtract;
		RgbBlendFuncs[uint32_t( Castor3D::ColourBlendFunc::eDot3RGB )] = GlES3BlendFunc::eDot3RGB;
		RgbBlendFuncs[uint32_t( Castor3D::ColourBlendFunc::eDot3RGBA )] = GlES3BlendFunc::eDot3RGBA;

		AlphaBlendFuncs[uint32_t( Castor3D::AlphaBlendFunc::eNoBlend )] = GlES3BlendFunc::eModulate;
		AlphaBlendFuncs[uint32_t( Castor3D::AlphaBlendFunc::eFirstArg )] = GlES3BlendFunc::eReplace;
		AlphaBlendFuncs[uint32_t( Castor3D::AlphaBlendFunc::eAdd )] = GlES3BlendFunc::eAdd;
		AlphaBlendFuncs[uint32_t( Castor3D::AlphaBlendFunc::eAddSigned )] = GlES3BlendFunc::eAddSigned;
		AlphaBlendFuncs[uint32_t( Castor3D::AlphaBlendFunc::eModulate )] = GlES3BlendFunc::eModulate;
		AlphaBlendFuncs[uint32_t( Castor3D::AlphaBlendFunc::eInterpolate )] = GlES3BlendFunc::eInterpolate;
		AlphaBlendFuncs[uint32_t( Castor3D::AlphaBlendFunc::eSubtract )] = GlES3BlendFunc::eSubtract;

		BlendOps[uint32_t( Castor3D::BlendOperation::eAdd )] = GlES3BlendOp::eAdd;
		BlendOps[uint32_t( Castor3D::BlendOperation::eSubtract )] = GlES3BlendOp::eSubtract;
		BlendOps[uint32_t( Castor3D::BlendOperation::eRevSubtract )] = GlES3BlendOp::eRevSubtract;
		BlendOps[uint32_t( Castor3D::BlendOperation::eMin )] = GlES3BlendOp::eMin;
		BlendOps[uint32_t( Castor3D::BlendOperation::eMax )] = GlES3BlendOp::eMax;

		PixelFormats[uint32_t( Castor::PixelFormat::eL8 )] = PixelFmt( GlES3Format::eRed, GlES3Internal::eR8, GlES3Type::eUnsignedByte );
		PixelFormats[uint32_t( Castor::PixelFormat::eL16F32F )] = PixelFmt( GlES3Format::eRed, GlES3Internal::eR16F, GlES3Type::eFloat );
		PixelFormats[uint32_t( Castor::PixelFormat::eL32F )] = PixelFmt( GlES3Format::eRed, GlES3Internal::eR32F, GlES3Type::eFloat );
		PixelFormats[uint32_t( Castor::PixelFormat::eA8L8 )] = PixelFmt( GlES3Format::eRG, GlES3Internal::eRG16, GlES3Type::eUnsignedByte );
		PixelFormats[uint32_t( Castor::PixelFormat::eAL16F32F )] = PixelFmt( GlES3Format::eRG, GlES3Internal::eRG16F, GlES3Type::eFloat );
		PixelFormats[uint32_t( Castor::PixelFormat::eAL32F )] = PixelFmt( GlES3Format::eRG, GlES3Internal::eRG32F, GlES3Type::eFloat );
		PixelFormats[uint32_t( Castor::PixelFormat::eA1R5G5B5 )] = PixelFmt( GlES3Format::eRGBA, GlES3Internal::eRGB5A1, GlES3Type::eUnsignedShort1555Rev );
		PixelFormats[uint32_t( Castor::PixelFormat::eA4R4G4B4 )] = PixelFmt( GlES3Format::eRGBA, GlES3Internal::eRGBA4, GlES3Type::eUnsignedShort4444Rev );
		PixelFormats[uint32_t( Castor::PixelFormat::eR5G6B5 )] = PixelFmt( GlES3Format::eRGB, GlES3Internal::eRGB16, GlES3Type::eUnsignedShort565Rev );
		PixelFormats[uint32_t( Castor::PixelFormat::eR8G8B8 )] = PixelFmt( GlES3Format::eRGB, GlES3Internal::eRGB8, GlES3Type::eUnsignedByte );
		PixelFormats[uint32_t( Castor::PixelFormat::eB8G8R8 )] = PixelFmt( GlES3Format::eBGR, GlES3Internal::eRGB8, GlES3Type::eUnsignedByte );
		PixelFormats[uint32_t( Castor::PixelFormat::eA8R8G8B8 )] = PixelFmt( GlES3Format::eRGBA, GlES3Internal::eRGBA8, GlES3Type::eUnsignedByte );
		PixelFormats[uint32_t( Castor::PixelFormat::eA8B8G8R8 )] = PixelFmt( GlES3Format::eBGRA, GlES3Internal::eRGBA8, GlES3Type::eUnsignedByte );
		PixelFormats[uint32_t( Castor::PixelFormat::eRGB16F )] = PixelFmt( GlES3Format::eRGB, GlES3Internal::eRGB16F, GlES3Type::eHalfFloat );
		PixelFormats[uint32_t( Castor::PixelFormat::eRGBA16F )] = PixelFmt( GlES3Format::eRGBA, GlES3Internal::eRGBA16F, GlES3Type::eHalfFloat );
		PixelFormats[uint32_t( Castor::PixelFormat::eRGB16F32F )] = PixelFmt( GlES3Format::eRGB, GlES3Internal::eRGB16F, GlES3Type::eFloat );
		PixelFormats[uint32_t( Castor::PixelFormat::eRGBA16F32F )] = PixelFmt( GlES3Format::eRGBA, GlES3Internal::eRGBA16F, GlES3Type::eFloat );
		PixelFormats[uint32_t( Castor::PixelFormat::eRGB32F )] = PixelFmt( GlES3Format::eRGB, GlES3Internal::eRGB32F, GlES3Type::eFloat );
		PixelFormats[uint32_t( Castor::PixelFormat::eRGBA32F )] = PixelFmt( GlES3Format::eRGB, GlES3Internal::eRGBA32F, GlES3Type::eFloat );
		PixelFormats[uint32_t( Castor::PixelFormat::eDXTC1 )] = PixelFmt( GlES3Format::eRGB, GlES3Internal::eDXT1, GlES3Type::eDefault );
		PixelFormats[uint32_t( Castor::PixelFormat::eDXTC3 )] = PixelFmt( GlES3Format::eRGBA, GlES3Internal::eDXT3, GlES3Type::eDefault );
		PixelFormats[uint32_t( Castor::PixelFormat::eDXTC5 )] = PixelFmt( GlES3Format::eRGBA, GlES3Internal::eDXT5, GlES3Type::eDefault );
		PixelFormats[uint32_t( Castor::PixelFormat::eYUY2 )] = PixelFmt( GlES3Format( 0 ), GlES3Internal( 0 ), GlES3Type::eDefault );
		PixelFormats[uint32_t( Castor::PixelFormat::eD16 )] = PixelFmt( GlES3Format::eDepth, GlES3Internal::eD16, GlES3Type::eUnsignedByte );
		PixelFormats[uint32_t( Castor::PixelFormat::eD24 )] = PixelFmt( GlES3Format::eDepth, GlES3Internal::eD24, GlES3Type::eUnsignedByte );
		PixelFormats[uint32_t( Castor::PixelFormat::eD24S8 )] = PixelFmt( GlES3Format::eDepthStencil, GlES3Internal::eD24S8, GlES3Type::eUnsignedInt248 );
		PixelFormats[uint32_t( Castor::PixelFormat::eD32 )] = PixelFmt( GlES3Format::eDepth, GlES3Internal::eD32, GlES3Type::eUnsignedByte );
		PixelFormats[uint32_t( Castor::PixelFormat::eD32F )] = PixelFmt( GlES3Format::eDepth, GlES3Internal::eD32, GlES3Type::eFloat );
		PixelFormats[uint32_t( Castor::PixelFormat::eS1 )] = PixelFmt( GlES3Format::eStencil, GlES3Internal::eS1, GlES3Type::eUnsignedByte );
		PixelFormats[uint32_t( Castor::PixelFormat::eS8 )] = PixelFmt( GlES3Format::eStencil, GlES3Internal::eS8, GlES3Type::eUnsignedByte );

		ShaderTypes[uint32_t( Castor3D::ShaderType::eVertex )] = GlES3ShaderType::eVertex;
		ShaderTypes[uint32_t( Castor3D::ShaderType::eHull )] = GlES3ShaderType::eTessControl;
		ShaderTypes[uint32_t( Castor3D::ShaderType::eDomain )] = GlES3ShaderType::eTessEvaluation;
		ShaderTypes[uint32_t( Castor3D::ShaderType::eGeometry )] = GlES3ShaderType::eGeometry;
		ShaderTypes[uint32_t( Castor3D::ShaderType::ePixel )] = GlES3ShaderType::eFragment;
		ShaderTypes[uint32_t( Castor3D::ShaderType::eCompute )] = GlES3ShaderType::eCompute;

		Internals[uint32_t( Castor::PixelFormat::eL8 )] = GlES3Internal::eR16;
		Internals[uint32_t( Castor::PixelFormat::eL16F32F )] = GlES3Internal::eR16F;
		Internals[uint32_t( Castor::PixelFormat::eL32F )] = GlES3Internal::eR32F;
		Internals[uint32_t( Castor::PixelFormat::eA8L8 )] = GlES3Internal::eRG16;
		Internals[uint32_t( Castor::PixelFormat::eAL16F32F )] = GlES3Internal::eRG16F;
		Internals[uint32_t( Castor::PixelFormat::eAL32F )] = GlES3Internal::eRG32F;
		Internals[uint32_t( Castor::PixelFormat::eA1R5G5B5 )] = GlES3Internal::eRGBA16UI;
		Internals[uint32_t( Castor::PixelFormat::eA4R4G4B4 )] = GlES3Internal::eRGBA16UI;
		Internals[uint32_t( Castor::PixelFormat::eR5G6B5 )] = GlES3Internal::eRGB16UI;
		Internals[uint32_t( Castor::PixelFormat::eR8G8B8 )] = GlES3Internal::eRGBA32UI;
		Internals[uint32_t( Castor::PixelFormat::eB8G8R8 )] = GlES3Internal::eRGBA32UI;
		Internals[uint32_t( Castor::PixelFormat::eA8R8G8B8 )] = GlES3Internal::eRGBA32UI;
		Internals[uint32_t( Castor::PixelFormat::eA8B8G8R8 )] = GlES3Internal::eRGBA32UI;
		Internals[uint32_t( Castor::PixelFormat::eRGB16F )] = GlES3Internal::eRGBA16F;
		Internals[uint32_t( Castor::PixelFormat::eRGBA16F )] = GlES3Internal::eRGBA16F;
		Internals[uint32_t( Castor::PixelFormat::eRGB16F32F )] = GlES3Internal::eRGBA16F;
		Internals[uint32_t( Castor::PixelFormat::eRGBA16F32F )] = GlES3Internal::eRGBA16F;
		Internals[uint32_t( Castor::PixelFormat::eRGB32F )] = GlES3Internal::eRGBA32F;
		Internals[uint32_t( Castor::PixelFormat::eRGBA32F )] = GlES3Internal::eRGBA32F;

		Attachments[uint32_t( Castor3D::AttachmentPoint::eNone )] = GlES3AttachmentPoint::eNone;
		Attachments[uint32_t( Castor3D::AttachmentPoint::eColour )] = GlES3AttachmentPoint::eColour0;
		Attachments[uint32_t( Castor3D::AttachmentPoint::eDepth )] = GlES3AttachmentPoint::eDepth;
		Attachments[uint32_t( Castor3D::AttachmentPoint::eStencil )] = GlES3AttachmentPoint::eStencil;

		FramebufferModes[uint32_t( Castor3D::FrameBufferTarget::eDraw )] = GlES3FrameBufferMode::eDraw;
		FramebufferModes[uint32_t( Castor3D::FrameBufferTarget::eRead )] = GlES3FrameBufferMode::eRead;
		FramebufferModes[uint32_t( Castor3D::FrameBufferTarget::eBoth )] = GlES3FrameBufferMode::eDefault;

		RboStorages[uint32_t( Castor::PixelFormat::eL8 )] = GlES3Internal::eL8;
		RboStorages[uint32_t( Castor::PixelFormat::eL16F32F )] = GlES3Internal::eR16F;
		RboStorages[uint32_t( Castor::PixelFormat::eL32F )] = GlES3Internal::eR32F;
		RboStorages[uint32_t( Castor::PixelFormat::eA8L8 )] = GlES3Internal::eL8A8;
		RboStorages[uint32_t( Castor::PixelFormat::eAL16F32F )] = GlES3Internal::eRG16F;
		RboStorages[uint32_t( Castor::PixelFormat::eAL32F )] = GlES3Internal::eRG32F;
		RboStorages[uint32_t( Castor::PixelFormat::eA1R5G5B5 )] = GlES3Internal::eRGB5A1;
		RboStorages[uint32_t( Castor::PixelFormat::eA4R4G4B4 )] = GlES3Internal::eRGBA4;
		RboStorages[uint32_t( Castor::PixelFormat::eR5G6B5 )] = GlES3Internal::eRGB565;
		RboStorages[uint32_t( Castor::PixelFormat::eR8G8B8 )] = GlES3Internal::eRGB8;
		RboStorages[uint32_t( Castor::PixelFormat::eB8G8R8 )] = GlES3Internal::eRGB8;
		RboStorages[uint32_t( Castor::PixelFormat::eA8R8G8B8 )] = GlES3Internal::eRGBA8;
		RboStorages[uint32_t( Castor::PixelFormat::eA8B8G8R8 )] = GlES3Internal::eRGBA8;
		RboStorages[uint32_t( Castor::PixelFormat::eRGB16F )] = GlES3Internal::eRGB16F;
		RboStorages[uint32_t( Castor::PixelFormat::eRGBA16F )] = GlES3Internal::eRGBA16F;
		RboStorages[uint32_t( Castor::PixelFormat::eRGB16F32F )] = GlES3Internal::eRGB16F;
		RboStorages[uint32_t( Castor::PixelFormat::eRGBA16F32F )] = GlES3Internal::eRGBA16F;
		RboStorages[uint32_t( Castor::PixelFormat::eRGB32F )] = GlES3Internal::eRGB32F;
		RboStorages[uint32_t( Castor::PixelFormat::eRGBA32F )] = GlES3Internal::eRGBA32F;
		RboStorages[uint32_t( Castor::PixelFormat::eDXTC1 )] = GlES3Internal::eDXT1;
		RboStorages[uint32_t( Castor::PixelFormat::eDXTC3 )] = GlES3Internal::eDXT3;
		RboStorages[uint32_t( Castor::PixelFormat::eDXTC5 )] = GlES3Internal::eDXT5;
		RboStorages[uint32_t( Castor::PixelFormat::eYUY2 )] = GlES3Internal( 0 );
		RboStorages[uint32_t( Castor::PixelFormat::eD16 )] = GlES3Internal::eD16;
		RboStorages[uint32_t( Castor::PixelFormat::eD24 )] = GlES3Internal::eD24;
		RboStorages[uint32_t( Castor::PixelFormat::eD24S8 )] = GlES3Internal::eD24;
		RboStorages[uint32_t( Castor::PixelFormat::eD32 )] = GlES3Internal::eD32;
		RboStorages[uint32_t( Castor::PixelFormat::eD32F )] = GlES3Internal::eD32F;
		RboStorages[uint32_t( Castor::PixelFormat::eS1 )] = GlES3Internal::eS1;
		RboStorages[uint32_t( Castor::PixelFormat::eS8 )] = GlES3Internal::eS8;

		Buffers[uint32_t( Castor3D::WindowBuffer::eNone )] = GlES3BufferBinding::eNone;
		Buffers[uint32_t( Castor3D::WindowBuffer::eFrontLeft )] = GlES3BufferBinding::eFrontLeft;
		Buffers[uint32_t( Castor3D::WindowBuffer::eFrontRight )] = GlES3BufferBinding::eFrontRight;
		Buffers[uint32_t( Castor3D::WindowBuffer::eBackLeft )] = GlES3BufferBinding::eBackLeft;
		Buffers[uint32_t( Castor3D::WindowBuffer::eBackRight )] = GlES3BufferBinding::eBackRight;
		Buffers[uint32_t( Castor3D::WindowBuffer::eFront )] = GlES3BufferBinding::eFront;
		Buffers[uint32_t( Castor3D::WindowBuffer::eBack )] = GlES3BufferBinding::eBack;
		Buffers[uint32_t( Castor3D::WindowBuffer::eLeft )] = GlES3BufferBinding::eLeft;
		Buffers[uint32_t( Castor3D::WindowBuffer::eRight )] = GlES3BufferBinding::eRight;
		Buffers[uint32_t( Castor3D::WindowBuffer::eFrontAndBack )] = GlES3BufferBinding::eFrontAndBack;

		Faces[uint32_t( Castor3D::Culling::eNone )] = GlES3Face( 0 );
		Faces[uint32_t( Castor3D::Culling::eFront )] = GlES3Face::eFront;
		Faces[uint32_t( Castor3D::Culling::eBack )] = GlES3Face::eBack;
		Faces[uint32_t( Castor3D::Culling::eFrontAndBack )] = GlES3Face::eBoth;

		FillModes[uint32_t( Castor3D::FillMode::ePoint )] = GlES3FillMode::ePoint;
		FillModes[uint32_t( Castor3D::FillMode::eLine )] = GlES3FillMode::eLine;
		FillModes[uint32_t( Castor3D::FillMode::eSolid )] = GlES3FillMode::eFill;

		DepthFuncs[uint32_t( Castor3D::DepthFunc::eNever )] = GlES3Comparator::eNever;
		DepthFuncs[uint32_t( Castor3D::DepthFunc::eLess )] = GlES3Comparator::eLess;
		DepthFuncs[uint32_t( Castor3D::DepthFunc::eEqual )] = GlES3Comparator::eEqual;
		DepthFuncs[uint32_t( Castor3D::DepthFunc::eLEqual )] = GlES3Comparator::eLEqual;
		DepthFuncs[uint32_t( Castor3D::DepthFunc::eGreater )] = GlES3Comparator::eGreater;
		DepthFuncs[uint32_t( Castor3D::DepthFunc::eNEqual )] = GlES3Comparator::eNEqual;
		DepthFuncs[uint32_t( Castor3D::DepthFunc::eGEqual )] = GlES3Comparator::eGEqual;
		DepthFuncs[uint32_t( Castor3D::DepthFunc::eAlways )] = GlES3Comparator::eAlways;

		WriteMasks[uint32_t( Castor3D::WritingMask::eZero )] = false;
		WriteMasks[uint32_t( Castor3D::WritingMask::eAll )] = true;

		StencilFuncs[uint32_t( Castor3D::StencilFunc::eNever )] = GlES3Comparator::eNever;
		StencilFuncs[uint32_t( Castor3D::StencilFunc::eLess )] = GlES3Comparator::eLess;
		StencilFuncs[uint32_t( Castor3D::StencilFunc::eEqual )] = GlES3Comparator::eLEqual;
		StencilFuncs[uint32_t( Castor3D::StencilFunc::eLEqual )] = GlES3Comparator::eEqual;
		StencilFuncs[uint32_t( Castor3D::StencilFunc::eGreater )] = GlES3Comparator::eNEqual;
		StencilFuncs[uint32_t( Castor3D::StencilFunc::eNEqual )] = GlES3Comparator::eGEqual;
		StencilFuncs[uint32_t( Castor3D::StencilFunc::eGEqual )] = GlES3Comparator::eGreater;
		StencilFuncs[uint32_t( Castor3D::StencilFunc::eAlways )] = GlES3Comparator::eAlways;

		StencilOps[uint32_t( Castor3D::StencilOp::eKeep )] = GlES3StencilOp::eKeep;
		StencilOps[uint32_t( Castor3D::StencilOp::eZero )] = GlES3StencilOp::eZero;
		StencilOps[uint32_t( Castor3D::StencilOp::eReplace )] = GlES3StencilOp::eReplace;
		StencilOps[uint32_t( Castor3D::StencilOp::eIncrement )] = GlES3StencilOp::eIncrement;
		StencilOps[uint32_t( Castor3D::StencilOp::eIncrWrap )] = GlES3StencilOp::eIncrementWrap;
		StencilOps[uint32_t( Castor3D::StencilOp::eDecrement )] = GlES3StencilOp::eDecrement;
		StencilOps[uint32_t( Castor3D::StencilOp::eDecrWrap )] = GlES3StencilOp::eDecrementWrap;
		StencilOps[uint32_t( Castor3D::StencilOp::eInvert )] = GlES3StencilOp::eInvert;

		BuffersTA[GlES3AttachmentPoint::eNone] = GlES3BufferBinding::eNone;
		BuffersTA[GlES3AttachmentPoint::eColour0] = GlES3BufferBinding::eColor0;
		BuffersTA[GlES3AttachmentPoint::eDepth] = GlES3BufferBinding::eDepth;
		BuffersTA[GlES3AttachmentPoint::eStencil] = GlES3BufferBinding::eStencil;

		Queries[uint32_t( Castor3D::QueryType::eTimeElapsed )] = GlES3QueryType::eTimeElapsed;
		Queries[uint32_t( Castor3D::QueryType::eSamplesPassed )] = GlES3QueryType::eSamplesPassed;
		Queries[uint32_t( Castor3D::QueryType::eAnySamplesPassed )] = GlES3QueryType::eAnySamplesPassed;
		Queries[uint32_t( Castor3D::QueryType::ePrimitivesGenerated )] = GlES3QueryType::ePrimitivesGenerated;
		Queries[uint32_t( Castor3D::QueryType::eTransformFeedbackPrimitivesWritten )] = GlES3QueryType::eTransformFeedbackPrimitivesWritten;
		Queries[uint32_t( Castor3D::QueryType::eAnySamplesPassedConservative )] = GlES3QueryType::eAnySamplesPassedConservative;
		Queries[uint32_t( Castor3D::QueryType::eTimestamp )] = GlES3QueryType::eTimestamp;

		QueryInfos[uint32_t( Castor3D::QueryInfo::eResult )] = GlES3QueryInfo::eResult;
		QueryInfos[uint32_t( Castor3D::QueryInfo::eResultAvailable )] = GlES3QueryInfo::eResultAvailable;
		QueryInfos[uint32_t( Castor3D::QueryInfo::eResultNoWait )] = GlES3QueryInfo::eResultNoWait;

		TextureStorages[uint32_t( Castor3D::TextureStorageType::eBuffer )] = GlES3TextureStorageType::eBuffer;
		TextureStorages[uint32_t( Castor3D::TextureStorageType::eOneDimension )] = GlES3TextureStorageType::e1D;
		TextureStorages[uint32_t( Castor3D::TextureStorageType::eOneDimensionArray )] = GlES3TextureStorageType::e2D;
		TextureStorages[uint32_t( Castor3D::TextureStorageType::eTwoDimensions )] = GlES3TextureStorageType::e2D;
		TextureStorages[uint32_t( Castor3D::TextureStorageType::eTwoDimensionsArray )] = GlES3TextureStorageType::e2DArray;
		TextureStorages[uint32_t( Castor3D::TextureStorageType::eTwoDimensionsMS )] = GlES3TextureStorageType::e2DMS;
		TextureStorages[uint32_t( Castor3D::TextureStorageType::eThreeDimensions )] = GlES3TextureStorageType::e3D;
		TextureStorages[uint32_t( Castor3D::TextureStorageType::eCubeMap )] = GlES3TextureStorageType::eCubeMap;
		TextureStorages[uint32_t( Castor3D::TextureStorageType::eCubeMapArray )] = GlES3TextureStorageType::eCubeMapArray;

		CubeMapFaces[uint32_t( Castor3D::CubeMapFace::ePositiveX )] = GlES3TexDim::ePositiveX;
		CubeMapFaces[uint32_t( Castor3D::CubeMapFace::eNegativeX )] = GlES3TexDim::eNegativeX;
		CubeMapFaces[uint32_t( Castor3D::CubeMapFace::ePositiveY )] = GlES3TexDim::ePositiveY;
		CubeMapFaces[uint32_t( Castor3D::CubeMapFace::eNegativeY )] = GlES3TexDim::eNegativeY;
		CubeMapFaces[uint32_t( Castor3D::CubeMapFace::ePositiveZ )] = GlES3TexDim::ePositiveZ;
		CubeMapFaces[uint32_t( Castor3D::CubeMapFace::eNegativeZ )] = GlES3TexDim::eNegativeZ;

		ComparisonModes[uint32_t( Castor3D::ComparisonMode::eNone )] = GlES3CompareMode::eNone;
		ComparisonModes[uint32_t( Castor3D::ComparisonMode::eRefToTexture )] = GlES3CompareMode::eRefToTexture;

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

	OpenGlES3::~OpenGlES3()
	{
	}

	bool OpenGlES3::PreInitialise( String const & p_strExtensions )
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
			m_gpu = GlES3Provider::eNvidia;
		}
		else if ( l_vendor.find( "ati" ) != String::npos
				  || l_vendor.find( "amd" ) != String::npos )
		{
			m_gpu = GlES3Provider::eATI;
		}
		else if ( l_vendor.find( "intel" ) != String::npos )
		{
			m_gpu = GlES3Provider::eIntel;
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

	bool OpenGlES3::Initialise()
	{
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
		gl_api::GetFunction( m_pfnBlendEquation, cuT( "glBlendEquation" ), cuT( "EXT" ) );
		gl_api::GetFunction( m_pfnBlendFuncSeparate, cuT( "glBlendFuncSeparate" ), cuT( "EXT" ) );
		gl_api::GetFunction( m_pfnBlendColor, cuT( "glBlendColor" ), cuT( "EXT" ) );
		gl_api::GetFunction( m_pfnStencilOpSeparate, cuT( "glStencilOpSeparate" ), cuT( "ATI" ) );
		gl_api::GetFunction( m_pfnStencilFuncSeparate, cuT( "glStencilFuncSeparate" ), cuT( "ATI" ) );
		gl_api::GetFunction( m_pfnStencilMaskSeparate, cuT( "glStencilMaskSeparate" ), cuT( "" ) );
		gl_api::GetFunction( m_pfnActiveTexture, cuT( "glActiveTexture" ), cuT( "ARB" ) );
		gl_api::GetFunction( m_pfnClientActiveTexture, cuT( "glClientActiveTexture" ), cuT( "ARB" ) );

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
			gl_api::GetFunction( m_pfnGetIntegerui64v, cuT( "glGetIntegerui64v" ), cuT( "NV" ) );
		}

		m_bBindVboToGpuAddress = HasExtension( NV_shader_buffer_load ) && HasExtension( NV_vertex_buffer_unified_memory );

		if ( m_iVersion >= 40 || HasExtension( ARB_draw_buffers_blend ) )
		{
			gl_api::GetFunction( m_pfnBlendEquationi, cuT( "glBlendEquationi" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnBlendFuncSeparatei, cuT( "glBlendFuncSeparatei" ), cuT( "ARB" ) );
		}
		else if ( HasExtension( AMD_draw_buffers_blend ) )
		{
			gl_api::GetFunction( cuT( "glBlendEquationIndexedAMD" ), m_pfnBlendEquationi );
			gl_api::GetFunction( cuT( "glBlendFuncIndexedAMD" ), m_pfnBlendFuncSeparatei );
		}

		m_bHasNonPowerOfTwoTextures = HasExtension( ARB_texture_non_power_of_two );

		if ( HasExtension( ARB_texture_multisample ) )
		{
			gl_api::GetFunction( m_pfnTexImage2DMultisample, cuT( "glTexImage2DMultisample" ), cuT( "" ) );
			gl_api::GetFunction( m_pfnSampleCoverage, cuT( "glSampleCoverage" ), cuT( "ARB" ) );
		}

		if ( HasExtension( ARB_imaging ) )
		{
		}

		if ( HasExtension( ARB_vertex_buffer_object ) )
		{
			m_bHasVbo = true;
			gl_api::GetFunction( m_pfnGenBuffers, cuT( "glGenBuffers" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnDeleteBuffers, cuT( "glDeleteBuffers" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnIsBuffer, cuT( "glIsBuffer" ), cuT( "ARB" ) );

			if ( HasExtension( ARB_pixel_buffer_object ) )
			{
				m_bHasPbo = true;
			}
		}

		if ( HasExtension( ARB_transform_feedback2 ) )
		{
			gl_api::GetFunction( m_pfnGenTransformFeedbacks, cuT( "glGenTransformFeedbacks" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnDeleteTransformFeedbacks, cuT( "glDeleteTransformFeedbacks" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnBindTransformFeedback, cuT( "glBindTransformFeedback" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnIsTransformFeedback, cuT( "glIsTransformFeedback" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnBeginTransformFeedback, cuT( "glBeginTransformFeedback" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnPauseTransformFeedback, cuT( "glPauseTransformFeedback" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnResumeTransformFeedback, cuT( "glResumeTransformFeedback" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnEndTransformFeedback, cuT( "glEndTransformFeedback" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnTransformFeedbackVaryings, cuT( "glTransformFeedbackVaryings" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnDrawTransformFeedback, cuT( "glDrawTransformFeedback" ), cuT( "ARB" ) );
		}

		if ( HasExtension( ARB_vertex_array_object ) )
		{
			m_bHasVao = true;
			gl_api::GetFunction( m_pfnGenVertexArrays, cuT( "glGenVertexArrays" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnBindVertexArray, cuT( "glBindVertexArray" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnIsVertexArray, cuT( "glIsVertexArray" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnDeleteVertexArrays, cuT( "glDeleteVertexArrays" ), cuT( "ARB" ) );
		}

		if ( HasExtension( ARB_texture_buffer_object ) )
		{
			m_bHasTbo = true;
			gl_api::GetFunction( m_pfnTexBuffer, cuT( "glTexBuffer" ), cuT( "EXT" ) );
		}

		if ( HasExtension( ARB_framebuffer_object ) )
		{
			m_bHasFbo = true;
			gl_api::GetFunction( m_pfnDrawBuffers, cuT( "glDrawBuffers" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnBlitFramebuffer, cuT( "glBlitFramebuffer" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnGenRenderbuffers, cuT( "glGenRenderbuffers" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnDeleteRenderbuffers, cuT( "glDeleteRenderbuffers" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnIsRenderbuffer, cuT( "glIsRenderbuffer" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnBindRenderbuffer, cuT( "glBindRenderbuffer" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnRenderbufferStorage, cuT( "glRenderbufferStorage" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnRenderbufferStorageMultisample, cuT( "glRenderbufferStorageMultisample" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnGetRenderbufferParameteriv, cuT( "glGetRenderbufferParameteriv" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnFramebufferRenderbuffer, cuT( "glFramebufferRenderbuffer" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnGenFramebuffers, cuT( "glGenFramebuffers" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnDeleteFramebuffers, cuT( "glDeleteFramebuffers" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnIsFramebuffer, cuT( "glIsFramebuffer" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnBindFramebuffer, cuT( "glBindFramebuffer" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnFramebufferTexture, cuT( "glFramebufferTexture" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnFramebufferTexture1D, cuT( "glFramebufferTexture1D" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnFramebufferTexture2D, cuT( "glFramebufferTexture2D" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnFramebufferTexture3D, cuT( "glFramebufferTexture3D" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnFramebufferTextureLayer, cuT( "glFramebufferTextureLayer" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnCheckFramebufferStatus, cuT( "glCheckFramebufferStatus" ), cuT( "ARB" ) );
		}
		else if ( HasExtension( EXT_framebuffer_object ) )
		{
			m_bHasFbo = true;
			gl_api::GetFunction( m_pfnDrawBuffers, cuT( "glDrawBuffers" ), cuT( "EXT" ) );
			gl_api::GetFunction( m_pfnBlitFramebuffer, cuT( "glBlitFramebuffer" ), cuT( "EXT" ) );
			gl_api::GetFunction( m_pfnGenRenderbuffers, cuT( "glGenRenderbuffers" ), cuT( "EXT" ) );
			gl_api::GetFunction( m_pfnDeleteRenderbuffers, cuT( "glDeleteRenderbuffers" ), cuT( "EXT" ) );
			gl_api::GetFunction( m_pfnIsRenderbuffer, cuT( "glIsRenderbuffer" ), cuT( "EXT" ) );
			gl_api::GetFunction( m_pfnBindRenderbuffer, cuT( "glBindRenderbuffer" ), cuT( "EXT" ) );
			gl_api::GetFunction( m_pfnRenderbufferStorage, cuT( "glRenderbufferStorage" ), cuT( "EXT" ) );
			gl_api::GetFunction( m_pfnRenderbufferStorageMultisample, cuT( "glRenderbufferStorageMultisample" ), cuT( "EXT" ) );
			gl_api::GetFunction( m_pfnGetRenderbufferParameteriv, cuT( "glGetRenderbufferParameteriv" ), cuT( "EXT" ) );
			gl_api::GetFunction( m_pfnFramebufferRenderbuffer, cuT( "glFramebufferRenderbuffer" ), cuT( "EXT" ) );
			gl_api::GetFunction( m_pfnGenFramebuffers, cuT( "glGenFramebuffers" ), cuT( "EXT" ) );
			gl_api::GetFunction( m_pfnDeleteFramebuffers, cuT( "glDeleteFramebuffers" ), cuT( "EXT" ) );
			gl_api::GetFunction( m_pfnIsFramebuffer, cuT( "glIsFramebuffer" ), cuT( "EXT" ) );
			gl_api::GetFunction( m_pfnBindFramebuffer, cuT( "glBindFramebuffer" ), cuT( "EXT" ) );
			gl_api::GetFunction( m_pfnFramebufferTexture, cuT( "glFramebufferTexture" ), cuT( "EXT" ) );
			gl_api::GetFunction( m_pfnFramebufferTexture1D, cuT( "glFramebufferTexture1D" ), cuT( "EXT" ) );
			gl_api::GetFunction( m_pfnFramebufferTexture2D, cuT( "glFramebufferTexture2D" ), cuT( "EXT" ) );
			gl_api::GetFunction( m_pfnFramebufferTexture3D, cuT( "glFramebufferTexture3D" ), cuT( "EXT" ) );
			gl_api::GetFunction( m_pfnFramebufferTextureLayer, cuT( "glFramebufferTextureLayer" ), cuT( "EXT" ) );
			gl_api::GetFunction( m_pfnCheckFramebufferStatus, cuT( "glCheckFramebufferStatus" ), cuT( "EXT" ) );
		}

		if ( HasExtension( ARB_sampler_objects ) )
		{
			m_bHasSpl = true;
			gl_api::GetFunction( m_pfnGenSamplers, cuT( "glGenSamplers" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnDeleteSamplers, cuT( "glDeleteSamplers" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnIsSampler, cuT( "glIsSampler" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnBindSampler, cuT( "glBindSampler" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnGetSamplerParameteruiv, cuT( "glGetSamplerParameterIuiv" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnGetSamplerParameterfv, cuT( "glGetSamplerParameterfv" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnGetSamplerParameteriv, cuT( "glGetSamplerParameteriv" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnSamplerParameteruiv, cuT( "glSamplerParameterIuiv" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnSamplerParameterf, cuT( "glSamplerParameterf" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnSamplerParameterfv, cuT( "glSamplerParameterfv" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnSamplerParameteri, cuT( "glSamplerParameteri" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnSamplerParameteriv, cuT( "glSamplerParameteriv" ), cuT( "ARB" ) );

			if ( HasExtension( EXT_texture_filter_anisotropic ) )
			{
				m_bHasAnisotropic = true;
			}
		}
		else if ( HasExtension( EXT_sampler_objects ) )
		{
			m_bHasSpl = true;
			gl_api::GetFunction( m_pfnGenSamplers, cuT( "glGenSamplers" ), cuT( "EXT" ) );
			gl_api::GetFunction( m_pfnDeleteSamplers, cuT( "glDeleteSamplers" ), cuT( "EXT" ) );
			gl_api::GetFunction( m_pfnIsSampler, cuT( "glIsSampler" ), cuT( "EXT" ) );
			gl_api::GetFunction( m_pfnBindSampler, cuT( "glBindSampler" ), cuT( "EXT" ) );
			gl_api::GetFunction( m_pfnGetSamplerParameteruiv, cuT( "glGetSamplerParameterIuiv" ), cuT( "EXT" ) );
			gl_api::GetFunction( m_pfnGetSamplerParameterfv, cuT( "glGetSamplerParameterfv" ), cuT( "EXT" ) );
			gl_api::GetFunction( m_pfnGetSamplerParameteriv, cuT( "glGetSamplerParameteriv" ), cuT( "EXT" ) );
			gl_api::GetFunction( m_pfnSamplerParameteruiv, cuT( "glSamplerParameterIuiv" ), cuT( "EXT" ) );
			gl_api::GetFunction( m_pfnSamplerParameterf, cuT( "glSamplerParameterf" ), cuT( "EXT" ) );
			gl_api::GetFunction( m_pfnSamplerParameterfv, cuT( "glSamplerParameterfv" ), cuT( "EXT" ) );
			gl_api::GetFunction( m_pfnSamplerParameteri, cuT( "glSamplerParameteri" ), cuT( "EXT" ) );
			gl_api::GetFunction( m_pfnSamplerParameteriv, cuT( "glSamplerParameteriv" ), cuT( "EXT" ) );

			if ( HasExtension( EXT_texture_filter_anisotropic ) )
			{
				m_bHasAnisotropic = true;
			}
		}

		if ( HasExtension( ARB_draw_instanced ) )
		{
			m_bHasInstancedDraw = true;
			gl_api::GetFunction( m_pfnDrawArraysInstanced, cuT( "glDrawArraysInstanced" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnDrawElementsInstanced, cuT( "glDrawElementsInstanced" ), cuT( "ARB" ) );

			if ( HasExtension( ARB_instanced_arrays ) )
			{
				m_bHasInstancedArrays = true;
				gl_api::GetFunction( m_pfnVertexAttribDivisor, cuT( "glVertexAttribDivisor" ), cuT( "ARB" ) );
			}
		}
		else if ( HasExtension( EXT_draw_instanced ) )
		{
			m_bHasInstancedDraw = true;
			gl_api::GetFunction( m_pfnDrawArraysInstanced, cuT( "glDrawArraysInstanced" ), cuT( "EXT" ) );
			gl_api::GetFunction( m_pfnDrawElementsInstanced, cuT( "glDrawElementsInstanced" ), cuT( "EXT" ) );

			if ( HasExtension( EXT_instanced_arrays ) )
			{
				m_bHasInstancedArrays = true;
				gl_api::GetFunction( m_pfnVertexAttribDivisor, cuT( "glVertexAttribDivisor" ), cuT( "EXT" ) );
			}
		}

		if ( HasExtension( ARB_vertex_program ) )
		{
			m_bHasVSh = true;
			gl_api::GetFunction( m_pfnCreateShader, cuT( "glCreateShader" ), cuT( "" ) );
			gl_api::GetFunction( m_pfnDeleteShader, cuT( "glDeleteShader" ), cuT( "" ) );
			gl_api::GetFunction( m_pfnIsShader, cuT( "glIsShader" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnAttachShader, cuT( "glAttachShader" ), cuT( "" ) );
			gl_api::GetFunction( m_pfnDetachShader, cuT( "glDetachShader" ), cuT( "" ) );
			gl_api::GetFunction( m_pfnCompileShader, cuT( "glCompileShader" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnGetShaderiv, cuT( "glGetShaderiv" ), cuT( "" ) );
			gl_api::GetFunction( m_pfnGetShaderInfoLog, cuT( "glGetShaderInfoLog" ), cuT( "" ) );
			gl_api::GetFunction( m_pfnShaderSource, cuT( "glShaderSource" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnCreateProgram, cuT( "glCreateProgram" ), cuT( "" ) );
			gl_api::GetFunction( m_pfnDeleteProgram, cuT( "glDeleteProgram" ), cuT( "" ) );
			gl_api::GetFunction( m_pfnLinkProgram, cuT( "glLinkProgram" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnUseProgram, cuT( "glUseProgram" ), cuT( "" ) );
			gl_api::GetFunction( m_pfnGetProgramiv, cuT( "glGetProgramiv" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnGetProgramInfoLog, cuT( "glGetProgramInfoLog" ), cuT( "" ) );
			gl_api::GetFunction( m_pfnGetAttribLocation, cuT( "glGetAttribLocation" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnIsProgram, cuT( "glIsProgram" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnEnableVertexAttribArray, cuT( "glEnableVertexAttribArray" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnVertexAttribPointer, cuT( "glVertexAttribPointer" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnVertexAttribIPointer, cuT( "glVertexAttribIPointer" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnDisableVertexAttribArray, cuT( "glDisableVertexAttribArray" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnProgramParameteri, cuT( "glProgramParameteri" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnGetUniformLocation, cuT( "glGetUniformLocation" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnGetActiveAttrib, cuT( "glGetActiveAttrib" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnGetActiveUniform, cuT( "glGetActiveUniform" ), cuT( "ARB" ) );

			if ( HasExtension( ARB_fragment_program ) )
			{
				m_bHasPSh = true;
				gl_api::GetFunction( m_pfnUniform1i, cuT( "glUniform1i" ), cuT( "ARB" ) );
				gl_api::GetFunction( m_pfnUniform2i, cuT( "glUniform2i" ), cuT( "ARB" ) );
				gl_api::GetFunction( m_pfnUniform3i, cuT( "glUniform3i" ), cuT( "ARB" ) );
				gl_api::GetFunction( m_pfnUniform4i, cuT( "glUniform4i" ), cuT( "ARB" ) );
				gl_api::GetFunction( m_pfnUniform1iv, cuT( "glUniform1iv" ), cuT( "ARB" ) );
				gl_api::GetFunction( m_pfnUniform2iv, cuT( "glUniform2iv" ), cuT( "ARB" ) );
				gl_api::GetFunction( m_pfnUniform3iv, cuT( "glUniform3iv" ), cuT( "ARB" ) );
				gl_api::GetFunction( m_pfnUniform4iv, cuT( "glUniform4iv" ), cuT( "ARB" ) );
				gl_api::GetFunction( m_pfnUniform1ui, cuT( "glUniform1ui" ), cuT( "EXT" ) );
				gl_api::GetFunction( m_pfnUniform2ui, cuT( "glUniform2ui" ), cuT( "EXT" ) );
				gl_api::GetFunction( m_pfnUniform3ui, cuT( "glUniform3ui" ), cuT( "EXT" ) );
				gl_api::GetFunction( m_pfnUniform4ui, cuT( "glUniform4ui" ), cuT( "EXT" ) );
				gl_api::GetFunction( m_pfnUniform1uiv, cuT( "glUniform1uiv" ), cuT( "EXT" ) );
				gl_api::GetFunction( m_pfnUniform2uiv, cuT( "glUniform2uiv" ), cuT( "EXT" ) );
				gl_api::GetFunction( m_pfnUniform3uiv, cuT( "glUniform3uiv" ), cuT( "EXT" ) );
				gl_api::GetFunction( m_pfnUniform4uiv, cuT( "glUniform4uiv" ), cuT( "EXT" ) );
				gl_api::GetFunction( m_pfnUniform1f, cuT( "glUniform1f" ), cuT( "ARB" ) );
				gl_api::GetFunction( m_pfnUniform2f, cuT( "glUniform2f" ), cuT( "ARB" ) );
				gl_api::GetFunction( m_pfnUniform3f, cuT( "glUniform3f" ), cuT( "ARB" ) );
				gl_api::GetFunction( m_pfnUniform4f, cuT( "glUniform4f" ), cuT( "ARB" ) );
				gl_api::GetFunction( m_pfnUniform1fv, cuT( "glUniform1fv" ), cuT( "ARB" ) );
				gl_api::GetFunction( m_pfnUniform2fv, cuT( "glUniform2fv" ), cuT( "ARB" ) );
				gl_api::GetFunction( m_pfnUniform3fv, cuT( "glUniform3fv" ), cuT( "ARB" ) );
				gl_api::GetFunction( m_pfnUniform4fv, cuT( "glUniform4fv" ), cuT( "ARB" ) );
				gl_api::GetFunction( m_pfnUniform1d, cuT( "glUniform1d" ), cuT( "" ) );
				gl_api::GetFunction( m_pfnUniform2d, cuT( "glUniform2d" ), cuT( "" ) );
				gl_api::GetFunction( m_pfnUniform3d, cuT( "glUniform3d" ), cuT( "" ) );
				gl_api::GetFunction( m_pfnUniform4d, cuT( "glUniform4d" ), cuT( "" ) );
				gl_api::GetFunction( m_pfnUniform1dv, cuT( "glUniform1dv" ), cuT( "" ) );
				gl_api::GetFunction( m_pfnUniform2dv, cuT( "glUniform2dv" ), cuT( "" ) );
				gl_api::GetFunction( m_pfnUniform3dv, cuT( "glUniform3dv" ), cuT( "" ) );
				gl_api::GetFunction( m_pfnUniform4dv, cuT( "glUniform4dv" ), cuT( "" ) );
				gl_api::GetFunction( m_pfnUniformMatrix2fv, cuT( "glUniformMatrix2fv" ), cuT( "ARB" ) );
				gl_api::GetFunction( m_pfnUniformMatrix2x3fv, cuT( "glUniformMatrix2x3fv" ), cuT( "" ) );
				gl_api::GetFunction( m_pfnUniformMatrix2x4fv, cuT( "glUniformMatrix2x4fv" ), cuT( "" ) );
				gl_api::GetFunction( m_pfnUniformMatrix3fv, cuT( "glUniformMatrix3fv" ), cuT( "ARB" ) );
				gl_api::GetFunction( m_pfnUniformMatrix3x2fv, cuT( "glUniformMatrix3x2fv" ), cuT( "" ) );
				gl_api::GetFunction( m_pfnUniformMatrix3x4fv, cuT( "glUniformMatrix3x4fv" ), cuT( "" ) );
				gl_api::GetFunction( m_pfnUniformMatrix4fv, cuT( "glUniformMatrix4fv" ), cuT( "ARB" ) );
				gl_api::GetFunction( m_pfnUniformMatrix4x2fv, cuT( "glUniformMatrix4x2fv" ), cuT( "" ) );
				gl_api::GetFunction( m_pfnUniformMatrix4x3fv, cuT( "glUniformMatrix4x3fv" ), cuT( "" ) );
				gl_api::GetFunction( m_pfnUniformMatrix2dv, cuT( "glUniformMatrix2dv" ), cuT( "" ) );
				gl_api::GetFunction( m_pfnUniformMatrix2x3dv, cuT( "glUniformMatrix2x3dv" ), cuT( "" ) );
				gl_api::GetFunction( m_pfnUniformMatrix2x4dv, cuT( "glUniformMatrix2x4dv" ), cuT( "" ) );
				gl_api::GetFunction( m_pfnUniformMatrix3dv, cuT( "glUniformMatrix3dv" ), cuT( "" ) );
				gl_api::GetFunction( m_pfnUniformMatrix3x2dv, cuT( "glUniformMatrix3x2dv" ), cuT( "" ) );
				gl_api::GetFunction( m_pfnUniformMatrix3x4dv, cuT( "glUniformMatrix3x4dv" ), cuT( "" ) );
				gl_api::GetFunction( m_pfnUniformMatrix4dv, cuT( "glUniformMatrix4dv" ), cuT( "" ) );
				gl_api::GetFunction( m_pfnUniformMatrix4x2dv, cuT( "glUniformMatrix4x2dv" ), cuT( "" ) );
				gl_api::GetFunction( m_pfnUniformMatrix4x3dv, cuT( "glUniformMatrix4x3dv" ), cuT( "" ) );

				if ( HasExtension( ARB_uniform_buffer_object ) )
				{
					m_bHasUbo = m_iGlslVersion >= 140;
					gl_api::GetFunction( m_pfnGetUniformBlockIndex, cuT( "glGetUniformBlockIndex" ), cuT( "ARB" ) );
					gl_api::GetFunction( m_pfnBindBufferBase, cuT( "glBindBufferBase" ), cuT( "ARB" ) );
					gl_api::GetFunction( m_pfnUniformBlockBinding, cuT( "glUniformBlockBinding" ), cuT( "ARB" ) );
					gl_api::GetFunction( m_pfnGetUniformIndices, cuT( "glGetUniformIndices" ), cuT( "ARB" ) );
					gl_api::GetFunction( m_pfnGetActiveUniformsiv, cuT( "glGetActiveUniformsiv" ), cuT( "ARB" ) );
					gl_api::GetFunction( m_pfnGetActiveUniformBlockiv, cuT( "glGetActiveUniformBlockiv" ), cuT( "ARB" ) );
				}
				else if ( HasExtension( EXT_uniform_buffer_object ) )
				{
					m_bHasUbo = m_iGlslVersion >= 140;
					gl_api::GetFunction( m_pfnGetUniformBlockIndex, cuT( "glGetUniformBlockIndex" ), cuT( "EXT" ) );
					gl_api::GetFunction( m_pfnBindBufferBase, cuT( "glBindBufferBase" ), cuT( "EXT" ) );
					gl_api::GetFunction( m_pfnUniformBlockBinding, cuT( "glUniformBlockBinding" ), cuT( "EXT" ) );
					gl_api::GetFunction( m_pfnGetUniformIndices, cuT( "glGetUniformIndices" ), cuT( "EXT" ) );
					gl_api::GetFunction( m_pfnGetActiveUniformsiv, cuT( "glGetActiveUniformsiv" ), cuT( "EXT" ) );
					gl_api::GetFunction( m_pfnGetActiveUniformBlockiv, cuT( "glGetActiveUniformBlockiv" ), cuT( "EXT" ) );
				}

				if ( HasExtension( ARB_geometry_shader4 ) || HasExtension( EXT_geometry_shader4 ) )
				{
					m_bHasGSh = true;
				}

				if ( HasExtension( ARB_tessellation_shader ) )
				{
					m_bHasTSh = true;
					gl_api::GetFunction( m_pfnPatchParameteri, cuT( "glPatchParameteri" ), cuT( "ARB" ) );
				}

				if ( HasExtension( ARB_compute_shader ) )
				{
					m_bHasCSh = true;
					gl_api::GetFunction( m_pfnDispatchCompute, cuT( "glDispatchCompute" ), cuT( "ARB" ) );

					if ( HasExtension( ARB_compute_variable_group_size ) )
					{
						m_bHasComputeVariableGroupSize = true;
						gl_api::GetFunction( m_pfnDispatchComputeGroupSize, cuT( "glDispatchComputeGroupSize" ), cuT( "ARB" ) );
					}
				}
			}
		}

		HasExtension( ARB_timer_query );
		gl_api::GetFunction( m_pfnGenQueries, cuT( "glGenQueries" ), cuT( "ARB" ) );
		gl_api::GetFunction( m_pfnDeleteQueries, cuT( "glDeleteQueries" ), cuT( "ARB" ) );
		gl_api::GetFunction( m_pfnIsQuery, cuT( "glIsQuery" ), cuT( "ARB" ) );
		gl_api::GetFunction( m_pfnBeginQuery, cuT( "glBeginQuery" ), cuT( "ARB" ) );
		gl_api::GetFunction( m_pfnEndQuery, cuT( "glEndQuery" ), cuT( "ARB" ) );
		gl_api::GetFunction( m_pfnQueryCounter, cuT( "glQueryCounter" ), cuT( "ARB" ) );
		gl_api::GetFunction( m_pfnGetQueryObjectiv, cuT( "glGetQueryObjectiv" ), cuT( "ARB" ) );
		gl_api::GetFunction( m_pfnGetQueryObjectuiv, cuT( "glGetQueryObjectuiv" ), cuT( "ARB" ) );
		gl_api::GetFunction( m_pfnGetQueryObjecti64v, cuT( "glGetQueryObjecti64v" ), cuT( "ARB" ) );
		gl_api::GetFunction( m_pfnGetQueryObjectui64v, cuT( "glGetQueryObjectui64v" ), cuT( "ARB" ) );

		if ( HasExtension( ARB_program_interface_query ) )
		{
			gl_api::GetFunction( m_pfnGetProgramInterfaceiv, cuT( "glGetProgramInterfaceiv" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnGetProgramResourceIndex, cuT( "glGetProgramResourceIndex" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnGetProgramResourceLocation, cuT( "glGetProgramResourceLocation" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnGetProgramResourceLocationIndex, cuT( "glGetProgramResourceLocationIndex" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnGetProgramResourceName, cuT( "glGetProgramResourceName" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnGetProgramResourceiv, cuT( "glGetProgramResourceiv" ), cuT( "ARB" ) );
		}

		if ( HasExtension( ARB_texture_storage ) )
		{
			gl_api::GetFunction( m_pfnTexStorage1D, cuT( "glTexStorage1D" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnTexStorage2D, cuT( "glTexStorage2D" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnTexStorage3D, cuT( "glTexStorage3D" ), cuT( "ARB" ) );
		}

		if ( HasExtension( ARB_texture_storage_multisample ) )
		{
			gl_api::GetFunction( m_pfnTexStorage2DMultisample, cuT( "glTexStorage2DMultisample" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnTexStorage3DMultisample, cuT( "glTexStorage3DMultisample" ), cuT( "ARB" ) );
		}

		if ( HasExtension( ARB_shader_storage_buffer_object ) )
		{
			m_bHasSsbo = true;
			gl_api::GetFunction( m_pfnShaderStorageBlockBinding, cuT( "glShaderStorageBlockBinding" ), cuT( "ARB" ) );
		}

		if ( HasExtension( ARB_shader_image_load_store ) )
		{
			gl_api::GetFunction( m_pfnMemoryBarrier, cuT( "glMemoryBarrier" ), cuT( "ARB" ) );
			gl_api::GetFunction( m_pfnMemoryBarrierByRegion, cuT( "glMemoryBarrierByRegion" ), cuT( "ARB" ) );
			
		}

		return true;
	}

	void OpenGlES3::InitialiseDebug()
	{
		m_debug.Initialise();
	}

	void OpenGlES3::Cleanup()
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
		m_bHasComputeVariableGroupSize = false;
		m_bHasNonPowerOfTwoTextures = false;
		m_bBindVboToGpuAddress = false;
		m_iGlslVersion = 0;
		m_iVersion = 0;
	}

	bool OpenGlES3::GlES3CheckError( std::string const & p_text )const
	{
		return m_debug.GlES3CheckError( p_text );
	}

	bool OpenGlES3::GlES3CheckError( std::wstring const & p_text )const
	{
		return m_debug.GlES3CheckError( p_text );
	}

	void OpenGlES3::DisplayExtensions()const
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
