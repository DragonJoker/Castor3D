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
	class CgGLShaderProgram;
	class CgGLVertexShader;
	class CgGLFragmentShader;
	class CgGLGeometryShader;
	class CgGLFrameVariableBase;
	class CgGLIntFrameVariable;
	class CgGLRealFrameVariable;
	template <typename T> class CgGLOneFrameVariable;
	template <typename T, size_t Count> class CgGLPointFrameVariable;
	template <typename T, size_t Rows, size_t Columns> class CgGLMatrixFrameVariable;
	class GLPipeline;

	typedef GLVertexAttribs<real, 3>		GLVertexAttribs3r;
	typedef GLVertexAttribs<real, 2>		GLVertexAttribs2r;
	typedef GLVertexAttribs<int, 3>			GLVertexAttribs3i;
	typedef GLVertexAttribs<int, 2>			GLVertexAttribs2i;
	typedef GLVertexAttribsBuffer<real, 3>	GLVertexAttribsBuffer3r;
	typedef GLVertexAttribsBuffer<real, 2>	GLVertexAttribsBuffer2r;
	typedef GLVertexAttribsBuffer<int, 3>	GLVertexAttribsBuffer3i;
	typedef GLVertexAttribsBuffer<int, 2>	GLVertexAttribsBuffer2i;

	typedef SmartPtr<GLVBOVertexBuffer>::Shared		GLVBOVertexBufferPtr;
	typedef SmartPtr<GLVBONormalsBuffer>::Shared		GLVBONormalsBufferPtr;
	typedef SmartPtr<GLVBOTextureBuffer>::Shared		GLVBOTextureBufferPtr;
	typedef SmartPtr<GLVertexAttribs3r>::Shared		GLVertexAttribs3rPtr;
	typedef SmartPtr<GLVertexAttribs2r>::Shared		GLVertexAttribs2rPtr;
	typedef SmartPtr<GLVertexAttribs3i>::Shared		GLVertexAttribs3iPtr;
	typedef SmartPtr<GLVertexAttribs2i>::Shared		GLVertexAttribs2iPtr;
	typedef SmartPtr<GLVertexAttribsBuffer3r>::Shared	GLVertexAttribsBuffer3rPtr;
	typedef SmartPtr<GLVertexAttribsBuffer2r>::Shared	GLVertexAttribsBuffer2rPtr;
	typedef SmartPtr<GLVertexAttribsBuffer3i>::Shared	GLVertexAttribsBuffer3iPtr;
	typedef SmartPtr<GLVertexAttribsBuffer2i>::Shared	GLVertexAttribsBuffer2iPtr;
	typedef SmartPtr <GLShaderObject>::Shared			GLShaderObjectPtr;
	typedef SmartPtr <GLShaderProgram>::Shared		GLShaderProgramPtr;
	typedef SmartPtr <GLVertexShader>::Shared			GLVertexShaderPtr;
	typedef SmartPtr <GLFragmentShader>::Shared		GLFragmentShaderPtr;
	typedef SmartPtr <GLGeometryShader>::Shared		GLGeometryShaderPtr;
	typedef SmartPtr <GLFrameVariableBase>::Shared	GLFrameVariablePtr;
	typedef SmartPtr <CgGLShaderProgram>::Shared		CgGLShaderProgramPtr;
	typedef SmartPtr <CgGLVertexShader>::Shared		CgGLVertexShaderPtr;
	typedef SmartPtr <CgGLFragmentShader>::Shared		CgGLFragmentShaderPtr;
	typedef SmartPtr <CgGLGeometryShader>::Shared		CgGLGeometryShaderPtr;
	typedef SmartPtr <CgGLFrameVariableBase>::Shared	CgGLFrameVariablePtr;

	typedef Container<	GLShaderProgramPtr>::Vector						GLShaderProgramPtrArray;
	typedef Container<	CgGLShaderProgramPtr>::Vector					CgGLShaderProgramPtrArray;
	typedef Container<	GLShaderObjectPtr>::Vector						GLShaderObjectPtrArray;
	typedef KeyedContainer<	String,				GLFrameVariablePtr>::Map	GLFrameVariablePtrStrMap;
	typedef KeyedContainer<	String,				CgGLFrameVariablePtr>::Map	CgGLFrameVariablePtrStrMap;

	void	GLCheckError			( const String & p_strText);
	void	CgCheckError			( const String & p_strText);
	Char *	GetGLSLErrorString		( int p_index);
	int		GetDrawType				( eDRAW_TYPE p_index);
	int 	GetEnvironmentMode		( TextureEnvironment::eMODE p_index);
	int 	GetRGBCombination		( TextureEnvironment::eRGB_COMBINATION p_index);
	int 	GetRGBOperand			( TextureEnvironment::eRGB_OPERAND p_index);
	int 	GetAlphaCombination		( TextureEnvironment::eALPHA_COMBINATION p_index);
	int 	GetAlphaOperand			( TextureEnvironment::eALPHA_OPERAND p_index);
	int 	GetCombinationSource	( TextureEnvironment::eCOMBINATION_SOURCE p_index);
	int 	GetTextureDimension		( TextureUnit::eDIMENSION p_index);
	int		GetLightIndex			( eLIGHT_INDEXES p_uiIndex);
	int		GetPixelFormatBits		( Castor::Resources::PixelFormat p_pixelFormat);
}

#if CASTOR_USE_DOUBLE
#	define glTranslate			glTranslated
#	define glRotate				glRotated
#	define glMultMatrix			glMultMatrixd
#	define glScale				glScaled
#	define glMultiTexCoord2		glMultiTexCoord2dARB
#	define GL_REAL				GL_DOUBLE
#	define glVertex2r			glVertex2d
#	define glVertex2rv			glVertex2dv
#	define glTexCoord2r			glTexCoord2d
#	define glTexCoord2rv		glTexCoord2dv
#	define glVertex3r			glVertex3d
#	define glVertex3rv			glVertex3dv
#	define glColor3r			glColor3d
#	define glColor3rv			glColor3dv
#else
#	define glTranslate			glTranslatef
#	define glRotate				glRotatef
#	define glMultMatrix			glMultMatrixf
#	define glScale				glScalef
#	define glMultiTexCoord2		glMultiTexCoord2fARB
#	define GL_REAL				GL_FLOAT
#	define glVertex2r			glVertex2f
#	define glVertex2rv			glVertex2fv
#	define glTexCoord2r			glTexCoord2f
#	define glTexCoord2rv		glTexCoord2fv
#	define glVertex3r			glVertex3f
#	define glVertex3rv			glVertex3fv
#	define glColor3r			glColor3f
#	define glColor3rv			glColor3fv

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
#	define CheckCgError( x) CgCheckError( x)
#else
#	define CheckGLError( x) GLCheckError( x)
#	define CheckCgError( x) CgCheckError( x)
#endif

#endif