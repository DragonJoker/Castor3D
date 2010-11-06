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

#	ifdef GL2RENDERSYSTEM_EXPORTS
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

	template <typename T, size_t Count> class GLVertexAttribsBuffer;
	class GLVBOVertexBuffer;
	class GLVBONormalsBuffer;
	class GLVBOTextureBuffer;
	typedef GLVertexAttribsBuffer<real, 3> GLVertexAttribsBuffer3r;
	typedef GLVertexAttribsBuffer<real, 2> GLVertexAttribsBuffer2r;

	typedef Templates::SharedPtr<GLVBOVertexBuffer>			GLVBOVertexBufferPtr;
	typedef Templates::SharedPtr<GLVBONormalsBuffer>		GLVBONormalsBufferPtr;
	typedef Templates::SharedPtr<GLVBOTextureBuffer>		GLVBOTextureBufferPtr;
	typedef Templates::SharedPtr<GLVertexAttribsBuffer2r>	GLVertexAttribsBuffer2rPtr;
	typedef Templates::SharedPtr<GLVertexAttribsBuffer3r>	GLVertexAttribsBuffer3rPtr;
	typedef Templates::SharedPtr<GLShaderProgram>			GLShaderProgramPtr;
}

typedef C3DMap( Castor3D::RenderWindow *, Castor3D::GLContext *) ContextPtrMap;

#if CASTOR_USE_DOUBLE
#	define glTranslate			glTranslated
#	define glRotate				glRotated
#	define glMultMatrix			glMultMatrixd
#	define glScale				glScaled
#	define glMultiTexCoord2		glMultiTexCoord2dARB
#	define GL_REAL				GL_DOUBLE
#	define glVertex2			glVertex2d
#	define glTexCoord2			glTexCoord2d
#	define glVertex3			glVertex3d
#else
#	define glTranslate			glTranslatef
#	define glRotate				glRotatef
#	define glMultMatrix			glMultMatrixf
#	define glScale				glScalef
#	define glMultiTexCoord2		glMultiTexCoord2fARB
#	define GL_REAL				GL_FLOAT
#	define glVertex2			glVertex2f
#	define glTexCoord2			glTexCoord2f
#	define glVertex3			glVertex3f
#endif

#	ifdef __GNUG__
#		undef CS3D_GL_API
#		define CS3D_GL_API
#		define GLGetProcAddress(x)	glXGetProcAddress(reinterpret_cast <const GLubyte *>( x))
#		define GLCreateContext		glXCreateNewContext
#		define GLMakeCurrent		glXMakeContextCurrent
//#		define GLCreateContext		glXCreateContext
//#		define GLMakeCurrent		glXMakeCurrent
#		define GLGetCurrentContext	glXGetCurrentContext
#	else
#		ifdef WIN32
#			define GLGetProcAddress(x)	wglGetProcAddress(x)
#			define GLCreateContext		wglCreateContext
#			define GLMakeCurrent		wglMakeCurrent
#			define GLGetCurrentContext	wglGetCurrentContext
#		endif
#	endif

#	if CHECK_MEMORYLEAKS
#		include <CastorUtils/Memory.h>
#	endif

#endif