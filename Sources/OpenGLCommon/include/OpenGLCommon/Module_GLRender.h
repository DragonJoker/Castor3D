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
#ifndef ___GL_ModuleRender___
#define ___GL_ModuleRender___

#include <Castor3D/material/TextureUnit.h>

namespace Castor3D
{
	class RenderWindow;
	class Geometry;

	template <typename T, size_t Count> class GLVertexAttribs;
	template <typename T> class GLVertexBufferObject;
	template <typename T, size_t Count> class GLVertexAttribsBuffer;
	class GLVBOVertexBuffer;
	class GLVBONormalsBuffer;
	class GLVBOTextureBuffer;
	class GLVertexInfosBufferObject;
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
	class GLWindowRenderer;
	class GLOverlayRenderer;
	class GLShaderObject;
	class GLShaderProgram;
	class GLVertexShader;
	class GLFragmentShader;
	class GLGeometryShader;
	class GLFrameVariableBase;
	class GLIntFrameVariable;
	class GLRealFrameVariable;
	template <typename T> class GLOneFrameVariable;
	template <typename T, size_t Count> class GLPointFrameVariable;
	template <typename T, size_t Rows, size_t Columns> class GLMatrixFrameVariable;
	class GLPipeline;

	typedef GLVertexAttribs<real, 3>		GLVertexAttribs3r;
	typedef GLVertexAttribs<real, 2>		GLVertexAttribs2r;
	typedef GLVertexAttribs<int, 3>			GLVertexAttribs3i;
	typedef GLVertexAttribs<int, 2>			GLVertexAttribs2i;
	typedef GLVertexAttribsBuffer<real, 3>	GLVertexAttribsBuffer3r;
	typedef GLVertexAttribsBuffer<real, 2>	GLVertexAttribsBuffer2r;
	typedef GLVertexAttribsBuffer<int, 3>	GLVertexAttribsBuffer3i;
	typedef GLVertexAttribsBuffer<int, 2>	GLVertexAttribsBuffer2i;

	typedef Templates::SharedPtr<GLVBOVertexBuffer>			GLVBOVertexBufferPtr;
	typedef Templates::SharedPtr<GLVBONormalsBuffer>		GLVBONormalsBufferPtr;
	typedef Templates::SharedPtr<GLVBOTextureBuffer>		GLVBOTextureBufferPtr;
	typedef Templates::SharedPtr<GLVertexAttribs3r>			GLVertexAttribs3rPtr;
	typedef Templates::SharedPtr<GLVertexAttribs2r>			GLVertexAttribs2rPtr;
	typedef Templates::SharedPtr<GLVertexAttribs3i>			GLVertexAttribs3iPtr;
	typedef Templates::SharedPtr<GLVertexAttribs2i>			GLVertexAttribs2iPtr;
	typedef Templates::SharedPtr<GLVertexAttribsBuffer3r>	GLVertexAttribsBuffer3rPtr;
	typedef Templates::SharedPtr<GLVertexAttribsBuffer2r>	GLVertexAttribsBuffer2rPtr;
	typedef Templates::SharedPtr<GLVertexAttribsBuffer3i>	GLVertexAttribsBuffer3iPtr;
	typedef Templates::SharedPtr<GLVertexAttribsBuffer2i>	GLVertexAttribsBuffer2iPtr;
	typedef Templates::SharedPtr <GLShaderObject>			GLShaderObjectPtr;
	typedef Templates::SharedPtr <GLShaderProgram>			GLShaderProgramPtr;
	typedef Templates::SharedPtr <GLVertexShader>			GLVertexShaderPtr;
	typedef Templates::SharedPtr <GLFragmentShader>			GLFragmentShaderPtr;
	typedef Templates::SharedPtr <GLGeometryShader>			GLGeometryShaderPtr;
	typedef Templates::SharedPtr <GLFrameVariableBase>		GLFrameVariablePtr;

	typedef C3DVector(	GLShaderProgramPtr)							GLShaderProgramPtrArray;
	typedef C3DVector(	GLShaderObjectPtr)							GLShaderObjectPtrArray;
	typedef C3DMap(		String,				GLFrameVariablePtr)		GLFrameVariablePtrStrMap;

	void	GLCheckError			( const String & p_strText);
	Char *	GetGLSLErrorString		( int p_index);
	int		GetDrawType				( eDRAW_TYPE p_index);
	int 	GetEnvironmentMode		( EnvironmentMode p_index);
	int 	GetRGBCombination		( RGBCombination p_index);
	int 	GetRGBOperand			( RGBOperand p_index);
	int 	GetAlphaCombination		( AlphaCombination p_index);
	int 	GetAlphaOperand			( AlphaOperand p_index);
	int 	GetCombinationSource	( CombinationSource p_index);
	int 	GetTextureDimension		( TextureUnit::eDIMENSION p_index);
	int		GetLightIndex			( LightIndexes p_uiIndex);
	int		GetPixelFormatBits		( Castor::Resource::PixelFormat p_pixelFormat);
}

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
#		define glCreateContext				glXCreateNewContext
#		define glMakeCurrent				glXMakeContextCurrent
#		define glDeleteContext				glXDestroyContext
#		define glSwapBuffers( context)		glXSwapBuffers( context->GetDisplay(), context->GetWindow())
#		define GL_CONTEXT_MAJOR_VERSION		GLX_CONTEXT_MAJOR_VERSION_ARB
#		define GL_CONTEXT_MINOR_VERSION		GLX_CONTEXT_MINOR_VERSION_ARB
#		define GL_CONTEXT_FLAGS_ARB			GLX_CONTEXT_FLAGS_ARB
#		define GL_FORWARD_COMPATIBLE_BIT	GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB
#		define glCreateContextAttribs		glXCreateContextAttribsARB
#	else
#		ifdef WIN32
#			define glCreateContext				wglCreateContext
#			define glMakeCurrent				wglMakeCurrent
#			define glSwapBuffers( context)		SwapBuffers( context->GetHDC())
#			define glDeleteContext				wglDeleteContext
#			define glShareLists					wglShareLists
#			define glGetCurrentDC				wglGetCurrentDC
#			define GL_CONTEXT_MAJOR_VERSION		WGL_CONTEXT_MAJOR_VERSION_ARB
#			define GL_CONTEXT_MINOR_VERSION		WGL_CONTEXT_MINOR_VERSION_ARB
#			define GL_CONTEXT_FLAGS_ARB			WGL_CONTEXT_FLAGS_ARB
#			define GL_FORWARD_COMPATIBLE_BIT	WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB
#			define glCreateContextAttribs		wglCreateContextAttribsARB
#		endif
#	endif

#ifdef _DEBUG
#	define CheckGLError( x) GLCheckError( x)
#else
#	define CheckGLError( x) GLCheckError( x)
#endif

#endif