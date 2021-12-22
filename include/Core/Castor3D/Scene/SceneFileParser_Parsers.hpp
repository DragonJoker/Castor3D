/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SceneFileParser_Parsers_H___
#define ___C3D_SceneFileParser_Parsers_H___

#include "Castor3D/Scene/SceneFileParser.hpp"

namespace castor3d
{
	// General parsers
	CU_DeclareAttributeParser( parserMaterial )
	CU_DeclareAttributeParser( parserSamplerState )

	// Root parsers
	CU_DeclareAttributeParser( parserRootScene )
	CU_DeclareAttributeParser( parserRootLoadingScreen )
	CU_DeclareAttributeParser( parserRootFont )
	CU_DeclareAttributeParser( parserRootPanelOverlay )
	CU_DeclareAttributeParser( parserRootBorderPanelOverlay )
	CU_DeclareAttributeParser( parserRootTextOverlay )
	CU_DeclareAttributeParser( parserRootDebugOverlays )
	CU_DeclareAttributeParser( parserRootWindow )
	CU_DeclareAttributeParser( parserRootMaterials )
	CU_DeclareAttributeParser( parserRootLpvGridSize )

	//Window parsers
	CU_DeclareAttributeParser( parserWindowRenderTarget )
	CU_DeclareAttributeParser( parserWindowVSync )
	CU_DeclareAttributeParser( parserWindowFullscreen )
	CU_DeclareAttributeParser( parserWindowEnd )

	//RenderTarget parsers
	CU_DeclareAttributeParser( parserRenderTargetScene )
	CU_DeclareAttributeParser( parserRenderTargetCamera )
	CU_DeclareAttributeParser( parserRenderTargetSize )
	CU_DeclareAttributeParser( parserRenderTargetFormat )
	CU_DeclareAttributeParser( parserRenderTargetStereo )
	CU_DeclareAttributeParser( parserRenderTargetPostEffect )
	CU_DeclareAttributeParser( parserRenderTargetToneMapping )
	CU_DeclareAttributeParser( parserRenderTargetSsao )
	CU_DeclareAttributeParser( parserRenderTargetEnd )

	// Sampler parsers
	CU_DeclareAttributeParser( parserSamplerMinFilter )
	CU_DeclareAttributeParser( parserSamplerMagFilter )
	CU_DeclareAttributeParser( parserSamplerMipFilter )
	CU_DeclareAttributeParser( parserSamplerMinLod )
	CU_DeclareAttributeParser( parserSamplerMaxLod )
	CU_DeclareAttributeParser( parserSamplerLodBias )
	CU_DeclareAttributeParser( parserSamplerUWrapMode )
	CU_DeclareAttributeParser( parserSamplerVWrapMode )
	CU_DeclareAttributeParser( parserSamplerWWrapMode )
	CU_DeclareAttributeParser( parserSamplerBorderColour )
	CU_DeclareAttributeParser( parserSamplerAnisotropicFiltering )
	CU_DeclareAttributeParser( parserSamplerMaxAnisotropy )
	CU_DeclareAttributeParser( parserSamplerComparisonMode )
	CU_DeclareAttributeParser( parserSamplerComparisonFunc )
	CU_DeclareAttributeParser( parserSamplerEnd )

	// Scene parsers
	CU_DeclareAttributeParser( parserSceneBkColour )
	CU_DeclareAttributeParser( parserSceneBkImage )
	CU_DeclareAttributeParser( parserSceneFont )
	CU_DeclareAttributeParser( parserSceneCamera )
	CU_DeclareAttributeParser( parserSceneLight )
	CU_DeclareAttributeParser( parserSceneCameraNode )
	CU_DeclareAttributeParser( parserSceneSceneNode )
	CU_DeclareAttributeParser( parserSceneObject )
	CU_DeclareAttributeParser( parserSceneAmbientLight )
	CU_DeclareAttributeParser( parserSceneImport )
	CU_DeclareAttributeParser( parserSceneBillboard )
	CU_DeclareAttributeParser( parserSceneAnimatedObjectGroup )
	CU_DeclareAttributeParser( parserScenePanelOverlay )
	CU_DeclareAttributeParser( parserSceneBorderPanelOverlay )
	CU_DeclareAttributeParser( parserSceneTextOverlay )
	CU_DeclareAttributeParser( parserSceneSkybox )
	CU_DeclareAttributeParser( parserSceneFogType )
	CU_DeclareAttributeParser( parserSceneFogDensity )
	CU_DeclareAttributeParser( parserSceneParticleSystem )
	CU_DeclareAttributeParser( parserMesh )
	CU_DeclareAttributeParser( parserDirectionalShadowCascades )
	CU_DeclareAttributeParser( parserLpvIndirectAttenuation )
	CU_DeclareAttributeParser( parserVoxelConeTracing )
	CU_DeclareAttributeParser( parserSceneEnd )

	// ParticleSystem parsers
	CU_DeclareAttributeParser( parserParticleSystemParent )
	CU_DeclareAttributeParser( parserParticleSystemCount )
	CU_DeclareAttributeParser( parserParticleSystemMaterial )
	CU_DeclareAttributeParser( parserParticleSystemDimensions )
	CU_DeclareAttributeParser( parserParticleSystemParticle )
	CU_DeclareAttributeParser( parserParticleSystemCSShader )
	CU_DeclareAttributeParser( parserParticleSystemEnd )

	// Particle parsers
	CU_DeclareAttributeParser( parserParticleType )
	CU_DeclareAttributeParser( parserParticleVariable )

	// Light parsers
	CU_DeclareAttributeParser( parserLightParent )
	CU_DeclareAttributeParser( parserLightType )
	CU_DeclareAttributeParser( parserLightColour )
	CU_DeclareAttributeParser( parserLightIntensity )
	CU_DeclareAttributeParser( parserLightAttenuation )
	CU_DeclareAttributeParser( parserLightCutOff )
	CU_DeclareAttributeParser( parserLightExponent )
	CU_DeclareAttributeParser( parserLightShadows )
	CU_DeclareAttributeParser( parserLightShadowProducer )
	CU_DeclareAttributeParser( parserLightEnd )

	// Shadows parsers
	CU_DeclareAttributeParser( parserShadowsProducer )
	CU_DeclareAttributeParser( parserShadowsFilter )
	CU_DeclareAttributeParser( parserShadowsGlobalIllumination )
	CU_DeclareAttributeParser( parserShadowsVolumetricSteps )
	CU_DeclareAttributeParser( parserShadowsVolumetricScatteringFactor )
	CU_DeclareAttributeParser( parserShadowsRawConfig )
	CU_DeclareAttributeParser( parserShadowsPcfConfig )
	CU_DeclareAttributeParser( parserShadowsVsmConfig )
	CU_DeclareAttributeParser( parserShadowsRsmConfig )
	CU_DeclareAttributeParser( parserShadowsLpvConfig )

	// Raw filter shadow parsers
	CU_DeclareAttributeParser( parserShadowsRawMinOffset )
	CU_DeclareAttributeParser( parserShadowsRawMaxSlopeOffset )

	// PCF shadow parsers
	CU_DeclareAttributeParser( parserShadowsPcfMinOffset )
	CU_DeclareAttributeParser( parserShadowsPcfMaxSlopeOffset )

	// VSM shadow parsers
	CU_DeclareAttributeParser( parserShadowsVsmVarianceMax )
	CU_DeclareAttributeParser( parserShadowsVsmVarianceBias )

	// RSM parsers
	CU_DeclareAttributeParser( parserRsmIntensity )
	CU_DeclareAttributeParser( parserRsmMaxRadius )
	CU_DeclareAttributeParser( parserRsmSampleCount )

	// LPV parsers
	CU_DeclareAttributeParser( parserLpvTexelAreaModifier )

	// Scene node parsers
	CU_DeclareAttributeParser( parserNodeParent )
	CU_DeclareAttributeParser( parserNodePosition )
	CU_DeclareAttributeParser( parserNodeOrientation )
	CU_DeclareAttributeParser( parserNodeDirection )
	CU_DeclareAttributeParser( parserNodeScale )
	CU_DeclareAttributeParser( parserNodeEnd )

	// Object Parsers
	CU_DeclareAttributeParser( parserObjectParent )
	CU_DeclareAttributeParser( parserObjectMaterial )
	CU_DeclareAttributeParser( parserObjectMaterials )
	CU_DeclareAttributeParser( parserObjectCastShadows )
	CU_DeclareAttributeParser( parserObjectReceivesShadows )
	CU_DeclareAttributeParser( parserObjectCulled )
	CU_DeclareAttributeParser( parserObjectEnd )

	// Object Materials Parsers
	CU_DeclareAttributeParser( parserObjectMaterialsMaterial )
	CU_DeclareAttributeParser( parserObjectMaterialsEnd )

	// Mesh Parsers
	CU_DeclareAttributeParser( parserMeshType )
	CU_DeclareAttributeParser( parserMeshSubmesh )
	CU_DeclareAttributeParser( parserMeshImport )
	CU_DeclareAttributeParser( parserMeshMorphImport )
	CU_DeclareAttributeParser( parserMeshDivide )
	CU_DeclareAttributeParser( parserMeshDefaultMaterial )
	CU_DeclareAttributeParser( parserMeshDefaultMaterials )
	CU_DeclareAttributeParser( parserMeshEnd )

	// Object Materials Parsers
	CU_DeclareAttributeParser( parserMeshDefaultMaterialsMaterial )
	CU_DeclareAttributeParser( parserMeshDefaultMaterialsEnd )

	// Submesh Parsers
	CU_DeclareAttributeParser( parserSubmeshVertex )
	CU_DeclareAttributeParser( parserSubmeshUV )
	CU_DeclareAttributeParser( parserSubmeshUVW )
	CU_DeclareAttributeParser( parserSubmeshNormal )
	CU_DeclareAttributeParser( parserSubmeshTangent )
	CU_DeclareAttributeParser( parserSubmeshFace )
	CU_DeclareAttributeParser( parserSubmeshFaceUV )
	CU_DeclareAttributeParser( parserSubmeshFaceUVW )
	CU_DeclareAttributeParser( parserSubmeshFaceNormals )
	CU_DeclareAttributeParser( parserSubmeshFaceTangents )
	CU_DeclareAttributeParser( parserSubmeshEnd )

	// Material Parsers
	CU_DeclareAttributeParser( parserMaterialType )
	CU_DeclareAttributeParser( parserMaterialPass )
	CU_DeclareAttributeParser( parserMaterialRenderPass )
	CU_DeclareAttributeParser( parserMaterialEnd )

	// Texture Config Parsers
	CU_DeclareAttributeParser( parserTexTransformRotate )
	CU_DeclareAttributeParser( parserTexTransformTranslate )
	CU_DeclareAttributeParser( parserTexTransformScale )
	CU_DeclareAttributeParser( parserTexTile )

	// Texture Animation Parsers
	CU_DeclareAttributeParser( parserTexAnimRotate )
	CU_DeclareAttributeParser( parserTexAnimTranslate )
	CU_DeclareAttributeParser( parserTexAnimScale )
	CU_DeclareAttributeParser( parserTexAnimTile )

	// Shader Parsers
	CU_DeclareAttributeParser( parserVertexShader )
	CU_DeclareAttributeParser( parserPixelShader )
	CU_DeclareAttributeParser( parserGeometryShader )
	CU_DeclareAttributeParser( parserHullShader )
	CU_DeclareAttributeParser( parserDomainShader )
	CU_DeclareAttributeParser( parserComputeShader )
	CU_DeclareAttributeParser( parserConstantsBuffer )
	CU_DeclareAttributeParser( parserShaderEnd )

	// Shader Program Parsers
	CU_DeclareAttributeParser( parserShaderProgramFile )
	CU_DeclareAttributeParser( parserShaderGroupSizes )

	// Shader Program UBO Parsers
	CU_DeclareAttributeParser( parserShaderUboShaders )
	CU_DeclareAttributeParser( parserShaderUboVariable )

	// Shader UBO Variables Parsers
	CU_DeclareAttributeParser( parserShaderVariableType )
	CU_DeclareAttributeParser( parserShaderVariableCount )
	CU_DeclareAttributeParser( parserShaderVariableValue )

	// Font Parsers
	CU_DeclareAttributeParser( parserFontFile )
	CU_DeclareAttributeParser( parserFontHeight )
	CU_DeclareAttributeParser( parserFontEnd )

	//Overlay Parsers
	CU_DeclareAttributeParser( parserOverlayPosition )
	CU_DeclareAttributeParser( parserOverlaySize )
	CU_DeclareAttributeParser( parserOverlayPixelSize )
	CU_DeclareAttributeParser( parserOverlayPixelPosition )
	CU_DeclareAttributeParser( parserOverlayMaterial )
	CU_DeclareAttributeParser( parserOverlayEnd )
	CU_DeclareAttributeParser( parserOverlayPanelOverlay )
	CU_DeclareAttributeParser( parserOverlayBorderPanelOverlay )
	CU_DeclareAttributeParser( parserOverlayTextOverlay )

	//PanelOverlay Parsers
	CU_DeclareAttributeParser( parserPanelOverlayUvs )

	//BorderPanelOverlay Parsers
	CU_DeclareAttributeParser( parserBorderPanelOverlaySizes )
	CU_DeclareAttributeParser( parserBorderPanelOverlayPixelSizes )
	CU_DeclareAttributeParser( parserBorderPanelOverlayMaterial )
	CU_DeclareAttributeParser( parserBorderPanelOverlayPosition )
	CU_DeclareAttributeParser( parserBorderPanelOverlayCenterUvs )
	CU_DeclareAttributeParser( parserBorderPanelOverlayOuterUvs )
	CU_DeclareAttributeParser( parserBorderPanelOverlayInnerUvs )

	// TextOverlay Parsers
	CU_DeclareAttributeParser( parserTextOverlayFont )
	CU_DeclareAttributeParser( parserTextOverlayText )
	CU_DeclareAttributeParser( parserTextOverlayTextWrapping )
	CU_DeclareAttributeParser( parserTextOverlayVerticalAlign )
	CU_DeclareAttributeParser( parserTextOverlayHorizontalAlign )
	CU_DeclareAttributeParser( parserTextOverlayTexturingMode )
	CU_DeclareAttributeParser( parserTextOverlayLineSpacingMode )

	// Camera parsers
	CU_DeclareAttributeParser( parserCameraParent )
	CU_DeclareAttributeParser( parserCameraViewport )
	CU_DeclareAttributeParser( parserCameraHdrConfig )
	CU_DeclareAttributeParser( parserCameraPrimitive )
	CU_DeclareAttributeParser( parserCameraEnd )

	// Viewport parsers
	CU_DeclareAttributeParser( parserViewportType )
	CU_DeclareAttributeParser( parserViewportLeft )
	CU_DeclareAttributeParser( parserViewportRight )
	CU_DeclareAttributeParser( parserViewportTop )
	CU_DeclareAttributeParser( parserViewportBottom )
	CU_DeclareAttributeParser( parserViewportNear )
	CU_DeclareAttributeParser( parserViewportFar )
	CU_DeclareAttributeParser( parserViewportSize )
	CU_DeclareAttributeParser( parserViewportFovY )
	CU_DeclareAttributeParser( parserViewportAspectRatio )

	// Billboard parsers
	CU_DeclareAttributeParser( parserBillboardParent )
	CU_DeclareAttributeParser( parserBillboardType )
	CU_DeclareAttributeParser( parserBillboardSize )
	CU_DeclareAttributeParser( parserBillboardPositions )
	CU_DeclareAttributeParser( parserBillboardMaterial )
	CU_DeclareAttributeParser( parserBillboardDimensions )
	CU_DeclareAttributeParser( parserBillboardEnd )
	CU_DeclareAttributeParser( parserBillboardPoint )

	// Animated object group parsers
	CU_DeclareAttributeParser( parserAnimatedObjectGroupAnimatedObject )
	CU_DeclareAttributeParser( parserAnimatedObjectGroupAnimatedTexture )
	CU_DeclareAttributeParser( parserAnimatedObjectGroupAnimation )
	CU_DeclareAttributeParser( parserAnimatedObjectGroupAnimationStart )
	CU_DeclareAttributeParser( parserAnimatedObjectGroupAnimationPause )
	CU_DeclareAttributeParser( parserAnimatedObjectGroupEnd )

	// Animated object group animation parsers
	CU_DeclareAttributeParser( parserAnimationLooped )
	CU_DeclareAttributeParser( parserAnimationScale )
	CU_DeclareAttributeParser( parserAnimationStartAt )
	CU_DeclareAttributeParser( parserAnimationStopAt )
	CU_DeclareAttributeParser( parserAnimationEnd )

	// Skybox parsers
	CU_DeclareAttributeParser( parserSkyboxEqui )
	CU_DeclareAttributeParser( parserSkyboxCross )
	CU_DeclareAttributeParser( parserSkyboxLeft )
	CU_DeclareAttributeParser( parserSkyboxRight )
	CU_DeclareAttributeParser( parserSkyboxTop )
	CU_DeclareAttributeParser( parserSkyboxBottom )
	CU_DeclareAttributeParser( parserSkyboxFront )
	CU_DeclareAttributeParser( parserSkyboxBack )
	CU_DeclareAttributeParser( parserSkyboxEnd )

	// SSAO parsers
	CU_DeclareAttributeParser( parserSsaoEnabled )
	CU_DeclareAttributeParser( parserSsaoHighQuality )
	CU_DeclareAttributeParser( parserSsaoUseNormalsBuffer )
	CU_DeclareAttributeParser( parserSsaoRadius )
	CU_DeclareAttributeParser( parserSsaoMinRadius )
	CU_DeclareAttributeParser( parserSsaoBias )
	CU_DeclareAttributeParser( parserSsaoIntensity )
	CU_DeclareAttributeParser( parserSsaoNumSamples )
	CU_DeclareAttributeParser( parserSsaoEdgeSharpness )
	CU_DeclareAttributeParser( parserSsaoBlurHighQuality )
	CU_DeclareAttributeParser( parserSsaoBlurStepSize )
	CU_DeclareAttributeParser( parserSsaoBlurRadius )
	CU_DeclareAttributeParser( parserSsaoBendStepCount )
	CU_DeclareAttributeParser( parserSsaoBendStepSize )
	CU_DeclareAttributeParser( parserSsaoEnd )

	// Subsurface Scattering parsers
	CU_DeclareAttributeParser( parserSubsurfaceScatteringStrength )
	CU_DeclareAttributeParser( parserSubsurfaceScatteringGaussianWidth )
	CU_DeclareAttributeParser( parserSubsurfaceScatteringTransmittanceProfile )
	CU_DeclareAttributeParser( parserSubsurfaceScatteringEnd )

	// Transmittance Profile parsers
	CU_DeclareAttributeParser( parserTransmittanceProfileFactor )

	// HDR config parsers
	CU_DeclareAttributeParser( parserHdrExponent )
	CU_DeclareAttributeParser( parserHdrGamma )

	// Voxel Cone Tracing config parsers
	CU_DeclareAttributeParser( parserVctEnabled )
	CU_DeclareAttributeParser( parserVctGridSize )
	CU_DeclareAttributeParser( parserVctNumCones )
	CU_DeclareAttributeParser( parserVctMaxDistance )
	CU_DeclareAttributeParser( parserVctRayStepSize )
	CU_DeclareAttributeParser( parserVctVoxelSize )
	CU_DeclareAttributeParser( parserVctConservativeRasterization )
	CU_DeclareAttributeParser( parserVctTemporalSmoothing )
	CU_DeclareAttributeParser( parserVctOcclusion )
	CU_DeclareAttributeParser( parserVctSecondaryBounce )
}

#endif
