/*
This source file is part of Castor3D (http://dragonjoker.co.cc

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_SceneFileParser_Parsers___
#define ___C3D_SceneFileParser_Parsers___

#include "Module_Scene.h"

namespace Castor3D
{
	// Root parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_RootSceneName)
	DECLARE_ATTRIBUTE_PARSER( Parser_RootCamera)
	DECLARE_ATTRIBUTE_PARSER( Parser_RootLight)
	DECLARE_ATTRIBUTE_PARSER( Parser_RootSceneNode)
	DECLARE_ATTRIBUTE_PARSER( Parser_RootMaterial)
	DECLARE_ATTRIBUTE_PARSER( Parser_RootObject)

	// Light parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_LightType)
	DECLARE_ATTRIBUTE_PARSER( Parser_LightPosition)
	DECLARE_ATTRIBUTE_PARSER( Parser_LightAmbient)
	DECLARE_ATTRIBUTE_PARSER( Parser_LightDiffuse)
	DECLARE_ATTRIBUTE_PARSER( Parser_LightSpecular)
	DECLARE_ATTRIBUTE_PARSER( Parser_LightAttenuation)
	DECLARE_ATTRIBUTE_PARSER( Parser_LightCutOff)
	DECLARE_ATTRIBUTE_PARSER( Parser_LightExponent)
	DECLARE_ATTRIBUTE_PARSER( Parser_LightOrientation)
	DECLARE_ATTRIBUTE_PARSER( Parser_LightEnd)

	// Scene node parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_NodeParent)
	DECLARE_ATTRIBUTE_PARSER( Parser_NodePosition)
	DECLARE_ATTRIBUTE_PARSER( Parser_NodeOrientation)
	DECLARE_ATTRIBUTE_PARSER( Parser_NodeScale)
	DECLARE_ATTRIBUTE_PARSER( Parser_NodeEnd)

	// Object Parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_ObjectParent)
	DECLARE_ATTRIBUTE_PARSER( Parser_ObjectMesh)
	DECLARE_ATTRIBUTE_PARSER( Parser_ObjectEnd)

	// Mesh Parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_MeshType)
	DECLARE_ATTRIBUTE_PARSER( Parser_MeshFile)
	DECLARE_ATTRIBUTE_PARSER( Parser_MeshMaterial)
	DECLARE_ATTRIBUTE_PARSER( Parser_MeshSubmesh)
	DECLARE_ATTRIBUTE_PARSER( Parser_MeshEnd)


	// Submesh Parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_SubmeshMaterial)
	DECLARE_ATTRIBUTE_PARSER( Parser_SubmeshVertex)
	DECLARE_ATTRIBUTE_PARSER( Parser_SubmeshSmoothingGroup)
	DECLARE_ATTRIBUTE_PARSER( Parser_SubmeshEnd)

	// Smoothing Group Parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_SmoothingGroupFace)
	DECLARE_ATTRIBUTE_PARSER( Parser_SmoothingGroupEnd)

	// Material Parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_MaterialPass)
	DECLARE_ATTRIBUTE_PARSER( Parser_MaterialEnd)

	// Pass Parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_PassAmbient)
	DECLARE_ATTRIBUTE_PARSER( Parser_PassDiffuse)
	DECLARE_ATTRIBUTE_PARSER( Parser_PassSpecular)
	DECLARE_ATTRIBUTE_PARSER( Parser_PassEmissive)
	DECLARE_ATTRIBUTE_PARSER( Parser_PassShininess)
	DECLARE_ATTRIBUTE_PARSER( Parser_PassTextureUnit)
	DECLARE_ATTRIBUTE_PARSER( Parser_PassShader)
	DECLARE_ATTRIBUTE_PARSER( Parser_PassEnd)

	// Texture Unit Parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_UnitImage)
	DECLARE_ATTRIBUTE_PARSER( Parser_UnitColour)
	DECLARE_ATTRIBUTE_PARSER( Parser_UnitMapType)
	DECLARE_ATTRIBUTE_PARSER( Parser_UnitEnd)

	// Shader Program Parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_ShaderVertexProgram)
	DECLARE_ATTRIBUTE_PARSER( Parser_ShaderPixelProgram)
	DECLARE_ATTRIBUTE_PARSER( Parser_ShaderGeometryProgram)
	DECLARE_ATTRIBUTE_PARSER( Parser_ShaderVertexFile)
	DECLARE_ATTRIBUTE_PARSER( Parser_ShaderPixelFile)
	DECLARE_ATTRIBUTE_PARSER( Parser_ShaderGeometryFile)
	DECLARE_ATTRIBUTE_PARSER( Parser_ShaderVariable)
	DECLARE_ATTRIBUTE_PARSER( Parser_ShaderEnd)

	// Shader Variables Parsers
	DECLARE_ATTRIBUTE_PARSER( Parser_ShaderVariableType)
	DECLARE_ATTRIBUTE_PARSER( Parser_ShaderVariableName)
	DECLARE_ATTRIBUTE_PARSER( Parser_ShaderVariableValue)
	DECLARE_ATTRIBUTE_PARSER( Parser_ShaderVariableEnd)
}

#endif