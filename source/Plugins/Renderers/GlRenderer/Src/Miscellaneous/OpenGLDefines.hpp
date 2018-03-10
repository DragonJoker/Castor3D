/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___GlRenderer_OpenGLDefines_HPP___
#define ___GlRenderer_OpenGLDefines_HPP___
#pragma once

#include <cstdint>

#if RENDERLIB_WIN32
#	define GLAPIENTRY __stdcall
#	include <Windows.h>
#	undef MemoryBarrier
#else
#	define GLAPIENTRY
#endif

#include <cstddef>

namespace gl_renderer
{
	using GLbitfield = unsigned int;
	using GLboolean = unsigned char;
	using GLbyte = signed char;
	using GLchar = char;
	using GLclampd = double;
	using GLclampf = float;
	using GLdouble = double;
	using GLenum = unsigned int;
	using GLfloat = float;
	using GLint = int;
	using GLint64 = int64_t;
	using GLintptr = ptrdiff_t;
	using GLshort = short;
	using GLsizei = int;
	using GLsizeiptr = ptrdiff_t;
	using GLubyte = unsigned char;
	using GLuint = unsigned int;
	using GLuint64 = uint64_t;
	using GLulong = unsigned long;
	using GLushort = unsigned short;
	using GLvoid = void;
	typedef struct __GLsync *GLsync;

	static GLuint constexpr GL_INVALID_INDEX = ~GLuint( 0u );
	static GLuint constexpr GL_NO_ERROR = 0;
	static GLboolean constexpr GL_TRUE = 1;
	static GLboolean constexpr GL_FALSE = 0;

	enum GlDebugOutput
		: GLenum
	{
		GL_DEBUG_OUTPUT_SYNCHRONOUS = 0x8242,
	};

	enum GlDebugSource
		: GLenum
	{
		GL_DEBUG_SOURCE_API = 0x8246,
		GL_DEBUG_SOURCE_WINDOW_SYSTEM = 0x8247,
		GL_DEBUG_SOURCE_SHADER_COMPILER = 0x8248,
		GL_DEBUG_SOURCE_THIRD_PARTY = 0x8249,
		GL_DEBUG_SOURCE_APPLICATION = 0x824A,
		GL_DEBUG_SOURCE_OTHER = 0x824B,
	};

	enum GlDebugType
	{
		GL_DEBUG_TYPE_ERROR = 0x824C,
		GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR = 0x824D,
		GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR = 0x824E,
		GL_DEBUG_TYPE_PORTABILITY = 0x824F,
		GL_DEBUG_TYPE_PERFORMANCE = 0x8250,
		GL_DEBUG_TYPE_OTHER = 0x8251,
		GL_DEBUG_TYPE_MARKER = 0x8268,
		GL_DEBUG_TYPE_PUSH_GROUP = 0x8269,
		GL_DEBUG_TYPE_POP_GROUP = 0x826A,
	};

	enum GlDebugSeverity
	{
		GL_DEBUG_SEVERITY_NOTIFICATION = 0x826B,
		GL_DEBUG_SEVERITY_HIGH = 0x9146,
		GL_DEBUG_SEVERITY_MEDIUM = 0x9147,
		GL_DEBUG_SEVERITY_LOW = 0x9148,
	};

	enum GlDebugCategory
	{
		GL_DEBUG_CATEGORY_API_ERROR_AMD = 0x9149,
		GL_DEBUG_CATEGORY_WINDOW_SYSTEM_AMD = 0x914A,
		GL_DEBUG_CATEGORY_DEPRECATION_AMD = 0x914B,
		GL_DEBUG_CATEGORY_UNDEFINED_BEHAVIOR_AMD = 0x914C,
		GL_DEBUG_CATEGORY_PERFORMANCE_AMD = 0x914D,
		GL_DEBUG_CATEGORY_SHADER_COMPILER_AMD = 0x914E,
		GL_DEBUG_CATEGORY_APPLICATION_AMD = 0x914F,
		GL_DEBUG_CATEGORY_OTHER_AMD = 0x9150,
	};

	enum PatchParameter
	{
		GL_PATCH_VERTICES = 0x8E72,
	};

	enum ContextFlag
	{
		GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT = 0x0001,
		GL_CONTEXT_FLAG_DEBUG_BIT = 0x0002,
	};

	enum ContextMaskFlag
	{
		GL_CONTEXT_CORE_PROFILE_BIT = 0x00000001,
	};

	enum InformationType
	{
		GL_VENDOR = 0x1F00,
		GL_RENDERER = 0x1F01,
		GL_VERSION = 0x1F02,
		GL_EXTENSIONS = 0x1F03,
	};

#if RENDERLIB_WIN32
	using PFN_wglCreateContext = HGLRC ( GLAPIENTRY * )( HDC );
	using PFN_wglDeleteContext = BOOL ( GLAPIENTRY * )( HGLRC );
	using PFN_wglMakeCurrent = BOOL ( GLAPIENTRY * )( HDC, HGLRC );
	using PFN_wglSwapIntervalEXT = BOOL( GLAPIENTRY * )( int interval );

	enum ContextParameter
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB = 0x2091,
		WGL_CONTEXT_MINOR_VERSION_ARB = 0x2092,
		WGL_CONTEXT_FLAGS_ARB = 0x2094,
		WGL_CONTEXT_PROFILE_MASK_ARB = 0x9126,
	};
#elif RENDERLIB_XLIB
#else
#endif

	using PFN_glActiveTexture = void ( GLAPIENTRY * )( GLenum texture );
	using PFN_glAttachShader = void ( GLAPIENTRY * )( GLuint program, GLuint shader );
	using PFN_glBeginQuery = void ( GLAPIENTRY * )( GLenum target, GLuint id );
	using PFN_glBindBuffer = void ( GLAPIENTRY * )( GLenum target, GLuint buffer );
	using PFN_glBindBufferBase = void ( GLAPIENTRY * )( GLenum target, GLuint index, GLuint buffer );
	using PFN_glBindBufferRange = void ( GLAPIENTRY * )( GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size );
	using PFN_glBindFramebuffer = void ( GLAPIENTRY * )( GLenum target, GLuint framebuffer );
	using PFN_glBindImageTexture = void ( GLAPIENTRY * )( GLuint unit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format );
	using PFN_glBindSampler = void ( GLAPIENTRY * )( GLuint unit, GLuint sampler );
	using PFN_glBindTexture = void ( GLAPIENTRY * )( GLenum target, GLuint texture );
	using PFN_glBindVertexArray = void ( GLAPIENTRY * )( GLuint array );
	using PFN_glBlendColor = void ( GLAPIENTRY * )( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha );
	using PFN_glBlendEquationSeparatei = void ( GLAPIENTRY * )( GLuint buf, GLenum modeRGB, GLenum modeAlpha );
	using PFN_glBlendFuncSeparatei = void ( GLAPIENTRY * )( GLuint buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha );
	using PFN_glBlitFramebuffer = void ( GLAPIENTRY * )( GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter );
	using PFN_glBufferStorage = void ( GLAPIENTRY * )( GLenum target, GLsizeiptr size, const void * data, GLbitfield flags );
	using PFN_glCheckFramebufferStatus = GLenum( GLAPIENTRY * )( GLenum target );
	using PFN_glClear = void ( GLAPIENTRY * )( GLbitfield mask );
	using PFN_glClearDepth = void ( GLAPIENTRY * )( GLdouble depth );
	using PFN_glClearStencil = void ( GLAPIENTRY * )( GLint stencil );
	using PFN_glClearBufferfi = void ( GLAPIENTRY * )( GLenum buffer, GLint drawBuffer, GLfloat depth, GLint stencil );
	using PFN_glClearBufferfv = void ( GLAPIENTRY * )( GLenum buffer, GLint drawBuffer, const GLfloat* value );
	using PFN_glClearBufferiv = void ( GLAPIENTRY * )( GLenum buffer, GLint drawBuffer, const GLint* value );
	using PFN_glClearBufferuiv = void ( GLAPIENTRY * )( GLenum buffer, GLint drawBuffer, const GLuint* value );
	using PFN_glClearColor = void ( GLAPIENTRY * )( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha );
	using PFN_glClearTexImage = void ( GLAPIENTRY * )( GLuint texture, GLint level, GLenum format, GLenum type, const void *data );
	using PFN_glClientWaitSync = GLenum( GLAPIENTRY * )( GLsync GLsync, GLbitfield flags, GLuint64 timeout );
	using PFN_glClipControl = void ( GLAPIENTRY * )( GLenum origin, GLenum depth );
	using PFN_glCompileShader = void ( GLAPIENTRY * )( GLuint shader );
	using PFN_glCompressedTexSubImage1D = void ( GLAPIENTRY * )( GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const GLvoid * data );
	using PFN_glCompressedTexSubImage2D = void ( GLAPIENTRY * )( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid * data );
	using PFN_glCompressedTexSubImage3D = void ( GLAPIENTRY * )( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid * data );
	using PFN_glCopyBufferSubData = void ( GLAPIENTRY * )( GLenum readtarget, GLenum writetarget, GLintptr readoffset, GLintptr writeoffset, GLsizeiptr size );
	using PFN_glCopyImageSubData = void ( GLAPIENTRY * )( GLuint srcName, GLenum srcTarget, GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ, GLuint dstName, GLenum dstTarget, GLint dstLevel, GLint dstX, GLint dstY, GLint dstZ, GLsizei srcWidth, GLsizei srcHeight, GLsizei srcDepth );
	using PFN_glCreateProgram = GLuint( GLAPIENTRY * )( void );
	using PFN_glCreateShader = GLuint( GLAPIENTRY * )( GLenum type );
	using PFN_glCreateShaderProgramv = GLuint( GLAPIENTRY * )( GLenum type, GLsizei count, const char ** strings );
	using PFN_glCullFace = void ( GLAPIENTRY * )( GLenum mode );
	using PFN_glDeleteBuffers = void ( GLAPIENTRY * )( GLsizei n, const GLuint * buffers );
	using PFN_glDeleteFramebuffers = void ( GLAPIENTRY * )( GLsizei n, const GLuint* framebuffers );
	using PFN_glDeleteProgram = void ( GLAPIENTRY * )( GLuint program );
	using PFN_glDeleteQueries = void ( GLAPIENTRY * )( GLsizei n, const GLuint * ids );
	using PFN_glDeleteSamplers = void ( GLAPIENTRY * )( GLsizei count, const GLuint * samplers );
	using PFN_glDeleteShader = void ( GLAPIENTRY * )( GLuint shader );
	using PFN_glDeleteSync = void ( GLAPIENTRY * )( GLsync GLsync );
	using PFN_glDeleteTextures = void ( GLAPIENTRY * )( GLsizei n, GLuint const * textures );
	using PFN_glDeleteVertexArrays = void ( GLAPIENTRY * )( GLsizei n, const GLuint* arrays );
	using PFN_glDepthFunc = void ( GLAPIENTRY * )( GLenum func );
	using PFN_glDepthMask = void ( GLAPIENTRY * )( GLboolean flag );
	using PFN_glDepthRange = void ( GLAPIENTRY * )( GLclampd zNear, GLclampd zFar );
	using PFN_glDisable = void ( GLAPIENTRY * )( GLenum cap );
	using PFN_glDispatchCompute = void ( GLAPIENTRY * )( GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z );
	using PFN_glDispatchComputeIndirect = void ( GLAPIENTRY * )( GLintptr indirect );
	using PFN_glDrawArrays = void ( GLAPIENTRY * )( GLenum mode, GLint first, GLsizei count );
	using PFN_glDrawArraysInstancedBaseInstance = void ( GLAPIENTRY * )( GLenum mode, GLint first, GLsizei count, GLsizei primcount, GLuint baseinstance );
	using PFN_glDrawBuffers = void ( GLAPIENTRY * )( GLsizei n, const GLenum* bufs );
	using PFN_glDrawElementsBaseVertex = void ( GLAPIENTRY * )( GLenum mode, GLsizei count, GLenum type, void *indices, GLint basevertex );
	using PFN_glDrawElementsInstanced = void ( GLAPIENTRY * )( GLenum mode, GLsizei count, GLenum type, const void * indices, GLsizei primcount );
	using PFN_glDrawElementsInstancedBaseInstance = void ( GLAPIENTRY * )( GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei primcount, GLuint baseinstance );
	using PFN_glDrawElementsInstancedBaseVertexBaseInstance = void ( GLAPIENTRY * )( GLenum mode, GLsizei count, GLenum type, GLvoid *indices, GLsizei primcount, GLint basevertex, GLuint baseinstance );
	using PFN_glEnable = void ( GLAPIENTRY * )( GLenum cap );
	using PFN_glEnableVertexAttribArray = void ( GLAPIENTRY * )( GLuint index );
	using PFN_glEndQuery = void ( GLAPIENTRY * )( GLenum target );
	using PFN_glFenceSync = GLsync( GLAPIENTRY * )( GLenum condition, GLbitfield flags );
	using PFN_glFinish = void ( GLAPIENTRY * )();
	using PFN_glFlushMappedBufferRange = void ( GLAPIENTRY * )( GLenum target, GLintptr offset, GLsizeiptr length );
	using PFN_glFramebufferTexture1D = void ( GLAPIENTRY * )( GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level );
	using PFN_glFramebufferTexture2D = void ( GLAPIENTRY * )( GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level );
	using PFN_glFramebufferTexture3D = void ( GLAPIENTRY * )( GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint layer );
	using PFN_glFramebufferTextureLayer = void ( GLAPIENTRY * )( GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer );
	using PFN_glFrontFace = void ( GLAPIENTRY * )( GLenum mode );
	using PFN_glGenBuffers = void ( GLAPIENTRY * )( GLsizei n, GLuint * buffers );
	using PFN_glGenFramebuffers = void ( GLAPIENTRY * )( GLsizei n, GLuint* framebuffers );
	using PFN_glGenQueries = void ( GLAPIENTRY * )( GLsizei n, GLuint * ids );
	using PFN_glGenSamplers = void ( GLAPIENTRY * )( GLsizei count, GLuint * samplers );
	using PFN_glGenTextures = void ( GLAPIENTRY * )( GLsizei n, GLuint * textures );
	using PFN_glGenerateMipmap = void ( GLAPIENTRY * )( GLenum target );
	using PFN_glGenVertexArrays = void ( GLAPIENTRY * )( GLsizei n, GLuint * arrays );
	using PFN_glGetError = GLenum( GLAPIENTRY * )( void );
	using PFN_glGetFloatv = void ( GLAPIENTRY * )( GLenum pname, GLfloat * data );
	using PFN_glGetIntegerv = void ( GLAPIENTRY * )( GLenum pname, GLint * data );
	using PFN_glGetProgramInfoLog = void ( GLAPIENTRY * )( GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog );
	using PFN_glGetProgramInterfaceiv = void ( GLAPIENTRY * )( GLuint program, GLenum programInterface, GLenum pname, GLint * params );
	using PFN_glGetProgramiv = void ( GLAPIENTRY * )( GLuint program, GLenum pname, GLint* param );
	using PFN_glGetProgramResourceiv = void ( GLAPIENTRY * )( GLuint program, GLenum programInterface, GLuint index, GLsizei propCount, const GLenum * props, GLsizei bufSize, GLsizei * length, GLint * params );
	using PFN_glGetProgramResourceIndex = GLuint ( GLAPIENTRY * )( GLuint program, GLenum programInterface, const char * name );
	using PFN_glGetProgramResourceName = void ( GLAPIENTRY * )( GLuint program, GLenum programInterface, GLuint index, GLsizei bufSize, GLsizei * length, char * name );
	using PFN_glGetQueryObjectui64v = void ( GLAPIENTRY * )( GLuint id, GLenum pname, GLuint64 * params );
	using PFN_glGetQueryObjectuiv = void ( GLAPIENTRY * )( GLuint id, GLenum pname, GLuint * params );
	using PFN_glGetShaderInfoLog = void ( GLAPIENTRY * )( GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog );
	using PFN_glGetShaderiv = void ( GLAPIENTRY * )( GLuint shader, GLenum pname, GLint* param );
	using PFN_glGetString = GLubyte *( GLAPIENTRY * )( GLenum name ); 
	using PFN_glGetTexImage = void ( GLAPIENTRY * )( GLenum target, GLint level, GLenum format, GLenum type, void *pixels );
	using PFN_glGetTexLevelParameterfv = void ( GLAPIENTRY * )( GLenum target, GLint level, GLenum pname, GLfloat * params );
	using PFN_glGetTexLevelParameteriv = void ( GLAPIENTRY * )( GLenum target, GLint level, GLenum pname, GLint * params );
	using PFN_glGetTexParameterfv = void ( GLAPIENTRY * )( GLenum target, GLenum pname, GLfloat * params );
	using PFN_glGetTexParameteriv = void ( GLAPIENTRY * )( GLenum target, GLenum pname, GLint * params );
	using PFN_glInvalidateBufferSubData = void ( GLAPIENTRY * )( GLuint buffer, GLintptr offset, GLsizeiptr length );
	using PFN_glLineWidth = void ( GLAPIENTRY * )( GLfloat width );
	using PFN_glLinkProgram = void ( GLAPIENTRY * )( GLuint program );
	using PFN_glLogicOp = void ( GLAPIENTRY * )( GLenum opcode );
	using PFN_glMapBufferRange = void * ( GLAPIENTRY * )( GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access );
	using PFN_glMemoryBarrier = void ( GLAPIENTRY * )( GLbitfield barriers );
	using PFN_glMinSampleShading = void ( GLAPIENTRY * )( GLfloat value );
	using PFN_glMultiDrawArraysIndirect = void ( GLAPIENTRY * )( GLenum mode, const void * indirect, GLsizei drawcount, GLsizei stride );
	using PFN_glMultiDrawElementsIndirect = void ( GLAPIENTRY * )( GLenum mode, GLenum type, const void * indirect, GLsizei drawcount, GLsizei stride );
	using PFN_glPatchParameteri = void ( GLAPIENTRY * )( GLenum pname, GLint value );
	using PFN_glPolygonMode = void ( GLAPIENTRY * )( GLenum face, GLenum mode );
	using PFN_glPolygonOffsetClampEXT = void ( GLAPIENTRY * )( GLfloat factor, GLfloat units, GLfloat clamp );
	using PFN_glQueryCounter = void ( GLAPIENTRY * )( GLuint id, GLenum target );
	using PFN_glReadBuffer = void ( GLAPIENTRY * )( GLenum mode );
	using PFN_glReadPixels = void( GLAPIENTRY * )( GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void *pixels );
	using PFN_glSamplerParameterf = void ( GLAPIENTRY * )( GLuint sampler, GLenum pname, GLfloat param );
	using PFN_glSamplerParameterfv = void ( GLAPIENTRY * )( GLuint sampler, GLenum pname, const GLfloat * param );
	using PFN_glSamplerParameteri = void ( GLAPIENTRY * )( GLuint sampler, GLenum pname, GLint param );
	using PFN_glSamplerParameteriv = void ( GLAPIENTRY * )( GLuint sampler, GLenum pname, const GLint * param );
	using PFN_glScissor = void ( GLAPIENTRY * )( GLint x, GLint y, GLsizei width, GLsizei height );
	using PFN_glShaderBinary = void ( GLAPIENTRY * )( GLsizei count, const GLuint *shaders, GLenum binaryFormat, const void * binary, GLsizei length );
	using PFN_glShaderSource = void ( GLAPIENTRY * )( GLuint shader, GLsizei count, const GLchar *const* string, const GLint* length );
	using PFN_glSpecializeShader = void ( GLAPIENTRY * )( GLuint shader, const GLchar * pEntryPoint, GLuint numSpecializationConstants, const GLuint * pConstantIndex, const GLuint *pConstantValue );
	using PFN_glStencilFuncSeparate = void ( GLAPIENTRY * )( GLenum frontfunc, GLenum backfunc, GLint ref, GLuint mask );
	using PFN_glStencilMaskSeparate = void ( GLAPIENTRY * )( GLenum face, GLuint mask );
	using PFN_glStencilOpSeparate = void ( GLAPIENTRY * )( GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass );
	using PFN_glTexBufferRange = void ( GLAPIENTRY * )( GLenum target, GLenum internalformat, GLuint buffer, GLintptr offset, GLsizeiptr size );
	using PFN_glTexImage1D = void ( GLAPIENTRY * )( GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const void * pixels );
	using PFN_glTexImage2D = void ( GLAPIENTRY * )( GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void * pixels );
	using PFN_glTexImage2DMultisample = void ( GLAPIENTRY * )( GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations );
	using PFN_glTexImage3D = void ( GLAPIENTRY * )( GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void * pixels );
	using PFN_glTexImage3DMultisample = void ( GLAPIENTRY * )( GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations );
	using PFN_glTexParameteri = void( GLAPIENTRY * )( GLenum target, GLenum pname, GLint param );
	using PFN_glTexSubImage1D = void ( GLAPIENTRY * )( GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void *pixels );
	using PFN_glTexSubImage2D = void ( GLAPIENTRY * )( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels );
	using PFN_glTexSubImage3D = void ( GLAPIENTRY * )( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels );
	using PFN_glTexStorage1D = void ( GLAPIENTRY * )( GLenum target, GLsizei levels, GLenum internalformat, GLsizei width );
	using PFN_glTexStorage2D = void ( GLAPIENTRY * )( GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height );
	using PFN_glTexStorage2DMultisample = void ( GLAPIENTRY * )( GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations );
	using PFN_glTexStorage3D = void ( GLAPIENTRY * )( GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth );
	using PFN_glTexStorage3DMultisample = void ( GLAPIENTRY * )( GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations );
	using PFN_glTextureView = void ( GLAPIENTRY * )( GLuint texture, GLenum target, GLuint origtexture, GLenum internalformat, GLuint minlevel, GLuint numlevels, GLuint minlayer, GLuint numlayers );
	using PFN_glUniform1fv = void ( GLAPIENTRY * )( GLint location, GLsizei count, const GLfloat* value );
	using PFN_glUniform1iv = void ( GLAPIENTRY * )( GLint location, GLsizei count, const GLint* value );
	using PFN_glUniform1uiv = void ( GLAPIENTRY * )( GLint location, GLsizei count, const GLuint *value );
	using PFN_glUniform2fv = void ( GLAPIENTRY * )( GLint location, GLsizei count, const GLfloat* value );
	using PFN_glUniform2iv = void ( GLAPIENTRY * )( GLint location, GLsizei count, const GLint* value );
	using PFN_glUniform2uiv = void ( GLAPIENTRY * )( GLint location, GLsizei count, const GLuint *value );
	using PFN_glUniform3fv = void ( GLAPIENTRY * )( GLint location, GLsizei count, const GLfloat* value );
	using PFN_glUniform3iv = void ( GLAPIENTRY * )( GLint location, GLsizei count, const GLint* value );
	using PFN_glUniform3uiv = void ( GLAPIENTRY * )( GLint location, GLsizei count, const GLuint *value );
	using PFN_glUniform4fv = void ( GLAPIENTRY * )( GLint location, GLsizei count, const GLfloat* value );
	using PFN_glUniform4iv = void ( GLAPIENTRY * )( GLint location, GLsizei count, const GLint* value );
	using PFN_glUniform4uiv = void ( GLAPIENTRY * )( GLint location, GLsizei count, const GLuint *value );
	using PFN_glUniformMatrix2fv = void ( GLAPIENTRY * )( GLint location, GLsizei count, GLboolean transpose, const GLfloat* value );
	using PFN_glUniformMatrix3fv = void ( GLAPIENTRY * )( GLint location, GLsizei count, GLboolean transpose, const GLfloat* value );
	using PFN_glUniformMatrix4fv = void ( GLAPIENTRY * )( GLint location, GLsizei count, GLboolean transpose, const GLfloat* value );
	using PFN_glUnmapBuffer = GLboolean( GLAPIENTRY * )( GLenum target );
	using PFN_glUseProgram = void ( GLAPIENTRY * )( GLuint program );
	using PFN_glVertexAttribDivisor = void ( GLAPIENTRY * )( GLuint index, GLuint divisor );
	using PFN_glVertexAttribIPointer = void ( GLAPIENTRY * )( GLuint index, GLint size, GLenum type, GLsizei stride, const void* pointer );
	using PFN_glVertexAttribPointer = void ( GLAPIENTRY * )( GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer );
	using PFN_glViewport = void ( GLAPIENTRY * )( GLint x, GLint y, GLsizei width, GLsizei height );
}

#endif
