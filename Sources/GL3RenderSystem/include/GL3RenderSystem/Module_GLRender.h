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
#ifndef ___MODULE_GLRENDER___
#define ___MODULE_GLRENDER___

#	ifdef GL3RENDERSYSTEM_EXPORTS
#		define CS3D_GL_API __declspec(dllexport)
#	else
#		define CS3D_GL_API __declspec(dllimport)
#	endif

namespace Castor3D
{
	class GLRenderSystem;
	class GLPlugin;
	class GLContext;
	class GLSubmeshRenderer;
	class GLMeshRenderer;
	class GLGeometryRenderer;
	class GLTextureEnvironmentRenderer;
	class GLTextureRenderer;
	class GLMaterialRenderer;
	class GLLightRenderer;
	class GLCameraRenderer;
	class GLViewportRenderer;
	class GLWindowRenderer;
	class GLSceneNodeRenderer;
	class GLOverlayRenderer;
	class GLShaderProgram;

	class RenderWindow;
	class Geometry;

	class GLUniformVariable;
	class GLOneIntUniformVariable;
	class GLTwoIntsUniformVariable;
	class GLThreeIntsUniformVariable;
	class GLFourIntsUniformVariable;
	class GLOneFloatUniformVariable;
	class GLTwoFloatsUniformVariable;
	class GLThreeFloatsUniformVariable;
	class GLFourFloatsUniformVariable;
	class GLMatrix2fUniformVariable;
	class GLMatrix3fUniformVariable;
	class GLMatrix4fUniformVariable;

	class GLUBOVariable;
	class GLUniformBufferObject;

	template <typename T, size_t Count> class GLVertexAttribsBuffer;
	class GLVBOVertexBuffer;
	class GLVBONormalsBuffer;
	class GLVBOTextureBuffer;
	typedef GLVertexAttribsBuffer<real, 3> GLVertexAttribsBuffer3r;
	typedef GLVertexAttribsBuffer<real, 2> GLVertexAttribsBuffer2r;
	typedef GLVertexAttribsBuffer<int, 2> GLVertexAttribsBuffer2i;

	typedef Templates::SharedPtr<GLVBOVertexBuffer>			GLVBOVertexBufferPtr;
	typedef Templates::SharedPtr<GLVBONormalsBuffer>		GLVBONormalsBufferPtr;
	typedef Templates::SharedPtr<GLVBOTextureBuffer>		GLVBOTextureBufferPtr;
	typedef Templates::SharedPtr<GLVertexAttribsBuffer2r>	GLVertexAttribsBuffer2rPtr;
	typedef Templates::SharedPtr<GLVertexAttribsBuffer3r>	GLVertexAttribsBuffer3rPtr;
	typedef Templates::SharedPtr<GLVertexAttribsBuffer2i>	GLVertexAttribsBuffer2iPtr;
	typedef Templates::SharedPtr<GLShaderProgram>			GLShaderProgramPtr;
	typedef Templates::SharedPtr<GLUniformBufferObject>		GLUniformBufferObjectPtr;
}

typedef C3DMap( Castor3D::RenderWindow *, Castor3D::GLContext *) ContextPtrMap;

#	ifdef __GNUG__
#		undef CS3D_GL_API
#		define CS3D_GL_API
#		define GLGetProcAddress(x)		glXGetProcAddress(reinterpret_cast <const GLubyte *>( x))
#		define GLCreateContext			glXCreateNewContext
#		define GLMakeCurrent			glXMakeContextCurrent
//#		define GLCreateContext			glXCreateContext
//#		define GLMakeCurrent			glXMakeCurrent
#		define GLGetCurrentContext		glXGetCurrentContext
#		define GLSwapBuffers( context)	glXSwapBuffers( context->GetDisplay(), context->GetWindow())
#	else
#		ifdef WIN32
#			define GLGetProcAddress(x)		wglGetProcAddress(x)
#			define GLCreateContext			wglCreateContext
#			define GLMakeCurrent			wglMakeCurrent
#			define GLGetCurrentContext		wglGetCurrentContext
#			define GLSwapBuffers( context)	SwapBuffers( context->GetDC())
#		endif
#	endif

#endif