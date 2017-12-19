/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SCENE_FILE_PARSER_PARSERS_H___
#define ___C3D_SCENE_FILE_PARSER_PARSERS_H___

#include "Castor3DPrerequisites.hpp"
#include "SceneFileParser.hpp"

namespace castor3d
{
	// Root parsers
	DECLARE_ATTRIBUTE_PARSER( parserRootMtlFile )
	DECLARE_ATTRIBUTE_PARSER( parserRootScene )
	DECLARE_ATTRIBUTE_PARSER( parserRootFont )
	DECLARE_ATTRIBUTE_PARSER( parserRootMaterial )
	DECLARE_ATTRIBUTE_PARSER( parserRootSamplerState )
	DECLARE_ATTRIBUTE_PARSER( parserRootPanelOverlay )
	DECLARE_ATTRIBUTE_PARSER( parserRootBorderPanelOverlay )
	DECLARE_ATTRIBUTE_PARSER( parserRootTextOverlay )
	DECLARE_ATTRIBUTE_PARSER( parserRootDebugOverlays )
	DECLARE_ATTRIBUTE_PARSER( parserRootWindow )
	DECLARE_ATTRIBUTE_PARSER( parserRootMaterials )

	//Window parsers
	DECLARE_ATTRIBUTE_PARSER( parserWindowRenderTarget )
	DECLARE_ATTRIBUTE_PARSER( parserWindowVSync )
	DECLARE_ATTRIBUTE_PARSER( parserWindowFullscreen )

	//RenderTarget parsers
	DECLARE_ATTRIBUTE_PARSER( parserRenderTargetScene )
	DECLARE_ATTRIBUTE_PARSER( parserRenderTargetCamera )
	DECLARE_ATTRIBUTE_PARSER( parserRenderTargetSize )
	DECLARE_ATTRIBUTE_PARSER( parserRenderTargetFormat )
	DECLARE_ATTRIBUTE_PARSER( parserRenderTargetStereo )
	DECLARE_ATTRIBUTE_PARSER( parserRenderTargetPostEffect )
	DECLARE_ATTRIBUTE_PARSER( parserRenderTargetToneMapping )
	DECLARE_ATTRIBUTE_PARSER( parserRenderTargetSsao )
	DECLARE_ATTRIBUTE_PARSER( parserRenderTargetEnd )

	// Sampler parsers
	DECLARE_ATTRIBUTE_PARSER( parserSamplerMinFilter )
	DECLARE_ATTRIBUTE_PARSER( parserSamplerMagFilter )
	DECLARE_ATTRIBUTE_PARSER( parserSamplerMipFilter )
	DECLARE_ATTRIBUTE_PARSER( parserSamplerMinLod )
	DECLARE_ATTRIBUTE_PARSER( parserSamplerMaxLod )
	DECLARE_ATTRIBUTE_PARSER( parserSamplerLodBias )
	DECLARE_ATTRIBUTE_PARSER( parserSamplerUWrapMode )
	DECLARE_ATTRIBUTE_PARSER( parserSamplerVWrapMode )
	DECLARE_ATTRIBUTE_PARSER( parserSamplerWWrapMode )
	DECLARE_ATTRIBUTE_PARSER( parserSamplerBorderColour )
	DECLARE_ATTRIBUTE_PARSER( parserSamplerMaxAnisotropy )
	DECLARE_ATTRIBUTE_PARSER( parserSamplerComparisonMode )
	DECLARE_ATTRIBUTE_PARSER( parserSamplerComparisonFunc )

	// Scene parsers
	DECLARE_ATTRIBUTE_PARSER( parserSceneInclude )
	DECLARE_ATTRIBUTE_PARSER( parserSceneBkColour )
	DECLARE_ATTRIBUTE_PARSER( parserSceneBkImage )
	DECLARE_ATTRIBUTE_PARSER( parserSceneFont )
	DECLARE_ATTRIBUTE_PARSER( parserSceneMaterial )
	DECLARE_ATTRIBUTE_PARSER( parserSceneSamplerState )
	DECLARE_ATTRIBUTE_PARSER( parserSceneCamera )
	DECLARE_ATTRIBUTE_PARSER( parserSceneLight )
	DECLARE_ATTRIBUTE_PARSER( parserSceneCameraNode )
	DECLARE_ATTRIBUTE_PARSER( parserSceneSceneNode )
	DECLARE_ATTRIBUTE_PARSER( parserSceneObject )
	DECLARE_ATTRIBUTE_PARSER( parserSceneAmbientLight )
	DECLARE_ATTRIBUTE_PARSER( parserSceneImport )
	DECLARE_ATTRIBUTE_PARSER( parserSceneBillboard )
	DECLARE_ATTRIBUTE_PARSER( parserSceneAnimatedObjectGroup )
	DECLARE_ATTRIBUTE_PARSER( parserScenePanelOverlay )
	DECLARE_ATTRIBUTE_PARSER( parserSceneBorderPanelOverlay )
	DECLARE_ATTRIBUTE_PARSER( parserSceneTextOverlay )
	DECLARE_ATTRIBUTE_PARSER( parserSceneSkybox )
	DECLARE_ATTRIBUTE_PARSER( parserSceneFogType )
	DECLARE_ATTRIBUTE_PARSER( parserSceneFogDensity )
	DECLARE_ATTRIBUTE_PARSER( parserSceneParticleSystem )
	DECLARE_ATTRIBUTE_PARSER( parserSceneHdrConfig )
	DECLARE_ATTRIBUTE_PARSER( parserMesh )

	// ParticleSystem parsers
	DECLARE_ATTRIBUTE_PARSER( parserParticleSystemParent )
	DECLARE_ATTRIBUTE_PARSER( parserParticleSystemCount )
	DECLARE_ATTRIBUTE_PARSER( parserParticleSystemMaterial )
	DECLARE_ATTRIBUTE_PARSER( parserParticleSystemDimensions )
	DECLARE_ATTRIBUTE_PARSER( parserParticleSystemParticle )
	DECLARE_ATTRIBUTE_PARSER( parserParticleSystemTFShader )
	DECLARE_ATTRIBUTE_PARSER( parserParticleSystemCSShader )

	// Particle parsers
	DECLARE_ATTRIBUTE_PARSER( parserParticleType )
	DECLARE_ATTRIBUTE_PARSER( parserParticleVariable )

	// Light parsers
	DECLARE_ATTRIBUTE_PARSER( parserLightParent )
	DECLARE_ATTRIBUTE_PARSER( parserLightType )
	DECLARE_ATTRIBUTE_PARSER( parserLightColour )
	DECLARE_ATTRIBUTE_PARSER( parserLightIntensity )
	DECLARE_ATTRIBUTE_PARSER( parserLightAttenuation )
	DECLARE_ATTRIBUTE_PARSER( parserLightCutOff )
	DECLARE_ATTRIBUTE_PARSER( parserLightExponent )
	DECLARE_ATTRIBUTE_PARSER( parserLightShadowProducer )

	// Scene node parsers
	DECLARE_ATTRIBUTE_PARSER( parserNodeParent )
	DECLARE_ATTRIBUTE_PARSER( parserNodePosition )
	DECLARE_ATTRIBUTE_PARSER( parserNodeOrientation )
	DECLARE_ATTRIBUTE_PARSER( parserNodeDirection )
	DECLARE_ATTRIBUTE_PARSER( parserNodeScale )

	// Object Parsers
	DECLARE_ATTRIBUTE_PARSER( parserObjectParent )
	DECLARE_ATTRIBUTE_PARSER( parserObjectMaterial )
	DECLARE_ATTRIBUTE_PARSER( parserObjectMaterials )
	DECLARE_ATTRIBUTE_PARSER( parserObjectCastShadows )
	DECLARE_ATTRIBUTE_PARSER( parserObjectReceivesShadows )
	DECLARE_ATTRIBUTE_PARSER( parserObjectEnd )

	// Object Materials Parsers
	DECLARE_ATTRIBUTE_PARSER( parserObjectMaterialsMaterial )
	DECLARE_ATTRIBUTE_PARSER( parserObjectMaterialsEnd )

	// Mesh Parsers
	DECLARE_ATTRIBUTE_PARSER( parserMeshType )
	DECLARE_ATTRIBUTE_PARSER( parserMeshSubmesh )
	DECLARE_ATTRIBUTE_PARSER( parserMeshImport )
	DECLARE_ATTRIBUTE_PARSER( parserMeshMorphImport )
	DECLARE_ATTRIBUTE_PARSER( parserMeshDivide )
	DECLARE_ATTRIBUTE_PARSER( parserMeshEnd )

	// Submesh Parsers
	DECLARE_ATTRIBUTE_PARSER( parserSubmeshVertex )
	DECLARE_ATTRIBUTE_PARSER( parserSubmeshUV )
	DECLARE_ATTRIBUTE_PARSER( parserSubmeshUVW )
	DECLARE_ATTRIBUTE_PARSER( parserSubmeshNormal )
	DECLARE_ATTRIBUTE_PARSER( parserSubmeshTangent )
	DECLARE_ATTRIBUTE_PARSER( parserSubmeshFace )
	DECLARE_ATTRIBUTE_PARSER( parserSubmeshFaceUV )
	DECLARE_ATTRIBUTE_PARSER( parserSubmeshFaceUVW )
	DECLARE_ATTRIBUTE_PARSER( parserSubmeshFaceNormals )
	DECLARE_ATTRIBUTE_PARSER( parserSubmeshFaceTangents )
	DECLARE_ATTRIBUTE_PARSER( parserSubmeshEnd )

	// Material Parsers
	DECLARE_ATTRIBUTE_PARSER( parserMaterialType )
	DECLARE_ATTRIBUTE_PARSER( parserMaterialPass )
	DECLARE_ATTRIBUTE_PARSER( parserMaterialEnd )

	// Pass Parsers
	DECLARE_ATTRIBUTE_PARSER( parserPassDiffuse )
	DECLARE_ATTRIBUTE_PARSER( parserPassSpecular )
	DECLARE_ATTRIBUTE_PARSER( parserPassAmbient )
	DECLARE_ATTRIBUTE_PARSER( parserPassEmissive )
	DECLARE_ATTRIBUTE_PARSER( parserPassShininess )
	DECLARE_ATTRIBUTE_PARSER( parserPassAlbedo )
	DECLARE_ATTRIBUTE_PARSER( parserPassRoughness )
	DECLARE_ATTRIBUTE_PARSER( parserPassMetallic )
	DECLARE_ATTRIBUTE_PARSER( parserPassGlossiness )
	DECLARE_ATTRIBUTE_PARSER( parserPassAlpha )
	DECLARE_ATTRIBUTE_PARSER( parserPassdoubleFace )
	DECLARE_ATTRIBUTE_PARSER( parserPassTextureUnit )
	DECLARE_ATTRIBUTE_PARSER( parserPassShader )
	DECLARE_ATTRIBUTE_PARSER( parserPassAlphaBlendMode )
	DECLARE_ATTRIBUTE_PARSER( parserPassColourBlendMode )
	DECLARE_ATTRIBUTE_PARSER( parserPassAlphaFunc )
	DECLARE_ATTRIBUTE_PARSER( parserPassRefractionRatio )
	DECLARE_ATTRIBUTE_PARSER( parserPassSubsurfaceScattering )
	DECLARE_ATTRIBUTE_PARSER( parserPassParallaxOcclusion )
	DECLARE_ATTRIBUTE_PARSER( parserPassEnd )

	// Texture Unit Parsers
	DECLARE_ATTRIBUTE_PARSER( parserUnitImage )
	DECLARE_ATTRIBUTE_PARSER( parserUnitRenderTarget )
	DECLARE_ATTRIBUTE_PARSER( parserUnitChannel )
	DECLARE_ATTRIBUTE_PARSER( parserUnitSampler )
	DECLARE_ATTRIBUTE_PARSER( parserUnitEnd )

	// Shader Parsers
	DECLARE_ATTRIBUTE_PARSER( parserVertexShader )
	DECLARE_ATTRIBUTE_PARSER( parserPixelShader )
	DECLARE_ATTRIBUTE_PARSER( parserGeometryShader )
	DECLARE_ATTRIBUTE_PARSER( parserHullShader )
	DECLARE_ATTRIBUTE_PARSER( parserdomainShader )
	DECLARE_ATTRIBUTE_PARSER( parserComputeShader )
	DECLARE_ATTRIBUTE_PARSER( parserConstantsBuffer )
	DECLARE_ATTRIBUTE_PARSER( parserShaderEnd )

	// Shader Program Parsers
	DECLARE_ATTRIBUTE_PARSER( parserShaderProgramFile )
	DECLARE_ATTRIBUTE_PARSER( parserShaderProgramSampler )
	DECLARE_ATTRIBUTE_PARSER( parserGeometryInputType )
	DECLARE_ATTRIBUTE_PARSER( parserGeometryOutputType )
	DECLARE_ATTRIBUTE_PARSER( parserGeometryOutputVtxCount )

	// Shader Program UBO Parsers
	DECLARE_ATTRIBUTE_PARSER( parserShaderUboShaders )
	DECLARE_ATTRIBUTE_PARSER( parserShaderUboVariable )

	// Shader UBO Variables Parsers
	DECLARE_ATTRIBUTE_PARSER( parserShaderVariableType )
	DECLARE_ATTRIBUTE_PARSER( parserShaderVariableCount )
	DECLARE_ATTRIBUTE_PARSER( parserShaderVariableValue )

	// Font Parsers
	DECLARE_ATTRIBUTE_PARSER( parserFontFile )
	DECLARE_ATTRIBUTE_PARSER( parserFontHeight )
	DECLARE_ATTRIBUTE_PARSER( parserFontEnd )

	//Overlay Parsers
	DECLARE_ATTRIBUTE_PARSER( parserOverlayPosition )
	DECLARE_ATTRIBUTE_PARSER( parserOverlaySize )
	DECLARE_ATTRIBUTE_PARSER( parserOverlayPixelSize )
	DECLARE_ATTRIBUTE_PARSER( parserOverlayPixelPosition )
	DECLARE_ATTRIBUTE_PARSER( parserOverlayMaterial )
	DECLARE_ATTRIBUTE_PARSER( parserOverlayEnd )
	DECLARE_ATTRIBUTE_PARSER( parserOverlayPanelOverlay )
	DECLARE_ATTRIBUTE_PARSER( parserOverlayBorderPanelOverlay )
	DECLARE_ATTRIBUTE_PARSER( parserOverlayTextOverlay )

	//PanelOverlay Parsers
	DECLARE_ATTRIBUTE_PARSER( parserPanelOverlayUvs )

	//BorderPanelOverlay Parsers
	DECLARE_ATTRIBUTE_PARSER( parserBorderPanelOverlaySizes )
	DECLARE_ATTRIBUTE_PARSER( parserBorderPanelOverlayPixelSizes )
	DECLARE_ATTRIBUTE_PARSER( parserBorderPanelOverlayMaterial )
	DECLARE_ATTRIBUTE_PARSER( parserBorderPanelOverlayPosition )
	DECLARE_ATTRIBUTE_PARSER( parserBorderPanelOverlayCenterUvs )
	DECLARE_ATTRIBUTE_PARSER( parserBorderPanelOverlayOuterUvs )
	DECLARE_ATTRIBUTE_PARSER( parserBorderPanelOverlayInnerUvs )

	// TextOverlay Parsers
	DECLARE_ATTRIBUTE_PARSER( parserTextOverlayFont )
	DECLARE_ATTRIBUTE_PARSER( parserTextOverlayText )
	DECLARE_ATTRIBUTE_PARSER( parserTextOverlayTextWrapping )
	DECLARE_ATTRIBUTE_PARSER( parserTextOverlayVerticalAlign )
	DECLARE_ATTRIBUTE_PARSER( parserTextOverlayHorizontalAlign )
	DECLARE_ATTRIBUTE_PARSER( parserTextOverlayTexturingMode )
	DECLARE_ATTRIBUTE_PARSER( parserTextOverlayLineSpacingMode )

	// Camera parsers
	DECLARE_ATTRIBUTE_PARSER( parserCameraParent )
	DECLARE_ATTRIBUTE_PARSER( parserCameraViewport )
	DECLARE_ATTRIBUTE_PARSER( parserCameraPrimitive )
	DECLARE_ATTRIBUTE_PARSER( parserCameraEnd )

	// Viewport parsers
	DECLARE_ATTRIBUTE_PARSER( parserViewportType )
	DECLARE_ATTRIBUTE_PARSER( parserViewportLeft )
	DECLARE_ATTRIBUTE_PARSER( parserViewportRight )
	DECLARE_ATTRIBUTE_PARSER( parserViewportTop )
	DECLARE_ATTRIBUTE_PARSER( parserViewportBottom )
	DECLARE_ATTRIBUTE_PARSER( parserViewportNear )
	DECLARE_ATTRIBUTE_PARSER( parserViewportFar )
	DECLARE_ATTRIBUTE_PARSER( parserViewportSize )
	DECLARE_ATTRIBUTE_PARSER( parserViewportFovY )
	DECLARE_ATTRIBUTE_PARSER( parserViewportAspectRatio )

	// Billboard parsers
	DECLARE_ATTRIBUTE_PARSER( parserBillboardParent )
	DECLARE_ATTRIBUTE_PARSER( parserBillboardType )
	DECLARE_ATTRIBUTE_PARSER( parserBillboardSize )
	DECLARE_ATTRIBUTE_PARSER( parserBillboardPositions )
	DECLARE_ATTRIBUTE_PARSER( parserBillboardMaterial )
	DECLARE_ATTRIBUTE_PARSER( parserBillboardDimensions )
	DECLARE_ATTRIBUTE_PARSER( parserBillboardEnd )
	DECLARE_ATTRIBUTE_PARSER( parserBillboardPoint )

	// Animated object group parsers
	DECLARE_ATTRIBUTE_PARSER( parserAnimatedObjectGroupAnimatedObject )
	DECLARE_ATTRIBUTE_PARSER( parserAnimatedObjectGroupAnimation )
	DECLARE_ATTRIBUTE_PARSER( parserAnimatedObjectGroupAnimationStart )
	DECLARE_ATTRIBUTE_PARSER( parserAnimatedObjectGroupAnimationPause )
	DECLARE_ATTRIBUTE_PARSER( parserAnimatedObjectGroupEnd )

	// Animated object group animation parsers
	DECLARE_ATTRIBUTE_PARSER( parserAnimationLooped )
	DECLARE_ATTRIBUTE_PARSER( parserAnimationScale )
	DECLARE_ATTRIBUTE_PARSER( parserAnimationEnd )

	// Skybox parsers
	DECLARE_ATTRIBUTE_PARSER( parserSkyboxEqui )
	DECLARE_ATTRIBUTE_PARSER( parserSkyboxLeft )
	DECLARE_ATTRIBUTE_PARSER( parserSkyboxRight )
	DECLARE_ATTRIBUTE_PARSER( parserSkyboxTop )
	DECLARE_ATTRIBUTE_PARSER( parserSkyboxBottom )
	DECLARE_ATTRIBUTE_PARSER( parserSkyboxFront )
	DECLARE_ATTRIBUTE_PARSER( parserSkyboxBack )
	DECLARE_ATTRIBUTE_PARSER( parserSkyboxEnd )

	// SSAO parsers
	DECLARE_ATTRIBUTE_PARSER( parserSsaoEnabled )
	DECLARE_ATTRIBUTE_PARSER( parserSsaoRadius )
	DECLARE_ATTRIBUTE_PARSER( parserSsaoBias )
	DECLARE_ATTRIBUTE_PARSER( parserSsaoIntensity )
	DECLARE_ATTRIBUTE_PARSER( parserSsaoEnd )

	// Subsurface Scattering parsers
	DECLARE_ATTRIBUTE_PARSER( parserSubsurfaceScatteringStrength )
	DECLARE_ATTRIBUTE_PARSER( parserSubsurfaceScatteringGaussianWidth )
	DECLARE_ATTRIBUTE_PARSER( parserSubsurfaceScatteringTransmittanceProfile )
	DECLARE_ATTRIBUTE_PARSER( parserSubsurfaceScatteringEnd )

	// Transmittance Profile parsers
	DECLARE_ATTRIBUTE_PARSER( parserTransmittanceProfileFactor )

	// HDR config parsers
	DECLARE_ATTRIBUTE_PARSER( parserHdrExponent )
	DECLARE_ATTRIBUTE_PARSER( parserHdrGamma )
}

#endif
