#include <GlRenderSystem/PrecompiledHeader.hpp>

#include <GlRenderSystem/OpenGl.hpp>
#include <GlRenderSystem/Module_GlRender.hpp>

using namespace Castor3D;

//*************************************************************************************************

OpenGl * OpenGl :: m_instance = NULL;

OpenGl :: OpenGl()
	:	m_pfnGetError					( NULL)
	,	m_pfnClearColor					( NULL)
	,	m_pfnShadeModel					( NULL)
	,	m_pfnViewport					( NULL)
	,	m_pfnMatrixMode					( NULL)
	,	m_pfnLoadIdentity				( NULL)
	,	m_pfnOrtho						( NULL)
	,	m_pfnFrustum					( NULL)
	,	m_pfnClear						( NULL)
	,	m_pfnDrawElements				( NULL)
	,	m_pfnEnable						( NULL)
	,	m_pfnDisable					( NULL)
	,	m_pfnColor3f					( NULL)
	,	m_pfnColor3fv					( NULL)
	,	m_pfnColor4f					( NULL)
	,	m_pfnColor4fv					( NULL)
	,	m_pfnLightf						( NULL)
	,	m_pfnLightfv					( NULL)
	,	m_pfnLightModeli				( NULL)
	,	m_pfnLightModeliv				( NULL)
	,	m_pfnLightModelf				( NULL)
	,	m_pfnLightModelfv				( NULL)
	,	m_pfnGenTextures				( NULL)
	,	m_pfnDeleteTextures				( NULL)
	,	m_pfnBindTexture				( NULL)
	,	m_pfnTexSubImage2D				( NULL)
	,	m_pfnTexImage2D					( NULL)
	,	m_pfnTexParameteri				( NULL)
	,	m_pfnGenerateMipmap				( NULL)
	,	m_pfnActiveTexture				( NULL)
	,	m_pfnClientActiveTexture		( NULL)
	,	m_pfnGetTexImage				( NULL)
	,	m_pfnTexEnvi					( NULL)
	,	m_pfnTexEnviv					( NULL)
	,	m_pfnTexEnvf					( NULL)
	,	m_pfnTexEnvfv					( NULL)
	,	m_pfnBlendFunc					( NULL)
	,	m_pfnAlphaFunc					( NULL)
	,	m_pfnIsTexture					( NULL)
	,	m_pfnEnableClientState			( NULL)
	,	m_pfnDisableClientState			( NULL)
	,	m_pfnGenBuffers					( NULL)
	,	m_pfnBindBuffer					( NULL)
	,	m_pfnDeleteBuffers				( NULL)
	,	m_pfnIsBuffer					( NULL)
	,	m_pfnBufferData					( NULL)
	,	m_pfnBufferSubData				( NULL)
	,	m_pfnGetBufferParameteriv		( NULL)
	,	m_pfnMapBuffer					( NULL)
	,	m_pfnUnmapBuffer				( NULL)
	,	m_pfnVertexPointer				( NULL)
	,	m_pfnNormalPointer				( NULL)
	,	m_pfnTangentPointer				( NULL)
	,	m_pfnColorPointer				( NULL)
	,	m_pfnTexCoordPointer			( NULL)
	,	m_pfnGenFramebuffers			( NULL)
	,	m_pfnDeleteFramebuffers			( NULL)
	,	m_pfnBindFramebuffer			( NULL)
	,	m_pfnFramebufferTexture2D		( NULL)
	,	m_pfnCheckFramebufferStatus		( NULL)
	,	m_pfnGetUniformLocation			( NULL)
	,	m_pfnUniform1i					( NULL)
	,	m_pfnUniform2i					( NULL)
	,	m_pfnUniform3i					( NULL)
	,	m_pfnUniform4i					( NULL)
	,	m_pfnUniform1iv					( NULL)
	,	m_pfnUniform2iv					( NULL)
	,	m_pfnUniform3iv					( NULL)
	,	m_pfnUniform4iv					( NULL)
	,	m_pfnUniform1ui					( NULL)
	,	m_pfnUniform2ui					( NULL)
	,	m_pfnUniform3ui					( NULL)
	,	m_pfnUniform4ui					( NULL)
	,	m_pfnUniform1uiv				( NULL)
	,	m_pfnUniform2uiv				( NULL)
	,	m_pfnUniform3uiv				( NULL)
	,	m_pfnUniform4uiv				( NULL)
	,	m_pfnUniform1f					( NULL)
	,	m_pfnUniform2f					( NULL)
	,	m_pfnUniform3f					( NULL)
	,	m_pfnUniform4f					( NULL)
	,	m_pfnUniform1fv					( NULL)
	,	m_pfnUniform2fv					( NULL)
	,	m_pfnUniform3fv					( NULL)
	,	m_pfnUniform4fv					( NULL)
	,	m_pfnUniform1d					( NULL)
	,	m_pfnUniform2d					( NULL)
	,	m_pfnUniform3d					( NULL)
	,	m_pfnUniform4d					( NULL)
	,	m_pfnUniform1dv					( NULL)
	,	m_pfnUniform2dv					( NULL)
	,	m_pfnUniform3dv					( NULL)
	,	m_pfnUniform4dv					( NULL)
	,	m_pfnUniformMatrix2fv			( NULL)
	,	m_pfnUniformMatrix2x3fv			( NULL)
	,	m_pfnUniformMatrix2x4fv			( NULL)
	,	m_pfnUniformMatrix3fv			( NULL)
	,	m_pfnUniformMatrix3x2fv			( NULL)
	,	m_pfnUniformMatrix3x4fv			( NULL)
	,	m_pfnUniformMatrix4fv			( NULL)
	,	m_pfnUniformMatrix4x2fv			( NULL)
	,	m_pfnUniformMatrix4x3fv			( NULL)
	,	m_pfnUniformMatrix2dv			( NULL)
	,	m_pfnUniformMatrix2x3dv			( NULL)
	,	m_pfnUniformMatrix2x4dv			( NULL)
	,	m_pfnUniformMatrix3dv			( NULL)
	,	m_pfnUniformMatrix3x2dv			( NULL)
	,	m_pfnUniformMatrix3x4dv			( NULL)
	,	m_pfnUniformMatrix4dv			( NULL)
	,	m_pfnUniformMatrix4x2dv			( NULL)
	,	m_pfnUniformMatrix4x3dv			( NULL)
	,	m_pfnCreateShader				( NULL)
	,	m_pfnDeleteShader				( NULL)
	,	m_pfnAttachShader				( NULL)
	,	m_pfnDetachShader				( NULL)
	,	m_pfnCompileShader				( NULL)
	,	m_pfnGetShaderiv				( NULL)
	,	m_pfnGetShaderInfoLog			( NULL)
	,	m_pfnShaderSource				( NULL)
	,	m_pfnCreateProgram				( NULL)
	,	m_pfnDeleteProgram				( NULL)
	,	m_pfnLinkProgram				( NULL)
	,	m_pfnUseProgram					( NULL)
	,	m_pfnGetProgramiv				( NULL)
	,	m_pfnGetProgramInfoLog			( NULL)
	,	m_pfnGetAttribLocation			( NULL)
	,	m_pfnIsProgram					( NULL)
	,	m_pfnTexGeni					( NULL)
	,	m_pfnPushMatrix					( NULL)
	,	m_pfnPopMatrix					( NULL)
	,	m_pfnTranslate					( NULL)
	,	m_pfnRotate						( NULL)
	,	m_pfnScale						( NULL)
	,	m_pfnMultMatrix					( NULL)
	,	m_pfnMakeCurrent				( NULL)
	,	m_pfnSwapBuffers				( NULL)
	,	m_pfnCreateContext				( NULL)
	,	m_pfnCreateContextAttribs		( NULL)
	,	m_pfnDeleteContext				( NULL)
	,	m_pfnCullFace					( NULL)
	,	m_pfnFrontFace					( NULL)
	,	m_pfnMaterialf					( NULL)
	,	m_pfnMaterialfv					( NULL)
	,	m_pfnSelectBuffer				( NULL)
	,	m_pfnGetIntegerv				( NULL)
	,	m_pfnRenderMode					( NULL)
	,	m_pfnEnableVertexAttribArray	( NULL)
	,	m_pfnVertexAttribPointer		( NULL)
	,	m_pfnBindVertexArray			( NULL)
	,	m_pfnDisableVertexAttribArray	( NULL)
	,	m_pfnGetUniformBlockIndex		( NULL)
	,	m_pfnBindBufferBase				( NULL)
	,	m_pfnUniformBlockBinding		( NULL)
	,	m_pfnGetUniformIndices			( NULL)
	,	m_pfnGetActiveUniformsiv		( NULL)
	,	m_pfnGenVertexArrays			( NULL)
	,	m_pfnDeleteVertexArrays			( NULL)
	,	m_pfnGetActiveUniformBlockiv	( NULL)
{
}

OpenGl :: ~OpenGl()
{
}

void OpenGl :: Initialise()
{
	if (m_instance == NULL)
	{
		m_instance = new OpenGl();

		glewInit();

		if (glGenerateMipmap != NULL)
		{
			m_instance->m_pfnGenerateMipmap			= glGenerateMipmap			;
		}
		else
		{
			m_instance->m_pfnGenerateMipmap			= glGenerateMipmapEXT		;
		}

		m_instance->m_pfnGetError					= & glGetError				;
		m_instance->m_pfnActiveTexture				= glActiveTexture			;
		m_instance->m_pfnClientActiveTexture		= glClientActiveTexture		;
		m_instance->m_pfnGenBuffers					= glGenBuffers				;
		m_instance->m_pfnBindBuffer					= glBindBuffer				;
		m_instance->m_pfnDeleteBuffers				= glDeleteBuffers			;
		m_instance->m_pfnIsBuffer					= glIsBuffer				;
		m_instance->m_pfnBufferData					= glBufferData				;
		m_instance->m_pfnBufferSubData				= glBufferSubData			;
		m_instance->m_pfnGetBufferParameteriv		= glGetBufferParameteriv	;
		m_instance->m_pfnMapBuffer					= glMapBuffer				;
		m_instance->m_pfnUnmapBuffer				= glUnmapBuffer				;
		m_instance->m_pfnGenFramebuffers			= glGenFramebuffers			;
		m_instance->m_pfnDeleteFramebuffers			= glDeleteFramebuffers		;
		m_instance->m_pfnBindFramebuffer			= glBindFramebuffer			;
		m_instance->m_pfnFramebufferTexture2D		= glFramebufferTexture2D	;
		m_instance->m_pfnCheckFramebufferStatus		= glCheckFramebufferStatus	;
		m_instance->m_pfnGetUniformLocation			= glGetUniformLocation		;
		m_instance->m_pfnUniform1i					= glUniform1i				;
		m_instance->m_pfnUniform2i					= glUniform2i				;
		m_instance->m_pfnUniform3i					= glUniform3i				;
		m_instance->m_pfnUniform4i					= glUniform4i				;
		m_instance->m_pfnUniform1iv					= glUniform1iv				;
		m_instance->m_pfnUniform2iv					= glUniform2iv				;
		m_instance->m_pfnUniform3iv					= glUniform3iv				;
		m_instance->m_pfnUniform4iv					= glUniform4iv				;
		m_instance->m_pfnUniform1ui					= glUniform1ui				;
		m_instance->m_pfnUniform2ui					= glUniform2ui				;
		m_instance->m_pfnUniform3ui					= glUniform3ui				;
		m_instance->m_pfnUniform4ui					= glUniform4ui				;
		m_instance->m_pfnUniform1uiv				= glUniform1uiv				;
		m_instance->m_pfnUniform2uiv				= glUniform2uiv				;
		m_instance->m_pfnUniform3uiv				= glUniform3uiv				;
		m_instance->m_pfnUniform4uiv				= glUniform4uiv				;
		m_instance->m_pfnUniform1f					= glUniform1f				;
		m_instance->m_pfnUniform2f					= glUniform2f				;
		m_instance->m_pfnUniform3f					= glUniform3f				;
		m_instance->m_pfnUniform4f					= glUniform4f				;
		m_instance->m_pfnUniform1fv					= glUniform1fv				;
		m_instance->m_pfnUniform2fv					= glUniform2fv				;
		m_instance->m_pfnUniform3fv					= glUniform3fv				;
		m_instance->m_pfnUniform4fv					= glUniform4fv				;
		m_instance->m_pfnUniform1d					= glUniform1d				;
		m_instance->m_pfnUniform2d					= glUniform2d				;
		m_instance->m_pfnUniform3d					= glUniform3d				;
		m_instance->m_pfnUniform4d					= glUniform4d				;
		m_instance->m_pfnUniform1dv					= glUniform1dv				;
		m_instance->m_pfnUniform2dv					= glUniform2dv				;
		m_instance->m_pfnUniform3dv					= glUniform3dv				;
		m_instance->m_pfnUniform4dv					= glUniform4dv				;
		m_instance->m_pfnUniformMatrix2fv			= glUniformMatrix2fv		;
		m_instance->m_pfnUniformMatrix2x3fv			= glUniformMatrix2x3fv		;
		m_instance->m_pfnUniformMatrix2x4fv			= glUniformMatrix2x4fv		;
		m_instance->m_pfnUniformMatrix3fv			= glUniformMatrix3fv		;
		m_instance->m_pfnUniformMatrix3x2fv			= glUniformMatrix3x2fv		;
		m_instance->m_pfnUniformMatrix3x4fv			= glUniformMatrix3x4fv		;
		m_instance->m_pfnUniformMatrix4fv			= glUniformMatrix4fv		;
		m_instance->m_pfnUniformMatrix4x2fv			= glUniformMatrix4x2fv		;
		m_instance->m_pfnUniformMatrix4x3fv			= glUniformMatrix4x3fv		;
		m_instance->m_pfnUniformMatrix2dv			= glUniformMatrix2dv		;
		m_instance->m_pfnUniformMatrix2x3dv			= glUniformMatrix2x3dv		;
		m_instance->m_pfnUniformMatrix2x4dv			= glUniformMatrix2x4dv		;
		m_instance->m_pfnUniformMatrix3dv			= glUniformMatrix3dv		;
		m_instance->m_pfnUniformMatrix3x2dv			= glUniformMatrix3x2dv		;
		m_instance->m_pfnUniformMatrix3x4dv			= glUniformMatrix3x4dv		;
		m_instance->m_pfnUniformMatrix4dv			= glUniformMatrix4dv		;
		m_instance->m_pfnUniformMatrix4x2dv			= glUniformMatrix4x2dv		;
		m_instance->m_pfnUniformMatrix4x3dv			= glUniformMatrix4x3dv		;
		m_instance->m_pfnCreateShader				= glCreateShader			;
		m_instance->m_pfnDeleteShader				= glDeleteShader			;
		m_instance->m_pfnAttachShader				= glAttachShader			;
		m_instance->m_pfnDetachShader				= glDetachShader			;
		m_instance->m_pfnCompileShader				= glCompileShader			;
		m_instance->m_pfnGetShaderiv				= glGetShaderiv				;
		m_instance->m_pfnGetShaderInfoLog			= glGetShaderInfoLog		;
		m_instance->m_pfnShaderSource				= glShaderSource			;
		m_instance->m_pfnCreateProgram				= glCreateProgram			;
		m_instance->m_pfnDeleteProgram				= glDeleteProgram			;
		m_instance->m_pfnLinkProgram				= glLinkProgram				;
		m_instance->m_pfnUseProgram					= glUseProgram				;
		m_instance->m_pfnGetProgramiv				= glGetProgramiv			;
		m_instance->m_pfnGetProgramInfoLog			= glGetProgramInfoLog		;
		m_instance->m_pfnGetAttribLocation			= glGetAttribLocation		;
		m_instance->m_pfnIsProgram					= glIsProgram				;
		m_instance->m_pfnClearColor					= & glClearColor			;
		m_instance->m_pfnShadeModel					= & glShadeModel			;
		m_instance->m_pfnViewport					= & glViewport				;
		m_instance->m_pfnMatrixMode					= & glMatrixMode			;
		m_instance->m_pfnLoadIdentity				= & glLoadIdentity			;
		m_instance->m_pfnOrtho						= & glOrtho					;
		m_instance->m_pfnClear						= & glClear					;
		m_instance->m_pfnDrawElements				= & glDrawElements			;
		m_instance->m_pfnEnable						= & glEnable				;
		m_instance->m_pfnDisable					= & glDisable				;
		m_instance->m_pfnGenTextures				= & glGenTextures			;
		m_instance->m_pfnDeleteTextures				= & glDeleteTextures		;
		m_instance->m_pfnBindTexture				= & glBindTexture			;
		m_instance->m_pfnTexSubImage2D				= & glTexSubImage2D			;
		m_instance->m_pfnTexImage2D					= & glTexImage2D			;
		m_instance->m_pfnTexParameteri				= & glTexParameteri			;
		m_instance->m_pfnEnableClientState			= & glEnableClientState		;
		m_instance->m_pfnDisableClientState			= & glDisableClientState	;
		m_instance->m_pfnVertexPointer				= & glVertexPointer			;
		m_instance->m_pfnTangentPointer				= glTangentPointerEXT		;
		m_instance->m_pfnColorPointer				= & glColorPointer			;
		m_instance->m_pfnNormalPointer				= & glNormalPointer			;
		m_instance->m_pfnGetTexImage				= & glGetTexImage			;
		m_instance->m_pfnTexCoordPointer			= & glTexCoordPointer		;
		m_instance->m_pfnTexEnvi					= & glTexEnvi				;
		m_instance->m_pfnTexEnviv					= & glTexEnviv				;
		m_instance->m_pfnTexEnvf					= & glTexEnvf				;
		m_instance->m_pfnTexEnvfv					= & glTexEnvfv				;
		m_instance->m_pfnBlendFunc					= & glBlendFunc				;
		m_instance->m_pfnAlphaFunc					= & glAlphaFunc				;
		m_instance->m_pfnColor3f					= & glColor3f				;
		m_instance->m_pfnColor3fv					= & glColor3fv				;
		m_instance->m_pfnColor4f					= & glColor4f				;
		m_instance->m_pfnColor4fv					= & glColor4fv				;
		m_instance->m_pfnLightModeli				= & glLightModeli			;
		m_instance->m_pfnLightModeliv				= & glLightModeliv			;
		m_instance->m_pfnLightModelf				= & glLightModelf			;
		m_instance->m_pfnLightModelfv				= & glLightModelfv			;
		m_instance->m_pfnIsTexture					= & glIsTexture				;
		m_instance->m_pfnTexGeni					= & glTexGeni				;
		m_instance->m_pfnPushMatrix					= & glPushMatrix			;
		m_instance->m_pfnPopMatrix					= & glPopMatrix				;
		m_instance->m_pfnTranslate					= & glTranslate				;
		m_instance->m_pfnRotate						= & glRotate				;
		m_instance->m_pfnScale						= & glScale					;
		m_instance->m_pfnMultMatrix					= & glMultMatrix			;
		m_instance->m_pfnFrustum					= & glFrustum				;
		m_instance->m_pfnCullFace					= & glCullFace				;
		m_instance->m_pfnFrontFace					= & glFrontFace				;
		m_instance->m_pfnMaterialf					= & glMaterialf				;
		m_instance->m_pfnMaterialfv					= & glMaterialfv			;
		m_instance->m_pfnLightf						= & glLightf				;
		m_instance->m_pfnLightfv					= & glLightfv				;
		m_instance->m_pfnSelectBuffer				= & glSelectBuffer			;
		m_instance->m_pfnGetIntegerv				= & glGetIntegerv			;
		m_instance->m_pfnRenderMode					= & glRenderMode			;
		m_instance->m_pfnEnableVertexAttribArray	= glEnableVertexAttribArray	;
		m_instance->m_pfnVertexAttribPointer		= glVertexAttribPointer		;
		m_instance->m_pfnDisableVertexAttribArray	= glDisableVertexAttribArray;
		m_instance->m_pfnGetUniformBlockIndex		= glGetUniformBlockIndex	;
		m_instance->m_pfnBindBufferBase				= glBindBufferBase			;
		m_instance->m_pfnUniformBlockBinding		= glUniformBlockBinding		;
		m_instance->m_pfnGetUniformIndices			= glGetUniformIndices		;
		m_instance->m_pfnGetActiveUniformsiv		= glGetActiveUniformsiv		;
		m_instance->m_pfnGenVertexArrays			= glGenVertexArrays			;
		m_instance->m_pfnBindVertexArray			= glBindVertexArray			;
		m_instance->m_pfnDeleteVertexArrays			= glDeleteVertexArrays		;
		m_instance->m_pfnGetActiveUniformBlockiv	= glGetActiveUniformBlockiv	;
#ifdef _WIN32
		m_instance->m_pfnMakeCurrent				= & wglMakeCurrent			;
		m_instance->m_pfnSwapBuffers				= & ::SwapBuffers			;
		m_instance->m_pfnCreateContext				= & wglCreateContext		;
		m_instance->m_pfnCreateContextAttribs		= wglCreateContextAttribsARB;
		m_instance->m_pfnDeleteContext				= & wglDeleteContext		;
#else
		m_instance->m_pfnMakeCurrent				= & glXMakeCurrent			;
		m_instance->m_pfnSwapBuffers				= & glXSwapBuffers			;
		m_instance->m_pfnCreateContext				= & glXCreateContext		;
		m_instance->m_pfnCreateContextAttribs		= glXCreateContextAttribsARB;
		m_instance->m_pfnDeleteContext				= & glXDestroyContext		;
#endif
	}
}

void OpenGl :: Cleanup()
{
	if (m_instance != NULL)
	{
		m_instance->m_pfnGetError					= NULL;
		m_instance->m_pfnGenerateMipmap				= NULL;
		m_instance->m_pfnActiveTexture				= NULL;
		m_instance->m_pfnClientActiveTexture		= NULL;
		m_instance->m_pfnGenBuffers					= NULL;
		m_instance->m_pfnBindBuffer					= NULL;
		m_instance->m_pfnDeleteBuffers				= NULL;
		m_instance->m_pfnIsBuffer					= NULL;
		m_instance->m_pfnBufferData					= NULL;
		m_instance->m_pfnBufferSubData				= NULL;
		m_instance->m_pfnGetBufferParameteriv		= NULL;
		m_instance->m_pfnMapBuffer					= NULL;
		m_instance->m_pfnUnmapBuffer				= NULL;
		m_instance->m_pfnGenFramebuffers			= NULL;
		m_instance->m_pfnDeleteFramebuffers			= NULL;
		m_instance->m_pfnBindFramebuffer			= NULL;
		m_instance->m_pfnFramebufferTexture2D		= NULL;
		m_instance->m_pfnCheckFramebufferStatus		= NULL;
		m_instance->m_pfnGetUniformLocation			= NULL;
		m_instance->m_pfnUniform1i					= NULL;
		m_instance->m_pfnUniform2i					= NULL;
		m_instance->m_pfnUniform3i					= NULL;
		m_instance->m_pfnUniform4i					= NULL;
		m_instance->m_pfnUniform1iv					= NULL;
		m_instance->m_pfnUniform2iv					= NULL;
		m_instance->m_pfnUniform3iv					= NULL;
		m_instance->m_pfnUniform4iv					= NULL;
		m_instance->m_pfnUniform1ui					= NULL;
		m_instance->m_pfnUniform2ui					= NULL;
		m_instance->m_pfnUniform3ui					= NULL;
		m_instance->m_pfnUniform4ui					= NULL;
		m_instance->m_pfnUniform1uiv				= NULL;
		m_instance->m_pfnUniform2uiv				= NULL;
		m_instance->m_pfnUniform3uiv				= NULL;
		m_instance->m_pfnUniform4uiv				= NULL;
		m_instance->m_pfnUniform1f					= NULL;
		m_instance->m_pfnUniform2f					= NULL;
		m_instance->m_pfnUniform3f					= NULL;
		m_instance->m_pfnUniform4f					= NULL;
		m_instance->m_pfnUniform1fv					= NULL;
		m_instance->m_pfnUniform2fv					= NULL;
		m_instance->m_pfnUniform3fv					= NULL;
		m_instance->m_pfnUniform4fv					= NULL;
		m_instance->m_pfnUniform1d					= NULL;
		m_instance->m_pfnUniform2d					= NULL;
		m_instance->m_pfnUniform3d					= NULL;
		m_instance->m_pfnUniform4d					= NULL;
		m_instance->m_pfnUniform1dv					= NULL;
		m_instance->m_pfnUniform2dv					= NULL;
		m_instance->m_pfnUniform3dv					= NULL;
		m_instance->m_pfnUniform4dv					= NULL;
		m_instance->m_pfnUniformMatrix2fv			= NULL;
		m_instance->m_pfnUniformMatrix2x3fv			= NULL;
		m_instance->m_pfnUniformMatrix2x4fv			= NULL;
		m_instance->m_pfnUniformMatrix3fv			= NULL;
		m_instance->m_pfnUniformMatrix3x2fv			= NULL;
		m_instance->m_pfnUniformMatrix3x4fv			= NULL;
		m_instance->m_pfnUniformMatrix4fv			= NULL;
		m_instance->m_pfnUniformMatrix4x2fv			= NULL;
		m_instance->m_pfnUniformMatrix4x3fv			= NULL;
		m_instance->m_pfnUniformMatrix2dv			= NULL;
		m_instance->m_pfnUniformMatrix2x3dv			= NULL;
		m_instance->m_pfnUniformMatrix2x4dv			= NULL;
		m_instance->m_pfnUniformMatrix3dv			= NULL;
		m_instance->m_pfnUniformMatrix3x2dv			= NULL;
		m_instance->m_pfnUniformMatrix3x4dv			= NULL;
		m_instance->m_pfnUniformMatrix4dv			= NULL;
		m_instance->m_pfnUniformMatrix4x2dv			= NULL;
		m_instance->m_pfnUniformMatrix4x3dv			= NULL;
		m_instance->m_pfnCreateShader				= NULL;
		m_instance->m_pfnDeleteShader				= NULL;
		m_instance->m_pfnAttachShader				= NULL;
		m_instance->m_pfnDetachShader				= NULL;
		m_instance->m_pfnCompileShader				= NULL;
		m_instance->m_pfnGetShaderiv				= NULL;
		m_instance->m_pfnGetShaderInfoLog			= NULL;
		m_instance->m_pfnShaderSource				= NULL;
		m_instance->m_pfnCreateProgram				= NULL;
		m_instance->m_pfnDeleteProgram				= NULL;
		m_instance->m_pfnLinkProgram				= NULL;
		m_instance->m_pfnUseProgram					= NULL;
		m_instance->m_pfnGetProgramiv				= NULL;
		m_instance->m_pfnGetProgramInfoLog			= NULL;
		m_instance->m_pfnGetAttribLocation			= NULL;
		m_instance->m_pfnIsProgram					= NULL;
		m_instance->m_pfnClearColor					= NULL;
		m_instance->m_pfnShadeModel					= NULL;
		m_instance->m_pfnViewport					= NULL;
		m_instance->m_pfnMatrixMode					= NULL;
		m_instance->m_pfnLoadIdentity				= NULL;
		m_instance->m_pfnOrtho						= NULL;
		m_instance->m_pfnClear						= NULL;
		m_instance->m_pfnDrawElements				= NULL;
		m_instance->m_pfnEnable						= NULL;
		m_instance->m_pfnDisable					= NULL;
		m_instance->m_pfnGenTextures				= NULL;
		m_instance->m_pfnDeleteTextures				= NULL;
		m_instance->m_pfnBindTexture				= NULL;
		m_instance->m_pfnTexSubImage2D				= NULL;
		m_instance->m_pfnTexImage2D					= NULL;
		m_instance->m_pfnTexParameteri				= NULL;
		m_instance->m_pfnEnableClientState			= NULL;
		m_instance->m_pfnDisableClientState			= NULL;
		m_instance->m_pfnVertexPointer				= NULL;
		m_instance->m_pfnNormalPointer				= NULL;
		m_instance->m_pfnTangentPointer				= NULL;
		m_instance->m_pfnColorPointer				= NULL;
		m_instance->m_pfnGetTexImage				= NULL;
		m_instance->m_pfnTexCoordPointer			= NULL;
		m_instance->m_pfnTexEnvi					= NULL;
		m_instance->m_pfnBlendFunc					= NULL;
		m_instance->m_pfnAlphaFunc					= NULL;
		m_instance->m_pfnColor3f					= NULL;
		m_instance->m_pfnColor3fv					= NULL;
		m_instance->m_pfnColor4f					= NULL;
		m_instance->m_pfnColor4fv					= NULL;
		m_instance->m_pfnLightModeli				= NULL;
		m_instance->m_pfnLightModeliv				= NULL;
		m_instance->m_pfnLightModelf				= NULL;
		m_instance->m_pfnLightModelfv				= NULL;
		m_instance->m_pfnIsTexture					= NULL;
		m_instance->m_pfnTexGeni					= NULL;
		m_instance->m_pfnPushMatrix					= NULL;
		m_instance->m_pfnPopMatrix					= NULL;
		m_instance->m_pfnTranslate					= NULL;
		m_instance->m_pfnRotate						= NULL;
		m_instance->m_pfnScale						= NULL;
		m_instance->m_pfnMultMatrix					= NULL;
		m_instance->m_pfnFrustum					= NULL;
		m_instance->m_pfnMakeCurrent				= NULL;
		m_instance->m_pfnSwapBuffers				= NULL;
		m_instance->m_pfnCreateContext				= NULL;
		m_instance->m_pfnCreateContextAttribs		= NULL;
		m_instance->m_pfnDeleteContext				= NULL;
		m_instance->m_pfnCullFace					= NULL;
		m_instance->m_pfnFrontFace					= NULL;
		m_instance->m_pfnMaterialf					= NULL;
		m_instance->m_pfnMaterialfv					= NULL;
		m_instance->m_pfnLightf						= NULL;
		m_instance->m_pfnLightfv					= NULL;
		m_instance->m_pfnSelectBuffer				= NULL;
		m_instance->m_pfnGetIntegerv				= NULL;
		m_instance->m_pfnRenderMode					= NULL;
		m_instance->m_pfnEnableVertexAttribArray	= NULL;
		m_instance->m_pfnVertexAttribPointer		= NULL;
		m_instance->m_pfnDisableVertexAttribArray	= NULL;
		m_instance->m_pfnGetUniformBlockIndex		= NULL;
		m_instance->m_pfnBindBufferBase				= NULL;
		m_instance->m_pfnUniformBlockBinding		= NULL;
		m_instance->m_pfnGetUniformIndices			= NULL;
		m_instance->m_pfnGetActiveUniformsiv		= NULL;
		m_instance->m_pfnGenVertexArrays			= NULL;
		m_instance->m_pfnBindVertexArray			= NULL;
		m_instance->m_pfnDeleteVertexArrays			= NULL;
		m_instance->m_pfnGetActiveUniformBlockiv	= NULL;
		delete m_instance;
		m_instance = NULL;
	}
}

GLenum OpenGl :: GetError()
{
	return m_instance->m_pfnGetError();
}

bool OpenGl :: ClearColor( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
	m_instance->m_pfnClearColor( red, green, blue, alpha);
	return glCheckError( "glClearColor");
}

bool OpenGl :: ShadeModel( GLenum mode)
{
	m_instance->m_pfnShadeModel( mode);
	return glCheckError( "glShadeModel");
}

bool OpenGl :: Clear( GLbitfield mask)
{
	m_instance->m_pfnClear( mask);
	return glCheckError( "glClear");
}

bool OpenGl :: DrawElements( GLenum mode, GLsizei count, GLenum type, const GLvoid *indices)
{
	m_instance->m_pfnDrawElements( mode, count, type, indices);
	return glCheckError( "glDrawElements");
}

bool OpenGl :: Enable( GLenum mode)
{
	m_instance->m_pfnEnable( mode);
	return glCheckError( "glEnable");
}

bool OpenGl :: Disable( GLenum mode)
{
	m_instance->m_pfnDisable( mode);
	return glCheckError( "glDisable");
}

bool OpenGl :: Color3f( GLfloat r, GLfloat g, GLfloat b)
{
	m_instance->m_pfnColor3f( r, g, b);
	return glCheckError( "glColor3f");
}

bool OpenGl :: Color3fv( const GLfloat * rgb)
{
	m_instance->m_pfnColor3fv( rgb);
	return glCheckError( "glColor3fv");
}

bool OpenGl :: Color4f( GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	m_instance->m_pfnColor4f( r, g, b, a);
	return glCheckError( "glColor4f");
}

bool OpenGl :: Color4fv( const GLfloat * rgba)
{
	m_instance->m_pfnColor4fv( rgba);
	return glCheckError( "glColor4fv");
}

bool OpenGl :: SelectBuffer( GLsizei size, GLuint * buffer)
{
	m_instance->m_pfnSelectBuffer( size, buffer);
	return glCheckError( "glSelectBuffer");
}

bool OpenGl :: GetIntegerv( GLenum pname, GLint * params)
{
	m_instance->m_pfnGetIntegerv( pname, params);
	return glCheckError( "glGetIntegerv");
}

GLint OpenGl :: RenderMode( GLenum mode)
{
	GLint l_iReturn = m_instance->m_pfnRenderMode( mode);

	if ( ! glCheckError( "glRenderMode"))
	{
		l_iReturn = GL_INVALID_INDEX;
	}

	return l_iReturn;
}

bool OpenGl :: Lightf( GLuint light, GLenum pname, GLfloat param)
{
	m_instance->m_pfnLightf( light, pname, param);
	return glCheckError( "glLightf");
}

bool OpenGl :: Lightfv( GLuint light, GLenum pname, const GLfloat * param)
{
	m_instance->m_pfnLightfv( light, pname, param);
	return glCheckError( "glLightfv");
}

bool OpenGl :: LightModeli( GLenum pname, GLint param)
{
	m_instance->m_pfnLightModeli( pname, param);
	return glCheckError( "glLightModeli");
}

bool OpenGl :: LightModeliv( GLenum pname, const GLint * param)
{
	m_instance->m_pfnLightModeliv( pname, param);
	return glCheckError( "glLightModeliv");
}

bool OpenGl :: LightModelf( GLenum pname, GLfloat param)
{
	m_instance->m_pfnLightModelf( pname, param);
	return glCheckError( "glLightModelf");
}

bool OpenGl :: LightModelfv( GLenum pname, const GLfloat * param)
{
	m_instance->m_pfnLightModelfv( pname, param);
	return glCheckError( "glLightModelfv");
}
#ifdef _WIN32
bool OpenGl :: MakeCurrent( HDC hdc, HGLRC hglrc)
{
	wglMakeCurrent( hdc, hglrc);
	return true;
}

bool OpenGl :: SwapBuffers( HDC hdc)
{
	::SwapBuffers( hdc);
	return true;
}

HGLRC OpenGl :: CreateContext( HDC hdc)
{
	return wglCreateContext( hdc);
}

bool OpenGl :: HasCreateContextAttribs()
{
	return m_instance->m_pfnCreateContextAttribs != NULL;
}

HGLRC OpenGl :: CreateContextAttribs( HDC hDC, HGLRC hShareContext, int const * attribList)
{
	return m_instance->m_pfnCreateContextAttribs( hDC, hShareContext, attribList);
}

bool OpenGl :: DeleteContext( HGLRC hContext)
{
	return wglDeleteContext( hContext) == TRUE;
}
#else
#	if CASTOR_GTK
bool OpenGl :: MakeCurrent( Display * pDisplay, GLXDrawable drawable, GLXContext context)
{
	m_instance->m_pfnMakeCurrent( pDisplay, drawable, context);
	return true;
}

bool OpenGl :: SwapBuffers( Display * pDisplay, GLXDrawable drawable)
{
	m_instance->m_pfnSwapBuffers( pDisplay, drawable);
	return true;
}

GLXContext OpenGl :: CreateContext( Display * pDisplay, XVisualInfo * pVisualInfo, GLXContext shareList, Bool direct)
{
	return m_instance->m_pfnCreateContext( pDisplay, pVisualInfo, shareList, direct);
}

bool OpenGl :: HasCreateContextAttribs()
{
	return m_instance->m_pfnCreateContextAttribs != NULL;
}

GLXContext OpenGl :: CreateContextAttribs( Display * pDisplay, GLXFBConfig fbconfig, GLXContext shareList, Bool direct, int const * attribList)
{
	return m_instance->m_pfnCreateContextAttribs( pDisplay, fbconfig, shareList, direct, attribList);
}

bool OpenGl :: DeleteContext( Display * pDisplay, GLXContext context)
{
	m_instance->m_pfnDeleteContext( pDisplay, context);
	return true;
}
#	endif
#endif
bool OpenGl :: Viewport( GLint x, GLint y, GLsizei width, GLsizei height)
{
	m_instance->m_pfnViewport( x, y, width, height);
	return glCheckError( "glViewport");
}

bool OpenGl :: MatrixMode( GLenum mode)
{
	m_instance->m_pfnMatrixMode( mode);
	return glCheckError( "glMatrixMode");
}

bool OpenGl :: LoadIdentity()
{
	m_instance->m_pfnLoadIdentity();
	return glCheckError( "glLoadIdentity");
}

bool OpenGl :: Ortho( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar)
{
	m_instance->m_pfnOrtho( left, right, bottom, top, zNear, zFar);
	return glCheckError( "glOrtho");
}

bool OpenGl :: Frustum( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar)
{
	m_instance->m_pfnFrustum( left, right, bottom, top, zNear, zFar);
	return glCheckError( "glFrustum");
}

bool OpenGl :: PushMatrix()
{
	m_instance->m_pfnPushMatrix();
	return glCheckError( "glPushMatrix");
}

bool OpenGl :: PopMatrix()
{
	m_instance->m_pfnPopMatrix();
	return glCheckError( "glPopMatrix");
}

bool OpenGl :: Translate( real x, real y, real z)
{
	m_instance->m_pfnTranslate( x, y, z);
	return glCheckError( "glTranslate");
}

bool OpenGl :: Rotate( real angle, real x, real y, real z)
{
	m_instance->m_pfnRotate( angle, x, y, z);
	return glCheckError( "glRotate");
}

bool OpenGl :: Scale( real x, real y, real z)
{
	m_instance->m_pfnScale( x, y, z);
	return glCheckError( "glScale");
}

bool OpenGl :: MultMatrix( real const * matrix)
{
	m_instance->m_pfnMultMatrix( matrix);
	return glCheckError( "glMultMatrix");
}

bool OpenGl :: CullFace( GLenum face)
{
	m_instance->m_pfnCullFace( face);
	return glCheckError( "glCullFace");
}

bool OpenGl :: FrontFace( GLenum face)
{
	m_instance->m_pfnFrontFace( face);
	return glCheckError( "glFrontFace");
}

bool OpenGl :: Materialf( GLenum face, GLenum pname, GLfloat param)
{
	m_instance->m_pfnMaterialf( face, pname, param);
	return glCheckError( "glMaterialf");
}

bool OpenGl :: Materialfv( GLenum face, GLenum pname, const GLfloat * param)
{
	m_instance->m_pfnMaterialfv( face, pname, param);
	return glCheckError( "glMaterialfv");
}

bool OpenGl :: GenTextures( GLsizei n, GLuint* textures)
{
	m_instance->m_pfnGenTextures( n, textures);
	return glCheckError( "glGenTextures");
}

bool OpenGl :: DeleteTextures( GLsizei n, const GLuint * textures)
{
	m_instance->m_pfnDeleteTextures( n, textures);
	return glCheckError( "glDeleteTextures");
}

bool OpenGl :: GenerateMipmap( GLenum target)
{
	m_instance->m_pfnGenerateMipmap( target);
	return glCheckError( "glGenerateMipmap");
}

bool OpenGl :: ActiveTexture( GLint texture)
{
	m_instance->m_pfnActiveTexture( texture);
	return glCheckError( "glActiveTexture");
}

bool OpenGl :: ClientActiveTexture( GLenum texture)
{
	m_instance->m_pfnClientActiveTexture( texture);
	return glCheckError( "glClientActiveTexture");
}

bool OpenGl :: BindTexture( GLenum mode, GLuint texture)
{
	m_instance->m_pfnBindTexture( mode, texture);
	return glCheckError( "glBindTexture");
}

bool OpenGl :: TexSubImage2D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid * data)
{
	m_instance->m_pfnTexSubImage2D( target, level, xoffset, yoffset, width, height, format, type, data);
	return glCheckError( "glTexSubImage2D");
}

bool OpenGl :: TexImage2D( GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid * data)
{
	m_instance->m_pfnTexImage2D( target, level, internalFormat, width, height, border, format, type, data);
	return glCheckError( "glTexImage2D");
}

bool OpenGl :: TexParameteri( GLenum target, GLenum pname, GLint param)
{
	m_instance->m_pfnTexParameteri( target, pname, param);
	return glCheckError( "glTexParameteri");
}

bool OpenGl :: GetTexImage( GLenum target, GLint level, GLenum format, GLenum type, GLvoid * img)
{
	m_instance->m_pfnGetTexImage( target, level, format, type, img);
	return glCheckError( "glGetTexImage");
}

bool OpenGl :: TexEnvi( GLenum target, GLenum pname, GLint param)
{
	m_instance->m_pfnTexEnvi( target, pname, param);
	return glCheckError( "glTexEnvi");
}

bool OpenGl :: TexEnviv( GLenum target, GLenum pname, const GLint * param)
{
	m_instance->m_pfnTexEnviv( target, pname, param);
	return glCheckError( "glTexEnviv");
}

bool OpenGl :: TexEnvf( GLenum target, GLenum pname, GLfloat param)
{
	m_instance->m_pfnTexEnvf( target, pname, param);
	return glCheckError( "glTexEnvf");
}

bool OpenGl :: TexEnvfv( GLenum target, GLenum pname, const GLfloat * param)
{
	m_instance->m_pfnTexEnvfv( target, pname, param);
	return glCheckError( "glTexEnvfv");
}

bool OpenGl :: BlendFunc( GLenum sfactor, GLenum dfactor)
{
	m_instance->m_pfnBlendFunc( sfactor, dfactor);
	return glCheckError( "glBlendFunc");
}

bool OpenGl :: AlphaFunc( GLenum func, GLclampf ref)
{
	m_instance->m_pfnAlphaFunc( func, ref);
	return glCheckError( "glAlphaFunc");
}

bool OpenGl :: IsTexture( GLuint texture)
{
	return m_instance->m_pfnIsTexture( texture) && glCheckError( "glIsTexture");
}

bool OpenGl :: TexGeni( GLenum coord, GLenum pname, GLint param)
{
	m_instance->m_pfnTexGeni( coord, pname, param);
	return glCheckError( "glTexGeni");
}

bool OpenGl :: EnableClientState( GLenum array)
{
	m_instance->m_pfnEnableClientState( array);
	return glCheckError( "glEnableClientState");
}

bool OpenGl :: DisableClientState( GLenum array)
{
	m_instance->m_pfnDisableClientState( array);
	return glCheckError( "glDisableClientState");
}

bool OpenGl :: GenBuffers( GLsizei n, GLuint* buffers)
{
	m_instance->m_pfnGenBuffers( n, buffers);
	return glCheckError( "glGenBuffers");
}

bool OpenGl :: BindBuffer( GLenum target, GLuint buffer)
{
	m_instance->m_pfnBindBuffer( target, buffer);
	return glCheckError( "glBindBuffer");
}

bool OpenGl :: DeleteBuffers( GLsizei n, const GLuint* buffers)
{
	m_instance->m_pfnDeleteBuffers( n, buffers);
	return glCheckError( "glDeleteBuffers");
}

bool OpenGl :: IsBuffer( GLuint buffer)
{
	return m_instance->m_pfnIsBuffer( buffer) && glCheckError( "glIsBuffer");
}

bool OpenGl :: BufferData( GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage)
{
	m_instance->m_pfnBufferData( target, size, data, usage);
	return glCheckError( "glBufferData");
}

bool OpenGl :: BufferSubData( GLenum target, GLintptrARB offset, GLsizeiptrARB size, const GLvoid * data)
{
	m_instance->m_pfnBufferSubData( target, offset, size, data);
	return glCheckError( "glBufferSubData");
}

bool OpenGl :: GetBufferParameteriv( GLenum target, GLenum pname, GLint * params)
{
	m_instance->m_pfnGetBufferParameteriv( target, pname, params);
	return glCheckError( "glGetBufferParameteriv");
}

GLvoid * OpenGl :: MapBuffer( GLenum target, GLenum access)
{
	GLvoid * l_pReturn = m_instance->m_pfnMapBuffer( target, access);

	if ( ! glCheckError( "glUnmapBuffer"))
	{
		l_pReturn = NULL;
	}

	return l_pReturn;
}

bool OpenGl :: UnmapBuffer( GLenum target)
{
	m_instance->m_pfnUnmapBuffer( target);
	return glCheckError( "glUnmapBuffer");
}

bool OpenGl :: VertexPointer( GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{
	m_instance->m_pfnVertexPointer( size, type, stride, pointer);
	return glCheckError( "glVertexPointer");
}

bool OpenGl :: NormalPointer( GLenum type, GLsizei stride, const GLvoid * pointer)
{
	m_instance->m_pfnNormalPointer( type, stride, pointer);
	return glCheckError( "glNormalPointer");
}

bool OpenGl :: TangentPointer( GLenum type, GLsizei stride, const GLvoid * pointer)
{
	m_instance->m_pfnTangentPointer( type, stride, pointer);
	return glCheckError( "glTangentPointer");
}

bool OpenGl :: HasTangentPointer()
{
	return m_instance->m_pfnTangentPointer != NULL;
}

bool OpenGl :: ColorPointer( GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{
	m_instance->m_pfnColorPointer( size, type, stride, pointer);
	return glCheckError( "glColorPointer");
}

bool OpenGl :: TexCoordPointer( GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{
	m_instance->m_pfnTexCoordPointer( size, type, stride, pointer);
	return glCheckError( "glTexCoordPointer");
}

bool OpenGl :: GenFramebuffers( GLsizei n, GLuint* framebuffers)
{
	m_instance->m_pfnGenFramebuffers( n, framebuffers);
	return glCheckError( "glGenFramebuffers");
}

bool OpenGl :: DeleteFramebuffers( GLsizei n, const GLuint* framebuffers)
{
	m_instance->m_pfnDeleteFramebuffers( n, framebuffers);
	return glCheckError( "glDeleteFramebuffers");
}

bool OpenGl :: BindFramebuffer( GLenum target, GLuint framebuffer)
{
	m_instance->m_pfnBindFramebuffer( target, framebuffer);
	return glCheckError( "glBindFramebuffer");
}

bool OpenGl :: FramebufferTexture2D( GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
	m_instance->m_pfnFramebufferTexture2D( target, attachment, textarget, texture, level);
	return glCheckError( "glFramebufferTexture2D");
}

GLenum OpenGl :: CheckFramebufferStatus( GLenum target)
{
	GLenum l_eReturn = m_instance->m_pfnCheckFramebufferStatus( target);

	if ( ! glCheckError( "glCheckFramebufferStatus"))
	{
		l_eReturn = GL_INVALID_INDEX;
	}

	return l_eReturn;
}

GLint OpenGl :: GetUniformLocation( GLuint program, const GLchar* name)
{
	GLint l_iReturn = m_instance->m_pfnGetUniformLocation( program, name);

	if ( ! glCheckError( "glGetUniformLocation"))
	{
		l_iReturn = GL_INVALID_INDEX;
	}

	return l_iReturn;
}

bool OpenGl :: Uniform1i( GLint location, GLint v0)
{
	m_instance->m_pfnUniform1i( location, v0);
	return glCheckError( "glUniform1i");
}

bool OpenGl :: Uniform2i( GLint location, GLint v0, GLint v1)
{
	m_instance->m_pfnUniform2i( location, v0, v1);
	return glCheckError( "glUniform2i");
}

bool OpenGl :: Uniform3i( GLint location, GLint v0, GLint v1, GLint v2)
{
	m_instance->m_pfnUniform3i( location, v0, v1, v2);
	return glCheckError( "glUniform3i");
}

bool OpenGl :: Uniform4i( GLint location, GLint v0, GLint v1, GLint v2, GLint v3)
{
	m_instance->m_pfnUniform4i( location, v0, v1, v2, v3);
	return glCheckError( "glUniform4i");
}

bool OpenGl :: Uniform1iv( GLint location, GLsizei count, const GLint * params)
{
	m_instance->m_pfnUniform1iv( location, count, params);
	return glCheckError( "glUniform1iv");
}

bool OpenGl :: Uniform2iv( GLint location, GLsizei count, const GLint * params)
{
	m_instance->m_pfnUniform2iv( location, count, params);
	return glCheckError( "glUniform2iv");
}

bool OpenGl :: Uniform3iv( GLint location, GLsizei count, const GLint * params)
{
	m_instance->m_pfnUniform3iv( location, count, params);
	return glCheckError( "glUniform3iv");
}

bool OpenGl :: Uniform4iv( GLint location, GLsizei count, const GLint * params)
{
	m_instance->m_pfnUniform4iv( location, count, params);
	return glCheckError( "glUniform4iv");
}

bool OpenGl :: Uniform1ui( GLint location, GLuint v0)
{
	m_instance->m_pfnUniform1ui( location, v0);
	return glCheckError( "glUniform1ui");
}

bool OpenGl :: Uniform2ui( GLint location, GLuint v0, GLuint v1)
{
	m_instance->m_pfnUniform2ui( location, v0, v1);
	return glCheckError( "glUniform2ui");
}

bool OpenGl :: Uniform3ui( GLint location, GLuint v0, GLuint v1, GLuint v2)
{
	m_instance->m_pfnUniform3ui( location, v0, v1, v2);
	return glCheckError( "glUniformu3i");
}

bool OpenGl :: Uniform4ui( GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3)
{
	m_instance->m_pfnUniform4ui( location, v0, v1, v2, v3);
	return glCheckError( "glUniform4ui");
}

bool OpenGl :: Uniform1uiv( GLint location, GLsizei count, const GLuint * params)
{
	m_instance->m_pfnUniform1uiv( location, count, params);
	return glCheckError( "glUniform1uiv");
}

bool OpenGl :: Uniform2uiv( GLint location, GLsizei count, const GLuint * params)
{
	m_instance->m_pfnUniform2uiv( location, count, params);
	return glCheckError( "glUniform2uiv");
}

bool OpenGl :: Uniform3uiv( GLint location, GLsizei count, const GLuint * params)
{
	m_instance->m_pfnUniform3uiv( location, count, params);
	return glCheckError( "glUniform3uiv");
}

bool OpenGl :: Uniform4uiv( GLint location, GLsizei count, const GLuint * params)
{
	m_instance->m_pfnUniform4uiv( location, count, params);
	return glCheckError( "glUniform4uiv");
}

bool OpenGl :: Uniform1f( GLint location, GLfloat v0)
{
	m_instance->m_pfnUniform1f( location, v0);
	return glCheckError( "glUniform1f");
}

bool OpenGl :: Uniform2f( GLint location, GLfloat v0, GLfloat v1)
{
	m_instance->m_pfnUniform2f( location, v0, v1);
	return glCheckError( "glUniform2f");
}

bool OpenGl :: Uniform3f( GLint location, GLfloat v0, GLfloat v1, GLfloat v2)
{
	m_instance->m_pfnUniform3f( location, v0, v1, v2);
	return glCheckError( "glUniform3f");
}

bool OpenGl :: Uniform4f( GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
	m_instance->m_pfnUniform4f( location, v0, v1, v2, v3);
	return glCheckError( "glUniform4f");
}

bool OpenGl :: Uniform1fv( GLint location, GLsizei count, const GLfloat * params)
{
	m_instance->m_pfnUniform1fv( location, count, params);
	return glCheckError( "glUniform1fv");
}

bool OpenGl :: Uniform2fv( GLint location, GLsizei count, const GLfloat * params)
{
	m_instance->m_pfnUniform2fv( location, count, params);
	return glCheckError( "glUniform2fv");
}

bool OpenGl :: Uniform3fv( GLint location, GLsizei count, const GLfloat * params)
{
	m_instance->m_pfnUniform3fv( location, count, params);
	return glCheckError( "glUniform3fv");
}

bool OpenGl :: Uniform4fv( GLint location, GLsizei count, const GLfloat * params)
{
	m_instance->m_pfnUniform4fv( location, count, params);
	return glCheckError( "glUniform4fv");
}

bool OpenGl :: Uniform1d( GLint location, GLdouble v0)
{
	m_instance->m_pfnUniform1d( location, v0);
	return glCheckError( "glUniform1d");
}

bool OpenGl :: Uniform2d( GLint location, GLdouble v0, GLdouble v1)
{
	m_instance->m_pfnUniform2d( location, v0, v1);
	return glCheckError( "glUniform2d");
}

bool OpenGl :: Uniform3d( GLint location, GLdouble v0, GLdouble v1, GLdouble v2)
{
	m_instance->m_pfnUniform3d( location, v0, v1, v2);
	return glCheckError( "glUniform3d");
}

bool OpenGl :: Uniform4d( GLint location, GLdouble v0, GLdouble v1, GLdouble v2, GLdouble v3)
{
	m_instance->m_pfnUniform4d( location, v0, v1, v2, v3);
	return glCheckError( "glUniform4d");
}

bool OpenGl :: Uniform1dv( GLint location, GLsizei count, const GLdouble * params)
{
	m_instance->m_pfnUniform1dv( location, count, params);
	return glCheckError( "glUniform1dv");
}

bool OpenGl :: Uniform2dv( GLint location, GLsizei count, const GLdouble * params)
{
	m_instance->m_pfnUniform2dv( location, count, params);
	return glCheckError( "glUniform2dv");
}

bool OpenGl :: Uniform3dv( GLint location, GLsizei count, const GLdouble * params)
{
	m_instance->m_pfnUniform3dv( location, count, params);
	return glCheckError( "glUniform3dv");
}

bool OpenGl :: Uniform4dv( GLint location, GLsizei count, const GLdouble * params)
{
	m_instance->m_pfnUniform4dv( location, count, params);
	return glCheckError( "glUniform4dv");
}

bool OpenGl :: UniformMatrix2fv( GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
	m_instance->m_pfnUniformMatrix2fv( location, count, transpose, value);
	return glCheckError( "glUniformMatrix2fv");
}

bool OpenGl :: UniformMatrix2x3fv( GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
	m_instance->m_pfnUniformMatrix2x3fv( location, count, transpose, value);
	return glCheckError( "glUniformMatrix2x3fv");
}

bool OpenGl :: UniformMatrix2x4fv( GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
	m_instance->m_pfnUniformMatrix2x4fv( location, count, transpose, value);
	return glCheckError( "glUniformMatrix2x4fv");
}

bool OpenGl :: UniformMatrix3fv( GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
	m_instance->m_pfnUniformMatrix3fv( location, count, transpose, value);
	return glCheckError( "glUniformMatrix3fv");
}

bool OpenGl :: UniformMatrix3x2fv( GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
	m_instance->m_pfnUniformMatrix3x2fv( location, count, transpose, value);
	return glCheckError( "glUniformMatrix3x2fv");
}

bool OpenGl :: UniformMatrix3x4fv( GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
	m_instance->m_pfnUniformMatrix3x4fv( location, count, transpose, value);
	return glCheckError( "glUniformMatrix3x4fv");
}

bool OpenGl :: UniformMatrix4fv( GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
	m_instance->m_pfnUniformMatrix4fv( location, count, transpose, value);
	return glCheckError( "glUniformMatrix4fv");
}

bool OpenGl :: UniformMatrix4x2fv( GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
	m_instance->m_pfnUniformMatrix4x2fv( location, count, transpose, value);
	return glCheckError( "glUniformMatrix4x2fv");
}

bool OpenGl :: UniformMatrix4x3fv( GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
	m_instance->m_pfnUniformMatrix4x3fv( location, count, transpose, value);
	return glCheckError( "glUniformMatrix4x3fv");
}

bool OpenGl :: UniformMatrix2dv( GLint location, GLsizei count, GLboolean transpose, const GLdouble * value)
{
	m_instance->m_pfnUniformMatrix2dv( location, count, transpose, value);
	return glCheckError( "glUniformMatrix2dv");
}

bool OpenGl :: UniformMatrix2x3dv( GLint location, GLsizei count, GLboolean transpose, const GLdouble * value)
{
	m_instance->m_pfnUniformMatrix2x3dv( location, count, transpose, value);
	return glCheckError( "glUniformMatrix2x3dv");
}

bool OpenGl :: UniformMatrix2x4dv( GLint location, GLsizei count, GLboolean transpose, const GLdouble * value)
{
	m_instance->m_pfnUniformMatrix2x4dv( location, count, transpose, value);
	return glCheckError( "glUniformMatrix2x4dv");
}

bool OpenGl :: UniformMatrix3dv( GLint location, GLsizei count, GLboolean transpose, const GLdouble * value)
{
	m_instance->m_pfnUniformMatrix3dv( location, count, transpose, value);
	return glCheckError( "glUniformMatrix3dv");
}

bool OpenGl :: UniformMatrix3x2dv( GLint location, GLsizei count, GLboolean transpose, const GLdouble * value)
{
	m_instance->m_pfnUniformMatrix3x2dv( location, count, transpose, value);
	return glCheckError( "glUniformMatrix3x2dv");
}

bool OpenGl :: UniformMatrix3x4dv( GLint location, GLsizei count, GLboolean transpose, const GLdouble * value)
{
	m_instance->m_pfnUniformMatrix3x4dv( location, count, transpose, value);
	return glCheckError( "glUniformMatrix3x4dv");
}

bool OpenGl :: UniformMatrix4dv( GLint location, GLsizei count, GLboolean transpose, const GLdouble * value)
{
	m_instance->m_pfnUniformMatrix4dv( location, count, transpose, value);
	return glCheckError( "glUniformMatrix4dv");
}

bool OpenGl :: UniformMatrix4x2dv( GLint location, GLsizei count, GLboolean transpose, const GLdouble * value)
{
	m_instance->m_pfnUniformMatrix4x2dv( location, count, transpose, value);
	return glCheckError( "glUniformMatrix4x2dv");
}

bool OpenGl :: UniformMatrix4x3dv( GLint location, GLsizei count, GLboolean transpose, const GLdouble * value)
{
	m_instance->m_pfnUniformMatrix4x3dv( location, count, transpose, value);
	return glCheckError( "glUniformMatrix4x3dv");
}

GLuint OpenGl :: GetUniformBlockIndex( GLuint program, const char* uniformBlockName)
{
	m_instance->m_pfnGetUniformBlockIndex( program, uniformBlockName);
	return glCheckError( "glGetUniformBlockIndex");
}

bool OpenGl :: BindBufferBase( GLenum target, GLuint index, GLuint buffer)
{
	m_instance->m_pfnBindBufferBase( target, index, buffer);
	return glCheckError( "glBindBufferBase");
}

bool OpenGl :: UniformBlockBinding( GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding)
{
	m_instance->m_pfnUniformBlockBinding( program, uniformBlockIndex, uniformBlockBinding);
	return glCheckError( "glUniformBlockBinding");
}

bool OpenGl :: GetUniformIndices( GLuint program, GLsizei uniformCount, const char** uniformNames, GLuint* uniformIndices)
{
	m_instance->m_pfnGetUniformIndices( program, uniformCount, uniformNames, uniformIndices);
	return glCheckError( "glGetUniformIndices");
}

bool OpenGl :: GetActiveUniformsiv( GLuint program, GLsizei uniformCount, const GLuint* uniformIndices, GLenum pname, GLint* params)
{
	m_instance->m_pfnGetActiveUniformsiv( program, uniformCount, uniformIndices, pname, params);
	return glCheckError( "glGetActiveUniformsiv");
}

bool OpenGl :: GetActiveUniformBlockiv( GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint* params)
{
	m_instance->m_pfnGetActiveUniformBlockiv( program, uniformBlockIndex, pname, params);
	return glCheckError( "glGetActiveUniformBlockiv");
}

GLuint OpenGl :: CreateShader( GLenum type)
{
	GLuint l_uiReturn = m_instance->m_pfnCreateShader( type);
	
	if ( ! glCheckError( "glCreateShader"))
	{
		l_uiReturn = GL_INVALID_INDEX;
	}

	return l_uiReturn;
}

bool OpenGl :: DeleteShader( GLuint shader)
{
	m_instance->m_pfnDeleteShader( shader);
	return glCheckError( "glDeleteShader");
}

bool OpenGl :: AttachShader( GLuint program, GLuint shader)
{
	m_instance->m_pfnAttachShader( program, shader);
	return glCheckError( "glAttachShader");
}

bool OpenGl :: DetachShader( GLuint program, GLuint shader)
{
	m_instance->m_pfnDetachShader( program, shader);
	return glCheckError( "glDetachShader");
}

bool OpenGl :: CompileShader( GLuint shader)
{
	m_instance->m_pfnCompileShader( shader);
	return glCheckError( "glCompileShader");
}

bool OpenGl :: GetShaderiv( GLuint shader, GLenum pname, GLint* param)
{
	m_instance->m_pfnGetShaderiv( shader, pname, param);
	return glCheckError( "glGetShaderiv");
}

bool OpenGl :: GetShaderInfoLog( GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog)
{
	m_instance->m_pfnGetShaderInfoLog( shader, bufSize, length, infoLog);
	return glCheckError( "glGetShaderInfoLog");
}

bool OpenGl :: ShaderSource( GLuint shader, GLsizei count, const GLchar** strings, const GLint* lengths)
{
	m_instance->m_pfnShaderSource( shader, count, strings, lengths);
	return glCheckError( "glShaderSource");
}

GLuint OpenGl :: CreateProgram()
{
	GLuint l_uiReturn = m_instance->m_pfnCreateProgram();

	if ( ! glCheckError( "glCreateProgram"))
	{
		l_uiReturn = GL_INVALID_INDEX;
	}

	return l_uiReturn;
}

bool OpenGl :: DeleteProgram( GLuint program)
{
	m_instance->m_pfnDeleteProgram( program);
	return glCheckError( "glDeleteProgram");
}

bool OpenGl :: LinkProgram( GLuint program)
{
	m_instance->m_pfnLinkProgram( program);
	return glCheckError( "glLinkProgram");
}

bool OpenGl :: UseProgram( GLuint program)
{
	m_instance->m_pfnUseProgram( program);
	return glCheckError( "glUseProgram");
}

bool OpenGl :: GetProgramiv( GLuint program, GLenum pname, GLint* param)
{
	m_instance->m_pfnGetProgramiv( program, pname, param);
	return glCheckError( "glGetProgramiv");
}

bool OpenGl :: GetProgramInfoLog( GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog)
{
	m_instance->m_pfnGetProgramInfoLog( program, bufSize, length, infoLog);
	return glCheckError( "glGetProgramInfoLog");
}

GLint OpenGl :: GetAttribLocation( GLuint program, const GLchar* name)
{
	GLint l_iReturn = m_instance->m_pfnGetAttribLocation( program, name);

	if ( ! glCheckError( "glGetAttribLocation"))
	{
		l_iReturn = GL_INVALID_INDEX;
	}

	return l_iReturn;
}

bool OpenGl :: IsProgram( GLuint program)
{
	m_instance->m_pfnIsProgram( program);
	return glCheckError( "glIsProgram");
}

bool OpenGl :: EnableVertexAttribArray( GLuint index)
{
	m_instance->m_pfnEnableVertexAttribArray( index);
	return glCheckError( "glEnableVertexAttribArray");
}

bool OpenGl :: VertexAttribPointer( GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid * pointer)
{
	m_instance->m_pfnVertexAttribPointer( index, size, type, normalized, stride, pointer);
	return glCheckError( "glVertexAttribPointer");
}

bool OpenGl :: DisableVertexAttribArray( GLuint index)
{
	m_instance->m_pfnDisableVertexAttribArray( index);
	return glCheckError( "glDisableVertexAttribArray");
}

bool OpenGl :: GenVertexArrays( GLsizei n, GLuint * arrays)
{
	m_instance->m_pfnGenVertexArrays( n, arrays);
	return glCheckError( "glGenVertexArrays");
}

bool OpenGl :: BindVertexArray( GLuint array)
{
	m_instance->m_pfnBindVertexArray( array);
	return glCheckError( "glBindVertexArray");
}

bool OpenGl :: DeleteVertexArrays( GLsizei n, GLuint * arrays)
{
	m_instance->m_pfnDeleteVertexArrays( n, arrays);
	return glCheckError( "glDeleteVertexArrays");
}

//*************************************************************************************************