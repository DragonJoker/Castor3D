/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
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
	DECLARE_ATTRIBUTE_PARSER( Parser_RootWindow )
	DECLARE_ATTRIBUTE_PARSER( Parser_RootPanelOverlay )
	DECLARE_ATTRIBUTE_PARSER( Parser_RootBorderPanelOverlay )
	DECLARE_ATTRIBUTE_PARSER( Parser_RootTextOverlay )
	DECLARE_ATTRIBUTE_PARSER( Parser_RootDebugOverlays )

	//Window parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_WindowRenderTarget )
	DECLARE_ATTRIBUTE_PARSER( Parser_WindowVSync )
	DECLARE_ATTRIBUTE_PARSER( Parser_WindowFullscreen )

	//RenderTarget parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_RenderTargetScene )
	DECLARE_ATTRIBUTE_PARSER( Parser_RenderTargetCamera )
	DECLARE_ATTRIBUTE_PARSER( Parser_RenderTargetSize )
	DECLARE_ATTRIBUTE_PARSER( Parser_RenderTargetFormat )
	DECLARE_ATTRIBUTE_PARSER( Parser_RenderTargetDepth )
	DECLARE_ATTRIBUTE_PARSER( Parser_RenderTargetMsaa )
	DECLARE_ATTRIBUTE_PARSER( Parser_RenderTargetSsaa )
	DECLARE_ATTRIBUTE_PARSER( Parser_RenderTargetStereo )
	DECLARE_ATTRIBUTE_PARSER( Parser_RenderTargetDeferred )
	DECLARE_ATTRIBUTE_PARSER( Parser_RenderTargetPostEffect )
	DECLARE_ATTRIBUTE_PARSER( Parser_RenderTargetEnd )

	// Sampler parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_SamplerMinFilter )
	DECLARE_ATTRIBUTE_PARSER( Parser_SamplerMagFilter )
	DECLARE_ATTRIBUTE_PARSER( Parser_SamplerMinLod )
	DECLARE_ATTRIBUTE_PARSER( Parser_SamplerMaxLod )
	DECLARE_ATTRIBUTE_PARSER( Parser_SamplerLodBias )
	DECLARE_ATTRIBUTE_PARSER( Parser_SamplerUWrapMode )
	DECLARE_ATTRIBUTE_PARSER( Parser_SamplerVWrapMode )
	DECLARE_ATTRIBUTE_PARSER( Parser_SamplerWWrapMode )
	DECLARE_ATTRIBUTE_PARSER( Parser_SamplerBorderColour )
	DECLARE_ATTRIBUTE_PARSER( Parser_SamplerMaxAnisotropy )

	// Scene parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_SceneBkColour )
	DECLARE_ATTRIBUTE_PARSER( Parser_SceneBkImage )
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

	// Light parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_LightParent )
	DECLARE_ATTRIBUTE_PARSER( Parser_LightType )
	DECLARE_ATTRIBUTE_PARSER( Parser_LightAmbient )
	DECLARE_ATTRIBUTE_PARSER( Parser_LightDiffuse )
	DECLARE_ATTRIBUTE_PARSER( Parser_LightSpecular )
	DECLARE_ATTRIBUTE_PARSER( Parser_LightAttenuation )
	DECLARE_ATTRIBUTE_PARSER( Parser_LightCutOff )
	DECLARE_ATTRIBUTE_PARSER( Parser_LightExponent )

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
	DECLARE_ATTRIBUTE_PARSER( Parser_ObjectEnd )

	// Object Materials Parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_ObjectMaterialsMaterial )
	DECLARE_ATTRIBUTE_PARSER( Parser_ObjectMaterialsEnd )

	// Mesh Parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_MeshType )
	DECLARE_ATTRIBUTE_PARSER( Parser_MeshNormals )
	DECLARE_ATTRIBUTE_PARSER( Parser_MeshFile )
	DECLARE_ATTRIBUTE_PARSER( Parser_MeshSubmesh )
	DECLARE_ATTRIBUTE_PARSER( Parser_MeshImport )
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
	DECLARE_ATTRIBUTE_PARSER( Parser_MaterialPass )
	DECLARE_ATTRIBUTE_PARSER( Parser_MaterialEnd )

	// Pass Parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_PassAmbient )
	DECLARE_ATTRIBUTE_PARSER( Parser_PassDiffuse )
	DECLARE_ATTRIBUTE_PARSER( Parser_PassSpecular )
	DECLARE_ATTRIBUTE_PARSER( Parser_PassEmissive )
	DECLARE_ATTRIBUTE_PARSER( Parser_PassShininess )
	DECLARE_ATTRIBUTE_PARSER( Parser_PassAlpha )
	DECLARE_ATTRIBUTE_PARSER( Parser_PassDoubleFace )
	DECLARE_ATTRIBUTE_PARSER( Parser_PassBlendFunc )
	DECLARE_ATTRIBUTE_PARSER( Parser_PassTextureUnit )
	DECLARE_ATTRIBUTE_PARSER( Parser_PassGlShader )
	DECLARE_ATTRIBUTE_PARSER( Parser_PassHlShader )
	DECLARE_ATTRIBUTE_PARSER( Parser_PassAlphaBlendMode )
	DECLARE_ATTRIBUTE_PARSER( Parser_PassColourBlendMode )

	// Texture Unit Parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_UnitImage )
	DECLARE_ATTRIBUTE_PARSER( Parser_UnitRenderTarget )
	DECLARE_ATTRIBUTE_PARSER( Parser_UnitMapType )
	DECLARE_ATTRIBUTE_PARSER( Parser_UnitAlphaFunc )
	DECLARE_ATTRIBUTE_PARSER( Parser_UnitRgbBlend )
	DECLARE_ATTRIBUTE_PARSER( Parser_UnitAlphaBlend )
	DECLARE_ATTRIBUTE_PARSER( Parser_UnitChannel )
	DECLARE_ATTRIBUTE_PARSER( Parser_UnitSampler )
	DECLARE_ATTRIBUTE_PARSER( Parser_UnitBlendColour )

	// Shader Parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_VertexShader )
	DECLARE_ATTRIBUTE_PARSER( Parser_PixelShader )
	DECLARE_ATTRIBUTE_PARSER( Parser_GeometryShader )
	DECLARE_ATTRIBUTE_PARSER( Parser_HullShader )
	DECLARE_ATTRIBUTE_PARSER( Parser_DomainShader )
	DECLARE_ATTRIBUTE_PARSER( Parser_ConstantsBuffer )
	DECLARE_ATTRIBUTE_PARSER( Parser_ShaderEnd )

	// Shader Program Parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_ShaderProgramFile )
	DECLARE_ATTRIBUTE_PARSER( Parser_ShaderProgramEntry )
	DECLARE_ATTRIBUTE_PARSER( Parser_ShaderProgramSampler )
	DECLARE_ATTRIBUTE_PARSER( Parser_GeometryInputType )
	DECLARE_ATTRIBUTE_PARSER( Parser_GeometryOutputType )
	DECLARE_ATTRIBUTE_PARSER( Parser_GeometryOutputVtxCount )

	// Shader Program Sampler Parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_ShaderSamplerValue )

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
	DECLARE_ATTRIBUTE_PARSER( Parser_BillboardPositions )
	DECLARE_ATTRIBUTE_PARSER( Parser_BillboardMaterial )
	DECLARE_ATTRIBUTE_PARSER( Parser_BillboardDimensions )
	DECLARE_ATTRIBUTE_PARSER( Parser_BillboardEnd )
	DECLARE_ATTRIBUTE_PARSER( Parser_BillboardPoint )

	// Animated object group parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_GroupAnimatedObject )
	DECLARE_ATTRIBUTE_PARSER( Parser_GroupAnimation )
}

#endif
