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
#ifndef ___GL3_ModuleRender___
#define ___GL3_ModuleRender___

#include <OpenGLCommon/Module_GLRender.h>

#	ifdef GL3RENDERSYSTEM_EXPORTS
#		define C3D_GL3_API __declspec(dllexport)
#	else
#		define C3D_GL3_API __declspec(dllimport)
#	endif

namespace Castor3D
{
	class GLUniformBufferObject;
	class GLUBOVariableBase;
	template <typename T> class GLUBOOneVariable;
	template <typename T, size_t Count> class GLUBOPointVariable;
	template <typename T, size_t Rows, size_t Columns> class GLUBOMatrixVariable;
	template <typename T, size_t Rows> class GLUBOSquareMatrixVariable;
	class GL3ShaderProgram;
	class GL3Context;
	class GL3LightRenderer;
	class GL3PassRenderer;

	class GL4ShaderProgram;
	class GL4LightRenderer;
	class GL4PassRenderer;

	typedef GLUBOOneVariable<float>					GLUBOFloatVariable;
	typedef GLUBOOneVariable<int>					GLUBOIntVariable;
	typedef GLUBOPointVariable<float,	2>			GLUBOPoint2fVariable;
	typedef GLUBOPointVariable<int,		2>			GLUBOPoint2iVariable;
	typedef GLUBOPointVariable<float,	3>			GLUBOPoint3fVariable;
	typedef GLUBOPointVariable<int,		3>			GLUBOPoint3iVariable;
	typedef GLUBOPointVariable<float,	4>			GLUBOPoint4fVariable;
	typedef GLUBOPointVariable<int,		4>			GLUBOPoint4iVariable;
	typedef GLUBOMatrixVariable<real,	2,	2>		GLUBOMatrix2x2rVariable;
	typedef GLUBOMatrixVariable<real,	3,	3>		GLUBOMatrix3x3rVariable;
	typedef GLUBOMatrixVariable<real,	4,	4>		GLUBOMatrix4x4rVariable;
	typedef GLUBOMatrixVariable<float,	2,	2>		GLUBOMatrix2x2fVariable;
	typedef GLUBOMatrixVariable<float,	3,	3>		GLUBOMatrix3x3fVariable;
	typedef GLUBOMatrixVariable<float,	4,	4>		GLUBOMatrix4x4fVariable;

	typedef Templates::SharedPtr <GLUniformBufferObject>		GLUniformBufferObjectPtr;
	typedef Templates::SharedPtr <GLUBOVariableBase>			GLUBOVariablePtr;
	typedef Templates::SharedPtr <GL3ShaderProgram>				GL3ShaderProgramPtr;
	typedef Templates::SharedPtr <GLUBOFloatVariable>			GLUBOFloatVariablePtr;
	typedef Templates::SharedPtr <GLUBOIntVariable>				GLUBOIntVariablePtr;
	typedef Templates::SharedPtr <GLUBOPoint2fVariable>			GLUBOPoint2fVariablePtr;
	typedef Templates::SharedPtr <GLUBOPoint2iVariable>			GLUBOPoint2iVariablePtr;
	typedef Templates::SharedPtr <GLUBOPoint3fVariable>			GLUBOPoint3fVariablePtr;
	typedef Templates::SharedPtr <GLUBOPoint3iVariable>			GLUBOPoint3iVariablePtr;
	typedef Templates::SharedPtr <GLUBOPoint4fVariable>			GLUBOPoint4fVariablePtr;
	typedef Templates::SharedPtr <GLUBOPoint4iVariable>			GLUBOPoint4iVariablePtr;
	typedef Templates::SharedPtr <GLUBOMatrix2x2rVariable>		GLUBOMatrix2x2rVariablePtr;
	typedef Templates::SharedPtr <GLUBOMatrix3x3rVariable>		GLUBOMatrix3x3rVariablePtr;
	typedef Templates::SharedPtr <GLUBOMatrix4x4rVariable>		GLUBOMatrix4x4rVariablePtr;
	typedef Templates::SharedPtr <GLUBOMatrix2x2fVariable>		GLUBOMatrix2x2fVariablePtr;
	typedef Templates::SharedPtr <GLUBOMatrix3x3fVariable>		GLUBOMatrix3x3fVariablePtr;
	typedef Templates::SharedPtr <GLUBOMatrix4x4fVariable>		GLUBOMatrix4x4fVariablePtr;


	typedef C3DMap(		String,				GLUniformBufferObjectPtr)	GLUniformBufferObjectPtrStrMap;
	typedef C3DMap(		String,				GLUBOVariablePtr)			GLUBOVariablePtrStrMap;
}

#endif