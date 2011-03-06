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
#ifndef ___Gl3_ModuleRender___
#define ___Gl3_ModuleRender___

#include <OpenGlCommon/Module_GlRender.h>

#ifdef _WIN32
#	ifdef Gl3RenderSystem_EXPORTS
#		define C3D_Gl3_API __declspec(dllexport)
#	else
#		define C3D_Gl3_API __declspec(dllimport)
#	endif
#else
#	define C3D_Gl3_API
#endif

namespace Castor3D
{
	class GlUniformBufferObject;
	class GlUboVariableBase;
	template <typename T> class GlUboOneVariable;
	template <typename T, size_t Count> class GlUboPointVariable;
	template <typename T, size_t Rows, size_t Columns> class GlUboMatrixVariable;
	template <typename T, size_t Rows> class GlUboSquareMatrixVariable;
	class Gl3ShaderProgram;
	class Gl3LightRenderer;
	class Gl3PassRenderer;
	class GlVertexArrayObjects;
	class GlTextureBufferObject;

	class Gl3Context;

	class Gl4ShaderProgram;
	class Gl4LightRenderer;
	class Gl4PassRenderer;

	typedef GlUboOneVariable<float>					GlUboFloatVariable;
	typedef GlUboOneVariable<int>					GlUboIntVariable;
	typedef GlUboPointVariable<float,	2>			GlUboPoint2fVariable;
	typedef GlUboPointVariable<int,		2>			GlUboPoint2iVariable;
	typedef GlUboPointVariable<float,	3>			GlUboPoint3fVariable;
	typedef GlUboPointVariable<int,		3>			GlUboPoint3iVariable;
	typedef GlUboPointVariable<float,	4>			GlUboPoint4fVariable;
	typedef GlUboPointVariable<int,		4>			GlUboPoint4iVariable;
	typedef GlUboMatrixVariable<real,	2,	2>		GlUboMatrix2x2rVariable;
	typedef GlUboMatrixVariable<real,	3,	3>		GlUboMatrix3x3rVariable;
	typedef GlUboMatrixVariable<real,	4,	4>		GlUboMatrix4x4rVariable;
	typedef GlUboMatrixVariable<float,	2,	2>		GlUboMatrix2x2fVariable;
	typedef GlUboMatrixVariable<float,	3,	3>		GlUboMatrix3x3fVariable;
	typedef GlUboMatrixVariable<float,	4,	4>		GlUboMatrix4x4fVariable;

	typedef shared_ptr <GlUniformBufferObject>		GlUniformBufferObjectPtr;
	typedef shared_ptr <GlUboVariableBase>			GlUboVariablePtr;
	typedef shared_ptr <Gl3ShaderProgram>			Gl3ShaderProgramPtr;
	typedef shared_ptr <GlUboFloatVariable>			GlUboFloatVariablePtr;
	typedef shared_ptr <GlUboIntVariable>			GlUboIntVariablePtr;
	typedef shared_ptr <GlUboPoint2fVariable>		GlUboPoint2fVariablePtr;
	typedef shared_ptr <GlUboPoint2iVariable>		GlUboPoint2iVariablePtr;
	typedef shared_ptr <GlUboPoint3fVariable>		GlUboPoint3fVariablePtr;
	typedef shared_ptr <GlUboPoint3iVariable>		GlUboPoint3iVariablePtr;
	typedef shared_ptr <GlUboPoint4fVariable>		GlUboPoint4fVariablePtr;
	typedef shared_ptr <GlUboPoint4iVariable>		GlUboPoint4iVariablePtr;
	typedef shared_ptr <GlUboMatrix2x2rVariable>	GlUboMatrix2x2rVariablePtr;
	typedef shared_ptr <GlUboMatrix3x3rVariable>	GlUboMatrix3x3rVariablePtr;
	typedef shared_ptr <GlUboMatrix4x4rVariable>	GlUboMatrix4x4rVariablePtr;
	typedef shared_ptr <GlUboMatrix2x2fVariable>	GlUboMatrix2x2fVariablePtr;
	typedef shared_ptr <GlUboMatrix3x3fVariable>	GlUboMatrix3x3fVariablePtr;
	typedef shared_ptr <GlUboMatrix4x4fVariable>	GlUboMatrix4x4fVariablePtr;
	typedef shared_ptr <GlVertexArrayObjects>		GlVertexArrayObjectsPtr;
	typedef shared_ptr <GlTextureBufferObject>		GlTextureBufferObjectPtr;


	typedef KeyedContainer<String,	GlUniformBufferObjectPtr>::Map	GlUniformBufferObjectPtrStrMap;
	typedef KeyedContainer<String,	GlUboVariablePtr>::Map			GlUboVariablePtrStrMap;
}

#endif
