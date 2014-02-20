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
#ifndef ___C3D_SceneFileParser_Parsers___
#define ___C3D_SceneFileParser_Parsers___

#include "Prerequisites.hpp"
#include "SceneFileParser.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

namespace Castor3D
{
	// Root parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_RootMtlFile				)
	DECLARE_ATTRIBUTE_PARSER( Parser_RootScene					)
	DECLARE_ATTRIBUTE_PARSER( Parser_RootFont					)
	DECLARE_ATTRIBUTE_PARSER( Parser_RootMaterial				)
	DECLARE_ATTRIBUTE_PARSER( Parser_RootSamplerState			)
	DECLARE_ATTRIBUTE_PARSER( Parser_RootWindow					)
	DECLARE_ATTRIBUTE_PARSER( Parser_RootPanelOverlay			)
	DECLARE_ATTRIBUTE_PARSER( Parser_RootBorderPanelOverlay		)
	DECLARE_ATTRIBUTE_PARSER( Parser_RootTextOverlay			)

	//Window parsers											)
	DECLARE_ATTRIBUTE_PARSER( Parser_WindowRenderTarget			)
	DECLARE_ATTRIBUTE_PARSER( Parser_WindowVSync				)
	DECLARE_ATTRIBUTE_PARSER( Parser_WindowFullscreen			)

	//RenderTarget parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_RenderTargetScene			)
	DECLARE_ATTRIBUTE_PARSER( Parser_RenderTargetCamera			)
	DECLARE_ATTRIBUTE_PARSER( Parser_RenderTargetSize			)
	DECLARE_ATTRIBUTE_PARSER( Parser_RenderTargetFormat			)
	DECLARE_ATTRIBUTE_PARSER( Parser_RenderTargetDepth			)
	DECLARE_ATTRIBUTE_PARSER( Parser_RenderTargetMsaa			)
	DECLARE_ATTRIBUTE_PARSER( Parser_RenderTargetSsaa			)
	DECLARE_ATTRIBUTE_PARSER( Parser_RenderTargetStereo			)
	DECLARE_ATTRIBUTE_PARSER( Parser_RenderTargetDeferred		)
	DECLARE_ATTRIBUTE_PARSER( Parser_RenderTargetEnd			)

	// Sampler parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_SamplerMinFilter			)
	DECLARE_ATTRIBUTE_PARSER( Parser_SamplerMagFilter			)
	DECLARE_ATTRIBUTE_PARSER( Parser_SamplerMinLod				)
	DECLARE_ATTRIBUTE_PARSER( Parser_SamplerMaxLod				)
	DECLARE_ATTRIBUTE_PARSER( Parser_SamplerLodBias				)
	DECLARE_ATTRIBUTE_PARSER( Parser_SamplerUWrapMode			)
	DECLARE_ATTRIBUTE_PARSER( Parser_SamplerVWrapMode			)
	DECLARE_ATTRIBUTE_PARSER( Parser_SamplerWWrapMode			)
	DECLARE_ATTRIBUTE_PARSER( Parser_SamplerBorderColour		)
	DECLARE_ATTRIBUTE_PARSER( Parser_SamplerMaxAnisotropy		)

	// Scene parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_SceneBkColour				)
	DECLARE_ATTRIBUTE_PARSER( Parser_SceneBkImage				)
	DECLARE_ATTRIBUTE_PARSER( Parser_SceneCamera				)
	DECLARE_ATTRIBUTE_PARSER( Parser_SceneLight					)
	DECLARE_ATTRIBUTE_PARSER( Parser_SceneCameraNode			)
	DECLARE_ATTRIBUTE_PARSER( Parser_SceneSceneNode				)
	DECLARE_ATTRIBUTE_PARSER( Parser_SceneObject				)
	DECLARE_ATTRIBUTE_PARSER( Parser_SceneAmbientLight			)
	DECLARE_ATTRIBUTE_PARSER( Parser_SceneImport				)
	DECLARE_ATTRIBUTE_PARSER( Parser_SceneBillboard				)
	DECLARE_ATTRIBUTE_PARSER( Parser_SceneAnimatedObjectGroup	)
	DECLARE_ATTRIBUTE_PARSER( Parser_ScenePanelOverlay			)
	DECLARE_ATTRIBUTE_PARSER( Parser_SceneBorderPanelOverlay	)
	DECLARE_ATTRIBUTE_PARSER( Parser_SceneTextOverlay			)

	// Light parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_LightParent				)
	DECLARE_ATTRIBUTE_PARSER( Parser_LightType					)
	DECLARE_ATTRIBUTE_PARSER( Parser_LightAmbient				)
	DECLARE_ATTRIBUTE_PARSER( Parser_LightDiffuse				)
	DECLARE_ATTRIBUTE_PARSER( Parser_LightSpecular				)
	DECLARE_ATTRIBUTE_PARSER( Parser_LightAttenuation			)
	DECLARE_ATTRIBUTE_PARSER( Parser_LightCutOff				)
	DECLARE_ATTRIBUTE_PARSER( Parser_LightExponent				)

	// Scene node parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_NodeParent					)
	DECLARE_ATTRIBUTE_PARSER( Parser_NodePosition				)
	DECLARE_ATTRIBUTE_PARSER( Parser_NodeOrientation			)
	DECLARE_ATTRIBUTE_PARSER( Parser_NodeScale					)

	// Object Parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_ObjectParent				)
	DECLARE_ATTRIBUTE_PARSER( Parser_ObjectMesh					)
	DECLARE_ATTRIBUTE_PARSER( Parser_ObjectEnd					)

	// Mesh Parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_MeshType					)
	DECLARE_ATTRIBUTE_PARSER( Parser_MeshNormals				)
	DECLARE_ATTRIBUTE_PARSER( Parser_MeshFile					)
	DECLARE_ATTRIBUTE_PARSER( Parser_MeshMaterial				)
	DECLARE_ATTRIBUTE_PARSER( Parser_MeshSubmesh				)
	DECLARE_ATTRIBUTE_PARSER( Parser_MeshImport					)
	DECLARE_ATTRIBUTE_PARSER( Parser_MeshDivide					)
	DECLARE_ATTRIBUTE_PARSER( Parser_MeshEnd					)

	// Submesh Parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_SubmeshMaterial			)
	DECLARE_ATTRIBUTE_PARSER( Parser_SubmeshVertex				)
	DECLARE_ATTRIBUTE_PARSER( Parser_SubmeshUV					)
	DECLARE_ATTRIBUTE_PARSER( Parser_SubmeshUVW					)
	DECLARE_ATTRIBUTE_PARSER( Parser_SubmeshNormal				)
	DECLARE_ATTRIBUTE_PARSER( Parser_SubmeshTangent				)
	DECLARE_ATTRIBUTE_PARSER( Parser_SubmeshFace				)
	DECLARE_ATTRIBUTE_PARSER( Parser_SubmeshFaceUV				)
	DECLARE_ATTRIBUTE_PARSER( Parser_SubmeshFaceUVW				)
	DECLARE_ATTRIBUTE_PARSER( Parser_SubmeshFaceNormals			)
	DECLARE_ATTRIBUTE_PARSER( Parser_SubmeshFaceTangents		)
	DECLARE_ATTRIBUTE_PARSER( Parser_SubmeshEnd					)

	// Material Parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_MaterialPass				)
	DECLARE_ATTRIBUTE_PARSER( Parser_MaterialEnd				)

	// Pass Parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_PassAmbient				)
	DECLARE_ATTRIBUTE_PARSER( Parser_PassDiffuse				)
	DECLARE_ATTRIBUTE_PARSER( Parser_PassSpecular				)
	DECLARE_ATTRIBUTE_PARSER( Parser_PassEmissive				)
	DECLARE_ATTRIBUTE_PARSER( Parser_PassShininess				)
	DECLARE_ATTRIBUTE_PARSER( Parser_PassAlpha					)
	DECLARE_ATTRIBUTE_PARSER( Parser_PassDoubleFace				)
	DECLARE_ATTRIBUTE_PARSER( Parser_PassBlendFunc				)
	DECLARE_ATTRIBUTE_PARSER( Parser_PassTextureUnit			)
	DECLARE_ATTRIBUTE_PARSER( Parser_PassGlShader				)
	DECLARE_ATTRIBUTE_PARSER( Parser_PassCgShader				)
	DECLARE_ATTRIBUTE_PARSER( Parser_PassHlShader				)

	// Texture Unit Parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_UnitImage					)
	DECLARE_ATTRIBUTE_PARSER( Parser_UnitRenderTarget			)
	DECLARE_ATTRIBUTE_PARSER( Parser_UnitMapType				)
	DECLARE_ATTRIBUTE_PARSER( Parser_UnitAlphaFunc				)
	DECLARE_ATTRIBUTE_PARSER( Parser_UnitRgbBlend				)
	DECLARE_ATTRIBUTE_PARSER( Parser_UnitAlphaBlend				)
	DECLARE_ATTRIBUTE_PARSER( Parser_UnitChannel				)
	DECLARE_ATTRIBUTE_PARSER( Parser_UnitSampler				)
	DECLARE_ATTRIBUTE_PARSER( Parser_UnitBlendColour			)

	// GLSL Shader Parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_GlVertexShader				)
	DECLARE_ATTRIBUTE_PARSER( Parser_GlPixelShader				)
	DECLARE_ATTRIBUTE_PARSER( Parser_GlGeometryShader			)
	DECLARE_ATTRIBUTE_PARSER( Parser_GlGeometryInputType		)
	DECLARE_ATTRIBUTE_PARSER( Parser_GlGeometryOutputType		)
	DECLARE_ATTRIBUTE_PARSER( Parser_GlGeometryOutputVtxCount	)
	DECLARE_ATTRIBUTE_PARSER( Parser_GlHullShader				)
	DECLARE_ATTRIBUTE_PARSER( Parser_GlDomainShader				)
	DECLARE_ATTRIBUTE_PARSER( Parser_GlShaderEnd				)

	// GLSL Shader Program Parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_GlShaderProgramFile		)
	DECLARE_ATTRIBUTE_PARSER( Parser_GlShaderProgramVariable	)

	// GLSL Shader Variables Parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_GlShaderVariableType		)
	DECLARE_ATTRIBUTE_PARSER( Parser_GlShaderVariableValue		)

	// HLSL Shader Parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_HlVertexShader				)
	DECLARE_ATTRIBUTE_PARSER( Parser_HlPixelShader				)
	DECLARE_ATTRIBUTE_PARSER( Parser_HlGeometryShader			)
	DECLARE_ATTRIBUTE_PARSER( Parser_HlHullShader				)
	DECLARE_ATTRIBUTE_PARSER( Parser_HlDomainShader				)
	DECLARE_ATTRIBUTE_PARSER( Parser_HlFile						)
	DECLARE_ATTRIBUTE_PARSER( Parser_HlShaderEnd				)

	// HLSL Shader Program Parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_HlShaderProgramFile		)
	DECLARE_ATTRIBUTE_PARSER( Parser_HlShaderProgramVariable	)
	DECLARE_ATTRIBUTE_PARSER( Parser_HlShaderProgramEntry		)

	// HLSL Shader Variables Parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_HlShaderVariableType		)
	DECLARE_ATTRIBUTE_PARSER( Parser_HlShaderVariableValue		)

	// Cg Shader Parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_CgVertexShader				)
	DECLARE_ATTRIBUTE_PARSER( Parser_CgPixelShader				)
	DECLARE_ATTRIBUTE_PARSER( Parser_CgGeometryShader			)
	DECLARE_ATTRIBUTE_PARSER( Parser_CgHullShader				)
	DECLARE_ATTRIBUTE_PARSER( Parser_CgDomainShader				)
	DECLARE_ATTRIBUTE_PARSER( Parser_CgFile						)
	DECLARE_ATTRIBUTE_PARSER( Parser_CgShaderEnd				)

	//Cg Shader Program Parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_CgShaderProgramFile		)
	DECLARE_ATTRIBUTE_PARSER( Parser_CgShaderProgramVariable	)
	DECLARE_ATTRIBUTE_PARSER( Parser_CgShaderProgramEntry		)

	// Cg Shader Variables Parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_CgShaderVariableType		)
	DECLARE_ATTRIBUTE_PARSER( Parser_CgShaderVariableValue		)

	// Font Parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_FontFile					)
	DECLARE_ATTRIBUTE_PARSER( Parser_FontHeight					)
	DECLARE_ATTRIBUTE_PARSER( Parser_FontEnd					)

	//Overlay Parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_OverlayPosition			)
	DECLARE_ATTRIBUTE_PARSER( Parser_OverlaySize				)
	DECLARE_ATTRIBUTE_PARSER( Parser_OverlayMaterial			)
	DECLARE_ATTRIBUTE_PARSER( Parser_OverlayEnd					)
	DECLARE_ATTRIBUTE_PARSER( Parser_OverlayPanelOverlay		)
	DECLARE_ATTRIBUTE_PARSER( Parser_OverlayBorderPanelOverlay	)
	DECLARE_ATTRIBUTE_PARSER( Parser_OverlayTextOverlay			)

	//BorderPanelOverlay Parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_BorderPanelOverlaySizes	)
	DECLARE_ATTRIBUTE_PARSER( Parser_BorderPanelOverlayMaterial	)

	// TextOverlay Parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_TextOverlayFont			)
	DECLARE_ATTRIBUTE_PARSER( Parser_TextOverlayText			)

	// Camera parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_CameraParent				)
	DECLARE_ATTRIBUTE_PARSER( Parser_CameraViewport				)
	DECLARE_ATTRIBUTE_PARSER( Parser_CameraPrimitive			)
	DECLARE_ATTRIBUTE_PARSER( Parser_CameraEnd					)

	// Viewport parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_ViewportType				)
	DECLARE_ATTRIBUTE_PARSER( Parser_ViewportLeft				)
	DECLARE_ATTRIBUTE_PARSER( Parser_ViewportRight				)
	DECLARE_ATTRIBUTE_PARSER( Parser_ViewportTop				)
	DECLARE_ATTRIBUTE_PARSER( Parser_ViewportBottom				)
	DECLARE_ATTRIBUTE_PARSER( Parser_ViewportNear				)
	DECLARE_ATTRIBUTE_PARSER( Parser_ViewportFar				)
	DECLARE_ATTRIBUTE_PARSER( Parser_ViewportSize				)
	DECLARE_ATTRIBUTE_PARSER( Parser_ViewportFovY				)
	DECLARE_ATTRIBUTE_PARSER( Parser_ViewportAspectRatio		)

	// Billboard parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_BillboardParent			)
	DECLARE_ATTRIBUTE_PARSER( Parser_BillboardPositions			)
	DECLARE_ATTRIBUTE_PARSER( Parser_BillboardMaterial			)
	DECLARE_ATTRIBUTE_PARSER( Parser_BillboardDimensions		)
	DECLARE_ATTRIBUTE_PARSER( Parser_BillboardEnd				)
	DECLARE_ATTRIBUTE_PARSER( Parser_BillboardPoint				)

	// Animated object group parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_GroupAnimatedObject		)
	DECLARE_ATTRIBUTE_PARSER( Parser_GroupAnimation				)
}

#pragma warning( pop )

#endif
