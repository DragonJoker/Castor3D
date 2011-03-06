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
#ifndef ___Gl_ModuleRender___
#define ___Gl_ModuleRender___

#include <Castor3D/material/TextureUnit.h>

namespace Castor3D
{
	class RenderWindow;
	class Geometry;

	template <typename T, size_t Count> class GlVertexAttribs;
	template <typename T> class GlVertexBufferObject;
	template <typename T, size_t Count> class GlVertexAttribsBuffer;
	class GlBufferBase;
	class GlAttribsBase;
	class GlVertexBuffer;
	class GlIndexBuffer;
	class GlRenderSystem;
	class GlPlugin;
	class GlSubmeshRenderer;
	class GlMeshRenderer;
	class GlGeometryRenderer;
	class GlTextureEnvironmentRenderer;
	class GlTextureRenderer;
	class GlMaterialRenderer;
	class GlLightRenderer;
	class GlCameraRenderer;
	class GlWindowRenderer;
	class GlOverlayRenderer;
	class GlShaderObject;
	class GlShaderProgram;
	class GlVertexShader;
	class GlFragmentShader;
	class GlGeometryShader;
	class GlFrameVariableBase;
	class GlIntFrameVariable;
	class GlRealFrameVariable;
	template <typename T> class GlOneFrameVariable;
	template <typename T, size_t Count> class GlPointFrameVariable;
	template <typename T, size_t Rows, size_t Columns> class GlMatrixFrameVariable;
	class CgGlShaderProgram;
	class CgGlVertexShader;
	class CgGlFragmentShader;
	class CgGlGeometryShader;
	class CgGlFrameVariableBase;
	class CgGlIntFrameVariable;
	class CgGlRealFrameVariable;
	template <typename T> class CgGlOneFrameVariable;
	template <typename T, size_t Count> class CgGlPointFrameVariable;
	template <typename T, size_t Rows, size_t Columns> class CgGlMatrixFrameVariable;
	class GlPipeline;
	class GlContext;

	typedef GlVertexAttribs<real, 3>		GlVertexAttribs3r;
	typedef GlVertexAttribs<real, 2>		GlVertexAttribs2r;
	typedef GlVertexAttribs<int, 3>			GlVertexAttribs3i;
	typedef GlVertexAttribs<int, 2>			GlVertexAttribs2i;
	typedef GlVertexAttribsBuffer<real, 3>	GlVertexAttribsBuffer3r;
	typedef GlVertexAttribsBuffer<real, 2>	GlVertexAttribsBuffer2r;
	typedef GlVertexAttribsBuffer<int, 3>	GlVertexAttribsBuffer3i;
	typedef GlVertexAttribsBuffer<int, 2>	GlVertexAttribsBuffer2i;

	typedef shared_ptr<	GlAttribsBase			>	GlVertexAttribsPtr;
	typedef shared_ptr<	GlVertexBuffer			>	GlVertexBufferPtr;
	typedef shared_ptr<	GlIndexBuffer			>	GlIndexBufferPtr;
	typedef shared_ptr<	GlVertexAttribs3r		>	GlVertexAttribs3rPtr;
	typedef shared_ptr<	GlVertexAttribs2r		>	GlVertexAttribs2rPtr;
	typedef shared_ptr<	GlVertexAttribs3i		>	GlVertexAttribs3iPtr;
	typedef shared_ptr<	GlVertexAttribs2i		>	GlVertexAttribs2iPtr;
	typedef shared_ptr<	GlVertexAttribsBuffer3r	>	GlVertexAttribsBuffer3rPtr;
	typedef shared_ptr<	GlVertexAttribsBuffer2r	>	GlVertexAttribsBuffer2rPtr;
	typedef shared_ptr<	GlVertexAttribsBuffer3i	>	GlVertexAttribsBuffer3iPtr;
	typedef shared_ptr<	GlVertexAttribsBuffer2i	>	GlVertexAttribsBuffer2iPtr;
	typedef shared_ptr<	GlShaderObject			>	GlShaderObjectPtr;
	typedef shared_ptr<	GlShaderProgram			>	GlShaderProgramPtr;
	typedef shared_ptr<	GlVertexShader			>	GlVertexShaderPtr;
	typedef shared_ptr<	GlFragmentShader		>	GlFragmentShaderPtr;
	typedef shared_ptr<	GlGeometryShader		>	GlGeometryShaderPtr;
	typedef shared_ptr<	GlFrameVariableBase		>	GlFrameVariablePtr;
	typedef shared_ptr<	CgGlShaderProgram		>	CgGlShaderProgramPtr;
	typedef shared_ptr<	CgGlVertexShader		>	CgGlVertexShaderPtr;
	typedef shared_ptr<	CgGlFragmentShader		>	CgGlFragmentShaderPtr;
	typedef shared_ptr<	CgGlGeometryShader		>	CgGlGeometryShaderPtr;
	typedef shared_ptr<	CgGlFrameVariableBase	>	CgGlFrameVariablePtr;

	typedef Container<		GlShaderProgramPtr							>::Vector	GlShaderProgramPtrArray;
	typedef Container<		CgGlShaderProgramPtr						>::Vector	CgGlShaderProgramPtrArray;
	typedef Container<		GlShaderObjectPtr							>::Vector	GlShaderObjectPtrArray;
	typedef KeyedContainer<	String,				GlFrameVariablePtr		>::Map		GlFrameVariablePtrStrMap;
	typedef KeyedContainer<	String,				CgGlFrameVariablePtr	>::Map		CgGlFrameVariablePtrStrMap;

	class GlEnum
	{
	public:
		struct GlPixelFmt
		{
			GLenum Format;
			GLenum Internal;
			GLenum Type;
		};

	private:
		static String		GlslStrings				[];
		static String		GlslErrors				[];
		static GLenum		DrawTypes				[eNbDrawTypes];
		static GLenum		EnvironmentModes		[TextureEnvironment::eNbEnvModes];
		static GLenum 		RgbCombinations			[TextureEnvironment::eNbRgbCombinations];
		static GLenum 		RgbOperands				[TextureEnvironment::eNbRgbOperands];
		static GLenum 		AlphaCombinations		[TextureEnvironment::eNbAlphaCombinations];
		static GLenum 		AlphaOperands			[TextureEnvironment::eNbAlphaOperands];
		static GLenum 		CombinationSources		[TextureEnvironment::eNbCombinationSources];
		static GLenum 		TextureDimensions		[TextureUnit::eNbTextureDimensions];
		static GLenum		AlphaFuncs				[TextureUnit::eNbAlphaFuncs];
		static GLenum		TextureWrapMode			[TextureUnit::eNbWrapModes];
		static GLenum		TextureInterpolation	[TextureUnit::eNbInterpolationModes];
		static GLenum 		LightIndexes			[8];
		static GLenum		SrcBlendFactors			[Pass::eNbSrcBlendFactors];
		static GLenum		DstBlendFactors			[Pass::eNbDstBlendFactors];
		static GLenum		Usages					[eNbUsages];
		static GLenum 		TextureArguments		[TextureUnit::eNbTextureArguments];
		static GLenum 		TextureRgbModes			[TextureUnit::eNbTextureRgbModes];
		static GLenum 		TextureAlphaModes		[TextureUnit::eNbTextureAlphaModes];
		static GlPixelFmt	PixelFormats			[eNbPixelFormats];

	public:
		static void			GlCheckError		( const String & p_strText, bool p_bThrows);
		static void			CgCheckError		( const String & p_strText, bool p_bThrows);
		static String		GetGlslErrorString	( int p_index);
		static GLenum		Get					( ePRIMITIVE_TYPE p_index);
		static GLenum 		Get					( TextureEnvironment::eMODE p_index);
		static GLenum 		Get					( TextureEnvironment::eRGB_COMBINATION p_index);
		static GLenum 		Get					( TextureEnvironment::eRGB_OPERAND p_index);
		static GLenum 		Get					( TextureEnvironment::eALPHA_COMBINATION p_index);
		static GLenum 		Get					( TextureEnvironment::eALPHA_OPERAND p_index);
		static GLenum 		Get					( TextureEnvironment::eCOMBINATION_SOURCE p_index);
		static GLenum 		Get					( TextureUnit::eDIMENSION p_index);
		static GLenum		Get					( eLIGHT_INDEXES p_uiIndex);
		static GLenum		Get					( TextureUnit::eALPHA_FUNC p_eAlphaFunc);
		static GLenum		Get					( TextureUnit::eWRAP_MODE p_eWrapMode);
		static GLenum		Get					( TextureUnit::eINTERPOLATION_MODE p_eInterpolationMode);
		static GLenum		Get					( TextureUnit::eTEXTURE_ARGUMENT p_eArgument);
		static GLenum		Get					( TextureUnit::eTEXTURE_RGB_MODE p_eMode);
		static GLenum		Get					( TextureUnit::eTEXTURE_ALPHA_MODE p_eMode);
		static GLenum		Get					( Pass::eSRC_BLEND_FACTOR p_eBlendFactor);
		static GLenum		Get					( Pass::eDST_BLEND_FACTOR p_eBlendFactor);
		static GlPixelFmt	Get					( Castor::Resources::ePIXEL_FORMAT p_pixelFormat);
		static GLenum		Get					( eELEMENT_USAGE p_eUsage);
		static GLenum		GetLockFlags		( size_t p_uiFlags);
		static GLenum 		GetBufferFlags		( size_t p_ulFlags);
	};
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

#   ifdef _WIN32
#	    define glCreateContext				wglCreateContext
#   	define glDeleteContext				wglDeleteContext
#   	define glShareLists					wglShareLists
#   	define glGetCurrentDC				wglGetCurrentDC
#   	define GL_CONTEXT_MAJOR_VERSION		WGL_CONTEXT_MAJOR_VERSION_ARB
#   	define GL_CONTEXT_MINOR_VERSION		WGL_CONTEXT_MINOR_VERSION_ARB
#   	define GL_CONTEXT_FLAGS_ARB			WGL_CONTEXT_FLAGS_ARB
#   	define GL_FORWARD_COMPATIBLE_BIT	WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB
#   	define glCreateContextAttribs		wglCreateContextAttribsARB
#   else
#   	ifdef __GNUG__
#   		undef CS3D_GL_API
#   		define CS3D_GL_API
#   		define glCreateContext				glXCreateNewContext
#   		define glDeleteContext				glXDestroyContext
#   		define GL_CONTEXT_MAJOR_VERSION		GLX_CONTEXT_MAJOR_VERSION_ARB
#   		define GL_CONTEXT_MINOR_VERSION		GLX_CONTEXT_MINOR_VERSION_ARB
#   		define GL_CONTEXT_FLAGS_ARB			GLX_CONTEXT_FLAGS_ARB
#	    	define GL_FORWARD_COMPATIBLE_BIT	GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB
#	    	define glCreateContextAttribs		glXCreateContextAttribsARB
#	    endif
#	endif

#   ifdef _DEBUG
#	    define CheckGlError( func, txt) {func;GlEnum::GlCheckError( txt, false);}
#	    define CheckCgError( func, txt) {func;GlEnum::CgCheckError( txt, false);}
#   else
#	    define CheckGlError( func) func
#	    define CheckCgError( func) func
#   endif

#define CASTOR_GTK 1

#endif
