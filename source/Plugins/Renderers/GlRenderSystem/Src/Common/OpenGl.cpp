#include "Common/OpenGl.hpp"

#include "Render/GlRenderSystem.hpp"

#include <Graphics/Rectangle.hpp>
#include <Log/Logger.hpp>
#include <Miscellaneous/Utils.hpp>

#include <GlslWriter.hpp>

#include <iomanip>

using namespace castor3d;
using namespace castor;

#define DEF_USE_DIRECT_STATE_ACCESS 0

namespace GlRender
{
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
		gl_api::getFunction( m_pfnTexImage3D, cuT( "glTexImage3D" ), cuT( "EXT" ) );
		gl_api::getFunction( m_pfnTexSubImage3D, cuT( "glTexSubImage3D" ), cuT( "EXT" ) );
		gl_api::getFunction( m_pfnGenerateMipmap, cuT( "glGenerateMipmap" ), cuT( "EXT" ) );
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
		gl_api::getFunction( m_pfnTextureSubImage1D, cuT( "glTextureSubImage1D" ), cuT( "EXT" ) );
		gl_api::getFunction( m_pfnTextureSubImage2D, cuT( "glTextureSubImage2D" ), cuT( "EXT" ) );
		gl_api::getFunction( m_pfnTextureSubImage3D, cuT( "glTextureSubImage3D" ), cuT( "EXT" ) );
		gl_api::getFunction( m_pfnTextureImage1D, cuT( "glTextureImage1D" ), cuT( "EXT" ) );
		gl_api::getFunction( m_pfnTextureImage2D, cuT( "glTextureImage2D" ), cuT( "EXT" ) );
		gl_api::getFunction( m_pfnTextureImage3D, cuT( "glTextureImage3D" ), cuT( "EXT" ) );
		gl_api::getFunction( m_pfnGetTextureImage, cuT( "glGetTextureImage" ), cuT( "EXT" ) );
		gl_api::getFunction( m_pfnGenerateMipmap, cuT( "glGenerateTextureMipmap" ), cuT( "EXT" ) );
	}

	//*************************************************************************************************

	BufFunctionsBase::BufFunctionsBase( OpenGl & p_gl )
		: Holder( p_gl )
	{
		if ( p_gl.hasExtension( NV_shader_buffer_load ) )
		{
			gl_api::getFunction( m_pfnMakeBufferResident, cuT( "glMakeBufferResident" ), cuT( "NV" ) );
			gl_api::getFunction( m_pfnMakeBufferNonResident, cuT( "glMakeBufferNonResident" ), cuT( "NV" ) );
			gl_api::getFunction( m_pfnIsBufferResident, cuT( "glIsBufferResident" ), cuT( "NV" ) );
			gl_api::getFunction( m_pfnMakeNamedBufferResident, cuT( "glMakeNamedBufferResident" ), cuT( "NV" ) );
			gl_api::getFunction( m_pfnMakeNamedBufferNonResident, cuT( "glMakeNamedBufferNonResident" ), cuT( "NV" ) );
			gl_api::getFunction( m_pfnIsNamedBufferResident, cuT( "glIsNamedBufferResident" ), cuT( "NV" ) );
			gl_api::getFunction( m_pfnGetBufferParameterui64v, cuT( "glGetBufferParameterui64v" ), cuT( "NV" ) );
			gl_api::getFunction( m_pfnGetNamedBufferParameterui64v, cuT( "glGetNamedBufferParameterui64v" ), cuT( "NV" ) );
		}

		if ( p_gl.hasExtension( NV_vertex_buffer_unified_memory ) )
		{
			gl_api::getFunction( m_pfnBufferAddressRange, cuT( "glBufferAddressRange" ), cuT( "NV" ) );
			gl_api::getFunction( m_pfnVertexFormat, cuT( "glVertexFormat" ), cuT( "NV" ) );
			gl_api::getFunction( m_pfnNormalFormat, cuT( "glNormalFormat" ), cuT( "NV" ) );
			gl_api::getFunction( m_pfnColorFormat, cuT( "glColorFormat" ), cuT( "NV" ) );
			gl_api::getFunction( m_pfnIndexFormat, cuT( "glIndexFormat" ), cuT( "NV" ) );
			gl_api::getFunction( m_pfnTexCoordFormat, cuT( "glTexCoordFormat" ), cuT( "NV" ) );
			gl_api::getFunction( m_pfnEdgeFlagFormat, cuT( "glEdgeFlagFormat" ), cuT( "NV" ) );
			gl_api::getFunction( m_pfnSecondaryColorFormat, cuT( "glSecondaryColorFormat" ), cuT( "NV" ) );
			gl_api::getFunction( m_pfnFogCoordFormat, cuT( "glFogCoordFormat" ), cuT( "NV" ) );
			gl_api::getFunction( m_pfnVertexAttribFormat, cuT( "glVertexAttribFormat" ), cuT( "NV" ) );
			gl_api::getFunction( m_pfnVertexAttribIFormat, cuT( "glVertexAttribIFormat" ), cuT( "NV" ) );
		}
	}

	//*************************************************************************************************

	BufFunctions::BufFunctions( OpenGl & p_gl )
		: BufFunctionsBase( p_gl )
	{
		gl_api::getFunction( m_pfnBindBuffer, cuT( "glBindBuffer" ), cuT( "ARB" ) );
		gl_api::getFunction( m_pfnBufferData, cuT( "glBufferData" ), cuT( "ARB" ) );
		gl_api::getFunction( m_pfnBufferSubData, cuT( "glBufferSubData" ), cuT( "ARB" ) );
		gl_api::getFunction( m_pfnCopyBufferSubData, cuT( "glCopyBufferSubData" ), cuT( "ARB" ) );
		gl_api::getFunction( m_pfnGetBufferParameteriv, cuT( "glGetBufferParameteriv" ), cuT( "ARB" ) );
		gl_api::getFunction( m_pfnMapBuffer, cuT( "glMapBuffer" ), cuT( "ARB" ) );
		gl_api::getFunction( m_pfnUnmapBuffer, cuT( "glUnmapBuffer" ), cuT( "ARB" ) );
		gl_api::getFunction( m_pfnMapBufferRange, cuT( "glMapBufferRange" ), cuT( "" ) );
		gl_api::getFunction( m_pfnFlushMappedBufferRange, cuT( "glFlushMappedBufferRange" ), cuT( "" ) );
	}

	//*************************************************************************************************

	BufFunctionsDSA::BufFunctionsDSA( OpenGl & p_gl )
		: BufFunctionsBase( p_gl )
		, m_uiBuffer( 0 )
	{
		gl_api::getFunction( m_pfnNamedBufferData, cuT( "glNamedBufferData" ), cuT( "EXT" ) );
		gl_api::getFunction( m_pfnNamedBufferSubData, cuT( "glNamedBufferSubData" ), cuT( "EXT" ) );
		gl_api::getFunction( m_pfnCopyNamedBufferSubData, cuT( "glCopyNamedBufferSubData" ), cuT( "ARB" ) );
		gl_api::getFunction( m_pfnGetNamedBufferParameteriv, cuT( "glGetNamedBufferParameteriv" ), cuT( "EXT" ) );
		gl_api::getFunction( m_pfnMapNamedBuffer, cuT( "glMapNamedBuffer" ), cuT( "EXT" ) );
		gl_api::getFunction( m_pfnUnmapNamedBuffer, cuT( "glUnmapNamedBuffer" ), cuT( "EXT" ) );
		gl_api::getFunction( m_pfnMapNamedBufferRange, cuT( "glMapNamedBufferRange" ), cuT( "EXT" ) );
		gl_api::getFunction( m_pfnFlushMappedNamedBufferRange, cuT( "glFlushMappedNamedBufferRange" ), cuT( "EXT" ) );
	}

	//*************************************************************************************************

#define CASTOR_DBG_WIN32 0

	OpenGl::OpenGl( GlRenderSystem & renderSystem )
		: m_pfnReadPixels{}
		, m_pfnBlitFramebuffer{}
		, m_pfnTexImage2DMultisample{}
		, m_pfnGetActiveUniform{}
		, m_pfnVertexAttribPointer{}
		, m_renderSystem{ renderSystem }
		, m_debug{ renderSystem }
	{
		uint32_t index = 0;
		GlslStrings[index++] = cuT( "[e00] GLSL is not available!" );
		GlslStrings[index++] = cuT( "[e01] Not a valid program object!" );
		GlslStrings[index++] = cuT( "[e02] Not a valid object!" );
		GlslStrings[index++] = cuT( "[e03] Out of memory!" );
		GlslStrings[index++] = cuT( "[e04] Unknown compiler error!" );
		GlslStrings[index++] = cuT( "[e05] Linker log is not available!" );
		GlslStrings[index++] = cuT( "[e06] Compiler log is not available!" );
		GlslStrings[index] = cuT( "[Empty]" );

		PrimitiveTypes[uint32_t( castor3d::Topology::ePoints )] = GlTopology::ePoints;
		PrimitiveTypes[uint32_t( castor3d::Topology::eLines )] = GlTopology::eLines;
		PrimitiveTypes[uint32_t( castor3d::Topology::eLineLoop )] = GlTopology::eLineLoop;
		PrimitiveTypes[uint32_t( castor3d::Topology::eLineStrip )] = GlTopology::eLineStrip;
		PrimitiveTypes[uint32_t( castor3d::Topology::eTriangles )] = GlTopology::eTriangles;
		PrimitiveTypes[uint32_t( castor3d::Topology::eTriangleStrips )] = GlTopology::eTriangleStrip;
		PrimitiveTypes[uint32_t( castor3d::Topology::eTriangleFan )] = GlTopology::eTriangleFan;
		PrimitiveTypes[uint32_t( castor3d::Topology::eQuads )] = GlTopology::eQuads;
		PrimitiveTypes[uint32_t( castor3d::Topology::eQuadStrips )] = GlTopology::eQuadStrip;
		PrimitiveTypes[uint32_t( castor3d::Topology::ePolygon )] = GlTopology::ePolygon;

		TextureDimensions[uint32_t( castor3d::TextureType::eBuffer )] = GlTexDim::eBuffer;
		TextureDimensions[uint32_t( castor3d::TextureType::eOneDimension )] = GlTexDim::e1D;
		TextureDimensions[uint32_t( castor3d::TextureType::eOneDimensionArray )] = GlTexDim::e1DArray;
		TextureDimensions[uint32_t( castor3d::TextureType::eTwoDimensions )] = GlTexDim::e2D;
		TextureDimensions[uint32_t( castor3d::TextureType::eTwoDimensionsArray )] = GlTexDim::e2DArray;
		TextureDimensions[uint32_t( castor3d::TextureType::eTwoDimensionsMS )] = GlTexDim::e2DMS;
		TextureDimensions[uint32_t( castor3d::TextureType::eTwoDimensionsMSArray )] = GlTexDim::e2DMSArray;
		TextureDimensions[uint32_t( castor3d::TextureType::eThreeDimensions )] = GlTexDim::e3D;
		TextureDimensions[uint32_t( castor3d::TextureType::eCube )] = GlTexDim::eCube;
		TextureDimensions[uint32_t( castor3d::TextureType::eCubeArray )] = GlTexDim::eCubeArray;

		AlphaFuncs[uint32_t( castor3d::ComparisonFunc::eAlways )] = GlComparator::eAlways;
		AlphaFuncs[uint32_t( castor3d::ComparisonFunc::eLess )] = GlComparator::eLess;
		AlphaFuncs[uint32_t( castor3d::ComparisonFunc::eLEqual )] = GlComparator::eLEqual;
		AlphaFuncs[uint32_t( castor3d::ComparisonFunc::eEqual )] = GlComparator::eEqual;
		AlphaFuncs[uint32_t( castor3d::ComparisonFunc::eNEqual )] = GlComparator::eNEqual;
		AlphaFuncs[uint32_t( castor3d::ComparisonFunc::eGEqual )] = GlComparator::eGEqual;
		AlphaFuncs[uint32_t( castor3d::ComparisonFunc::eGreater )] = GlComparator::eGreater;
		AlphaFuncs[uint32_t( castor3d::ComparisonFunc::eNever )] = GlComparator::eNever;

		TextureWrapMode[uint32_t( castor3d::WrapMode::eRepeat )] = GlWrapMode::eRepeat;
		TextureWrapMode[uint32_t( castor3d::WrapMode::eMirroredRepeat )] = GlWrapMode::eMirroredRepeat;
		TextureWrapMode[uint32_t( castor3d::WrapMode::eClampToBorder )] = GlWrapMode::eClampToBorder;
		TextureWrapMode[uint32_t( castor3d::WrapMode::eClampToEdge )] = GlWrapMode::eClampToEdge;

		TextureInterpolation[uint32_t( castor3d::InterpolationMode::eUndefined )] = GlInterpolationMode::eNearest;
		TextureInterpolation[uint32_t( castor3d::InterpolationMode::eNearest )] = GlInterpolationMode::eNearest;
		TextureInterpolation[uint32_t( castor3d::InterpolationMode::eLinear )] = GlInterpolationMode::eLinear;

		BlendFactors[uint32_t( castor3d::BlendOperand::eZero )] = GlBlendFactor::eZero;
		BlendFactors[uint32_t( castor3d::BlendOperand::eOne )] = GlBlendFactor::eOne;
		BlendFactors[uint32_t( castor3d::BlendOperand::eSrcColour )] = GlBlendFactor::eSrcColour;
		BlendFactors[uint32_t( castor3d::BlendOperand::eInvSrcColour )] = GlBlendFactor::eInvSrcColour;
		BlendFactors[uint32_t( castor3d::BlendOperand::eDstColour )] = GlBlendFactor::eDstColour;
		BlendFactors[uint32_t( castor3d::BlendOperand::eInvDstColour )] = GlBlendFactor::eInvDstColour;
		BlendFactors[uint32_t( castor3d::BlendOperand::eSrcAlpha )] = GlBlendFactor::eSrcAlpha;
		BlendFactors[uint32_t( castor3d::BlendOperand::eInvSrcAlpha )] = GlBlendFactor::eInvSrcAlpha;
		BlendFactors[uint32_t( castor3d::BlendOperand::eDstAlpha )] = GlBlendFactor::eDstAlpha;
		BlendFactors[uint32_t( castor3d::BlendOperand::eInvDstAlpha )] = GlBlendFactor::eInvDstAlpha;
		BlendFactors[uint32_t( castor3d::BlendOperand::eConstant )] = GlBlendFactor::eConstant;
		BlendFactors[uint32_t( castor3d::BlendOperand::eInvConstant )] = GlBlendFactor::eInvConstant;
		BlendFactors[uint32_t( castor3d::BlendOperand::eSrcAlphaSaturate )] = GlBlendFactor::eSrcAlphaSaturate;
		BlendFactors[uint32_t( castor3d::BlendOperand::eSrc1Colour )] = GlBlendFactor::eSrc1Colour;
		BlendFactors[uint32_t( castor3d::BlendOperand::eInvSrc1Colour )] = GlBlendFactor::eInvSrc1Colour;
		BlendFactors[uint32_t( castor3d::BlendOperand::eSrc1Alpha )] = GlBlendFactor::eSrc1Alpha;
		BlendFactors[uint32_t( castor3d::BlendOperand::eInvSrc1Alpha )] = GlBlendFactor::eInvSrc1Alpha;

		TextureArguments[uint32_t( castor3d::BlendSource::eTexture )] = GlBlendSource::eTexture;
		TextureArguments[uint32_t( castor3d::BlendSource::eTexture0 )] = GlBlendSource::eTexture0;
		TextureArguments[uint32_t( castor3d::BlendSource::eTexture1 )] = GlBlendSource::eTexture1;
		TextureArguments[uint32_t( castor3d::BlendSource::eTexture2 )] = GlBlendSource::eTexture2;
		TextureArguments[uint32_t( castor3d::BlendSource::eTexture3 )] = GlBlendSource::eTexture3;
		TextureArguments[uint32_t( castor3d::BlendSource::eConstant )] = GlBlendSource::eConstant;
		TextureArguments[uint32_t( castor3d::BlendSource::eDiffuse )] = GlBlendSource::eColour;
		TextureArguments[uint32_t( castor3d::BlendSource::ePrevious )] = GlBlendSource::ePrevious;

		TextureBlendFuncs[uint32_t( castor3d::TextureBlendFunc::eNoBlend )] = GlBlendFunc::eModulate;
		TextureBlendFuncs[uint32_t( castor3d::TextureBlendFunc::eFirstArg )] = GlBlendFunc::eReplace;
		TextureBlendFuncs[uint32_t( castor3d::TextureBlendFunc::eAdd )] = GlBlendFunc::eAdd;
		TextureBlendFuncs[uint32_t( castor3d::TextureBlendFunc::eAddSigned )] = GlBlendFunc::eAddSigned;
		TextureBlendFuncs[uint32_t( castor3d::TextureBlendFunc::eModulate )] = GlBlendFunc::eModulate;
		TextureBlendFuncs[uint32_t( castor3d::TextureBlendFunc::eInterpolate )] = GlBlendFunc::eInterpolate;
		TextureBlendFuncs[uint32_t( castor3d::TextureBlendFunc::eSubtract )] = GlBlendFunc::eSubtract;

		BlendOps[uint32_t( castor3d::BlendOperation::eAdd )] = GlBlendOp::eAdd;
		BlendOps[uint32_t( castor3d::BlendOperation::eSubtract )] = GlBlendOp::eSubtract;
		BlendOps[uint32_t( castor3d::BlendOperation::eRevSubtract )] = GlBlendOp::eRevSubtract;
		BlendOps[uint32_t( castor3d::BlendOperation::eMin )] = GlBlendOp::eMin;
		BlendOps[uint32_t( castor3d::BlendOperation::eMax )] = GlBlendOp::eMax;

		PixelFormats[uint32_t( castor::PixelFormat::eL8 )] = PixelFmt( GlFormat::eRed, GlInternal::eR8, GlType::eUnsignedByte );
		PixelFormats[uint32_t( castor::PixelFormat::eL16F32F )] = PixelFmt( GlFormat::eRed, GlInternal::eR16F, GlType::eFloat );
		PixelFormats[uint32_t( castor::PixelFormat::eL32F )] = PixelFmt( GlFormat::eRed, GlInternal::eR32F, GlType::eFloat );
		PixelFormats[uint32_t( castor::PixelFormat::eA8L8 )] = PixelFmt( GlFormat::eRG, GlInternal::eRG16, GlType::eUnsignedByte );
		PixelFormats[uint32_t( castor::PixelFormat::eAL16F32F )] = PixelFmt( GlFormat::eRG, GlInternal::eRG16F, GlType::eFloat );
		PixelFormats[uint32_t( castor::PixelFormat::eAL32F )] = PixelFmt( GlFormat::eRG, GlInternal::eRG32F, GlType::eFloat );
		PixelFormats[uint32_t( castor::PixelFormat::eA1R5G5B5 )] = PixelFmt( GlFormat::eRGBA, GlInternal::eRGB5A1, GlType::eUnsignedShort1555Rev );
		PixelFormats[uint32_t( castor::PixelFormat::eA4R4G4B4 )] = PixelFmt( GlFormat::eRGBA, GlInternal::eRGBA4, GlType::eUnsignedShort4444Rev );
		PixelFormats[uint32_t( castor::PixelFormat::eR5G6B5 )] = PixelFmt( GlFormat::eRGB, GlInternal::eRGB16, GlType::eUnsignedShort565Rev );
		PixelFormats[uint32_t( castor::PixelFormat::eR8G8B8 )] = PixelFmt( GlFormat::eRGB, GlInternal::eRGB8, GlType::eUnsignedByte );
		PixelFormats[uint32_t( castor::PixelFormat::eB8G8R8 )] = PixelFmt( GlFormat::eBGR, GlInternal::eRGB8, GlType::eUnsignedByte );
		PixelFormats[uint32_t( castor::PixelFormat::eA8R8G8B8 )] = PixelFmt( GlFormat::eRGBA, GlInternal::eRGBA8, GlType::eUnsignedByte );
		PixelFormats[uint32_t( castor::PixelFormat::eA8B8G8R8 )] = PixelFmt( GlFormat::eBGRA, GlInternal::eRGBA8, GlType::eUnsignedByte );
		PixelFormats[uint32_t( castor::PixelFormat::eRGB16F )] = PixelFmt( GlFormat::eRGB, GlInternal::eRGB16F, GlType::eHalfFloat );
		PixelFormats[uint32_t( castor::PixelFormat::eRGBA16F )] = PixelFmt( GlFormat::eRGBA, GlInternal::eRGBA16F, GlType::eHalfFloat );
		PixelFormats[uint32_t( castor::PixelFormat::eRGB16F32F )] = PixelFmt( GlFormat::eRGB, GlInternal::eRGB16F, GlType::eFloat );
		PixelFormats[uint32_t( castor::PixelFormat::eRGBA16F32F )] = PixelFmt( GlFormat::eRGBA, GlInternal::eRGBA16F, GlType::eFloat );
		PixelFormats[uint32_t( castor::PixelFormat::eRGB32F )] = PixelFmt( GlFormat::eRGB, GlInternal::eRGB32F, GlType::eFloat );
		PixelFormats[uint32_t( castor::PixelFormat::eRGBA32F )] = PixelFmt( GlFormat::eRGB, GlInternal::eRGBA32F, GlType::eFloat );
		PixelFormats[uint32_t( castor::PixelFormat::eDXTC1 )] = PixelFmt( GlFormat::eRGB, GlInternal::eDXT1, GlType::eDefault );
		PixelFormats[uint32_t( castor::PixelFormat::eDXTC3 )] = PixelFmt( GlFormat::eRGBA, GlInternal::eDXT3, GlType::eDefault );
		PixelFormats[uint32_t( castor::PixelFormat::eDXTC5 )] = PixelFmt( GlFormat::eRGBA, GlInternal::eDXT5, GlType::eDefault );
		PixelFormats[uint32_t( castor::PixelFormat::eYUY2 )] = PixelFmt( GlFormat( 0 ), GlInternal( 0 ), GlType::eDefault );
		PixelFormats[uint32_t( castor::PixelFormat::eD16 )] = PixelFmt( GlFormat::eDepth, GlInternal::eD16, GlType::eUnsignedByte );
		PixelFormats[uint32_t( castor::PixelFormat::eD24 )] = PixelFmt( GlFormat::eDepth, GlInternal::eD24, GlType::eUnsignedByte );
		PixelFormats[uint32_t( castor::PixelFormat::eD24S8 )] = PixelFmt( GlFormat::eDepthStencil, GlInternal::eD24S8, GlType::eUnsignedInt248 );
		PixelFormats[uint32_t( castor::PixelFormat::eD32 )] = PixelFmt( GlFormat::eDepth, GlInternal::eD32, GlType::eUnsignedByte );
		PixelFormats[uint32_t( castor::PixelFormat::eD32F )] = PixelFmt( GlFormat::eDepth, GlInternal::eD32, GlType::eFloat );
		PixelFormats[uint32_t( castor::PixelFormat::eD32FS8 )] = PixelFmt( GlFormat::eDepthStencil, GlInternal::eD32FS8, GlType::eFloat );
		PixelFormats[uint32_t( castor::PixelFormat::eS1 )] = PixelFmt( GlFormat::eStencil, GlInternal::eS1, GlType::eUnsignedByte );
		PixelFormats[uint32_t( castor::PixelFormat::eS8 )] = PixelFmt( GlFormat::eStencil, GlInternal::eS8, GlType::eUnsignedByte );

		ShaderTypes[uint32_t( castor3d::ShaderType::eVertex )] = GlShaderType::eVertex;
		ShaderTypes[uint32_t( castor3d::ShaderType::eHull )] = GlShaderType::eTessControl;
		ShaderTypes[uint32_t( castor3d::ShaderType::eDomain )] = GlShaderType::eTessEvaluation;
		ShaderTypes[uint32_t( castor3d::ShaderType::eGeometry )] = GlShaderType::eGeometry;
		ShaderTypes[uint32_t( castor3d::ShaderType::ePixel )] = GlShaderType::eFragment;
		ShaderTypes[uint32_t( castor3d::ShaderType::eCompute )] = GlShaderType::eCompute;

		Internals[uint32_t( castor::PixelFormat::eL8 )] = GlInternal::eR16;
		Internals[uint32_t( castor::PixelFormat::eL16F32F )] = GlInternal::eR16F;
		Internals[uint32_t( castor::PixelFormat::eL32F )] = GlInternal::eR32F;
		Internals[uint32_t( castor::PixelFormat::eA8L8 )] = GlInternal::eRG16;
		Internals[uint32_t( castor::PixelFormat::eAL16F32F )] = GlInternal::eRG16F;
		Internals[uint32_t( castor::PixelFormat::eAL32F )] = GlInternal::eRG32F;
		Internals[uint32_t( castor::PixelFormat::eA1R5G5B5 )] = GlInternal::eRGBA16UI;
		Internals[uint32_t( castor::PixelFormat::eA4R4G4B4 )] = GlInternal::eRGBA16UI;
		Internals[uint32_t( castor::PixelFormat::eR5G6B5 )] = GlInternal::eRGB16UI;
		Internals[uint32_t( castor::PixelFormat::eR8G8B8 )] = GlInternal::eRGBA32UI;
		Internals[uint32_t( castor::PixelFormat::eB8G8R8 )] = GlInternal::eRGBA32UI;
		Internals[uint32_t( castor::PixelFormat::eA8R8G8B8 )] = GlInternal::eRGBA32UI;
		Internals[uint32_t( castor::PixelFormat::eA8B8G8R8 )] = GlInternal::eRGBA32UI;
		Internals[uint32_t( castor::PixelFormat::eRGB16F )] = GlInternal::eRGBA16F;
		Internals[uint32_t( castor::PixelFormat::eRGBA16F )] = GlInternal::eRGBA16F;
		Internals[uint32_t( castor::PixelFormat::eRGB16F32F )] = GlInternal::eRGBA16F;
		Internals[uint32_t( castor::PixelFormat::eRGBA16F32F )] = GlInternal::eRGBA16F;
		Internals[uint32_t( castor::PixelFormat::eRGB32F )] = GlInternal::eRGBA32F;
		Internals[uint32_t( castor::PixelFormat::eRGBA32F )] = GlInternal::eRGBA32F;

		Attachments[uint32_t( castor3d::AttachmentPoint::eNone )] = GlAttachmentPoint::eNone;
		Attachments[uint32_t( castor3d::AttachmentPoint::eColour )] = GlAttachmentPoint::eColour0;
		Attachments[uint32_t( castor3d::AttachmentPoint::eDepth )] = GlAttachmentPoint::eDepth;
		Attachments[uint32_t( castor3d::AttachmentPoint::eStencil )] = GlAttachmentPoint::eStencil;
		Attachments[uint32_t( castor3d::AttachmentPoint::eDepthStencil )] = GlAttachmentPoint::eDepthStencil;

		FramebufferModes[uint32_t( castor3d::FrameBufferTarget::eDraw )] = GlFrameBufferMode::eDraw;
		FramebufferModes[uint32_t( castor3d::FrameBufferTarget::eRead )] = GlFrameBufferMode::eRead;
		FramebufferModes[uint32_t( castor3d::FrameBufferTarget::eBoth )] = GlFrameBufferMode::eDefault;

		RboStorages[uint32_t( castor::PixelFormat::eL8 )] = GlInternal::eL8;
		RboStorages[uint32_t( castor::PixelFormat::eL16F32F )] = GlInternal::eR16F;
		RboStorages[uint32_t( castor::PixelFormat::eL32F )] = GlInternal::eR32F;
		RboStorages[uint32_t( castor::PixelFormat::eA8L8 )] = GlInternal::eL8A8;
		RboStorages[uint32_t( castor::PixelFormat::eAL16F32F )] = GlInternal::eRG16F;
		RboStorages[uint32_t( castor::PixelFormat::eAL32F )] = GlInternal::eRG32F;
		RboStorages[uint32_t( castor::PixelFormat::eA1R5G5B5 )] = GlInternal::eRGB5A1;
		RboStorages[uint32_t( castor::PixelFormat::eA4R4G4B4 )] = GlInternal::eRGBA4;
		RboStorages[uint32_t( castor::PixelFormat::eR5G6B5 )] = GlInternal::eRGB565;
		RboStorages[uint32_t( castor::PixelFormat::eR8G8B8 )] = GlInternal::eRGB8;
		RboStorages[uint32_t( castor::PixelFormat::eB8G8R8 )] = GlInternal::eRGB8;
		RboStorages[uint32_t( castor::PixelFormat::eA8R8G8B8 )] = GlInternal::eRGBA8;
		RboStorages[uint32_t( castor::PixelFormat::eA8B8G8R8 )] = GlInternal::eRGBA8;
		RboStorages[uint32_t( castor::PixelFormat::eRGB16F )] = GlInternal::eRGB16F;
		RboStorages[uint32_t( castor::PixelFormat::eRGBA16F )] = GlInternal::eRGBA16F;
		RboStorages[uint32_t( castor::PixelFormat::eRGB16F32F )] = GlInternal::eRGB16F;
		RboStorages[uint32_t( castor::PixelFormat::eRGBA16F32F )] = GlInternal::eRGBA16F;
		RboStorages[uint32_t( castor::PixelFormat::eRGB32F )] = GlInternal::eRGB32F;
		RboStorages[uint32_t( castor::PixelFormat::eRGBA32F )] = GlInternal::eRGBA32F;
		RboStorages[uint32_t( castor::PixelFormat::eDXTC1 )] = GlInternal::eDXT1;
		RboStorages[uint32_t( castor::PixelFormat::eDXTC3 )] = GlInternal::eDXT3;
		RboStorages[uint32_t( castor::PixelFormat::eDXTC5 )] = GlInternal::eDXT5;
		RboStorages[uint32_t( castor::PixelFormat::eYUY2 )] = GlInternal( 0 );
		RboStorages[uint32_t( castor::PixelFormat::eD16 )] = GlInternal::eD16;
		RboStorages[uint32_t( castor::PixelFormat::eD24 )] = GlInternal::eD24;
		RboStorages[uint32_t( castor::PixelFormat::eD24S8 )] = GlInternal::eD24S8;
		RboStorages[uint32_t( castor::PixelFormat::eD32 )] = GlInternal::eD32;
		RboStorages[uint32_t( castor::PixelFormat::eD32F )] = GlInternal::eD32F;
		RboStorages[uint32_t( castor::PixelFormat::eD32FS8 )] = GlInternal::eD32FS8;
		RboStorages[uint32_t( castor::PixelFormat::eS1 )] = GlInternal::eS1;
		RboStorages[uint32_t( castor::PixelFormat::eS8 )] = GlInternal::eS8;

		Buffers[uint32_t( castor3d::WindowBuffer::eNone )] = GlBufferBinding::eNone;
		Buffers[uint32_t( castor3d::WindowBuffer::eFrontLeft )] = GlBufferBinding::eFrontLeft;
		Buffers[uint32_t( castor3d::WindowBuffer::eFrontRight )] = GlBufferBinding::eFrontRight;
		Buffers[uint32_t( castor3d::WindowBuffer::eBackLeft )] = GlBufferBinding::eBackLeft;
		Buffers[uint32_t( castor3d::WindowBuffer::eBackRight )] = GlBufferBinding::eBackRight;
		Buffers[uint32_t( castor3d::WindowBuffer::eFront )] = GlBufferBinding::eFront;
		Buffers[uint32_t( castor3d::WindowBuffer::eBack )] = GlBufferBinding::eBack;
		Buffers[uint32_t( castor3d::WindowBuffer::eLeft )] = GlBufferBinding::eLeft;
		Buffers[uint32_t( castor3d::WindowBuffer::eRight )] = GlBufferBinding::eRight;
		Buffers[uint32_t( castor3d::WindowBuffer::eFrontAndBack )] = GlBufferBinding::eFrontAndBack;

		Faces[uint32_t( castor3d::Culling::eNone )] = GlFace( 0 );
		Faces[uint32_t( castor3d::Culling::eFront )] = GlFace::eFront;
		Faces[uint32_t( castor3d::Culling::eBack )] = GlFace::eBack;
		Faces[uint32_t( castor3d::Culling::eFrontAndBack )] = GlFace::eBoth;

		FillModes[uint32_t( castor3d::FillMode::ePoint )] = GlFillMode::ePoint;
		FillModes[uint32_t( castor3d::FillMode::eLine )] = GlFillMode::eLine;
		FillModes[uint32_t( castor3d::FillMode::eSolid )] = GlFillMode::eFill;

		DepthFuncs[uint32_t( castor3d::DepthFunc::eNever )] = GlComparator::eNever;
		DepthFuncs[uint32_t( castor3d::DepthFunc::eLess )] = GlComparator::eLess;
		DepthFuncs[uint32_t( castor3d::DepthFunc::eEqual )] = GlComparator::eEqual;
		DepthFuncs[uint32_t( castor3d::DepthFunc::eLEqual )] = GlComparator::eLEqual;
		DepthFuncs[uint32_t( castor3d::DepthFunc::eGreater )] = GlComparator::eGreater;
		DepthFuncs[uint32_t( castor3d::DepthFunc::eNEqual )] = GlComparator::eNEqual;
		DepthFuncs[uint32_t( castor3d::DepthFunc::eGEqual )] = GlComparator::eGEqual;
		DepthFuncs[uint32_t( castor3d::DepthFunc::eAlways )] = GlComparator::eAlways;

		WriteMasks[uint32_t( castor3d::WritingMask::eZero )] = false;
		WriteMasks[uint32_t( castor3d::WritingMask::eAll )] = true;

		StencilFuncs[uint32_t( castor3d::StencilFunc::eNever )] = GlComparator::eNever;
		StencilFuncs[uint32_t( castor3d::StencilFunc::eLess )] = GlComparator::eLess;
		StencilFuncs[uint32_t( castor3d::StencilFunc::eLEqual )] = GlComparator::eLEqual;
		StencilFuncs[uint32_t( castor3d::StencilFunc::eEqual )] = GlComparator::eEqual;
		StencilFuncs[uint32_t( castor3d::StencilFunc::eNEqual )] = GlComparator::eNEqual;
		StencilFuncs[uint32_t( castor3d::StencilFunc::eGEqual )] = GlComparator::eGEqual;
		StencilFuncs[uint32_t( castor3d::StencilFunc::eGreater )] = GlComparator::eGreater;
		StencilFuncs[uint32_t( castor3d::StencilFunc::eAlways )] = GlComparator::eAlways;

		StencilOps[uint32_t( castor3d::StencilOp::eKeep )] = GlStencilOp::eKeep;
		StencilOps[uint32_t( castor3d::StencilOp::eZero )] = GlStencilOp::eZero;
		StencilOps[uint32_t( castor3d::StencilOp::eReplace )] = GlStencilOp::eReplace;
		StencilOps[uint32_t( castor3d::StencilOp::eIncrement )] = GlStencilOp::eIncrement;
		StencilOps[uint32_t( castor3d::StencilOp::eIncrWrap )] = GlStencilOp::eIncrementWrap;
		StencilOps[uint32_t( castor3d::StencilOp::eDecrement )] = GlStencilOp::eDecrement;
		StencilOps[uint32_t( castor3d::StencilOp::eDecrWrap )] = GlStencilOp::eDecrementWrap;
		StencilOps[uint32_t( castor3d::StencilOp::eInvert )] = GlStencilOp::eInvert;

		BuffersTA[GlAttachmentPoint::eNone] = GlBufferBinding::eNone;
		BuffersTA[GlAttachmentPoint::eColour0] = GlBufferBinding::eColor0;
		BuffersTA[GlAttachmentPoint::eDepth] = GlBufferBinding::eDepth;
		BuffersTA[GlAttachmentPoint::eStencil] = GlBufferBinding::eStencil;
		BuffersTA[GlAttachmentPoint::eDepthStencil] = GlBufferBinding::eDepthStencil;

		Queries[uint32_t( castor3d::QueryType::eTimeElapsed )] = GlQueryType::eTimeElapsed;
		Queries[uint32_t( castor3d::QueryType::eSamplesPassed )] = GlQueryType::eSamplesPassed;
		Queries[uint32_t( castor3d::QueryType::eAnySamplesPassed )] = GlQueryType::eAnySamplesPassed;
		Queries[uint32_t( castor3d::QueryType::ePrimitivesGenerated )] = GlQueryType::ePrimitivesGenerated;
		Queries[uint32_t( castor3d::QueryType::eTransformFeedbackPrimitivesWritten )] = GlQueryType::eTransformFeedbackPrimitivesWritten;
		Queries[uint32_t( castor3d::QueryType::eAnySamplesPassedConservative )] = GlQueryType::eAnySamplesPassedConservative;
		Queries[uint32_t( castor3d::QueryType::eTimestamp )] = GlQueryType::eTimestamp;

		QueryInfos[uint32_t( castor3d::QueryInfo::eResult )] = GlQueryInfo::eResult;
		QueryInfos[uint32_t( castor3d::QueryInfo::eResultAvailable )] = GlQueryInfo::eResultAvailable;
		QueryInfos[uint32_t( castor3d::QueryInfo::eResultNoWait )] = GlQueryInfo::eResultNoWait;

		TextureStorages[uint32_t( castor3d::TextureStorageType::eBuffer )] = GlTextureStorageType::eBuffer;
		TextureStorages[uint32_t( castor3d::TextureStorageType::eOneDimension )] = GlTextureStorageType::e1D;
		TextureStorages[uint32_t( castor3d::TextureStorageType::eOneDimensionArray )] = GlTextureStorageType::e2D;
		TextureStorages[uint32_t( castor3d::TextureStorageType::eTwoDimensions )] = GlTextureStorageType::e2D;
		TextureStorages[uint32_t( castor3d::TextureStorageType::eTwoDimensionsArray )] = GlTextureStorageType::e2DArray;
		TextureStorages[uint32_t( castor3d::TextureStorageType::eTwoDimensionsMS )] = GlTextureStorageType::e2DMS;
		TextureStorages[uint32_t( castor3d::TextureStorageType::eThreeDimensions )] = GlTextureStorageType::e3D;
		TextureStorages[uint32_t( castor3d::TextureStorageType::eCubeMap )] = GlTextureStorageType::eCubeMap;
		TextureStorages[uint32_t( castor3d::TextureStorageType::eCubeMapArray )] = GlTextureStorageType::eCubeMapArray;

		CubeMapFaces[uint32_t( castor3d::CubeMapFace::ePositiveX )] = GlTexDim::ePositiveX;
		CubeMapFaces[uint32_t( castor3d::CubeMapFace::eNegativeX )] = GlTexDim::eNegativeX;
		CubeMapFaces[uint32_t( castor3d::CubeMapFace::ePositiveY )] = GlTexDim::ePositiveY;
		CubeMapFaces[uint32_t( castor3d::CubeMapFace::eNegativeY )] = GlTexDim::eNegativeY;
		CubeMapFaces[uint32_t( castor3d::CubeMapFace::ePositiveZ )] = GlTexDim::ePositiveZ;
		CubeMapFaces[uint32_t( castor3d::CubeMapFace::eNegativeZ )] = GlTexDim::eNegativeZ;

		ComparisonModes[uint32_t( castor3d::ComparisonMode::eNone )] = GlCompareMode::eNone;
		ComparisonModes[uint32_t( castor3d::ComparisonMode::eRefToTexture )] = GlCompareMode::eRefToTexture;

		Components[castor3d::AttachmentPoint::eColour] = GlComponent::eColour;
		Components[castor3d::AttachmentPoint::eDepth] = GlComponent::eDepth;
		Components[castor3d::AttachmentPoint::eStencil] = GlComponent::eStencil;

		cleanup();

#if defined( CASTOR_PLATFORM_WINDOWS )

		m_pfnMakeCurrent = &wglMakeCurrent;
		m_pfnSwapBuffers = &::SwapBuffers;
		m_pfnCreateContext = &wglCreateContext;
		m_pfnDeleteContext = &wglDeleteContext;

#elif defined( CASTOR_PLATFORM_LINUX )

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

	bool OpenGl::preInitialise( String const & p_strExtensions )
	{
		char const * extensions = ( char const * )glGetString( GL_EXTENSIONS );

		if ( extensions )
		{
			m_extensions = string::stringCast< xchar >( extensions ) + p_strExtensions;
		}
		else
		{
			m_extensions = p_strExtensions;
		}

		m_vendor = string::stringCast< xchar >( ( char const * )glGetString( GL_VENDOR ) );
		m_renderer = string::stringCast< xchar >( ( char const * )glGetString( GL_RENDERER ) );
		m_version = string::stringCast< xchar >( ( char const * )glGetString( GL_VERSION ) );

		auto vendor = string::lowerCase( m_vendor );

		if ( vendor.find( "nvidia" ) != String::npos )
		{
			m_gpu = GlProvider::eNvidia;
		}
		else if ( vendor.find( "ati" ) != String::npos
				  || vendor.find( "amd" ) != String::npos )
		{
			m_gpu = GlProvider::eATI;
		}
		else if ( vendor.find( "intel" ) != String::npos )
		{
			m_gpu = GlProvider::eIntel;
		}

		double version{ 0u };
		StringStream stream( m_version );
		stream >> version;
		m_iVersion = int( version * 10 );

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

#if defined( CASTOR_PLATFORM_WINDOWS )

		if ( hasExtension( ARB_create_context ) )
		{
			gl_api::getFunction( cuT( "wglCreateContextAttribsARB" ), m_pfnCreateContextAttribs );
		}

#elif defined( CASTOR_PLATFORM_LINUX )

		if ( hasExtension( ARB_create_context ) )
		{
			gl_api::getFunction( cuT( "glXCreateContextAttribsARB" ), m_pfnCreateContextAttribs );
		}
		else
		{
			gl_api::getFunction( cuT( "glXCreateContextAttribsARB" ), m_pfnCreateContextAttribs );
		}

#else

#	error "Yet unsupported OS"

#endif

		return true;
	}

	bool OpenGl::initialise()
	{
		m_pfnClearColor = &glClearColor;
		m_pfnClearDepth = &glClearDepth;
		m_pfnViewport = &glViewport;
		m_pfnClear = &glClear;
		m_pfnDrawArrays = &glDrawArrays;
		m_pfnDrawElements = &glDrawElements;
		m_pfnEnable = &glEnable;
		m_pfnDisable = &glDisable;
		m_pfnEnableTweak = &glEnable;
		m_pfnDisableTweak = &glDisable;
		m_pfnEnableTexDim = &glEnable;
		m_pfnDisableTexDim = &glDisable;
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
		gl_api::getFunction( m_pfnBlendEquation, cuT( "glBlendEquation" ), cuT( "EXT" ) );
		gl_api::getFunction( m_pfnBlendFuncSeparate, cuT( "glBlendFuncSeparate" ), cuT( "EXT" ) );
		gl_api::getFunction( m_pfnBlendColor, cuT( "glBlendColor" ), cuT( "EXT" ) );
		gl_api::getFunction( m_pfnStencilOpSeparate, cuT( "glStencilOpSeparate" ), cuT( "ATI" ) );
		gl_api::getFunction( m_pfnStencilFuncSeparate, cuT( "glStencilFuncSeparate" ), cuT( "ATI" ) );
		gl_api::getFunction( m_pfnStencilMaskSeparate, cuT( "glStencilMaskSeparate" ), cuT( "" ) );
		gl_api::getFunction( m_pfnActiveTexture, cuT( "glActiveTexture" ), cuT( "ARB" ) );
		gl_api::getFunction( m_pfnClientActiveTexture, cuT( "glClientActiveTexture" ), cuT( "ARB" ) );

#if defined( CASTOR_PLATFORM_WINDOWS )

		if ( hasExtension( ARB_create_context ) )
		{
			gl_api::getFunction( cuT( "wglCreateContextAttribsARB" ), m_pfnCreateContextAttribs );
		}

		if ( hasExtension( EXT_swap_control ) )
		{
			gl_api::getFunction( cuT( "wglSwapIntervalEXT" ), m_pfnSwapInterval );
		}

#elif defined( CASTOR_PLATFORM_LINUX )

		if ( hasExtension( ARB_create_context ) )
		{
			gl_api::getFunction( cuT( "glXCreateContextAttribsARB" ), m_pfnCreateContextAttribs );
		}
		else
		{
			gl_api::getFunction( cuT( "glXCreateContextAttribsARB" ), m_pfnCreateContextAttribs );
		}

		if ( hasExtension( EXT_swap_control ) )
		{
			gl_api::getFunction( cuT( "glXSwapIntervalEXT" ), m_pfnSwapInterval );
		}
		else
		{
			gl_api::getFunction( cuT( "glXSwapIntervalEXT" ), m_pfnSwapInterval );
		}

#else

#	error "Yet unsupported OS"

#endif
#if DEF_USE_DIRECT_STATE_ACCESS

		if ( hasExtension( EXT_direct_state_access ) )
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

		if ( hasExtension( NV_shader_buffer_load ) )
		{
			gl_api::getFunction( m_pfnGetIntegerui64v, cuT( "glGetIntegerui64v" ), cuT( "NV" ) );
		}

		m_bBindVboToGpuAddress = hasExtension( NV_shader_buffer_load ) && hasExtension( NV_vertex_buffer_unified_memory );

		if ( m_iVersion >= 40 || hasExtension( ARB_draw_buffers_blend ) )
		{
			gl_api::getFunction( m_pfnBlendEquationi, cuT( "glBlendEquationi" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnBlendFuncSeparatei, cuT( "glBlendFuncSeparatei" ), cuT( "ARB" ) );
		}
		else if ( hasExtension( AMD_draw_buffers_blend ) )
		{
			gl_api::getFunction( cuT( "glBlendEquationIndexedAMD" ), m_pfnBlendEquationi );
			gl_api::getFunction( cuT( "glBlendFuncIndexedAMD" ), m_pfnBlendFuncSeparatei );
		}

		m_bHasNonPowerOfTwoTextures = hasExtension( ARB_texture_non_power_of_two );

		if ( hasExtension( ARB_texture_multisample ) )
		{
			gl_api::getFunction( m_pfnTexImage2DMultisample, cuT( "glTexImage2DMultisample" ), cuT( "" ) );
			gl_api::getFunction( m_pfnSampleCoverage, cuT( "glSampleCoverage" ), cuT( "ARB" ) );
		}

		if ( hasExtension( ARB_imaging ) )
		{
		}

		if ( hasExtension( ARB_vertex_buffer_object ) )
		{
			m_bHasVbo = true;
			gl_api::getFunction( m_pfnGenBuffers, cuT( "glGenBuffers" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnDeleteBuffers, cuT( "glDeleteBuffers" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnIsBuffer, cuT( "glIsBuffer" ), cuT( "ARB" ) );

			if ( hasExtension( ARB_pixel_buffer_object ) )
			{
				m_bHasPbo = true;
			}
		}

		if ( hasExtension( ARB_transform_feedback2 ) )
		{
			gl_api::getFunction( m_pfnGenTransformFeedbacks, cuT( "glGenTransformFeedbacks" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnDeleteTransformFeedbacks, cuT( "glDeleteTransformFeedbacks" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnBindTransformFeedback, cuT( "glBindTransformFeedback" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnIsTransformFeedback, cuT( "glIsTransformFeedback" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnBeginTransformFeedback, cuT( "glBeginTransformFeedback" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnPauseTransformFeedback, cuT( "glPauseTransformFeedback" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnResumeTransformFeedback, cuT( "glResumeTransformFeedback" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnEndTransformFeedback, cuT( "glEndTransformFeedback" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnTransformFeedbackVaryings, cuT( "glTransformFeedbackVaryings" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnDrawTransformFeedback, cuT( "glDrawTransformFeedback" ), cuT( "ARB" ) );
		}

		if ( hasExtension( ARB_vertex_array_object ) )
		{
			m_bHasVao = true;
			gl_api::getFunction( m_pfnGenVertexArrays, cuT( "glGenVertexArrays" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnBindVertexArray, cuT( "glBindVertexArray" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnIsVertexArray, cuT( "glIsVertexArray" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnDeleteVertexArrays, cuT( "glDeleteVertexArrays" ), cuT( "ARB" ) );
		}

		if ( hasExtension( ARB_texture_buffer_object ) )
		{
			m_bHasTbo = true;
			gl_api::getFunction( m_pfnTexBuffer, cuT( "glTexBuffer" ), cuT( "EXT" ) );
		}

		if ( hasExtension( ARB_framebuffer_object ) )
		{
			m_bHasFbo = true;
			gl_api::getFunction( m_pfnDrawBuffers, cuT( "glDrawBuffers" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnBlitFramebuffer, cuT( "glBlitFramebuffer" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnGenRenderbuffers, cuT( "glGenRenderbuffers" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnDeleteRenderbuffers, cuT( "glDeleteRenderbuffers" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnIsRenderbuffer, cuT( "glIsRenderbuffer" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnBindRenderbuffer, cuT( "glBindRenderbuffer" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnRenderbufferStorage, cuT( "glRenderbufferStorage" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnRenderbufferStorageMultisample, cuT( "glRenderbufferStorageMultisample" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnGetRenderbufferParameteriv, cuT( "glGetRenderbufferParameteriv" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnFramebufferRenderbuffer, cuT( "glFramebufferRenderbuffer" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnGenFramebuffers, cuT( "glGenFramebuffers" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnDeleteFramebuffers, cuT( "glDeleteFramebuffers" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnIsFramebuffer, cuT( "glIsFramebuffer" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnBindFramebuffer, cuT( "glBindFramebuffer" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnFramebufferTexture, cuT( "glFramebufferTexture" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnFramebufferTexture1D, cuT( "glFramebufferTexture1D" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnFramebufferTexture2D, cuT( "glFramebufferTexture2D" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnFramebufferTexture3D, cuT( "glFramebufferTexture3D" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnFramebufferTextureLayer, cuT( "glFramebufferTextureLayer" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnCheckFramebufferStatus, cuT( "glCheckFramebufferStatus" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnClearBufferfv, cuT( "glClearBufferfv" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnClearBufferiv, cuT( "glClearBufferiv" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnClearBufferfi, cuT( "glClearBufferfi" ), cuT( "ARB" ) );
		}
		else if ( hasExtension( EXT_framebuffer_object ) )
		{
			m_bHasFbo = true;
			gl_api::getFunction( m_pfnDrawBuffers, cuT( "glDrawBuffers" ), cuT( "EXT" ) );
			gl_api::getFunction( m_pfnBlitFramebuffer, cuT( "glBlitFramebuffer" ), cuT( "EXT" ) );
			gl_api::getFunction( m_pfnGenRenderbuffers, cuT( "glGenRenderbuffers" ), cuT( "EXT" ) );
			gl_api::getFunction( m_pfnDeleteRenderbuffers, cuT( "glDeleteRenderbuffers" ), cuT( "EXT" ) );
			gl_api::getFunction( m_pfnIsRenderbuffer, cuT( "glIsRenderbuffer" ), cuT( "EXT" ) );
			gl_api::getFunction( m_pfnBindRenderbuffer, cuT( "glBindRenderbuffer" ), cuT( "EXT" ) );
			gl_api::getFunction( m_pfnRenderbufferStorage, cuT( "glRenderbufferStorage" ), cuT( "EXT" ) );
			gl_api::getFunction( m_pfnRenderbufferStorageMultisample, cuT( "glRenderbufferStorageMultisample" ), cuT( "EXT" ) );
			gl_api::getFunction( m_pfnGetRenderbufferParameteriv, cuT( "glGetRenderbufferParameteriv" ), cuT( "EXT" ) );
			gl_api::getFunction( m_pfnFramebufferRenderbuffer, cuT( "glFramebufferRenderbuffer" ), cuT( "EXT" ) );
			gl_api::getFunction( m_pfnGenFramebuffers, cuT( "glGenFramebuffers" ), cuT( "EXT" ) );
			gl_api::getFunction( m_pfnDeleteFramebuffers, cuT( "glDeleteFramebuffers" ), cuT( "EXT" ) );
			gl_api::getFunction( m_pfnIsFramebuffer, cuT( "glIsFramebuffer" ), cuT( "EXT" ) );
			gl_api::getFunction( m_pfnBindFramebuffer, cuT( "glBindFramebuffer" ), cuT( "EXT" ) );
			gl_api::getFunction( m_pfnFramebufferTexture, cuT( "glFramebufferTexture" ), cuT( "EXT" ) );
			gl_api::getFunction( m_pfnFramebufferTexture1D, cuT( "glFramebufferTexture1D" ), cuT( "EXT" ) );
			gl_api::getFunction( m_pfnFramebufferTexture2D, cuT( "glFramebufferTexture2D" ), cuT( "EXT" ) );
			gl_api::getFunction( m_pfnFramebufferTexture3D, cuT( "glFramebufferTexture3D" ), cuT( "EXT" ) );
			gl_api::getFunction( m_pfnFramebufferTextureLayer, cuT( "glFramebufferTextureLayer" ), cuT( "EXT" ) );
			gl_api::getFunction( m_pfnCheckFramebufferStatus, cuT( "glCheckFramebufferStatus" ), cuT( "EXT" ) );
			gl_api::getFunction( m_pfnClearBufferfv, cuT( "glClearBufferfv" ), cuT( "EXT" ) );
			gl_api::getFunction( m_pfnClearBufferiv, cuT( "glClearBufferiv" ), cuT( "EXT" ) );
			gl_api::getFunction( m_pfnClearBufferfi, cuT( "glClearBufferfi" ), cuT( "EXT" ) );
		}

		if ( hasExtension( ARB_sampler_objects ) )
		{
			m_bHasSpl = true;
			gl_api::getFunction( m_pfnGenSamplers, cuT( "glGenSamplers" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnDeleteSamplers, cuT( "glDeleteSamplers" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnIsSampler, cuT( "glIsSampler" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnBindSampler, cuT( "glBindSampler" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnGetSamplerParameteruiv, cuT( "glGetSamplerParameterIuiv" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnGetSamplerParameterfv, cuT( "glGetSamplerParameterfv" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnGetSamplerParameteriv, cuT( "glGetSamplerParameteriv" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnSamplerParameteruiv, cuT( "glSamplerParameterIuiv" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnSamplerParameterf, cuT( "glSamplerParameterf" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnSamplerParameterfv, cuT( "glSamplerParameterfv" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnSamplerParameteri, cuT( "glSamplerParameteri" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnSamplerParameteriv, cuT( "glSamplerParameteriv" ), cuT( "ARB" ) );

			if ( hasExtension( EXT_texture_filter_anisotropic ) )
			{
				m_bHasAnisotropic = true;
			}
		}
		else if ( hasExtension( EXT_sampler_objects ) )
		{
			m_bHasSpl = true;
			gl_api::getFunction( m_pfnGenSamplers, cuT( "glGenSamplers" ), cuT( "EXT" ) );
			gl_api::getFunction( m_pfnDeleteSamplers, cuT( "glDeleteSamplers" ), cuT( "EXT" ) );
			gl_api::getFunction( m_pfnIsSampler, cuT( "glIsSampler" ), cuT( "EXT" ) );
			gl_api::getFunction( m_pfnBindSampler, cuT( "glBindSampler" ), cuT( "EXT" ) );
			gl_api::getFunction( m_pfnGetSamplerParameteruiv, cuT( "glGetSamplerParameterIuiv" ), cuT( "EXT" ) );
			gl_api::getFunction( m_pfnGetSamplerParameterfv, cuT( "glGetSamplerParameterfv" ), cuT( "EXT" ) );
			gl_api::getFunction( m_pfnGetSamplerParameteriv, cuT( "glGetSamplerParameteriv" ), cuT( "EXT" ) );
			gl_api::getFunction( m_pfnSamplerParameteruiv, cuT( "glSamplerParameterIuiv" ), cuT( "EXT" ) );
			gl_api::getFunction( m_pfnSamplerParameterf, cuT( "glSamplerParameterf" ), cuT( "EXT" ) );
			gl_api::getFunction( m_pfnSamplerParameterfv, cuT( "glSamplerParameterfv" ), cuT( "EXT" ) );
			gl_api::getFunction( m_pfnSamplerParameteri, cuT( "glSamplerParameteri" ), cuT( "EXT" ) );
			gl_api::getFunction( m_pfnSamplerParameteriv, cuT( "glSamplerParameteriv" ), cuT( "EXT" ) );

			if ( hasExtension( EXT_texture_filter_anisotropic ) )
			{
				m_bHasAnisotropic = true;
			}
		}

		if ( hasExtension( ARB_draw_instanced ) )
		{
			m_bHasInstancedDraw = true;
			gl_api::getFunction( m_pfnDrawArraysInstanced, cuT( "glDrawArraysInstanced" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnDrawElementsInstanced, cuT( "glDrawElementsInstanced" ), cuT( "ARB" ) );

			if ( hasExtension( ARB_instanced_arrays ) )
			{
				m_bHasInstancedArrays = true;
				gl_api::getFunction( m_pfnVertexAttribDivisor, cuT( "glVertexAttribDivisor" ), cuT( "ARB" ) );
			}
		}
		else if ( hasExtension( EXT_draw_instanced ) )
		{
			m_bHasInstancedDraw = true;
			gl_api::getFunction( m_pfnDrawArraysInstanced, cuT( "glDrawArraysInstanced" ), cuT( "EXT" ) );
			gl_api::getFunction( m_pfnDrawElementsInstanced, cuT( "glDrawElementsInstanced" ), cuT( "EXT" ) );

			if ( hasExtension( EXT_instanced_arrays ) )
			{
				m_bHasInstancedArrays = true;
				gl_api::getFunction( m_pfnVertexAttribDivisor, cuT( "glVertexAttribDivisor" ), cuT( "EXT" ) );
			}
		}

		if ( hasExtension( ARB_vertex_program ) )
		{
			m_bHasVSh = true;
			gl_api::getFunction( m_pfnCreateShader, cuT( "glCreateShader" ), cuT( "" ) );
			gl_api::getFunction( m_pfnDeleteShader, cuT( "glDeleteShader" ), cuT( "" ) );
			gl_api::getFunction( m_pfnIsShader, cuT( "glIsShader" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnAttachShader, cuT( "glAttachShader" ), cuT( "" ) );
			gl_api::getFunction( m_pfnDetachShader, cuT( "glDetachShader" ), cuT( "" ) );
			gl_api::getFunction( m_pfnCompileShader, cuT( "glCompileShader" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnGetShaderiv, cuT( "glGetShaderiv" ), cuT( "" ) );
			gl_api::getFunction( m_pfnGetShaderInfoLog, cuT( "glGetShaderInfoLog" ), cuT( "" ) );
			gl_api::getFunction( m_pfnShaderSource, cuT( "glShaderSource" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnCreateProgram, cuT( "glCreateProgram" ), cuT( "" ) );
			gl_api::getFunction( m_pfnDeleteProgram, cuT( "glDeleteProgram" ), cuT( "" ) );
			gl_api::getFunction( m_pfnLinkProgram, cuT( "glLinkProgram" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnUseProgram, cuT( "glUseProgram" ), cuT( "" ) );
			gl_api::getFunction( m_pfnGetProgramiv, cuT( "glGetProgramiv" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnGetProgramInfoLog, cuT( "glGetProgramInfoLog" ), cuT( "" ) );
			gl_api::getFunction( m_pfnGetAttribLocation, cuT( "glGetAttribLocation" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnIsProgram, cuT( "glIsProgram" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnEnableVertexAttribArray, cuT( "glEnableVertexAttribArray" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnVertexAttribPointer, cuT( "glVertexAttribPointer" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnVertexAttribIPointer, cuT( "glVertexAttribIPointer" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnDisableVertexAttribArray, cuT( "glDisableVertexAttribArray" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnProgramParameteri, cuT( "glProgramParameteri" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnGetUniformLocation, cuT( "glGetUniformLocation" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnGetActiveAttrib, cuT( "glGetActiveAttrib" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnGetActiveUniform, cuT( "glGetActiveUniform" ), cuT( "ARB" ) );

			if ( hasExtension( ARB_fragment_program ) )
			{
				m_bHasPSh = true;
				gl_api::getFunction( m_pfnUniform1i, cuT( "glUniform1i" ), cuT( "ARB" ) );
				gl_api::getFunction( m_pfnUniform2i, cuT( "glUniform2i" ), cuT( "ARB" ) );
				gl_api::getFunction( m_pfnUniform3i, cuT( "glUniform3i" ), cuT( "ARB" ) );
				gl_api::getFunction( m_pfnUniform4i, cuT( "glUniform4i" ), cuT( "ARB" ) );
				gl_api::getFunction( m_pfnUniform1iv, cuT( "glUniform1iv" ), cuT( "ARB" ) );
				gl_api::getFunction( m_pfnUniform2iv, cuT( "glUniform2iv" ), cuT( "ARB" ) );
				gl_api::getFunction( m_pfnUniform3iv, cuT( "glUniform3iv" ), cuT( "ARB" ) );
				gl_api::getFunction( m_pfnUniform4iv, cuT( "glUniform4iv" ), cuT( "ARB" ) );
				gl_api::getFunction( m_pfnUniform1ui, cuT( "glUniform1ui" ), cuT( "EXT" ) );
				gl_api::getFunction( m_pfnUniform2ui, cuT( "glUniform2ui" ), cuT( "EXT" ) );
				gl_api::getFunction( m_pfnUniform3ui, cuT( "glUniform3ui" ), cuT( "EXT" ) );
				gl_api::getFunction( m_pfnUniform4ui, cuT( "glUniform4ui" ), cuT( "EXT" ) );
				gl_api::getFunction( m_pfnUniform1uiv, cuT( "glUniform1uiv" ), cuT( "EXT" ) );
				gl_api::getFunction( m_pfnUniform2uiv, cuT( "glUniform2uiv" ), cuT( "EXT" ) );
				gl_api::getFunction( m_pfnUniform3uiv, cuT( "glUniform3uiv" ), cuT( "EXT" ) );
				gl_api::getFunction( m_pfnUniform4uiv, cuT( "glUniform4uiv" ), cuT( "EXT" ) );
				gl_api::getFunction( m_pfnUniform1f, cuT( "glUniform1f" ), cuT( "ARB" ) );
				gl_api::getFunction( m_pfnUniform2f, cuT( "glUniform2f" ), cuT( "ARB" ) );
				gl_api::getFunction( m_pfnUniform3f, cuT( "glUniform3f" ), cuT( "ARB" ) );
				gl_api::getFunction( m_pfnUniform4f, cuT( "glUniform4f" ), cuT( "ARB" ) );
				gl_api::getFunction( m_pfnUniform1fv, cuT( "glUniform1fv" ), cuT( "ARB" ) );
				gl_api::getFunction( m_pfnUniform2fv, cuT( "glUniform2fv" ), cuT( "ARB" ) );
				gl_api::getFunction( m_pfnUniform3fv, cuT( "glUniform3fv" ), cuT( "ARB" ) );
				gl_api::getFunction( m_pfnUniform4fv, cuT( "glUniform4fv" ), cuT( "ARB" ) );
				gl_api::getFunction( m_pfnUniform1d, cuT( "glUniform1d" ), cuT( "" ) );
				gl_api::getFunction( m_pfnUniform2d, cuT( "glUniform2d" ), cuT( "" ) );
				gl_api::getFunction( m_pfnUniform3d, cuT( "glUniform3d" ), cuT( "" ) );
				gl_api::getFunction( m_pfnUniform4d, cuT( "glUniform4d" ), cuT( "" ) );
				gl_api::getFunction( m_pfnUniform1dv, cuT( "glUniform1dv" ), cuT( "" ) );
				gl_api::getFunction( m_pfnUniform2dv, cuT( "glUniform2dv" ), cuT( "" ) );
				gl_api::getFunction( m_pfnUniform3dv, cuT( "glUniform3dv" ), cuT( "" ) );
				gl_api::getFunction( m_pfnUniform4dv, cuT( "glUniform4dv" ), cuT( "" ) );
				gl_api::getFunction( m_pfnUniformMatrix2fv, cuT( "glUniformMatrix2fv" ), cuT( "ARB" ) );
				gl_api::getFunction( m_pfnUniformMatrix2x3fv, cuT( "glUniformMatrix2x3fv" ), cuT( "" ) );
				gl_api::getFunction( m_pfnUniformMatrix2x4fv, cuT( "glUniformMatrix2x4fv" ), cuT( "" ) );
				gl_api::getFunction( m_pfnUniformMatrix3fv, cuT( "glUniformMatrix3fv" ), cuT( "ARB" ) );
				gl_api::getFunction( m_pfnUniformMatrix3x2fv, cuT( "glUniformMatrix3x2fv" ), cuT( "" ) );
				gl_api::getFunction( m_pfnUniformMatrix3x4fv, cuT( "glUniformMatrix3x4fv" ), cuT( "" ) );
				gl_api::getFunction( m_pfnUniformMatrix4fv, cuT( "glUniformMatrix4fv" ), cuT( "ARB" ) );
				gl_api::getFunction( m_pfnUniformMatrix4x2fv, cuT( "glUniformMatrix4x2fv" ), cuT( "" ) );
				gl_api::getFunction( m_pfnUniformMatrix4x3fv, cuT( "glUniformMatrix4x3fv" ), cuT( "" ) );
				gl_api::getFunction( m_pfnUniformMatrix2dv, cuT( "glUniformMatrix2dv" ), cuT( "" ) );
				gl_api::getFunction( m_pfnUniformMatrix2x3dv, cuT( "glUniformMatrix2x3dv" ), cuT( "" ) );
				gl_api::getFunction( m_pfnUniformMatrix2x4dv, cuT( "glUniformMatrix2x4dv" ), cuT( "" ) );
				gl_api::getFunction( m_pfnUniformMatrix3dv, cuT( "glUniformMatrix3dv" ), cuT( "" ) );
				gl_api::getFunction( m_pfnUniformMatrix3x2dv, cuT( "glUniformMatrix3x2dv" ), cuT( "" ) );
				gl_api::getFunction( m_pfnUniformMatrix3x4dv, cuT( "glUniformMatrix3x4dv" ), cuT( "" ) );
				gl_api::getFunction( m_pfnUniformMatrix4dv, cuT( "glUniformMatrix4dv" ), cuT( "" ) );
				gl_api::getFunction( m_pfnUniformMatrix4x2dv, cuT( "glUniformMatrix4x2dv" ), cuT( "" ) );
				gl_api::getFunction( m_pfnUniformMatrix4x3dv, cuT( "glUniformMatrix4x3dv" ), cuT( "" ) );

				if ( hasExtension( ARB_uniform_buffer_object ) )
				{
					m_bHasUbo = m_iGlslVersion >= 140;
					gl_api::getFunction( m_pfnGetUniformBlockIndex, cuT( "glGetUniformBlockIndex" ), cuT( "ARB" ) );
					gl_api::getFunction( m_pfnBindBufferBase, cuT( "glBindBufferBase" ), cuT( "ARB" ) );
					gl_api::getFunction( m_pfnUniformBlockBinding, cuT( "glUniformBlockBinding" ), cuT( "ARB" ) );
					gl_api::getFunction( m_pfnGetUniformIndices, cuT( "glGetUniformIndices" ), cuT( "ARB" ) );
					gl_api::getFunction( m_pfnGetActiveUniformsiv, cuT( "glGetActiveUniformsiv" ), cuT( "ARB" ) );
					gl_api::getFunction( m_pfnGetActiveUniformBlockiv, cuT( "glGetActiveUniformBlockiv" ), cuT( "ARB" ) );
				}
				else if ( hasExtension( EXT_uniform_buffer_object ) )
				{
					m_bHasUbo = m_iGlslVersion >= 140;
					gl_api::getFunction( m_pfnGetUniformBlockIndex, cuT( "glGetUniformBlockIndex" ), cuT( "EXT" ) );
					gl_api::getFunction( m_pfnBindBufferBase, cuT( "glBindBufferBase" ), cuT( "EXT" ) );
					gl_api::getFunction( m_pfnUniformBlockBinding, cuT( "glUniformBlockBinding" ), cuT( "EXT" ) );
					gl_api::getFunction( m_pfnGetUniformIndices, cuT( "glGetUniformIndices" ), cuT( "EXT" ) );
					gl_api::getFunction( m_pfnGetActiveUniformsiv, cuT( "glGetActiveUniformsiv" ), cuT( "EXT" ) );
					gl_api::getFunction( m_pfnGetActiveUniformBlockiv, cuT( "glGetActiveUniformBlockiv" ), cuT( "EXT" ) );
				}

				if ( hasExtension( ARB_geometry_shader4 ) || hasExtension( EXT_geometry_shader4 ) )
				{
					m_bHasGSh = true;
				}

				if ( hasExtension( ARB_tessellation_shader ) )
				{
					m_bHasTSh = true;
					gl_api::getFunction( m_pfnPatchParameteri, cuT( "glPatchParameteri" ), cuT( "ARB" ) );
				}

				if ( hasExtension( ARB_compute_shader ) )
				{
					m_bHasCSh = true;
					gl_api::getFunction( m_pfnDispatchCompute, cuT( "glDispatchCompute" ), cuT( "ARB" ) );

					if ( hasExtension( ARB_compute_variable_group_size ) )
					{
						m_bHasComputeVariableGroupSize = true;
						gl_api::getFunction( m_pfnDispatchComputeGroupSize, cuT( "glDispatchComputeGroupSize" ), cuT( "ARB" ) );
					}
				}
			}
		}

		hasExtension( ARB_timer_query );
		gl_api::getFunction( m_pfnGenQueries, cuT( "glGenQueries" ), cuT( "ARB" ) );
		gl_api::getFunction( m_pfnDeleteQueries, cuT( "glDeleteQueries" ), cuT( "ARB" ) );
		gl_api::getFunction( m_pfnIsQuery, cuT( "glIsQuery" ), cuT( "ARB" ) );
		gl_api::getFunction( m_pfnBeginQuery, cuT( "glBeginQuery" ), cuT( "ARB" ) );
		gl_api::getFunction( m_pfnEndQuery, cuT( "glEndQuery" ), cuT( "ARB" ) );
		gl_api::getFunction( m_pfnQueryCounter, cuT( "glQueryCounter" ), cuT( "ARB" ) );
		gl_api::getFunction( m_pfnGetQueryObjectiv, cuT( "glGetQueryObjectiv" ), cuT( "ARB" ) );
		gl_api::getFunction( m_pfnGetQueryObjectuiv, cuT( "glGetQueryObjectuiv" ), cuT( "ARB" ) );
		gl_api::getFunction( m_pfnGetQueryObjecti64v, cuT( "glGetQueryObjecti64v" ), cuT( "ARB" ) );
		gl_api::getFunction( m_pfnGetQueryObjectui64v, cuT( "glGetQueryObjectui64v" ), cuT( "ARB" ) );

		if ( hasExtension( ARB_program_interface_query ) )
		{
			gl_api::getFunction( m_pfnGetProgramInterfaceiv, cuT( "glGetProgramInterfaceiv" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnGetProgramResourceIndex, cuT( "glGetProgramResourceIndex" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnGetProgramResourceLocation, cuT( "glGetProgramResourceLocation" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnGetProgramResourceLocationIndex, cuT( "glGetProgramResourceLocationIndex" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnGetProgramResourceName, cuT( "glGetProgramResourceName" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnGetProgramResourceiv, cuT( "glGetProgramResourceiv" ), cuT( "ARB" ) );
		}

		if ( hasExtension( ARB_texture_storage ) )
		{
			gl_api::getFunction( m_pfnTexStorage1D, cuT( "glTexStorage1D" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnTexStorage2D, cuT( "glTexStorage2D" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnTexStorage3D, cuT( "glTexStorage3D" ), cuT( "ARB" ) );
		}

		if ( hasExtension( ARB_texture_storage_multisample ) )
		{
			gl_api::getFunction( m_pfnTexStorage2DMultisample, cuT( "glTexStorage2DMultisample" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnTexStorage3DMultisample, cuT( "glTexStorage3DMultisample" ), cuT( "ARB" ) );
		}

		if ( hasExtension( ARB_shader_storage_buffer_object ) )
		{
			m_bHasSsbo = true;
			gl_api::getFunction( m_pfnShaderStorageBlockBinding, cuT( "glShaderStorageBlockBinding" ), cuT( "ARB" ) );
		}

		if ( hasExtension( ARB_shader_image_load_store ) )
		{
			gl_api::getFunction( m_pfnMemoryBarrier, cuT( "glMemoryBarrier" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnMemoryBarrierByRegion, cuT( "glMemoryBarrierByRegion" ), cuT( "ARB" ) );
		}

		if ( hasExtension( ARB_draw_indirect ) )
		{
			gl_api::getFunction( m_pfnDrawArraysIndirect, cuT( "glDrawArraysIndirect" ), cuT( "ARB" ) );
			gl_api::getFunction( m_pfnDrawElementsIndirect, cuT( "glDrawElementsIndirect" ), cuT( "ARB" ) );
		}

		return true;
	}

	void OpenGl::initialiseDebug()
	{
		m_debug.initialise();
	}

	void OpenGl::cleanup()
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

	bool OpenGl::checkError( std::string const & p_text )const
	{
		return m_debug.checkError( p_text );
	}

	bool OpenGl::checkError( std::wstring const & p_text )const
	{
		return m_debug.checkError( p_text );
	}

	void OpenGl::displayExtensions()const
	{
		auto array = string::split( m_extensions, cuT( " " ), 0xFFFFFFFF, false );
		std::sort( array.begin(), array.end() );

		for ( auto extension : array )
		{
			Logger::logDebug( extension );
		}
	}

//*************************************************************************************************
}
