﻿/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___C3D_SCENE_FILE_PARSER_PARSERS_H___
#define ___C3D_SCENE_FILE_PARSER_PARSERS_H___

#include "Castor3DPrerequisites.hpp"
#include "SceneFileParser.hpp"

namespace Castor3D
{
	// Root parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_RootMtlFile )
	DECLARE_ATTRIBUTE_PARSER( Parser_RootScene )
	DECLARE_ATTRIBUTE_PARSER( Parser_RootFont )
	DECLARE_ATTRIBUTE_PARSER( Parser_RootMaterial )
	DECLARE_ATTRIBUTE_PARSER( Parser_RootSamplerState )
	DECLARE_ATTRIBUTE_PARSER( Parser_RootPanelOverlay )
	DECLARE_ATTRIBUTE_PARSER( Parser_RootBorderPanelOverlay )
	DECLARE_ATTRIBUTE_PARSER( Parser_RootTextOverlay )
	DECLARE_ATTRIBUTE_PARSER( Parser_RootDebugOverlays )
	DECLARE_ATTRIBUTE_PARSER( Parser_RootWindow )

	//Window parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_WindowRenderTarget )
	DECLARE_ATTRIBUTE_PARSER( Parser_WindowVSync )
	DECLARE_ATTRIBUTE_PARSER( Parser_WindowFullscreen )

	//RenderTarget parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_RenderTargetScene )
	DECLARE_ATTRIBUTE_PARSER( Parser_RenderTargetCamera )
	DECLARE_ATTRIBUTE_PARSER( Parser_RenderTargetSize )
	DECLARE_ATTRIBUTE_PARSER( Parser_RenderTargetFormat )
	DECLARE_ATTRIBUTE_PARSER( Parser_RenderTargetStereo )
	DECLARE_ATTRIBUTE_PARSER( Parser_RenderTargetPostEffect )
	DECLARE_ATTRIBUTE_PARSER( Parser_RenderTargetToneMapping )
	DECLARE_ATTRIBUTE_PARSER( Parser_RenderTargetSsao )
	DECLARE_ATTRIBUTE_PARSER( Parser_RenderTargetEnd )

	// Sampler parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_SamplerMinFilter )
	DECLARE_ATTRIBUTE_PARSER( Parser_SamplerMagFilter )
	DECLARE_ATTRIBUTE_PARSER( Parser_SamplerMipFilter )
	DECLARE_ATTRIBUTE_PARSER( Parser_SamplerMinLod )
	DECLARE_ATTRIBUTE_PARSER( Parser_SamplerMaxLod )
	DECLARE_ATTRIBUTE_PARSER( Parser_SamplerLodBias )
	DECLARE_ATTRIBUTE_PARSER( Parser_SamplerUWrapMode )
	DECLARE_ATTRIBUTE_PARSER( Parser_SamplerVWrapMode )
	DECLARE_ATTRIBUTE_PARSER( Parser_SamplerWWrapMode )
	DECLARE_ATTRIBUTE_PARSER( Parser_SamplerBorderColour )
	DECLARE_ATTRIBUTE_PARSER( Parser_SamplerMaxAnisotropy )
	DECLARE_ATTRIBUTE_PARSER( Parser_SamplerComparisonMode )
	DECLARE_ATTRIBUTE_PARSER( Parser_SamplerComparisonFunc )

	// Scene parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_SceneInclude )
	DECLARE_ATTRIBUTE_PARSER( Parser_SceneMaterials )
	DECLARE_ATTRIBUTE_PARSER( Parser_SceneBkColour )
	DECLARE_ATTRIBUTE_PARSER( Parser_SceneBkImage )
	DECLARE_ATTRIBUTE_PARSER( Parser_SceneFont )
	DECLARE_ATTRIBUTE_PARSER( Parser_SceneMaterial )
	DECLARE_ATTRIBUTE_PARSER( Parser_SceneSamplerState )
	DECLARE_ATTRIBUTE_PARSER( Parser_SceneCamera )
	DECLARE_ATTRIBUTE_PARSER( Parser_SceneLight )
	DECLARE_ATTRIBUTE_PARSER( Parser_SceneCameraNode )
	DECLARE_ATTRIBUTE_PARSER( Parser_SceneSceneNode )
	DECLARE_ATTRIBUTE_PARSER( Parser_SceneObject )
	DECLARE_ATTRIBUTE_PARSER( Parser_SceneAmbientLight )
	DECLARE_ATTRIBUTE_PARSER( Parser_SceneImport )
	DECLARE_ATTRIBUTE_PARSER( Parser_SceneBillboard )
	DECLARE_ATTRIBUTE_PARSER( Parser_SceneAnimatedObjectGroup )
	DECLARE_ATTRIBUTE_PARSER( Parser_ScenePanelOverlay )
	DECLARE_ATTRIBUTE_PARSER( Parser_SceneBorderPanelOverlay )
	DECLARE_ATTRIBUTE_PARSER( Parser_SceneTextOverlay )
	DECLARE_ATTRIBUTE_PARSER( Parser_SceneSkybox )
	DECLARE_ATTRIBUTE_PARSER( Parser_SceneFogType )
	DECLARE_ATTRIBUTE_PARSER( Parser_SceneFogDensity )
	DECLARE_ATTRIBUTE_PARSER( Parser_SceneParticleSystem )

	// ParticleSystem parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_ParticleSystemParent )
	DECLARE_ATTRIBUTE_PARSER( Parser_ParticleSystemCount )
	DECLARE_ATTRIBUTE_PARSER( Parser_ParticleSystemMaterial )
	DECLARE_ATTRIBUTE_PARSER( Parser_ParticleSystemDimensions )
	DECLARE_ATTRIBUTE_PARSER( Parser_ParticleSystemParticle )
	DECLARE_ATTRIBUTE_PARSER( Parser_ParticleSystemTFShader )
	DECLARE_ATTRIBUTE_PARSER( Parser_ParticleSystemCSShader )

	// Particle parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_ParticleType )
	DECLARE_ATTRIBUTE_PARSER( Parser_ParticleVariable )

	// Light parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_LightParent )
	DECLARE_ATTRIBUTE_PARSER( Parser_LightType )
	DECLARE_ATTRIBUTE_PARSER( Parser_LightColour )
	DECLARE_ATTRIBUTE_PARSER( Parser_LightIntensity )
	DECLARE_ATTRIBUTE_PARSER( Parser_LightAttenuation )
	DECLARE_ATTRIBUTE_PARSER( Parser_LightCutOff )
	DECLARE_ATTRIBUTE_PARSER( Parser_LightExponent )
	DECLARE_ATTRIBUTE_PARSER( Parser_LightShadowProducer )

	// Scene node parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_NodeParent )
	DECLARE_ATTRIBUTE_PARSER( Parser_NodePosition )
	DECLARE_ATTRIBUTE_PARSER( Parser_NodeOrientation )
	DECLARE_ATTRIBUTE_PARSER( Parser_NodeScale )

	// Object Parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_ObjectParent )
	DECLARE_ATTRIBUTE_PARSER( Parser_ObjectMesh )
	DECLARE_ATTRIBUTE_PARSER( Parser_ObjectMaterial )
	DECLARE_ATTRIBUTE_PARSER( Parser_ObjectMaterials )
	DECLARE_ATTRIBUTE_PARSER( Parser_ObjectCastShadows )
	DECLARE_ATTRIBUTE_PARSER( Parser_ObjectReceivesShadows )
	DECLARE_ATTRIBUTE_PARSER( Parser_ObjectEnd )

	// Object Materials Parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_ObjectMaterialsMaterial )
	DECLARE_ATTRIBUTE_PARSER( Parser_ObjectMaterialsEnd )

	// Mesh Parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_MeshType )
	DECLARE_ATTRIBUTE_PARSER( Parser_MeshSubmesh )
	DECLARE_ATTRIBUTE_PARSER( Parser_MeshImport )
	DECLARE_ATTRIBUTE_PARSER( Parser_MeshMorphImport )
	DECLARE_ATTRIBUTE_PARSER( Parser_MeshDivide )
	DECLARE_ATTRIBUTE_PARSER( Parser_MeshEnd )

	// Submesh Parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_SubmeshVertex )
	DECLARE_ATTRIBUTE_PARSER( Parser_SubmeshUV )
	DECLARE_ATTRIBUTE_PARSER( Parser_SubmeshUVW )
	DECLARE_ATTRIBUTE_PARSER( Parser_SubmeshNormal )
	DECLARE_ATTRIBUTE_PARSER( Parser_SubmeshTangent )
	DECLARE_ATTRIBUTE_PARSER( Parser_SubmeshFace )
	DECLARE_ATTRIBUTE_PARSER( Parser_SubmeshFaceUV )
	DECLARE_ATTRIBUTE_PARSER( Parser_SubmeshFaceUVW )
	DECLARE_ATTRIBUTE_PARSER( Parser_SubmeshFaceNormals )
	DECLARE_ATTRIBUTE_PARSER( Parser_SubmeshFaceTangents )
	DECLARE_ATTRIBUTE_PARSER( Parser_SubmeshEnd )

	// Material Parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_MaterialType )
	DECLARE_ATTRIBUTE_PARSER( Parser_MaterialPass )
	DECLARE_ATTRIBUTE_PARSER( Parser_MaterialEnd )

	// Pass Parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_PassDiffuse )
	DECLARE_ATTRIBUTE_PARSER( Parser_PassSpecular )
	DECLARE_ATTRIBUTE_PARSER( Parser_PassAmbient )
	DECLARE_ATTRIBUTE_PARSER( Parser_PassEmissive )
	DECLARE_ATTRIBUTE_PARSER( Parser_PassShininess )
	DECLARE_ATTRIBUTE_PARSER( Parser_PassAlbedo )
	DECLARE_ATTRIBUTE_PARSER( Parser_PassRoughness )
	DECLARE_ATTRIBUTE_PARSER( Parser_PassMetallic )
	DECLARE_ATTRIBUTE_PARSER( Parser_PassGlossiness )
	DECLARE_ATTRIBUTE_PARSER( Parser_PassAlpha )
	DECLARE_ATTRIBUTE_PARSER( Parser_PassDoubleFace )
	DECLARE_ATTRIBUTE_PARSER( Parser_PassTextureUnit )
	DECLARE_ATTRIBUTE_PARSER( Parser_PassShader )
	DECLARE_ATTRIBUTE_PARSER( Parser_PassAlphaBlendMode )
	DECLARE_ATTRIBUTE_PARSER( Parser_PassColourBlendMode )
	DECLARE_ATTRIBUTE_PARSER( Parser_PassAlphaFunc )
	DECLARE_ATTRIBUTE_PARSER( Parser_PassRefractionRatio )
	DECLARE_ATTRIBUTE_PARSER( Parser_PassEnd )

	// Texture Unit Parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_UnitImage )
	DECLARE_ATTRIBUTE_PARSER( Parser_UnitRenderTarget )
	DECLARE_ATTRIBUTE_PARSER( Parser_UnitChannel )
	DECLARE_ATTRIBUTE_PARSER( Parser_UnitSampler )
	DECLARE_ATTRIBUTE_PARSER( Parser_UnitEnd )

	// Shader Parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_VertexShader )
	DECLARE_ATTRIBUTE_PARSER( Parser_PixelShader )
	DECLARE_ATTRIBUTE_PARSER( Parser_GeometryShader )
	DECLARE_ATTRIBUTE_PARSER( Parser_HullShader )
	DECLARE_ATTRIBUTE_PARSER( Parser_DomainShader )
	DECLARE_ATTRIBUTE_PARSER( Parser_ComputeShader )
	DECLARE_ATTRIBUTE_PARSER( Parser_ConstantsBuffer )
	DECLARE_ATTRIBUTE_PARSER( Parser_ShaderEnd )

	// Shader Program Parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_ShaderProgramFile )
	DECLARE_ATTRIBUTE_PARSER( Parser_ShaderProgramSampler )
	DECLARE_ATTRIBUTE_PARSER( Parser_GeometryInputType )
	DECLARE_ATTRIBUTE_PARSER( Parser_GeometryOutputType )
	DECLARE_ATTRIBUTE_PARSER( Parser_GeometryOutputVtxCount )

	// Shader Program UBO Parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_ShaderUboShaders )
	DECLARE_ATTRIBUTE_PARSER( Parser_ShaderUboVariable )

	// Shader UBO Variables Parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_ShaderVariableType )
	DECLARE_ATTRIBUTE_PARSER( Parser_ShaderVariableCount )
	DECLARE_ATTRIBUTE_PARSER( Parser_ShaderVariableValue )

	// Font Parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_FontFile )
	DECLARE_ATTRIBUTE_PARSER( Parser_FontHeight )
	DECLARE_ATTRIBUTE_PARSER( Parser_FontEnd )

	//Overlay Parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_OverlayPosition )
	DECLARE_ATTRIBUTE_PARSER( Parser_OverlaySize )
	DECLARE_ATTRIBUTE_PARSER( Parser_OverlayPixelSize )
	DECLARE_ATTRIBUTE_PARSER( Parser_OverlayPixelPosition )
	DECLARE_ATTRIBUTE_PARSER( Parser_OverlayMaterial )
	DECLARE_ATTRIBUTE_PARSER( Parser_OverlayEnd )
	DECLARE_ATTRIBUTE_PARSER( Parser_OverlayPanelOverlay )
	DECLARE_ATTRIBUTE_PARSER( Parser_OverlayBorderPanelOverlay )
	DECLARE_ATTRIBUTE_PARSER( Parser_OverlayTextOverlay )

	//PanelOverlay Parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_PanelOverlayUvs )

	//BorderPanelOverlay Parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_BorderPanelOverlaySizes )
	DECLARE_ATTRIBUTE_PARSER( Parser_BorderPanelOverlayPixelSizes )
	DECLARE_ATTRIBUTE_PARSER( Parser_BorderPanelOverlayMaterial )
	DECLARE_ATTRIBUTE_PARSER( Parser_BorderPanelOverlayPosition )
	DECLARE_ATTRIBUTE_PARSER( Parser_BorderPanelOverlayCenterUvs )
	DECLARE_ATTRIBUTE_PARSER( Parser_BorderPanelOverlayOuterUvs )
	DECLARE_ATTRIBUTE_PARSER( Parser_BorderPanelOverlayInnerUvs )

	// TextOverlay Parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_TextOverlayFont )
	DECLARE_ATTRIBUTE_PARSER( Parser_TextOverlayText )
	DECLARE_ATTRIBUTE_PARSER( Parser_TextOverlayTextWrapping )
	DECLARE_ATTRIBUTE_PARSER( Parser_TextOverlayVerticalAlign )
	DECLARE_ATTRIBUTE_PARSER( Parser_TextOverlayHorizontalAlign )
	DECLARE_ATTRIBUTE_PARSER( Parser_TextOverlayTexturingMode )
	DECLARE_ATTRIBUTE_PARSER( Parser_TextOverlayLineSpacingMode )

	// Camera parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_CameraParent )
	DECLARE_ATTRIBUTE_PARSER( Parser_CameraViewport )
	DECLARE_ATTRIBUTE_PARSER( Parser_CameraPrimitive )
	DECLARE_ATTRIBUTE_PARSER( Parser_CameraEnd )

	// Viewport parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_ViewportType )
	DECLARE_ATTRIBUTE_PARSER( Parser_ViewportLeft )
	DECLARE_ATTRIBUTE_PARSER( Parser_ViewportRight )
	DECLARE_ATTRIBUTE_PARSER( Parser_ViewportTop )
	DECLARE_ATTRIBUTE_PARSER( Parser_ViewportBottom )
	DECLARE_ATTRIBUTE_PARSER( Parser_ViewportNear )
	DECLARE_ATTRIBUTE_PARSER( Parser_ViewportFar )
	DECLARE_ATTRIBUTE_PARSER( Parser_ViewportSize )
	DECLARE_ATTRIBUTE_PARSER( Parser_ViewportFovY )
	DECLARE_ATTRIBUTE_PARSER( Parser_ViewportAspectRatio )

	// Billboard parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_BillboardParent )
	DECLARE_ATTRIBUTE_PARSER( Parser_BillboardType )
	DECLARE_ATTRIBUTE_PARSER( Parser_BillboardSize )
	DECLARE_ATTRIBUTE_PARSER( Parser_BillboardPositions )
	DECLARE_ATTRIBUTE_PARSER( Parser_BillboardMaterial )
	DECLARE_ATTRIBUTE_PARSER( Parser_BillboardDimensions )
	DECLARE_ATTRIBUTE_PARSER( Parser_BillboardEnd )
	DECLARE_ATTRIBUTE_PARSER( Parser_BillboardPoint )

	// Animated object group parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_AnimatedObjectGroupAnimatedObject )
	DECLARE_ATTRIBUTE_PARSER( Parser_AnimatedObjectGroupAnimation )
	DECLARE_ATTRIBUTE_PARSER( Parser_AnimatedObjectGroupAnimationStart )
	DECLARE_ATTRIBUTE_PARSER( Parser_AnimatedObjectGroupEnd )

	// Animated object group animation parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_AnimationLooped )
	DECLARE_ATTRIBUTE_PARSER( Parser_AnimationScale )
	DECLARE_ATTRIBUTE_PARSER( Parser_AnimationEnd )

	// Skybox parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_SkyboxEqui )
	DECLARE_ATTRIBUTE_PARSER( Parser_SkyboxLeft )
	DECLARE_ATTRIBUTE_PARSER( Parser_SkyboxRight )
	DECLARE_ATTRIBUTE_PARSER( Parser_SkyboxTop )
	DECLARE_ATTRIBUTE_PARSER( Parser_SkyboxBottom )
	DECLARE_ATTRIBUTE_PARSER( Parser_SkyboxFront )
	DECLARE_ATTRIBUTE_PARSER( Parser_SkyboxBack )
	DECLARE_ATTRIBUTE_PARSER( Parser_SkyboxEnd )

	//SSAO parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_SsaoEnabled )
	DECLARE_ATTRIBUTE_PARSER( Parser_SsaoRadius )
	DECLARE_ATTRIBUTE_PARSER( Parser_SsaoBias )
	DECLARE_ATTRIBUTE_PARSER( Parser_SsaoEnd )
}

#endif
