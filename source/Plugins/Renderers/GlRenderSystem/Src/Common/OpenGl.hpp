/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___C3DGL_GlObjects___
#define ___C3DGL_GlObjects___

#include <Config/PlatformConfig.hpp>

#if defined( CASTOR_PLATFORM_WINDOWS )
#	include <Windows.h>
#elif defined( CASTOR_PLATFORM_LINUX )
#	include <X11/Xlib.h>
#	include <GL/glx.h>
#	define GLX_GLXEXT_PROTOTYPES
#	include <GL/glxext.h>
#endif
#include <GL/gl.h>

#include "Common/GlObject.hpp"
#include "Miscellaneous/GlDebug.hpp"

#include <Log/Logger.hpp>

#include <GlslWriterPrerequisites.hpp>

#include <Texture/TextureUnit.hpp>

#if defined( MemoryBarrier )
#	undef MemoryBarrier
#endif

#define GL_USE_STD_FUNCTION 0

#if !defined( CALLBACK )
#	if defined( CASTOR_PLATFORM_WINDOWS )
#		define CALLBACK __stdcall
#	else
#		define CALLBACK
#	endif
#endif

namespace GlRender
{
	//*********************************************************************************************

	template< typename TRet, typename ... TParams >
	class GlFunction
	{
	public:

		using CFuncType = TRet( CALLBACK * )( TParams... );

#if GL_USE_STD_FUNCTION

		using FuncType = std::function< TRet( TParams... ) >;

		inline GlFunction( CFuncType p_function )
			: m_function{ p_function }
		{
		}

		inline GlFunction & operator=( CFuncType p_function )
		{
			m_function = p_function;
			return *this;
		}

#else

		using FuncType = CFuncType;

#endif

		inline GlFunction() = default;

		template< typename FuncT >
		inline GlFunction( FuncT p_function )
			: m_function{ reinterpret_cast< FuncType >( p_function ) }
		{
		}

		template< typename FuncT >
		inline GlFunction & operator=( FuncT p_function )
		{
			m_function = reinterpret_cast< FuncType >( p_function );
			return *this;
		}

		inline TRet operator()( TParams ... p_params )const
		{
			REQUIRE( m_function );
			return m_function( std::forward< TParams >( p_params )... );
		}

		bool operator==( nullptr_t )const
		{
			return m_function == nullptr;
		}

		bool operator!=( nullptr_t )const
		{
			return m_function != nullptr;
		}

	private:
		FuncType m_function{ nullptr };
	};

	//*********************************************************************************************

	class TexFunctionsBase
		: public Holder
	{
	public:
		TexFunctionsBase( OpenGl & p_gl );
		virtual void GenerateMipmap( GlTexDim p_target )const = 0;
		virtual void BindTexture( GlTexDim p_target, uint32_t texture )const = 0;
		virtual void TexSubImage1D( GlTextureStorageType p_target, int level, int xoffset, int width, GlFormat format, GlType type, void const * data )const = 0;
		virtual void TexSubImage2D( GlTextureStorageType p_target, int level, int xoffset, int yoffset, int width, int height, GlFormat format, GlType type, void const * data )const = 0;
		virtual void TexSubImage2D( GlTextureStorageType p_target, int level, Castor::Position const & p_position, Castor::Size const & p_size, GlFormat format, GlType type, void const * data )const = 0;
		virtual void TexSubImage2D( GlTextureStorageType p_target, int level, Castor::Rectangle const & p_rect, GlFormat format, GlType type, void const * data )const = 0;
		virtual void TexSubImage3D( GlTextureStorageType p_target, int level, int xoffset, int yoffset, int zoffset, int width, int height, int depth, GlFormat format, GlType type, void const * data )const = 0;
		virtual void TexImage1D( GlTextureStorageType p_target, int level, GlInternal internalFormat, int width, int border, GlFormat format, GlType type, void const * data )const = 0;
		virtual void TexImage2D( GlTextureStorageType p_target, int level, GlInternal internalFormat, int width, int height, int border, GlFormat format, GlType type, void const * data )const = 0;
		virtual void TexImage2D( GlTextureStorageType p_target, int level, GlInternal internalFormat, Castor::Size const & p_size, int border, GlFormat format, GlType type, void const * data )const = 0;
		virtual void TexImage3D( GlTextureStorageType p_target, int level, GlInternal internalFormat, int width, int height, int depth, int border, GlFormat format, GlType type, void const * data )const = 0;
		virtual void GetTexImage( GlTextureStorageType p_target, int level, GlFormat format, GlType type, void * img )const = 0;
	};

	class TexFunctions
		: public TexFunctionsBase
	{
	public:
		TexFunctions( OpenGl & p_gl );
		inline void BindTexture( GlTexDim p_target, uint32_t texture )const override;
		inline void GenerateMipmap( GlTexDim p_target )const override;
		inline void TexSubImage1D( GlTextureStorageType p_target, int level, int xoffset, int width, GlFormat format, GlType type, void const * data )const override;
		inline void TexSubImage2D( GlTextureStorageType p_target, int level, int xoffset, int yoffset, int width, int height, GlFormat format, GlType type, void const * data )const override;
		inline void TexSubImage2D( GlTextureStorageType p_target, int level, Castor::Position const & p_position, Castor::Size const & p_size, GlFormat format, GlType type, void const * data )const override;
		inline void TexSubImage2D( GlTextureStorageType p_target, int level, Castor::Rectangle const & p_rect, GlFormat format, GlType type, void const * data )const override;
		inline void TexSubImage3D( GlTextureStorageType p_target, int level, int xoffset, int yoffset, int zoffset, int width, int height, int depth, GlFormat format, GlType type, void const * data )const override;
		inline void TexImage1D( GlTextureStorageType p_target, int level, GlInternal internalFormat, int width, int border, GlFormat format, GlType type, void const * data )const override;
		inline void TexImage2D( GlTextureStorageType p_target, int level, GlInternal internalFormat, int width, int height, int border, GlFormat format, GlType type, void const * data )const override;
		inline void TexImage2D( GlTextureStorageType p_target, int level, GlInternal internalFormat, Castor::Size const & p_size, int border, GlFormat format, GlType type, void const * data )const override;
		inline void TexImage3D( GlTextureStorageType p_target, int level, GlInternal internalFormat, int width, int height, int depth, int border, GlFormat format, GlType type, void const * data )const override;
		inline void GetTexImage( GlTextureStorageType p_target, int level, GlFormat format, GlType type, void * img )const override;

		GlFunction< void, uint32_t, uint32_t > m_pfnBindTexture;
		GlFunction< void, uint32_t, int , int , int , uint32_t , uint32_t , void const * > m_pfnTexSubImage1D;
		GlFunction< void, uint32_t, int , int , int , int , int , uint32_t , uint32_t , void const * > m_pfnTexSubImage2D;
		GlFunction< void, uint32_t, int , int , int , int , int , int , int , uint32_t , uint32_t , void const * > m_pfnTexSubImage3D;
		GlFunction< void, uint32_t, int , int , int , int , uint32_t , uint32_t , void const * > m_pfnTexImage1D;
		GlFunction< void, uint32_t, int , int , int , int , int , uint32_t , uint32_t , void const * > m_pfnTexImage2D;
		GlFunction< void, uint32_t, int , int , int , int , int , int , uint32_t , uint32_t , void const * > m_pfnTexImage3D;
		GlFunction< void, uint32_t, int , uint32_t , uint32_t , void * > m_pfnGetTexImage;
		GlFunction< void, uint32_t > m_pfnGenerateMipmap;
	};

	class TexFunctionsDSA
		: public TexFunctionsBase
	{
	public:
		TexFunctionsDSA( OpenGl & p_gl );
		inline void BindTexture( GlTexDim /*p_target*/, uint32_t texture )const override
		{
			m_uiTexture = texture;
		}
		inline void GenerateMipmap( GlTexDim p_target )const override;
		inline void TexSubImage1D( GlTextureStorageType p_target, int level, int xoffset, int width, GlFormat format, GlType type, void const * data )const override;
		inline void TexSubImage2D( GlTextureStorageType p_target, int level, int xoffset, int yoffset, int width, int height, GlFormat format, GlType type, void const * data )const override;
		inline void TexSubImage2D( GlTextureStorageType p_target, int level, Castor::Position const & p_position, Castor::Size const & p_size, GlFormat format, GlType type, void const * data )const override;
		inline void TexSubImage2D( GlTextureStorageType p_target, int level, Castor::Rectangle const & p_rect, GlFormat format, GlType type, void const * data )const override;
		inline void TexSubImage3D( GlTextureStorageType p_target, int level, int xoffset, int yoffset, int zoffset, int width, int height, int depth, GlFormat format, GlType type, void const * data )const override;
		inline void TexImage1D( GlTextureStorageType p_target, int level, GlInternal internalFormat, int width, int border, GlFormat format, GlType type, void const * data )const override;
		inline void TexImage2D( GlTextureStorageType p_target, int level, GlInternal internalFormat, int width, int height, int border, GlFormat format, GlType type, void const * data )const override;
		inline void TexImage2D( GlTextureStorageType p_target, int level, GlInternal internalFormat, Castor::Size const & p_size, int border, GlFormat format, GlType type, void const * data )const override;
		inline void TexImage3D( GlTextureStorageType p_target, int level, GlInternal internalFormat, int width, int height, int depth, int border, GlFormat format, GlType type, void const * data )const override;
		inline void GetTexImage( GlTextureStorageType p_target, int level, GlFormat format, GlType type, void * img )const override;

		mutable uint32_t m_uiTexture;
		GlFunction< void, uint32_t, uint32_t , int , int , int , uint32_t , uint32_t , void const * > m_pfnTextureSubImage1D;
		GlFunction< void, uint32_t, uint32_t , int , int , int , int , int , uint32_t , uint32_t , void const * > m_pfnTextureSubImage2D;
		GlFunction< void, uint32_t, uint32_t , int , int , int , int , int , int , int , uint32_t , uint32_t , void const * > m_pfnTextureSubImage3D;
		GlFunction< void, uint32_t, uint32_t , int , int , int , int , uint32_t , uint32_t , void const * > m_pfnTextureImage1D;
		GlFunction< void, uint32_t, uint32_t , int , int , int , int , int , uint32_t , uint32_t , void const * > m_pfnTextureImage2D;
		GlFunction< void, uint32_t, uint32_t , int , int , int , int , int , int , uint32_t , uint32_t , void const * > m_pfnTextureImage3D;
		GlFunction< void, uint32_t, uint32_t , int , uint32_t , uint32_t , void * > m_pfnGetTextureImage;
		GlFunction< void, uint32_t, uint32_t > m_pfnGenerateMipmap;
	};

	class BufFunctionsBase
		: public Holder
	{
	public:
		BufFunctionsBase( OpenGl & p_gl );
		virtual void BindBuffer( GlBufferTarget target, uint32_t buffer )const = 0;
		virtual void BufferData( GlBufferTarget target, ptrdiff_t size, void const * data, GlBufferMode usage )const = 0;
		virtual void BufferSubData( GlBufferTarget target, ptrdiff_t offset, ptrdiff_t size, void const * data )const = 0;
		virtual void CopyBufferSubData( GlBufferTarget readtarget, GlBufferTarget writetarget, ptrdiff_t readoffset, ptrdiff_t writeoffset, ptrdiff_t size )const = 0;
		virtual void * MapBuffer( GlBufferTarget target, GlAccessType access )const = 0;
		virtual void UnmapBuffer( GlBufferTarget target )const = 0;
		virtual void * MapBufferRange( GlBufferTarget target, ptrdiff_t offset, ptrdiff_t length, uint32_t access )const = 0;
		virtual void GetBufferParameter( GlBufferTarget target, GlBufferParameter pname, int * params )const = 0;
		virtual void FlushMappedBufferRange( GlBufferTarget target, ptrdiff_t offset, ptrdiff_t length )const = 0;

		/*@name NV_vertex_buffer_unified_memory extension */
		//@{

		inline void BufferAddressRange( GlAddress pname, uint32_t index, uint64_t address, size_t length )const;
		inline void VertexFormat( int size, GlType type, int stride )const;
		inline void NormalFormat( GlType type, int stride )const;
		inline void ColorFormat( int size, GlType type, int stride )const;
		inline void IndexFormat( GlType type, int stride )const;
		inline void TexCoordFormat( int size, GlType type, int stride )const;
		inline void EdgeFlagFormat( int stride )const;
		inline void SecondaryColorFormat( int size, GlType type, int stride )const;
		inline void FogCoordFormat( uint32_t type, int stride )const;
		inline void VertexAttribFormat( uint32_t index, int size, GlType type, bool normalized, int stride )const;
		inline void VertexAttribIFormat( uint32_t index, int size, GlType type, int stride )const;

		//@}
		/*@name NV_shader_buffer_load extension */
		//@{

		inline void MakeBufferResident( GlBufferTarget target, GlAccessType access )const;
		inline void MakeBufferNonResident( GlBufferTarget target )const;
		inline bool IsBufferResident( GlBufferTarget target )const;
		inline void MakeNamedBufferResident( uint32_t buffer, GlAccessType access )const;
		inline void MakeNamedBufferNonResident( uint32_t buffer )const;
		inline bool IsNamedBufferResident( uint32_t buffer )const;
		inline void GetBufferParameter( GlBufferTarget target, GlBufferParameter pname, uint64_t * params )const;
		inline void GetNamedBufferParameter( uint32_t buffer, GlBufferParameter pname,  uint64_t * params )const;

		//@}
		/*@name NV_vertex_buffer_unified_memory extension */
		//@{

		GlFunction< void, uint32_t, uint32_t , uint64_t , size_t > m_pfnBufferAddressRange;
		GlFunction< void, int, uint32_t , int > m_pfnVertexFormat;
		GlFunction< void, uint32_t, int > m_pfnNormalFormat;
		GlFunction< void, int, uint32_t , int > m_pfnColorFormat;
		GlFunction< void, uint32_t, int > m_pfnIndexFormat;
		GlFunction< void, int, uint32_t , int > m_pfnTexCoordFormat;
		GlFunction< void, int > m_pfnEdgeFlagFormat;
		GlFunction< void, int, uint32_t , int > m_pfnSecondaryColorFormat;
		GlFunction< void, uint32_t, int > m_pfnFogCoordFormat;
		GlFunction< void, uint32_t, int , uint32_t , bool , int > m_pfnVertexAttribFormat;
		GlFunction< void, uint32_t, int , uint32_t , int > m_pfnVertexAttribIFormat;

		//@}
		/*@name NV_vertex_buffer_unified_memory extension */
		//@{

		GlFunction< void, uint32_t, uint32_t > m_pfnMakeBufferResident;
		GlFunction< void, uint32_t > m_pfnMakeBufferNonResident;
		GlFunction< bool, uint32_t > m_pfnIsBufferResident;
		GlFunction< void, uint32_t, uint32_t > m_pfnMakeNamedBufferResident;
		GlFunction< void, uint32_t > m_pfnMakeNamedBufferNonResident;
		GlFunction< bool, uint32_t > m_pfnIsNamedBufferResident;
		GlFunction< void, uint32_t, uint32_t , uint64_t *> m_pfnGetBufferParameterui64v;
		GlFunction< void, uint32_t, uint32_t , uint64_t *> m_pfnGetNamedBufferParameterui64v;

		//@}
	};

	class BufFunctions
		: public BufFunctionsBase
	{
	public:
		BufFunctions( OpenGl & p_gl );
		inline void BindBuffer( GlBufferTarget target, uint32_t buffer )const override;
		inline void BufferData( GlBufferTarget target, ptrdiff_t size, void const * data, GlBufferMode usage )const override;
		inline void BufferSubData( GlBufferTarget target, ptrdiff_t offset, ptrdiff_t size, void const * data )const override;
		inline void CopyBufferSubData( GlBufferTarget readtarget, GlBufferTarget writetarget, ptrdiff_t readoffset, ptrdiff_t writeoffset, ptrdiff_t size )const override;
		inline void * MapBuffer( GlBufferTarget target, GlAccessType access )const override;
		inline void UnmapBuffer( GlBufferTarget target )const override;
		inline void * MapBufferRange( GlBufferTarget target, ptrdiff_t offset, ptrdiff_t length, uint32_t access )const override;
		inline void GetBufferParameter( GlBufferTarget target, GlBufferParameter pname, int * params )const override;
		inline void FlushMappedBufferRange( GlBufferTarget target, ptrdiff_t offset, ptrdiff_t length )const override;

		GlFunction< void, uint32_t, uint32_t > m_pfnBindBuffer;
		GlFunction< void, uint32_t, ptrdiff_t , void const *, uint32_t > m_pfnBufferData;
		GlFunction< void, uint32_t, ptrdiff_t , ptrdiff_t , void const * > m_pfnBufferSubData;
		GlFunction< void, uint32_t, uint32_t , ptrdiff_t , ptrdiff_t , ptrdiff_t > m_pfnCopyBufferSubData;
		GlFunction< void *, uint32_t, uint32_t > m_pfnMapBuffer;
		GlFunction< uint8_t, uint32_t > m_pfnUnmapBuffer;
		GlFunction< void *, uint32_t, ptrdiff_t , ptrdiff_t , uint32_t > m_pfnMapBufferRange;
		GlFunction< void, uint32_t, ptrdiff_t , ptrdiff_t > m_pfnFlushMappedBufferRange;
		GlFunction< void, uint32_t, uint32_t , int *> m_pfnGetBufferParameteriv;
	};

	class BufFunctionsDSA
		: public BufFunctionsBase
	{
	public:
		BufFunctionsDSA( OpenGl & p_gl );
		inline void BindBuffer( GlBufferTarget /*target*/, uint32_t buffer )const override
		{
			m_uiBuffer = buffer;
		}
		inline void BufferData( GlBufferTarget target, ptrdiff_t size, void const * data, GlBufferMode usage )const override;
		inline void BufferSubData( GlBufferTarget target, ptrdiff_t offset, ptrdiff_t size, void const * data )const override;
		inline void CopyBufferSubData( GlBufferTarget readtarget, GlBufferTarget writetarget, ptrdiff_t readoffset, ptrdiff_t writeoffset, ptrdiff_t size )const override;
		inline void * MapBuffer( GlBufferTarget target, GlAccessType access )const override;
		inline void UnmapBuffer( GlBufferTarget target )const override;
		inline void * MapBufferRange( GlBufferTarget target, ptrdiff_t offset, ptrdiff_t length, uint32_t access )const override;
		inline void GetBufferParameter( GlBufferTarget target, GlBufferParameter pname, int * params )const override;
		inline void FlushMappedBufferRange( GlBufferTarget target, ptrdiff_t offset, ptrdiff_t length )const override;

		mutable uint32_t m_uiBuffer;
		GlFunction< void, uint32_t, ptrdiff_t , void const *, uint32_t > m_pfnNamedBufferData;
		GlFunction< void, uint32_t, ptrdiff_t , ptrdiff_t , void const * > m_pfnNamedBufferSubData;
		GlFunction< void, uint32_t, uint32_t , ptrdiff_t , ptrdiff_t , ptrdiff_t > m_pfnCopyNamedBufferSubData;
		GlFunction< void *, uint32_t, uint32_t > m_pfnMapNamedBuffer;
		GlFunction< uint8_t, uint32_t > m_pfnUnmapNamedBuffer;
		GlFunction< void *, uint32_t, ptrdiff_t , ptrdiff_t , uint32_t > m_pfnMapNamedBufferRange;
		GlFunction< void, uint32_t, ptrdiff_t , ptrdiff_t > m_pfnFlushMappedNamedBufferRange;
		GlFunction< void, uint32_t, uint32_t , int *> m_pfnGetNamedBufferParameteriv;
	};

	class OpenGl
		: public Castor::NonCopyable
	{
	public:
		struct PixelFmt
		{
			GlFormat Format;
			GlInternal Internal;
			GlType Type;

			PixelFmt() {}

			PixelFmt( GlFormat p_format, GlInternal p_eInternal, GlType p_type )
				: Format( p_format )
				, Internal( p_eInternal )
				, Type( p_type )
			{
			}
		};

	public:
		C3D_Gl_API OpenGl( GlRenderSystem & p_renderSystem );
		C3D_Gl_API ~OpenGl();
		C3D_Gl_API bool PreInitialise( Castor::String const & p_strExtensions );
		C3D_Gl_API bool Initialise();
		C3D_Gl_API void InitialiseDebug();
		C3D_Gl_API void Cleanup();
		C3D_Gl_API bool GlCheckError( std::string const & p_strText )const;
		C3D_Gl_API bool GlCheckError( std::wstring const & p_strText )const;
		C3D_Gl_API void DisplayExtensions()const;

		inline OpenGl const & GetOpenGl()const
		{
			return *this;
		}

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
		inline bool HasSsbo()const;
		inline bool HasInstancing()const;
		inline bool HasComputeVariableGroupSize()const;
		inline bool HasNonPowerOfTwoTextures()const;
		inline bool CanBindVboToGpuAddress()const;
		inline Castor::String const & GetGlslErrorString( int p_index )const;
		inline GlTopology Get( Castor3D::Topology p_index )const;
		inline GlTexDim Get( Castor3D::TextureType p_index )const;
		inline GlComparator Get( Castor3D::ComparisonFunc p_eAlphaFunc )const;
		inline GlWrapMode Get( Castor3D::WrapMode p_eWrapMode )const;
		inline GlInterpolationMode Get( Castor3D::InterpolationMode p_interpolation )const;
		inline GlBlendSource Get( Castor3D::BlendSource p_eArgument )const;
		inline GlBlendFunc Get( Castor3D::TextureBlendFunc p_mode )const;
		inline GlBlendFactor Get( Castor3D::BlendOperand p_eBlendFactor )const;
		inline PixelFmt const & Get( Castor::PixelFormat p_pixelFormat )const;
		inline GlShaderType Get( Castor3D::ShaderType p_type )const;
		inline GlInternal GetInternal( Castor::PixelFormat p_format )const;
		inline Castor::FlagCombination< GlBufferBit > GetComponents( Castor3D::BufferComponents const & p_components )const;
		inline GlComponent GetComponent( Castor3D::AttachmentPoint p_component )const;
		inline GlAttachmentPoint Get( Castor3D::AttachmentPoint p_eAttachment )const;
		inline GlFrameBufferMode Get( Castor3D::FrameBufferTarget p_target )const;
		inline GlInternal GetRboStorage( Castor::PixelFormat p_pixelFormat )const;
		inline GlBufferBinding Get( Castor3D::WindowBuffer p_buffer )const;
		inline GlBufferBinding Get( GlAttachmentPoint p_buffer )const;
		inline GlFace Get( Castor3D::Culling p_eFace )const;
		inline GlFillMode Get( Castor3D::FillMode p_mode )const;
		inline GlComparator Get( Castor3D::StencilFunc p_func )const;
		inline GlStencilOp Get( Castor3D::StencilOp p_eOp )const;
		inline GlBlendOp Get( Castor3D::BlendOperation p_eOp )const;
		inline GlComparator Get( Castor3D::DepthFunc p_func )const;
		inline GlQueryType Get( Castor3D::QueryType p_value )const;
		inline GlQueryInfo Get( Castor3D::QueryInfo p_value )const;
		inline GlTextureStorageType Get( Castor3D::TextureStorageType p_value )const;
		inline GlTexDim Get( Castor3D::CubeMapFace p_value )const;
		inline GlCompareMode Get( Castor3D::ComparisonMode p_value )const;
		inline bool Get( Castor3D::WritingMask p_eMask )const;
		inline Castor::FlagCombination< GlBarrierBit > Get( Castor3D::MemoryBarriers const & p_barriers )const;
		inline Castor::String const & GetVendor()const;
		inline Castor::String const & GetRenderer()const;
		inline Castor::String const & GetStrVersion()const;
		inline int GetVersion()const;
		inline int GetGlslVersion()const;
		inline GlRenderSystem & GetRenderSystem();
		inline GlRenderSystem const & GetRenderSystem()const;
		inline bool HasExtension( Castor::String const & p_strExtName, bool p_log = true )const;
		inline GlBufferMode GetBufferFlags( uint32_t p_flags )const;
		inline GlProvider GetProvider()const;

		/**@name General Functions */
		//@{

		inline void ClearColor( float red, float green, float blue, float alpha )const;
		inline void ClearColor( Castor::Colour const & p_colour )const;
		inline void ClearDepth( double value )const;
		inline void Clear( uint32_t mask )const;
		inline void Enable( GlTweak mode )const;
		inline void Disable( GlTweak mode )const;
		inline void Enable( GlTexDim texture )const;
		inline void Disable( GlTexDim texture )const;
		inline void SelectBuffer( int size, uint32_t * buffer )const;
		inline void GetIntegerv( uint32_t pname, int * params )const;
		inline void GetIntegerv( GlMax pname, int * params )const;
		inline void GetIntegerv( GlMin pname, int * params )const;
		inline void GetIntegerv( GlGpuInfo pname, int * params )const;
		inline void GetIntegerv( uint32_t pname, uint64_t * params )const;
		inline void DepthFunc( GlComparator p_func )const;
		inline void DepthMask( bool p_bFlag )const;
		inline void ColorMask( bool p_r, bool p_g, bool p_b, bool p_a )const;
		inline void PolygonMode( GlFace p_eFacing, GlFillMode p_mode )const;
		inline void StencilOp( GlStencilOp p_eStencilFail, GlStencilOp p_eDepthFail, GlStencilOp p_eStencilPass )const;
		inline void StencilFunc( GlComparator p_func, int p_iRef, uint32_t p_uiMask )const;
		inline void StencilMask( uint32_t p_uiMask )const;
		inline void StencilOpSeparate( GlFace p_eFacing, GlStencilOp p_eStencilFail, GlStencilOp p_eDepthFail, GlStencilOp p_eStencilPass )const;
		inline void StencilFuncSeparate( GlFace p_eFacing, GlComparator p_func, int p_iRef, uint32_t p_uiMask )const;
		inline void StencilMaskSeparate( GlFace p_eFacing, uint32_t p_uiMask )const;
		inline void Hint( GlHint p_eHint, GlHintValue p_eValue )const;
		inline void PolygonOffset( float p_fFactor, float p_fUnits )const;
		inline void BlendColor( Castor::Colour const & p_clrFactors )const;
		inline void SampleCoverage( float fValue, bool invert )const;
		inline void Viewport( int x, int y, int width, int height )const;

		//@}
		/**@name Draw Functions */
		//@{

		inline void DrawArrays( GlTopology mode, int first, int count )const;
		inline void DrawElements( GlTopology mode, int count, GlType type, const void * indices )const;
		inline void DrawArraysInstanced( GlTopology mode, int first, int count, int primcount )const;
		inline void DrawElementsInstanced( GlTopology mode, int count, GlType type, const void * indices, int primcount )const;

		//@}
		/**@name Instanciation Functions */
		//@{

		inline void VertexAttribDivisor( uint32_t index, uint32_t divisor )const;

		//@}
		/**@name Context functions */
		//@{

#if defined( CASTOR_PLATFORM_WINDOWS )

		inline void MakeCurrent( HDC hdc, HGLRC hglrc )const;
		inline void SwapBuffers( HDC hdc )const;
		inline void SwapInterval( int interval )const;
		inline HGLRC CreateContext( HDC hdc )const;
		inline HGLRC CreateContextAttribs( HDC hDC, HGLRC hShareContext, int const * attribList )const;
		inline bool DeleteContext( HGLRC hContext )const;
		inline bool HasCreateContextAttribs()const;

#elif defined( CASTOR_PLATFORM_LINUX )

		inline void MakeCurrent( Display * pDisplay, GLXDrawable drawable, GLXContext context )const;
		inline void SwapBuffers( Display * pDisplay, GLXDrawable drawable )const;
		inline void SwapInterval( Display * pDisplay, GLXDrawable drawable, int interval )const;
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

		inline void CullFace( GlFace face )const;
		inline void FrontFace( GlFrontFaceDirection face )const;
		inline void BlendFunc( GlBlendFactor sfactor, GlBlendFactor dfactor )const;
		inline void BlendFunc( GlBlendFactor p_eRgbSrc, GlBlendFactor p_eRgbDst, GlBlendFactor p_eAlphaSrc, GlBlendFactor p_eAlphaDst )const;
		inline void BlendFunc( uint32_t p_index, GlBlendFactor p_eRgbSrc, GlBlendFactor p_eRgbDst, GlBlendFactor p_eAlphaSrc, GlBlendFactor p_eAlphaDst )const;
		inline void BlendEquation( GlBlendOp p_eOp )const;
		inline void BlendEquation( uint32_t p_uiBuffer, GlBlendOp p_eOp )const;
		inline void ComparisonFunc( GlComparator func, float ref )const;

		//@}
		/**@name Texture functions */
		//@{

		inline void GenTextures( int n, uint32_t * textures )const;
		inline void DeleteTextures( int n, uint32_t const * textures )const;
		inline bool IsTexture( uint32_t texture )const;
		inline void BindTexture( GlTexDim p_target, uint32_t texture )const;
		inline void ActiveTexture( GlTextureIndex target )const;
		inline void ClientActiveTexture( GlTextureIndex target )const;
		inline void GenerateMipmap( GlTexDim p_target )const;
		inline void TexSubImage1D( GlTextureStorageType p_target, int level, int xoffset, int width, GlFormat format, GlType type, void const * data )const;
		inline void TexSubImage2D( GlTextureStorageType p_target, int level, int xoffset, int yoffset, int width, int height, GlFormat format, GlType type, void const * data )const;
		inline void TexSubImage2D( GlTextureStorageType p_target, int level, Castor::Position const & p_position, Castor::Size const & p_size, GlFormat format, GlType type, void const * data )const;
		inline void TexSubImage2D( GlTextureStorageType p_target, int level, Castor::Rectangle const & p_rect, GlFormat format, GlType type, void const * data )const;
		inline void TexSubImage3D( GlTextureStorageType p_target, int level, int xoffset, int yoffset, int zoffset, int width, int height, int depth, GlFormat format, GlType type, void const * data )const;
		inline void TexImage1D( GlTextureStorageType p_target, int level, GlInternal internalFormat, int width, int border, GlFormat format, GlType type, void const * data )const;
		inline void TexImage2D( GlTextureStorageType p_target, int level, GlInternal internalFormat, int width, int height, int border, GlFormat format, GlType type, void const * data )const;
		inline void TexImage2D( GlTextureStorageType p_target, int level, GlInternal internalFormat, Castor::Size const & p_size, int border, GlFormat format, GlType type, void const * data )const;
		inline void TexImage3D( GlTextureStorageType p_target, int level, GlInternal internalFormat, int width, int height, int depth, int border, GlFormat format, GlType type, void const * data )const;
		inline void TexImage2DMultisample( GlTextureStorageType p_target, int p_samples, GlInternal p_internalFormat, int p_width, int p_height, bool p_fixedSampleLocations )const;
		inline void TexImage2DMultisample( GlTextureStorageType p_target, int p_samples, GlInternal p_internalFormat, Castor::Size const & p_size, bool p_fixedSampleLocations )const;
		inline void GetTexImage( GlTextureStorageType p_target, int level, GlFormat format, GlType type, void * img )const;
		inline void ReadBuffer( GlBufferBinding p_buffer )const;
		inline void ReadPixels( int x, int y, int width, int height, GlFormat format, GlType type, void * pixels )const;
		inline void ReadPixels( Castor::Position const & p_position, Castor::Size const & p_size, GlFormat format, GlType type, void * pixels )const;
		inline void ReadPixels( Castor::Rectangle const & p_rect, GlFormat format, GlType type, void * pixels )const;
		inline void DrawBuffer( GlBufferBinding p_buffer )const;
		inline void DrawPixels( int width, int height, GlFormat format, GlType type, void const * data )const;
		inline void PixelStore( GlStorageMode p_mode, int p_iParam )const;
		inline void PixelStore( GlStorageMode p_mode, float p_fParam )const;
		inline void TexStorage1D( GlTextureStorageType target, GLint levels, GlInternal internalformat, GLsizei width )const;
		inline void TexStorage2D( GlTextureStorageType target, GLint levels, GlInternal internalformat, GLsizei width, GLsizei height )const;
		inline void TexStorage3D( GlTextureStorageType target, GLint levels, GlInternal internalformat, GLsizei width, GLsizei height, GLsizei depth )const;
		inline void TexStorage2DMultisample( GlTextureStorageType target, GLsizei samples, GlInternal internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations )const;
		inline void TexStorage3DMultisample( GlTextureStorageType target, GLsizei samples, GlInternal internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations )const;

		//@}
		/**@name Sampler functions */
		//@{

		inline void GenSamplers( int count, uint32_t * samplers )const;
		inline void DeleteSamplers( int count, const uint32_t * samplers )const;
		inline bool IsSampler( uint32_t sampler )const;
		inline void BindSampler( uint32_t unit, uint32_t sampler )const;
		inline void GetSamplerParameter( uint32_t sampler, GlSamplerParameter pname, uint32_t * params )const;
		inline void GetSamplerParameter( uint32_t sampler, GlSamplerParameter pname, float * params )const;
		inline void GetSamplerParameter( uint32_t sampler, GlSamplerParameter pname, int * params )const;
		inline void SetSamplerParameter( uint32_t sampler, GlSamplerParameter pname, float param )const;
		inline void SetSamplerParameter( uint32_t sampler, GlSamplerParameter pname, const float * params )const;
		inline void SetSamplerParameter( uint32_t sampler, GlSamplerParameter pname, int param )const;
		inline void SetSamplerParameter( uint32_t sampler, GlSamplerParameter pname, const int * params )const;
		inline void SetSamplerParameter( uint32_t sampler, GlSamplerParameter pname, const uint32_t * params )const;

		//@}
		/**@name Texture Buffer objects functions */
		//@{

		inline void TexBuffer( GlTexDim p_target, GlInternal p_internalFormat, uint32_t buffer )const;

		//@}
		/**@name Buffer objects functions */
		//@{

		inline void GenBuffers( int n, uint32_t * buffers )const;
		inline void DeleteBuffers( int n, uint32_t const * buffers )const;
		inline bool IsBuffer( uint32_t buffer )const;
		inline void BindBuffer( GlBufferTarget target, uint32_t buffer )const;
		inline void BufferData( GlBufferTarget target, ptrdiff_t size, void const * data, GlBufferMode usage )const;
		inline void BufferSubData( GlBufferTarget target, ptrdiff_t offset, ptrdiff_t size, void const * data )const;
		inline void CopyBufferSubData( GlBufferTarget readtarget, GlBufferTarget writetarget, ptrdiff_t readoffset, ptrdiff_t writeoffset, ptrdiff_t size )const;
		inline void * MapBuffer( GlBufferTarget target, GlAccessType access )const;
		inline void UnmapBuffer( GlBufferTarget target )const;
		inline void * MapBufferRange( GlBufferTarget target, ptrdiff_t offset, ptrdiff_t length, uint32_t access )const;
		inline void GetBufferParameter( GlBufferTarget target, GlBufferParameter pname, int * params )const;
		inline void GetBufferParameter( GlBufferTarget target, GlBufferParameter pname, uint64_t * params )const;
		inline void FlushMappedBufferRange( GlBufferTarget target, ptrdiff_t offset, ptrdiff_t length )const;
		inline void BufferAddressRange( GlAddress pname, uint32_t index, uint64_t address, size_t length )const;
		inline void VertexFormat( int size, GlType type, int stride )const;
		inline void NormalFormat( GlType type, int stride )const;
		inline void ColorFormat( int size, GlType type, int stride )const;
		inline void IndexFormat( GlType type, int stride )const;
		inline void TexCoordFormat( int size, GlType type, int stride )const;
		inline void EdgeFlagFormat( int stride )const;
		inline void SecondaryColorFormat( int size, GlType type, int stride )const;
		inline void FogCoordFormat( uint32_t type, int stride )const;
		inline void VertexAttribFormat( uint32_t index, int size, GlType type, bool normalized, int stride )const;
		inline void VertexAttribIFormat( uint32_t index, int size, GlType type, int stride )const;
		inline void MakeBufferResident( GlBufferTarget target, GlAccessType access )const;
		inline void MakeBufferNonResident( GlBufferTarget target )const;
		inline bool IsBufferResident( GlBufferTarget target )const;
		inline void MakeNamedBufferResident( uint32_t buffer, GlAccessType access )const;
		inline void MakeNamedBufferNonResident( uint32_t buffer )const;
		inline bool IsNamedBufferResident( uint32_t buffer )const;
		inline void GetNamedBufferParameter( uint32_t buffer, GlBufferParameter pname, uint64_t * params )const;

		//@}
		/**@name Transform Feedback functions */
		//@{

		/** see https://www.opengl.org/sdk/docs/man4/html/glGenTransformFeedbacks.xhtml
		*/
		inline void GenTransformFeedbacks( int n, uint32_t * buffers )const;

		/** see https://www.opengl.org/sdk/docs/man4/html/glDeleteTransformFeedbacks.xhtml
		*/
		inline void DeleteTransformFeedbacks( int n, uint32_t const * buffers )const;

		/** see https://www.opengl.org/sdk/docs/man4/html/glBindTransformFeedback.xhtml
		*/
		inline void BindTransformFeedback( GlBufferTarget target, uint32_t buffer )const;

		/** see https://www.opengl.org/sdk/docs/man4/html/glIsTransformFeedback.xhtml
		*/
		inline bool IsTransformFeedback( uint32_t buffer )const;

		/** see https://www.opengl.org/sdk/docs/man4/html/glBeginTransformFeedback.xhtml
		*/
		inline void BeginTransformFeedback( GlTopology primitive )const;

		/** see https://www.opengl.org/sdk/docs/man4/html/glPauseTransformFeedback.xhtml
		*/
		inline void PauseTransformFeedback()const;

		/** see https://www.opengl.org/sdk/docs/man4/html/glResumeTransformFeedback.xhtml
		*/
		inline void ResumeTransformFeedback()const;

		/** see https://www.opengl.org/sdk/docs/man4/html/glBeginTransformFeedback.xhtml
		*/
		inline void EndTransformFeedback()const;

		/** see https://www.opengl.org/sdk/docs/man/html/glTransformFeedbackVaryings.xhtml
		*/
		inline void TransformFeedbackVaryings( uint32_t program, int count, char const ** varyings, GlAttributeLayout bufferMode )const;

		/** see https://www.opengl.org/sdk/docs/man/html/glDrawTransformFeedback.xhtml
		*/
		inline void DrawTransformFeedback( GlTopology mode, uint32_t p_id )const;

		//@}
		/**@name FBO functions */
		//@{

		/** see https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glGenFramebuffers.xml
		*/
		inline void GenFramebuffers( int n, uint32_t * framebuffers )const;

		/** see https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glDeleteFramebuffers.xml
		*/
		inline void DeleteFramebuffers( int n, uint32_t const * framebuffers )const;

		/** see https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glIsFramebuffer.xml
		*/
		inline bool IsFramebuffer( uint32_t framebuffer )const;

		/** see https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glBindFramebuffer.xml
		*/
		inline void BindFramebuffer( GlFrameBufferMode p_eBindingMode, uint32_t framebuffer )const;

		/** see https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glFramebufferTexture.xml
		*/
		inline void FramebufferTexture( GlFrameBufferMode p_eBindingMode, GlAttachmentPoint p_eAttachment, uint32_t texture, int level )const;

		/** see https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glFramebufferTexture.xml
		*/
		inline void FramebufferTexture1D( GlFrameBufferMode p_eBindingMode, GlAttachmentPoint p_eAttachment, GlTexDim textarget, uint32_t texture, int level )const;

		/** see https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glFramebufferTexture.xml
		*/
		inline void FramebufferTexture2D( GlFrameBufferMode p_eBindingMode, GlAttachmentPoint p_eAttachment, GlTexDim textarget, uint32_t texture, int level )const;

		/** see https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glFramebufferTexture.xml
		*/
		inline void FramebufferTexture3D( GlFrameBufferMode p_eBindingMode, GlAttachmentPoint p_eAttachment, GlTexDim textarget, uint32_t texture, int level, int layer )const;

		/** see https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glFramebufferTextureLayer.xml
		*/
		inline void FramebufferTextureLayer( GlFrameBufferMode p_eBindingMode, GlAttachmentPoint p_eAttachment, uint32_t texture, int level, int layer )const;

		/** see https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glFramebufferRenderbuffer.xml
		*/
		inline void FramebufferRenderbuffer( GlFrameBufferMode p_eBindingMode, GlAttachmentPoint p_eAttachment, GlRenderBufferMode p_eRboTarget, uint32_t renderbufferId )const;

		/** see https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glCheckFramebufferStatus.xml
		*/
		inline GlFramebufferStatus CheckFramebufferStatus( GlFrameBufferMode p_eBindingMode )const;

		/** see https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glGenRenderbuffers.xml
		*/
		inline void GenRenderbuffers( int n, uint32_t * ids )const;

		/** see https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glDeleteRenderbuffers.xml
		*/
		inline void DeleteRenderbuffers( int n, uint32_t const * ids )const;

		/** see https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glIsFramebuffer.xml
		*/
		inline bool IsRenderbuffer( uint32_t renderbuffer )const;

		/** see https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glBindRenderbuffer.xml
		*/
		inline void BindRenderbuffer( GlRenderBufferMode p_eBindingMode, uint32_t id )const;
		inline void RenderbufferStorage( GlRenderBufferMode p_eBindingMode, GlInternal internalFormat, int width, int height )const;
		inline void RenderbufferStorageMultisample( GlRenderBufferMode p_eBindingMode, int p_iSamples, GlInternal internalFormat, int width, int height )const;
		inline void RenderbufferStorage( GlRenderBufferMode p_eBindingMode, GlInternal internalFormat, Castor::Size const & size )const;
		inline void RenderbufferStorageMultisample( GlRenderBufferMode p_eBindingMode, int p_iSamples, GlInternal internalFormat, Castor::Size const & size )const;
		inline void GetRenderbufferParameteriv( GlRenderBufferMode p_eBindingMode, GlRenderBufferParameter param, int * value )const;
		inline void BlitFramebuffer( int srcX0, int srcY0, int srcX1, int srcY1, int dstX0, int dstY0, int dstX1, int dstY1, uint32_t mask, GlInterpolationMode filter )const;
		inline void BlitFramebuffer( Castor::Rectangle const & rcSrc, Castor::Rectangle const & rcDst, uint32_t mask, GlInterpolationMode filter )const;
		inline void DrawBuffers( int n, const uint32_t * bufs )const;

		/** see https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glClearBuffer.xhtml
		*/
		inline void ClearBuffer( GlComponent buffer, uint32_t id, float const * values )const;

		/** see https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glClearBuffer.xhtml
		*/
		inline void ClearBuffer( GlComponent buffer, uint32_t id, int const * values )const;

		/** see https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glClearBuffer.xhtml
		*/
		inline void ClearBuffer( GlComponent buffer, uint32_t id, float depth, int stencil )const;

		//@}
		/**@name Uniform variable Functions */
		//@{

		inline int GetUniformLocation( uint32_t program, char const * name )const;
		inline void SetUniform( int location, int v0 )const;
		inline void SetUniform( int location, uint32_t v0 )const;
		inline void SetUniform( int location, float v0 )const;
		inline void SetUniform( int location, double v0 )const;
		inline void SetUniform( int location, int v0, int v1 )const;
		inline void SetUniform( int location, int v0, int v1, int v2 )const;
		inline void SetUniform( int location, int v0, int v1, int v2, int v3 )const;
		inline void SetUniform( int location, uint32_t v0, uint32_t v1 )const;
		inline void SetUniform( int location, uint32_t v0, uint32_t v1, uint32_t v2 )const;
		inline void SetUniform( int location, uint32_t v0, uint32_t v1, uint32_t v2, uint32_t v3 )const;
		inline void SetUniform( int location, float v0, float v1 )const;
		inline void SetUniform( int location, float v0, float v1, float v2 )const;
		inline void SetUniform( int location, float v0, float v1, float v2, float v3 )const;
		inline void SetUniform( int location, double v0, double v1 )const;
		inline void SetUniform( int location, double v0, double v1, double v2 )const;
		inline void SetUniform( int location, double v0, double v1, double v2, double v3 )const;
		inline void SetUniform1v( int location, int count, int const * params )const;
		inline void SetUniform2v( int location, int count, int const * params )const;
		inline void SetUniform3v( int location, int count, int const * params )const;
		inline void SetUniform4v( int location, int count, int const * params )const;
		inline void SetUniform1v( int location, int count, uint32_t const * params )const;
		inline void SetUniform2v( int location, int count, uint32_t const * params )const;
		inline void SetUniform3v( int location, int count, uint32_t const * params )const;
		inline void SetUniform4v( int location, int count, uint32_t const * params )const;
		inline void SetUniform1v( int location, int count, float const * params )const;
		inline void SetUniform2v( int location, int count, float const * params )const;
		inline void SetUniform3v( int location, int count, float const * params )const;
		inline void SetUniform4v( int location, int count, float const * params )const;
		inline void SetUniform1v( int location, int count, double const * params )const;
		inline void SetUniform2v( int location, int count, double const * params )const;
		inline void SetUniform3v( int location, int count, double const * params )const;
		inline void SetUniform4v( int location, int count, double const * params )const;
		inline void SetUniform2x2v( int location, int count, bool transpose, float const * value )const;
		inline void SetUniform2x3v( int location, int count, bool transpose, float const * value )const;
		inline void SetUniform2x4v( int location, int count, bool transpose, float const * value )const;
		inline void SetUniform3x3v( int location, int count, bool transpose, float const * value )const;
		inline void SetUniform3x2v( int location, int count, bool transpose, float const * value )const;
		inline void SetUniform3x4v( int location, int count, bool transpose, float const * value )const;
		inline void SetUniform4x4v( int location, int count, bool transpose, float const * value )const;
		inline void SetUniform4x2v( int location, int count, bool transpose, float const * value )const;
		inline void SetUniform4x3v( int location, int count, bool transpose, float const * value )const;
		inline void SetUniform2x2v( int location, int count, bool transpose, double const * value )const;
		inline void SetUniform2x3v( int location, int count, bool transpose, double const * value )const;
		inline void SetUniform2x4v( int location, int count, bool transpose, double const * value )const;
		inline void SetUniform3x3v( int location, int count, bool transpose, double const * value )const;
		inline void SetUniform3x2v( int location, int count, bool transpose, double const * value )const;
		inline void SetUniform3x4v( int location, int count, bool transpose, double const * value )const;
		inline void SetUniform4x4v( int location, int count, bool transpose, double const * value )const;
		inline void SetUniform4x2v( int location, int count, bool transpose, double const * value )const;
		inline void SetUniform4x3v( int location, int count, bool transpose, double const * value )const;

		//@}
		/**@name Uniform Buffer Objects Functions */
		//@{

		inline uint32_t GetUniformBlockIndex( uint32_t shader, char const * uniformBlockName )const;
		inline void BindBufferBase( GlBufferTarget target, uint32_t index, uint32_t buffer )const;
		inline void UniformBlockBinding( uint32_t shader, uint32_t uniformBlockIndex, uint32_t uniformBlockBinding )const;
		inline void GetUniformIndices( uint32_t shader, int uniformCount, char const ** uniformNames, uint32_t * uniformIndices )const;
		inline void GetActiveUniformsiv( uint32_t shader, int uniformCount, uint32_t const * uniformIndices, GlUniformValue pname, int * params )const;
		inline void GetActiveUniformBlockiv( uint32_t shader, uint32_t uniformBlockIndex, GlUniformBlockValue pname, int * params )const;

		//@}
		/**@name Shader object Functions */
		//@{

		inline uint32_t CreateShader( GlShaderType type )const;
		inline void DeleteShader( uint32_t program )const;
		inline bool IsShader( uint32_t program )const;
		inline void AttachShader( uint32_t program, uint32_t shader )const;
		inline void DetachShader( uint32_t program, uint32_t shader )const;
		inline void CompileShader( uint32_t program )const;
		inline void GetShaderiv( uint32_t program, GlShaderStatus pname, int * param )const;
		inline void GetShaderInfoLog( uint32_t program, int bufSize, int * length, char * infoLog )const;
		inline void ShaderSource( uint32_t program, int count, char const ** strings, int const * lengths )const;
		inline void GetActiveAttrib( uint32_t program, uint32_t index, int bufSize, int * length, int * size, uint32_t * type, char * name )const;
		inline void GetActiveUniform( uint32_t program, uint32_t index, int bufSize, int * length, int * size, uint32_t * type, char * name )const;

		//@}
		/**@name Shader program Functions */
		//@{

		inline uint32_t CreateProgram()const;
		inline void DeleteProgram( uint32_t program )const;
		inline void LinkProgram( uint32_t program )const;
		inline void UseProgram( uint32_t program )const;
		inline void GetProgramiv( uint32_t program, GlShaderStatus pname, int * param )const;
		inline void GetProgramInfoLog( uint32_t program, int bufSize, int * length, char * infoLog )const;
		inline int GetAttribLocation( uint32_t program, char const * name )const;
		inline bool IsProgram( uint32_t program )const;
		inline void ProgramParameteri( uint32_t program, uint32_t pname, int value )const;

		/** see https://www.opengl.org/sdk/docs/man/html/glDispatchCompute.xhtml
		*/
		inline void DispatchCompute( uint32_t num_groups_x, uint32_t num_groups_y, uint32_t num_groups_z )const;

		/** see https://www.opengl.org/sdk/docs/man/html/glDispatchComputeGroupSize.xhtml
		*/
		inline void DispatchComputeGroupSize( uint32_t num_groups_x, uint32_t num_groups_y, uint32_t num_groups_z, uint32_t work_group_size_x, uint32_t work_group_size_y, uint32_t work_group_size_z )const;

		/** see https://www.opengl.org/sdk/docs/man/html/glShaderStorageBlockBinding.xhtml
		*/
		inline void ShaderStorageBlockBinding( uint32_t shader, uint32_t storageBlockIndex, uint32_t storageBlockBinding )const;

		//@}
		/**@name Vertex Attribute Pointer functions */
		//@{

		inline void EnableVertexAttribArray( uint32_t index )const;
		inline void VertexAttribPointer( uint32_t index, int size, GlType type, bool normalized, int stride, void const * pointer )const;
		inline void VertexAttribPointer( uint32_t index, int size, GlType type, int stride, void const * pointer )const;
		inline void DisableVertexAttribArray( uint32_t index )const;

		//@}
		/**@name Vertex Array Objects */
		//@{

		inline void GenVertexArrays( int n, uint32_t * arrays )const;
		inline bool IsVertexArray( uint32_t array )const;
		inline void BindVertexArray( uint32_t array )const;
		inline void DeleteVertexArrays( int n, uint32_t const * arrays )const;

		//@}
		/**@name Tesselation functions */
		//@{

		inline void PatchParameter( GlPatchParameter p_param, int p_iValue )const;

		//@}
		/**@name Query functions */
		//@{

		inline void GenQueries( int p_n, uint32_t * p_queries )const;
		inline void DeleteQueries( int p_n, uint32_t const * p_queries )const;
		inline bool IsQuery( uint32_t p_query )const;
		inline void BeginQuery( GlQueryType p_target, uint32_t p_query )const;
		inline void EndQuery( GlQueryType p_target )const;
		inline void QueryCounter( uint32_t p_id, GlQueryType p_target )const;
		inline void GetQueryObjectInfos( uint32_t p_id, GlQueryInfo p_name, int32_t * p_params )const;
		inline void GetQueryObjectInfos( uint32_t p_id, GlQueryInfo p_name, uint32_t * p_params )const;
		inline void GetQueryObjectInfos( uint32_t p_id, GlQueryInfo p_name, int64_t * p_params )const;
		inline void GetQueryObjectInfos( uint32_t p_id, GlQueryInfo p_name, uint64_t * p_params )const;

		//@}
		/**@name GL_ARB_program_interface_query */
		//@{

		inline void GetProgramInterfaceInfos( uint32_t program, GlslInterface programInterface, GlslDataName name, int * params );
		inline int GetProgramResourceIndex( uint32_t program, GlslInterface programInterface, char const * const name );
		inline int GetProgramResourceLocation( uint32_t program, GlslInterface programInterface, char const * const name );
		inline int GetProgramResourceLocationIndex( uint32_t program, GlslInterface programInterface, char const * const name );
		inline void GetProgramResourceName( uint32_t program, GlslInterface programInterface, uint32_t index, int bufSize, int * length, char * name );
		inline void GetProgramResourceInfos( uint32_t program, GlslInterface programInterface, uint32_t index, int propCount, uint32_t * props, int bufSize, int * length, int * params );

		//@}
		/**@name Memory transactions functions */
		//@{

		/** see https://www.opengl.org/sdk/docs/man/html/glMemoryBarrier.xhtml
		*/
		inline void MemoryBarrier( Castor::FlagCombination< GlBarrierBit > const & barriers )const;

		/** see https://www.opengl.org/sdk/docs/man/html/glMemoryBarrier.xhtml
		*/
		inline void MemoryBarrierByRegion( Castor::FlagCombination< GlBarrierBit > const & barriers )const;

		//@}
		/**@name Other functions */
		//@{

		inline GlAccessType GetLockFlags( Castor3D::AccessTypes const & p_flags )const;
		inline Castor::FlagCombination< GlBufferMappingBit > GetBitfieldFlags( Castor3D::AccessTypes const & p_flags )const;
		inline Castor3D::ElementType Get( GlslAttributeType p_type )const;

#if C3D_TRACE_OBJECTS

		template< typename T >
		inline bool Track( T * p_object, std::string const & p_name, std::string const & p_file, int p_line )const
		{
			return m_debug.Track( p_object, p_name, p_file, p_line );
		}

		template< typename T >
		inline bool UnTrack( T * p_object )const
		{
			return m_debug.UnTrack( p_object );
		}

#endif
#if C3DGL_CHECK_TEXTURE_UNIT

		void TrackTexture( uint32_t p_name, uint32_t p_index )const
		{
			m_debug.BindTexture( p_name, p_index );
		}

		void TrackSampler( uint32_t p_name, uint32_t p_index )const
		{
			m_debug.BindSampler( p_name, p_index );
		}

		void CheckTextureUnits()const
		{
			m_debug.CheckTextureUnits();
		}

#endif

		//@}

	private:
		std::array< Castor::String, 8u > GlslStrings;
		std::array< Castor::String, 8u > GlslErrors;
		std::array< GlTopology, size_t( Castor3D::Topology::eCount ) > PrimitiveTypes;
		std::array< GlTexDim, size_t( Castor3D::TextureType::eCount ) > TextureDimensions;
		std::array< GlComparator, size_t( Castor3D::ComparisonFunc::eCount ) > AlphaFuncs;
		std::array< GlWrapMode, size_t( Castor3D::WrapMode::eCount ) > TextureWrapMode;
		std::array< GlInterpolationMode, size_t( Castor3D::InterpolationMode::eCount ) > TextureInterpolation;
		std::array< GlBlendFactor, size_t( Castor3D::BlendOperand::eCount ) > BlendFactors;
		std::array< GlBlendSource, size_t( Castor3D::BlendSource::eCount ) > TextureArguments;
		std::array< GlBlendFunc, size_t( Castor3D::TextureBlendFunc::eCount ) > TextureBlendFuncs;
		std::array< GlBlendOp, size_t( Castor3D::BlendOperation::eCount ) > BlendOps;
		std::array< PixelFmt, size_t( Castor::PixelFormat::eCount ) > PixelFormats;
		std::array< GlShaderType, size_t( Castor3D::ShaderType::eCount ) > ShaderTypes;
		std::array< GlInternal, size_t( Castor::PixelFormat::eCount ) > Internals;
		std::array< GlAttachmentPoint, size_t( Castor3D::AttachmentPoint::eCount ) > Attachments;
		std::array< GlFrameBufferMode, size_t( Castor3D::FrameBufferMode::eCount ) > FramebufferModes;
		std::array< GlInternal, size_t( Castor::PixelFormat::eCount ) > RboStorages;
		std::array< GlBufferBinding, size_t( Castor3D::WindowBuffer::eCount ) > Buffers;
		std::array< GlFace, size_t( Castor3D::Culling::eCount ) > Faces;
		std::array< GlFillMode, 3u > FillModes;
		std::array< GlStencilOp, size_t( Castor3D::StencilOp::eCount ) > StencilOps;
		std::array< GlComparator, size_t( Castor3D::StencilFunc::eCount ) > StencilFuncs;
		std::array< GlQueryType, size_t( Castor3D::QueryType::eCount ) > Queries;
		std::array< GlQueryInfo, size_t( Castor3D::QueryInfo::eCount ) > QueryInfos;
		std::array< GlTextureStorageType, size_t( Castor3D::TextureStorageType::eCount ) > TextureStorages;
		std::array< GlTexDim, size_t( Castor3D::CubeMapFace::eCount ) > CubeMapFaces;
		std::array< GlCompareMode, size_t( Castor3D::ComparisonMode::eCount ) > ComparisonModes;
		std::array< bool, size_t( Castor3D::WritingMask::eCount ) > WriteMasks;
		std::array< GlComparator, size_t( Castor3D::DepthFunc::eCount ) > DepthFuncs;
		std::map< GlAttachmentPoint, GlBufferBinding > BuffersTA;
		std::map< Castor3D::AttachmentPoint, GlComponent > Components;

		bool m_bHasVao{ false };
		bool m_bHasUbo{ false };
		bool m_bHasPbo{ false };
		bool m_bHasTbo{ false };
		bool m_bHasFbo{ false };
		bool m_bHasVbo{ false };
		bool m_bHasSsbo{ false };
		bool m_bHasVSh{ false };
		bool m_bHasPSh{ false };
		bool m_bHasGSh{ false };
		bool m_bHasTSh{ false };
		bool m_bHasCSh{ false };
		bool m_bHasSpl{ false };
		bool m_bHasComputeVariableGroupSize{ false };
		bool m_bHasAnisotropic{ false };
		bool m_bBindVboToGpuAddress{ false };
		Castor::String m_extensions;
		Castor::String m_vendor;
		Castor::String m_renderer;
		Castor::String m_version;
		int m_iVersion{ 0 };
		int m_iGlslVersion{ 0 };
		bool m_bHasInstancedDraw{ false };
		bool m_bHasInstancedArrays{ false };
		bool m_bHasDirectStateAccess{ false };
		bool m_bHasNonPowerOfTwoTextures{ false };
		TexFunctionsBase * m_pTexFunctions{ nullptr };
		BufFunctionsBase * m_pBufFunctions{ nullptr };
		GlRenderSystem & m_renderSystem;
		GlProvider m_gpu{ GlProvider::eUnknown };

		GlDebug m_debug;

		/**@name General */
		//@{

		GlFunction< void, float, float , float , float > m_pfnClearColor;
		GlFunction< void, double > m_pfnClearDepth;
		GlFunction< void, uint32_t > m_pfnClear;
		GlFunction< void, uint32_t > m_pfnEnable;
		GlFunction< void, uint32_t > m_pfnDisable;
		GlFunction< void, GlTweak > m_pfnEnableTweak;
		GlFunction< void, GlTweak > m_pfnDisableTweak;
		GlFunction< void, GlTexDim > m_pfnEnableTexDim;
		GlFunction< void, GlTexDim > m_pfnDisableTexDim;
		GlFunction< void, int, uint32_t *> m_pfnSelectBuffer;
		GlFunction< void, uint32_t, int *> m_pfnGetIntegerv;

		//@}
		/**@name Depth stencil state */
		//@{

		GlFunction< void, uint32_t > m_pfnDepthFunc;
		GlFunction< void, uint8_t > m_pfnDepthMask;
		GlFunction< void, uint8_t, uint8_t , uint8_t , uint8_t > m_pfnColorMask;
		GlFunction< void, uint32_t, uint32_t , uint32_t > m_pfnStencilOp;
		GlFunction< void, uint32_t, int , uint32_t > m_pfnStencilFunc;
		GlFunction< void, uint32_t > m_pfnStencilMask;
		GlFunction< void, uint32_t, uint32_t , uint32_t , uint32_t > m_pfnStencilOpSeparate;
		GlFunction< void, uint32_t, uint32_t , int , uint32_t > m_pfnStencilFuncSeparate;
		GlFunction< void, uint32_t, uint32_t > m_pfnStencilMaskSeparate;

		//@}
		/**@name Rasterizer state */
		//@{

		GlFunction< void, uint32_t, uint32_t > m_pfnPolygonMode;
		GlFunction< void, uint32_t > m_pfnCullFace;
		GlFunction< void, uint32_t > m_pfnFrontFace;
		GlFunction< void, uint32_t, uint32_t > m_pfnHint;
		GlFunction< void, float, float > m_pfnPolygonOffset;

		//@}
		/**@name Blend state */
		//@{

		GlFunction< void, float, float , float , float > m_pfnBlendColor;
		GlFunction< void, uint32_t, uint32_t , uint32_t , uint32_t > m_pfnBlendFuncSeparate;
		GlFunction< void, uint32_t, uint32_t , uint32_t , uint32_t , uint32_t > m_pfnBlendFuncSeparatei;
		GlFunction< void, uint32_t > m_pfnBlendEquation;
		GlFunction< void, uint32_t, uint32_t > m_pfnBlendEquationi;
		GlFunction< void, float, uint8_t > m_pfnSampleCoverage;

		//@}
		/**@name Buffer rendering */
		//@{

		GlFunction< void, GlTopology, int , int > m_pfnDrawArrays;
		GlFunction< void, GlTopology, int , GlType, void const * > m_pfnDrawElements;
		GlFunction< void, GlTopology, int , int , int > m_pfnDrawArraysInstanced;
		GlFunction< void, GlTopology, int , GlType, void const * , int > m_pfnDrawElementsInstanced;
		GlFunction< void, uint32_t, uint32_t > m_pfnVertexAttribDivisor;

		//@}
		/**@name Context */
		//@{

#if defined( CASTOR_PLATFORM_WINDOWS )
		GlFunction< BOOL, HDC, HGLRC > m_pfnMakeCurrent;
		GlFunction< BOOL, HDC > m_pfnSwapBuffers;
		GlFunction< HGLRC, HDC > m_pfnCreateContext;
		GlFunction< BOOL, HGLRC > m_pfnDeleteContext;
		GlFunction< HGLRC, HDC, HGLRC , int const *> m_pfnCreateContextAttribs;
		GlFunction< BOOL, int > m_pfnSwapInterval;
#elif defined ( CASTOR_PLATFORM_LINUX )
		GlFunction< int, Display *, GLXDrawable , GLXContext > m_pfnMakeCurrent;
		GlFunction< void, Display *, GLXDrawable > m_pfnSwapBuffers;
		GlFunction< GLXContext, Display *, XVisualInfo *, GLXContext , Bool > m_pfnCreateContext;
		GlFunction< void, Display *, GLXContext > m_pfnDeleteContext;
		GlFunction< GLXContext, Display *, GLXFBConfig , GLXContext , Bool , int const * > m_pfnCreateContextAttribs;
		GlFunction< void, Display *, GLXDrawable , int > m_pfnSwapInterval;
#else
#	error "Yet unsupported OS"
#endif

		//@}
		/**@name Matrix */
		//@{

		GlFunction< void, int, int , int , int > m_pfnViewport;

		//@}
		/**@name Material */
		//@{

		GlFunction< void, uint32_t, uint32_t > m_pfnBlendFunc;
		GlFunction< void, uint32_t, float > m_pfnAlphaFunc;
		GlFunction< void, uint32_t, uint32_t , float > m_pfnMaterialf;
		GlFunction< void, uint32_t, uint32_t , float const *> m_pfnMaterialfv;

		//@}
		/**@name Texture */
		//@{

		GlFunction< void, int, uint32_t *> m_pfnGenTextures;
		GlFunction< void, int, uint32_t const * > m_pfnDeleteTextures;
		GlFunction< uint8_t, uint32_t > m_pfnIsTexture;
		GlFunction< void, uint32_t > m_pfnActiveTexture;
		GlFunction< void, uint32_t > m_pfnClientActiveTexture;
		GlFunction< void, uint32_t > m_pfnReadBuffer;
		GlFunction< void, int, int , int , int , uint32_t , uint32_t , void * > m_pfnReadPixels;
		GlFunction< void, uint32_t > m_pfnDrawBuffer;
		GlFunction< void, int, int , uint32_t , uint32_t , void const * > m_pfnDrawPixels;
		GlFunction< void, uint32_t, int > m_pfnPixelStorei;
		GlFunction< void, uint32_t, float > m_pfnPixelStoref;
		GlFunction< void, GLenum, GLint , GLint , GLsizei > m_pfnTexStorage1D;
		GlFunction< void, GLenum, GLint , GLint , GLsizei , GLsizei > m_pfnTexStorage2D;
		GlFunction< void, GLenum, GLint , GLint , GLsizei , GLsizei , GLsizei > m_pfnTexStorage3D;
		GlFunction< void, GLenum, GLsizei , GLint , GLsizei , GLsizei , GLboolean > m_pfnTexStorage2DMultisample;
		GlFunction< void, GLenum, GLsizei , GLint , GLsizei , GLsizei , GLsizei , GLboolean > m_pfnTexStorage3DMultisample;

		//@}
		/**@name Sampler */
		//@{

		GlFunction< void, int, uint32_t const * > m_pfnDeleteSamplers;
		GlFunction< void, int, uint32_t *> m_pfnGenSamplers;
		GlFunction< uint8_t, uint32_t > m_pfnIsSampler;
		GlFunction< void, uint32_t, uint32_t > m_pfnBindSampler;
		GlFunction< void, uint32_t, uint32_t , uint32_t *> m_pfnGetSamplerParameteruiv;
		GlFunction< void, uint32_t, uint32_t , float *> m_pfnGetSamplerParameterfv;
		GlFunction< void, uint32_t, uint32_t , int *> m_pfnGetSamplerParameteriv;
		GlFunction< void, uint32_t, uint32_t , uint32_t const *> m_pfnSamplerParameteruiv;
		GlFunction< void, uint32_t, uint32_t , float > m_pfnSamplerParameterf;
		GlFunction< void, uint32_t, uint32_t , float const *> m_pfnSamplerParameterfv;
		GlFunction< void, uint32_t, uint32_t , int > m_pfnSamplerParameteri;
		GlFunction< void, uint32_t, uint32_t , int const *> m_pfnSamplerParameteriv;

		//@}
		/**@name Texture Buffer Objects */
		//@{

		GlFunction< void, uint32_t, uint32_t , uint32_t > m_pfnTexBuffer;

		//@}
		/**@name Buffer Objects */
		//@{

		GlFunction< void, int, uint32_t *> m_pfnGenBuffers;
		GlFunction< void, int, uint32_t const * > m_pfnDeleteBuffers;
		GlFunction< uint8_t, uint32_t > m_pfnIsBuffer;

		//@}
		/**@name Transform Feedback */
		//@{

		GlFunction< void, int, uint32_t *> m_pfnGenTransformFeedbacks;
		GlFunction< void, int, uint32_t const * > m_pfnDeleteTransformFeedbacks;
		GlFunction< void, GlBufferTarget, uint32_t > m_pfnBindTransformFeedback;
		GlFunction< uint8_t, uint32_t > m_pfnIsTransformFeedback;
		GlFunction< void, uint32_t > m_pfnBeginTransformFeedback;
		GlFunction< void > m_pfnPauseTransformFeedback;
		GlFunction< void > m_pfnResumeTransformFeedback;
		GlFunction< void > m_pfnEndTransformFeedback;
		GlFunction< void, uint32_t, int , char const **, GlAttributeLayout > m_pfnTransformFeedbackVaryings;
		GlFunction< void, uint32_t, uint32_t > m_pfnDrawTransformFeedback;

		//@}
		/**@name FBO */
		//@{

		GlFunction< void, int, uint32_t *> m_pfnGenFramebuffers;
		GlFunction< void, int, uint32_t const * > m_pfnDeleteFramebuffers;
		GlFunction< uint8_t, uint32_t > m_pfnIsFramebuffer;
		GlFunction< void, uint32_t, uint32_t > m_pfnBindFramebuffer;
		GlFunction< uint32_t, uint32_t > m_pfnCheckFramebufferStatus;
		GlFunction< void, uint32_t, uint32_t , uint32_t , int > m_pfnFramebufferTexture;
		GlFunction< void, uint32_t, uint32_t , uint32_t , uint32_t , int > m_pfnFramebufferTexture1D;
		GlFunction< void, uint32_t, uint32_t , uint32_t , uint32_t , int > m_pfnFramebufferTexture2D;
		GlFunction< void, uint32_t, uint32_t , uint32_t , int , int > m_pfnFramebufferTextureLayer;
		GlFunction< void, uint32_t, uint32_t , uint32_t , uint32_t , int , int > m_pfnFramebufferTexture3D;
		GlFunction< void, int, int , int , int , int , int , int , int , uint32_t , uint32_t > m_pfnBlitFramebuffer;
		GlFunction< void, int, uint32_t const * > m_pfnDrawBuffers;
		GlFunction< void, uint32_t, uint32_t, float const * > m_pfnClearBufferfv;
		GlFunction< void, uint32_t, uint32_t, int const * > m_pfnClearBufferiv;
		GlFunction< void, uint32_t, uint32_t, float, int > m_pfnClearBufferfi;

		//@}
		/**@name RBO */
		//@{

		GlFunction< void, uint32_t, uint32_t , uint32_t , uint32_t > m_pfnFramebufferRenderbuffer;
		GlFunction< void, int, uint32_t *> m_pfnGenRenderbuffers;
		GlFunction< void, int, uint32_t const * > m_pfnDeleteRenderbuffers;
		GlFunction< uint8_t, uint32_t > m_pfnIsRenderbuffer;
		GlFunction< void, uint32_t, uint32_t > m_pfnBindRenderbuffer;
		GlFunction< void, uint32_t, uint32_t , int , int > m_pfnRenderbufferStorage;
		GlFunction< void, uint32_t, int , uint32_t , int , int > m_pfnRenderbufferStorageMultisample;
		GlFunction< void, uint32_t, int , int , int , int , uint8_t > m_pfnTexImage2DMultisample;
		GlFunction< void, uint32_t, uint32_t , int *> m_pfnGetRenderbufferParameteriv;

		//@}
		/**@name Uniform variables */
		//@{

		GlFunction< int, uint32_t, char const * > m_pfnGetUniformLocation;
		GlFunction< void, uint32_t, uint32_t , int , int * , int * , uint32_t * , char * > m_pfnGetActiveUniform;
		GlFunction< void, int, int > m_pfnUniform1i;
		GlFunction< void, int, int , int > m_pfnUniform2i;
		GlFunction< void, int, int , int , int > m_pfnUniform3i;
		GlFunction< void, int, int , int , int , int > m_pfnUniform4i;
		GlFunction< void, int, int , int const *> m_pfnUniform1iv;
		GlFunction< void, int, int , int const *> m_pfnUniform2iv;
		GlFunction< void, int, int , int const *> m_pfnUniform3iv;
		GlFunction< void, int, int , int const *> m_pfnUniform4iv;
		GlFunction< void, int, uint32_t > m_pfnUniform1ui;
		GlFunction< void, int, uint32_t, uint32_t > m_pfnUniform2ui;
		GlFunction< void, int, uint32_t, uint32_t, uint32_t > m_pfnUniform3ui;
		GlFunction< void, int, uint32_t, uint32_t, uint32_t, uint32_t > m_pfnUniform4ui;
		GlFunction< void, int, int, uint32_t const *> m_pfnUniform1uiv;
		GlFunction< void, int, int, uint32_t const *> m_pfnUniform2uiv;
		GlFunction< void, int, int, uint32_t const *> m_pfnUniform3uiv;
		GlFunction< void, int, int, uint32_t const *> m_pfnUniform4uiv;
		GlFunction< void, int, float > m_pfnUniform1f;
		GlFunction< void, int, float , float > m_pfnUniform2f;
		GlFunction< void, int, float , float , float > m_pfnUniform3f;
		GlFunction< void, int, float , float , float , float > m_pfnUniform4f;
		GlFunction< void, int, int , float const *> m_pfnUniform1fv;
		GlFunction< void, int, int , float const *> m_pfnUniform2fv;
		GlFunction< void, int, int , float const *> m_pfnUniform3fv;
		GlFunction< void, int, int , float const *> m_pfnUniform4fv;
		GlFunction< void, int, double > m_pfnUniform1d;
		GlFunction< void, int, double , double > m_pfnUniform2d;
		GlFunction< void, int, double , double , double > m_pfnUniform3d;
		GlFunction< void, int, double , double , double , double > m_pfnUniform4d;
		GlFunction< void, int, int , double const *> m_pfnUniform1dv;
		GlFunction< void, int, int , double const *> m_pfnUniform2dv;
		GlFunction< void, int, int , double const *> m_pfnUniform3dv;
		GlFunction< void, int, int , double const *> m_pfnUniform4dv;
		GlFunction< void, int, int , uint8_t , float const * > m_pfnUniformMatrix2fv;
		GlFunction< void, int, int , uint8_t , float const * > m_pfnUniformMatrix2x3fv;
		GlFunction< void, int, int , uint8_t , float const * > m_pfnUniformMatrix2x4fv;
		GlFunction< void, int, int , uint8_t , float const * > m_pfnUniformMatrix3fv;
		GlFunction< void, int, int , uint8_t , float const * > m_pfnUniformMatrix3x2fv;
		GlFunction< void, int, int , uint8_t , float const * > m_pfnUniformMatrix3x4fv;
		GlFunction< void, int, int , uint8_t , float const * > m_pfnUniformMatrix4fv;
		GlFunction< void, int, int , uint8_t , float const * > m_pfnUniformMatrix4x2fv;
		GlFunction< void, int, int , uint8_t , float const * > m_pfnUniformMatrix4x3fv;
		GlFunction< void, int, int , uint8_t , double const * > m_pfnUniformMatrix2dv;
		GlFunction< void, int, int , uint8_t , double const * > m_pfnUniformMatrix2x3dv;
		GlFunction< void, int, int , uint8_t , double const * > m_pfnUniformMatrix2x4dv;
		GlFunction< void, int, int , uint8_t , double const * > m_pfnUniformMatrix3dv;
		GlFunction< void, int, int , uint8_t , double const * > m_pfnUniformMatrix3x2dv;
		GlFunction< void, int, int , uint8_t , double const * > m_pfnUniformMatrix3x4dv;
		GlFunction< void, int, int , uint8_t , double const * > m_pfnUniformMatrix4dv;
		GlFunction< void, int, int , uint8_t , double const * > m_pfnUniformMatrix4x2dv;
		GlFunction< void, int, int , uint8_t , double const * > m_pfnUniformMatrix4x3dv;

		//@}
		/**@name Uniform buffer object */
		//@{

		GlFunction< uint32_t, uint32_t, char const * > m_pfnGetUniformBlockIndex;
		GlFunction< void, uint32_t, uint32_t , uint32_t > m_pfnBindBufferBase;
		GlFunction< void, uint32_t, uint32_t , uint32_t > m_pfnUniformBlockBinding;
		GlFunction< void, uint32_t, int , char const **, uint32_t * > m_pfnGetUniformIndices;
		GlFunction< void, uint32_t, int , uint32_t const *, uint32_t , int * > m_pfnGetActiveUniformsiv;
		GlFunction< void, uint32_t, uint32_t , uint32_t , int * > m_pfnGetActiveUniformBlockiv;

		//@}
		/**@name Shader object */
		//@{

		GlFunction< uint32_t, uint32_t > m_pfnCreateShader;
		GlFunction< void, uint32_t > m_pfnDeleteShader;
		GlFunction< uint8_t, uint32_t > m_pfnIsShader;
		GlFunction< void, uint32_t, uint32_t > m_pfnAttachShader;
		GlFunction< void, uint32_t, uint32_t > m_pfnDetachShader;
		GlFunction< void, uint32_t > m_pfnCompileShader;
		GlFunction< void, uint32_t, uint32_t , int *> m_pfnGetShaderiv;
		GlFunction< void, uint32_t, int , int *, char * > m_pfnGetShaderInfoLog;
		GlFunction< void, uint32_t, int , char const **, int const * > m_pfnShaderSource;

		//@}
		/**@name Shader program */
		//@{

		GlFunction< uint32_t > m_pfnCreateProgram;
		GlFunction< void, uint32_t > m_pfnDeleteProgram;
		GlFunction< uint8_t, uint32_t > m_pfnIsProgram;
		GlFunction< void, uint32_t > m_pfnLinkProgram;
		GlFunction< void, uint32_t > m_pfnValidateProgram;
		GlFunction< void, uint32_t > m_pfnUseProgram;
		GlFunction< void, uint32_t, uint32_t , int *> m_pfnGetProgramiv;
		GlFunction< void, uint32_t, int , int *, char * > m_pfnGetProgramInfoLog;
		GlFunction< int, uint32_t, char const * > m_pfnGetAttribLocation;
		GlFunction< void, uint32_t, uint32_t , int > m_pfnProgramParameteri;
		GlFunction< void, uint32_t, uint32_t , int , int * , int * , uint32_t * , char * > m_pfnGetActiveAttrib;
		GlFunction< void, uint32_t, uint32_t , uint32_t > m_pfnDispatchCompute;
		GlFunction< void, uint32_t, uint32_t , uint32_t , uint32_t , uint32_t , uint32_t > m_pfnDispatchComputeGroupSize;
		GlFunction< void, uint32_t, uint32_t , uint32_t > m_pfnShaderStorageBlockBinding;

		//@}
		/**@name Vertex Attribute Pointer */
		//@{

		GlFunction< void, uint32_t > m_pfnEnableVertexAttribArray;
		GlFunction< void, uint32_t, int , uint32_t , uint8_t , int , void const * > m_pfnVertexAttribPointer;
		GlFunction< void, uint32_t, int , uint32_t , int , void const * > m_pfnVertexAttribIPointer;
		GlFunction< void, uint32_t > m_pfnDisableVertexAttribArray;

		//@}
		/**@name Vertex Array Object */
		//@{

		GlFunction< void, int, uint32_t *> m_pfnGenVertexArrays;
		GlFunction< void, int, uint32_t const * > m_pfnDeleteVertexArrays;
		GlFunction< uint8_t, uint32_t > m_pfnIsVertexArray;
		GlFunction< void, uint32_t > m_pfnBindVertexArray;

		//@}
		/*@name NV_vertex_buffer_unified_memory extension */
		//@{

		GlFunction< void, uint32_t, uint64_t *> m_pfnGetIntegerui64v;

		//@}
		/*@name Queries */
		//@{

		/** see https://www.opengl.org/sdk/docs/man/html/glGenQueries.xhtml
		*/
		GlFunction< void, int, uint32_t *> m_pfnGenQueries;

		/** see https://www.opengl.org/sdk/docs/man/html/glDeleteQueries.xhtml
		*/
		GlFunction< void, int, uint32_t const * > m_pfnDeleteQueries;

		/** see https://www.opengl.org/sdk/docs/man/html/glIsQuery.xhtml
		*/
		GlFunction< uint8_t, uint32_t > m_pfnIsQuery;

		/** see https://www.opengl.org/sdk/docs/man/html/glBeginQuery.xhtml
		*/
		GlFunction< void, uint32_t, uint32_t > m_pfnBeginQuery;

		/** see https://www.opengl.org/sdk/docs/man/html/glEndQuery.xhtml
		*/
		GlFunction< void, uint32_t > m_pfnEndQuery;

		/** see https://www.opengl.org/sdk/docs/man/html/glQueryCounter.xhtml
		*/
		GlFunction< void, uint32_t, uint32_t > m_pfnQueryCounter;

		/** see https://www.opengl.org/sdk/docs/man/html/glGetQueryObject.xhtml
		*/
		GlFunction< void, uint32_t, uint32_t , int32_t *> m_pfnGetQueryObjectiv;

		/** see https://www.opengl.org/sdk/docs/man/html/glGetQueryObject.xhtml
		*/
		GlFunction< void, uint32_t, uint32_t , uint32_t *> m_pfnGetQueryObjectuiv;

		/** see https://www.opengl.org/sdk/docs/man/html/glGetQueryObject.xhtml
		*/
		GlFunction< void, uint32_t, uint32_t , int64_t *> m_pfnGetQueryObjecti64v;

		/** see https://www.opengl.org/sdk/docs/man/html/glGetQueryObject.xhtml
		*/
		GlFunction< void, uint32_t, uint32_t , uint64_t *> m_pfnGetQueryObjectui64v;

		//@}
		/*@name GL_ARB_program_interface_query */
		//@{

		/** see https://www.opengl.org/sdk/docs/man/html/glGetProgramInterface.xhtml
		*/
		GlFunction< void, uint32_t, uint32_t , uint32_t , int * > m_pfnGetProgramInterfaceiv;

		/** see https://www.opengl.org/sdk/docs/man/html/glGetProgramResourceIndex.xhtml
		*/
		GlFunction< int, uint32_t, uint32_t , char const * > m_pfnGetProgramResourceIndex;

		/** see https://www.opengl.org/sdk/docs/man/html/glGetProgramResourceLocation.xhtml
		*/
		GlFunction< int, uint32_t, uint32_t , char const * > m_pfnGetProgramResourceLocation;

		/** see https://www.opengl.org/sdk/docs/man/html/glGetProgramResourceLocationIndex.xhtml
		*/
		GlFunction< int, uint32_t, uint32_t , char const * > m_pfnGetProgramResourceLocationIndex;

		/** see https://www.opengl.org/sdk/docs/man/html/glGetProgramResourceName.xhtml
		*/
		GlFunction< void, uint32_t, uint32_t , uint32_t , int , int * , char * > m_pfnGetProgramResourceName;

		/** see https://www.opengl.org/sdk/docs/man/html/glGetProgramResource.xhtml
		*/
		GlFunction< void, uint32_t, uint32_t , uint32_t , int , uint32_t * , int , int * , int * > m_pfnGetProgramResourceiv;

		//@}
		/**@name Memory transactions */
		//@{

		/** see https://www.opengl.org/sdk/docs/man/html/glMemoryBarrier.xhtml
		*/
		GlFunction< void, uint32_t > m_pfnMemoryBarrier;

		/** see https://www.opengl.org/sdk/docs/man/html/glMemoryBarrier.xhtml
		*/
		GlFunction< void, uint32_t > m_pfnMemoryBarrierByRegion;

		//@}

		GlFunction< void, uint32_t, int > m_pfnPatchParameteri;
	};

	namespace gl_api
	{
		template< typename Func >
		bool GetFunction( Castor::String const & p_name, Func & p_func )
		{
#if defined( CASTOR_PLATFORM_WINDOWS )
			p_func = reinterpret_cast< Func >( wglGetProcAddress( Castor::string::string_cast< char >( p_name ).c_str() ) );
#else
			p_func = reinterpret_cast< Func >( glXGetProcAddressARB( reinterpret_cast< GLubyte const * >( Castor::string::string_cast< char >( p_name ).c_str() ) ) );
#endif
			return p_func != nullptr;
		}

		template< typename Ret, typename ... Arguments >
		bool GetFunction( Castor::String const & p_name, GlFunction< Ret, Arguments... > & p_func )
		{
			typedef Ret( CALLBACK * PFNType )( Arguments... );
			PFNType l_pfnResult = nullptr;

			if ( GetFunction( p_name, l_pfnResult ) )
			{
				p_func = l_pfnResult;
			}

			return l_pfnResult != nullptr;
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

		template< typename T >
		inline void GetFunction( T & p_function, Castor::String const & p_name, Castor::String const & p_extension )
		{
			if ( !gl_api::GetFunction( p_name, p_function ) )
			{
				if ( !gl_api::GetFunction( p_name + p_extension, p_function ) )
				{
					Castor::Logger::LogWarning( cuT( "Unable to retrieve function " ) + p_name );
				}
			}
		}
	}

#	define MAKE_GL_EXTENSION( x )	static const Castor::String x = cuT( "GL_" ) cuT( #x )

	MAKE_GL_EXTENSION( AMD_draw_buffers_blend );
	MAKE_GL_EXTENSION( AMDX_debug_output );
	MAKE_GL_EXTENSION( ARB_compute_shader );
	MAKE_GL_EXTENSION( ARB_compute_variable_group_size );
	MAKE_GL_EXTENSION( ARB_debug_output );
	MAKE_GL_EXTENSION( ARB_draw_buffers_blend );
	MAKE_GL_EXTENSION( ARB_draw_instanced );
	MAKE_GL_EXTENSION( ARB_explicit_uniform_location );
	MAKE_GL_EXTENSION( ARB_fragment_program );
	MAKE_GL_EXTENSION( ARB_framebuffer_object );
	MAKE_GL_EXTENSION( ARB_geometry_shader4 );
	MAKE_GL_EXTENSION( ARB_imaging );
	MAKE_GL_EXTENSION( ARB_instanced_arrays );
	MAKE_GL_EXTENSION( ARB_pixel_buffer_object );
	MAKE_GL_EXTENSION( ARB_program_interface_query );
	MAKE_GL_EXTENSION( ARB_sampler_objects );
	MAKE_GL_EXTENSION( ARB_shader_atomic_counters );
	MAKE_GL_EXTENSION( ARB_shader_image_load_store );
	MAKE_GL_EXTENSION( ARB_shader_storage_buffer_object );
	MAKE_GL_EXTENSION( ARB_tessellation_shader );
	MAKE_GL_EXTENSION( ARB_texture_buffer_object );
	MAKE_GL_EXTENSION( ARB_texture_multisample );
	MAKE_GL_EXTENSION( ARB_texture_non_power_of_two );
	MAKE_GL_EXTENSION( ARB_texture_storage );
	MAKE_GL_EXTENSION( ARB_texture_storage_multisample );
	MAKE_GL_EXTENSION( ARB_timer_query );
	MAKE_GL_EXTENSION( ARB_uniform_buffer_object );
	MAKE_GL_EXTENSION( ARB_vertex_array_object );
	MAKE_GL_EXTENSION( ARB_vertex_buffer_object );
	MAKE_GL_EXTENSION( ARB_vertex_program );
	MAKE_GL_EXTENSION( ARB_transform_feedback2 );
	MAKE_GL_EXTENSION( ATI_meminfo );
	MAKE_GL_EXTENSION( EXT_direct_state_access );
	MAKE_GL_EXTENSION( EXT_draw_instanced );
	MAKE_GL_EXTENSION( EXT_framebuffer_object );
	MAKE_GL_EXTENSION( EXT_geometry_shader4 );
	MAKE_GL_EXTENSION( EXT_instanced_arrays );
	MAKE_GL_EXTENSION( EXT_sampler_objects );
	MAKE_GL_EXTENSION( EXT_texture_filter_anisotropic );
	MAKE_GL_EXTENSION( EXT_uniform_buffer_object );
	MAKE_GL_EXTENSION( KHR_debug );
	MAKE_GL_EXTENSION( NV_shader_buffer_load );
	MAKE_GL_EXTENSION( NV_vertex_buffer_unified_memory );
	MAKE_GL_EXTENSION( NVX_gpu_memory_info );

#	if defined( CASTOR_PLATFORM_WINDOWS )

#	define MAKE_WGL_EXTENSION( x )	static const Castor::String x = cuT( "WGL_" ) cuT( #x );
	MAKE_WGL_EXTENSION( ARB_create_context )
	MAKE_WGL_EXTENSION( ARB_pixel_format )
	MAKE_WGL_EXTENSION( EXT_swap_control )

#	elif defined( CASTOR_PLATFORM_LINUX )

#	define MAKE_GLX_EXTENSION( x )	static const Castor::String x = cuT( "GLX_" ) cuT( #x );
	MAKE_GLX_EXTENSION( ARB_create_context )
	MAKE_GLX_EXTENSION( EXT_swap_control )

#	else

#		error "Yet unsupported OS"

#	endif
}

#include "OpenGl.inl"

#endif
