/*
This source file is part of Castor3D (https://sourceforge.net/projects/castor3d/)

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
#ifndef ___C3DGL_GlObjects___
#define ___C3DGL_GlObjects___

#include "glew.h"

//*************************************************************************************************

#ifndef _WIN32
#	define CALLBACK
#else
#	define CALLBACK __stdcall
#endif

typedef void		(CALLBACK * PFNGLCLEARCOLORPROC)			( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
typedef void		(CALLBACK * PFNGLSHADEMODELPROC)			( GLenum mode);
typedef void		(CALLBACK * PFNGLVIEWPORTPROC)				( GLint x, GLint y, GLsizei width, GLsizei height);
typedef void		(CALLBACK * PFNGLMATRIXMODEPROC)			( GLenum mode);
typedef void		(CALLBACK * PFNGLLOADIDENTITYPROC)			( void);
typedef void		(CALLBACK * PFNGLORTHOPROC)					( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
typedef void		(CALLBACK * PFNGLFRUSTUMPROC)				( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
typedef void		(CALLBACK * PFNGLCLEARPROC)					( GLbitfield mask);
typedef void		(CALLBACK * PFNGLDRAWELEMENTSPROC)			( GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);
typedef void		(CALLBACK * PFNGLENABLEPROC)				( GLenum mode);
typedef void		(CALLBACK * PFNGLDISABLEPROC)				( GLenum mode);
typedef void		(CALLBACK * PFNGLGENTEXTURESPROC)			( GLsizei n, GLuint * textures);
typedef void		(CALLBACK * PFNGLDELETETEXTURESPROC)		( GLsizei n, const GLuint * textures);
typedef void		(CALLBACK * PFNGLBINDTEXTUREPROC)			( GLenum mode, GLuint texture);
typedef void		(CALLBACK * PFNGLTEXSUBIMAGE2DPROC)			( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid * data);
typedef void		(CALLBACK * PFNGLTEXIMAGE2DPROC)			( GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid * data);
typedef void		(CALLBACK * PFNGLTEXPARAMETERIPROC)			( GLenum target, GLenum pname, GLint param);
typedef void		(CALLBACK * PFNGLGETTEXIMAGEPROC)			( GLenum target, GLint level, GLenum format, GLenum type, GLvoid * pixels);
typedef void		(CALLBACK * PFNGLENABLECLIENTSTATEPROC)		( GLenum array);
typedef void		(CALLBACK * PFNGLDISABLECLIENTSTATEPROC)	( GLenum array);
typedef void		(CALLBACK * PFNGLVERTEXPOINTERPROC)			( GLint size, GLenum type, GLsizei stride, const GLvoid * pointer);
typedef void		(CALLBACK * PFNGLNORMALPOINTERPROC)			( GLenum type, GLsizei stride, const GLvoid * pointer);
typedef void		(CALLBACK * PFNGLTANGENTPOINTERPROC)		( GLenum type, GLsizei stride, const GLvoid * pointer);
typedef void		(CALLBACK * PFNGLCOLORPOINTERPROC)			( GLint size, GLenum type, GLsizei stride, const GLvoid * pointer);
typedef void		(CALLBACK * PFNGLTEXCOORDPOINTERPROC)		( GLint size, GLenum type, GLsizei stride, const GLvoid  *pointer);
typedef void		(CALLBACK * PFNGLTEXENVIPROC)				( GLenum target, GLenum pname, GLint param);
typedef void		(CALLBACK * PFNGLTEXENVIVPROC)				( GLenum target, GLenum pname, const GLint * param);
typedef void		(CALLBACK * PFNGLTEXENVFPROC)				( GLenum target, GLenum pname, GLfloat param);
typedef void		(CALLBACK * PFNGLTEXENVFVPROC)				( GLenum target, GLenum pname, const GLfloat * param);
typedef void		(CALLBACK * PFNGLBLENDFUNCPROC)				( GLenum sfactor, GLenum dfactor);
typedef void		(CALLBACK * PFNGLALPHAFUNCPROC)				( GLenum func, GLclampf ref);
typedef GLenum		(CALLBACK * PFNGLGETERRORPROC)				( void);
typedef void		(CALLBACK * PFNGLCOLOR3FPROC)				( GLfloat r, GLfloat g, GLfloat b);
typedef void		(CALLBACK * PFNGLCOLOR3FVPROC)				( const GLfloat * rgb);
typedef void		(CALLBACK * PFNGLCOLOR4FPROC)				( GLfloat r, GLfloat g, GLfloat b, GLfloat a);
typedef void		(CALLBACK * PFNGLCOLOR4FVPROC)				( const GLfloat * rgba);
typedef void		(CALLBACK * PFNGLLIGHTMODELIPROC)			( GLenum pname, GLint param);
typedef void		(CALLBACK * PFNGLLIGHTMODELIVPROC)			( GLenum pname, const GLint * param);
typedef void		(CALLBACK * PFNGLLIGHTMODELFPROC)			( GLenum pname, GLfloat param);
typedef void		(CALLBACK * PFNGLLIGHTMODELFVPROC)			( GLenum pname, const GLfloat * param);
typedef GLboolean	(CALLBACK * PFNGLISTEXTUREPROC)				( GLuint texture);
typedef void		(CALLBACK * PFNGLTEXGENIPROC)				( GLenum coord, GLenum pname, GLint param);
typedef void		(CALLBACK * PFNGLPUSHMATRIXPROC)			();
typedef void		(CALLBACK * PFNGLPOPMATRIXPROC)				();
typedef void		(CALLBACK * PFNGLTRANSLATEPROC)				( real x, real y, real z);
typedef void		(CALLBACK * PFNGLROTATEPROC)				( real angle, real x, real y, real z);
typedef void		(CALLBACK * PFNGLSCALEPROC)					( real x, real y, real z);
typedef void		(CALLBACK * PFNMULTMATRIXPROC)				( real const * matrix);
typedef void		(CALLBACK * PFNGLCULLFACEPROC)				( GLenum face);
typedef void		(CALLBACK * PFNGLFRONTFACEPROC)				( GLenum face);
typedef void		(CALLBACK * PFNGLMATERIALFPROC)				( GLenum face, GLenum pname, GLfloat param);
typedef void		(CALLBACK * PFNGLMATERIALFVPROC)			( GLenum face, GLenum pname, const GLfloat * params);
typedef void		(CALLBACK * PFNGLLIGHTFPROC)				( GLuint light, GLenum pname, GLfloat param);
typedef void		(CALLBACK * PFNGLLIGHTFVPROC)				( GLuint light, GLenum pname, const GLfloat * params);
typedef void		(CALLBACK * PFNGLSELECTBUFFERPROC)			( GLsizei size, GLuint * buffer);
typedef void		(CALLBACK * PFNGLGETINTEGERVPROC)			( GLenum pname, GLint * params);
typedef GLint		(CALLBACK * PFNGLRENDERMODEPROC)			( GLenum mode);

#ifdef _WIN32
typedef BOOL		(CALLBACK * PFNGLMAKECURRENTPROC)			( HDC hdc, HGLRC hglrc);
typedef BOOL		(CALLBACK * PFNGLSWAPBUFFERSPROC)			( HDC hdc);
typedef HGLRC		(CALLBACK * PFNGLCREATECONTEXTPROC)			( HDC hdc);
typedef BOOL		(CALLBACK * PFNGLDELETECONTEXTPROC)			( HGLRC hContext);
typedef HGLRC		(CALLBACK * PFNGLCREATECONTEXTATTRIBSPROC)	( HDC hDC, HGLRC hShareContext, int const * attribList);
#else
#	if CASTOR_GTK
typedef BOOL		(CALLBACK * PFNGLMAKECURRENTPROC)			( Display * pDisplay, GLXDrawable drawable, GLXContext context);
typedef BOOL		(CALLBACK * PFNGLSWAPBUFFERSPROC)			( Display * pDisplay, GLXDrawable drawable);
typedef GLXContext	(CALLBACK * PFNGLCREATECONTEXTPROC)			( Display * pDisplay, XVisualInfo * pVisualInfo, GLXContext shareList, Bool direct);
typedef BOOL		(CALLBACK * PFNGLDELETECONTEXTPROC)			( Display * pDisplay, GLXContext context);
typedef GLXContext	(CALLBACK * PFNGLCREATECONTEXTATTRIBSPROC)	( Display * pDisplay, GLXFBConfig fbconfig, GLXContext shareList, Bool direct, int const * attribList);
#endif
#endif


//*************************************************************************************************

namespace Castor3D
{
	class OpenGl
	{
	private:
		/**@name General */
		//@{
		PFNGLGETERRORPROC					m_pfnGetError;
		PFNGLCLEARCOLORPROC					m_pfnClearColor;
		PFNGLSHADEMODELPROC					m_pfnShadeModel;
		PFNGLCLEARPROC						m_pfnClear;
		PFNGLDRAWELEMENTSPROC				m_pfnDrawElements;
		PFNGLENABLEPROC						m_pfnEnable;
		PFNGLDISABLEPROC					m_pfnDisable;
		PFNGLCOLOR3FPROC					m_pfnColor3f;
		PFNGLCOLOR3FVPROC					m_pfnColor3fv;
		PFNGLCOLOR4FPROC					m_pfnColor4f;
		PFNGLCOLOR4FVPROC					m_pfnColor4fv;
		PFNGLSELECTBUFFERPROC				m_pfnSelectBuffer;
		PFNGLGETINTEGERVPROC				m_pfnGetIntegerv;
		PFNGLRENDERMODEPROC					m_pfnRenderMode;
		//@}

		/**@name Light */
		//@{
		PFNGLLIGHTFPROC						m_pfnLightf;
		PFNGLLIGHTFVPROC					m_pfnLightfv;
		PFNGLLIGHTMODELIPROC				m_pfnLightModeli;
		PFNGLLIGHTMODELIVPROC				m_pfnLightModeliv;
		PFNGLLIGHTMODELFPROC				m_pfnLightModelf;
		PFNGLLIGHTMODELFVPROC				m_pfnLightModelfv;
		//@}

		/**@name Context */
		//@{
		PFNGLMAKECURRENTPROC				m_pfnMakeCurrent;
		PFNGLSWAPBUFFERSPROC				m_pfnSwapBuffers;
		PFNGLCREATECONTEXTPROC				m_pfnCreateContext;
		PFNGLCREATECONTEXTATTRIBSPROC		m_pfnCreateContextAttribs;
		PFNGLDELETECONTEXTPROC				m_pfnDeleteContext;
		//@}

		/**@name Matrix */
		//@{
		PFNGLVIEWPORTPROC					m_pfnViewport;
		PFNGLMATRIXMODEPROC					m_pfnMatrixMode;
		PFNGLLOADIDENTITYPROC				m_pfnLoadIdentity;
		PFNGLORTHOPROC						m_pfnOrtho;
		PFNGLFRUSTUMPROC					m_pfnFrustum;
		PFNGLPUSHMATRIXPROC					m_pfnPushMatrix;
		PFNGLPOPMATRIXPROC					m_pfnPopMatrix;
		PFNGLTRANSLATEPROC					m_pfnTranslate;
		PFNGLROTATEPROC						m_pfnRotate;
		PFNGLSCALEPROC						m_pfnScale;
		PFNMULTMATRIXPROC					m_pfnMultMatrix;
		//@}

		/**@name Material */
		//@{
		PFNGLCULLFACEPROC					m_pfnCullFace;
		PFNGLFRONTFACEPROC					m_pfnFrontFace;
		PFNGLMATERIALFPROC					m_pfnMaterialf;
		PFNGLMATERIALFVPROC					m_pfnMaterialfv;
		//@}

		/**@name Texture */
		//@{
		PFNGLGENTEXTURESPROC				m_pfnGenTextures;
		PFNGLDELETETEXTURESPROC				m_pfnDeleteTextures;
		PFNGLBINDTEXTUREPROC				m_pfnBindTexture;
		PFNGLTEXSUBIMAGE2DPROC				m_pfnTexSubImage2D;
		PFNGLTEXIMAGE2DPROC					m_pfnTexImage2D;
		PFNGLTEXPARAMETERIPROC				m_pfnTexParameteri;
		PFNGLGENERATEMIPMAPPROC				m_pfnGenerateMipmap;
		PFNGLACTIVETEXTUREPROC				m_pfnActiveTexture;
		PFNGLCLIENTACTIVETEXTUREPROC		m_pfnClientActiveTexture;
		PFNGLGETTEXIMAGEPROC				m_pfnGetTexImage;
		PFNGLTEXENVIPROC					m_pfnTexEnvi;
		PFNGLTEXENVIVPROC					m_pfnTexEnviv;
		PFNGLTEXENVFPROC					m_pfnTexEnvf;
		PFNGLTEXENVFVPROC					m_pfnTexEnvfv;
		PFNGLBLENDFUNCPROC					m_pfnBlendFunc;
		PFNGLALPHAFUNCPROC					m_pfnAlphaFunc;
		PFNGLISTEXTUREPROC					m_pfnIsTexture;
		PFNGLTEXGENIPROC					m_pfnTexGeni;
		//@}

		/**@name Buffer Objects */
		//@{
		PFNGLENABLECLIENTSTATEPROC			m_pfnEnableClientState;
		PFNGLDISABLECLIENTSTATEPROC			m_pfnDisableClientState;
		PFNGLGENBUFFERSPROC					m_pfnGenBuffers;
		PFNGLBINDBUFFERPROC					m_pfnBindBuffer;
		PFNGLDELETEBUFFERSPROC				m_pfnDeleteBuffers;
		PFNGLISBUFFERPROC					m_pfnIsBuffer;
		PFNGLBUFFERDATAPROC					m_pfnBufferData;
		PFNGLBUFFERSUBDATAARBPROC			m_pfnBufferSubData;
		PFNGLGETBUFFERPARAMETERIVPROC		m_pfnGetBufferParameteriv;
		PFNGLMAPBUFFERPROC					m_pfnMapBuffer;
		PFNGLUNMAPBUFFERPROC				m_pfnUnmapBuffer;
		PFNGLVERTEXPOINTERPROC				m_pfnVertexPointer;
		PFNGLNORMALPOINTERPROC				m_pfnNormalPointer;
		PFNGLTANGENTPOINTERPROC				m_pfnTangentPointer;
		PFNGLCOLORPOINTERPROC				m_pfnColorPointer;
		PFNGLTEXCOORDPOINTERPROC			m_pfnTexCoordPointer;
		//@}

		/**@name FBO */
		//@{
		PFNGLGENFRAMEBUFFERSPROC			m_pfnGenFramebuffers;
		PFNGLDELETEFRAMEBUFFERSPROC			m_pfnDeleteFramebuffers;
		PFNGLBINDFRAMEBUFFERPROC			m_pfnBindFramebuffer;
		PFNGLFRAMEBUFFERTEXTURE2DPROC		m_pfnFramebufferTexture2D;
		PFNGLCHECKFRAMEBUFFERSTATUSPROC		m_pfnCheckFramebufferStatus;
		//@}

		/**@name Uniform variables */
		//@{
		PFNGLGETUNIFORMLOCATIONPROC			m_pfnGetUniformLocation;
		PFNGLUNIFORM1IPROC					m_pfnUniform1i;
		PFNGLUNIFORM2IPROC					m_pfnUniform2i;
		PFNGLUNIFORM3IPROC					m_pfnUniform3i;
		PFNGLUNIFORM4IPROC					m_pfnUniform4i;
		PFNGLUNIFORM1IVPROC					m_pfnUniform1iv;
		PFNGLUNIFORM2IVPROC					m_pfnUniform2iv;
		PFNGLUNIFORM3IVPROC					m_pfnUniform3iv;
		PFNGLUNIFORM4IVPROC					m_pfnUniform4iv;
		PFNGLUNIFORM1UIPROC					m_pfnUniform1ui;
		PFNGLUNIFORM2UIPROC					m_pfnUniform2ui;
		PFNGLUNIFORM3UIPROC					m_pfnUniform3ui;
		PFNGLUNIFORM4UIPROC					m_pfnUniform4ui;
		PFNGLUNIFORM1UIVPROC				m_pfnUniform1uiv;
		PFNGLUNIFORM2UIVPROC				m_pfnUniform2uiv;
		PFNGLUNIFORM3UIVPROC				m_pfnUniform3uiv;
		PFNGLUNIFORM4UIVPROC				m_pfnUniform4uiv;
		PFNGLUNIFORM1FPROC					m_pfnUniform1f;
		PFNGLUNIFORM2FPROC					m_pfnUniform2f;
		PFNGLUNIFORM3FPROC					m_pfnUniform3f;
		PFNGLUNIFORM4FPROC					m_pfnUniform4f;
		PFNGLUNIFORM1FVPROC					m_pfnUniform1fv;
		PFNGLUNIFORM2FVPROC					m_pfnUniform2fv;
		PFNGLUNIFORM3FVPROC					m_pfnUniform3fv;
		PFNGLUNIFORM4FVPROC					m_pfnUniform4fv;
		PFNGLUNIFORM1DPROC					m_pfnUniform1d;
		PFNGLUNIFORM2DPROC					m_pfnUniform2d;
		PFNGLUNIFORM3DPROC					m_pfnUniform3d;
		PFNGLUNIFORM4DPROC					m_pfnUniform4d;
		PFNGLUNIFORM1DVPROC					m_pfnUniform1dv;
		PFNGLUNIFORM2DVPROC					m_pfnUniform2dv;
		PFNGLUNIFORM3DVPROC					m_pfnUniform3dv;
		PFNGLUNIFORM4DVPROC					m_pfnUniform4dv;
		PFNGLUNIFORMMATRIX2FVPROC			m_pfnUniformMatrix2fv;
		PFNGLUNIFORMMATRIX2X3FVPROC			m_pfnUniformMatrix2x3fv;
		PFNGLUNIFORMMATRIX2X4FVPROC			m_pfnUniformMatrix2x4fv;
		PFNGLUNIFORMMATRIX3FVPROC			m_pfnUniformMatrix3fv;
		PFNGLUNIFORMMATRIX3X2FVPROC			m_pfnUniformMatrix3x2fv;
		PFNGLUNIFORMMATRIX3X4FVPROC			m_pfnUniformMatrix3x4fv;
		PFNGLUNIFORMMATRIX4FVPROC			m_pfnUniformMatrix4fv;
		PFNGLUNIFORMMATRIX4X2FVPROC			m_pfnUniformMatrix4x2fv;
		PFNGLUNIFORMMATRIX4X3FVPROC			m_pfnUniformMatrix4x3fv;
		PFNGLUNIFORMMATRIX2DVPROC			m_pfnUniformMatrix2dv;
		PFNGLUNIFORMMATRIX2X3DVPROC			m_pfnUniformMatrix2x3dv;
		PFNGLUNIFORMMATRIX2X4DVPROC			m_pfnUniformMatrix2x4dv;
		PFNGLUNIFORMMATRIX3DVPROC			m_pfnUniformMatrix3dv;
		PFNGLUNIFORMMATRIX3X2DVPROC			m_pfnUniformMatrix3x2dv;
		PFNGLUNIFORMMATRIX3X4DVPROC			m_pfnUniformMatrix3x4dv;
		PFNGLUNIFORMMATRIX4DVPROC			m_pfnUniformMatrix4dv;
		PFNGLUNIFORMMATRIX4X2DVPROC			m_pfnUniformMatrix4x2dv;
		PFNGLUNIFORMMATRIX4X3DVPROC			m_pfnUniformMatrix4x3dv;
		PFNGLGETUNIFORMBLOCKINDEXPROC		m_pfnGetUniformBlockIndex;
		PFNGLBINDBUFFERBASEPROC				m_pfnBindBufferBase;
		PFNGLUNIFORMBLOCKBINDINGPROC		m_pfnUniformBlockBinding;
		PFNGLGETUNIFORMINDICESPROC			m_pfnGetUniformIndices;
		PFNGLGETACTIVEUNIFORMSIVPROC		m_pfnGetActiveUniformsiv;
		PFNGLGETACTIVEUNIFORMBLOCKIVPROC	m_pfnGetActiveUniformBlockiv;
		//@}

		/**@name Shader object */
		//@{
		PFNGLCREATESHADERPROC				m_pfnCreateShader;
		PFNGLDELETESHADERPROC				m_pfnDeleteShader;
		PFNGLATTACHSHADERPROC				m_pfnAttachShader;
		PFNGLDETACHSHADERPROC				m_pfnDetachShader;
		PFNGLCOMPILESHADERPROC				m_pfnCompileShader;
		PFNGLGETSHADERIVPROC				m_pfnGetShaderiv;
		PFNGLGETSHADERINFOLOGPROC			m_pfnGetShaderInfoLog;
		PFNGLSHADERSOURCEPROC				m_pfnShaderSource;
		//@}

		/**@name Shader program */
		//@{
		PFNGLCREATEPROGRAMPROC				m_pfnCreateProgram;
		PFNGLDELETEPROGRAMPROC				m_pfnDeleteProgram;
		PFNGLLINKPROGRAMPROC				m_pfnLinkProgram;
		PFNGLUSEPROGRAMPROC					m_pfnUseProgram;
		PFNGLGETPROGRAMIVPROC				m_pfnGetProgramiv;
		PFNGLGETPROGRAMINFOLOGPROC			m_pfnGetProgramInfoLog;
		PFNGLGETATTRIBLOCATIONPROC			m_pfnGetAttribLocation;
		PFNGLISPROGRAMPROC					m_pfnIsProgram;
		//@}

		/**@name Vertex Attribute Pointer */
		//@{
		PFNGLENABLEVERTEXATTRIBARRAYPROC	m_pfnEnableVertexAttribArray;
		PFNGLVERTEXATTRIBPOINTERPROC		m_pfnVertexAttribPointer;
		PFNGLDISABLEVERTEXATTRIBARRAYPROC	m_pfnDisableVertexAttribArray;
		//@}

		/**@name Vertex Array Object */
		//@{
		PFNGLGENVERTEXARRAYSPROC			m_pfnGenVertexArrays;
		PFNGLDELETEVERTEXARRAYSPROC			m_pfnDeleteVertexArrays;
		PFNGLBINDVERTEXARRAYPROC			m_pfnBindVertexArray;
		//@}

		//! Private static instance, initialisable only one time
		static OpenGl * m_instance;

		/**@name Construction / Destruction => Private so only available through Initialise and Cleanup */
		//@{
		OpenGl();
		~OpenGl();
		//@}

	public:
		static void		Initialise	();
		static void		Cleanup		();

		/**@name General Functions */
		//@{
		static GLenum	GetError				();
		static bool		ClearColor				( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
		static bool		ShadeModel				( GLenum mode);
		static bool		Clear					( GLbitfield mask);
		static bool		DrawElements			( GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);
		static bool		Enable					( GLenum mode);
		static bool		Disable					( GLenum mode);
		static bool		Color3f					( GLfloat r, GLfloat g, GLfloat b);
		static bool		Color3fv				( const GLfloat * rgb);
		static bool		Color4f					( GLfloat r, GLfloat g, GLfloat b, GLfloat a);
		static bool		Color4fv				( const GLfloat * rgba);
		static bool		SelectBuffer			( GLsizei size, GLuint * buffer);
		static bool		GetIntegerv				( GLenum pname, GLint * params);
		static GLint	RenderMode				( GLenum mode);
		//@}

		/**@name Light Functions */
		//@{
		static bool		Lightf					( GLuint light, GLenum pname, GLfloat param);
		static bool		Lightfv					( GLuint light, GLenum pname, const GLfloat * param);
		static bool		LightModeli				( GLenum pname, GLint param);
		static bool		LightModeliv			( GLenum pname, const GLint * param);
		static bool		LightModelf				( GLenum pname, GLfloat param);
		static bool		LightModelfv			( GLenum pname, const GLfloat * param);
		//@}

		/**@name Context functions */
		//@{
#ifdef _WIN32
		static bool		MakeCurrent				( HDC hdc, HGLRC hglrc);
		static bool		SwapBuffers				( HDC hdc);
		static HGLRC	CreateContext			( HDC hdc);
		static HGLRC	CreateContextAttribs	( HDC hDC, HGLRC hShareContext, int const * attribList);
		static bool		DeleteContext			( HGLRC hContext);
		static bool		HasCreateContextAttribs	();
#else
#	if CASTOR_GTK
		static bool		MakeCurrent				( Display * pDisplay, GLXDrawable drawable, GLXContext context);
		static bool		SwapBuffers				( Display * pDisplay, GLXDrawable drawable);
		static HGLRC	CreateContext			( Display * pDisplay, XVisualInfo * pVisualInfo, GLXContext shareList, Bool direct);
		static HGLRC	CreateContextAttribs	( Display * pDisplay, GLXFBConfig fbconfig, GLXContext shareList, Bool direct, int const * attribList);
		static bool		DeleteContext			( Display * pDisplay, GLXContext context);
		static bool		HasCreateContextAttribs	();
#	endif
#endif
		//@}

		/**@name Matrix functions */
		//@{
		static bool		Viewport				( GLint x, GLint y, GLsizei width, GLsizei height);
		static bool		MatrixMode				( GLenum mode);
		static bool		LoadIdentity			();
		static bool		Ortho					( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
		static bool		Frustum					( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
		static bool		PushMatrix				();
		static bool		PopMatrix				();
		static bool		Translate				( real x, real y, real z);
		static bool		Rotate					( real angle, real x, real y, real z);
		static bool		Scale					( real x, real y, real z);
		static bool		MultMatrix				( real const * matrix);
		//@}

		/**@name Material functions */
		//@{
		static bool		CullFace				( GLenum face);
		static bool		FrontFace				( GLenum face);
		static bool		Materialf				( GLenum face, GLenum pname, GLfloat param);
		static bool		Materialfv				( GLenum face, GLenum pname, const GLfloat * param);
		//@}

		/**@name Texture functions */
		//@{
		static bool		GenTextures				( GLsizei n, GLuint * textures);
		static bool		DeleteTextures			( GLsizei n, const GLuint * textures);
		static bool		GenerateMipmap			( GLenum target);
		static bool		ActiveTexture			( GLint texture);
		static bool		ClientActiveTexture		( GLenum texture);
		static bool		BindTexture				( GLenum mode, GLuint texture);
		static bool		TexSubImage2D			( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid * data);
		static bool		TexImage2D				( GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid * data);
		static bool		TexParameteri			( GLenum target, GLenum pname, GLint param);
		static bool		GetTexImage				( GLenum target, GLint level, GLenum format, GLenum type, GLvoid * img);
		static bool		TexEnvi					( GLenum target, GLenum pname, GLint param);
		static bool		TexEnviv				( GLenum target, GLenum pname, const GLint * param);
		static bool		TexEnvf					( GLenum target, GLenum pname, GLfloat param);
		static bool		TexEnvfv				( GLenum target, GLenum pname, const GLfloat * param);
		static bool		BlendFunc				( GLenum sfactor, GLenum dfactor);
		static bool		AlphaFunc				( GLenum func, GLclampf ref);
		static bool		IsTexture				( GLuint texture);
		static bool		TexGeni					( GLenum coord, GLenum pname, GLint param);
		//@}

		/**@name Buffer objects functions */
		//@{
		static bool		EnableClientState		( GLenum array);
		static bool		DisableClientState		( GLenum array);
		static bool		GenBuffers				( GLsizei n, GLuint* buffers);
		static bool		BindBuffer				( GLenum target, GLuint buffer);
		static bool		DeleteBuffers			( GLsizei n, const GLuint* buffers);
		static bool		IsBuffer				( GLuint buffer);
		static bool		BufferData				( GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage);
		static bool		BufferSubData			( GLenum target, GLintptrARB offset, GLsizeiptrARB size, const GLvoid * data);
		static GLvoid *	MapBuffer				( GLenum target, GLenum access);
		static bool		UnmapBuffer				( GLenum target);
		static bool		GetBufferParameteriv	( GLenum target, GLenum pname, GLint * params);
		static bool		VertexPointer			( GLint size, GLenum type, GLsizei stride, const GLvoid * pointer);
		static bool		NormalPointer			( GLenum type, GLsizei stride, const GLvoid * pointer);
		static bool		TangentPointer			( GLenum type, GLsizei stride, const GLvoid * pointer);
		static bool		HasTangentPointer		();
		static bool		ColorPointer			( GLint size, GLenum type, GLsizei stride, const GLvoid * pointer);
		static bool		TexCoordPointer			( GLint size, GLenum type, GLsizei stride, const GLvoid * pointer);
		//@}

		/**@name FBO functions */
		//@{
		static bool		GenFramebuffers			( GLsizei n, GLuint* framebuffers);
		static bool		DeleteFramebuffers		( GLsizei n, const GLuint* framebuffers);
		static bool		BindFramebuffer			( GLenum target, GLuint framebuffer);
		static bool		FramebufferTexture2D	( GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
		static GLenum	CheckFramebufferStatus	( GLenum target);
		//@}

		/**@name Uniform variable Functions */
		//@{
		static GLint	GetUniformLocation		( GLuint program, const GLchar* name);
		static bool		Uniform1i				( GLint location, GLint v0);
		static bool		Uniform2i				( GLint location, GLint v0, GLint v1);
		static bool		Uniform3i				( GLint location, GLint v0, GLint v1, GLint v2);
		static bool		Uniform4i				( GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
		static bool		Uniform1iv				( GLint location, GLsizei count, const GLint * params);
		static bool		Uniform2iv				( GLint location, GLsizei count, const GLint * params);
		static bool		Uniform3iv				( GLint location, GLsizei count, const GLint * params);
		static bool		Uniform4iv				( GLint location, GLsizei count, const GLint * params);
		static bool		Uniform1ui				( GLint location, GLuint v0);
		static bool		Uniform2ui				( GLint location, GLuint v0, GLuint v1);
		static bool		Uniform3ui				( GLint location, GLuint v0, GLuint v1, GLuint v2);
		static bool		Uniform4ui				( GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
		static bool		Uniform1uiv				( GLint location, GLsizei count, const GLuint * params);
		static bool		Uniform2uiv				( GLint location, GLsizei count, const GLuint * params);
		static bool		Uniform3uiv				( GLint location, GLsizei count, const GLuint * params);
		static bool		Uniform4uiv				( GLint location, GLsizei count, const GLuint * params);
		static bool		Uniform1f				( GLint location, GLfloat v0);
		static bool		Uniform2f				( GLint location, GLfloat v0, GLfloat v1);
		static bool		Uniform3f				( GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
		static bool		Uniform4f				( GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
		static bool		Uniform1fv				( GLint location, GLsizei count, const GLfloat * params);
		static bool		Uniform2fv				( GLint location, GLsizei count, const GLfloat * params);
		static bool		Uniform3fv				( GLint location, GLsizei count, const GLfloat * params);
		static bool		Uniform4fv				( GLint location, GLsizei count, const GLfloat * params);
		static bool		Uniform1d				( GLint location, GLdouble v0);
		static bool		Uniform2d				( GLint location, GLdouble v0, GLdouble v1);
		static bool		Uniform3d				( GLint location, GLdouble v0, GLdouble v1, GLdouble v2);
		static bool		Uniform4d				( GLint location, GLdouble v0, GLdouble v1, GLdouble v2, GLdouble v3);
		static bool		Uniform1dv				( GLint location, GLsizei count, const GLdouble * params);
		static bool		Uniform2dv				( GLint location, GLsizei count, const GLdouble * params);
		static bool		Uniform3dv				( GLint location, GLsizei count, const GLdouble * params);
		static bool		Uniform4dv				( GLint location, GLsizei count, const GLdouble * params);
		static bool		UniformMatrix2fv		( GLint location, GLsizei count, GLboolean transpose, const GLfloat * value);
		static bool		UniformMatrix2x3fv		( GLint location, GLsizei count, GLboolean transpose, const GLfloat * value);
		static bool		UniformMatrix2x4fv		( GLint location, GLsizei count, GLboolean transpose, const GLfloat * value);
		static bool		UniformMatrix3fv		( GLint location, GLsizei count, GLboolean transpose, const GLfloat * value);
		static bool		UniformMatrix3x2fv		( GLint location, GLsizei count, GLboolean transpose, const GLfloat * value);
		static bool		UniformMatrix3x4fv		( GLint location, GLsizei count, GLboolean transpose, const GLfloat * value);
		static bool		UniformMatrix4fv		( GLint location, GLsizei count, GLboolean transpose, const GLfloat * value);
		static bool		UniformMatrix4x2fv		( GLint location, GLsizei count, GLboolean transpose, const GLfloat * value);
		static bool		UniformMatrix4x3fv		( GLint location, GLsizei count, GLboolean transpose, const GLfloat * value);
		static bool		UniformMatrix2dv		( GLint location, GLsizei count, GLboolean transpose, const GLdouble * value);
		static bool		UniformMatrix2x3dv		( GLint location, GLsizei count, GLboolean transpose, const GLdouble * value);
		static bool		UniformMatrix2x4dv		( GLint location, GLsizei count, GLboolean transpose, const GLdouble * value);
		static bool		UniformMatrix3dv		( GLint location, GLsizei count, GLboolean transpose, const GLdouble * value);
		static bool		UniformMatrix3x2dv		( GLint location, GLsizei count, GLboolean transpose, const GLdouble * value);
		static bool		UniformMatrix3x4dv		( GLint location, GLsizei count, GLboolean transpose, const GLdouble * value);
		static bool		UniformMatrix4dv		( GLint location, GLsizei count, GLboolean transpose, const GLdouble * value);
		static bool		UniformMatrix4x2dv		( GLint location, GLsizei count, GLboolean transpose, const GLdouble * value);
		static bool		UniformMatrix4x3dv		( GLint location, GLsizei count, GLboolean transpose, const GLdouble * value);
		static GLuint	GetUniformBlockIndex	( GLuint program, const char* uniformBlockName);
		static bool		BindBufferBase			( GLenum target, GLuint index, GLuint buffer);
		static bool		UniformBlockBinding		( GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding);
		static bool		GetUniformIndices		( GLuint program, GLsizei uniformCount, const char** uniformNames, GLuint* uniformIndices);
		static bool		GetActiveUniformsiv		( GLuint program, GLsizei uniformCount, const GLuint* uniformIndices, GLenum pname, GLint* params);
		static bool		GetActiveUniformBlockiv	( GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint* params);
		//@}

		/**@name Shader object Functions */
		//@{
		static GLuint	CreateShader			( GLenum type);
		static bool		DeleteShader			( GLuint shader);
		static bool		AttachShader			( GLuint program, GLuint shader);
		static bool		DetachShader			( GLuint program, GLuint shader);
		static bool		CompileShader			( GLuint shader);
		static bool		GetShaderiv				( GLuint shader, GLenum pname, GLint* param);
		static bool		GetShaderInfoLog		( GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
		static bool		ShaderSource			( GLuint shader, GLsizei count, const GLchar** strings, const GLint* lengths);
		//@}

		/**@name Shader program Functions */
		//@{
		static GLuint	CreateProgram			();
		static bool		DeleteProgram			( GLuint program);
		static bool		LinkProgram				( GLuint program);
		static bool		UseProgram				( GLuint program);
		static bool		GetProgramiv			( GLuint program, GLenum pname, GLint* param);
		static bool		GetProgramInfoLog		( GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
		static GLint	GetAttribLocation		( GLuint program, const GLchar* name);
		static bool		IsProgram				( GLuint program);
		//@}

		/**@name Vertex Attribute Pointer functions */
		//@{
		static bool		EnableVertexAttribArray	( GLuint index);
		static bool		VertexAttribPointer		( GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid * pointer);
		static bool		DisableVertexAttribArray( GLuint index);
		//@}

		/**@name Vertex Array Objects */
		//@{
		static bool		GenVertexArrays			( GLsizei n, GLuint * arrays);
		static bool		BindVertexArray			( GLuint array);
		static bool		DeleteVertexArrays		( GLsizei n, GLuint * arrays);
		//@}
	};
}

#endif