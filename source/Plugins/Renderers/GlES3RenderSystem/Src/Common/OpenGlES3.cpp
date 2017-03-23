#include "Common/OpenGlES3.hpp"

#include "Render/GlES3RenderSystem.hpp"

#include <Graphics/Rectangle.hpp>
#include <Log/Logger.hpp>
#include <Miscellaneous/Utils.hpp>

#include <GlslWriter.hpp>

#include <EGL/egl.h>

#include <iomanip>

using namespace Castor3D;
using namespace Castor;

#define DEF_USE_DIRECT_STATE_ACCESS 0

namespace GlES3Render
{
	//*************************************************************************************************

#define CASTOR_DBG_WIN32 0

	OpenGlES3::OpenGlES3( GlES3RenderSystem & p_renderSystem )
		: m_renderSystem{ p_renderSystem }
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
		PixelFormats[uint32_t( Castor::PixelFormat::eD32FS8 )] = PixelFmt( GlFormat::eDepthStencil, GlInternal::eD32FS8, GlType::eFloat );
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
		Attachments[uint32_t( Castor3D::AttachmentPoint::eDepthStencil )] = GlES3AttachmentPoint::eDepthStencil;

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
		RboStorages[uint32_t( Castor::PixelFormat::eD32FS8 )] = GlES3Internal::eD32FS8;
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
		StencilFuncs[uint32_t( Castor3D::StencilFunc::eLEqual )] = GlES3Comparator::eLEqual;
		StencilFuncs[uint32_t( Castor3D::StencilFunc::eEqual )] = GlES3Comparator::eEqual;
		StencilFuncs[uint32_t( Castor3D::StencilFunc::eNEqual )] = GlES3Comparator::eNEqual;
		StencilFuncs[uint32_t( Castor3D::StencilFunc::eGEqual )] = GlES3Comparator::eGEqual;
		StencilFuncs[uint32_t( Castor3D::StencilFunc::eGreater )] = GlES3Comparator::eGreater;
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
		BuffersTA[GlES3AttachmentPoint::eDepthStencil] = GlES3BufferBinding::eDepthStencil;

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

		return true;
	}

	bool OpenGlES3::Initialise()
	{
		m_bHasNonPowerOfTwoTextures = true;
		m_bHasVbo = true;
		m_bHasPbo = true;
		m_bHasVao = true;
		m_bHasTbo = false;
		m_bHasFbo = true;
		m_bHasSpl = true;
		m_bHasAnisotropic = false;
		m_bHasInstancedDraw = true;
		m_bHasInstancedArrays = true;
		m_bHasVSh = true;
		m_bHasPSh = true;
		m_bHasUbo = m_iGlslVersion >= 140;
		m_bHasGSh = true;
		m_bHasTSh = true;
		m_bHasCSh = false;
		m_bHasComputeVariableGroupSize = true;
		m_bHasSsbo = false;
		return true;
	}

	void OpenGlES3::InitialiseDebug()
	{
		m_debug.Initialise();
	}

	void OpenGlES3::Cleanup()
	{
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
