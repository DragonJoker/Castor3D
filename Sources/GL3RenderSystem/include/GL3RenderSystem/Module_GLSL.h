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
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___GL_Module_GLSL___
#define ___GL_Module_GLSL___

#include "Module_GLRender.h"

namespace Castor3D
{
	class GLShaderObject;
	class GLShaderProgram;
	class GLVertexShader;
	class GLFragmentShader;
	class GLGeometryShader;
	class GLUniformVariable;
	class GLUniformBufferObject;
	class GLUBOVariable;

	typedef Templates::SharedPtr <GLShaderObject>			GLShaderObjectPtr;
	typedef Templates::SharedPtr <GLShaderProgram>			GLShaderProgramPtr;
	typedef Templates::SharedPtr <GLVertexShader>			GLVertexShaderPtr;
	typedef Templates::SharedPtr <GLFragmentShader>			GLFragmentShaderPtr;
	typedef Templates::SharedPtr <GLGeometryShader>			GLGeometryShaderPtr;
	typedef Templates::SharedPtr <GLUniformVariable>		GLUniformVariablePtr;
	typedef Templates::SharedPtr <GLUniformBufferObject>	GLUniformBufferObjectPtr;
	typedef Templates::SharedPtr <GLUBOVariable>			GLUBOVariablePtr;

	typedef C3DVector(	GLShaderProgramPtr)								GLShaderProgramPtrArray;
	typedef C3DVector(	GLShaderObjectPtr)								GLShaderObjectPtrArray;
	typedef C3DMap(		String,				GLUniformVariablePtr)		GLUniformVariablePtrStrMap;
	typedef C3DMap(		String,				GLUniformBufferObjectPtr)	GLUniformBufferObjectPtrStrMap;
	typedef C3DMap(		String,				GLUBOVariablePtr)			GLUBOVariablePtrStrMap;
}

#endif