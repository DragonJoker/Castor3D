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

#include "GlRenderSystemPrerequisites.hpp"

#include "GlObject.hpp"

#include <TextureUnit.hpp>

#if !defined( CALLBACK )
#	if defined( _WIN32 )
#		define CALLBACK __stdcall
#	else
#		define CALLBACK
#	endif
#endif

#if defined( __linux__ )
#	include <GL/glx.h>
#	define GLX_GLXEXT_PROTOTYPES
#	include <GL/glxext.h>
#endif

//*************************************************************************************************

namespace GlRender
{
	class TexFunctionsBase
		: public Holder
	{
	public:
		TexFunctionsBase( OpenGl & p_gl );
		virtual bool GenerateMipmap( eGL_TEXDIM p_target ) = 0;
		virtual bool BindTexture( eGL_TEXDIM p_target, uint32_t texture ) = 0;
		virtual bool TexSubImage1D( eGL_TEXDIM p_target, int level, int xoffset, int width, eGL_FORMAT format, eGL_TYPE type, void const * data ) = 0;
		virtual bool TexSubImage2D( eGL_TEXDIM p_target, int level, int xoffset, int yoffset, int width, int height, eGL_FORMAT format, eGL_TYPE type, void const * data ) = 0;
		virtual bool TexSubImage2D( eGL_TEXDIM p_target, int level, Castor::Position const & p_position, Castor::Size const & p_size, eGL_FORMAT format, eGL_TYPE type, void const * data ) = 0;
		virtual bool TexSubImage2D( eGL_TEXDIM p_target, int level, Castor::Rectangle const & p_rect, eGL_FORMAT format, eGL_TYPE type, void const * data ) = 0;
		virtual bool TexSubImage3D( eGL_TEXDIM p_target, int level, int xoffset, int yoffset, int zoffset, int width, int height, int depth, eGL_FORMAT format, eGL_TYPE type, void const * data ) = 0;
		virtual bool TexImage1D( eGL_TEXDIM p_target, int level, eGL_INTERNAL internalFormat, int width, int border, eGL_FORMAT format, uint32_t type, void const * data ) = 0;
		virtual bool TexImage2D( eGL_TEXDIM p_target, int level, eGL_INTERNAL internalFormat, int width, int height, int border, eGL_FORMAT format, uint32_t type, void const * data ) = 0;
		virtual bool TexImage2D( eGL_TEXDIM p_target, int level, eGL_INTERNAL internalFormat, Castor::Size const & p_size, int border, eGL_FORMAT format, uint32_t type, void const * data ) = 0;
		virtual bool TexImage3D( eGL_TEXDIM p_target, int level, eGL_INTERNAL internalFormat, int width, int height, int depth, int border, eGL_FORMAT format, uint32_t type, void const * data ) = 0;
		virtual bool TexParameter( eGL_TEXDIM p_target, eGL_TEXTURE_PARAMETER pname, int param ) = 0;
		virtual bool TexParameter( eGL_TEXDIM p_target, eGL_TEXTURE_PARAMETER pname, float param ) = 0;
		virtual bool TexParameter( eGL_TEXDIM p_target, eGL_TEXTURE_PARAMETER pname, const int * params ) = 0;
		virtual bool TexParameter( eGL_TEXDIM p_target, eGL_TEXTURE_PARAMETER pname, const float * params ) = 0;
		virtual bool GetTexImage( eGL_TEXDIM p_target, int level, eGL_FORMAT format, eGL_TYPE type, void * img ) = 0;
	};

	class TexFunctions
		: public TexFunctionsBase
	{
	public:
		TexFunctions( OpenGl & p_gl );
		virtual bool BindTexture( eGL_TEXDIM p_target, uint32_t texture );
		virtual bool GenerateMipmap( eGL_TEXDIM p_target );
		virtual bool TexSubImage1D( eGL_TEXDIM p_target, int level, int xoffset, int width, eGL_FORMAT format, eGL_TYPE type, void const * data );
		virtual bool TexSubImage2D( eGL_TEXDIM p_target, int level, int xoffset, int yoffset, int width, int height, eGL_FORMAT format, eGL_TYPE type, void const * data );
		virtual bool TexSubImage2D( eGL_TEXDIM p_target, int level, Castor::Position const & p_position, Castor::Size const & p_size, eGL_FORMAT format, eGL_TYPE type, void const * data );
		virtual bool TexSubImage2D( eGL_TEXDIM p_target, int level, Castor::Rectangle const & p_rect, eGL_FORMAT format, eGL_TYPE type, void const * data );
		virtual bool TexSubImage3D( eGL_TEXDIM p_target, int level, int xoffset, int yoffset, int zoffset, int width, int height, int depth, eGL_FORMAT format, eGL_TYPE type, void const * data );
		virtual bool TexImage1D( eGL_TEXDIM p_target, int level, eGL_INTERNAL internalFormat, int width, int border, eGL_FORMAT format, uint32_t type, void const * data );
		virtual bool TexImage2D( eGL_TEXDIM p_target, int level, eGL_INTERNAL internalFormat, int width, int height, int border, eGL_FORMAT format, uint32_t type, void const * data );
		virtual bool TexImage2D( eGL_TEXDIM p_target, int level, eGL_INTERNAL internalFormat, Castor::Size const & p_size, int border, eGL_FORMAT format, uint32_t type, void const * data );
		virtual bool TexImage3D( eGL_TEXDIM p_target, int level, eGL_INTERNAL internalFormat, int width, int height, int depth, int border, eGL_FORMAT format, uint32_t type, void const * data );
		virtual bool TexParameter( eGL_TEXDIM p_target, eGL_TEXTURE_PARAMETER pname, int param );
		virtual bool TexParameter( eGL_TEXDIM p_target, eGL_TEXTURE_PARAMETER pname, float param );
		virtual bool TexParameter( eGL_TEXDIM p_target, eGL_TEXTURE_PARAMETER pname, const int * params );
		virtual bool TexParameter( eGL_TEXDIM p_target, eGL_TEXTURE_PARAMETER pname, const float * params );
		virtual bool GetTexImage( eGL_TEXDIM p_target, int level, eGL_FORMAT format, eGL_TYPE type, void * img );

		std::function< void( uint32_t mode, uint32_t texture ) > m_pfnBindTexture;
#if CASTOR_HAS_VARIADIC_TEMPLATES
		std::function< void( uint32_t target, int level, int xoffset, int width, uint32_t format, uint32_t type, void const * data ) > m_pfnTexSubImage1D;
		std::function< void( uint32_t target, int level, int xoffset, int yoffset, int width, int height, uint32_t format, uint32_t type, void const * data ) > m_pfnTexSubImage2D;
		std::function< void( uint32_t target, int level, int xoffset, int yoffset, int zoffset, int width, int height, int depth, uint32_t format, uint32_t type, void const * data ) > m_pfnTexSubImage3D;
		std::function< void( uint32_t target, int level, int internalFormat, int width, int border, uint32_t format, uint32_t type, void const * data ) > m_pfnTexImage1D;
		std::function< void( uint32_t target, int level, int internalFormat, int width, int height, int border, uint32_t format, uint32_t type, void const * data ) > m_pfnTexImage2D;
		std::function< void( uint32_t target, int level, int internalFormat, int width, int height, int depth, int border, uint32_t format, uint32_t type, void const * data ) > m_pfnTexImage3D;
#else
		void ( CALLBACK * m_pfnTexSubImage1D )( uint32_t target, int level, int xoffset, int width, uint32_t format, uint32_t type, void const * data );
		void ( CALLBACK * m_pfnTexSubImage2D )( uint32_t target, int level, int xoffset, int yoffset, int width, int height, uint32_t format, uint32_t type, void const * data );
		void ( CALLBACK * m_pfnTexSubImage3D )( uint32_t target, int level, int xoffset, int yoffset, int zoffset, int width, int height, int depth, uint32_t format, uint32_t type, void const * data );
		void ( CALLBACK * m_pfnTexImage1D )( uint32_t target, int level, int internalFormat, int width, int border, uint32_t format, uint32_t type, void const * data );
		void ( CALLBACK * m_pfnTexImage2D )( uint32_t target, int level, int internalFormat, int width, int height, int border, uint32_t format, uint32_t type, void const * data );
		void ( CALLBACK * m_pfnTexImage3D )( uint32_t target, int level, int internalFormat, int width, int height, int depth, int border, uint32_t format, uint32_t type, void const * data );
#endif
		std::function< void( uint32_t target, int level, uint32_t format, uint32_t type, void * pixels ) > m_pfnGetTexImage;
		std::function< void( uint32_t target, uint32_t pname, int param ) > m_pfnTexParameteri;
		std::function< void( uint32_t target, uint32_t pname, float param ) > m_pfnTexParameterf;
		std::function< void( uint32_t target, uint32_t pname, const int * params ) > m_pfnTexParameteriv;
		std::function< void( uint32_t target, uint32_t pname, const float * param ) > m_pfnTexParameterfv;
		std::function< void( uint32_t target ) > m_pfnGenerateMipmap;
	};

	class TexFunctionsDSA
		: public TexFunctionsBase
	{
	public:
		TexFunctionsDSA( OpenGl & p_gl );
		virtual bool BindTexture( eGL_TEXDIM /*p_target*/, uint32_t texture )
		{
			m_uiTexture = texture;
			return true;
		}
		virtual bool GenerateMipmap( eGL_TEXDIM p_target );
		virtual bool TexSubImage1D( eGL_TEXDIM p_target, int level, int xoffset, int width, eGL_FORMAT format, eGL_TYPE type, void const * data );
		virtual bool TexSubImage2D( eGL_TEXDIM p_target, int level, int xoffset, int yoffset, int width, int height, eGL_FORMAT format, eGL_TYPE type, void const * data );
		virtual bool TexSubImage2D( eGL_TEXDIM p_target, int level, Castor::Position const & p_position, Castor::Size const & p_size, eGL_FORMAT format, eGL_TYPE type, void const * data );
		virtual bool TexSubImage2D( eGL_TEXDIM p_target, int level, Castor::Rectangle const & p_rect, eGL_FORMAT format, eGL_TYPE type, void const * data );
		virtual bool TexSubImage3D( eGL_TEXDIM p_target, int level, int xoffset, int yoffset, int zoffset, int width, int height, int depth, eGL_FORMAT format, eGL_TYPE type, void const * data );
		virtual bool TexImage1D( eGL_TEXDIM p_target, int level, eGL_INTERNAL internalFormat, int width, int border, eGL_FORMAT format, uint32_t type, void const * data );
		virtual bool TexImage2D( eGL_TEXDIM p_target, int level, eGL_INTERNAL internalFormat, int width, int height, int border, eGL_FORMAT format, uint32_t type, void const * data );
		virtual bool TexImage2D( eGL_TEXDIM p_target, int level, eGL_INTERNAL internalFormat, Castor::Size const & p_size, int border, eGL_FORMAT format, uint32_t type, void const * data );
		virtual bool TexImage3D( eGL_TEXDIM p_target, int level, eGL_INTERNAL internalFormat, int width, int height, int depth, int border, eGL_FORMAT format, uint32_t type, void const * data );
		virtual bool TexParameter( eGL_TEXDIM p_target, eGL_TEXTURE_PARAMETER pname, int param );
		virtual bool TexParameter( eGL_TEXDIM p_target, eGL_TEXTURE_PARAMETER pname, float param );
		virtual bool TexParameter( eGL_TEXDIM p_target, eGL_TEXTURE_PARAMETER pname, const int * params );
		virtual bool TexParameter( eGL_TEXDIM p_target, eGL_TEXTURE_PARAMETER pname, const float * params );
		virtual bool GetTexImage( eGL_TEXDIM p_target, int level, eGL_FORMAT format, eGL_TYPE type, void * img );

		uint32_t m_uiTexture;
#if CASTOR_HAS_VARIADIC_TEMPLATES
		std::function< void( uint32_t texture, uint32_t target, int level, int xoffset, int width, uint32_t format, uint32_t type, void const * data ) > m_pfnTextureSubImage1D;
		std::function< void( uint32_t texture, uint32_t target, int level, int xoffset, int yoffset, int width, int height, uint32_t format, uint32_t type, void const * data ) > m_pfnTextureSubImage2D;
		std::function< void( uint32_t texture, uint32_t target, int level, int xoffset, int yoffset, int zoffset, int width, int height, int depth, uint32_t format, uint32_t type, void const * data ) > m_pfnTextureSubImage3D;
		std::function< void( uint32_t texture, uint32_t target, int level, int internalFormat, int width, int border, uint32_t format, uint32_t type, void const * data ) > m_pfnTextureImage1D;
		std::function< void( uint32_t texture, uint32_t target, int level, int internalFormat, int width, int height, int border, uint32_t format, uint32_t type, void const * data ) > m_pfnTextureImage2D;
		std::function< void( uint32_t texture, uint32_t target, int level, int internalFormat, int width, int height, int depth, int border, uint32_t format, uint32_t type, void const * data ) > m_pfnTextureImage3D;
		std::function< void( uint32_t texture, uint32_t target, int level, uint32_t format, uint32_t type, void * pixels ) > m_pfnGetTextureImage;
#else
		void ( CALLBACK * m_pfnTextureSubImage1D )( uint32_t texture, uint32_t target, int level, int xoffset, int width, uint32_t format, uint32_t type, void const * data );
		void ( CALLBACK * m_pfnTextureSubImage2D )( uint32_t texture, uint32_t target, int level, int xoffset, int yoffset, int width, int height, uint32_t format, uint32_t type, void const * data );
		void ( CALLBACK * m_pfnTextureSubImage3D )( uint32_t texture, uint32_t target, int level, int xoffset, int yoffset, int zoffset, int width, int height, int depth, uint32_t format, uint32_t type, void const * data );
		void ( CALLBACK * m_pfnTextureImage1D )( uint32_t texture, uint32_t target, int level, int internalFormat, int width, int border, uint32_t format, uint32_t type, void const * data );
		void ( CALLBACK * m_pfnTextureImage2D )( uint32_t texture, uint32_t target, int level, int internalFormat, int width, int height, int border, uint32_t format, uint32_t type, void const * data );
		void ( CALLBACK * m_pfnTextureImage3D )( uint32_t texture, uint32_t target, int level, int internalFormat, int width, int height, int depth, int border, uint32_t format, uint32_t type, void const * data );
		void ( CALLBACK * m_pfnGetTextureImage )( uint32_t texture, uint32_t target, int level, uint32_t format, uint32_t type, void * pixels );
#endif
		std::function< void( uint32_t texture, uint32_t target, uint32_t pname, int param ) > m_pfnTextureParameteri;
		std::function< void( uint32_t texture, uint32_t target, uint32_t pname, float param ) > m_pfnTextureParameterf;
		std::function< void( uint32_t texture, uint32_t target, uint32_t pname, const int * params ) > m_pfnTextureParameteriv;
		std::function< void( uint32_t texture, uint32_t target, uint32_t pname, const float * param ) > m_pfnTextureParameterfv;
		std::function< void( uint32_t texture, uint32_t target ) > m_pfnGenerateTextureMipmap;
	};

	class BufFunctionsBase
		: public Holder
	{
	public:
		BufFunctionsBase( OpenGl & p_gl );
		virtual bool EnableClientState( eGL_BUFFER_USAGE p_eArray );
		virtual bool DisableClientState( eGL_BUFFER_USAGE p_eArray );
		virtual bool BindBuffer( eGL_BUFFER_TARGET target, uint32_t buffer ) = 0;
		virtual bool BufferData( eGL_BUFFER_TARGET target, ptrdiff_t size, void const * data, eGL_BUFFER_MODE usage ) = 0;
		virtual bool BufferSubData( eGL_BUFFER_TARGET target, ptrdiff_t offset, ptrdiff_t size, void const * data ) = 0;
		virtual void * MapBuffer( eGL_BUFFER_TARGET target, eGL_LOCK access ) = 0;
		virtual bool UnmapBuffer( eGL_BUFFER_TARGET target ) = 0;
		virtual void * MapBufferRange( eGL_BUFFER_TARGET target, ptrdiff_t offset, ptrdiff_t length, uint32_t access ) = 0;
		virtual bool GetBufferParameter( eGL_BUFFER_TARGET target, eGL_BUFFER_PARAMETER pname, int * params ) = 0;
		virtual bool FlushMappedBufferRange( eGL_BUFFER_TARGET target, ptrdiff_t offset, ptrdiff_t length ) = 0;

		/*@name NV_vertex_buffer_unified_memory extension */
		//@{

		virtual bool BufferAddressRange( eGL_ADDRESS pname, uint32_t index, uint64_t address, size_t length );
		virtual bool VertexFormat( int size, eGL_TYPE type, int stride );
		virtual bool NormalFormat( eGL_TYPE type, int stride );
		virtual bool ColorFormat( int size, eGL_TYPE type, int stride );
		virtual bool IndexFormat( eGL_TYPE type, int stride );
		virtual bool TexCoordFormat( int size, eGL_TYPE type, int stride );
		virtual bool EdgeFlagFormat( int stride );
		virtual bool SecondaryColorFormat( int size, eGL_TYPE type, int stride );
		virtual bool FogCoordFormat( uint32_t type, int stride );
		virtual bool VertexAttribFormat( uint32_t index, int size, eGL_TYPE type, bool normalized, int stride );
		virtual bool VertexAttribIFormat( uint32_t index, int size, eGL_TYPE type, int stride );

		//@}
		/*@name NV_shader_buffer_load extension */
		//@{

		virtual bool MakeBufferResident( eGL_BUFFER_TARGET target, eGL_RESIDENT_BUFFER_ACCESS access );
		virtual bool MakeBufferNonResident( eGL_BUFFER_TARGET target );
		virtual bool IsBufferResident( eGL_BUFFER_TARGET target );
		virtual bool MakeNamedBufferResident( uint32_t buffer, eGL_RESIDENT_BUFFER_ACCESS access );
		virtual bool MakeNamedBufferNonResident( uint32_t buffer );
		virtual bool IsNamedBufferResident( uint32_t buffer );
		virtual bool GetBufferParameter( eGL_BUFFER_TARGET target, eGL_BUFFER_PARAMETER pname, uint64_t * params );
		virtual bool GetNamedBufferParameter( uint32_t buffer, eGL_BUFFER_PARAMETER pname,  uint64_t * params );

		//@}

		std::function< void( uint32_t array ) > m_pfnEnableClientState;
		std::function< void( uint32_t array ) > m_pfnDisableClientState;

		/*@name NV_vertex_buffer_unified_memory extension */
		//@{

		std::function< void( uint32_t pname, uint32_t index, uint64_t address, size_t length ) > m_pfnBufferAddressRange;
		std::function< void( int size, uint32_t type, int stride ) > m_pfnVertexFormat;
		std::function< void( uint32_t type, int stride ) > m_pfnNormalFormat;
		std::function< void( int size, uint32_t type, int stride ) > m_pfnColorFormat;
		std::function< void( uint32_t type, int stride ) > m_pfnIndexFormat;
		std::function< void( int size, uint32_t type, int stride ) > m_pfnTexCoordFormat;
		std::function< void( int stride ) > m_pfnEdgeFlagFormat;
		std::function< void( int size, uint32_t type, int stride ) > m_pfnSecondaryColorFormat;
		std::function< void( uint32_t type, int stride ) > m_pfnFogCoordFormat;
		std::function< void( uint32_t index, int size, uint32_t type, bool normalized, int stride ) > m_pfnVertexAttribFormat;
		std::function< void( uint32_t index, int size, uint32_t type, int stride ) > m_pfnVertexAttribIFormat;

		//@}
		/*@name NV_vertex_buffer_unified_memory extension */
		//@{

		std::function< void( uint32_t target, uint32_t access ) > m_pfnMakeBufferResident;
		std::function< void( uint32_t target ) > m_pfnMakeBufferNonResident;
		std::function< bool( uint32_t target ) > m_pfnIsBufferResident;
		std::function< void( uint32_t buffer, uint32_t access ) > m_pfnMakeNamedBufferResident;
		std::function< void( uint32_t buffer ) > m_pfnMakeNamedBufferNonResident;
		std::function< bool( uint32_t buffer ) > m_pfnIsNamedBufferResident;
		std::function< void( uint32_t target, uint32_t pname, uint64_t * params ) > m_pfnGetBufferParameterui64v;
		std::function< void( uint32_t buffer, uint32_t pname,  uint64_t * params ) > m_pfnGetNamedBufferParameterui64v;

		//@}
	};

	class BufFunctions
		: public BufFunctionsBase
	{
	public:
		BufFunctions( OpenGl & p_gl );
		virtual bool BindBuffer( eGL_BUFFER_TARGET target, uint32_t buffer );
		virtual bool BufferData( eGL_BUFFER_TARGET target, ptrdiff_t size, void const * data, eGL_BUFFER_MODE usage );
		virtual bool BufferSubData( eGL_BUFFER_TARGET target, ptrdiff_t offset, ptrdiff_t size, void const * data );
		virtual void * MapBuffer( eGL_BUFFER_TARGET target, eGL_LOCK access );
		virtual bool UnmapBuffer( eGL_BUFFER_TARGET target );
		virtual void * MapBufferRange( eGL_BUFFER_TARGET target, ptrdiff_t offset, ptrdiff_t length, uint32_t access );
		virtual bool GetBufferParameter( eGL_BUFFER_TARGET target, eGL_BUFFER_PARAMETER pname, int * params );
		virtual bool FlushMappedBufferRange( eGL_BUFFER_TARGET target, ptrdiff_t offset, ptrdiff_t length );

		std::function< void( uint32_t target, uint32_t buffer ) > m_pfnBindBuffer;
		std::function< void( uint32_t target, ptrdiff_t size, void const * data, uint32_t usage ) > m_pfnBufferData;
		std::function< void( uint32_t target, ptrdiff_t offset, ptrdiff_t size, void const * data ) > m_pfnBufferSubData;
		std::function< void * ( uint32_t target, uint32_t access ) > m_pfnMapBuffer;
		std::function< uint8_t( uint32_t target ) > m_pfnUnmapBuffer;
		std::function< void * ( uint32_t target, ptrdiff_t offset, ptrdiff_t length, uint32_t access ) > m_pfnMapBufferRange;
		std::function< void( uint32_t target, ptrdiff_t offset, ptrdiff_t length ) > m_pfnFlushMappedBufferRange;
		std::function< void( uint32_t target, uint32_t pname, int * params ) > m_pfnGetBufferParameteriv;
	};

	class BufFunctionsDSA
		: public BufFunctionsBase
	{
	public:
		BufFunctionsDSA( OpenGl & p_gl );
		virtual bool BindBuffer( eGL_BUFFER_TARGET /*target*/, uint32_t buffer )
		{
			m_uiBuffer = buffer;
			return true;
		}
		virtual bool BufferData( eGL_BUFFER_TARGET target, ptrdiff_t size, void const * data, eGL_BUFFER_MODE usage );
		virtual bool BufferSubData( eGL_BUFFER_TARGET target, ptrdiff_t offset, ptrdiff_t size, void const * data );
		virtual void * MapBuffer( eGL_BUFFER_TARGET target, eGL_LOCK access );
		virtual bool UnmapBuffer( eGL_BUFFER_TARGET target );
		virtual void * MapBufferRange( eGL_BUFFER_TARGET target, ptrdiff_t offset, ptrdiff_t length, uint32_t access );
		virtual bool GetBufferParameter( eGL_BUFFER_TARGET target, eGL_BUFFER_PARAMETER pname, int * params );
		virtual bool FlushMappedBufferRange( eGL_BUFFER_TARGET target, ptrdiff_t offset, ptrdiff_t length );

		uint32_t m_uiBuffer;
		std::function< void( uint32_t buffer, ptrdiff_t size, void const * data, uint32_t usage ) > m_pfnNamedBufferData;
		std::function< void( uint32_t buffer, ptrdiff_t offset, ptrdiff_t size, void const * data ) > m_pfnNamedBufferSubData;
		std::function< void * ( uint32_t buffer, uint32_t access ) > m_pfnMapNamedBuffer;
		std::function< uint8_t ( uint32_t buffer ) > m_pfnUnmapNamedBuffer;
		std::function< void * ( uint32_t buffer, ptrdiff_t offset, ptrdiff_t length, uint32_t access ) > m_pfnMapNamedBufferRange;
		std::function< void( uint32_t buffer, ptrdiff_t offset, ptrdiff_t length ) > m_pfnFlushMappedNamedBufferRange;
		std::function< void( uint32_t buffer, uint32_t pname, int * params ) > m_pfnGetNamedBufferParameteriv;
	};

	class OpenGl
		: public Castor::NonCopyable
	{
	public:
		struct PixelFmt
		{
			eGL_FORMAT Format;
			eGL_INTERNAL Internal;
			eGL_TYPE Type;

			PixelFmt() {}

			PixelFmt( eGL_FORMAT p_eFormat, eGL_INTERNAL p_eInternal, eGL_TYPE p_type )
				: Format( p_eFormat )
				, Internal( p_eInternal )
				, Type( p_type )
			{
			}
		};

		typedef void ( CALLBACK * PFNGLDEBUGPROC )( uint32_t source, uint32_t type, uint32_t id, uint32_t severity, int length, const char * message, void * userParam );
		typedef void ( CALLBACK * PFNGLDEBUGAMDPROC )( uint32_t id, uint32_t category, uint32_t severity, int length, const char * message, void * userParam );

	public:
		C3D_Gl_API OpenGl( GlRenderSystem & p_renderSystem );
		C3D_Gl_API ~OpenGl();

		C3D_Gl_API bool PreInitialise( Castor::String const & p_strExtensions );
		C3D_Gl_API bool Initialise();
		C3D_Gl_API void Cleanup();
		C3D_Gl_API bool HasMultiTexturing()const;

		inline bool HasVao()const
		{
			return m_bHasVao;
		}
		inline bool HasUbo()const
		{
			return m_bHasUbo;
		}
		inline bool HasPbo()const
		{
			return m_bHasPbo;
		}
		inline bool HasTbo()const
		{
			return m_bHasTbo;
		}
		inline bool HasFbo()const
		{
			return m_bHasFbo;
		}
		inline bool HasVSh()const
		{
			return m_bHasVSh;
		}
		inline bool HasPSh()const
		{
			return m_bHasPSh;
		}
		inline bool HasGSh()const
		{
			return m_bHasGSh;
		}
		inline bool HasTSh()const
		{
			return m_bHasTSh;
		}
		inline bool HasCSh()const
		{
			return m_bHasCSh;
		}
		inline bool HasSpl()const
		{
			return m_bHasSpl;
		}
		inline bool HasVbo()const
		{
			return m_bHasVbo;
		}
		inline bool HasDepthClipping()const
		{
			return m_bHasDepthClipping;
		}
		inline bool HasInstancing()const
		{
			return m_bHasInstancedDraw && m_bHasInstancedArrays;
		}
		inline bool HasNonPowerOfTwoTextures()const
		{
			return m_bHasNonPowerOfTwoTextures;
		}
		inline bool CanBindVboToGpuAddress()const
		{
			return m_bBindVboToGpuAddress;
		}

		/**@name General Functions */
		//@{

		C3D_Gl_API uint32_t GetError()const;
		C3D_Gl_API bool ClearColor( float red, float green, float blue, float alpha )const;
		C3D_Gl_API bool ClearColor( Castor::Colour const & p_colour )const;
		C3D_Gl_API bool ShadeModel( eGL_SHADE_MODEL mode )const;
		C3D_Gl_API bool Clear( uint32_t mask )const;
		C3D_Gl_API bool Enable( eGL_TWEAK mode )const;
		C3D_Gl_API bool Disable( eGL_TWEAK mode )const;
		C3D_Gl_API bool Enable( eGL_TEXDIM texture )const;
		C3D_Gl_API bool Disable( eGL_TEXDIM texture )const;
		C3D_Gl_API bool SelectBuffer( int size, uint32_t * buffer )const;
		C3D_Gl_API bool GetIntegerv( uint32_t pname, int * params )const;
		C3D_Gl_API bool GetIntegerv( uint32_t pname, uint64_t * params )const;
		C3D_Gl_API int RenderMode( eGL_RENDER_MODE mode )const;
		C3D_Gl_API bool DepthFunc( eGL_FUNC p_func )const;
		C3D_Gl_API bool DepthMask( bool p_bFlag )const;
		C3D_Gl_API bool ColorMask( bool p_r, bool p_g, bool p_b, bool p_a )const;
		C3D_Gl_API bool DebugMessageCallback( PFNGLDEBUGPROC pfnProc, void * p_pThis )const;
		C3D_Gl_API bool DebugMessageCallback( PFNGLDEBUGAMDPROC pfnProc, void * p_pThis )const;
		C3D_Gl_API bool PolygonMode( eGL_FACE p_eFacing, eGL_FILL_MODE p_mode )const;
		C3D_Gl_API bool StencilOp( eGL_STENCIL_OP p_eStencilFail, eGL_STENCIL_OP p_eDepthFail, eGL_STENCIL_OP p_eStencilPass )const;
		C3D_Gl_API bool StencilFunc( eGL_FUNC p_func, int p_iRef, uint32_t p_uiMask )const;
		C3D_Gl_API bool StencilMask( uint32_t p_uiMask )const;
		C3D_Gl_API bool StencilOpSeparate( eGL_FACE p_eFacing, eGL_STENCIL_OP p_eStencilFail, eGL_STENCIL_OP p_eDepthFail, eGL_STENCIL_OP p_eStencilPass )const;
		C3D_Gl_API bool StencilFuncSeparate( eGL_FACE p_eFacing, eGL_FUNC p_func, int p_iRef, uint32_t p_uiMask )const;
		C3D_Gl_API bool StencilMaskSeparate( eGL_FACE p_eFacing, uint32_t p_uiMask )const;
		C3D_Gl_API bool Hint( eGL_HINT p_eHint, eGL_HINT_VALUE p_eValue )const;
		C3D_Gl_API bool PolygonOffset( float p_fFactor, float p_fUnits )const;
		C3D_Gl_API bool BlendColor( Castor::Colour const & p_clrFactors )const;
		C3D_Gl_API bool SampleCoverage( float fValue, bool invert )const;
		C3D_Gl_API bool Viewport( int x, int y, int width, int height )const;

		//@}
		/**@name Draw Functions */
		//@{

		C3D_Gl_API bool DrawArrays( eGL_PRIMITIVE mode, int first, int count )const;
		C3D_Gl_API bool DrawElements( eGL_PRIMITIVE mode, int count, eGL_TYPE type, const void * indices )const;
		C3D_Gl_API bool DrawArraysInstanced( eGL_PRIMITIVE mode, int first, int count, int primcount )const;
		C3D_Gl_API bool DrawElementsInstanced( eGL_PRIMITIVE mode, int count, eGL_TYPE type, const void * indices, int primcount )const;

		//@}
		/**@name Instanciation Functions */
		//@{

		C3D_Gl_API bool VertexAttribDivisor( uint32_t index, uint32_t divisor )const;

		//@}
		/**@name Context functions */
		//@{

#if defined( _WIN32 )
		C3D_Gl_API bool MakeCurrent( HDC hdc, HGLRC hglrc )const;
		C3D_Gl_API bool SwapBuffers( HDC hdc )const;
		C3D_Gl_API bool SwapInterval( int interval )const;
		C3D_Gl_API HGLRC CreateContext( HDC hdc )const;
		C3D_Gl_API HGLRC CreateContextAttribs( HDC hDC, HGLRC hShareContext, int const * attribList )const;
		C3D_Gl_API bool DeleteContext( HGLRC hContext )const;
		C3D_Gl_API bool HasCreateContextAttribs()const;
#elif defined( __linux__ )
		C3D_Gl_API bool MakeCurrent( Display * pDisplay, GLXDrawable drawable, GLXContext context )const;
		C3D_Gl_API bool SwapBuffers( Display * pDisplay, GLXDrawable drawable )const;
		C3D_Gl_API bool SwapInterval( Display * pDisplay, GLXDrawable drawable, int interval )const;
		C3D_Gl_API GLXContext CreateContext( Display * pDisplay, XVisualInfo * pVisualInfo, GLXContext shareList, Bool direct )const;
		C3D_Gl_API GLXContext CreateContextAttribs( Display * pDisplay, GLXFBConfig fbconfig, GLXContext shareList, Bool direct, int const * attribList )const;
		C3D_Gl_API bool DeleteContext( Display * pDisplay, GLXContext context )const;
		C3D_Gl_API bool HasCreateContextAttribs()const;
#else
#	error "Yet unsupported OS"
#endif

		//@}
		/**@name Material functions */
		//@{

		C3D_Gl_API bool CullFace( eGL_FACE face )const;
		C3D_Gl_API bool FrontFace( eGL_FRONT_FACE_DIRECTION face )const;
		C3D_Gl_API bool Material( eGL_DRAW_BUFFER_MODE face, eGL_MATERIAL_COMPONENT pname, float param )const;
		C3D_Gl_API bool Material( eGL_DRAW_BUFFER_MODE face, eGL_MATERIAL_COMPONENT pname, float const * param )const;
		C3D_Gl_API bool BlendFunc( eGL_BLEND_FACTOR sfactor, eGL_BLEND_FACTOR dfactor )const;
		C3D_Gl_API bool BlendFunc( eGL_BLEND_FACTOR p_eRgbSrc, eGL_BLEND_FACTOR p_eRgbDst, eGL_BLEND_FACTOR p_eAlphaSrc, eGL_BLEND_FACTOR p_eAlphaDst )const;
		C3D_Gl_API bool BlendFunc( uint32_t p_index, eGL_BLEND_FACTOR p_eRgbSrc, eGL_BLEND_FACTOR p_eRgbDst, eGL_BLEND_FACTOR p_eAlphaSrc, eGL_BLEND_FACTOR p_eAlphaDst )const;
		C3D_Gl_API bool BlendEquation( eGL_BLEND_OP p_eOp )const;
		C3D_Gl_API bool BlendEquation( uint32_t p_uiBuffer, eGL_BLEND_OP p_eOp )const;
		C3D_Gl_API bool AlphaFunc( eGL_FUNC func, float ref )const;

		//@}
		/**@name Texture functions */
		//@{

		C3D_Gl_API bool GenTextures( int n, uint32_t * textures )const;
		C3D_Gl_API bool DeleteTextures( int n, uint32_t const * textures )const;
		C3D_Gl_API bool IsTexture( uint32_t texture )const;
		C3D_Gl_API bool BindTexture( eGL_TEXDIM p_target, uint32_t texture )const;
		C3D_Gl_API bool ActiveTexture( eGL_TEXTURE_INDEX target )const;
		C3D_Gl_API bool ClientActiveTexture( eGL_TEXTURE_INDEX target )const;
		C3D_Gl_API bool EnableClientState( eGL_BUFFER_USAGE p_eArray )const;
		C3D_Gl_API bool DisableClientState( eGL_BUFFER_USAGE p_eArray )const;
		C3D_Gl_API bool GenerateMipmap( eGL_TEXDIM p_target )const;
		C3D_Gl_API bool TexSubImage1D( eGL_TEXDIM p_target, int level, int xoffset, int width, eGL_FORMAT format, eGL_TYPE type, void const * data )const;
		C3D_Gl_API bool TexSubImage2D( eGL_TEXDIM p_target, int level, int xoffset, int yoffset, int width, int height, eGL_FORMAT format, eGL_TYPE type, void const * data )const;
		C3D_Gl_API bool TexSubImage2D( eGL_TEXDIM p_target, int level, Castor::Position const & p_position, Castor::Size const & p_size, eGL_FORMAT format, eGL_TYPE type, void const * data )const;
		C3D_Gl_API bool TexSubImage2D( eGL_TEXDIM p_target, int level, Castor::Rectangle const & p_rect, eGL_FORMAT format, eGL_TYPE type, void const * data )const;
		C3D_Gl_API bool TexSubImage3D( eGL_TEXDIM p_target, int level, int xoffset, int yoffset, int zoffset, int width, int height, int depth, eGL_FORMAT format, eGL_TYPE type, void const * data )const;
		C3D_Gl_API bool TexImage1D( eGL_TEXDIM p_target, int level, eGL_INTERNAL internalFormat, int width, int border, eGL_FORMAT format, uint32_t type, void const * data )const;
		C3D_Gl_API bool TexImage2D( eGL_TEXDIM p_target, int level, eGL_INTERNAL internalFormat, int width, int height, int border, eGL_FORMAT format, uint32_t type, void const * data )const;
		C3D_Gl_API bool TexImage2D( eGL_TEXDIM p_target, int level, eGL_INTERNAL internalFormat, Castor::Size const & p_size, int border, eGL_FORMAT format, uint32_t type, void const * data )const;
		C3D_Gl_API bool TexImage3D( eGL_TEXDIM p_target, int level, eGL_INTERNAL internalFormat, int width, int height, int depth, int border, eGL_FORMAT format, uint32_t type, void const * data )const;
		C3D_Gl_API bool TexImage2DMultisample( eGL_TEXDIM p_target, int p_iSamples, eGL_INTERNAL p_eInternalFormat, int p_iWidth, int p_iHeight, bool p_bFixedSampleLocations )const;
		C3D_Gl_API bool TexImage2DMultisample( eGL_TEXDIM p_target, int p_iSamples, eGL_INTERNAL p_eInternalFormat, Castor::Size const & p_size, bool p_bFixedSampleLocations )const;
		C3D_Gl_API bool TexParameter( eGL_TEXDIM p_target, eGL_TEXTURE_PARAMETER pname, int param )const;
		C3D_Gl_API bool TexParameter( eGL_TEXDIM p_target, eGL_TEXTURE_PARAMETER pname, float param )const;
		C3D_Gl_API bool TexParameter( eGL_TEXDIM p_target, eGL_TEXTURE_PARAMETER pname, const int * params )const;
		C3D_Gl_API bool TexParameter( eGL_TEXDIM p_target, eGL_TEXTURE_PARAMETER pname, const float * params )const;
		C3D_Gl_API bool GetTexImage( eGL_TEXDIM p_target, int level, eGL_FORMAT format, eGL_TYPE type, void * img )const;
		C3D_Gl_API bool TexEnvi( eGL_TEXENV_TARGET target, eGL_TEXENV_ARGNAME pname, int param )const;
		C3D_Gl_API bool TexEnviv( eGL_TEXENV_TARGET target, eGL_TEXENV_ARGNAME pname, int const * param )const;
		C3D_Gl_API bool TexEnvf( eGL_TEXENV_TARGET target, eGL_TEXENV_ARGNAME pname, float param )const;
		C3D_Gl_API bool TexEnvfv( eGL_TEXENV_TARGET target, eGL_TEXENV_ARGNAME pname, float const * param )const;
		C3D_Gl_API bool TexGeni( eGL_TEXGEN_COORD coord, eGL_TEXGEN_PARAM param, eGL_TEXGEN_MODE mode )const;
		C3D_Gl_API bool ReadBuffer( eGL_BUFFER p_buffer )const;
		C3D_Gl_API bool ReadPixels( int x, int y, int width, int height, eGL_FORMAT format, eGL_TYPE type, void * pixels )const;
		C3D_Gl_API bool ReadPixels( Castor::Position const & p_position, Castor::Size const & p_size, eGL_FORMAT format, eGL_TYPE type, void * pixels )const;
		C3D_Gl_API bool ReadPixels( Castor::Rectangle const & p_rect, eGL_FORMAT format, eGL_TYPE type, void * pixels )const;
		C3D_Gl_API bool DrawBuffer( eGL_BUFFER p_buffer )const;
		C3D_Gl_API bool DrawPixels( int width, int height, eGL_FORMAT format, eGL_TYPE type, void const * data )const;
		C3D_Gl_API bool PixelStore( eGL_STORAGE_MODE p_mode, int p_iParam )const;
		C3D_Gl_API bool PixelStore( eGL_STORAGE_MODE p_mode, float p_fParam )const;

		//@}
		/**@name Sampler functions */
		//@{

		C3D_Gl_API bool GenSamplers( int count, uint32_t * samplers )const;
		C3D_Gl_API bool DeleteSamplers( int count, const uint32_t * samplers )const;
		C3D_Gl_API bool IsSampler( uint32_t sampler )const;
		C3D_Gl_API bool BindSampler( uint32_t unit, uint32_t sampler )const;
		C3D_Gl_API bool GetSamplerParameter( uint32_t sampler, eGL_SAMPLER_PARAMETER pname, uint32_t * params )const;
		C3D_Gl_API bool GetSamplerParameter( uint32_t sampler, eGL_SAMPLER_PARAMETER pname, float * params )const;
		C3D_Gl_API bool GetSamplerParameter( uint32_t sampler, eGL_SAMPLER_PARAMETER pname, int * params )const;
		C3D_Gl_API bool SetSamplerParameter( uint32_t sampler, eGL_SAMPLER_PARAMETER pname, float param )const;
		C3D_Gl_API bool SetSamplerParameter( uint32_t sampler, eGL_SAMPLER_PARAMETER pname, const float * params )const;
		C3D_Gl_API bool SetSamplerParameter( uint32_t sampler, eGL_SAMPLER_PARAMETER pname, int param )const;
		C3D_Gl_API bool SetSamplerParameter( uint32_t sampler, eGL_SAMPLER_PARAMETER pname, const int * params )const;
		C3D_Gl_API bool SetSamplerParameter( uint32_t sampler, eGL_SAMPLER_PARAMETER pname, const uint32_t * params )const;

		//@}
		/**@name Texture Buffer objects functions */
		//@{

		C3D_Gl_API bool TexBuffer( eGL_TEXDIM p_target, eGL_INTERNAL_FORMAT p_eInternalFormat, uint32_t buffer )const;

		//@}
		/**@name Buffer objects functions */
		//@{

		C3D_Gl_API bool GenBuffers( int n, uint32_t * buffers )const;
		C3D_Gl_API bool DeleteBuffers( int n, uint32_t const * buffers )const;
		C3D_Gl_API bool IsBuffer( uint32_t buffer )const;
		C3D_Gl_API bool BindBuffer( eGL_BUFFER_TARGET target, uint32_t buffer )const;
		C3D_Gl_API bool BufferData( eGL_BUFFER_TARGET target, ptrdiff_t size, void const * data, eGL_BUFFER_MODE usage )const;
		C3D_Gl_API bool BufferSubData( eGL_BUFFER_TARGET target, ptrdiff_t offset, ptrdiff_t size, void const * data )const;
		C3D_Gl_API void * MapBuffer( eGL_BUFFER_TARGET target, eGL_LOCK access )const;
		C3D_Gl_API bool UnmapBuffer( eGL_BUFFER_TARGET target )const;
		C3D_Gl_API void * MapBufferRange( eGL_BUFFER_TARGET target, ptrdiff_t offset, ptrdiff_t length, uint32_t access )const;
		C3D_Gl_API bool GetBufferParameter( eGL_BUFFER_TARGET target, eGL_BUFFER_PARAMETER pname, int * params )const;
		C3D_Gl_API bool GetBufferParameter( eGL_BUFFER_TARGET target, eGL_BUFFER_PARAMETER pname, uint64_t * params )const;
		C3D_Gl_API bool FlushMappedBufferRange( eGL_BUFFER_TARGET target, ptrdiff_t offset, ptrdiff_t length )const;
		C3D_Gl_API bool VertexPointer( int size, uint32_t type, uint32_t stride, void const * pointer )const;
		C3D_Gl_API bool NormalPointer( uint32_t type, uint32_t stride, void const * pointer )const;
		C3D_Gl_API bool TangentPointer( uint32_t type, uint32_t stride, void const * pointer )const;
		C3D_Gl_API bool BinormalPointer( uint32_t type, uint32_t stride, void const * pointer )const;
		C3D_Gl_API bool HasTangentPointer()const;
		C3D_Gl_API bool HasBinormalPointer()const;
		C3D_Gl_API bool ColorPointer( int size, uint32_t type, uint32_t stride, void const * pointer )const;
		C3D_Gl_API bool TexCoordPointer( int size, uint32_t type, uint32_t stride, void const * pointer )const;
		C3D_Gl_API bool BufferAddressRange( eGL_ADDRESS pname, uint32_t index, uint64_t address, size_t length )const;
		C3D_Gl_API bool VertexFormat( int size, eGL_TYPE type, int stride )const;
		C3D_Gl_API bool NormalFormat( eGL_TYPE type, int stride )const;
		C3D_Gl_API bool ColorFormat( int size, eGL_TYPE type, int stride )const;
		C3D_Gl_API bool IndexFormat( eGL_TYPE type, int stride )const;
		C3D_Gl_API bool TexCoordFormat( int size, eGL_TYPE type, int stride )const;
		C3D_Gl_API bool EdgeFlagFormat( int stride )const;
		C3D_Gl_API bool SecondaryColorFormat( int size, eGL_TYPE type, int stride )const;
		C3D_Gl_API bool FogCoordFormat( uint32_t type, int stride )const;
		C3D_Gl_API bool VertexAttribFormat( uint32_t index, int size, eGL_TYPE type, bool normalized, int stride )const;
		C3D_Gl_API bool VertexAttribIFormat( uint32_t index, int size, eGL_TYPE type, int stride )const;
		C3D_Gl_API bool MakeBufferResident( eGL_BUFFER_TARGET target, eGL_RESIDENT_BUFFER_ACCESS access )const;
		C3D_Gl_API bool MakeBufferNonResident( eGL_BUFFER_TARGET target )const;
		C3D_Gl_API bool IsBufferResident( eGL_BUFFER_TARGET target )const;
		C3D_Gl_API bool MakeNamedBufferResident( uint32_t buffer, eGL_RESIDENT_BUFFER_ACCESS access )const;
		C3D_Gl_API bool MakeNamedBufferNonResident( uint32_t buffer )const;
		C3D_Gl_API bool IsNamedBufferResident( uint32_t buffer )const;
		C3D_Gl_API bool GetNamedBufferParameter( uint32_t buffer, eGL_BUFFER_PARAMETER pname, uint64_t * params )const;

		//@}
		/**@name FBO functions */
		//@{

		/** see https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glGenFramebuffers.xml
		*/
		C3D_Gl_API bool GenFramebuffers( int n, uint32_t * framebuffers )const;

		/** see https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glDeleteFramebuffers.xml
		*/
		C3D_Gl_API bool DeleteFramebuffers( int n, uint32_t const * framebuffers )const;

		/** see https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glIsFramebuffer.xml
		*/
		C3D_Gl_API bool IsFramebuffer( uint32_t framebuffer )const;

		/** see https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glBindFramebuffer.xml
		*/
		C3D_Gl_API bool BindFramebuffer( eGL_FRAMEBUFFER_MODE p_eBindingMode, uint32_t framebuffer )const;

		/** see https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glFramebufferTexture.xml
		*/
		C3D_Gl_API bool FramebufferTexture( eGL_FRAMEBUFFER_MODE p_eBindingMode, eGL_TEXTURE_ATTACHMENT p_eAttachment, uint32_t texture, int level )const;

		/** see https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glFramebufferTexture.xml
		*/
		C3D_Gl_API bool FramebufferTexture1D( eGL_FRAMEBUFFER_MODE p_eBindingMode, eGL_TEXTURE_ATTACHMENT p_eAttachment, eGL_TEXDIM textarget, uint32_t texture, int level )const;

		/** see https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glFramebufferTexture.xml
		*/
		C3D_Gl_API bool FramebufferTexture2D( eGL_FRAMEBUFFER_MODE p_eBindingMode, eGL_TEXTURE_ATTACHMENT p_eAttachment, eGL_TEXDIM textarget, uint32_t texture, int level )const;

		/** see https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glFramebufferTexture.xml
		*/
		C3D_Gl_API bool FramebufferTexture3D( eGL_FRAMEBUFFER_MODE p_eBindingMode, eGL_TEXTURE_ATTACHMENT p_eAttachment, eGL_TEXDIM textarget, uint32_t texture, int level, int layer )const;

		/** see https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glFramebufferTextureLayer.xml
		*/
		C3D_Gl_API bool FramebufferTextureLayer( eGL_FRAMEBUFFER_MODE p_eBindingMode, eGL_TEXTURE_ATTACHMENT p_eAttachment, uint32_t texture, int level, int layer )const;

		/** see https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glFramebufferRenderbuffer.xml
		*/
		C3D_Gl_API bool FramebufferRenderbuffer( eGL_FRAMEBUFFER_MODE p_eBindingMode, eGL_RENDERBUFFER_ATTACHMENT p_eAttachment, eGL_RENDERBUFFER_MODE p_eRboTarget, uint32_t renderbufferId )const;

		/** see https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glCheckFramebufferStatus.xml
		*/
		C3D_Gl_API uint32_t CheckFramebufferStatus( eGL_FRAMEBUFFER_MODE p_eBindingMode )const;

		/** see https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glGenRenderbuffers.xml
		*/
		C3D_Gl_API bool GenRenderbuffers( int n, uint32_t * ids )const;

		/** see https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glDeleteRenderbuffers.xml
		*/
		C3D_Gl_API bool DeleteRenderbuffers( int n, uint32_t const * ids )const;

		/** see https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glIsFramebuffer.xml
		*/
		C3D_Gl_API bool IsRenderbuffer( uint32_t renderbuffer )const;

		/** see https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glBindRenderbuffer.xml
		*/
		C3D_Gl_API bool BindRenderbuffer( eGL_RENDERBUFFER_MODE p_eBindingMode, uint32_t id )const;
		C3D_Gl_API bool RenderbufferStorage( eGL_RENDERBUFFER_MODE p_eBindingMode, eGL_RENDERBUFFER_STORAGE internalFormat, int width, int height )const;
		C3D_Gl_API bool RenderbufferStorageMultisample( eGL_RENDERBUFFER_MODE p_eBindingMode, int p_iSamples, eGL_RENDERBUFFER_STORAGE internalFormat, int width, int height )const;
		C3D_Gl_API bool RenderbufferStorage( eGL_RENDERBUFFER_MODE p_eBindingMode, eGL_RENDERBUFFER_STORAGE internalFormat, Castor::Size const & size )const;
		C3D_Gl_API bool RenderbufferStorageMultisample( eGL_RENDERBUFFER_MODE p_eBindingMode, int p_iSamples, eGL_RENDERBUFFER_STORAGE internalFormat, Castor::Size const & size )const;
		C3D_Gl_API bool GetRenderbufferParameteriv( eGL_RENDERBUFFER_MODE p_eBindingMode, eGL_RENDERBUFFER_PARAMETER param, int * value )const;
		C3D_Gl_API bool BlitFramebuffer( int srcX0, int srcY0, int srcX1, int srcY1, int dstX0, int dstY0, int dstX1, int dstY1, uint32_t mask, uint32_t filter )const;
		C3D_Gl_API bool BlitFramebuffer( Castor::Rectangle const & rcSrc, Castor::Rectangle const & rcDst, uint32_t mask, uint32_t filter )const;
		C3D_Gl_API bool DrawBuffers( int n, const uint32_t * bufs )const;

		//@}
		/**@name Uniform variable Functions */
		//@{

		C3D_Gl_API int GetUniformLocation( uint32_t program, char const * name )const;
		C3D_Gl_API bool SetUniform( int location, int v0 )const;
		C3D_Gl_API bool SetUniform( int location, uint32_t v0 )const;
		C3D_Gl_API bool SetUniform( int location, float v0 )const;
		C3D_Gl_API bool SetUniform( int location, double v0 )const;
		C3D_Gl_API bool SetUniform( int location, int v0, int v1 )const;
		C3D_Gl_API bool SetUniform( int location, int v0, int v1, int v2 )const;
		C3D_Gl_API bool SetUniform( int location, int v0, int v1, int v2, int v3 )const;
		C3D_Gl_API bool SetUniform( int location, uint32_t v0, uint32_t v1 )const;
		C3D_Gl_API bool SetUniform( int location, uint32_t v0, uint32_t v1, uint32_t v2 )const;
		C3D_Gl_API bool SetUniform( int location, uint32_t v0, uint32_t v1, uint32_t v2, uint32_t v3 )const;
		C3D_Gl_API bool SetUniform( int location, float v0, float v1 )const;
		C3D_Gl_API bool SetUniform( int location, float v0, float v1, float v2 )const;
		C3D_Gl_API bool SetUniform( int location, float v0, float v1, float v2, float v3 )const;
		C3D_Gl_API bool SetUniform( int location, double v0, double v1 )const;
		C3D_Gl_API bool SetUniform( int location, double v0, double v1, double v2 )const;
		C3D_Gl_API bool SetUniform( int location, double v0, double v1, double v2, double v3 )const;
		C3D_Gl_API bool SetUniform1v( int location, int count, int const * params )const;
		C3D_Gl_API bool SetUniform2v( int location, int count, int const * params )const;
		C3D_Gl_API bool SetUniform3v( int location, int count, int const * params )const;
		C3D_Gl_API bool SetUniform4v( int location, int count, int const * params )const;
		C3D_Gl_API bool SetUniform1v( int location, int count, uint32_t const * params )const;
		C3D_Gl_API bool SetUniform2v( int location, int count, uint32_t const * params )const;
		C3D_Gl_API bool SetUniform3v( int location, int count, uint32_t const * params )const;
		C3D_Gl_API bool SetUniform4v( int location, int count, uint32_t const * params )const;
		C3D_Gl_API bool SetUniform1v( int location, int count, float const * params )const;
		C3D_Gl_API bool SetUniform2v( int location, int count, float const * params )const;
		C3D_Gl_API bool SetUniform3v( int location, int count, float const * params )const;
		C3D_Gl_API bool SetUniform4v( int location, int count, float const * params )const;
		C3D_Gl_API bool SetUniform1v( int location, int count, double const * params )const;
		C3D_Gl_API bool SetUniform2v( int location, int count, double const * params )const;
		C3D_Gl_API bool SetUniform3v( int location, int count, double const * params )const;
		C3D_Gl_API bool SetUniform4v( int location, int count, double const * params )const;
		C3D_Gl_API bool SetUniformMatrix2x2v( int location, int count, bool transpose, float const * value )const;
		C3D_Gl_API bool SetUniformMatrix2x3v( int location, int count, bool transpose, float const * value )const;
		C3D_Gl_API bool SetUniformMatrix2x4v( int location, int count, bool transpose, float const * value )const;
		C3D_Gl_API bool SetUniformMatrix3x3v( int location, int count, bool transpose, float const * value )const;
		C3D_Gl_API bool SetUniformMatrix3x2v( int location, int count, bool transpose, float const * value )const;
		C3D_Gl_API bool SetUniformMatrix3x4v( int location, int count, bool transpose, float const * value )const;
		C3D_Gl_API bool SetUniformMatrix4x4v( int location, int count, bool transpose, float const * value )const;
		C3D_Gl_API bool SetUniformMatrix4x2v( int location, int count, bool transpose, float const * value )const;
		C3D_Gl_API bool SetUniformMatrix4x3v( int location, int count, bool transpose, float const * value )const;
		C3D_Gl_API bool SetUniformMatrix2x2v( int location, int count, bool transpose, double const * value )const;
		C3D_Gl_API bool SetUniformMatrix2x3v( int location, int count, bool transpose, double const * value )const;
		C3D_Gl_API bool SetUniformMatrix2x4v( int location, int count, bool transpose, double const * value )const;
		C3D_Gl_API bool SetUniformMatrix3x3v( int location, int count, bool transpose, double const * value )const;
		C3D_Gl_API bool SetUniformMatrix3x2v( int location, int count, bool transpose, double const * value )const;
		C3D_Gl_API bool SetUniformMatrix3x4v( int location, int count, bool transpose, double const * value )const;
		C3D_Gl_API bool SetUniformMatrix4x4v( int location, int count, bool transpose, double const * value )const;
		C3D_Gl_API bool SetUniformMatrix4x2v( int location, int count, bool transpose, double const * value )const;
		C3D_Gl_API bool SetUniformMatrix4x3v( int location, int count, bool transpose, double const * value )const;

		//@}
		/**@name Uniform Buffer Objects Functions */
		//@{

		C3D_Gl_API uint32_t GetUniformBlockIndex( uint32_t program, char const * uniformBlockName )const;
		C3D_Gl_API bool BindBufferBase( uint32_t target, uint32_t index, uint32_t buffer )const;
		C3D_Gl_API bool UniformBlockBinding( uint32_t program, uint32_t uniformBlockIndex, uint32_t uniformBlockBinding )const;
		C3D_Gl_API bool GetUniformIndices( uint32_t program, int uniformCount, char const ** uniformNames, uint32_t * uniformIndices )const;
		C3D_Gl_API bool GetActiveUniformsiv( uint32_t program, int uniformCount, uint32_t const * uniformIndices, eGL_UNIFORM_NAME pname, int * params )const;
		C3D_Gl_API bool GetActiveUniformBlockiv( uint32_t program, uint32_t uniformBlockIndex, eGL_UNIFORM_NAME pname, int * params )const;

		//@}
		/**@name Shader object Functions */
		//@{

		C3D_Gl_API uint32_t CreateShader( eGL_SHADER_TYPE type )const;
		C3D_Gl_API bool DeleteShader( uint32_t shader )const;
		C3D_Gl_API bool IsShader( uint32_t shader )const;
		C3D_Gl_API bool AttachShader( uint32_t program, uint32_t shader )const;
		C3D_Gl_API bool DetachShader( uint32_t program, uint32_t shader )const;
		C3D_Gl_API bool CompileShader( uint32_t shader )const;
		C3D_Gl_API bool GetShaderiv( uint32_t shader, uint32_t pname, int * param )const;
		C3D_Gl_API bool GetShaderInfoLog( uint32_t shader, int bufSize, int * length, char * infoLog )const;
		C3D_Gl_API bool ShaderSource( uint32_t shader, int count, char const ** strings, int const * lengths )const;

		//@}
		/**@name Shader program Functions */
		//@{

		C3D_Gl_API uint32_t CreateProgram()const;
		C3D_Gl_API bool DeleteProgram( uint32_t program )const;
		C3D_Gl_API bool LinkProgram( uint32_t program )const;
		C3D_Gl_API bool UseProgram( uint32_t program )const;
		C3D_Gl_API bool GetProgramiv( uint32_t program, uint32_t pname, int * param )const;
		C3D_Gl_API bool GetProgramInfoLog( uint32_t program, int bufSize, int * length, char * infoLog )const;
		C3D_Gl_API int GetAttribLocation( uint32_t program, char const * name )const;
		C3D_Gl_API bool IsProgram( uint32_t program )const;
		C3D_Gl_API bool ProgramParameteri( uint32_t program, uint32_t pname, int value )const;

		//@}
		/**@name Vertex Attribute Pointer functions */
		//@{

		C3D_Gl_API bool EnableVertexAttribArray( uint32_t index )const;
		C3D_Gl_API bool VertexAttribPointer( uint32_t index, int size, eGL_TYPE type, bool normalized, int stride, void const * pointer )const;
		C3D_Gl_API bool VertexAttribPointer( uint32_t index, int size, eGL_TYPE type, int stride, void const * pointer )const;
		C3D_Gl_API bool DisableVertexAttribArray( uint32_t index )const;

		//@}
		/**@name Vertex Array Objects */
		//@{

		C3D_Gl_API bool GenVertexArrays( int n, uint32_t * arrays )const;
		C3D_Gl_API bool IsVertexArray( uint32_t array )const;
		C3D_Gl_API bool BindVertexArray( uint32_t array )const;
		C3D_Gl_API bool DeleteVertexArrays( int n, uint32_t const * arrays )const;

		//@}
		/**@name Tesselation functions */
		//@{

		C3D_Gl_API bool PatchParameter( eGL_PATCH_PARAMETER p_eParam, int p_iValue )const;

		//@}
		/**@name Query functions */
		//@{

		C3D_Gl_API bool GenQueries( int p_n, uint32_t * p_queries )const;
		C3D_Gl_API bool DeleteQueries( int p_n, uint32_t const * p_queries )const;
		C3D_Gl_API bool IsQuery( uint32_t p_query )const;
		C3D_Gl_API bool BeginQuery( eGL_QUERY p_target, uint32_t p_query )const;
		C3D_Gl_API bool EndQuery( eGL_QUERY p_target )const;
		C3D_Gl_API bool QueryCounter( uint32_t p_id, eGL_QUERY p_target )const;
		C3D_Gl_API bool GetQueryObjectInfos( uint32_t p_id, eGL_QUERY_INFO p_name, int32_t * p_params )const;
		C3D_Gl_API bool GetQueryObjectInfos( uint32_t p_id, eGL_QUERY_INFO p_name, uint32_t * p_params )const;
		C3D_Gl_API bool GetQueryObjectInfos( uint32_t p_id, eGL_QUERY_INFO p_name, int64_t * p_params )const;
		C3D_Gl_API bool GetQueryObjectInfos( uint32_t p_id, eGL_QUERY_INFO p_name, uint64_t * p_params )const;

		//@}
		/**@name Other functions */
		//@{

		C3D_Gl_API bool GlCheckError( std::string const & p_strText )const;
		C3D_Gl_API bool GlCheckError( std::wstring const & p_strText )const;
		C3D_Gl_API eGL_LOCK GetLockFlags( uint32_t p_flags )const;
		C3D_Gl_API uint32_t GetBitfieldFlags( uint32_t p_flags )const;

#if !defined( NDEBUG )

		C3D_Gl_API void Track( void * p_object, std::string const & p_name, std::string const & p_file, int line )const;
		C3D_Gl_API void UnTrack( void * p_object )const;

		template< typename CreaionFunction, typename DestructionFunction >
		void Track( Object< CreaionFunction, DestructionFunction > * p_object, std::string const & p_name, std::string const & p_file, int p_line )const
		{
			Track( reinterpret_cast< void * >( p_object ), p_name + cuT( " (OpenGL Name: " ) + Castor::string::to_string( p_object->GetGlName() ) + cuT( ")" ), p_file, p_line );
		}

#endif

		Castor::String GetGlslErrorString( int p_index )const
		{
			return GlslStrings[p_index];
		}
		inline eGL_PRIMITIVE Get( Castor3D::eTOPOLOGY p_index )const
		{
			return PrimitiveTypes[p_index];
		}
		inline eGL_TEXDIM Get( Castor3D::eTEXTURE_TYPE p_index )const
		{
			if ( p_index == Castor3D::eTEXTURE_TYPE_BUFFER )
			{
				if ( true )//!HasTbo() )
				{
					p_index = Castor3D::eTEXTURE_TYPE_1D;
				}
			}

			return TextureDimensions[p_index];
		}
		inline eGL_FUNC Get( Castor3D::eALPHA_FUNC p_eAlphaFunc )const
		{
			return AlphaFuncs[p_eAlphaFunc];
		}
		inline eGL_WRAP_MODE Get( Castor3D::eWRAP_MODE p_eWrapMode )const
		{
			return TextureWrapMode[p_eWrapMode];
		}
		inline eGL_INTERPOLATION_MODE Get( Castor3D::eINTERPOLATION_MODE p_eInterpolationMode )const
		{
			return TextureInterpolation[p_eInterpolationMode];
		}
		inline int Get( Castor3D::eBLEND_SOURCE p_eArgument )const
		{
			return TextureArguments[p_eArgument];
		}
		inline eGL_BLEND_FUNC Get( Castor3D::eRGB_BLEND_FUNC p_mode )const
		{
			return RgbBlendFuncs[p_mode];
		}
		inline eGL_BLEND_FUNC Get( Castor3D::eALPHA_BLEND_FUNC p_mode )const
		{
			return AlphaBlendFuncs[p_mode];
		}
		inline eGL_BLEND_FACTOR Get( Castor3D::eBLEND p_eBlendFactor )const
		{
			return BlendFactors[p_eBlendFactor];
		}
		inline PixelFmt Get( Castor::ePIXEL_FORMAT p_pixelFormat )const
		{
			return PixelFormats[p_pixelFormat];
		}
		inline eGL_BUFFER_USAGE Get( Castor3D::eELEMENT_USAGE p_eUsage )const
		{
			return Usages[p_eUsage];
		}
		inline eGL_SHADER_TYPE Get( Castor3D::eSHADER_TYPE p_type )const
		{
			return ShaderTypes[p_type];
		}
		inline eGL_INTERNAL_FORMAT GetInternal( Castor::ePIXEL_FORMAT p_eFormat )const
		{
			return Internals[p_eFormat];
		}
		inline uint32_t GetComponents( uint32_t p_uiComponents )const
		{
			return ( p_uiComponents & Castor3D::eBUFFER_COMPONENT_COLOUR ? eGL_BUFFER_BIT_COLOR : 0 ) | ( p_uiComponents & Castor3D::eBUFFER_COMPONENT_DEPTH ? eGL_BUFFER_BIT_DEPTH : 0 ) | ( p_uiComponents & Castor3D::eBUFFER_COMPONENT_STENCIL ? eGL_BUFFER_BIT_STENCIL : 0 );
		}
		inline eGL_TEXTURE_ATTACHMENT Get( Castor3D::eATTACHMENT_POINT p_eAttachment )const
		{
			return Attachments[p_eAttachment];
		}
		inline eGL_FRAMEBUFFER_MODE Get( Castor3D::eFRAMEBUFFER_TARGET p_target )const
		{
			return FramebufferModes[p_target];
		}
		inline eGL_RENDERBUFFER_ATTACHMENT GetRboAttachment( Castor3D::eATTACHMENT_POINT p_eAttachment )const
		{
			return RboAttachments[p_eAttachment];
		}
		inline eGL_RENDERBUFFER_STORAGE GetRboStorage( Castor::ePIXEL_FORMAT p_pixelFormat )const
		{
			return RboStorages[p_pixelFormat];
		}
		inline eGL_TWEAK Get( Castor3D::eTWEAK p_eTweak )const
		{
			return Tweaks[p_eTweak];
		}
		inline eGL_BUFFER Get( Castor3D::eBUFFER p_buffer )const
		{
			return Buffers[p_buffer];
		}
		inline eGL_BUFFER Get( eGL_TEXTURE_ATTACHMENT p_buffer )
		{
			return BuffersTA[p_buffer];
		}
		inline eGL_BUFFER Get( eGL_RENDERBUFFER_ATTACHMENT p_buffer )
		{
			return BuffersRBA[p_buffer];
		}
		inline eGL_FACE Get( Castor3D::eFACE p_eFace )const
		{
			return Faces[p_eFace];
		}
		inline eGL_FILL_MODE Get( Castor3D::eFILL_MODE p_mode )const
		{
			return FillModes[p_mode];
		}
		inline eGL_FUNC Get( Castor3D::eSTENCIL_FUNC p_func )const
		{
			return StencilFuncs[p_func];
		}
		inline eGL_STENCIL_OP Get( Castor3D::eSTENCIL_OP p_eOp )const
		{
			return StencilOps[p_eOp];
		}
		inline eGL_BLEND_OP Get( Castor3D::eBLEND_OP p_eOp )const
		{
			return BlendOps[p_eOp];
		}
		inline eGL_FUNC Get( Castor3D::eDEPTH_FUNC p_func )const
		{
			return DepthFuncs[p_func];
		}
		inline bool Get( Castor3D::eWRITING_MASK p_eMask )const
		{
			return WriteMasks[p_eMask];
		}
		inline bool HasDebugOutput()const
		{
			return ( m_pfnDebugMessageCallback || m_pfnDebugMessageCallbackAMD );
		}
		inline Castor::String const & GetVendor()const
		{
			return m_vendor;
		}
		inline Castor::String const & GetRenderer()const
		{
			return m_renderer;
		}
		inline Castor::String const & GetStrVersion()const
		{
			return m_version;
		}
		inline int GetVersion()const
		{
			return m_iVersion;
		}
		inline int GetGlslVersion()const
		{
			return m_iGlslVersion;
		}
		C3D_Gl_API bool HasExtension( Castor::String const & p_strExtName )const;
		inline eGL_BUFFER_MODE GetBufferFlags( uint32_t p_flags )const
		{
			eGL_BUFFER_MODE l_eReturn = eGL_BUFFER_MODE( 0 );

			if ( p_flags & Castor3D::eBUFFER_ACCESS_TYPE_DYNAMIC )
			{
				if ( p_flags & Castor3D::eBUFFER_ACCESS_NATURE_READ )
				{
					l_eReturn = eGL_BUFFER_MODE_DYNAMIC_READ;
				}
				else if ( p_flags & Castor3D::eBUFFER_ACCESS_NATURE_DRAW )
				{
					l_eReturn = eGL_BUFFER_MODE_DYNAMIC_DRAW;
				}
				else if ( p_flags & Castor3D::eBUFFER_ACCESS_NATURE_COPY )
				{
					l_eReturn = eGL_BUFFER_MODE_DYNAMIC_COPY;
				}
			}
			else if ( p_flags & Castor3D::eBUFFER_ACCESS_TYPE_STATIC )
			{
				if ( p_flags & Castor3D::eBUFFER_ACCESS_NATURE_READ )
				{
					l_eReturn = eGL_BUFFER_MODE_STATIC_READ;
				}
				else if ( p_flags & Castor3D::eBUFFER_ACCESS_NATURE_DRAW )
				{
					l_eReturn = eGL_BUFFER_MODE_STATIC_DRAW;
				}
				else if ( p_flags & Castor3D::eBUFFER_ACCESS_NATURE_COPY )
				{
					l_eReturn = eGL_BUFFER_MODE_STATIC_COPY;
				}
			}
			else if ( p_flags & Castor3D::eBUFFER_ACCESS_TYPE_STREAM )
			{
				if ( p_flags & Castor3D::eBUFFER_ACCESS_NATURE_READ )
				{
					l_eReturn = eGL_BUFFER_MODE_STREAM_READ;
				}
				else if ( p_flags & Castor3D::eBUFFER_ACCESS_NATURE_DRAW )
				{
					l_eReturn = eGL_BUFFER_MODE_STREAM_DRAW;
				}
				else if ( p_flags & Castor3D::eBUFFER_ACCESS_NATURE_COPY )
				{
					l_eReturn = eGL_BUFFER_MODE_STREAM_COPY;
				}
			}

			return l_eReturn;
		}
		//@}

		C3D_Gl_API static void CALLBACK StDebugLog( eGL_DEBUG_SOURCE source, eGL_DEBUG_TYPE type, uint32_t id, eGL_DEBUG_SEVERITY severity, int length, const char * message, void * userParam );
		C3D_Gl_API static void CALLBACK StDebugLogAMD( uint32_t id, eGL_DEBUG_CATEGORY category, eGL_DEBUG_SEVERITY severity, int length, const char * message, void * userParam );

	private:
		bool DoGlCheckError( Castor::String const & p_strText )const;
		void DebugLog( eGL_DEBUG_SOURCE source, eGL_DEBUG_TYPE type, uint32_t id, eGL_DEBUG_SEVERITY severity, int length, const char * message )const;
		void DebugLogAMD( uint32_t id, eGL_DEBUG_CATEGORY category, eGL_DEBUG_SEVERITY severity, int length, const char * message )const;

	private:
		Castor::String GlslStrings[8];
		Castor::String GlslErrors[8];
		eGL_PRIMITIVE PrimitiveTypes[Castor3D::eTOPOLOGY_COUNT];
		eGL_TEXDIM TextureDimensions[Castor3D::eTEXTURE_TYPE_COUNT];
		eGL_FUNC AlphaFuncs[Castor3D::eALPHA_FUNC_COUNT];
		eGL_WRAP_MODE TextureWrapMode[Castor3D::eWRAP_MODE_COUNT];
		eGL_INTERPOLATION_MODE TextureInterpolation[Castor3D::eINTERPOLATION_MODE_COUNT];
		eGL_BLEND_FACTOR BlendFactors[Castor3D::eBLEND_COUNT];
		eGL_BUFFER_USAGE Usages[Castor3D::eELEMENT_USAGE_COUNT];
		eGL_BLEND_SOURCE TextureArguments[Castor3D::eBLEND_SOURCE_COUNT];
		eGL_BLEND_FUNC RgbBlendFuncs[Castor3D::eRGB_BLEND_FUNC_COUNT];
		eGL_BLEND_FUNC AlphaBlendFuncs[Castor3D::eALPHA_BLEND_FUNC_COUNT];
		eGL_BLEND_OP BlendOps[Castor3D::eBLEND_OP_COUNT];
		PixelFmt PixelFormats[Castor::ePIXEL_FORMAT_COUNT];
		eGL_SHADER_TYPE ShaderTypes[Castor3D::eSHADER_TYPE_COUNT];
		eGL_INTERNAL_FORMAT Internals[Castor::ePIXEL_FORMAT_COUNT];
		eGL_TEXTURE_ATTACHMENT Attachments[Castor3D::eATTACHMENT_POINT_COUNT];
		eGL_FRAMEBUFFER_MODE FramebufferModes[Castor3D::eFRAMEBUFFER_MODE_COUNT];
		eGL_RENDERBUFFER_ATTACHMENT RboAttachments[Castor3D::eATTACHMENT_POINT_COUNT];
		eGL_RENDERBUFFER_STORAGE RboStorages[Castor::ePIXEL_FORMAT_COUNT];
		eGL_TWEAK Tweaks[Castor3D::eTWEAK_COUNT];
		eGL_BUFFER Buffers[Castor3D::eBUFFER_COUNT];
		eGL_FACE Faces[Castor3D::eFACE_COUNT];
		eGL_FILL_MODE FillModes[3];
		eGL_STENCIL_OP StencilOps[Castor3D::eSTENCIL_OP_COUNT];
		eGL_FUNC StencilFuncs[Castor3D::eSTENCIL_FUNC_COUNT];
		bool WriteMasks[Castor3D::eWRITING_MASK_COUNT];
		eGL_FUNC DepthFuncs[Castor3D::eDEPTH_FUNC_COUNT];
		std::map< eGL_TEXTURE_ATTACHMENT, eGL_BUFFER > BuffersTA;
		std::map< eGL_RENDERBUFFER_ATTACHMENT, eGL_BUFFER > BuffersRBA;

		bool m_bHasVao;
		bool m_bHasUbo;
		bool m_bHasPbo;
		bool m_bHasTbo;
		bool m_bHasFbo;
		bool m_bHasVbo;
		bool m_bHasVSh;
		bool m_bHasPSh;
		bool m_bHasGSh;
		bool m_bHasTSh;
		bool m_bHasCSh;
		bool m_bHasSpl;
		bool m_bHasDepthClipping;
		bool m_bHasAnisotropic;
		bool m_bBindVboToGpuAddress;
		Castor::String m_extensions;
		Castor::String m_vendor;
		Castor::String m_renderer;
		Castor::String m_version;
		int m_iVersion;
		int m_iGlslVersion;
		bool m_bHasInstancedDraw;
		bool m_bHasInstancedArrays;
		bool m_bHasDirectStateAccess;
		bool m_bHasNonPowerOfTwoTextures;
		TexFunctionsBase * m_pTexFunctions;
		BufFunctionsBase * m_pBufFunctions;
		GlRenderSystem & m_renderSystem;

		/**@name General */
		//@{

		std::function< uint32_t() > m_pfnGetError;
		std::function< void( float red, float green, float blue, float alpha ) > m_pfnClearColor;
		std::function< void( uint32_t mode ) > m_pfnShadeModel;
		std::function< void( uint32_t mask ) > m_pfnClear;
		std::function< void( uint32_t mode ) > m_pfnEnable;
		std::function< void( uint32_t mode ) > m_pfnDisable;
		std::function< void( int size, uint32_t * buffer ) > m_pfnSelectBuffer;
		std::function< void( uint32_t pname, int * params ) > m_pfnGetIntegerv;
		std::function< int( uint32_t mode ) > m_pfnRenderMode;
		std::function< void( PFNGLDEBUGPROC callback, void * userParam ) > m_pfnDebugMessageCallback;
		std::function< void( PFNGLDEBUGAMDPROC callback, void * userParam ) > m_pfnDebugMessageCallbackAMD;

		//@}
		/**@name Depth stencil state */
		//@{

		std::function< void( uint32_t func ) >m_pfnDepthFunc;
		std::function< void( uint8_t flag ) > m_pfnDepthMask;
		std::function< void( uint8_t r, uint8_t g, uint8_t b, uint8_t a ) > m_pfnColorMask;
		std::function< void( uint32_t sfail, uint32_t dpfail, uint32_t dppass ) > m_pfnStencilOp;
		std::function< void( uint32_t func, int ref, uint32_t mask ) > m_pfnStencilFunc;
		std::function< void( uint32_t mask ) > m_pfnStencilMask;
		std::function< void( uint32_t face, uint32_t sfail, uint32_t dpfail, uint32_t dppass ) > m_pfnStencilOpSeparate;
		std::function< void( uint32_t frontFunc, uint32_t backFunc, int ref, uint32_t mask ) > m_pfnStencilFuncSeparate;
		std::function< void( uint32_t face, uint32_t mask ) > m_pfnStencilMaskSeparate;

		//@}
		/**@name Rasterizer state */
		//@{

		std::function< void( uint32_t face, uint32_t mode ) > m_pfnPolygonMode;
		std::function< void( uint32_t face ) > m_pfnCullFace;
		std::function< void( uint32_t face ) > m_pfnFrontFace;
		std::function< void( uint32_t target, uint32_t mode ) > m_pfnHint;
		std::function< void( float factor, float units ) >m_pfnPolygonOffset;

		//@}
		/**@name Blend state */
		//@{

		std::function< void( float red, float green, float blue, float alpha ) > m_pfnBlendColor;
		std::function< void( uint32_t srcRGB, uint32_t dstRGB, uint32_t srcAlpha, uint32_t dstAlpha ) > m_pfnBlendFuncSeparate;
		std::function< void( uint32_t buf, uint32_t srcRGB, uint32_t dstRGB, uint32_t srcAlpha, uint32_t dstAlpha ) > m_pfnBlendFuncSeparatei;
		std::function< void( uint32_t mode ) > m_pfnBlendEquation;
		std::function< void( uint32_t buf, uint32_t mode ) > m_pfnBlendEquationi;
		std::function< void( float value, uint8_t invert ) > m_pfnSampleCoverage;

		//@}
		/**@name Buffer rendering */
		//@{

		std::function< void( uint32_t mode, int first, int count ) > m_pfnDrawArrays;
		std::function< void( uint32_t mode, int count, uint32_t type, void const * indices ) > m_pfnDrawElements;
		std::function< void( uint32_t mode, int first, int count, int primcount ) > m_pfnDrawArraysInstanced;
		std::function< void( uint32_t mode, int count, uint32_t type, const void * indices, int primcount ) > m_pfnDrawElementsInstanced;
		std::function< void( uint32_t index, uint32_t divisor ) > m_pfnVertexAttribDivisor;

		//@}
		/**@name Context */
		//@{

#if defined( _WIN32 )
		std::function< BOOL( HDC hdc, HGLRC hglrc ) > m_pfnMakeCurrent;
		std::function< BOOL( HDC hdc ) > m_pfnSwapBuffers;
		std::function< HGLRC( HDC hdc ) > m_pfnCreateContext;
		std::function< BOOL( HGLRC hContext ) > m_pfnDeleteContext;
		std::function< HGLRC( HDC hDC, HGLRC hShareContext, int const * attribList ) > m_pfnCreateContextAttribs;
		std::function< BOOL( int interval ) > m_pfnSwapInterval;
#elif defined ( __linux__ )
		std::function< int( Display * pDisplay, GLXDrawable drawable, GLXContext context ) > m_pfnMakeCurrent;
		std::function< void( Display * pDisplay, GLXDrawable drawable ) > m_pfnSwapBuffers;
		std::function< GLXContext( Display * pDisplay, XVisualInfo * pVisualInfo, GLXContext shareList, Bool direct ) > m_pfnCreateContext;
		std::function< void( Display * pDisplay, GLXContext context ) > m_pfnDeleteContext;
		std::function< GLXContext( Display * pDisplay, GLXFBConfig fbconfig, GLXContext shareList, Bool direct, int const * attribList ) > m_pfnCreateContextAttribs;
		std::function< void( Display * pDisplay, GLXDrawable drawable, int interval ) > m_pfnSwapInterval;
#else
#	error "Yet unsupported OS"
#endif

		//@}
		/**@name Matrix */
		//@{

		std::function< void( int x, int y, int width, int height ) > m_pfnViewport;

		//@}
		/**@name Material */
		//@{

		std::function< void( uint32_t sfactor, uint32_t dfactor ) > m_pfnBlendFunc;
		std::function< void( uint32_t func, float ref ) > m_pfnAlphaFunc;
		std::function< void( uint32_t face, uint32_t pname, float param ) > m_pfnMaterialf;
		std::function< void( uint32_t face, uint32_t pname, float const * params ) > m_pfnMaterialfv;

		//@}
		/**@name Texture */
		//@{

		std::function< void( int n, uint32_t * textures ) > m_pfnGenTextures;
		std::function< void( int n, uint32_t const * textures ) > m_pfnDeleteTextures;
		std::function< uint8_t( uint32_t texture ) > m_pfnIsTexture;
		std::function< void( uint32_t texture ) > m_pfnActiveTexture;
		std::function< void( uint32_t texture ) > m_pfnClientActiveTexture;
		std::function< void( uint32_t target, uint32_t pname, int param ) > m_pfnTexEnvi;
		std::function< void( uint32_t target, uint32_t pname, int const * param ) > m_pfnTexEnviv;
		std::function< void( uint32_t target, uint32_t pname, float param ) > m_pfnTexEnvf;
		std::function< void( uint32_t target, uint32_t pname, float const * param ) > m_pfnTexEnvfv;
		std::function< void( uint32_t coord, uint32_t pname, int param ) > m_pfnTexGeni;
		std::function< void( uint32_t mode ) > m_pfnReadBuffer;
#if CASTOR_HAS_VARIADIC_TEMPLATES
		std::function< void( int x, int y, int width, int height, uint32_t format, uint32_t type, void * pixels ) > m_pfnReadPixels;
#else
		void ( CALLBACK * m_pfnReadPixels )( int x, int y, int width, int height, uint32_t format, uint32_t type, void * pixels );
#endif
		std::function< void( uint32_t mode ) > m_pfnDrawBuffer;
		std::function< void( int width, int height, uint32_t format, uint32_t type, void const * data ) > m_pfnDrawPixels;
		std::function< void( uint32_t pname, int param ) > m_pfnPixelStorei;
		std::function< void( uint32_t pname, float param ) > m_pfnPixelStoref;

		//@}
		/**@name Sampler */
		//@{

		std::function< void( int count, const uint32_t * samplers ) > m_pfnDeleteSamplers;
		std::function< void( int count, uint32_t * samplers ) > m_pfnGenSamplers;
		std::function< uint8_t( uint32_t sampler ) > m_pfnIsSampler;
		std::function< void( uint32_t unit, uint32_t sampler ) > m_pfnBindSampler;
		std::function< void( uint32_t sampler, uint32_t pname, uint32_t * params ) > m_pfnGetSamplerParameteruiv;
		std::function< void( uint32_t sampler, uint32_t pname, float * params ) > m_pfnGetSamplerParameterfv;
		std::function< void( uint32_t sampler, uint32_t pname, int * params ) > m_pfnGetSamplerParameteriv;
		std::function< void( uint32_t sampler, uint32_t pname, const uint32_t * params ) > m_pfnSamplerParameteruiv;
		std::function< void( uint32_t sampler, uint32_t pname, float param ) > m_pfnSamplerParameterf;
		std::function< void( uint32_t sampler, uint32_t pname, const float * params ) > m_pfnSamplerParameterfv;
		std::function< void( uint32_t sampler, uint32_t pname, int param ) > m_pfnSamplerParameteri;
		std::function< void( uint32_t sampler, uint32_t pname, const int * params ) > m_pfnSamplerParameteriv;

		//@}
		/**@name Texture Buffer Objects */
		//@{

		std::function< void( uint32_t target, uint32_t internalFormat, uint32_t buffer ) > m_pfnTexBuffer;

		//@}
		/**@name Buffer Objects */
		//@{

		std::function< void( int n, uint32_t * buffers ) > m_pfnGenBuffers;
		std::function< void( int n, uint32_t const * buffers ) > m_pfnDeleteBuffers;
		std::function< uint8_t( uint32_t buffer ) > m_pfnIsBuffer;
		std::function< void( int size, uint32_t type, int stride, void const * pointer ) > m_pfnVertexPointer;
		std::function< void( uint32_t type, int stride, void const * pointer ) > m_pfnNormalPointer;
		std::function< void( uint32_t type, int stride, void * pointer ) > m_pfnTangentPointer;
		std::function< void( uint32_t type, int stride, void * pointer ) > m_pfnBinormalPointer;
		std::function< void( int size, uint32_t type, int stride, void const * pointer ) > m_pfnColorPointer;
		std::function< void( int size, uint32_t type, int stride, void  const * pointer ) > m_pfnTexCoordPointer;

		//@}
		/**@name FBO */
		//@{

		std::function< void( int n, uint32_t * framebuffers ) > m_pfnGenFramebuffers;
		std::function< void( int n, uint32_t const * framebuffers ) > m_pfnDeleteFramebuffers;
		std::function< uint8_t( uint32_t framebuffer ) > m_pfnIsFramebuffer;
		std::function< void( uint32_t target, uint32_t framebuffer ) > m_pfnBindFramebuffer;
		std::function< uint32_t( uint32_t target ) > m_pfnCheckFramebufferStatus;
		std::function< void( uint32_t target, uint32_t attachment, uint32_t texture, int level ) > m_pfnFramebufferTexture;
		std::function< void( uint32_t target, uint32_t attachment, uint32_t textarget, uint32_t texture, int level ) > m_pfnFramebufferTexture1D;
		std::function< void( uint32_t target, uint32_t attachment, uint32_t textarget, uint32_t texture, int level ) > m_pfnFramebufferTexture2D;
		std::function< void( uint32_t target, uint32_t attachment, uint32_t texture, int level, int layer ) > m_pfnFramebufferTextureLayer;
#if CASTOR_HAS_VARIADIC_TEMPLATES
		std::function< void( uint32_t target, uint32_t attachment, uint32_t textarget, uint32_t texture, int level, int layer ) > m_pfnFramebufferTexture3D;
		std::function< void( int srcX0, int srcY0, int srcX1, int srcY1, int dstX0, int dstY0, int dstX1, int dstY1, uint32_t mask, uint32_t filter ) > m_pfnBlitFramebuffer;
#else
		void ( CALLBACK * m_pfnFramebufferTexture3D )( uint32_t target, uint32_t attachment, uint32_t textarget, uint32_t texture, int level, int layer );
		void ( CALLBACK * m_pfnBlitFramebuffer )( int srcX0, int srcY0, int srcX1, int srcY1, int dstX0, int dstY0, int dstX1, int dstY1, uint32_t mask, uint32_t filter );
#endif
		std::function< void( int n, uint32_t const * bufs ) > m_pfnDrawBuffers;

		//@}
		/**@name RBO */
		//@{

		std::function< void( uint32_t target, uint32_t attachmentPoint, uint32_t renderbufferTarget, uint32_t renderbufferId ) > m_pfnFramebufferRenderbuffer;
		std::function< void( int n, uint32_t * renderbuffers ) > m_pfnGenRenderbuffers;
		std::function< void( int n, uint32_t const * renderbuffers ) > m_pfnDeleteRenderbuffers;
		std::function< uint8_t( uint32_t renderbuffer ) > m_pfnIsRenderbuffer;
		std::function< void( uint32_t target, uint32_t renderbuffer ) > m_pfnBindRenderbuffer;
		std::function< void( uint32_t target, uint32_t internalFormat, int width, int height ) > m_pfnRenderbufferStorage;
		std::function< void( uint32_t target, int isamples, uint32_t internalFormat, int width, int height ) > m_pfnRenderbufferStorageMultisample;
#if CASTOR_HAS_VARIADIC_TEMPLATES
		std::function< void( uint32_t target, int samples, int internalformat, int width, int height, uint8_t fixedsamplelocations ) > m_pfnTexImage2DMultisample;
#else
		void ( CALLBACK * m_pfnTexImage2DMultisample )( uint32_t target, int samples, int internalformat, int width, int height, uint8_t fixedsamplelocations );
#endif
		std::function< void( uint32_t target, uint32_t param, int * value ) > m_pfnGetRenderbufferParameteriv;

		//@}
		/**@name Uniform variables */
		//@{

		std::function< int( uint32_t program, char const * name ) > m_pfnGetUniformLocation;
#if CASTOR_HAS_VARIADIC_TEMPLATES
		std::function< void( uint32_t program, uint32_t index, int maxLength, int * length, int * size, uint32_t * type, char * name ) > m_pfnGetActiveUniform;
#else
		void ( CALLBACK * m_pfnGetActiveUniform )( uint32_t program, uint32_t index, int maxLength, int * length, int * size, uint32_t * type, char * name );
#endif
		std::function< void( int location, int v0 ) > m_pfnUniform1i;
		std::function< void( int location, int v0, int v1 ) > m_pfnUniform2i;
		std::function< void( int location, int v0, int v1, int v2 ) > m_pfnUniform3i;
		std::function< void( int location, int v0, int v1, int v2, int v3 ) > m_pfnUniform4i;
		std::function< void( int location, int count, int const * value ) > m_pfnUniform1iv;
		std::function< void( int location, int count, int const * value ) > m_pfnUniform2iv;
		std::function< void( int location, int count, int const * value ) > m_pfnUniform3iv;
		std::function< void( int location, int count, int const * value ) > m_pfnUniform4iv;
		std::function< void( int, uint32_t ) > m_pfnUniform1ui;
		std::function< void( int, uint32_t, uint32_t ) > m_pfnUniform2ui;
		std::function< void( int, uint32_t, uint32_t, uint32_t ) > m_pfnUniform3ui;
		std::function< void( int, uint32_t, uint32_t, uint32_t, uint32_t ) > m_pfnUniform4ui;
		std::function< void( int, int, uint32_t const * ) > m_pfnUniform1uiv;
		std::function< void( int, int, uint32_t const * ) > m_pfnUniform2uiv;
		std::function< void( int, int, uint32_t const * ) > m_pfnUniform3uiv;
		std::function< void( int, int, uint32_t const * ) > m_pfnUniform4uiv;
		std::function< void( int location, float v0 ) > m_pfnUniform1f;
		std::function< void( int location, float v0, float v1 ) > m_pfnUniform2f;
		std::function< void( int location, float v0, float v1, float v2 ) > m_pfnUniform3f;
		std::function< void( int location, float v0, float v1, float v2, float v3 ) > m_pfnUniform4f;
		std::function< void( int location, int count, float const * value ) > m_pfnUniform1fv;
		std::function< void( int location, int count, float const * value ) > m_pfnUniform2fv;
		std::function< void( int location, int count, float const * value ) > m_pfnUniform3fv;
		std::function< void( int location, int count, float const * value ) > m_pfnUniform4fv;
		std::function< void( int location, double x ) > m_pfnUniform1d;
		std::function< void( int location, double x, double y ) > m_pfnUniform2d;
		std::function< void( int location, double x, double y, double z ) > m_pfnUniform3d;
		std::function< void( int location, double x, double y, double z, double w ) > m_pfnUniform4d;
		std::function< void( int location, int count, double const * value ) > m_pfnUniform1dv;
		std::function< void( int location, int count, double const * value ) > m_pfnUniform2dv;
		std::function< void( int location, int count, double const * value ) > m_pfnUniform3dv;
		std::function< void( int location, int count, double const * value ) > m_pfnUniform4dv;
		std::function< void( int location, int count, uint8_t transpose, float const * value ) >m_pfnUniformMatrix2fv;
		std::function< void( int location, int count, uint8_t transpose, float const * value ) >m_pfnUniformMatrix2x3fv;
		std::function< void( int location, int count, uint8_t transpose, float const * value ) >m_pfnUniformMatrix2x4fv;
		std::function< void( int location, int count, uint8_t transpose, float const * value ) >m_pfnUniformMatrix3fv;
		std::function< void( int location, int count, uint8_t transpose, float const * value ) >m_pfnUniformMatrix3x2fv;
		std::function< void( int location, int count, uint8_t transpose, float const * value ) >m_pfnUniformMatrix3x4fv;
		std::function< void( int location, int count, uint8_t transpose, float const * value ) >m_pfnUniformMatrix4fv;
		std::function< void( int location, int count, uint8_t transpose, float const * value ) >m_pfnUniformMatrix4x2fv;
		std::function< void( int location, int count, uint8_t transpose, float const * value ) >m_pfnUniformMatrix4x3fv;
		std::function< void( int location, int count, uint8_t transpose, double const * value ) > m_pfnUniformMatrix2dv;
		std::function< void( int location, int count, uint8_t transpose, double const * value ) > m_pfnUniformMatrix2x3dv;
		std::function< void( int location, int count, uint8_t transpose, double const * value ) > m_pfnUniformMatrix2x4dv;
		std::function< void( int location, int count, uint8_t transpose, double const * value ) > m_pfnUniformMatrix3dv;
		std::function< void( int location, int count, uint8_t transpose, double const * value ) > m_pfnUniformMatrix3x2dv;
		std::function< void( int location, int count, uint8_t transpose, double const * value ) > m_pfnUniformMatrix3x4dv;
		std::function< void( int location, int count, uint8_t transpose, double const * value ) > m_pfnUniformMatrix4dv;
		std::function< void( int location, int count, uint8_t transpose, double const * value ) > m_pfnUniformMatrix4x2dv;
		std::function< void( int location, int count, uint8_t transpose, double const * value ) > m_pfnUniformMatrix4x3dv;

		//@}
		/**@name Uniform buffer object */
		//@{

		std::function< uint32_t( uint32_t program, char const * uniformBlockName ) > m_pfnGetUniformBlockIndex;
		std::function< void( uint32_t target, uint32_t index, uint32_t buffer ) > m_pfnBindBufferBase;
		std::function< void( uint32_t program, uint32_t uniformBlockIndex, uint32_t uniformBlockBinding ) > m_pfnUniformBlockBinding;
		std::function< void( uint32_t program, int uniformCount, char const ** uniformNames, uint32_t * uniformIndices ) > m_pfnGetUniformIndices;
		std::function< void( uint32_t program, int uniformCount, uint32_t const * uniformIndices, uint32_t pname, int * params ) > m_pfnGetActiveUniformsiv;
		std::function< void( uint32_t program, uint32_t uniformBlockIndex, uint32_t pname, int * params ) > m_pfnGetActiveUniformBlockiv;

		//@}
		/**@name Shader object */
		//@{

		std::function< uint32_t( uint32_t type ) > m_pfnCreateShader;
		std::function< void( uint32_t shader ) > m_pfnDeleteShader;
		std::function< uint8_t( uint32_t shader ) > m_pfnIsShader;
		std::function< void( uint32_t program, uint32_t shader ) > m_pfnAttachShader;
		std::function< void( uint32_t program, uint32_t shader ) > m_pfnDetachShader;
		std::function< void( uint32_t shader ) > m_pfnCompileShader;
		std::function< void( uint32_t shader, uint32_t pname, int * param ) > m_pfnGetShaderiv;
		std::function< void( uint32_t shader, int bufSize, int * length, char * infoLog ) > m_pfnGetShaderInfoLog;
		std::function< void( uint32_t shader, int count, const char ** string, const int * length ) > m_pfnShaderSource;

		//@}
		/**@name Shader program */
		//@{

		std::function< uint32_t() > m_pfnCreateProgram;
		std::function< void( uint32_t program ) > m_pfnDeleteProgram;
		std::function< uint8_t( uint32_t program ) > m_pfnIsProgram;
		std::function< void( uint32_t program ) > m_pfnLinkProgram;
		std::function< void( uint32_t program ) > m_pfnValidateProgram;
		std::function< void( uint32_t program ) > m_pfnUseProgram;
		std::function< void( uint32_t program, uint32_t pname, int * param ) > m_pfnGetProgramiv;
		std::function< void( uint32_t program, int bufSize, int * length, char * infoLog ) > m_pfnGetProgramInfoLog;
		std::function< int( uint32_t program, char const * name ) > m_pfnGetAttribLocation;
		std::function< void( uint32_t program, uint32_t pname, int value ) > m_pfnProgramParameteri;

		//@}
		/**@name Vertex Attribute Pointer */
		//@{

		std::function< void( uint32_t ) > m_pfnEnableVertexAttribArray;
#if CASTOR_HAS_VARIADIC_TEMPLATES
		std::function< void( uint32_t index, int size, uint32_t type, uint8_t normalized, int stride, void const * pointer ) > m_pfnVertexAttribPointer;
#else
		void ( CALLBACK * m_pfnVertexAttribPointer )( uint32_t index, int size, uint32_t type, uint8_t normalized, int stride, void const * pointer );
#endif
		std::function< void( uint32_t index, int size, uint32_t type, int stride, void const * pointer ) > m_pfnVertexAttribIPointer;
		std::function< void( uint32_t ) > m_pfnDisableVertexAttribArray;

		//@}
		/**@name Vertex Array Object */
		//@{

		std::function< void( int n, uint32_t * arrays ) > m_pfnGenVertexArrays;
		std::function< void( int n, uint32_t const * arrays ) > m_pfnDeleteVertexArrays;
		std::function< uint8_t( uint32_t array ) > m_pfnIsVertexArray;
		std::function< void( uint32_t array ) > m_pfnBindVertexArray;

		//@}
		/*@name NV_vertex_buffer_unified_memory extension */
		//@{

		std::function< void( uint32_t value, uint64_t * result ) > m_pfnGetIntegerui64v;

		//@}

		//@}
		/*@name Queries */
		//@{

		std::function< void( int n, uint32_t * queries ) > m_pfnGenQueries;
		std::function< void( int n, uint32_t const * queries ) > m_pfnDeleteQueries;
		std::function< uint8_t( uint32_t query ) > m_pfnIsQuery;
		std::function< void( uint32_t target, uint32_t query ) > m_pfnBeginQuery;
		std::function< void( uint32_t target ) > m_pfnEndQuery;
		std::function< void( uint32_t id, uint32_t target ) > m_pfnQueryCounter;
		std::function< void( uint32_t id, uint32_t pname, int32_t * params ) > m_pfnGetQueryObjectiv;
		std::function< void( uint32_t id, uint32_t pname, uint32_t * params ) > m_pfnGetQueryObjectuiv;
		std::function< void( uint32_t id, uint32_t pname, int64_t * params ) > m_pfnGetQueryObjecti64v;
		std::function< void( uint32_t id, uint32_t pname, uint64_t * params ) > m_pfnGetQueryObjectui64v;

		//@}

		std::function< void( uint32_t p_param, int p_value ) > m_pfnPatchParameteri;
	};

	namespace gl_api
	{
		template< typename Func >
		bool GetFunction( Castor::String const & p_name, Func & p_func )
		{
#if defined( _WIN32 )
			p_func = reinterpret_cast< Func >( wglGetProcAddress( Castor::string::string_cast< char >( p_name ).c_str() ) );
#else
			p_func = reinterpret_cast< Func >( glXGetProcAddressARB( ( GLubyte const * )Castor::string::string_cast< char >( p_name ).c_str() ) );
#endif
			return p_func != NULL;
		}

#if CASTOR_HAS_VARIADIC_TEMPLATES
		template< typename Ret, typename ... Arguments >
		bool GetFunction( Castor::String const & p_name, std::function< Ret( Arguments... ) > & p_func )
		{
			typedef Ret( CALLBACK * PFNType )( Arguments... );
			PFNType l_pfnResult = NULL;

			if ( GetFunction( p_name, l_pfnResult ) )
			{
				p_func = l_pfnResult;
			}

			return l_pfnResult != NULL;
		}
#else
		template< typename Ret >
		bool GetFunction( Castor::String const & p_name, std::function< Ret() > & p_func )
		{
			typedef Ret( CALLBACK * PFNType )();
			PFNType l_pfnResult = NULL;

			if ( GetFunction( p_name, l_pfnResult ) )
			{
				p_func = l_pfnResult;
			}

			return l_pfnResult != NULL;
		}

		template< typename Ret, typename T1 >
		bool GetFunction( Castor::String const & p_name, std::function< Ret( T1 ) > & p_func )
		{
			typedef Ret( CALLBACK * PFNType )( T1 );
			PFNType l_pfnResult = NULL;

			if ( GetFunction( p_name, l_pfnResult ) )
			{
				p_func = l_pfnResult;
			}

			return l_pfnResult != NULL;
		}

		template< typename Ret, typename T1, typename T2 >
		bool GetFunction( Castor::String const & p_name, std::function< Ret( T1, T2 ) > & p_func )
		{
			typedef Ret( CALLBACK * PFNType )( T1, T2 );
			PFNType l_pfnResult = NULL;

			if ( GetFunction( p_name, l_pfnResult ) )
			{
				p_func = l_pfnResult;
			}

			return l_pfnResult != NULL;
		}

		template< typename Ret, typename T1, typename T2, typename T3 >
		bool GetFunction( Castor::String const & p_name, std::function< Ret( T1, T2, T3 ) > & p_func )
		{
			typedef Ret( CALLBACK * PFNType )( T1, T2, T3 );
			PFNType l_pfnResult = NULL;

			if ( GetFunction( p_name, l_pfnResult ) )
			{
				p_func = l_pfnResult;
			}

			return l_pfnResult != NULL;
		}

		template< typename Ret, typename T1, typename T2, typename T3, typename T4 >
		bool GetFunction( Castor::String const & p_name, std::function< Ret( T1, T2, T3, T4 ) > & p_func )
		{
			typedef Ret( CALLBACK * PFNType )( T1, T2, T3, T4 );
			PFNType l_pfnResult = NULL;

			if ( GetFunction( p_name, l_pfnResult ) )
			{
				p_func = l_pfnResult;
			}

			return l_pfnResult != NULL;
		}

		template< typename Ret, typename T1, typename T2, typename T3, typename T4, typename T5 >
		bool GetFunction( Castor::String const & p_name, std::function< Ret( T1, T2, T3, T4, T5 ) > & p_func )
		{
			typedef Ret( CALLBACK * PFNType )( T1, T2, T3, T4, T5 );
			PFNType l_pfnResult = NULL;

			if ( GetFunction( p_name, l_pfnResult ) )
			{
				p_func = l_pfnResult;
			}

			return l_pfnResult != NULL;
		}

		template< typename Ret, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6 >
		bool GetFunction( Castor::String const & p_name, std::function< Ret( T1, T2, T3, T4, T5, T6 ) > & p_func )
		{
			typedef Ret( CALLBACK * PFNType )( T1, T2, T3, T4, T5, T6 );
			PFNType l_pfnResult = NULL;

			if ( GetFunction( p_name, l_pfnResult ) )
			{
				p_func = l_pfnResult;
			}

			return l_pfnResult != NULL;
		}

		template< typename Ret, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7 >
		bool GetFunction( Castor::String const & p_name, std::function< Ret( T1, T2, T3, T4, T5, T6, T7 ) > & p_func )
		{
			typedef Ret( CALLBACK * PFNType )( T1, T2, T3, T4, T5, T6, T7 );
			PFNType l_pfnResult = NULL;

			if ( GetFunction( p_name, l_pfnResult ) )
			{
				p_func = l_pfnResult;
			}

			return l_pfnResult != NULL;
		}

		template< typename Ret, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8 >
		bool GetFunction( Castor::String const & p_name, std::function< Ret( T1, T2, T3, T4, T5, T6, T7, T8 ) > & p_func )
		{
			typedef Ret( CALLBACK * PFNType )( T1, T2, T3, T4, T5, T6, T7, T8 );
			PFNType l_pfnResult = NULL;

			if ( GetFunction( p_name, l_pfnResult ) )
			{
				p_func = l_pfnResult;
			}

			return l_pfnResult != NULL;
		}

		template< typename Ret, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9 >
		bool GetFunction( Castor::String const & p_name, std::function< Ret( T1, T2, T3, T4, T5, T6, T7, T8, T9 ) > & p_func )
		{
			typedef Ret( CALLBACK * PFNType )( T1, T2, T3, T4, T5, T6, T7, T8, T9 );
			PFNType l_pfnResult = NULL;

			if ( GetFunction( p_name, l_pfnResult ) )
			{
				p_func = l_pfnResult;
			}

			return l_pfnResult != NULL;
		}

		template< typename Ret, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10 >
		bool GetFunction( Castor::String const & p_name, std::function< Ret( T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 ) > & p_func )
		{
			typedef Ret( CALLBACK * PFNType )( T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 );
			PFNType l_pfnResult = NULL;

			if ( GetFunction( p_name, l_pfnResult ) )
			{
				p_func = l_pfnResult;
			}

			return l_pfnResult != NULL;
		}
#endif
	}

#	define MAKE_GL_EXTENSION( x )	static const Castor::String x = cuT( "GL_" ) cuT( #x );
#	define MAKE_WGL_EXTENSION( x )	static const Castor::String x = cuT( "WGL_" ) cuT( #x );
#	define MAKE_GLX_EXTENSION( x )	static const Castor::String x = cuT( "GLX_" ) cuT( #x );

	MAKE_GL_EXTENSION( ARB_draw_buffers_blend )
	MAKE_GL_EXTENSION( AMD_draw_buffers_blend )
	MAKE_GL_EXTENSION( EXT_clip_volume_hint )
	MAKE_GL_EXTENSION( ARB_texture_multisample )
	MAKE_GL_EXTENSION( ARB_imaging )
	MAKE_GL_EXTENSION( ARB_debug_output )
	MAKE_GL_EXTENSION( AMDX_debug_output )
	MAKE_GL_EXTENSION( ARB_vertex_buffer_object )
	MAKE_GL_EXTENSION( EXT_coordinate_frame )
	MAKE_GL_EXTENSION( ARB_pixel_buffer_object )
	MAKE_GL_EXTENSION( ARB_vertex_array_object )
	MAKE_GL_EXTENSION( ARB_texture_buffer_object )
	MAKE_GL_EXTENSION( EXT_framebuffer_object )
	MAKE_GL_EXTENSION( ARB_framebuffer_object )
	MAKE_GL_EXTENSION( ARB_sampler_objects )
	MAKE_GL_EXTENSION( EXT_texture_filter_anisotropic )
	MAKE_GL_EXTENSION( ARB_vertex_program )
	MAKE_GL_EXTENSION( ARB_fragment_program )
	MAKE_GL_EXTENSION( ARB_uniform_buffer_object )
	MAKE_GL_EXTENSION( ARB_geometry_shader4 )
	MAKE_GL_EXTENSION( EXT_geometry_shader4 )
	MAKE_GL_EXTENSION( ARB_tessellation_shader )
	MAKE_GL_EXTENSION( ARB_compute_shader )
	MAKE_GL_EXTENSION( ARB_draw_instanced )
	MAKE_GL_EXTENSION( ARB_instanced_arrays )
	MAKE_GL_EXTENSION( EXT_draw_instanced )
	MAKE_GL_EXTENSION( EXT_instanced_arrays )
	MAKE_GL_EXTENSION( EXT_direct_state_access )
	MAKE_GL_EXTENSION( ARB_texture_non_power_of_two )
	MAKE_GL_EXTENSION( NV_vertex_buffer_unified_memory )
	MAKE_GL_EXTENSION( NV_shader_buffer_load )
	MAKE_GL_EXTENSION( ARB_timer_query )

#	if defined( _WIN32 )
	MAKE_WGL_EXTENSION( ARB_create_context )
	MAKE_WGL_EXTENSION( ARB_pixel_format )
	MAKE_WGL_EXTENSION( EXT_swap_control )
#	elif defined( __linux__ )
	MAKE_GLX_EXTENSION( ARB_create_context )
	MAKE_GLX_EXTENSION( EXT_swap_control )
#	else
#		error "Yet unsupported OS"
#	endif
}

#endif
