/*
See LICENSE file in root folder
*/
#ifndef ___CC3D_Castor3D_H___
#define ___CC3D_Castor3D_H___

#include "Castor3DEnums.h"
#include "Castor3DTypes.h"

#include <stdint.h>
#include <stdbool.h>
#include <uchar.h>

#if !defined( _WIN32 )
#	define C3D_CAPI
#else
#	if defined( CCastor3D_EXPORTS )
#		define C3D_CAPI __declspec( dllexport )
#	else
#		define C3D_CAPI __declspec( dllimport )
#	endif
#endif

#if defined( _WIN32 )
#	include <Windows.h>
#	define C3D_CAPIMETHOD( FuncName ) C3D_CAPI C3D_RESULT __stdcall FuncName
#	define C3D_CAPIMETHODIMP C3D_RESULT __stdcall
#else
#	define C3D_CAPIMETHOD( FuncName ) C3D_CAPI C3D_RESULT FuncName
#	define C3D_CAPIMETHODIMP C3D_RESULT
#endif

#ifdef __cplusplus
extern "C"
{
#endif

	// C3DVec2
	C3D_CAPIMETHOD( c3dVec2_negate )( C3DVec2 * object );
	C3D_CAPIMETHOD( c3dVec2_normalise )( C3DVec2 * object );
	C3D_CAPIMETHOD( c3dVec2_length )( C3DVec2 const * object, float * result );
	C3D_CAPIMETHOD( c3dVec2_dot )( C3DVec2 const * lhs, C3DVec2 const * rhs, float * result );
	C3D_CAPIMETHOD( c3dVec2_compMul )( C3DVec2 const * lhs, C3DVec2 const * rhs, C3DVec2 * result );
	C3D_CAPIMETHOD( c3dVec2_compAdd )( C3DVec2 const * lhs, C3DVec2 const * rhs, C3DVec2 * result );
	C3D_CAPIMETHOD( c3dVec2_compSub )( C3DVec2 const * lhs, C3DVec2 const * rhs, C3DVec2 * result );
	C3D_CAPIMETHOD( c3dVec2_mul )( C3DVec2 const * lhs, float rhs, C3DVec2 * result );
	C3D_CAPIMETHOD( c3dVec2_div )( C3DVec2 const * lhs, float rhs, C3DVec2 * result );
	C3D_CAPIMETHOD( c3dVec2_add )( C3DVec2 const * lhs, float rhs, C3DVec2 * result );
	C3D_CAPIMETHOD( c3dVec2_sub )( C3DVec2 const * lhs, float rhs, C3DVec2 * result );

	// C3DVec3
	C3D_CAPIMETHOD( c3dVec3_negate )( C3DVec3 * object );
	C3D_CAPIMETHOD( c3dVec3_normalise )( C3DVec3 * object );
	C3D_CAPIMETHOD( c3dVec3_length )( C3DVec3 const * object, float * result );
	C3D_CAPIMETHOD( c3dVec3_dot )( C3DVec3 const * lhs, C3DVec3 const * rhs, float * result );
	C3D_CAPIMETHOD( c3dVec3_cross )( C3DVec3 const * lhs, C3DVec3 const * rhs, C3DVec3 * result );
	C3D_CAPIMETHOD( c3dVec3_compMul )( C3DVec3 const * lhs, C3DVec3 const * rhs, C3DVec3 * result );
	C3D_CAPIMETHOD( c3dVec3_compAdd )( C3DVec3 const * lhs, C3DVec3 const * rhs, C3DVec3 * result );
	C3D_CAPIMETHOD( c3dVec3_compSub )( C3DVec3 const * lhs, C3DVec3 const * rhs, C3DVec3 * result );
	C3D_CAPIMETHOD( c3dVec3_mul )( C3DVec3 const * lhs, float rhs, C3DVec3 * result );
	C3D_CAPIMETHOD( c3dVec3_div )( C3DVec3 const * lhs, float rhs, C3DVec3 * result );
	C3D_CAPIMETHOD( c3dVec3_add )( C3DVec3 const * lhs, float rhs, C3DVec3 * result );
	C3D_CAPIMETHOD( c3dVec3_sub )( C3DVec3 const * lhs, float rhs, C3DVec3 * result );

	// C3DQuat
	C3D_CAPIMETHOD( c3dQuat_fromMatrix )( C3DMat4 const * matrix, C3DQuat * result );
	C3D_CAPIMETHOD( c3dQuat_fromAxisAngle )( C3DVec3 const * axis, float angle, C3DQuat * result );
	C3D_CAPIMETHOD( c3dQuat_fromAxes )( C3DVec3 const * x, C3DVec3 const * y, C3DVec3 const * z, C3DQuat * result );
	C3D_CAPIMETHOD( c3dQuat_getRotationMatrix )( C3DQuat const * object, C3DMat4 * result );
	C3D_CAPIMETHOD( c3dQuat_getAxisAngle )( C3DQuat const * object, C3DVec3 * axis, float * angle );
	C3D_CAPIMETHOD( c3dQuat_getAxes )( C3DQuat const * object, C3DVec3 * x, C3DVec3 * y, C3DVec3 * z );
	C3D_CAPIMETHOD( c3dQuat_getPitch )( C3DQuat const * object, float * result );
	C3D_CAPIMETHOD( c3dQuat_getYaw )( C3DQuat const * object, float * result );
	C3D_CAPIMETHOD( c3dQuat_getRoll )( C3DQuat const * object, float * result );
	C3D_CAPIMETHOD( c3dQuat_transform )( C3DQuat const * object, C3DVec3 const * val, C3DVec3 * result );
	C3D_CAPIMETHOD( c3dQuat_getMagnitude )( C3DQuat const * object, float * result );
	C3D_CAPIMETHOD( c3dQuat_conjugate )( C3DQuat * object );
	C3D_CAPIMETHOD( c3dQuat_slerp )( C3DQuat const * lhs, C3DQuat const * rhs, float percent, C3DQuat * result );
	C3D_CAPIMETHOD( c3dQuat_mix )( C3DQuat const * lhs, C3DQuat const * rhs, float percent, C3DQuat * result );
	C3D_CAPIMETHOD( c3dQuat_mul )( C3DQuat const * lhs, C3DQuat const * rhs, C3DQuat * result );

	// C3DMat4
	C3D_CAPIMETHOD( c3dMat4_setIdentity )( C3DMat4 * object );
	C3D_CAPIMETHOD( c3dMat4_transpose )( C3DMat4 * object );
	C3D_CAPIMETHOD( c3dMat4_invert )( C3DMat4 * object );
	C3D_CAPIMETHOD( c3dMat4_compMul )( C3DMat4 const * lhs, C3DMat4 const * rhs, C3DMat4 * result );
	C3D_CAPIMETHOD( c3dMat4_mul )( C3DMat4 const * lhs, C3DMat4 const * rhs, C3DMat4 * result );

	// C3DGlyph
	C3D_CAPIMETHOD( c3dGlyph_delete )( C3DGlyph * object );
	C3D_CAPIMETHOD( c3dGlyph_getSize )( C3DGlyph const * object, C3DVec2 * result );
	C3D_CAPIMETHOD( c3dGlyph_getBearing )( C3DGlyph const * object, C3DVec2 * result );
	C3D_CAPIMETHOD( c3dGlyph_getAdvance )( C3DGlyph const * object, float * result );
	C3D_CAPIMETHOD( c3dGlyph_getBitmapSize )( C3DGlyph const * object, C3DSize * result );

	// C3DFont
	C3D_CAPIMETHOD( c3dFont_delete )( C3DFont * object );
	C3D_CAPIMETHOD( c3dFont_getHeight )( C3DFont const * object, uint32_t * result );
	C3D_CAPIMETHOD( c3dFont_getMaxGlyphHeight )( C3DFont const * object, uint32_t * result );
	C3D_CAPIMETHOD( c3dFont_getMaxGlyphWidth )( C3DFont const * object, uint32_t * result );
	C3D_CAPIMETHOD( c3dFont_getMaxImageHeight )( C3DFont const * object, uint32_t * result );
	C3D_CAPIMETHOD( c3dFont_getMaxImageWidth )( C3DFont const * object, uint32_t * result );
	C3D_CAPIMETHOD( c3dFont_getMaxBearing )( C3DFont const * object, C3DVec2 * result );
	C3D_CAPIMETHOD( c3dFont_getGlyph )( C3DFont const * object, char32_t index, C3DGlyph ** result );

	// C3DPixelBuffer
	C3D_CAPIMETHOD( c3dPixelBuffer_create )( C3DSize const * size, C3D_PIXEL_FORMAT pf, C3DPixelBuffer ** result );
	C3D_CAPIMETHOD( c3dPixelBuffer_delete )( C3DPixelBuffer * object );
	C3D_CAPIMETHOD( c3dPixelBuffer_getDimensions )( C3DPixelBuffer const * object, C3DSize * result );
	C3D_CAPIMETHOD( c3dPixelBuffer_getWidth )( C3DPixelBuffer const * object, uint32_t * result );
	C3D_CAPIMETHOD( c3dPixelBuffer_getHeight )( C3DPixelBuffer const * object, uint32_t * result );
	C3D_CAPIMETHOD( c3dPixelBuffer_getPixelFormat )( C3DPixelBuffer const * object, C3D_PIXEL_FORMAT * result );

	// C3DImage
	C3D_CAPIMETHOD( c3dImage_delete )( C3DImage * object );
	C3D_CAPIMETHOD( c3dImage_getBuffer )( C3DImage const * object, C3DPixelBuffer ** result );
	C3D_CAPIMETHOD( c3dImage_resample )( C3DImage const * object, C3DSize const * val );
	C3D_CAPIMETHOD( c3dImage_fill )( C3DImage const * object, C3DRgbaColour const * val );

	// C3DLogger
	C3D_CAPIMETHOD( c3dLogger_create )( C3D_LOG_TYPE level, C3DLogger ** result );
	C3D_CAPIMETHOD( c3dLogger_delete )( C3DLogger * object );
	C3D_CAPIMETHOD( c3dLogger_setFileName )( C3DLogger const * object, C3DString name, C3D_LOG_TYPE target );
	C3D_CAPIMETHOD( c3dLogger_logTrace )( C3DLogger const * object, C3DString msg );
	C3D_CAPIMETHOD( c3dLogger_logDebug )( C3DLogger const * object, C3DString msg );
	C3D_CAPIMETHOD( c3dLogger_logInfo )( C3DLogger const * object, C3DString msg );
	C3D_CAPIMETHOD( c3dLogger_logWarning )( C3DLogger const * object, C3DString msg );
	C3D_CAPIMETHOD( c3dLogger_logError )( C3DLogger const * object, C3DString msg );

	// C3DRenderTarget
	C3D_CAPIMETHOD( c3dRenderTarget_delete )( C3DRenderTarget * object );
	C3D_CAPIMETHOD( c3dRenderTarget_initialise )( C3DRenderTarget * object );
	C3D_CAPIMETHOD( c3dRenderTarget_cleanup )( C3DRenderTarget const * object );
	C3D_CAPIMETHOD( c3dRenderTarget_getCamera )( C3DRenderTarget const * object, C3DCamera ** result );
	C3D_CAPIMETHOD( c3dRenderTarget_setCamera )( C3DRenderTarget const * object, C3DCamera const * camera );
	C3D_CAPIMETHOD( c3dRenderTarget_getViewportType )( C3DRenderTarget const * object, C3D_VIEWPORT_TYPE * result );
	C3D_CAPIMETHOD( c3dRenderTarget_setViewportType )( C3DRenderTarget const * object, C3D_VIEWPORT_TYPE val );
	C3D_CAPIMETHOD( c3dRenderTarget_getScene )( C3DRenderTarget const * object, C3DScene ** result );
	C3D_CAPIMETHOD( c3dRenderTarget_setScene )( C3DRenderTarget const * object, C3DScene const * val );
	C3D_CAPIMETHOD( c3dRenderTarget_getPixelFormat )( C3DRenderTarget const * object, C3D_PIXEL_FORMAT * result );
	C3D_CAPIMETHOD( c3dRenderTarget_getRenderSize )( C3DRenderTarget const * object, C3DSize * result );
	C3D_CAPIMETHOD( c3dRenderTarget_getDisplaySize )( C3DRenderTarget const * object, C3DSize * result );

	// C3DSampler
	C3D_CAPIMETHOD( c3dSampler_delete )( C3DSampler * object );
	C3D_CAPIMETHOD( c3dSampler_getMinFilter )( C3DSampler const * object, C3D_FILTER_MODE * result );
	C3D_CAPIMETHOD( c3dSampler_setMinFilter )( C3DSampler const * object, C3D_FILTER_MODE val );
	C3D_CAPIMETHOD( c3dSampler_getMagFilter )( C3DSampler const * object, C3D_FILTER_MODE * result );
	C3D_CAPIMETHOD( c3dSampler_setMagFilter )( C3DSampler const * object, C3D_FILTER_MODE val );
	C3D_CAPIMETHOD( c3dSampler_getMipmapMode )( C3DSampler const * object, C3D_MIPMAP_MODE * result );
	C3D_CAPIMETHOD( c3dSampler_setMipmapMode )( C3DSampler const * object, C3D_MIPMAP_MODE val );
	C3D_CAPIMETHOD( c3dSampler_getWrapModeU )( C3DSampler const * object, C3D_WRAP_MODE * result );
	C3D_CAPIMETHOD( c3dSampler_setWrapModeU )( C3DSampler const * object, C3D_WRAP_MODE val );
	C3D_CAPIMETHOD( c3dSampler_getWrapModeV )( C3DSampler const * object, C3D_WRAP_MODE * result );
	C3D_CAPIMETHOD( c3dSampler_setWrapModeV )( C3DSampler const * object, C3D_WRAP_MODE val );
	C3D_CAPIMETHOD( c3dSampler_getWrapModeW )( C3DSampler const * object, C3D_WRAP_MODE * result );
	C3D_CAPIMETHOD( c3dSampler_setWrapModeW )( C3DSampler const * object, C3D_WRAP_MODE val );
	C3D_CAPIMETHOD( c3dSampler_getMaxAnisotropy )( C3DSampler const * object, float * result );
	C3D_CAPIMETHOD( c3dSampler_setMaxAnisotropy )( C3DSampler const * object, float val );
	C3D_CAPIMETHOD( c3dSampler_getMinLod )( C3DSampler const * object, float * result );
	C3D_CAPIMETHOD( c3dSampler_setMinLod )( C3DSampler const * object, float val );
	C3D_CAPIMETHOD( c3dSampler_getMaxLod )( C3DSampler const * object, float * result );
	C3D_CAPIMETHOD( c3dSampler_setMaxLod )( C3DSampler const * object, float val );
	C3D_CAPIMETHOD( c3dSampler_getLodBias )( C3DSampler const * object, float * result );
	C3D_CAPIMETHOD( c3dSampler_setLodBias )( C3DSampler const * object, float val );
	C3D_CAPIMETHOD( c3dSampler_getBorderColour )( C3DSampler const * object, C3D_BORDER_COLOUR * result );
	C3D_CAPIMETHOD( c3dSampler_setBorderColour )( C3DSampler const * object, C3D_BORDER_COLOUR val );

	// C3DPass
	C3D_CAPIMETHOD( c3dPass_delete )( C3DPass * object );
	C3D_CAPIMETHOD( c3dPass_getTwoSided )( C3DPass const * object, bool * result );
	C3D_CAPIMETHOD( c3dPass_setTwoSided )( C3DPass const * object, bool val );
	C3D_CAPIMETHOD( c3dPass_getOpacity )( C3DPass const * object, float * result );
	C3D_CAPIMETHOD( c3dPass_setOpacity )( C3DPass const * object, float val );
	C3D_CAPIMETHOD( c3dPass_getEmissiveColour )( C3DPass const * object, C3DRgbColour * result );
	C3D_CAPIMETHOD( c3dPass_setEmissiveColour )( C3DPass const * object, C3DRgbColour const * val );
	C3D_CAPIMETHOD( c3dPass_getEmissiveFactor )( C3DPass const * object, float * result );
	C3D_CAPIMETHOD( c3dPass_setEmissiveFactor )( C3DPass const * object, float val );
	C3D_CAPIMETHOD( c3dPass_getRefractionRatio )( C3DPass const * object, float * result );
	C3D_CAPIMETHOD( c3dPass_setRefractionRatio )( C3DPass const * object, float val );
	C3D_CAPIMETHOD( c3dPass_getAlphaFunc )( C3DPass const * object, C3D_COMPARISON_FUNC * result );
	C3D_CAPIMETHOD( c3dPass_setAlphaFunc )( C3DPass const * object, C3D_COMPARISON_FUNC val );
	C3D_CAPIMETHOD( c3dPass_getBlendAlphaFunc )( C3DPass const * object, C3D_COMPARISON_FUNC * result );
	C3D_CAPIMETHOD( c3dPass_setBlendAlphaFunc )( C3DPass const * object, C3D_COMPARISON_FUNC val );
	C3D_CAPIMETHOD( c3dPass_getAlphaRefValue )( C3DPass const * object, float * v );
	C3D_CAPIMETHOD( c3dPass_setAlphaRefValue )( C3DPass const * object, float val );
	C3D_CAPIMETHOD( c3dPass_getDiffuse )( C3DPass const * object, C3DRgbColour * result );
	C3D_CAPIMETHOD( c3dPass_setDiffuse )( C3DPass const * object, C3DRgbColour const * val );
	C3D_CAPIMETHOD( c3dPass_getAlbedo )( C3DPass const * object, C3DHdrRgbColour * result );
	C3D_CAPIMETHOD( c3dPass_setAlbedo )( C3DPass const * object, C3DHdrRgbColour const * val );
	C3D_CAPIMETHOD( c3dPass_getSpecular )( C3DPass const * object, C3DRgbColour * result );
	C3D_CAPIMETHOD( c3dPass_setSpecular )( C3DPass const * object, C3DRgbColour const * val );
	C3D_CAPIMETHOD( c3dPass_getSpecularFactor )( C3DPass const * object, float * result );
	C3D_CAPIMETHOD( c3dPass_setSpecularFactor )( C3DPass const * object, float val );
	C3D_CAPIMETHOD( c3dPass_getMetalness )( C3DPass const * object, float * result );
	C3D_CAPIMETHOD( c3dPass_setMetalness )( C3DPass const * object, float val );
	C3D_CAPIMETHOD( c3dPass_getShininess )( C3DPass const * object, float * result );
	C3D_CAPIMETHOD( c3dPass_setShininess )( C3DPass const * object, float val );
	C3D_CAPIMETHOD( c3dPass_getRoughness )( C3DPass const * object, float * result );
	C3D_CAPIMETHOD( c3dPass_setRoughness )( C3DPass const * object, float val );
	C3D_CAPIMETHOD( c3dPass_getGlossiness )( C3DPass const * object, float * result );
	C3D_CAPIMETHOD( c3dPass_setGlossiness )( C3DPass const * object, float val );

	// C3DMaterial
	C3D_CAPIMETHOD( c3dMaterial_delete )( C3DMaterial * object );
	C3D_CAPIMETHOD( c3dMaterial_getPassCount )( C3DMaterial const * object, uint32_t * result );
	C3D_CAPIMETHOD( c3dMaterial_getPass )( C3DMaterial const * object, uint32_t index, C3DPass ** result );
	C3D_CAPIMETHOD( c3dMaterial_createPass )( C3DMaterial const * object, C3DPass ** result );
	C3D_CAPIMETHOD( c3dMaterial_removePass )( C3DMaterial const * object, C3DPass * pass );

	// C3DTriFaceMapping
	C3D_CAPIMETHOD( c3dTriFaceMapping_delete )( C3DTriFaceMapping * object );
	C3D_CAPIMETHOD( c3dTriFaceMapping_getFacesCount )( C3DTriFaceMapping const * object, uint32_t * result );
	C3D_CAPIMETHOD( c3dTriFaceMapping_addFace )( C3DTriFaceMapping const * object, uint32_t x, uint32_t y, uint32_t z );

	// C3DLineMapping
	C3D_CAPIMETHOD( c3dLineMapping_delete )( C3DLineMapping * object );
	C3D_CAPIMETHOD( c3dLineMapping_getLinesCount )( C3DLineMapping const * object, uint32_t * result );
	C3D_CAPIMETHOD( c3dLineMapping_addLine )( C3DLineMapping const * object, uint32_t x, uint32_t y );

	// C3DSubmesh
	C3D_CAPIMETHOD( c3dSubmesh_delete )( C3DSubmesh * object );
	C3D_CAPIMETHOD( c3dSubmesh_getPointsCount )( C3DSubmesh const * object, uint32_t * result );
	C3D_CAPIMETHOD( c3dSubmesh_getIndexMappingType )( C3DSubmesh const * object, C3D_INDEX_MAPPING_TYPE * result );
	C3D_CAPIMETHOD( c3dSubmesh_getTriFaceMapping )( C3DSubmesh const * object, C3DTriFaceMapping ** result );
	C3D_CAPIMETHOD( c3dSubmesh_getLineMapping )( C3DSubmesh const * object, C3DLineMapping ** result );
	C3D_CAPIMETHOD( c3dSubmesh_addPoint )( C3DSubmesh const * object, C3DVec3 const * val );

	// C3DMesh
	C3D_CAPIMETHOD( c3dMesh_delete )( C3DMesh * object );
	C3D_CAPIMETHOD( c3dMesh_getSubmeshCount )( C3DMesh const * object, uint32_t * result );
	C3D_CAPIMETHOD( c3dMesh_getSubmesh )( C3DMesh const * object, uint32_t val, C3DSubmesh ** result );
	C3D_CAPIMETHOD( c3dMesh_createSubmesh )( C3DMesh const * object, C3DSubmesh ** result );
	C3D_CAPIMETHOD( c3dMesh_removeSubmesh )( C3DMesh const * object, C3DSubmesh * val );

	// C3DSkybox
	C3D_CAPIMETHOD( c3dSkybox_delete )( C3DSkybox * object );
	C3D_CAPIMETHOD( c3dSkybox_setLeftImage )( C3DSkybox const * object, C3DString filePath );
	C3D_CAPIMETHOD( c3dSkybox_setRightImage )( C3DSkybox const * object, C3DString filePath );
	C3D_CAPIMETHOD( c3dSkybox_setTopImage )( C3DSkybox const * object, C3DString filePath );
	C3D_CAPIMETHOD( c3dSkybox_setBottomImage )( C3DSkybox const * object, C3DString filePath );
	C3D_CAPIMETHOD( c3dSkybox_setFrontImage )( C3DSkybox const * object, C3DString filePath );
	C3D_CAPIMETHOD( c3dSkybox_setBackImage )( C3DSkybox const * object, C3DString filePath );
	C3D_CAPIMETHOD( c3dSkybox_setCrossImage )( C3DSkybox const * object, C3DString filePath );
	C3D_CAPIMETHOD( c3dSkybox_setEquirectangularImage )( C3DSkybox const * object, C3DString filePath, uint32_t size );

	// C3DShadow
	C3D_CAPIMETHOD( c3dShadow_delete )( C3DShadow * object );
	C3D_CAPIMETHOD( c3dShadow_getEnabled )( C3DShadow const * object, bool * result );
	C3D_CAPIMETHOD( c3dShadow_setEnabled )( C3DShadow const * object, bool val );
	C3D_CAPIMETHOD( c3dShadow_getFilter )( C3DShadow const * object, C3D_SHADOW_TYPE * result );
	C3D_CAPIMETHOD( c3dShadow_setFilter )( C3DShadow const * object, C3D_SHADOW_TYPE val );
	C3D_CAPIMETHOD( c3dShadow_getRawMinOffset )( C3DShadow const * object, float * result );
	C3D_CAPIMETHOD( c3dShadow_setRawMinOffset )( C3DShadow const * object, float val );
	C3D_CAPIMETHOD( c3dShadow_getRawMaxSlopeOffset )( C3DShadow const * object, float * result );
	C3D_CAPIMETHOD( c3dShadow_setRawMaxSlopeOffset )( C3DShadow const * object, float val );
	C3D_CAPIMETHOD( c3dShadow_getPcfMinOffset )( C3DShadow const * object, float * result );
	C3D_CAPIMETHOD( c3dShadow_setPcfMinOffset )( C3DShadow const * object, float val );
	C3D_CAPIMETHOD( c3dShadow_getPcfMaxSlopeOffset )( C3DShadow const * object, float * result );
	C3D_CAPIMETHOD( c3dShadow_setPcfMaxSlopeOffset )( C3DShadow const * object, float val );
	C3D_CAPIMETHOD( c3dShadow_getPcfFilterSize )( C3DShadow const * object, uint32_t * result );
	C3D_CAPIMETHOD( c3dShadow_setPcfFilterSize )( C3DShadow const * object, uint32_t val );
	C3D_CAPIMETHOD( c3dShadow_getPcfSampleCount )( C3DShadow const * object, uint32_t * result );
	C3D_CAPIMETHOD( c3dShadow_setPcfSampleCount )( C3DShadow const * object, uint32_t val );
	C3D_CAPIMETHOD( c3dShadow_getVsmMinVariance )( C3DShadow const * object, float * result );
	C3D_CAPIMETHOD( c3dShadow_setVsmMinVariance )( C3DShadow const * object, float val );
	C3D_CAPIMETHOD( c3dShadow_getVsmLightBleedingReduction )( C3DShadow const * object, float * result );
	C3D_CAPIMETHOD( c3dShadow_setVsmLightBleedingReduction )( C3DShadow const * object, float val );

	// C3DDirectionalLight
	C3D_CAPIMETHOD( c3dDirectionalLight_delete )( C3DDirectionalLight * object );
	C3D_CAPIMETHOD( c3dDirectionalLight_getColour )( C3DDirectionalLight const * object, C3DVec3 * result );
	C3D_CAPIMETHOD( c3dDirectionalLight_setColour )( C3DDirectionalLight const * object, C3DVec3 const * val );
	C3D_CAPIMETHOD( c3dDirectionalLight_getShadows )( C3DDirectionalLight const * object, C3DShadow ** result );
	C3D_CAPIMETHOD( c3dDirectionalLight_getIllumination )( C3DDirectionalLight const * object, float * result );
	C3D_CAPIMETHOD( c3dDirectionalLight_setIllumination )( C3DDirectionalLight const * object, float val );

	// C3DPointLight
	C3D_CAPIMETHOD( c3dPointLight_delete )( C3DPointLight * object );
	C3D_CAPIMETHOD( c3dPointLight_getColour )( C3DPointLight const * object, C3DVec3 * result );
	C3D_CAPIMETHOD( c3dPointLight_setColour )( C3DPointLight const * object, C3DVec3 const * val );
	C3D_CAPIMETHOD( c3dPointLight_getShadows )( C3DPointLight const * object, C3DShadow ** result );
	C3D_CAPIMETHOD( c3dPointLight_getAttenuationRange )( C3DPointLight const * object, float * result );
	C3D_CAPIMETHOD( c3dPointLight_setAttenuationRange )( C3DPointLight const * object, float val );
	C3D_CAPIMETHOD( c3dPointLight_getIntensity )( C3DPointLight const * object, float * result );
	C3D_CAPIMETHOD( c3dPointLight_setIntensity )( C3DPointLight const * object, float val );

	// C3DSpotLight
	C3D_CAPIMETHOD( c3dSpotLight_delete )( C3DSpotLight * object );
	C3D_CAPIMETHOD( c3dSpotLight_getColour )( C3DSpotLight const * object, C3DVec3 * result );
	C3D_CAPIMETHOD( c3dSpotLight_setColour )( C3DSpotLight const * object, C3DVec3 const * val );
	C3D_CAPIMETHOD( c3dSpotLight_getShadows )( C3DSpotLight const * object, C3DShadow ** result );
	C3D_CAPIMETHOD( c3dSpotLight_getAttenuationRange )( C3DSpotLight const * object, float * result );
	C3D_CAPIMETHOD( c3dSpotLight_setAttenuationRange )( C3DSpotLight const * object, float val );
	C3D_CAPIMETHOD( c3dSpotLight_getIntensity )( C3DSpotLight const * object, float * result );
	C3D_CAPIMETHOD( c3dSpotLight_setIntensity )( C3DSpotLight const * object, float val );
	C3D_CAPIMETHOD( c3dSpotLight_getExponent )( C3DSpotLight const * object, float * result );
	C3D_CAPIMETHOD( c3dSpotLight_setExponent )( C3DSpotLight const * object, float val );
	C3D_CAPIMETHOD( c3dSpotLight_getInnerCutOff )( C3DSpotLight const * object, float * result );
	C3D_CAPIMETHOD( c3dSpotLight_setInnerCutOff )( C3DSpotLight const * object, float val );
	C3D_CAPIMETHOD( c3dSpotLight_getOuterCutOff )( C3DSpotLight const * object, float * result );
	C3D_CAPIMETHOD( c3dSpotLight_setOuterCutOff )( C3DSpotLight const * object, float val );

	// C3DCamera
	C3D_CAPIMETHOD( c3dCamera_delete )( C3DCamera * object );
	C3D_CAPIMETHOD( c3dCamera_getName )( C3DCamera const * object, C3DString * result );
	C3D_CAPIMETHOD( c3dCamera_getScene )( C3DCamera const * object, C3DScene ** result );
	C3D_CAPIMETHOD( c3dCamera_getNode )( C3DCamera const * object, C3DSceneNode ** result );
	C3D_CAPIMETHOD( c3dCamera_attachTo )( C3DCamera const * object, C3DSceneNode const * val );
	C3D_CAPIMETHOD( c3dCamera_detach )( C3DCamera const * object );
	C3D_CAPIMETHOD( c3dCamera_getViewportType )( C3DCamera const * object, C3D_VIEWPORT_TYPE * result );
	C3D_CAPIMETHOD( c3dCamera_setViewportType )( C3DCamera const * object, C3D_VIEWPORT_TYPE val );

	// C3DLight
	C3D_CAPIMETHOD( c3dLight_delete )( C3DLight * object );
	C3D_CAPIMETHOD( c3dLight_getName )( C3DLight const * object, C3DString * result );
	C3D_CAPIMETHOD( c3dLight_getScene )( C3DLight const * object, C3DScene ** result );
	C3D_CAPIMETHOD( c3dLight_getNode )( C3DLight const * object, C3DSceneNode ** result );
	C3D_CAPIMETHOD( c3dLight_attachTo )( C3DLight const * object, C3DSceneNode const * val );
	C3D_CAPIMETHOD( c3dLight_detach )( C3DLight const * object );
	C3D_CAPIMETHOD( c3dLight_getLightType )( C3DLight const * object, C3D_LIGHT_TYPE * result );
	C3D_CAPIMETHOD( c3dLight_getDirectionalLight )( C3DLight const * object, C3DDirectionalLight ** result );
	C3D_CAPIMETHOD( c3dLight_getPointLight )( C3DLight const * object, C3DPointLight ** result );
	C3D_CAPIMETHOD( c3dLight_getSpotLight )( C3DLight const * object, C3DSpotLight ** result );

	// C3DLightGroup
	C3D_CAPIMETHOD( c3dLightGroup_delete )( C3DLightGroup * object );
	C3D_CAPIMETHOD( c3dLightGroup_getName )( C3DLightGroup const * object, C3DString * result );
	C3D_CAPIMETHOD( c3dLightGroup_getScene )( C3DLightGroup const * object, C3DScene ** result );
	C3D_CAPIMETHOD( c3dLightGroup_addLight )( C3DLightGroup const * object, C3DSceneNode const * val );
	C3D_CAPIMETHOD( c3dLightGroup_removeLight )( C3DLightGroup const * object, C3DSceneNode const * val );
	C3D_CAPIMETHOD( c3dLightGroup_getLightType )( C3DLightGroup const * object, C3D_LIGHT_TYPE * result );
	C3D_CAPIMETHOD( c3dLightGroup_getDirectionalLight )( C3DLightGroup const * object, C3DDirectionalLight ** result );
	C3D_CAPIMETHOD( c3dLightGroup_getPointLight )( C3DLightGroup const * object, C3DPointLight ** result );
	C3D_CAPIMETHOD( c3dLightGroup_getSpotLight )( C3DLightGroup const * object, C3DSpotLight ** result );

	// C3DGeometry
	C3D_CAPIMETHOD( c3dGeometry_delete )( C3DGeometry * object );
	C3D_CAPIMETHOD( c3dGeometry_getName )( C3DGeometry const * object, C3DString * result );
	C3D_CAPIMETHOD( c3dGeometry_getScene )( C3DGeometry const * object, C3DScene ** result );
	C3D_CAPIMETHOD( c3dGeometry_getNode )( C3DGeometry const * object, C3DSceneNode ** result );
	C3D_CAPIMETHOD( c3dGeometry_attachTo )( C3DGeometry const * object, C3DSceneNode const * val );
	C3D_CAPIMETHOD( c3dGeometry_detach )( C3DGeometry const * object );
	C3D_CAPIMETHOD( c3dGeometry_getMesh )( C3DGeometry const * object, C3DMesh ** result );
	C3D_CAPIMETHOD( c3dGeometry_setMesh )( C3DGeometry const * object, C3DMesh const * val );
	C3D_CAPIMETHOD( c3dGeometry_getMaterial )( C3DGeometry const * object, C3DSubmesh const * submesh, C3DMaterial ** result );
	C3D_CAPIMETHOD( c3dGeometry_setMaterial )( C3DGeometry const * object, C3DSubmesh const * submesh, C3DMaterial const * val );

	// C3DSceneNode
	C3D_CAPIMETHOD( c3dSceneNode_delete )( C3DSceneNode * object );
	C3D_CAPIMETHOD( c3dSceneNode_getPosition )( C3DSceneNode const * object, C3DVec3 * result );
	C3D_CAPIMETHOD( c3dSceneNode_setPosition )( C3DSceneNode const * object, C3DVec3 const * val );
	C3D_CAPIMETHOD( c3dSceneNode_getOrientation )( C3DSceneNode const * object, C3DQuat * result );
	C3D_CAPIMETHOD( c3dSceneNode_setOrientation )( C3DSceneNode const * object, C3DQuat const * val );
	C3D_CAPIMETHOD( c3dSceneNode_getScaling )( C3DSceneNode const * object, C3DVec3 * result );
	C3D_CAPIMETHOD( c3dSceneNode_setScaling )( C3DSceneNode const * object, C3DVec3 const * val );
	C3D_CAPIMETHOD( c3dSceneNode_getParent )( C3DSceneNode const * object, C3DSceneNode ** result );
	C3D_CAPIMETHOD( c3dSceneNode_attachTo )( C3DSceneNode const * object, C3DSceneNode const * val );
	C3D_CAPIMETHOD( c3dSceneNode_detach )( C3DSceneNode const * object );
	C3D_CAPIMETHOD( c3dSceneNode_yaw )( C3DSceneNode const * object, float val );
	C3D_CAPIMETHOD( c3dSceneNode_pitch )( C3DSceneNode const * object, float val );
	C3D_CAPIMETHOD( c3dSceneNode_roll )( C3DSceneNode const * object, float val );
	C3D_CAPIMETHOD( c3dSceneNode_rotate )( C3DSceneNode const * object, C3DQuat const * val );
	C3D_CAPIMETHOD( c3dSceneNode_translate )( C3DSceneNode const * object, C3DVec3 const * val );
	C3D_CAPIMETHOD( c3dSceneNode_scale )( C3DSceneNode const * object, C3DVec3 const * val );

	// C3DPanelOverlay
	C3D_CAPIMETHOD( c3dPanelOverlay_delete )( C3DPanelOverlay * object );
	C3D_CAPIMETHOD( c3dPanelOverlay_getPosition )( C3DPanelOverlay const * object, C3DPosition * result );
	C3D_CAPIMETHOD( c3dPanelOverlay_setPosition )( C3DPanelOverlay const * object, C3DPosition const * val );
	C3D_CAPIMETHOD( c3dPanelOverlay_getSize )( C3DPanelOverlay const * object, C3DSize * result );
	C3D_CAPIMETHOD( c3dPanelOverlay_setSize )( C3DPanelOverlay const * object, C3DSize const * val );
	C3D_CAPIMETHOD( c3dPanelOverlay_getVisible )( C3DPanelOverlay const * object, bool * result );
	C3D_CAPIMETHOD( c3dPanelOverlay_setVisible )( C3DPanelOverlay const * object, bool val );
	C3D_CAPIMETHOD( c3dPanelOverlay_getMaterial )( C3DPanelOverlay const * object, C3DMaterial ** result );
	C3D_CAPIMETHOD( c3dPanelOverlay_setMaterial )( C3DPanelOverlay const * object, C3DMaterial const * val );

	// C3DBorderPanelOverlay
	C3D_CAPIMETHOD( c3dBorderPanelOverlay_delete )( C3DBorderPanelOverlay * object );
	C3D_CAPIMETHOD( c3dBorderPanelOverlay_getPosition )( C3DBorderPanelOverlay const * object, C3DPosition * result );
	C3D_CAPIMETHOD( c3dBorderPanelOverlay_setPosition )( C3DBorderPanelOverlay const * object, C3DPosition const * val );
	C3D_CAPIMETHOD( c3dBorderPanelOverlay_getSize )( C3DBorderPanelOverlay const * object, C3DSize * result );
	C3D_CAPIMETHOD( c3dBorderPanelOverlay_setSize )( C3DBorderPanelOverlay const * object, C3DSize const * val );
	C3D_CAPIMETHOD( c3dBorderPanelOverlay_getVisible )( C3DBorderPanelOverlay const * object, bool * result );
	C3D_CAPIMETHOD( c3dBorderPanelOverlay_setVisible )( C3DBorderPanelOverlay const * object, bool val );
	C3D_CAPIMETHOD( c3dBorderPanelOverlay_getMaterial )( C3DBorderPanelOverlay const * object, C3DMaterial ** result );
	C3D_CAPIMETHOD( c3dBorderPanelOverlay_setMaterial )( C3DBorderPanelOverlay const * object, C3DMaterial const * val );
	C3D_CAPIMETHOD( c3dBorderPanelOverlay_getLeftBorderSize )( C3DBorderPanelOverlay const * object, uint32_t * result );
	C3D_CAPIMETHOD( c3dBorderPanelOverlay_setLeftBorderSize )( C3DBorderPanelOverlay const * object, uint32_t val );
	C3D_CAPIMETHOD( c3dBorderPanelOverlay_getRightBorderSize )( C3DBorderPanelOverlay const * object, uint32_t * result );
	C3D_CAPIMETHOD( c3dBorderPanelOverlay_setRightBorderSize )( C3DBorderPanelOverlay const * object, uint32_t val );
	C3D_CAPIMETHOD( c3dBorderPanelOverlay_getTopBorderSize )( C3DBorderPanelOverlay const * object, uint32_t * result );
	C3D_CAPIMETHOD( c3dBorderPanelOverlay_setTopBorderSize )( C3DBorderPanelOverlay const * object, uint32_t val );
	C3D_CAPIMETHOD( c3dBorderPanelOverlay_getBottomBorderSize )( C3DBorderPanelOverlay const * object, uint32_t * result );
	C3D_CAPIMETHOD( c3dBorderPanelOverlay_setBottomBorderSize )( C3DBorderPanelOverlay const * object, uint32_t val );
	C3D_CAPIMETHOD( c3dBorderPanelOverlay_getBorderMaterial )( C3DBorderPanelOverlay const * object, C3DMaterial ** result );
	C3D_CAPIMETHOD( c3dBorderPanelOverlay_setBorderMaterial )( C3DBorderPanelOverlay const * object, C3DMaterial const * val );

	// C3DTextOverlay
	C3D_CAPIMETHOD( c3dTextOverlay_delete )( C3DTextOverlay * object );
	C3D_CAPIMETHOD( c3dTextOverlay_getPosition )( C3DTextOverlay const * object, C3DPosition * result );
	C3D_CAPIMETHOD( c3dTextOverlay_setPosition )( C3DTextOverlay const * object, C3DPosition const * val );
	C3D_CAPIMETHOD( c3dTextOverlay_getSize )( C3DTextOverlay const * object, C3DSize * result );
	C3D_CAPIMETHOD( c3dTextOverlay_setSize )( C3DTextOverlay const * object, C3DSize const * val );
	C3D_CAPIMETHOD( c3dTextOverlay_getVisible )( C3DTextOverlay const * object, bool * result );
	C3D_CAPIMETHOD( c3dTextOverlay_setVisible )( C3DTextOverlay const * object, bool val );
	C3D_CAPIMETHOD( c3dTextOverlay_getMaterial )( C3DTextOverlay const * object, C3DMaterial ** result );
	C3D_CAPIMETHOD( c3dTextOverlay_setMaterial )( C3DTextOverlay const * object, C3DMaterial const * val );
	C3D_CAPIMETHOD( c3dTextOverlay_getFont )( C3DTextOverlay const * object, C3DString * result );
	C3D_CAPIMETHOD( c3dTextOverlay_setFont )( C3DTextOverlay const * object, C3DString val );
	C3D_CAPIMETHOD( c3dTextOverlay_getCaption )( C3DTextOverlay const * object, C3DString * result );
	C3D_CAPIMETHOD( c3dTextOverlay_setCaption )( C3DTextOverlay const * object, C3DString val );

	// C3DOverlay
	C3D_CAPIMETHOD( c3dOverlay_delete )( C3DOverlay * object );
	C3D_CAPIMETHOD( c3dOverlay_getName )( C3DOverlay const * object, C3DString * result );
	C3D_CAPIMETHOD( c3dOverlay_getOverlayType )( C3DOverlay const * object, C3D_OVERLAY_TYPE * result );
	C3D_CAPIMETHOD( c3dOverlay_getPanelOverlay )( C3DOverlay const * object, C3DPanelOverlay ** result );
	C3D_CAPIMETHOD( c3dOverlay_getBorderPanelOverlay )( C3DOverlay const * object, C3DBorderPanelOverlay ** result );
	C3D_CAPIMETHOD( c3dOverlay_getTextOverlay )( C3DOverlay const * object, C3DTextOverlay ** result );
	C3D_CAPIMETHOD( c3dOverlay_getChildrenCount )( C3DOverlay const * object, uint32_t level, uint32_t * result );

	// C3DScene
	C3D_CAPIMETHOD( c3dScene_delete )( C3DScene * object );
	C3D_CAPIMETHOD( c3dScene_getBackgroundColour )( C3DScene const * object, C3DRgbColour * result );
	C3D_CAPIMETHOD( c3dScene_setBackgroundColour )( C3DScene const * object, C3DRgbColour const * val );
	C3D_CAPIMETHOD( c3dScene_setBackgroundImage )( C3DScene const * object, C3DString filePath );
	C3D_CAPIMETHOD( c3dScene_setBackgroundSkybox )( C3DScene const * object, C3DSkybox * skybox );
	C3D_CAPIMETHOD( c3dScene_getName )( C3DScene const * object, C3DString * result );
	C3D_CAPIMETHOD( c3dScene_getAmbientLight )( C3DScene const * object, C3DRgbColour * result );
	C3D_CAPIMETHOD( c3dScene_setAmbientLight )( C3DScene const * object, C3DRgbColour const * val );
	C3D_CAPIMETHOD( c3dScene_getRootNode )( C3DScene const * object, C3DSceneNode ** result );
	C3D_CAPIMETHOD( c3dScene_getObjectRootNode )( C3DScene const * object, C3DSceneNode ** result );
	C3D_CAPIMETHOD( c3dScene_getCameraRootNode )( C3DScene const * object, C3DSceneNode ** result );
	C3D_CAPIMETHOD( c3dScene_addNode )( C3DScene const * object, C3DSceneNode * result );
	C3D_CAPIMETHOD( c3dScene_addGeometry )( C3DScene const * object, C3DGeometry * result );
	C3D_CAPIMETHOD( c3dScene_addCamera )( C3DScene const * object, C3DCamera * result );
	C3D_CAPIMETHOD( c3dScene_addLight )( C3DScene const * object, C3DLight * result );
	C3D_CAPIMETHOD( c3dScene_addLightGroup )( C3DScene const * object, C3DLightGroup * result );
	C3D_CAPIMETHOD( c3dScene_addMesh )( C3DScene const * object, C3DMesh * result );
	C3D_CAPIMETHOD( c3dScene_removeNode )( C3DScene const * object, C3DSceneNode * node );
	C3D_CAPIMETHOD( c3dScene_removeGeometry )( C3DScene const * object, C3DGeometry * geometry );
	C3D_CAPIMETHOD( c3dScene_removeCamera )( C3DScene const * object, C3DCamera * camera );
	C3D_CAPIMETHOD( c3dScene_removeLight )( C3DScene const * object, C3DLight * light );
	C3D_CAPIMETHOD( c3dScene_removeLightGroup )( C3DScene const * object, C3DLightGroup * group );
	C3D_CAPIMETHOD( c3dScene_removeMesh )( C3DScene const * object, C3DMesh * mesh );
	C3D_CAPIMETHOD( c3dScene_getNode )( C3DScene const * object, C3DString name, C3DSceneNode ** result );
	C3D_CAPIMETHOD( c3dScene_getGeometry )( C3DScene const * object, C3DString name, C3DGeometry ** result );
	C3D_CAPIMETHOD( c3dScene_getCamera )( C3DScene const * object, C3DString name, C3DCamera ** result );
	C3D_CAPIMETHOD( c3dScene_getLight )( C3DScene const * object, C3DString name, C3DLight ** result );
	C3D_CAPIMETHOD( c3dScene_getLightGroup )( C3DScene const * object, C3DString name, C3DLightGroup ** result );
	C3D_CAPIMETHOD( c3dScene_getMesh )( C3DScene const * object, C3DString name, C3DMesh ** result );
	C3D_CAPIMETHOD( c3dScene_createNode )( C3DScene const * object, C3DString name, C3DSceneNode const * parent, C3DSceneNode ** result );
	C3D_CAPIMETHOD( c3dScene_createGeometry )( C3DScene const * object, C3DString name, C3DMesh const * mesh, C3DSceneNode const * parent, C3DGeometry ** result );
	C3D_CAPIMETHOD( c3dScene_createCamera )( C3DScene const * object, C3DString name, C3DSceneNode const * node, C3DCamera ** result );
	C3D_CAPIMETHOD( c3dScene_createLight )( C3DScene const * object, C3DString name, C3DSceneNode const * node, C3D_LIGHT_TYPE type, C3DLight ** result );
	C3D_CAPIMETHOD( c3dScene_createLightGroup )( C3DScene const * object, C3DString name, C3D_LIGHT_TYPE type, C3DLightGroup ** result );
	C3D_CAPIMETHOD( c3dScene_createMesh )( C3DScene const * object, C3DString type, C3DString name, C3DMesh ** result );

	// C3DRenderWindow
	C3D_CAPIMETHOD( c3dRenderWindow_delete )( C3DRenderWindow * val );
	C3D_CAPIMETHOD( c3dRenderWindow_getName )( C3DRenderWindow const * object, C3DString * result );
	C3D_CAPIMETHOD( c3dRenderWindow_initialise )( C3DRenderWindow const * object, C3DRenderTarget const * target );
	C3D_CAPIMETHOD( c3dRenderWindow_cleanup )( C3DRenderWindow const * object );
	C3D_CAPIMETHOD( c3dRenderWindow_resize )( C3DRenderWindow const * object, C3DSize const * size );
	C3D_CAPIMETHOD( c3dRenderWindow_onMouseMove )( C3DRenderWindow const * object, C3DPosition const * pos, bool isCtrlDown, bool isAltDown, bool isShiftDown, bool * result );
	C3D_CAPIMETHOD( c3dRenderWindow_onMouseLButtonDown )( C3DRenderWindow const * object, bool isCtrlDown, bool isAltDown, bool isShiftDown, bool * result );
	C3D_CAPIMETHOD( c3dRenderWindow_onMouseLButtonUp )( C3DRenderWindow const * object, bool isCtrlDown, bool isAltDown, bool isShiftDown, bool * result );
	C3D_CAPIMETHOD( c3dRenderWindow_onMouseMButtonDown )( C3DRenderWindow const * object, bool isCtrlDown, bool isAltDown, bool isShiftDown, bool * result );
	C3D_CAPIMETHOD( c3dRenderWindow_onMouseMButtonUp )( C3DRenderWindow const * object, bool isCtrlDown, bool isAltDown, bool isShiftDown, bool * result );
	C3D_CAPIMETHOD( c3dRenderWindow_onMouseRButtonDown )( C3DRenderWindow const * object, bool isCtrlDown, bool isAltDown, bool isShiftDown, bool * result );
	C3D_CAPIMETHOD( c3dRenderWindow_onMouseRButtonUp )( C3DRenderWindow const * object, bool isCtrlDown, bool isAltDown, bool isShiftDown, bool * result );
	C3D_CAPIMETHOD( c3dRenderWindow_onKeyboardKeyDown )( C3DRenderWindow const * object, C3D_KEYBOARD_KEY key, bool isCtrlDown, bool isAltDown, bool isShiftDown, bool * result );
	C3D_CAPIMETHOD( c3dRenderWindow_onKeyboardKeyUp )( C3DRenderWindow const * object, C3D_KEYBOARD_KEY key, bool isCtrlDown, bool isAltDown, bool isShiftDown, bool * result );
	C3D_CAPIMETHOD( c3dRenderWindow_onKeyboardChar )( C3DRenderWindow const * object, C3D_KEYBOARD_KEY key, C3DString c, bool * result );

	// C3DEngine
	C3D_CAPIMETHOD( c3dEngine_create )( C3DString appName, bool enableValidation, C3DEngine ** result );
	C3D_CAPIMETHOD( c3dEngine_delete )( C3DEngine * object );
	C3D_CAPIMETHOD( c3dEngine_initialise )( C3DEngine const * object, uint32_t fps, bool threaded );
	C3D_CAPIMETHOD( c3dEngine_cleanup )( C3DEngine const * object );
	C3D_CAPIMETHOD( c3dEngine_clearScenes )( C3DEngine const * object );
	C3D_CAPIMETHOD( c3dEngine_loadRenderer )( C3DEngine const * object, C3DString type );
	C3D_CAPIMETHOD( c3dEngine_loadPlugin )( C3DEngine const * object, C3DString path );
	C3D_CAPIMETHOD( c3dEngine_loadScene )( C3DEngine const * object, C3DString name, C3DRenderTarget ** pTarget );
	C3D_CAPIMETHOD( c3dEngine_loadImageFromFile )( C3DEngine const * object, C3DString name, C3DString path, C3DImage ** result );
	C3D_CAPIMETHOD( c3dEngine_loadImageFromFormat )( C3DEngine const * object, C3DString name, C3D_PIXEL_FORMAT fmt, C3DSize const * size, C3DImage ** result );
	C3D_CAPIMETHOD( c3dEngine_loadFontFromFile )( C3DEngine const * object, C3DString name, C3DString path, uint32_t height, C3DFont ** result );
	C3D_CAPIMETHOD( c3dEngine_addScene )( C3DEngine const * object, C3DScene * val );
	C3D_CAPIMETHOD( c3dEngine_addOverlay )( C3DEngine const * object, C3DOverlay * val );
	C3D_CAPIMETHOD( c3dEngine_addSampler )( C3DEngine const * object, C3DSampler * val );
	C3D_CAPIMETHOD( c3dEngine_removeScene )( C3DEngine const * object, C3DScene * val );
	C3D_CAPIMETHOD( c3dEngine_removeOverlay )( C3DEngine const * object, C3DOverlay * val );
	C3D_CAPIMETHOD( c3dEngine_removeSampler )( C3DEngine const * object, C3DSampler * val );
	C3D_CAPIMETHOD( c3dEngine_getScene )( C3DEngine const * object, C3DString name, C3DScene ** result );
	C3D_CAPIMETHOD( c3dEngine_getOverlay )( C3DEngine const * object, C3DString name, C3DOverlay ** result );
	C3D_CAPIMETHOD( c3dEngine_getSampler )( C3DEngine const * object, C3DString name, C3DSampler ** result );
	C3D_CAPIMETHOD( c3dEngine_renderOneFrame )( C3DEngine const * object );
	C3D_CAPIMETHOD( c3dEngine_startRendering )( C3DEngine const * object );
	C3D_CAPIMETHOD( c3dEngine_pauseRendering )( C3DEngine const * object );
	C3D_CAPIMETHOD( c3dEngine_resumeRendering )( C3DEngine const * object );
	C3D_CAPIMETHOD( c3dEngine_endRendering )( C3DEngine const * object );
	C3D_CAPIMETHOD( c3dEngine_registerGuiCallbacks )( C3DEngine const * object, C3DGuiCallbacks * callbacks );
	C3D_CAPIMETHOD( c3dEngine_unregisterGuiCallbacks )( C3DEngine const * object );
	C3D_CAPIMETHOD( c3dEngine_postEvent )( C3DEngine const * object, C3DEvent event );
	C3D_CAPIMETHOD( c3dEngine_createOverlay )( C3DEngine const * object, C3D_OVERLAY_TYPE type, C3DString name, C3DOverlay const * parent, C3DOverlay ** result );
	C3D_CAPIMETHOD( c3dEngine_createRenderWindow )( C3DEngine const * object, C3DString name, C3DSize const * size, C3DWindowHandle handle, C3DRenderWindow ** result );
	C3D_CAPIMETHOD( c3dEngine_createSampler )( C3DEngine const * object, C3DString name, C3DSampler ** result );
	C3D_CAPIMETHOD( c3dEngine_createScene )( C3DEngine const * object, C3DString name, C3DScene ** result );
	C3D_CAPIMETHOD( c3dEngine_createSkybox )( C3DEngine const * object, C3DScene const * scene, C3DSkybox ** result );

	// Castor3D
	C3D_CAPIMETHOD( c3d_getPluginsDirectory )( C3DChar * result, size_t resultSize );
	C3D_CAPIMETHOD( c3d_getEngineDirectory )( C3DChar * result, size_t resultSize );
	C3D_CAPIMETHOD( c3d_getDataDirectory )( C3DChar * result, size_t resultSize );

#ifdef __cplusplus
}
#endif

#endif
