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

#include "GlObject.hpp"

#include <GlslWriterPrerequisites.hpp>

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
		inline bool BindTexture( eGL_TEXDIM p_target, uint32_t texture );
		inline bool GenerateMipmap( eGL_TEXDIM p_target );
		inline bool TexSubImage1D( eGL_TEXDIM p_target, int level, int xoffset, int width, eGL_FORMAT format, eGL_TYPE type, void const * data );
		inline bool TexSubImage2D( eGL_TEXDIM p_target, int level, int xoffset, int yoffset, int width, int height, eGL_FORMAT format, eGL_TYPE type, void const * data );
		inline bool TexSubImage2D( eGL_TEXDIM p_target, int level, Castor::Position const & p_position, Castor::Size const & p_size, eGL_FORMAT format, eGL_TYPE type, void const * data );
		inline bool TexSubImage2D( eGL_TEXDIM p_target, int level, Castor::Rectangle const & p_rect, eGL_FORMAT format, eGL_TYPE type, void const * data );
		inline bool TexSubImage3D( eGL_TEXDIM p_target, int level, int xoffset, int yoffset, int zoffset, int width, int height, int depth, eGL_FORMAT format, eGL_TYPE type, void const * data );
		inline bool TexImage1D( eGL_TEXDIM p_target, int level, eGL_INTERNAL internalFormat, int width, int border, eGL_FORMAT format, uint32_t type, void const * data );
		inline bool TexImage2D( eGL_TEXDIM p_target, int level, eGL_INTERNAL internalFormat, int width, int height, int border, eGL_FORMAT format, uint32_t type, void const * data );
		inline bool TexImage2D( eGL_TEXDIM p_target, int level, eGL_INTERNAL internalFormat, Castor::Size const & p_size, int border, eGL_FORMAT format, uint32_t type, void const * data );
		inline bool TexImage3D( eGL_TEXDIM p_target, int level, eGL_INTERNAL internalFormat, int width, int height, int depth, int border, eGL_FORMAT format, uint32_t type, void const * data );
		inline bool TexParameter( eGL_TEXDIM p_target, eGL_TEXTURE_PARAMETER pname, int param );
		inline bool TexParameter( eGL_TEXDIM p_target, eGL_TEXTURE_PARAMETER pname, float param );
		inline bool TexParameter( eGL_TEXDIM p_target, eGL_TEXTURE_PARAMETER pname, const int * params );
		inline bool TexParameter( eGL_TEXDIM p_target, eGL_TEXTURE_PARAMETER pname, const float * params );
		inline bool GetTexImage( eGL_TEXDIM p_target, int level, eGL_FORMAT format, eGL_TYPE type, void * img );

		std::function< void( uint32_t mode, uint32_t texture ) > m_pfnBindTexture;
		std::function< void( uint32_t target, int level, int xoffset, int width, uint32_t format, uint32_t type, void const * data ) > m_pfnTexSubImage1D;
		std::function< void( uint32_t target, int level, int xoffset, int yoffset, int width, int height, uint32_t format, uint32_t type, void const * data ) > m_pfnTexSubImage2D;
		std::function< void( uint32_t target, int level, int xoffset, int yoffset, int zoffset, int width, int height, int depth, uint32_t format, uint32_t type, void const * data ) > m_pfnTexSubImage3D;
		std::function< void( uint32_t target, int level, int internalFormat, int width, int border, uint32_t format, uint32_t type, void const * data ) > m_pfnTexImage1D;
		std::function< void( uint32_t target, int level, int internalFormat, int width, int height, int border, uint32_t format, uint32_t type, void const * data ) > m_pfnTexImage2D;
		std::function< void( uint32_t target, int level, int internalFormat, int width, int height, int depth, int border, uint32_t format, uint32_t type, void const * data ) > m_pfnTexImage3D;
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
		inline bool BindTexture( eGL_TEXDIM /*p_target*/, uint32_t texture )
		{
			m_uiTexture = texture;
			return true;
		}
		inline bool GenerateMipmap( eGL_TEXDIM p_target );
		inline bool TexSubImage1D( eGL_TEXDIM p_target, int level, int xoffset, int width, eGL_FORMAT format, eGL_TYPE type, void const * data );
		inline bool TexSubImage2D( eGL_TEXDIM p_target, int level, int xoffset, int yoffset, int width, int height, eGL_FORMAT format, eGL_TYPE type, void const * data );
		inline bool TexSubImage2D( eGL_TEXDIM p_target, int level, Castor::Position const & p_position, Castor::Size const & p_size, eGL_FORMAT format, eGL_TYPE type, void const * data );
		inline bool TexSubImage2D( eGL_TEXDIM p_target, int level, Castor::Rectangle const & p_rect, eGL_FORMAT format, eGL_TYPE type, void const * data );
		inline bool TexSubImage3D( eGL_TEXDIM p_target, int level, int xoffset, int yoffset, int zoffset, int width, int height, int depth, eGL_FORMAT format, eGL_TYPE type, void const * data );
		inline bool TexImage1D( eGL_TEXDIM p_target, int level, eGL_INTERNAL internalFormat, int width, int border, eGL_FORMAT format, uint32_t type, void const * data );
		inline bool TexImage2D( eGL_TEXDIM p_target, int level, eGL_INTERNAL internalFormat, int width, int height, int border, eGL_FORMAT format, uint32_t type, void const * data );
		inline bool TexImage2D( eGL_TEXDIM p_target, int level, eGL_INTERNAL internalFormat, Castor::Size const & p_size, int border, eGL_FORMAT format, uint32_t type, void const * data );
		inline bool TexImage3D( eGL_TEXDIM p_target, int level, eGL_INTERNAL internalFormat, int width, int height, int depth, int border, eGL_FORMAT format, uint32_t type, void const * data );
		inline bool TexParameter( eGL_TEXDIM p_target, eGL_TEXTURE_PARAMETER pname, int param );
		inline bool TexParameter( eGL_TEXDIM p_target, eGL_TEXTURE_PARAMETER pname, float param );
		inline bool TexParameter( eGL_TEXDIM p_target, eGL_TEXTURE_PARAMETER pname, const int * params );
		inline bool TexParameter( eGL_TEXDIM p_target, eGL_TEXTURE_PARAMETER pname, const float * params );
		inline bool GetTexImage( eGL_TEXDIM p_target, int level, eGL_FORMAT format, eGL_TYPE type, void * img );

		uint32_t m_uiTexture;
		std::function< void( uint32_t texture, uint32_t target, int level, int xoffset, int width, uint32_t format, uint32_t type, void const * data ) > m_pfnTextureSubImage1D;
		std::function< void( uint32_t texture, uint32_t target, int level, int xoffset, int yoffset, int width, int height, uint32_t format, uint32_t type, void const * data ) > m_pfnTextureSubImage2D;
		std::function< void( uint32_t texture, uint32_t target, int level, int xoffset, int yoffset, int zoffset, int width, int height, int depth, uint32_t format, uint32_t type, void const * data ) > m_pfnTextureSubImage3D;
		std::function< void( uint32_t texture, uint32_t target, int level, int internalFormat, int width, int border, uint32_t format, uint32_t type, void const * data ) > m_pfnTextureImage1D;
		std::function< void( uint32_t texture, uint32_t target, int level, int internalFormat, int width, int height, int border, uint32_t format, uint32_t type, void const * data ) > m_pfnTextureImage2D;
		std::function< void( uint32_t texture, uint32_t target, int level, int internalFormat, int width, int height, int depth, int border, uint32_t format, uint32_t type, void const * data ) > m_pfnTextureImage3D;
		std::function< void( uint32_t texture, uint32_t target, int level, uint32_t format, uint32_t type, void * pixels ) > m_pfnGetTextureImage;
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

		inline bool BufferAddressRange( eGL_ADDRESS pname, uint32_t index, uint64_t address, size_t length );
		inline bool VertexFormat( int size, eGL_TYPE type, int stride );
		inline bool NormalFormat( eGL_TYPE type, int stride );
		inline bool ColorFormat( int size, eGL_TYPE type, int stride );
		inline bool IndexFormat( eGL_TYPE type, int stride );
		inline bool TexCoordFormat( int size, eGL_TYPE type, int stride );
		inline bool EdgeFlagFormat( int stride );
		inline bool SecondaryColorFormat( int size, eGL_TYPE type, int stride );
		inline bool FogCoordFormat( uint32_t type, int stride );
		inline bool VertexAttribFormat( uint32_t index, int size, eGL_TYPE type, bool normalized, int stride );
		inline bool VertexAttribIFormat( uint32_t index, int size, eGL_TYPE type, int stride );

		//@}
		/*@name NV_shader_buffer_load extension */
		//@{

		inline bool MakeBufferResident( eGL_BUFFER_TARGET target, eGL_RESIDENT_BUFFER_ACCESS access );
		inline bool MakeBufferNonResident( eGL_BUFFER_TARGET target );
		inline bool IsBufferResident( eGL_BUFFER_TARGET target );
		inline bool MakeNamedBufferResident( uint32_t buffer, eGL_RESIDENT_BUFFER_ACCESS access );
		inline bool MakeNamedBufferNonResident( uint32_t buffer );
		inline bool IsNamedBufferResident( uint32_t buffer );
		inline bool GetBufferParameter( eGL_BUFFER_TARGET target, eGL_BUFFER_PARAMETER pname, uint64_t * params );
		inline bool GetNamedBufferParameter( uint32_t buffer, eGL_BUFFER_PARAMETER pname,  uint64_t * params );

		//@}
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
		inline bool BindBuffer( eGL_BUFFER_TARGET target, uint32_t buffer );
		inline bool BufferData( eGL_BUFFER_TARGET target, ptrdiff_t size, void const * data, eGL_BUFFER_MODE usage );
		inline bool BufferSubData( eGL_BUFFER_TARGET target, ptrdiff_t offset, ptrdiff_t size, void const * data );
		inline void * MapBuffer( eGL_BUFFER_TARGET target, eGL_LOCK access );
		inline bool UnmapBuffer( eGL_BUFFER_TARGET target );
		inline void * MapBufferRange( eGL_BUFFER_TARGET target, ptrdiff_t offset, ptrdiff_t length, uint32_t access );
		inline bool GetBufferParameter( eGL_BUFFER_TARGET target, eGL_BUFFER_PARAMETER pname, int * params );
		inline bool FlushMappedBufferRange( eGL_BUFFER_TARGET target, ptrdiff_t offset, ptrdiff_t length );

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
		inline bool BindBuffer( eGL_BUFFER_TARGET /*target*/, uint32_t buffer )
		{
			m_uiBuffer = buffer;
			return true;
		}
		inline bool BufferData( eGL_BUFFER_TARGET target, ptrdiff_t size, void const * data, eGL_BUFFER_MODE usage );
		inline bool BufferSubData( eGL_BUFFER_TARGET target, ptrdiff_t offset, ptrdiff_t size, void const * data );
		inline void * MapBuffer( eGL_BUFFER_TARGET target, eGL_LOCK access );
		inline bool UnmapBuffer( eGL_BUFFER_TARGET target );
		inline void * MapBufferRange( eGL_BUFFER_TARGET target, ptrdiff_t offset, ptrdiff_t length, uint32_t access );
		inline bool GetBufferParameter( eGL_BUFFER_TARGET target, eGL_BUFFER_PARAMETER pname, int * params );
		inline bool FlushMappedBufferRange( eGL_BUFFER_TARGET target, ptrdiff_t offset, ptrdiff_t length );

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

			PixelFmt( eGL_FORMAT p_format, eGL_INTERNAL p_eInternal, eGL_TYPE p_type )
				: Format( p_format )
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
		C3D_Gl_API GLSL::GlslWriter CreateGlslWriter()const;
		C3D_Gl_API bool GlCheckError( std::string const & p_strText )const;
		C3D_Gl_API bool GlCheckError( std::wstring const & p_strText )const;

		inline bool HasMultiTexturing()const;
		inline bool HasVao()const;
		inline bool HasUbo()const;
		inline bool HasPbo()const;
		inline bool HasTbo()const;
		inline bool HasFbo()const;
		inline bool HasVSh()const;
		inline bool HasPSh()const;
		inline bool HasGSh()const;
		inline bool HasTSh()const;
		inline bool HasCSh()const;
		inline bool HasSpl()const;
		inline bool HasVbo()const;
		inline bool HasDepthClipping()const;
		inline bool HasInstancing()const;
		inline bool HasNonPowerOfTwoTextures()const;
		inline bool CanBindVboToGpuAddress()const;
		inline Castor::String const & GetGlslErrorString( int p_index )const;
		inline eGL_PRIMITIVE Get( Castor3D::eTOPOLOGY p_index )const;
		inline eGL_TEXDIM Get( Castor3D::eTEXTURE_TYPE p_index )const;
		inline eGL_FUNC Get( Castor3D::eALPHA_FUNC p_eAlphaFunc )const;
		inline eGL_WRAP_MODE Get( Castor3D::eWRAP_MODE p_eWrapMode )const;
		inline eGL_INTERPOLATION_MODE Get( Castor3D::eINTERPOLATION_MODE p_interpolation )const;
		inline int Get( Castor3D::eBLEND_SOURCE p_eArgument )const;
		inline eGL_BLEND_FUNC Get( Castor3D::eRGB_BLEND_FUNC p_mode )const;
		inline eGL_BLEND_FUNC Get( Castor3D::eALPHA_BLEND_FUNC p_mode )const;
		inline eGL_BLEND_FACTOR Get( Castor3D::eBLEND p_eBlendFactor )const;
		inline PixelFmt const & Get( Castor::ePIXEL_FORMAT p_pixelFormat )const;
		inline eGL_SHADER_TYPE Get( Castor3D::eSHADER_TYPE p_type )const;
		inline eGL_INTERNAL_FORMAT GetInternal( Castor::ePIXEL_FORMAT p_format )const;
		inline uint32_t GetComponents( uint32_t p_components )const;
		inline eGL_TEXTURE_ATTACHMENT Get( Castor3D::eATTACHMENT_POINT p_eAttachment )const;
		inline eGL_FRAMEBUFFER_MODE Get( Castor3D::eFRAMEBUFFER_TARGET p_target )const;
		inline eGL_RENDERBUFFER_ATTACHMENT GetRboAttachment( Castor3D::eATTACHMENT_POINT p_eAttachment )const;
		inline eGL_RENDERBUFFER_STORAGE GetRboStorage( Castor::ePIXEL_FORMAT p_pixelFormat )const;
		inline eGL_TWEAK Get( Castor3D::eTWEAK p_eTweak )const;
		inline eGL_BUFFER Get( Castor3D::eBUFFER p_buffer )const;
		inline eGL_BUFFER Get( eGL_TEXTURE_ATTACHMENT p_buffer )const;
		inline eGL_BUFFER Get( eGL_RENDERBUFFER_ATTACHMENT p_buffer )const;
		inline eGL_FACE Get( Castor3D::eFACE p_eFace )const;
		inline eGL_FILL_MODE Get( Castor3D::eFILL_MODE p_mode )const;
		inline eGL_FUNC Get( Castor3D::eSTENCIL_FUNC p_func )const;
		inline eGL_STENCIL_OP Get( Castor3D::eSTENCIL_OP p_eOp )const;
		inline eGL_BLEND_OP Get( Castor3D::eBLEND_OP p_eOp )const;
		inline eGL_FUNC Get( Castor3D::eDEPTH_FUNC p_func )const;
		inline bool Get( Castor3D::eWRITING_MASK p_eMask )const;
		inline bool HasDebugOutput()const;
		inline Castor::String const & GetVendor()const;
		inline Castor::String const & GetRenderer()const;
		inline Castor::String const & GetStrVersion()const;
		inline int GetVersion()const;
		inline int GetGlslVersion()const;
		inline GlRenderSystem & GetRenderSystem();
		inline GlRenderSystem const & GetRenderSystem()const;
		inline bool HasExtension( Castor::String const & p_strExtName, bool p_log = true )const;
		inline eGL_BUFFER_MODE GetBufferFlags( uint32_t p_flags )const;

		/**@name General Functions */
		//@{

		inline uint32_t GetError()const;
		inline bool ClearColor( float red, float green, float blue, float alpha )const;
		inline bool ClearColor( Castor::Colour const & p_colour )const;
		inline bool ShadeModel( eGL_SHADE_MODEL mode )const;
		inline bool Clear( uint32_t mask )const;
		inline bool Enable( eGL_TWEAK mode )const;
		inline bool Disable( eGL_TWEAK mode )const;
		inline bool Enable( eGL_TEXDIM texture )const;
		inline bool Disable( eGL_TEXDIM texture )const;
		inline bool SelectBuffer( int size, uint32_t * buffer )const;
		inline bool GetIntegerv( uint32_t pname, int * params )const;
		inline bool GetIntegerv( uint32_t pname, uint64_t * params )const;
		inline int RenderMode( eGL_RENDER_MODE mode )const;
		inline bool DepthFunc( eGL_FUNC p_func )const;
		inline bool DepthMask( bool p_bFlag )const;
		inline bool ColorMask( bool p_r, bool p_g, bool p_b, bool p_a )const;
		inline bool DebugMessageCallback( PFNGLDEBUGPROC pfnProc, void * p_pThis )const;
		inline bool DebugMessageCallback( PFNGLDEBUGAMDPROC pfnProc, void * p_pThis )const;
		inline bool PolygonMode( eGL_FACE p_eFacing, eGL_FILL_MODE p_mode )const;
		inline bool StencilOp( eGL_STENCIL_OP p_eStencilFail, eGL_STENCIL_OP p_eDepthFail, eGL_STENCIL_OP p_eStencilPass )const;
		inline bool StencilFunc( eGL_FUNC p_func, int p_iRef, uint32_t p_uiMask )const;
		inline bool StencilMask( uint32_t p_uiMask )const;
		inline bool StencilOpSeparate( eGL_FACE p_eFacing, eGL_STENCIL_OP p_eStencilFail, eGL_STENCIL_OP p_eDepthFail, eGL_STENCIL_OP p_eStencilPass )const;
		inline bool StencilFuncSeparate( eGL_FACE p_eFacing, eGL_FUNC p_func, int p_iRef, uint32_t p_uiMask )const;
		inline bool StencilMaskSeparate( eGL_FACE p_eFacing, uint32_t p_uiMask )const;
		inline bool Hint( eGL_HINT p_eHint, eGL_HINT_VALUE p_eValue )const;
		inline bool PolygonOffset( float p_fFactor, float p_fUnits )const;
		inline bool BlendColor( Castor::Colour const & p_clrFactors )const;
		inline bool SampleCoverage( float fValue, bool invert )const;
		inline bool Viewport( int x, int y, int width, int height )const;

		//@}
		/**@name Draw Functions */
		//@{

		inline bool DrawArrays( eGL_PRIMITIVE mode, int first, int count )const;
		inline bool DrawElements( eGL_PRIMITIVE mode, int count, eGL_TYPE type, const void * indices )const;
		inline bool DrawArraysInstanced( eGL_PRIMITIVE mode, int first, int count, int primcount )const;
		inline bool DrawElementsInstanced( eGL_PRIMITIVE mode, int count, eGL_TYPE type, const void * indices, int primcount )const;

		//@}
		/**@name Instanciation Functions */
		//@{

		inline bool VertexAttribDivisor( uint32_t index, uint32_t divisor )const;

		//@}
		/**@name Context functions */
		//@{

#if defined( _WIN32 )

		inline bool MakeCurrent( HDC hdc, HGLRC hglrc )const;
		inline bool SwapBuffers( HDC hdc )const;
		inline bool SwapInterval( int interval )const;
		inline HGLRC CreateContext( HDC hdc )const;
		inline HGLRC CreateContextAttribs( HDC hDC, HGLRC hShareContext, int const * attribList )const;
		inline bool DeleteContext( HGLRC hContext )const;
		inline bool HasCreateContextAttribs()const;

#elif defined( __linux__ )

		inline bool MakeCurrent( Display * pDisplay, GLXDrawable drawable, GLXContext context )const;
		inline bool SwapBuffers( Display * pDisplay, GLXDrawable drawable )const;
		inline bool SwapInterval( Display * pDisplay, GLXDrawable drawable, int interval )const;
		inline GLXContext CreateContext( Display * pDisplay, XVisualInfo * pVisualInfo, GLXContext shareList, Bool direct )const;
		inline GLXContext CreateContextAttribs( Display * pDisplay, GLXFBConfig fbconfig, GLXContext shareList, Bool direct, int const * attribList )const;
		inline bool DeleteContext( Display * pDisplay, GLXContext context )const;
		inline bool HasCreateContextAttribs()const;

#else

#	error "Yet unsupported OS"

#endif

		//@}
		/**@name Material functions */
		//@{

		inline bool CullFace( eGL_FACE face )const;
		inline bool FrontFace( eGL_FRONT_FACE_DIRECTION face )const;
		inline bool Material( eGL_DRAW_BUFFER_MODE face, eGL_MATERIAL_COMPONENT pname, float param )const;
		inline bool Material( eGL_DRAW_BUFFER_MODE face, eGL_MATERIAL_COMPONENT pname, float const * param )const;
		inline bool BlendFunc( eGL_BLEND_FACTOR sfactor, eGL_BLEND_FACTOR dfactor )const;
		inline bool BlendFunc( eGL_BLEND_FACTOR p_eRgbSrc, eGL_BLEND_FACTOR p_eRgbDst, eGL_BLEND_FACTOR p_eAlphaSrc, eGL_BLEND_FACTOR p_eAlphaDst )const;
		inline bool BlendFunc( uint32_t p_index, eGL_BLEND_FACTOR p_eRgbSrc, eGL_BLEND_FACTOR p_eRgbDst, eGL_BLEND_FACTOR p_eAlphaSrc, eGL_BLEND_FACTOR p_eAlphaDst )const;
		inline bool BlendEquation( eGL_BLEND_OP p_eOp )const;
		inline bool BlendEquation( uint32_t p_uiBuffer, eGL_BLEND_OP p_eOp )const;
		inline bool AlphaFunc( eGL_FUNC func, float ref )const;

		//@}
		/**@name Texture functions */
		//@{

		inline bool GenTextures( int n, uint32_t * textures )const;
		inline bool DeleteTextures( int n, uint32_t const * textures )const;
		inline bool IsTexture( uint32_t texture )const;
		inline bool BindTexture( eGL_TEXDIM p_target, uint32_t texture )const;
		inline bool ActiveTexture( eGL_TEXTURE_INDEX target )const;
		inline bool ClientActiveTexture( eGL_TEXTURE_INDEX target )const;
		inline bool GenerateMipmap( eGL_TEXDIM p_target )const;
		inline bool TexSubImage1D( eGL_TEXDIM p_target, int level, int xoffset, int width, eGL_FORMAT format, eGL_TYPE type, void const * data )const;
		inline bool TexSubImage2D( eGL_TEXDIM p_target, int level, int xoffset, int yoffset, int width, int height, eGL_FORMAT format, eGL_TYPE type, void const * data )const;
		inline bool TexSubImage2D( eGL_TEXDIM p_target, int level, Castor::Position const & p_position, Castor::Size const & p_size, eGL_FORMAT format, eGL_TYPE type, void const * data )const;
		inline bool TexSubImage2D( eGL_TEXDIM p_target, int level, Castor::Rectangle const & p_rect, eGL_FORMAT format, eGL_TYPE type, void const * data )const;
		inline bool TexSubImage3D( eGL_TEXDIM p_target, int level, int xoffset, int yoffset, int zoffset, int width, int height, int depth, eGL_FORMAT format, eGL_TYPE type, void const * data )const;
		inline bool TexImage1D( eGL_TEXDIM p_target, int level, eGL_INTERNAL internalFormat, int width, int border, eGL_FORMAT format, uint32_t type, void const * data )const;
		inline bool TexImage2D( eGL_TEXDIM p_target, int level, eGL_INTERNAL internalFormat, int width, int height, int border, eGL_FORMAT format, uint32_t type, void const * data )const;
		inline bool TexImage2D( eGL_TEXDIM p_target, int level, eGL_INTERNAL internalFormat, Castor::Size const & p_size, int border, eGL_FORMAT format, uint32_t type, void const * data )const;
		inline bool TexImage3D( eGL_TEXDIM p_target, int level, eGL_INTERNAL internalFormat, int width, int height, int depth, int border, eGL_FORMAT format, uint32_t type, void const * data )const;
		inline bool TexImage2DMultisample( eGL_TEXDIM p_target, int p_iSamples, eGL_INTERNAL p_eInternalFormat, int p_iWidth, int p_iHeight, bool p_bFixedSampleLocations )const;
		inline bool TexImage2DMultisample( eGL_TEXDIM p_target, int p_iSamples, eGL_INTERNAL p_eInternalFormat, Castor::Size const & p_size, bool p_bFixedSampleLocations )const;
		inline bool TexParameter( eGL_TEXDIM p_target, eGL_TEXTURE_PARAMETER pname, int param )const;
		inline bool TexParameter( eGL_TEXDIM p_target, eGL_TEXTURE_PARAMETER pname, float param )const;
		inline bool TexParameter( eGL_TEXDIM p_target, eGL_TEXTURE_PARAMETER pname, const int * params )const;
		inline bool TexParameter( eGL_TEXDIM p_target, eGL_TEXTURE_PARAMETER pname, const float * params )const;
		inline bool GetTexImage( eGL_TEXDIM p_target, int level, eGL_FORMAT format, eGL_TYPE type, void * img )const;
		inline bool TexEnvi( eGL_TEXENV_TARGET target, eGL_TEXENV_ARGNAME pname, int param )const;
		inline bool TexEnviv( eGL_TEXENV_TARGET target, eGL_TEXENV_ARGNAME pname, int const * param )const;
		inline bool TexEnvf( eGL_TEXENV_TARGET target, eGL_TEXENV_ARGNAME pname, float param )const;
		inline bool TexEnvfv( eGL_TEXENV_TARGET target, eGL_TEXENV_ARGNAME pname, float const * param )const;
		inline bool TexGeni( eGL_TEXGEN_COORD coord, eGL_TEXGEN_PARAM param, eGL_TEXGEN_MODE mode )const;
		inline bool ReadBuffer( eGL_BUFFER p_buffer )const;
		inline bool ReadPixels( int x, int y, int width, int height, eGL_FORMAT format, eGL_TYPE type, void * pixels )const;
		inline bool ReadPixels( Castor::Position const & p_position, Castor::Size const & p_size, eGL_FORMAT format, eGL_TYPE type, void * pixels )const;
		inline bool ReadPixels( Castor::Rectangle const & p_rect, eGL_FORMAT format, eGL_TYPE type, void * pixels )const;
		inline bool DrawBuffer( eGL_BUFFER p_buffer )const;
		inline bool DrawPixels( int width, int height, eGL_FORMAT format, eGL_TYPE type, void const * data )const;
		inline bool PixelStore( eGL_STORAGE_MODE p_mode, int p_iParam )const;
		inline bool PixelStore( eGL_STORAGE_MODE p_mode, float p_fParam )const;

		//@}
		/**@name Sampler functions */
		//@{

		inline bool GenSamplers( int count, uint32_t * samplers )const;
		inline bool DeleteSamplers( int count, const uint32_t * samplers )const;
		inline bool IsSampler( uint32_t sampler )const;
		inline bool BindSampler( uint32_t unit, uint32_t sampler )const;
		inline bool GetSamplerParameter( uint32_t sampler, eGL_SAMPLER_PARAMETER pname, uint32_t * params )const;
		inline bool GetSamplerParameter( uint32_t sampler, eGL_SAMPLER_PARAMETER pname, float * params )const;
		inline bool GetSamplerParameter( uint32_t sampler, eGL_SAMPLER_PARAMETER pname, int * params )const;
		inline bool SetSamplerParameter( uint32_t sampler, eGL_SAMPLER_PARAMETER pname, float param )const;
		inline bool SetSamplerParameter( uint32_t sampler, eGL_SAMPLER_PARAMETER pname, const float * params )const;
		inline bool SetSamplerParameter( uint32_t sampler, eGL_SAMPLER_PARAMETER pname, int param )const;
		inline bool SetSamplerParameter( uint32_t sampler, eGL_SAMPLER_PARAMETER pname, const int * params )const;
		inline bool SetSamplerParameter( uint32_t sampler, eGL_SAMPLER_PARAMETER pname, const uint32_t * params )const;

		//@}
		/**@name Texture Buffer objects functions */
		//@{

		inline bool TexBuffer( eGL_TEXDIM p_target, eGL_INTERNAL_FORMAT p_eInternalFormat, uint32_t buffer )const;

		//@}
		/**@name Buffer objects functions */
		//@{

		inline bool GenBuffers( int n, uint32_t * buffers )const;
		inline bool DeleteBuffers( int n, uint32_t const * buffers )const;
		inline bool IsBuffer( uint32_t buffer )const;
		inline bool BindBuffer( eGL_BUFFER_TARGET target, uint32_t buffer )const;
		inline bool BufferData( eGL_BUFFER_TARGET target, ptrdiff_t size, void const * data, eGL_BUFFER_MODE usage )const;
		inline bool BufferSubData( eGL_BUFFER_TARGET target, ptrdiff_t offset, ptrdiff_t size, void const * data )const;
		inline void * MapBuffer( eGL_BUFFER_TARGET target, eGL_LOCK access )const;
		inline bool UnmapBuffer( eGL_BUFFER_TARGET target )const;
		inline void * MapBufferRange( eGL_BUFFER_TARGET target, ptrdiff_t offset, ptrdiff_t length, uint32_t access )const;
		inline bool GetBufferParameter( eGL_BUFFER_TARGET target, eGL_BUFFER_PARAMETER pname, int * params )const;
		inline bool GetBufferParameter( eGL_BUFFER_TARGET target, eGL_BUFFER_PARAMETER pname, uint64_t * params )const;
		inline bool FlushMappedBufferRange( eGL_BUFFER_TARGET target, ptrdiff_t offset, ptrdiff_t length )const;
		inline bool VertexPointer( int size, uint32_t type, uint32_t stride, void const * pointer )const;
		inline bool NormalPointer( uint32_t type, uint32_t stride, void const * pointer )const;
		inline bool TangentPointer( uint32_t type, uint32_t stride, void const * pointer )const;
		inline bool BinormalPointer( uint32_t type, uint32_t stride, void const * pointer )const;
		inline bool HasTangentPointer()const;
		inline bool HasBinormalPointer()const;
		inline bool ColorPointer( int size, uint32_t type, uint32_t stride, void const * pointer )const;
		inline bool TexCoordPointer( int size, uint32_t type, uint32_t stride, void const * pointer )const;
		inline bool BufferAddressRange( eGL_ADDRESS pname, uint32_t index, uint64_t address, size_t length )const;
		inline bool VertexFormat( int size, eGL_TYPE type, int stride )const;
		inline bool NormalFormat( eGL_TYPE type, int stride )const;
		inline bool ColorFormat( int size, eGL_TYPE type, int stride )const;
		inline bool IndexFormat( eGL_TYPE type, int stride )const;
		inline bool TexCoordFormat( int size, eGL_TYPE type, int stride )const;
		inline bool EdgeFlagFormat( int stride )const;
		inline bool SecondaryColorFormat( int size, eGL_TYPE type, int stride )const;
		inline bool FogCoordFormat( uint32_t type, int stride )const;
		inline bool VertexAttribFormat( uint32_t index, int size, eGL_TYPE type, bool normalized, int stride )const;
		inline bool VertexAttribIFormat( uint32_t index, int size, eGL_TYPE type, int stride )const;
		inline bool MakeBufferResident( eGL_BUFFER_TARGET target, eGL_RESIDENT_BUFFER_ACCESS access )const;
		inline bool MakeBufferNonResident( eGL_BUFFER_TARGET target )const;
		inline bool IsBufferResident( eGL_BUFFER_TARGET target )const;
		inline bool MakeNamedBufferResident( uint32_t buffer, eGL_RESIDENT_BUFFER_ACCESS access )const;
		inline bool MakeNamedBufferNonResident( uint32_t buffer )const;
		inline bool IsNamedBufferResident( uint32_t buffer )const;
		inline bool GetNamedBufferParameter( uint32_t buffer, eGL_BUFFER_PARAMETER pname, uint64_t * params )const;

		//@}
		/**@name FBO functions */
		//@{

		/** see https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glGenFramebuffers.xml
		*/
		inline bool GenFramebuffers( int n, uint32_t * framebuffers )const;

		/** see https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glDeleteFramebuffers.xml
		*/
		inline bool DeleteFramebuffers( int n, uint32_t const * framebuffers )const;

		/** see https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glIsFramebuffer.xml
		*/
		inline bool IsFramebuffer( uint32_t framebuffer )const;

		/** see https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glBindFramebuffer.xml
		*/
		inline bool BindFramebuffer( eGL_FRAMEBUFFER_MODE p_eBindingMode, uint32_t framebuffer )const;

		/** see https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glFramebufferTexture.xml
		*/
		inline bool FramebufferTexture( eGL_FRAMEBUFFER_MODE p_eBindingMode, eGL_TEXTURE_ATTACHMENT p_eAttachment, uint32_t texture, int level )const;

		/** see https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glFramebufferTexture.xml
		*/
		inline bool FramebufferTexture1D( eGL_FRAMEBUFFER_MODE p_eBindingMode, eGL_TEXTURE_ATTACHMENT p_eAttachment, eGL_TEXDIM textarget, uint32_t texture, int level )const;

		/** see https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glFramebufferTexture.xml
		*/
		inline bool FramebufferTexture2D( eGL_FRAMEBUFFER_MODE p_eBindingMode, eGL_TEXTURE_ATTACHMENT p_eAttachment, eGL_TEXDIM textarget, uint32_t texture, int level )const;

		/** see https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glFramebufferTexture.xml
		*/
		inline bool FramebufferTexture3D( eGL_FRAMEBUFFER_MODE p_eBindingMode, eGL_TEXTURE_ATTACHMENT p_eAttachment, eGL_TEXDIM textarget, uint32_t texture, int level, int layer )const;

		/** see https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glFramebufferTextureLayer.xml
		*/
		inline bool FramebufferTextureLayer( eGL_FRAMEBUFFER_MODE p_eBindingMode, eGL_TEXTURE_ATTACHMENT p_eAttachment, uint32_t texture, int level, int layer )const;

		/** see https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glFramebufferRenderbuffer.xml
		*/
		inline bool FramebufferRenderbuffer( eGL_FRAMEBUFFER_MODE p_eBindingMode, eGL_RENDERBUFFER_ATTACHMENT p_eAttachment, eGL_RENDERBUFFER_MODE p_eRboTarget, uint32_t renderbufferId )const;

		/** see https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glCheckFramebufferStatus.xml
		*/
		inline eGL_FRAMEBUFFER_STATUS CheckFramebufferStatus( eGL_FRAMEBUFFER_MODE p_eBindingMode )const;

		/** see https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glGenRenderbuffers.xml
		*/
		inline bool GenRenderbuffers( int n, uint32_t * ids )const;

		/** see https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glDeleteRenderbuffers.xml
		*/
		inline bool DeleteRenderbuffers( int n, uint32_t const * ids )const;

		/** see https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glIsFramebuffer.xml
		*/
		inline bool IsRenderbuffer( uint32_t renderbuffer )const;

		/** see https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glBindRenderbuffer.xml
		*/
		inline bool BindRenderbuffer( eGL_RENDERBUFFER_MODE p_eBindingMode, uint32_t id )const;
		inline bool RenderbufferStorage( eGL_RENDERBUFFER_MODE p_eBindingMode, eGL_RENDERBUFFER_STORAGE internalFormat, int width, int height )const;
		inline bool RenderbufferStorageMultisample( eGL_RENDERBUFFER_MODE p_eBindingMode, int p_iSamples, eGL_RENDERBUFFER_STORAGE internalFormat, int width, int height )const;
		inline bool RenderbufferStorage( eGL_RENDERBUFFER_MODE p_eBindingMode, eGL_RENDERBUFFER_STORAGE internalFormat, Castor::Size const & size )const;
		inline bool RenderbufferStorageMultisample( eGL_RENDERBUFFER_MODE p_eBindingMode, int p_iSamples, eGL_RENDERBUFFER_STORAGE internalFormat, Castor::Size const & size )const;
		inline bool GetRenderbufferParameteriv( eGL_RENDERBUFFER_MODE p_eBindingMode, eGL_RENDERBUFFER_PARAMETER param, int * value )const;
		inline bool BlitFramebuffer( int srcX0, int srcY0, int srcX1, int srcY1, int dstX0, int dstY0, int dstX1, int dstY1, uint32_t mask, uint32_t filter )const;
		inline bool BlitFramebuffer( Castor::Rectangle const & rcSrc, Castor::Rectangle const & rcDst, uint32_t mask, uint32_t filter )const;
		inline bool DrawBuffers( int n, const uint32_t * bufs )const;

		//@}
		/**@name Uniform variable Functions */
		//@{

		inline int GetUniformLocation( uint32_t program, char const * name )const;
		inline bool SetUniform( int location, int v0 )const;
		inline bool SetUniform( int location, uint32_t v0 )const;
		inline bool SetUniform( int location, float v0 )const;
		inline bool SetUniform( int location, double v0 )const;
		inline bool SetUniform( int location, int v0, int v1 )const;
		inline bool SetUniform( int location, int v0, int v1, int v2 )const;
		inline bool SetUniform( int location, int v0, int v1, int v2, int v3 )const;
		inline bool SetUniform( int location, uint32_t v0, uint32_t v1 )const;
		inline bool SetUniform( int location, uint32_t v0, uint32_t v1, uint32_t v2 )const;
		inline bool SetUniform( int location, uint32_t v0, uint32_t v1, uint32_t v2, uint32_t v3 )const;
		inline bool SetUniform( int location, float v0, float v1 )const;
		inline bool SetUniform( int location, float v0, float v1, float v2 )const;
		inline bool SetUniform( int location, float v0, float v1, float v2, float v3 )const;
		inline bool SetUniform( int location, double v0, double v1 )const;
		inline bool SetUniform( int location, double v0, double v1, double v2 )const;
		inline bool SetUniform( int location, double v0, double v1, double v2, double v3 )const;
		inline bool SetUniform1v( int location, int count, int const * params )const;
		inline bool SetUniform2v( int location, int count, int const * params )const;
		inline bool SetUniform3v( int location, int count, int const * params )const;
		inline bool SetUniform4v( int location, int count, int const * params )const;
		inline bool SetUniform1v( int location, int count, uint32_t const * params )const;
		inline bool SetUniform2v( int location, int count, uint32_t const * params )const;
		inline bool SetUniform3v( int location, int count, uint32_t const * params )const;
		inline bool SetUniform4v( int location, int count, uint32_t const * params )const;
		inline bool SetUniform1v( int location, int count, float const * params )const;
		inline bool SetUniform2v( int location, int count, float const * params )const;
		inline bool SetUniform3v( int location, int count, float const * params )const;
		inline bool SetUniform4v( int location, int count, float const * params )const;
		inline bool SetUniform1v( int location, int count, double const * params )const;
		inline bool SetUniform2v( int location, int count, double const * params )const;
		inline bool SetUniform3v( int location, int count, double const * params )const;
		inline bool SetUniform4v( int location, int count, double const * params )const;
		inline bool SetUniformMatrix2x2v( int location, int count, bool transpose, float const * value )const;
		inline bool SetUniformMatrix2x3v( int location, int count, bool transpose, float const * value )const;
		inline bool SetUniformMatrix2x4v( int location, int count, bool transpose, float const * value )const;
		inline bool SetUniformMatrix3x3v( int location, int count, bool transpose, float const * value )const;
		inline bool SetUniformMatrix3x2v( int location, int count, bool transpose, float const * value )const;
		inline bool SetUniformMatrix3x4v( int location, int count, bool transpose, float const * value )const;
		inline bool SetUniformMatrix4x4v( int location, int count, bool transpose, float const * value )const;
		inline bool SetUniformMatrix4x2v( int location, int count, bool transpose, float const * value )const;
		inline bool SetUniformMatrix4x3v( int location, int count, bool transpose, float const * value )const;
		inline bool SetUniformMatrix2x2v( int location, int count, bool transpose, double const * value )const;
		inline bool SetUniformMatrix2x3v( int location, int count, bool transpose, double const * value )const;
		inline bool SetUniformMatrix2x4v( int location, int count, bool transpose, double const * value )const;
		inline bool SetUniformMatrix3x3v( int location, int count, bool transpose, double const * value )const;
		inline bool SetUniformMatrix3x2v( int location, int count, bool transpose, double const * value )const;
		inline bool SetUniformMatrix3x4v( int location, int count, bool transpose, double const * value )const;
		inline bool SetUniformMatrix4x4v( int location, int count, bool transpose, double const * value )const;
		inline bool SetUniformMatrix4x2v( int location, int count, bool transpose, double const * value )const;
		inline bool SetUniformMatrix4x3v( int location, int count, bool transpose, double const * value )const;

		//@}
		/**@name Uniform Buffer Objects Functions */
		//@{

		inline uint32_t GetUniformBlockIndex( uint32_t shader, char const * uniformBlockName )const;
		inline bool BindBufferBase( uint32_t target, uint32_t index, uint32_t buffer )const;
		inline bool UniformBlockBinding( uint32_t shader, uint32_t uniformBlockIndex, uint32_t uniformBlockBinding )const;
		inline bool GetUniformIndices( uint32_t shader, int uniformCount, char const ** uniformNames, uint32_t * uniformIndices )const;
		inline bool GetActiveUniformsiv( uint32_t shader, int uniformCount, uint32_t const * uniformIndices, eGL_UNIFORM_NAME pname, int * params )const;
		inline bool GetActiveUniformBlockiv( uint32_t shader, uint32_t uniformBlockIndex, eGL_UNIFORM_NAME pname, int * params )const;

		//@}
		/**@name Shader object Functions */
		//@{

		inline uint32_t CreateShader( eGL_SHADER_TYPE type )const;
		inline bool DeleteShader( uint32_t program )const;
		inline bool IsShader( uint32_t program )const;
		inline bool AttachShader( uint32_t program, uint32_t shader )const;
		inline bool DetachShader( uint32_t program, uint32_t shader )const;
		inline bool CompileShader( uint32_t program )const;
		inline bool GetShaderiv( uint32_t program, uint32_t pname, int * param )const;
		inline bool GetShaderInfoLog( uint32_t program, int bufSize, int * length, char * infoLog )const;
		inline bool ShaderSource( uint32_t program, int count, char const ** strings, int const * lengths )const;
		inline bool GetActiveAttrib( uint32_t program, uint32_t index, int bufSize, int * length, int * size, uint32_t * type, char * name )const;

		//@}
		/**@name Shader program Functions */
		//@{

		inline uint32_t CreateProgram()const;
		inline bool DeleteProgram( uint32_t program )const;
		inline bool LinkProgram( uint32_t program )const;
		inline bool UseProgram( uint32_t program )const;
		inline bool GetProgramiv( uint32_t program, uint32_t pname, int * param )const;
		inline bool GetProgramInfoLog( uint32_t program, int bufSize, int * length, char * infoLog )const;
		inline int GetAttribLocation( uint32_t program, char const * name )const;
		inline bool IsProgram( uint32_t program )const;
		inline bool ProgramParameteri( uint32_t program, uint32_t pname, int value )const;

		//@}
		/**@name Vertex Attribute Pointer functions */
		//@{

		inline bool EnableVertexAttribArray( uint32_t index )const;
		inline bool VertexAttribPointer( uint32_t index, int size, eGL_TYPE type, bool normalized, int stride, void const * pointer )const;
		inline bool VertexAttribPointer( uint32_t index, int size, eGL_TYPE type, int stride, void const * pointer )const;
		inline bool DisableVertexAttribArray( uint32_t index )const;

		//@}
		/**@name Vertex Array Objects */
		//@{

		inline bool GenVertexArrays( int n, uint32_t * arrays )const;
		inline bool IsVertexArray( uint32_t array )const;
		inline bool BindVertexArray( uint32_t array )const;
		inline bool DeleteVertexArrays( int n, uint32_t const * arrays )const;

		//@}
		/**@name Tesselation functions */
		//@{

		inline bool PatchParameter( eGL_PATCH_PARAMETER p_eParam, int p_iValue )const;

		//@}
		/**@name Query functions */
		//@{

		inline bool GenQueries( int p_n, uint32_t * p_queries )const;
		inline bool DeleteQueries( int p_n, uint32_t const * p_queries )const;
		inline bool IsQuery( uint32_t p_query )const;
		inline bool BeginQuery( eGL_QUERY p_target, uint32_t p_query )const;
		inline bool EndQuery( eGL_QUERY p_target )const;
		inline bool QueryCounter( uint32_t p_id, eGL_QUERY p_target )const;
		inline bool GetQueryObjectInfos( uint32_t p_id, eGL_QUERY_INFO p_name, int32_t * p_params )const;
		inline bool GetQueryObjectInfos( uint32_t p_id, eGL_QUERY_INFO p_name, uint32_t * p_params )const;
		inline bool GetQueryObjectInfos( uint32_t p_id, eGL_QUERY_INFO p_name, int64_t * p_params )const;
		inline bool GetQueryObjectInfos( uint32_t p_id, eGL_QUERY_INFO p_name, uint64_t * p_params )const;

		//@}
		/**@name GL_ARB_program_interface_query */
		//@{

		inline bool GetProgramInterfaceInfos( uint32_t program, eGLSL_INTERFACE programInterface, eGLSL_DATA_NAME name, int * params );
		inline int GetProgramResourceIndex( uint32_t program, eGLSL_INTERFACE programInterface, char const * const name );
		inline int GetProgramResourceLocation( uint32_t program, eGLSL_INTERFACE programInterface, char const * const name );
		inline int GetProgramResourceLocationIndex( uint32_t program, eGLSL_INTERFACE programInterface, char const * const name );
		inline bool GetProgramResourceName( uint32_t program, eGLSL_INTERFACE programInterface, uint32_t index, int bufSize, int * length, char * name );
		inline bool GetProgramResourceInfos( uint32_t program, eGLSL_INTERFACE programInterface, uint32_t index, int propCount, uint32_t * props, int bufSize, int * length, int * params );

		//@}
		/**@name Other functions */
		//@{

		inline eGL_LOCK GetLockFlags( uint32_t p_flags )const;
		inline uint32_t GetBitfieldFlags( uint32_t p_flags )const;
		inline Castor3D::eELEMENT_TYPE Get( eGLSL_ATTRIBUTE_TYPE p_type )const;

#if !defined( NDEBUG )

		C3D_Gl_API void Track( void * p_object, std::string const & p_name, std::string const & p_file, int line )const;
		C3D_Gl_API void UnTrack( void * p_object )const;

		template< typename CreationFunction, typename DestructionFunction >
		inline void Track( Object< CreationFunction, DestructionFunction > * p_object, std::string const & p_name, std::string const & p_file, int p_line )const
		{
			Track( reinterpret_cast< void * >( p_object ), p_name + cuT( " (OpenGL Name: " ) + Castor::string::to_string( p_object->GetGlName() ) + cuT( ")" ), p_file, p_line );
		}

#endif

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
		std::function< void( int x, int y, int width, int height, uint32_t format, uint32_t type, void * pixels ) > m_pfnReadPixels;
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
		std::function< void( uint32_t target, uint32_t attachment, uint32_t textarget, uint32_t texture, int level, int layer ) > m_pfnFramebufferTexture3D;
		std::function< void( int srcX0, int srcY0, int srcX1, int srcY1, int dstX0, int dstY0, int dstX1, int dstY1, uint32_t mask, uint32_t filter ) > m_pfnBlitFramebuffer;
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
		std::function< void( uint32_t target, int samples, int internalformat, int width, int height, uint8_t fixedsamplelocations ) > m_pfnTexImage2DMultisample;
		std::function< void( uint32_t target, uint32_t param, int * value ) > m_pfnGetRenderbufferParameteriv;

		//@}
		/**@name Uniform variables */
		//@{

		std::function< int( uint32_t program, char const * name ) > m_pfnGetUniformLocation;
		std::function< void( uint32_t program, uint32_t index, int maxLength, int * length, int * size, uint32_t * type, char * name ) > m_pfnGetActiveUniform;
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
		std::function< void( uint32_t program, uint32_t index, int bufSize, int * length, int * size, uint32_t * type, char * name ) > m_pfnGetActiveAttrib;

		//@}
		/**@name Vertex Attribute Pointer */
		//@{

		std::function< void( uint32_t ) > m_pfnEnableVertexAttribArray;
		std::function< void( uint32_t index, int size, uint32_t type, uint8_t normalized, int stride, void const * pointer ) > m_pfnVertexAttribPointer;
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
		/*@name Queries */
		//@{

		/** see https://www.opengl.org/sdk/docs/man/html/glGenQueries.xhtml
		*/
		std::function< void( int n, uint32_t * queries ) > m_pfnGenQueries;

		/** see https://www.opengl.org/sdk/docs/man/html/glDeleteQueries.xhtml
		*/
		std::function< void( int n, uint32_t const * queries ) > m_pfnDeleteQueries;

		/** see https://www.opengl.org/sdk/docs/man/html/glIsQuery.xhtml
		*/
		std::function< uint8_t( uint32_t query ) > m_pfnIsQuery;

		/** see https://www.opengl.org/sdk/docs/man/html/glBeginQuery.xhtml
		*/
		std::function< void( uint32_t target, uint32_t query ) > m_pfnBeginQuery;

		/** see https://www.opengl.org/sdk/docs/man/html/glEndQuery.xhtml
		*/
		std::function< void( uint32_t target ) > m_pfnEndQuery;

		/** see https://www.opengl.org/sdk/docs/man/html/glQueryCounter.xhtml
		*/
		std::function< void( uint32_t id, uint32_t target ) > m_pfnQueryCounter;

		/** see https://www.opengl.org/sdk/docs/man/html/glGetQueryObject.xhtml
		*/
		std::function< void( uint32_t id, uint32_t pname, int32_t * params ) > m_pfnGetQueryObjectiv;

		/** see https://www.opengl.org/sdk/docs/man/html/glGetQueryObject.xhtml
		*/
		std::function< void( uint32_t id, uint32_t pname, uint32_t * params ) > m_pfnGetQueryObjectuiv;

		/** see https://www.opengl.org/sdk/docs/man/html/glGetQueryObject.xhtml
		*/
		std::function< void( uint32_t id, uint32_t pname, int64_t * params ) > m_pfnGetQueryObjecti64v;

		/** see https://www.opengl.org/sdk/docs/man/html/glGetQueryObject.xhtml
		*/
		std::function< void( uint32_t id, uint32_t pname, uint64_t * params ) > m_pfnGetQueryObjectui64v;

		//@}
		/*@name GL_ARB_program_interface_query */
		//@{

		/** see https://www.opengl.org/sdk/docs/man/html/glGetProgramInterface.xhtml
		*/
		std::function< void( uint32_t program, uint32_t programInterface, uint32_t name, int * params ) > m_pfnGetProgramInterfaceiv;

		/** see https://www.opengl.org/sdk/docs/man/html/glGetProgramResourceIndex.xhtml
		*/
		std::function< int( uint32_t program, uint32_t programInterface, char const * const name ) > m_pfnGetProgramResourceIndex;

		/** see https://www.opengl.org/sdk/docs/man/html/glGetProgramResourceLocation.xhtml
		*/
		std::function< int( uint32_t program, uint32_t programInterface, char const * const name ) > m_pfnGetProgramResourceLocation;

		/** see https://www.opengl.org/sdk/docs/man/html/glGetProgramResourceLocationIndex.xhtml
		*/
		std::function< int( uint32_t program, uint32_t programInterface, char const * const name ) > m_pfnGetProgramResourceLocationIndex;

		/** see https://www.opengl.org/sdk/docs/man/html/glGetProgramResourceName.xhtml
		*/
		std::function< void( uint32_t program, uint32_t programInterface, uint32_t index, int bufSize, int * length, char * name ) > m_pfnGetProgramResourceName;

		/** see https://www.opengl.org/sdk/docs/man/html/glGetProgramResource.xhtml
		*/
		std::function< void( uint32_t program, uint32_t programInterface, uint32_t index, int propCount, uint32_t * props, int bufSize, int * length, int * params ) > m_pfnGetProgramResourceiv;

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
			return p_func != nullptr;
		}

		template< typename Ret, typename ... Arguments >
		bool GetFunction( Castor::String const & p_name, std::function< Ret( Arguments... ) > & p_func )
		{
			typedef Ret( CALLBACK * PFNType )( Arguments... );
			PFNType l_pfnResult = nullptr;

			if ( GetFunction( p_name, l_pfnResult ) )
			{
				p_func = l_pfnResult;
			}

			return l_pfnResult != nullptr;
		}
	}

#	define MAKE_GL_EXTENSION( x )	static const Castor::String x = cuT( "GL_" ) cuT( #x );

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
	MAKE_GL_EXTENSION( ARB_program_interface_query )

#	if defined( _WIN32 )

#	define MAKE_WGL_EXTENSION( x )	static const Castor::String x = cuT( "WGL_" ) cuT( #x );
	MAKE_WGL_EXTENSION( ARB_create_context )
	MAKE_WGL_EXTENSION( ARB_pixel_format )
	MAKE_WGL_EXTENSION( EXT_swap_control )

#	elif defined( __linux__ )

#	define MAKE_GLX_EXTENSION( x )	static const Castor::String x = cuT( "GLX_" ) cuT( #x );
	MAKE_GLX_EXTENSION( ARB_create_context )
	MAKE_GLX_EXTENSION( EXT_swap_control )

#	else

#		error "Yet unsupported OS"

#	endif
}

#include "OpenGl.inl"

#endif
