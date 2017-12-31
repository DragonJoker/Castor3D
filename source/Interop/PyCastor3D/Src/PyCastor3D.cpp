#include "PyCastor3DPch.hpp"

#include "PyCastor3DPrerequisites.hpp"

#include <Cache/LightCache.hpp>

using namespace castor;
using namespace castor3d;

#define CACHE_GETTER( owner, name )\
	name##Cache & ( owner::*resget##name##Cache )() = &owner::get##name##Cache

void ExportCastor3D()
{
	// Make "from castor.gfx import <whatever>" work
	py::object l_module( py::handle<>( py::borrowed( PyImport_AddModule( "castor.gfx" ) ) ) );
	// Make "from castor import gfx" work
	py::scope().attr( "gfx" ) = l_module;
	// set the current scope to the new sub-module
	py::scope l_scope = l_module;
	//@}
	/**@group_name eLIGHT_TYPE */
	//@{
	py::enum_< LightType >( "LightType" )
	.value( "DIRECTIONAL", LightType::eDirectional )
	.value( "POINT", LightType::ePoint )
	.value( "SPOT", LightType::eSpot )
	;
	//@}
	/**@group_name MovableType */
	//@{
	py::enum_< MovableType >( "MovableType" )
	.value( "CAMERA", MovableType::eCamera )
	.value( "GEOMETRY", MovableType::eGeometry )
	.value( "LIGHT", MovableType::eLight )
	.value( "BILLBOARD", MovableType::eBillboard )
	;
	//@}
	/**@group_name eOVERLAY_TYPE */
	//@{
	py::enum_< OverlayType >( "OverlayType" )
	.value( "PANEL", OverlayType::ePanel )
	.value( "BORDER_PANEL", OverlayType::eBorderPanel )
	.value( "TEXT", OverlayType::eText )
	;
	//@}
	/**@group_name eVIEWPORT_TYPE */
	//@{
	py::enum_< ViewportType >( "ViewportType" )
	.value( "ORTHO", ViewportType::eOrtho )
	.value( "PERSPECTIVE", ViewportType::ePerspective )
	.value( "FRUSTUM", ViewportType::eFrustum )
	;
	//@}
	/**@group_name Topology */
	//@{
	py::enum_< Topology >( "Topology" )
	.value( "POINTS", Topology::ePoints )
	.value( "LINES", Topology::eLines )
	.value( "LINE_LOOP", Topology::eLineLoop )
	.value( "LINE_STRIP", Topology::eLineStrip )
	.value( "TRIANGLES", Topology::eTriangles )
	.value( "TRIANGLE_STRIPS", Topology::eTriangleStrips )
	.value( "TRIANGLE_FAN", Topology::eTriangleFan )
	.value( "QUADS", Topology::eQuads )
	.value( "QUAD_STRIPS", Topology::eQuadStrips )
	.value( "POLYGON", Topology::ePolygon )
	;
	//@}
	/**@group_name TextureChannel */
	//@{
	py::enum_< TextureChannel >( "TextureChannel" )
	.value( "DIFFUSE", TextureChannel::eDiffuse )
	.value( "NORMAL", TextureChannel::eNormal )
	.value( "OPACITY", TextureChannel::eOpacity )
	.value( "SPECULAR", TextureChannel::eSpecular )
	.value( "HEIGHT", TextureChannel::eHeight )
	.value( "EMISSIVE", TextureChannel::eEmissive )
	.value( "GLOSS", TextureChannel::eGloss )
	.value( "ALL", TextureChannel::eAll )
	;
	//@}
	/**@group_name RgbBlendFunc */
	//@{
	py::enum_< ColourBlendFunc >( "RgbBlendFunc" )
	.value( "NONE", ColourBlendFunc::eNoBlend )
	.value( "FIRST_ARG", ColourBlendFunc::eFirstArg )
	.value( "ADD", ColourBlendFunc::eAdd )
	.value( "ADD_SIGNED", ColourBlendFunc::eAddSigned )
	.value( "MODULATE", ColourBlendFunc::eModulate )
	.value( "INTERPOLATE", ColourBlendFunc::eInterpolate )
	.value( "SUBTRACT", ColourBlendFunc::eSubtract )
	.value( "DOT3_RGB", ColourBlendFunc::edot3RGB )
	.value( "DOT3_RGBA", ColourBlendFunc::edot3RGBA )
	;
	//@}
	/**@group_name AlphaBlendFunc */
	//@{
	py::enum_< AlphaBlendFunc >( "AlphaBlendFunc" )
	.value( "NONE", AlphaBlendFunc::eNoBlend )
	.value( "FIRST_ARG", AlphaBlendFunc::eFirstArg )
	.value( "ADD", AlphaBlendFunc::eAdd )
	.value( "ADD_SIGNED", AlphaBlendFunc::eAddSigned )
	.value( "MODULATE", AlphaBlendFunc::eModulate )
	.value( "INTERPOLATE", AlphaBlendFunc::eInterpolate )
	.value( "SUBTRACT", AlphaBlendFunc::eSubtract )
	;
	//@}
	/**@group_name BlendSource */
	//@{
	py::enum_< BlendSource >( "BlendSource" )
	.value( "TEXTURE", BlendSource::eTexture )
	.value( "TEXTURE0", BlendSource::eTexture0 )
	.value( "TEXTURE1", BlendSource::eTexture1 )
	.value( "TEXTURE2,", BlendSource::eTexture2 )
	.value( "TEXTURE3", BlendSource::eTexture3 )
	.value( "CONSTANT", BlendSource::eConstant )
	.value( "DIFFUSE", BlendSource::eDiffuse )
	.value( "PREVIOUS", BlendSource::ePrevious )
	;
	//@}
	/**@group_name AlphaFunc */
	//@{
	py::enum_< ComparisonFunc >( "ComparisonFunc" )
	.value( "ALWAYS", ComparisonFunc::eAlways )
	.value( "LESS", ComparisonFunc::eLess )
	.value( "LESS_OR_EQUAL", ComparisonFunc::eLEqual )
	.value( "EQUAL", ComparisonFunc::eEqual )
	.value( "NOT_EQUAL", ComparisonFunc::eNEqual )
	.value( "GREATER_OR_EQUAL", ComparisonFunc::eGEqual )
	.value( "GREATER", ComparisonFunc::eGreater )
	.value( "NEVER", ComparisonFunc::eNever )
	;
	//@}
	/**@group_name TextureType */
	//@{
	py::enum_< TextureType >( "TextureType" )
	.value( "1D", TextureType::eOneDimension )
	.value( "1DARRAY", TextureType::eOneDimensionArray )
	.value( "2D", TextureType::eTwoDimensions )
	.value( "2DARRAY", TextureType::eTwoDimensionsArray )
	.value( "2DMS", TextureType::eTwoDimensionsMS )
	.value( "2DMSARRAY", TextureType::eTwoDimensionsMSArray )
	.value( "3D", TextureType::eThreeDimensions )
	.value( "BUFFER", TextureType::eBuffer )
	.value( "CUBE", TextureType::eCube )
	.value( "CUBEARRAY", TextureType::eCubeArray )
	;
	//@}
	/**@group_name InterpolationMode */
	//@{
	py::enum_< InterpolationMode >( "InterpolationMode" )
	.value( "NEAREST", InterpolationMode::eNearest )
	.value( "LINEAR", InterpolationMode::eLinear )
	;
	//@}
	/**@group_name WrapMode */
	//@{
	py::enum_< WrapMode >( "WrapMode" )
	.value( "REPEAT", WrapMode::eRepeat )
	.value( "MIRRORED_REPEAT", WrapMode::eMirroredRepeat )
	.value( "CLAMP_TO_BORDER", WrapMode::eClampToBorder )
	.value( "CLAMP_TO_EDGE", WrapMode::eClampToEdge )
	;
	//@}
	/**@group_name BlendOperation */
	//@{
	py::enum_< BlendOperation >( "BlendOperation" )
	.value( "ADD", BlendOperation::eAdd )
	.value( "SUBTRACT", BlendOperation::eSubtract )
	.value( "REV_SUBTRACT", BlendOperation::eRevSubtract )
	.value( "MIN", BlendOperation::eMin )
	.value( "MAX", BlendOperation::eMax )
	;
	//@}
	/**@group_name BlendOperand */
	//@{
	py::enum_< BlendOperand >( "BlendOperand" )
	.value( "ZERO", BlendOperand::eZero )
	.value( "ONE", BlendOperand::eOne )
	.value( "SRC_COLOUR", BlendOperand::eSrc1Colour )
	.value( "INV_SRC_COLOUR", BlendOperand::eInvSrcColour )
	.value( "DST_COLOUR", BlendOperand::eDstColour )
	.value( "INV_DST_COLOUR", BlendOperand::eInvDstColour )
	.value( "SRC_ALPHA", BlendOperand::eSrcAlpha )
	.value( "INV_SRC_ALPHA", BlendOperand::eInvSrcAlpha )
	.value( "DST_ALPHA", BlendOperand::eDstAlpha )
	.value( "INV_DST_ALPHA", BlendOperand::eInvDstAlpha )
	.value( "CONSTANT", BlendOperand::eConstant )
	.value( "INV_CONSTANT", BlendOperand::eInvConstant )
	.value( "SRC_ALPHA_SATURATE", BlendOperand::eSrcAlphaSaturate )
	.value( "SRC1_COLOUR", BlendOperand::eSrc1Colour )
	.value( "INV_SRC1_COLOUR", BlendOperand::eInvSrc1Colour )
	.value( "SRC1_ALPHA", BlendOperand::eSrc1Alpha )
	.value( "INV_SRC1_ALPHA", BlendOperand::eInvSrc1Alpha )
	;
	//@}
	/**@group_name DepthFunc */
	//@{
	py::enum_< DepthFunc >( "DepthFunc" )
	.value( "NEVER", DepthFunc::eNever )
	.value( "LESS", DepthFunc::eLess )
	.value( "EQUAL", DepthFunc::eEqual )
	.value( "LEQUAL", DepthFunc::eLEqual )
	.value( "GREATER", DepthFunc::eGreater )
	.value( "NOTEQUAL", DepthFunc::eNEqual )
	.value( "GEQUAL", DepthFunc::eGEqual )
	.value( "ALWAYS", DepthFunc::eAlways )
	;
	//@}
	/**@group_name WritingMask */
	//@{
	py::enum_< WritingMask >( "WritingMask" )
	.value( "ZERO", WritingMask::eZero )
	.value( "ALL", WritingMask::eAll )
	;
	//@}
	/**@group_name StencilFunc */
	//@{
	py::enum_< StencilFunc >( "StencilFunc" )
	.value( "NEVER", StencilFunc::eNever )
	.value( "LESS", StencilFunc::eLess )
	.value( "EQUAL", StencilFunc::eEqual )
	.value( "LEQUAL", StencilFunc::eLEqual )
	.value( "GREATER", StencilFunc::eGreater )
	.value( "NOTEQUAL", StencilFunc::eNEqual )
	.value( "GEQUAL", StencilFunc::eGEqual )
	.value( "ALWAYS", StencilFunc::eAlways )
	;
	//@}
	/**@group_name StencilOp */
	//@{
	py::enum_< StencilOp >( "StencilOp" )
	.value( "KEEP", StencilOp::eKeep )
	.value( "ZERO", StencilOp::eZero )
	.value( "REPLACE", StencilOp::eReplace )
	.value( "INCR", StencilOp::eIncrement )
	.value( "INCR_WRAP", StencilOp::eIncrWrap )
	.value( "DECR", StencilOp::eDecrement )
	.value( "DECR_WRAP", StencilOp::eDecrWrap )
	.value( "INVERT", StencilOp::eInvert )
	;
	//@}
	/**@group_name eFILL_MODE */
	//@{
	py::enum_< FillMode >( "FillMode" )
	.value( "POINT", FillMode::ePoint )
	.value( "LINE", FillMode::eLine )
	.value( "SOLID", FillMode::eSolid )
	;
	//@}
	/**@group_name eFACE */
	//@{
	py::enum_< Culling >( "Culling" )
	.value( "NONE", Culling::eNone )
	.value( "FRONT", Culling::eFront )
	.value( "BACK", Culling::eBack )
	.value( "FRONT_AND_BACK", Culling::eFrontAndBack )
	;
	//@}
	/**@group_name ShaderType */
	//@{
	py::enum_< ShaderType >( "ShaderType" )
	.value( "NONE", ShaderType::eNone )
	.value( "VERTEX", ShaderType::eVertex )
	.value( "HULL", ShaderType::eHull )
	.value( "DOMAIN", ShaderType::eDomain )
	.value( "GEOMETRY", ShaderType::eGeometry )
	.value( "PIXEL", ShaderType::ePixel )
	.value( "COMPUTE", ShaderType::eCompute )
	;
	//@}
	/**@group_name eSHADER_MODEL */
	//@{
	py::enum_< ShaderModel >( "ShaderModel" )
	.value( "1", ShaderModel::eModel1 )
	.value( "2", ShaderModel::eModel2 )
	.value( "3", ShaderModel::eModel3 )
	.value( "4", ShaderModel::eModel4 )
	.value( "5", ShaderModel::eModel5 )
	;
	//@}
	/**@group_name BlendState */
	//@{
	py::class_< BlendState, boost::noncopyable >( "BlendState", py::no_init )
	.add_property( "independant_blend", &BlendState::isIndependantBlendEnabled, &BlendState::enableIndependantBlend, "The independant blend enabled status" )
	.add_property( "blend_factors", py::make_function( &BlendState::getBlendFactors, py::return_value_policy< py::copy_const_reference >() ), &BlendState::setBlendFactors, "The blend factors" )
	.add_property( "blend_enabled", cpy::make_getter( &BlendState::isBlendEnabled, 0 ), cpy::make_blend_setter( &BlendState::enableBlend, 0 ), "The blend enabled status" )
	.add_property( "rgb_src_blend", cpy::make_getter( &BlendState::getRgbSrcBlend, 0 ), cpy::make_blend_setter( &BlendState::setRgbSrcBlend, 0 ), "The source source RGB blend value" )
	.add_property( "rgb_dst_blend", cpy::make_getter( &BlendState::getRgbDstBlend, 0 ), cpy::make_blend_setter( &BlendState::setRgbDstBlend, 0 ), "The source destination RGB blend value" )
	.add_property( "rgb_blend_op", cpy::make_getter( &BlendState::getRgbBlendOp, 0 ), cpy::make_blend_setter( &BlendState::setRgbBlendOp, 0 ), "The RGB blend operator" )
	.add_property( "alpha_src_blend", cpy::make_getter( &BlendState::getAlphaSrcBlend, 0 ), cpy::make_blend_setter( &BlendState::setAlphaSrcBlend, 0 ), "The source source Alpha blend value" )
	.add_property( "alpha_dst_blend", cpy::make_getter( &BlendState::getAlphaDstBlend, 0 ), cpy::make_blend_setter( &BlendState::setAlphaDstBlend, 0 ), "The source destination Alpha blend value" )
	.add_property( "alpha_blend_op", cpy::make_getter( &BlendState::getAlphaBlendOp, 0 ), cpy::make_blend_setter( &BlendState::setAlphaBlendOp, 0 ), "The Alpha blend operator" )
	.add_property( "write_mask", cpy::make_getter( &BlendState::getWriteMask, 0 ), cpy::make_blend_setter( &BlendState::setWriteMask, 0 ), "The render target write mask" )
	.add_property( "blend_2_enabled", cpy::make_getter( &BlendState::isBlendEnabled, 1 ), cpy::make_blend_setter( &BlendState::enableBlend, 1 ), "The blend enabled status, for 2nd unit" )
	.add_property( "rgb_src_blend_2", cpy::make_getter( &BlendState::getRgbSrcBlend, 1 ), cpy::make_blend_setter( &BlendState::setRgbSrcBlend, 1 ), "The source source RGB blend value, for 2nd unit" )
	.add_property( "rgb_dst_blend_2", cpy::make_getter( &BlendState::getRgbDstBlend, 1 ), cpy::make_blend_setter( &BlendState::setRgbDstBlend, 1 ), "The source destination RGB blend value, for 2nd unit" )
	.add_property( "rgb_blend_2_op", cpy::make_getter( &BlendState::getRgbBlendOp, 1 ), cpy::make_blend_setter( &BlendState::setRgbBlendOp, 1 ), "The RGB blend operato, for 2nd unitr" )
	.add_property( "alpha_src_blend_2", cpy::make_getter( &BlendState::getAlphaSrcBlend, 1 ), cpy::make_blend_setter( &BlendState::setAlphaSrcBlend, 1 ), "The source source Alpha blend value, for 2nd unit" )
	.add_property( "alpha_dst_blend_2", cpy::make_getter( &BlendState::getAlphaDstBlend, 1 ), cpy::make_blend_setter( &BlendState::setAlphaDstBlend, 1 ), "The source destination Alpha blend value, for 2nd unit" )
	.add_property( "alpha_blend_2_op", cpy::make_getter( &BlendState::getAlphaBlendOp, 1 ), cpy::make_blend_setter( &BlendState::setAlphaBlendOp, 1 ), "The Alpha blend operator, for 2nd unit" )
	.add_property( "write_mask_2", cpy::make_getter( &BlendState::getWriteMask, 1 ), cpy::make_blend_setter( &BlendState::setWriteMask, 1 ), "The render target write mask, for 2nd unit" )
	.add_property( "blend_3_enabled", cpy::make_getter( &BlendState::isBlendEnabled, 2 ), cpy::make_blend_setter( &BlendState::enableBlend, 2 ), "The blend enabled status, for 3rd unit" )
	.add_property( "rgb_src_blend_3", cpy::make_getter( &BlendState::getRgbSrcBlend, 2 ), cpy::make_blend_setter( &BlendState::setRgbSrcBlend, 2 ), "The source source RGB blend value, for 3rd unit" )
	.add_property( "rgb_dst_blend_3", cpy::make_getter( &BlendState::getRgbDstBlend, 2 ), cpy::make_blend_setter( &BlendState::setRgbDstBlend, 2 ), "The source destination RGB blend value, for 3rd unit" )
	.add_property( "rgb_blend_3_op", cpy::make_getter( &BlendState::getRgbBlendOp, 2 ), cpy::make_blend_setter( &BlendState::setRgbBlendOp, 2 ), "The RGB blend operator, for 3rd unit" )
	.add_property( "alpha_src_blend_3", cpy::make_getter( &BlendState::getAlphaSrcBlend, 2 ), cpy::make_blend_setter( &BlendState::setAlphaSrcBlend, 2 ), "The source source Alpha blend value, for 3rd unit" )
	.add_property( "alpha_dst_blend_3", cpy::make_getter( &BlendState::getAlphaDstBlend, 2 ), cpy::make_blend_setter( &BlendState::setAlphaDstBlend, 2 ), "The source destination Alpha blend value, for 3rd unit" )
	.add_property( "alpha_blend_3_op", cpy::make_getter( &BlendState::getAlphaBlendOp, 2 ), cpy::make_blend_setter( &BlendState::setAlphaBlendOp, 2 ), "The Alpha blend operator, for 3rd unit" )
	.add_property( "write_mask_3", cpy::make_getter( &BlendState::getWriteMask, 2 ), cpy::make_blend_setter( &BlendState::setWriteMask, 2 ), "The render target write mask, for 3rd unit" )
	.add_property( "blend_4_enabled", cpy::make_getter( &BlendState::isBlendEnabled, 3 ), cpy::make_blend_setter( &BlendState::enableBlend, 3 ), "The blend enabled status, for 4th unit" )
	.add_property( "rgb_src_blend_4", cpy::make_getter( &BlendState::getRgbSrcBlend, 3 ), cpy::make_blend_setter( &BlendState::setRgbSrcBlend, 3 ), "The source source RGB blend value, for 4th unit" )
	.add_property( "rgb_dst_blend_4", cpy::make_getter( &BlendState::getRgbDstBlend, 3 ), cpy::make_blend_setter( &BlendState::setRgbDstBlend, 3 ), "The source destination RGB blend value, for 4th unit" )
	.add_property( "rgb_blend_4_op", cpy::make_getter( &BlendState::getRgbBlendOp, 3 ), cpy::make_blend_setter( &BlendState::setRgbBlendOp, 3 ), "The RGB blend operator, for 4th unit" )
	.add_property( "alpha_src_blend_4", cpy::make_getter( &BlendState::getAlphaSrcBlend, 3 ), cpy::make_blend_setter( &BlendState::setAlphaSrcBlend, 3 ), "The source source Alpha blend value, for 4th unit" )
	.add_property( "alpha_dst_blend_4", cpy::make_getter( &BlendState::getAlphaDstBlend, 3 ), cpy::make_blend_setter( &BlendState::setAlphaDstBlend, 3 ), "The source destination Alpha blend value, for 4th unit" )
	.add_property( "alpha_blend_4_op", cpy::make_getter( &BlendState::getAlphaBlendOp, 3 ), cpy::make_blend_setter( &BlendState::setAlphaBlendOp, 3 ), "The Alpha blend operator, for 4th unit" )
	.add_property( "write_mask_4", cpy::make_getter( &BlendState::getWriteMask, 3 ), cpy::make_blend_setter( &BlendState::setWriteMask, 3 ), "The render target write mask, for 4th unit" )
	.add_property( "blend_5_enabled", cpy::make_getter( &BlendState::isBlendEnabled, 4 ), cpy::make_blend_setter( &BlendState::enableBlend, 4 ), "The blend enabled status, for 5th unit" )
	.add_property( "rgb_src_blend_5", cpy::make_getter( &BlendState::getRgbSrcBlend, 4 ), cpy::make_blend_setter( &BlendState::setRgbSrcBlend, 4 ), "The source source RGB blend value, for 5th unit" )
	.add_property( "rgb_dst_blend_5", cpy::make_getter( &BlendState::getRgbDstBlend, 4 ), cpy::make_blend_setter( &BlendState::setRgbDstBlend, 4 ), "The source destination RGB blend value, for 5th unit" )
	.add_property( "rgb_blend_5_op", cpy::make_getter( &BlendState::getRgbBlendOp, 4 ), cpy::make_blend_setter( &BlendState::setRgbBlendOp, 4 ), "The RGB blend operator, for 5th unit" )
	.add_property( "alpha_src_blend_5", cpy::make_getter( &BlendState::getAlphaSrcBlend, 4 ), cpy::make_blend_setter( &BlendState::setAlphaSrcBlend, 4 ), "The source source Alpha blend value, for 5th unit" )
	.add_property( "alpha_dst_blend_5", cpy::make_getter( &BlendState::getAlphaDstBlend, 4 ), cpy::make_blend_setter( &BlendState::setAlphaDstBlend, 4 ), "The source destination Alpha blend value, for 5th unit" )
	.add_property( "alpha_blend_5_op", cpy::make_getter( &BlendState::getAlphaBlendOp, 4 ), cpy::make_blend_setter( &BlendState::setAlphaBlendOp, 4 ), "The Alpha blend operator, for 5th unit" )
	.add_property( "write_mask_5", cpy::make_getter( &BlendState::getWriteMask, 4 ), cpy::make_blend_setter( &BlendState::setWriteMask, 4 ), "The render target write mask, for 5th unit" )
	.add_property( "blend_6_enabled", cpy::make_getter( &BlendState::isBlendEnabled, 5 ), cpy::make_blend_setter( &BlendState::enableBlend, 5 ), "The blend enabled status, for 6th unit" )
	.add_property( "rgb_src_blend_6", cpy::make_getter( &BlendState::getRgbSrcBlend, 5 ), cpy::make_blend_setter( &BlendState::setRgbSrcBlend, 5 ), "The source source RGB blend value, for 6th unit" )
	.add_property( "rgb_dst_blend_6", cpy::make_getter( &BlendState::getRgbDstBlend, 5 ), cpy::make_blend_setter( &BlendState::setRgbDstBlend, 5 ), "The source destination RGB blend value, for 6th unit" )
	.add_property( "rgb_blend_6_op", cpy::make_getter( &BlendState::getRgbBlendOp, 5 ), cpy::make_blend_setter( &BlendState::setRgbBlendOp, 5 ), "The RGB blend operator, for 6th unit" )
	.add_property( "alpha_src_blend_6", cpy::make_getter( &BlendState::getAlphaSrcBlend, 5 ), cpy::make_blend_setter( &BlendState::setAlphaSrcBlend, 5 ), "The source source Alpha blend value, for 6th unit" )
	.add_property( "alpha_dst_blend_6", cpy::make_getter( &BlendState::getAlphaDstBlend, 5 ), cpy::make_blend_setter( &BlendState::setAlphaDstBlend, 5 ), "The source destination Alpha blend value, for 6th unit" )
	.add_property( "alpha_blend_6_op", cpy::make_getter( &BlendState::getAlphaBlendOp, 5 ), cpy::make_blend_setter( &BlendState::setAlphaBlendOp, 5 ), "The Alpha blend operator, for 6th unit" )
	.add_property( "write_mask_6", cpy::make_getter( &BlendState::getWriteMask, 5 ), cpy::make_blend_setter( &BlendState::setWriteMask, 5 ), "The render target write mask, for 6th unit" )
	.add_property( "blend_7_enabled", cpy::make_getter( &BlendState::isBlendEnabled, 6 ), cpy::make_blend_setter( &BlendState::enableBlend, 6 ), "The blend enabled status, for 7th unit" )
	.add_property( "rgb_src_blend_7", cpy::make_getter( &BlendState::getRgbSrcBlend, 6 ), cpy::make_blend_setter( &BlendState::setRgbSrcBlend, 6 ), "The source source RGB blend value, for 7th unit" )
	.add_property( "rgb_dst_blend_7", cpy::make_getter( &BlendState::getRgbDstBlend, 6 ), cpy::make_blend_setter( &BlendState::setRgbDstBlend, 6 ), "The source destination RGB blend value, for 7th unit" )
	.add_property( "rgb_blend_7_op", cpy::make_getter( &BlendState::getRgbBlendOp, 6 ), cpy::make_blend_setter( &BlendState::setRgbBlendOp, 6 ), "The RGB blend operator, for 7th unit" )
	.add_property( "alpha_src_blend_7", cpy::make_getter( &BlendState::getAlphaSrcBlend, 6 ), cpy::make_blend_setter( &BlendState::setAlphaSrcBlend, 6 ), "The source source Alpha blend value, for 7th unit" )
	.add_property( "alpha_dst_blend_7", cpy::make_getter( &BlendState::getAlphaDstBlend, 6 ), cpy::make_blend_setter( &BlendState::setAlphaDstBlend, 6 ), "The source destination Alpha blend value, for 7th unit" )
	.add_property( "alpha_blend_7_op", cpy::make_getter( &BlendState::getAlphaBlendOp, 6 ), cpy::make_blend_setter( &BlendState::setAlphaBlendOp, 6 ), "The Alpha blend operator, for 7th unit" )
	.add_property( "write_mask_7", cpy::make_getter( &BlendState::getWriteMask, 6 ), cpy::make_blend_setter( &BlendState::setWriteMask, 6 ), "The render target write mask, for 7th unit" )
	.add_property( "blend_8_enabled", cpy::make_getter( &BlendState::isBlendEnabled, 7 ), cpy::make_blend_setter( &BlendState::enableBlend, 7 ), "The blend enabled status, for 8th unit" )
	.add_property( "rgb_src_blend_8", cpy::make_getter( &BlendState::getRgbSrcBlend, 7 ), cpy::make_blend_setter( &BlendState::setRgbSrcBlend, 7 ), "The source source RGB blend value, for 8th unit" )
	.add_property( "rgb_dst_blend_8", cpy::make_getter( &BlendState::getRgbDstBlend, 7 ), cpy::make_blend_setter( &BlendState::setRgbDstBlend, 7 ), "The source destination RGB blend valu, for 8th unite" )
	.add_property( "rgb_blend_8_op", cpy::make_getter( &BlendState::getRgbBlendOp, 7 ), cpy::make_blend_setter( &BlendState::setRgbBlendOp, 7 ), "The RGB blend operator, for 8th unit" )
	.add_property( "alpha_src_blend_8", cpy::make_getter( &BlendState::getAlphaSrcBlend, 7 ), cpy::make_blend_setter( &BlendState::setAlphaSrcBlend, 7 ), "The source source Alpha blend value, for 8th unit" )
	.add_property( "alpha_dst_blend_8", cpy::make_getter( &BlendState::getAlphaDstBlend, 7 ), cpy::make_blend_setter( &BlendState::setAlphaDstBlend, 7 ), "The source destination Alpha blend value, for 8th unit" )
	.add_property( "alpha_blend_8_op", cpy::make_getter( &BlendState::getAlphaBlendOp, 7 ), cpy::make_blend_setter( &BlendState::setAlphaBlendOp, 7 ), "The Alpha blend operator, for 8th unit" )
	.add_property( "write_mask_8", cpy::make_getter( &BlendState::getWriteMask, 7 ), cpy::make_blend_setter( &BlendState::setWriteMask, 7 ), "The render target write mask, for 8th unit" )
	;
	//@}
	/**@group_name DepthStencilState */
	//@{
	py::class_< DepthStencilState, boost::noncopyable >( "DepthStencilState", py::no_init )
	.add_property( "depth_test", &DepthStencilState::getDepthTest, &DepthStencilState::setDepthTest, "The depth test enabled status" )
	.add_property( "depth_func", &DepthStencilState::getDepthFunc, &DepthStencilState::setDepthFunc, "The depth test function" )
	.add_property( "depth_mask", &DepthStencilState::getDepthMask, &DepthStencilState::setDepthMask, "The depth test mask" )
	.add_property( "depth_near", &DepthStencilState::getDepthNear, "The depth near distance" )
	.add_property( "depth_far", &DepthStencilState::getDepthFar, "The depth far distance" )
	.add_property( "stencil_test", &DepthStencilState::getStencilTest, &DepthStencilState::setStencilTest, "The stencil test enabled status" )
	.add_property( "stencil_read_mask", &DepthStencilState::getStencilReadMask, &DepthStencilState::setStencilReadMask, "The stencil test read mask" )
	.add_property( "stencil_write_mask", &DepthStencilState::getStencilWriteMask, &DepthStencilState::setStencilWriteMask, "The stencil test write mask" )
	.add_property( "stencil_front_func", &DepthStencilState::getStencilFrontFunc, &DepthStencilState::setStencilFrontFunc, "The stencil test function, for the front faces" )
	.add_property( "stencil_front_fail_op", &DepthStencilState::getStencilFrontFailOp, &DepthStencilState::setStencilFrontFailOp, "The stencil test failed operation, for the front faces" )
	.add_property( "stencil_front_depth fail_op", &DepthStencilState::getStencilFrontDepthFailOp, &DepthStencilState::setStencilFrontDepthFailOp, "The stencil test, depth test failed operation, for the front faces" )
	.add_property( "stencil_front_pass_op", &DepthStencilState::getStencilFrontPassOp, &DepthStencilState::setStencilFrontPassOp, "The stencil test passed operation, for the front faces" )
	.add_property( "stencil_back_func", &DepthStencilState::getStencilBackFunc, &DepthStencilState::setStencilBackFunc, "The stencil test function, for the back faces" )
	.add_property( "stencil_back_fail_op", &DepthStencilState::getStencilBackFailOp, &DepthStencilState::setStencilBackFailOp, "The stencil test failed operation, for the back faces" )
	.add_property( "stencil_back_depth fail_op", &DepthStencilState::getStencilBackDepthFailOp, &DepthStencilState::setStencilBackDepthFailOp, "The stencil test, depth test failed operation, for the back faces" )
	.add_property( "stencil_back_pass_op", &DepthStencilState::getStencilBackPassOp, &DepthStencilState::setStencilBackPassOp, "The stencil test passed operation, for the back faces" )
	.def( "set_depth_range", &DepthStencilState::setDepthRange )
	;
	//@}
	/**@group_name MultisampleState */
	//@{
	py::class_< MultisampleState, boost::noncopyable >( "MultisampleState", py::no_init )
	.add_property( "multisample", &MultisampleState::getMultisample, &MultisampleState::setMultisample, "The multisample enabled status" )
	.add_property( "alpha_to_coverage", &MultisampleState::isAlphaToCoverageEnabled, &MultisampleState::enableAlphaToCoverage, "The alpha to coverage enabled status" )
	.add_property( "sample_coverage_mask", py::make_function( &MultisampleState::getSampleCoverageMask ), &MultisampleState::setSampleCoverageMask, "The sample coverage mask" )
	;
	//@}
	/**@group_name RasteriserState */
	//@{
	py::class_< RasteriserState, boost::noncopyable >( "RasteriserState", py::no_init )
	.add_property( "fill_mode", &RasteriserState::getFillMode, &RasteriserState::setFillMode, "The fill mode" )
	.add_property( "cull_faces", &RasteriserState::getCulledFaces, &RasteriserState::setCulledFaces, "The faces culled by rasteriser" )
	.add_property( "ccw", &RasteriserState::getFrontCCW, &RasteriserState::setFrontCCW, "The faces which are considered front facing (true => CCW, false => CW)" )
	.add_property( "antialiased_lines", &RasteriserState::getAntialiasedLines, &RasteriserState::setAntialiasedLines, "The antialiased lines status" )
	.add_property( "depth_bias", &RasteriserState::getDepthBiasUnits, &RasteriserState::setDepthBiasUnits, "The depth bias units" )
	.add_property( "depth_bias", &RasteriserState::getDepthBiasFactor, &RasteriserState::setDepthBiasFactor, "The depth bias factor" )
	.add_property( "depth_clipping", &RasteriserState::getDepthClipping, &RasteriserState::setDepthClipping, "The depth clipping enabled status" )
	.add_property( "scissor_test", &RasteriserState::getScissor, &RasteriserState::setScissor, "The scissor test enabled status" )
	;
	//@}
	/**@group_name Sampler */
	//@{
	py::class_< Sampler, boost::noncopyable >( "Sampler", py::no_init )
	.add_property( "name", py::make_function( &Sampler::getName, py::return_value_policy< py::copy_const_reference >() ), cpy::make_setter( &Sampler::setName ), "The sampler name" )
	.add_property( "min_filter", cpy::make_getter( &Sampler::getInterpolationMode, InterpolationFilter::eMin ), cpy::make_setter( &Sampler::setInterpolationMode, InterpolationFilter::eMin ), "The sampler min filter" )
	.add_property( "mag_filter", cpy::make_getter( &Sampler::getInterpolationMode, InterpolationFilter::eMag ), cpy::make_setter( &Sampler::setInterpolationMode, InterpolationFilter::eMag ), "The sampler mag filter" )
	.add_property( "mip_filter", cpy::make_getter( &Sampler::getInterpolationMode, InterpolationFilter::eMip ), cpy::make_setter( &Sampler::setInterpolationMode, InterpolationFilter::eMip ), "The sampler mip filter" )
	.add_property( "u_wrap", cpy::make_getter( &Sampler::getWrappingMode, TextureUVW::eU ), cpy::make_setter( &Sampler::setWrappingMode, TextureUVW::eU ), "The sampler U wrap mode" )
	.add_property( "v_wrap", cpy::make_getter( &Sampler::getWrappingMode, TextureUVW::eV ), cpy::make_setter( &Sampler::setWrappingMode, TextureUVW::eV ), "The sampler V wrap mode" )
	.add_property( "w_wrap", cpy::make_getter( &Sampler::getWrappingMode, TextureUVW::eW ), cpy::make_setter( &Sampler::setWrappingMode, TextureUVW::eW ), "The sampler W wrap mode" )
	.add_property( "max_anisotropy", &Sampler::getMaxAnisotropy, &Sampler::setMaxAnisotropy, "The sampler max anisotropy" )
	.add_property( "min_lod", &Sampler::getMinLod, &Sampler::setMinLod, "The sampler min LOD" )
	.add_property( "max_lod", &Sampler::getMaxLod, &Sampler::setMaxLod, "The sampler max LOD" )
	.add_property( "lod_bias", &Sampler::getLodBias, &Sampler::setLodBias, "The sampler LOD bias" )
	.add_property( "border_colour", py::make_function( &Sampler::getBorderColour, py::return_value_policy< py::copy_const_reference >() ), cpy::make_setter( &Sampler::setBorderColour ), "The sampler border colour" )
	.def( "initialise", &Sampler::Initialise )
	.def( "cleanup", &Sampler::Cleanup )
	;
	//@}
	/**@group_name TextureImage */
	//@{
	void ( TextureImage::*dynamicTextureImagesetter )( PxBufferBaseSPtr ) = &TextureImage::initialiseSource;
	void ( TextureImage::*staticTextureImagesetter)( Path const &, Path const & ) = &TextureImage::initialiseSource;
	void ( TextureImage::*resizer2d )( Size const & ) = &TextureImage::Resize;
	void ( TextureImage::*resizer3d )( Point3ui const & ) = &TextureImage::Resize;
	py::class_< TextureImage, boost::noncopyable >( "TextureImage", py::no_init )
	.add_property( "buffer", &TextureImage::getBuffer, "The texture image buffer" )
	.def( "set_source", staticTextureImagesetter, "Sets the texture image" )
	.def( "set_source", dynamicTextureImagesetter, "Sets the texture image" )
	.def( "resize", resizer2d, "Resizes the texture image" )
	.def( "resize", resizer3d, "Resizes the texture image" )
	;
	//@}
	/**@group_name TextureLayout */
	//@{
	TextureImage &( TextureLayout::*textureLayoutImagegetter )( size_t ) = &TextureLayout::getImage;
	py::class_< TextureLayout, boost::noncopyable >( "TextureLayout", py::no_init )
	.add_property( "type", &TextureLayout::getType, "The texture type" )
	.def( "image", py::make_function( textureLayoutImagegetter, py::return_value_policy< py::copy_non_const_reference >() ) )
	.def( "initialise", &TextureLayout::Initialise )
	.def( "cleanup", &TextureLayout::Cleanup )
	;
	//@}
	/**@group_name TextureUnit */
	//@{
	py::class_< TextureUnit, boost::noncopyable >( "TextureUnit", py::no_init )
	.add_property( "texture", &TextureUnit::getTexture, &TextureUnit::setTexture, "The unit texture" )
	.add_property( "alpha_test_func", &TextureUnit::getAlphaFunc, &TextureUnit::setAlphaFunc, "The Alpha Test function" )
	.add_property( "alpha_test_value", &TextureUnit::getAlphaValue, &TextureUnit::setAlphaValue, "The Alpha Test reference value" )
	.add_property( "alpha_blend_func", &TextureUnit::getAlpFunction, &TextureUnit::setAlpFunction, "The Alpha Blending function" )
	.add_property( "alpha_blend_arg_0", cpy::make_getter( &TextureUnit::getAlpArgument, BlendSrcIndex::eIndex0 ), cpy::make_setter( &TextureUnit::setAlpArgument, BlendSrcIndex::eIndex0 ), "The Alpha Blending argument for index 0" )
	.add_property( "alpha_blend_arg_1", cpy::make_getter( &TextureUnit::getAlpArgument, BlendSrcIndex::eIndex1 ), cpy::make_setter( &TextureUnit::setAlpArgument, BlendSrcIndex::eIndex1 ), "The Alpha Blending argument for index 1" )
	.add_property( "alpha_blend_arg_2", cpy::make_getter( &TextureUnit::getAlpArgument, BlendSrcIndex::eIndex2 ), cpy::make_setter( &TextureUnit::setAlpArgument, BlendSrcIndex::eIndex2 ), "The Alpha Blending argument for index 2" )
	.add_property( "rgb_blend_func", &TextureUnit::getRgbFunction, &TextureUnit::setRgbFunction, "The RGB Blending function" )
	.add_property( "rgb_blend_arg_0", cpy::make_getter( &TextureUnit::getRgbArgument, BlendSrcIndex::eIndex0 ), cpy::make_setter( &TextureUnit::setRgbArgument, BlendSrcIndex::eIndex0 ), "The RGB Blending argument for index 0" )
	.add_property( "rgb_blend_arg_1", cpy::make_getter( &TextureUnit::getRgbArgument, BlendSrcIndex::eIndex1 ), cpy::make_setter( &TextureUnit::setRgbArgument, BlendSrcIndex::eIndex1 ), "The RGB Blending argument for index 1" )
	.add_property( "rgb_blend_arg_2", cpy::make_getter( &TextureUnit::getRgbArgument, BlendSrcIndex::eIndex2 ), cpy::make_setter( &TextureUnit::setRgbArgument, BlendSrcIndex::eIndex2 ), "The RGB Blending argument for index 2" )
	.add_property( "channel", &TextureUnit::getChannel, &TextureUnit::setChannel, "The texture channel" )
	.add_property( "blend_colour", py::make_function( &TextureUnit::getBlendColour, py::return_value_policy< py::copy_const_reference >() ), &TextureUnit::setBlendColour, "The texture blend colour" )
	.def( "set_texture", &TextureUnit::setTexture )
	;
	//@}
	/**@group_name ShaderProgram */
	//@{
	void( ShaderProgram::*shaderProgramFilesetter )( ShaderType, ShaderModel, Path const & ) = &ShaderProgram::setFile;
	py::class_< ShaderProgram, boost::noncopyable >( "ShaderProgram", py::no_init )
	.def( "get_file", &ShaderProgram::getFile )
	.def( "set_file", shaderProgramFilesetter )
	.def( "get_source", &ShaderProgram::getSource )
	.def( "set_source", &ShaderProgram::setSource )
	.def( "initialise", &ShaderProgram::Initialise )
	.def( "cleanup", &ShaderProgram::Cleanup )
	.def( "create_object", &ShaderProgram::createObject )
	;
	//@}
	/**@group_name LegacyPass */
	//@{
	TextureUnitSPtr( Pass::*passChannelTextureUnitgetter )( TextureChannel )const = &Pass::getTextureUnit;
	typedef TextureUnitPtrArrayIt( Pass::*TextureUnitPtrArrayItFunc )( );
	py::class_< Pass, boost::noncopyable >( "Pass", py::no_init )
	.add_property( "diffuse", cpy::make_getter( &LegacyPass::getDiffuse, py::return_value_policy< py::copy_const_reference >() ), cpy::make_setter( &LegacyPass::setDiffuse ), "The pass diffuse colour" )
	.add_property( "specular", cpy::make_getter( &LegacyPass::getSpecular, py::return_value_policy< py::copy_const_reference >() ), cpy::make_setter( &LegacyPass::setSpecular ), "The pass specular colour" )
	.add_property( "emissive", cpy::make_getter( &LegacyPass::getEmissive, py::return_value_policy< py::copy_const_reference >() ), cpy::make_setter( &LegacyPass::setEmissive ), "The pass emissive colour" )
	.add_property( "shininess", &LegacyPass::getShininess, &LegacyPass::setShininess, "The pass shininess value" )
	.add_property( "two_sided", &LegacyPass::IsTwoSided, &LegacyPass::setTwoSided, "The pass two sided status" )
	.add_property( "alpha", &LegacyPass::getOpacity, &LegacyPass::setOpacity, "The pass global alpha value" )
	.def( "create_texture_unit", &LegacyPass::addTextureUnit )
	.def( "destroy_pass", &LegacyPass::destroyTextureUnit )
	.def( "get_texture_unit_at_channel", passChannelTextureUnitgetter )
	.def( "units", py::range< TextureUnitPtrArrayItFunc, TextureUnitPtrArrayItFunc >( &LegacyPass::begin, &LegacyPass::end ) )
	;
	//@}
	/**@group_name Material */
	//@{
	typedef PassPtrArrayIt( Material::*PassPtrArrayItFunc )( );
	py::class_< Material >( "Material", py::init< String const &, Engine & >() )
	.add_property( "pass_count", &Material::getPassCount, "The material's passes count" )
	.def( "initialise", &Material::Initialise )
	.def( "cleanup", &Material::Cleanup )
	.def( "create_pass", &Material::createPass )
	.def( "destroy_pass", &Material::destroyPass )
	.def( "passes", py::range< PassPtrArrayItFunc, PassPtrArrayItFunc >( &Material::begin, &Material::end ) )
	;
	//@}
	/**@group_name Submesh */
	//@{
	py::class_< Submesh, boost::noncopyable >( "Submesh", py::no_init )
	.add_property( "faces_count", &Submesh::getFaceCount, "The total submesh's faces count" )
	.add_property( "vertex_count", &Submesh::getPointsCount, "The total submesh's vertices count" )
	.add_property( "topology", &Submesh::getTopology, &Submesh::setTopology, "The submesh topology" )
	.def( "add_point", cpy::PointAdder() )
	.def( "add_face", cpy::FaceAdder() )
	;
	//@}
	/**@group_name Mesh */
	//@{
	typedef SubmeshPtrArrayIt( Mesh::*SubmeshPtrArrayItFunc )();
	Animation & ( Mesh::*meshAnimationgetter )( castor::String const & ) = &Mesh::getAnimation;
	py::class_< Mesh, boost::noncopyable >( "Mesh", py::no_init )
	.add_property( "submesh_count", &Mesh::getSubmeshCount, "The total mesh's submeshes count" )
	.add_property( "faces_count", &Mesh::getFaceCount, "The total mesh's faces count" )
	.add_property( "vertex_count", &Mesh::getVertexCount, "The total mesh's vertices count" )
	.add_property( "submeshes", py::range< SubmeshPtrArrayItFunc, SubmeshPtrArrayItFunc >( &Mesh::begin, &Mesh::end ) )
	.def( "create_submesh", &Mesh::createSubmesh )
	.def( "delete_submesh", &Mesh::deleteSubmesh )
	.def( "animations", cpy::make_map_wrapper< AnimationPtrStrMap >( "Animations", &Mesh::getAnimations ) )
	.def( "create_animation", py::make_function (&Mesh::createAnimation, py::return_value_policy< py::copy_non_const_reference >() ) )
	.def( "get_animation", py::make_function( meshAnimationgetter, py::return_value_policy< py::copy_non_const_reference >() ) )
	;
	//@}
	/**@group_name SceneNodeCache */
	//@{
	SceneNodeSPtr( SceneNodeCache::*sceneNodeCreator )( castor::String const &, SceneNodeSPtr ) = &SceneNodeCache::Add;
	py::class_< SceneNodeCache, boost::noncopyable >( "SceneNodeCache", py::no_init )
	.def( "add", sceneNodeCreator, "Adds a SceneNode to the cache" )
	.def( "remove", &SceneNodeCache::Remove, "Finds a SceneNode" )
	.def( "has", &SceneNodeCache::Has, "Tells if the cache has a SceneNode" )
	.def( "find", &SceneNodeCache::Find, "Finds a SceneNode in the cache" )
	;
	//@}
	/**@group_name CameraCache */
	//@{
	CameraSPtr( CameraCache::*cameraCreator )( castor::String const &, SceneNodeSPtr, Viewport && ) = &CameraCache::Add;
	py::class_< CameraCache, boost::noncopyable >( "CameraCache", py::no_init )
	.def( "add", cameraCreator, "Adds a Camera to the cache" )
	.def( "remove", &CameraCache::Remove, "Finds a Camera" )
	.def( "has", &CameraCache::Has, "Tells if the cache has a Camera" )
	.def( "find", &CameraCache::Find, "Finds a Camera in the cache" )
	;
	//@}
	/**@group_name GeometryCache */
	//@{
	GeometrySPtr( GeometryCache::*geometryCreator )( castor::String const &, SceneNodeSPtr, MeshSPtr ) = &GeometryCache::Add;
	py::class_< GeometryCache, boost::noncopyable >( "GeometryCache", py::no_init )
	.def( "add", geometryCreator, "Adds a Geometry to the cache" )
	.def( "remove", &GeometryCache::Remove, "Finds a Geometry" )
	.def( "has", &GeometryCache::Has, "Tells if the cache has a Geometry" )
	.def( "find", &GeometryCache::Find, "Finds a Geometry in the cache" )
	;
	//@}
	/**@group_name LightCache */
	//@{
	LightSPtr( LightCache::*lightCreator )( castor::String const &, SceneNodeSPtr, LightType ) = &LightCache::Add;
	py::class_< LightCache, boost::noncopyable >( "LightCache", py::no_init )
	.def( "add", lightCreator, "Adds a Light to the cache" )
	.def( "remove", &LightCache::Remove, "Removes a Light from the cache" )
	.def( "has", &LightCache::Has, "Tells if the cache has a Light" )
	.def( "find", &LightCache::Find, "Finds a Light in the cache" )
	;
	/**@group_name MeshCache */
	//@{
	py::class_< MeshCache, boost::noncopyable >( "MeshCache", py::no_init )
	.def( "add", &MeshCache::Add<>, "Adds a Mesh to the cache" )
	.def( "remove", &MeshCache::Remove, "Removes a Mesh from the cache" )
	.def( "has", &MeshCache::Has, "Tells if the cache has a Mesh" )
	.def( "find", &MeshCache::Find, "Finds a Mesh in the cache" )
	;
	//@}
	/**@group_name RenderWindowCache */
	//@{
	py::class_< RenderWindowCache, boost::noncopyable >( "RenderWindowCache", py::no_init )
	.def( "add", &RenderWindowCache::Add<>, "Adds a RenderWindow to the cache" )
	.def( "remove", &RenderWindowCache::Remove, "Removes a RenderWindow from the cache" )
	.def( "has", &RenderWindowCache::Has, "Tells if the cache has a RenderWindow" )
	.def( "find", &RenderWindowCache::Find, "Finds a RenderWindow in the cache" )
	;
	//@}
	/**@group_name MaterialCache */
	//@{
	MaterialSPtr( MaterialCache::*mtlmgrCreate )( castor::String const & ) = &MaterialCache::Add;
	py::class_< MaterialCache, boost::noncopyable >( "MaterialCache", py::no_init )
	.def( "add", mtlmgrCreate, "Adds a Material to the cache" )
	.def( "remove", &MaterialCache::Remove, "Removes a Material from the cache" )
	.def( "has", &MaterialCache::Has, "Tells if the cache has a Material" )
	.def( "find", &MaterialCache::Find, "Finds a Material in the cache" )
	;
	//@}
	/**@group_name SceneCache */
	//@{
	SceneSPtr( SceneCache::*scnmgrCreate )( castor::String const & ) = &SceneCache::Add;
	py::class_< SceneCache, boost::noncopyable >( "SceneCache", py::no_init )
	.def( "add", scnmgrCreate, "Adds a Scene to the cache" )
	.def( "remove", &SceneCache::Remove, "Removes a Scene from the cache" )
	.def( "has", &SceneCache::Has, "Tells if the cache has a Scene" )
	.def( "find", &SceneCache::Find, "Finds a Scene in the cache" )
	;
	//@}
	/**@group_name OverlayCache */
	//@{
	OverlaySPtr( OverlayCache::*ovlCreate )( castor::String const &, OverlayType, SceneSPtr, OverlaySPtr ) = &OverlayCache::Add;
	py::class_< OverlayCache, boost::noncopyable >( "OverlayCache", py::no_init )
	.def( "add", ovlCreate, "Adds an Overlay to the cache" )
	.def( "remove", &OverlayCache::Remove, "Removes a Overlay from the cache" )
	.def( "has", &OverlayCache::Has, "Tells if the cache has a Overlay" )
	.def( "find", &OverlayCache::Find, "Finds a Overlay in the cache" )
	;
	//@}
	/**@group_name PluginCache */
	//@{
	PluginSPtr( PluginCache::*pluginLoader )( castor::Path const & ) = &PluginCache::loadPlugin;
	py::class_< PluginCache, boost::noncopyable >( "PluginCache", py::no_init )
	.def( "load_plugin", pluginLoader )
	.def( "load_plugins", &PluginCache::loadAllPlugins )
	.def( "get_plugins", &PluginCache::getPlugins )
	;
	//@}
	/**@group_name Scene */
	//@{
	CACHE_GETTER( Scene, SceneNode );
	CACHE_GETTER( Scene, Camera );
	CACHE_GETTER( Scene, Geometry );
	CACHE_GETTER( Scene, Light );
	CACHE_GETTER( Scene, Mesh );
	py::class_< Scene, boost::noncopyable >( "Scene", py::no_init )
	.add_property( "background_colour", py::make_function( &Scene::getBackgroundColour, py::return_value_policy< py::copy_const_reference >() ), &Scene::setBackgroundColour, "The background colour" )
	.add_property( "name", py::make_function( &Scene::getName, py::return_value_policy< py::copy_const_reference >() ), &Scene::setName, "The scene name" )
	.add_property( "ambient_light", py::make_function( &Scene::getAmbientLight, py::return_value_policy< py::copy_const_reference >() ), &Scene::setAmbientLight, "The ambient light colour" )
	.add_property( "root_node", &Scene::getRootNode, "The root scene node" )
	.add_property( "root_object_node", &Scene::getObjectRootNode, "The objects root scene node" )
	.add_property( "root_camera_node", &Scene::getCameraRootNode, "The cameras root scene node" )
	.def( "cleanup", &Scene::Cleanup )
	.def( "nodes", py::make_function( resgetSceneNodeCache, py::return_value_policy< py::copy_non_const_reference >() ), "The SceneNodes cache" )
	.def( "geometries", py::make_function( resgetGeometryCache, py::return_value_policy< py::copy_non_const_reference >() ), "The Geometries cache" )
	.def( "cameras", py::make_function( resgetCameraCache, py::return_value_policy< py::copy_non_const_reference >() ), "The Cameras cache" )
	.def( "lights", py::make_function( resgetLightCache, py::return_value_policy< py::copy_non_const_reference >() ), "The Lights cache" )
	.def( "meshes", py::make_function( resgetMeshCache, py::return_value_policy< py::copy_non_const_reference >() ), "The Meshs cache" )
	.def( "get_background_image", &Scene::getBackgroundImage )
	.def( "set_background_image", &Scene::setBackground )
	;
	//@}
	//@}
	/**@group_name RenderWindow */
	//@{
	void( RenderWindow::*RenderWindowResizer )( Size const & ) = &RenderWindow::Resize;
	py::class_< RenderWindow, boost::noncopyable >( "RenderWindow", py::no_init )
	.add_property( "samples_count", &RenderWindow::getSamplesCount, &RenderWindow::setSamplesCount, "The samples count, if multisampling" )
	.add_property( "camera", &RenderWindow::getCamera, &RenderWindow::setCamera, "The window camera" )
	.add_property( "viewport", &RenderWindow::getViewportType, &RenderWindow::setViewportType, "The viewport type" )
	.add_property( "scene", &RenderWindow::getScene, &RenderWindow::setScene, "The rendered scene" )
	.add_property( "pixel_format", &RenderWindow::getPixelFormat, &RenderWindow::setPixelFormat, "The window pixel format" )
	.add_property( "size", &RenderWindow::getSize, "The window size" )
	.add_property( "name", py::make_function( &RenderWindow::getName, py::return_value_policy< py::copy_const_reference >() ), "The window name" )
	.def( "initialise", &RenderWindow::Initialise )
	.def( "cleanup", &RenderWindow::Cleanup )
	.def( "resize", RenderWindowResizer )
	;
	//@}
	/**@group_name MovableObject */
	//@{
	py::class_< MovableObject, boost::noncopyable >( "MovableObject", py::no_init )
	.add_property( "name", py::make_function( &MovableObject::getName, py::return_value_policy< py::copy_const_reference >() ), "The object name" )
	.add_property( "type", &MovableObject::getType, "The movable type" )
	.add_property( "scene", py::make_function( &MovableObject::getScene, py::return_value_policy< py::return_by_value >() ), "The object's parent scene" )
	.def( "attach", &MovableObject::attachTo )
	.def( "detach", &MovableObject::Detach )
	;
	//@}
	/**@group_name LightCategory */
	//@{
	py::class_< LightCategory, boost::noncopyable >( "LightCategory", py::no_init )
	.add_property( "colour", cpy::make_getter( &LightCategory::getColour, py::return_value_policy< py::copy_const_reference >() ), cpy::make_setter( &LightCategory::setColour ), "The light colour" )
	.add_property( "ambient_intensity", cpy::make_getter( &LightCategory::getAmbientIntensity ), cpy::make_setter( &LightCategory::setAmbientIntensity ), "The light ambient intensity" )
	.add_property( "diffuse_intensity", cpy::make_getter( &LightCategory::getDiffuseIntensity ), cpy::make_setter( &LightCategory::setDiffuseIntensity ), "The light diffuse intensity" )
	.add_property( "specular_intensity", cpy::make_getter( &LightCategory::getSpecularIntensity ), cpy::make_setter( &LightCategory::setSpecularIntensity ), "The light specular intensity" )
	;
	//@}
	/**@group_name DirectionalLight */
	//@{
	py::class_< DirectionalLight, py::bases< LightCategory >, boost::noncopyable >( "DirectionalLight", py::no_init )
	;
	//@}
	/**@group_name PointLight */
	//@{
	py::class_< PointLight, py::bases< LightCategory >, boost::noncopyable >( "PointLight", py::no_init )
	.add_property( "attenuation", cpy::make_getter( &PointLight::getAttenuation, py::return_value_policy< py::copy_const_reference >() ), cpy::make_setter( &PointLight::setAttenuation ), "The light attenuation values (constant, linear and quadratic)" )
	;
	//@}
	/**@group_name SpotLight */
	//@{
	py::class_< SpotLight, py::bases< LightCategory >, boost::noncopyable >( "SpotLight", py::no_init )
	.add_property( "attenuation", cpy::make_getter( &PointLight::getAttenuation, py::return_value_policy< py::copy_const_reference >() ), cpy::make_setter( &PointLight::setAttenuation ), "The light attenuation values (constant, linear and quadratic)" )
	.add_property( "exponent", &SpotLight::getExponent, &SpotLight::setExponent, "The light exponent value" )
	.add_property( "cut_off", &SpotLight::getCutOff, &SpotLight::setCutOff, "The light cut off value" )
	;
	//@}
	/**@group_name Camera */
	//@{
	void ( Camera::*cameraResizer )( Size const & ) = &Camera::Resize;
	py::class_< Camera, py::bases< MovableObject >, boost::noncopyable >( "Camera", py::no_init )
	.add_property( "viewport", &Camera::getViewportType, &Camera::setViewportType, "The camera viewport type" )
	.add_property( "width", &Camera::getWidth, "The camera horizontal resolution" )
	.add_property( "height", &Camera::getHeight, "The camera vertical resolution" )
	.def( "resize", cameraResizer )
	;
	//@}
	/**@group_name Light */
	//@{
	py::class_< Light, py::bases< MovableObject >, boost::noncopyable >( "Light", py::no_init )
	.add_property( "light_type", &Light::getLightType, "The light type" )
	.add_property( "directional", &Light::getDirectionalLight, "The directional light category" )
	.add_property( "point", &Light::getPointLight, "The point light category" )
	.add_property( "spot", &Light::getSpotLight, "The spot light category" )
	;
	//@}
	/**@group_name Geometry */
	//@{
	py::class_< Geometry, py::bases< MovableObject >, boost::noncopyable >( "Geometry", py::no_init )
	.add_property( "mesh", &Geometry::getMesh, &Geometry::setMesh, "The geometry's mesh" )
	.def( "get_material", &Geometry::getMaterial )
	.def( "set_material", &Geometry::setMaterial )
	;
	//@}
	/**@group_name SceneNode */
	//@{
	py::class_< SceneNode, std::shared_ptr< SceneNode >, boost::noncopyable >( "SceneNode", py::no_init )
	.add_property( "position", cpy::make_getter( &SceneNode::getPosition, py::return_value_policy< py::copy_const_reference >() ), cpy::make_setter( &SceneNode::setPosition ), "The node position" )
	.add_property( "orientation", cpy::make_getter( &SceneNode::getOrientation, py::return_value_policy< py::copy_const_reference >() ), cpy::make_setter( &SceneNode::setOrientation ), "The node orientation" )
	.add_property( "scale", cpy::make_getter( &SceneNode::getScale, py::return_value_policy< py::copy_const_reference >() ), cpy::make_setter( &SceneNode::setScale ), "The node scale" )
	.def( "attach_object", &SceneNode::attachObject )
	.def( "detach_object", &SceneNode::detachObject )
	.def( "attach_to", &SceneNode::attachTo )
	.def( "detach_from", &SceneNode::Detach )
	.def( "yaw", &SceneNode::Yaw )
	.def( "pitch", &SceneNode::Pitch )
	.def( "roll", &SceneNode::Roll )
	.def( "rotate", cpy::make_setter( &SceneNode::Rotate ) )
	.def( "translate", cpy::make_setter( &SceneNode::Translate ) )
	.def( "scale", cpy::make_setter( &SceneNode::Scale ) )
	;
	//@}
	/**@group_name OverlayCategory */
	//@{
	py::class_< OverlayCategory, boost::noncopyable >( "OverlayCategory", py::no_init )
	.add_property( "type", &OverlayCategory::getType, "The overlay type" )
	.add_property( "position", cpy::make_getter( &OverlayCategory::getPosition, py::return_value_policy< py::copy_const_reference >() ), cpy::make_setter( &OverlayCategory::setPosition ), "The overlay position, relative to its parent" )
	.add_property( "size", cpy::make_getter( &OverlayCategory::getSize, py::return_value_policy< py::copy_const_reference >() ), cpy::make_setter( &OverlayCategory::setSize ), "The overlay size, relative to its parent" )
	.add_property( "visible", &Overlay::isVisible, &Overlay::setVisible, "The overlay visibility" )
	.add_property( "material", &Overlay::getMaterial, &Overlay::setMaterial, "The overlay material" )
	;
	//@}
	/**@group_name PanelOverlay */
	//@{
	py::class_< PanelOverlay, py::bases< OverlayCategory >, boost::noncopyable >( "PanelOverlay", py::no_init );
	//@}
	/**@group_name BorderPanelOverlay */
	//@{
	py::class_< BorderPanelOverlay, py::bases< OverlayCategory >, boost::noncopyable >( "BorderPanelOverlay", py::no_init )
	.add_property( "border_left", &BorderPanelOverlay::getLeftBorderSize, &BorderPanelOverlay::setLeftBorderSize, "The bottom border width" )
	.add_property( "border_right", &BorderPanelOverlay::getRightBorderSize, &BorderPanelOverlay::setRightBorderSize, "The right border width" )
	.add_property( "border_top", &BorderPanelOverlay::getTopBorderSize, &BorderPanelOverlay::setTopBorderSize, "The top border width" )
	.add_property( "border_bottom", &BorderPanelOverlay::getBottomBorderSize, &BorderPanelOverlay::setBottomBorderSize, "The bottom border width" )
	.add_property( "border_material", &BorderPanelOverlay::getBorderMaterial, &BorderPanelOverlay::setBorderMaterial, "The border material" )
	;
	//@}
	/**@group_name TextOverlay */
	//@{
	void( TextOverlay::*captionsetter )( castor::String const & ) = &TextOverlay::setCaption;
	py::class_< TextOverlay, py::bases< OverlayCategory >, boost::noncopyable >( "TextOverlay", py::no_init )
	.add_property( "font", py::make_function( &TextOverlay::getFontName, py::return_value_policy< py::copy_const_reference >() ), &TextOverlay::setFont, "The text font" )
	.add_property( "caption", &TextOverlay::getCaption, captionsetter, "The text caption" )
	;
	//@}
	/**@group_name Overlay */
	//@{
	uint32_t( Overlay::*ovgetChildsCount )( )const = &Overlay::getChildrenCount;
	typedef Overlay::iterator( Overlay::*OverlayPtrIntMapItFunc )( );
	py::class_< Overlay, boost::noncopyable >( "Overlay", py::no_init )
	.add_property( "panel", &Overlay::getPanelOverlay, "The panel overlay category" )
	.add_property( "border_panel", &Overlay::getBorderPanelOverlay, "The border panel overlay category" )
	.add_property( "text", &Overlay::getTextOverlay, "The text overlay category" )
	.add_property( "name", py::make_function( &Overlay::getName, py::return_value_policy< py::copy_const_reference >() ), &Overlay::setName, "The overlay name" )
	.add_property( "childs", py::range< OverlayPtrIntMapItFunc, OverlayPtrIntMapItFunc >( &Overlay::begin, &Overlay::end ) )
	.def( "childs_count", ovgetChildsCount )
	.def( "add_child", &Overlay::addChild )
	;
	//@}
	/**@group_name AnimatedSkeleton */
	//@{
	typedef AnimationPtrStrMapIt( AnimatedSkeleton::*AnimatedSkeletonAnimationsItFunc )();
	py::class_< AnimatedSkeleton, boost::noncopyable >( "AnimatedSkeleton", py::no_init )
	.def( "start_all_animations", &AnimatedSkeleton::startAllAnimations )
	.def( "stop_all_animations", &AnimatedSkeleton::stopAllAnimations )
	.def( "pause_all_animations", &AnimatedSkeleton::pauseAllAnimations )
	.def( "start_animation", &AnimatedSkeleton::startAnimation )
	.def( "stop_animation", &AnimatedSkeleton::stopAnimation )
	.def( "pause_animation", &AnimatedSkeleton::stopAnimation )
	.def( "get_animation", py::make_function( &AnimatedSkeleton::getAnimation, py::return_value_policy< py::copy_non_const_reference >{} ) )
	;
	//@}
	/**@group_name AnimatedMesh */
	//@{
	typedef AnimationPtrStrMapIt( AnimatedMesh::*AnimatedMeshAnimationsItFunc )();
	py::class_< AnimatedMesh, boost::noncopyable >( "AnimatedMesh", py::no_init )
	.def( "start_all_animations", &AnimatedMesh::startAllAnimations )
	.def( "stop_all_animations", &AnimatedMesh::stopAllAnimations )
	.def( "pause_all_animations", &AnimatedMesh::pauseAllAnimations )
	.def( "start_animation", &AnimatedMesh::startAnimation )
	.def( "stop_animation", &AnimatedMesh::stopAnimation )
	.def( "pause_animation", &AnimatedMesh::stopAnimation )
	.def( "get_animation", py::make_function( &AnimatedMesh::getAnimation, py::return_value_policy< py::copy_non_const_reference >{} ) )
	;
	//@}
	/**@group_name AnimatedObjectGroup */
	//@{
	typedef castor::StrsetIt( AnimatedObjectGroup::*AnimatedObjectGroupAnimationsItFunc )( );
	typedef AnimatedObjectPtrStrMapIt( AnimatedObjectGroup::*AnimatedObjectGroupObjectsItFunc )( );
	py::class_< AnimatedObjectGroup >( "AnimatedObjectGroup", py::no_init )
	.add_property( "scene", py::make_function( &AnimatedObjectGroup::getScene, py::return_value_policy< py::reference_existing_object >() ) )
	//.def( "objects", &AnimatedObjectGroup::getObjects )
	.def( "start_all_animations", &AnimatedObjectGroup::startAllAnimations )
	.def( "stop_all_animations", &AnimatedObjectGroup::stopAllAnimations )
	.def( "pause_all_animations", &AnimatedObjectGroup::pauseAllAnimations )
	.def( "start_animation", &AnimatedObjectGroup::startAnimation )
	.def( "stop_animation", &AnimatedObjectGroup::stopAnimation )
	.def( "pause_animation", &AnimatedObjectGroup::stopAnimation )
	.def( "add_animation", &AnimatedObjectGroup::addAnimation )
	.def( "set_animation_looped", &AnimatedObjectGroup::setAnimationLooped )
	;
	//@}
	/**@group_name Engine */
	//@{
	CACHE_GETTER( Engine, Scene );
	CACHE_GETTER( Engine, Plugin );
	CACHE_GETTER( Engine, Material );
	CACHE_GETTER( Engine, Overlay );
	CACHE_GETTER( Engine, RenderWindow );
	py::class_< Engine, boost::noncopyable >( "Engine", py::init<>() )
	.def( "initialise", &Engine::Initialise )
	.def( "cleanup", &Engine::Cleanup )
	.def( "load_renderer", &Engine::loadRenderer )
	.def( "scenes", py::make_function( resgetSceneCache, py::return_value_policy< py::copy_non_const_reference >() ) )
	.def( "plugins", py::make_function( resgetPluginCache, py::return_value_policy< py::copy_non_const_reference >() ) )
	.def( "materials", py::make_function( resgetMaterialCache, py::return_value_policy< py::copy_non_const_reference >() ) )
	.def( "overlays", py::make_function( resgetOverlayCache, py::return_value_policy< py::copy_non_const_reference >() ) )
	.def( "render_windows", py::make_function( resgetRenderWindowCache, py::return_value_policy< py::copy_non_const_reference >() ), "The RenderWindows cache" )
	;
	//@}
}
