/* See LICENSE file in root folder */
#ifndef ___C3DGLES3_GlES3Objects___
#define ___C3DGLES3_GlES3Objects___

#include <egl/egl.h>
#include <GLES3/gl3.h>

#include "Common/GlES3Object.hpp"
#include "Miscellaneous/GlES3Debug.hpp"

#include <Log/Logger.hpp>

#include <GlslWriterPrerequisites.hpp>

#include <Texture/TextureUnit.hpp>

#if defined( MemoryBarrier )
#	undef MemoryBarrier
#endif

//*************************************************************************************************

namespace GlES3Render
{
	class OpenGlES3
		: public Castor::NonCopyable
	{
	public:
		struct PixelFmt
		{
			GlES3Format Format;
			GlES3Internal Internal;
			GlES3Type Type;

			PixelFmt() {}

			PixelFmt( GlES3Format p_format, GlES3Internal p_eInternal, GlES3Type p_type )
				: Format( p_format )
				, Internal( p_eInternal )
				, Type( p_type )
			{
			}
		};

	public:
		OpenGlES3( GlES3RenderSystem & p_renderSystem );
		~OpenGlES3();
		bool PreInitialise( Castor::String const & p_strExtensions );
		bool Initialise();
		void InitialiseDebug();
		void Cleanup();
		bool GlES3CheckError( std::string const & p_strText )const;
		bool GlES3CheckError( std::wstring const & p_strText )const;
		void DisplayExtensions()const;

		inline OpenGlES3 const & GetOpenGlES3()const
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
		inline GlES3Topology Get( Castor3D::Topology p_index )const;
		inline GlES3TexDim Get( Castor3D::TextureType p_index )const;
		inline GlES3Comparator Get( Castor3D::ComparisonFunc p_eAlphaFunc )const;
		inline GlES3WrapMode Get( Castor3D::WrapMode p_eWrapMode )const;
		inline GlES3InterpolationMode Get( Castor3D::InterpolationMode p_interpolation )const;
		inline GlES3BlendSource Get( Castor3D::BlendSource p_eArgument )const;
		inline GlES3BlendFunc Get( Castor3D::ColourBlendFunc p_mode )const;
		inline GlES3BlendFunc Get( Castor3D::AlphaBlendFunc p_mode )const;
		inline GlES3BlendFactor Get( Castor3D::BlendOperand p_eBlendFactor )const;
		inline PixelFmt const & Get( Castor::PixelFormat p_pixelFormat )const;
		inline GlES3ShaderType Get( Castor3D::ShaderType p_type )const;
		inline GlES3Internal GetInternal( Castor::PixelFormat p_format )const;
		inline Castor::FlagCombination< GlES3BufferBit > GetComponents( Castor::FlagCombination< Castor3D::BufferComponent > const & p_components )const;
		inline GlES3AttachmentPoint Get( Castor3D::AttachmentPoint p_eAttachment )const;
		inline GlES3FrameBufferMode Get( Castor3D::FrameBufferTarget p_target )const;
		inline GlES3Internal GetRboStorage( Castor::PixelFormat p_pixelFormat )const;
		inline GlES3BufferBinding Get( Castor3D::WindowBuffer p_buffer )const;
		inline GlES3BufferBinding Get( GlES3AttachmentPoint p_buffer )const;
		inline GlES3Face Get( Castor3D::Culling p_eFace )const;
		inline GlES3FillMode Get( Castor3D::FillMode p_mode )const;
		inline GlES3Comparator Get( Castor3D::StencilFunc p_func )const;
		inline GlES3StencilOp Get( Castor3D::StencilOp p_eOp )const;
		inline GlES3BlendOp Get( Castor3D::BlendOperation p_eOp )const;
		inline GlES3Comparator Get( Castor3D::DepthFunc p_func )const;
		inline GlES3QueryType Get( Castor3D::QueryType p_value )const;
		inline GlES3QueryInfo Get( Castor3D::QueryInfo p_value )const;
		inline GlES3TextureStorageType Get( Castor3D::TextureStorageType p_value )const;
		inline GlES3TexDim Get( Castor3D::CubeMapFace p_value )const;
		inline GlES3CompareMode Get( Castor3D::ComparisonMode p_value )const;
		inline bool Get( Castor3D::WritingMask p_eMask )const;
		inline Castor::FlagCombination< GlES3BarrierBit > Get( Castor::FlagCombination< Castor3D::MemoryBarrier > const & p_barriers )const;
		inline Castor::String const & GetVendor()const;
		inline Castor::String const & GetRenderer()const;
		inline Castor::String const & GetStrVersion()const;
		inline int GetVersion()const;
		inline int GetGlslVersion()const;
		inline GlES3RenderSystem & GetRenderSystem();
		inline GlES3RenderSystem const & GetRenderSystem()const;
		inline GlES3BufferMode GetBufferFlags( uint32_t p_flags )const;
		inline GlES3Provider GetProvider()const;

		/**@name General Functions */
		//@{

		inline void ClearColor( float red, float green, float blue, float alpha )const;
		inline void ClearColor( Castor::Colour const & p_colour )const;
		inline void ClearDepth( double value )const;
		inline void Clear( uint32_t mask )const;
		inline void Enable( GlES3Tweak mode )const;
		inline void Disable( GlES3Tweak mode )const;
		inline void Enable( GlES3TexDim texture )const;
		inline void Disable( GlES3TexDim texture )const;
		inline void GetIntegerv( uint32_t pname, int * params )const;
		inline void GetIntegerv( GlES3Max pname, int * params )const;
		inline void GetIntegerv( GlES3Min pname, int * params )const;
		inline void GetIntegerv( GlES3GpuInfo pname, int * params )const;
		inline void GetIntegerv( uint32_t pname, uint64_t * params )const;
		inline void DepthFunc( GlES3Comparator p_func )const;
		inline void DepthMask( bool p_bFlag )const;
		inline void ColorMask( bool p_r, bool p_g, bool p_b, bool p_a )const;
		inline void StencilOp( GlES3StencilOp p_eStencilFail, GlES3StencilOp p_eDepthFail, GlES3StencilOp p_eStencilPass )const;
		inline void StencilFunc( GlES3Comparator p_func, int p_iRef, uint32_t p_uiMask )const;
		inline void StencilMask( uint32_t p_uiMask )const;
		inline void StencilOpSeparate( GlES3Face p_eFacing, GlES3StencilOp p_eStencilFail, GlES3StencilOp p_eDepthFail, GlES3StencilOp p_eStencilPass )const;
		inline void StencilFuncSeparate( GlES3Face p_eFacing, GlES3Comparator p_func, int p_iRef, uint32_t p_uiMask )const;
		inline void StencilMaskSeparate( GlES3Face p_eFacing, uint32_t p_uiMask )const;
		inline void Hint( GlES3Hint p_eHint, GlES3HintValue p_eValue )const;
		inline void PolygonOffset( float p_fFactor, float p_fUnits )const;
		inline void BlendColor( Castor::Colour const & p_clrFactors )const;
		inline void SampleCoverage( float fValue, bool invert )const;
		inline void Viewport( int x, int y, int width, int height )const;

		//@}
		/**@name Draw Functions */
		//@{

		inline void DrawArrays( GlES3Topology mode, int first, int count )const;
		inline void DrawElements( GlES3Topology mode, int count, GlES3Type type, const void * indices )const;
		inline void DrawArraysInstanced( GlES3Topology mode, int first, int count, int primcount )const;
		inline void DrawElementsInstanced( GlES3Topology mode, int count, GlES3Type type, const void * indices, int primcount )const;

		//@}
		/**@name Instanciation Functions */
		//@{

		inline void VertexAttribDivisor( uint32_t index, uint32_t divisor )const;

		//@}
		/**@name Context functions */
		//@{

		inline void MakeCurrent( EGLDisplay display, EGLSurface surfaceDraw, EGLSurface surfaceRead, EGLContext context )const;
		inline void SwapBuffers( EGLDisplay display, EGLSurface surface )const;
		inline void SwapInterval( EGLDisplay display, int interval )const;
		inline EGLContext CreateContext( EGLDisplay display, EGLConfig config, EGLContext share, int * attribs )const;
		inline bool DeleteContext( EGLDisplay display, EGLContext context )const;

		//@}
		/**@name Material functions */
		//@{

		inline void CullFace( GlES3Face face )const;
		inline void FrontFace( GlES3FrontFaceDirection face )const;
		inline void BlendFunc( GlES3BlendFactor sfactor, GlES3BlendFactor dfactor )const;
		inline void BlendFunc( GlES3BlendFactor p_eRgbSrc, GlES3BlendFactor p_eRgbDst, GlES3BlendFactor p_eAlphaSrc, GlES3BlendFactor p_eAlphaDst )const;
		inline void BlendEquation( GlES3BlendOp p_eOp )const;

		//@}
		/**@name Texture functions */
		//@{

		inline void GenTextures( int n, uint32_t * textures )const;
		inline void DeleteTextures( int n, uint32_t const * textures )const;
		inline bool IsTexture( uint32_t texture )const;
		inline void BindTexture( GlES3TexDim p_target, uint32_t texture )const;
		inline void ActiveTexture( GlES3TextureIndex target )const;
		inline void GenerateMipmap( GlES3TexDim p_target )const;
		inline void TexSubImage1D( GlES3TextureStorageType p_target, int level, int xoffset, int width, GlES3Format format, GlES3Type type, void const * data )const;
		inline void TexSubImage2D( GlES3TextureStorageType p_target, int level, int xoffset, int yoffset, int width, int height, GlES3Format format, GlES3Type type, void const * data )const;
		inline void TexSubImage2D( GlES3TextureStorageType p_target, int level, Castor::Position const & p_position, Castor::Size const & p_size, GlES3Format format, GlES3Type type, void const * data )const;
		inline void TexSubImage2D( GlES3TextureStorageType p_target, int level, Castor::Rectangle const & p_rect, GlES3Format format, GlES3Type type, void const * data )const;
		inline void TexSubImage3D( GlES3TextureStorageType p_target, int level, int xoffset, int yoffset, int zoffset, int width, int height, int depth, GlES3Format format, GlES3Type type, void const * data )const;
		inline void TexImage1D( GlES3TextureStorageType p_target, int level, GlES3Internal internalFormat, int width, int border, GlES3Format format, GlES3Type type, void const * data )const;
		inline void TexImage2D( GlES3TextureStorageType p_target, int level, GlES3Internal internalFormat, int width, int height, int border, GlES3Format format, GlES3Type type, void const * data )const;
		inline void TexImage2D( GlES3TextureStorageType p_target, int level, GlES3Internal internalFormat, Castor::Size const & p_size, int border, GlES3Format format, GlES3Type type, void const * data )const;
		inline void TexImage3D( GlES3TextureStorageType p_target, int level, GlES3Internal internalFormat, int width, int height, int depth, int border, GlES3Format format, GlES3Type type, void const * data )const;
		inline void TexImage2DMultisample( GlES3TextureStorageType p_target, int p_samples, GlES3Internal p_internalFormat, int p_width, int p_height, bool p_fixedSampleLocations )const;
		inline void TexImage2DMultisample( GlES3TextureStorageType p_target, int p_samples, GlES3Internal p_internalFormat, Castor::Size const & p_size, bool p_fixedSampleLocations )const;
		inline void ReadBuffer( GlES3BufferBinding p_buffer )const;
		inline void ReadPixels( int x, int y, int width, int height, GlES3Format format, GlES3Type type, void * pixels )const;
		inline void ReadPixels( Castor::Position const & p_position, Castor::Size const & p_size, GlES3Format format, GlES3Type type, void * pixels )const;
		inline void ReadPixels( Castor::Rectangle const & p_rect, GlES3Format format, GlES3Type type, void * pixels )const;
		inline void PixelStore( GlES3StorageMode p_mode, int p_iParam )const;
		inline void TexStorage1D( GlES3TextureStorageType target, GLint levels, GlES3Internal internalformat, GLsizei width )const;
		inline void TexStorage2D( GlES3TextureStorageType target, GLint levels, GlES3Internal internalformat, GLsizei width, GLsizei height )const;
		inline void TexStorage3D( GlES3TextureStorageType target, GLint levels, GlES3Internal internalformat, GLsizei width, GLsizei height, GLsizei depth )const;
		inline void TexStorage2DMultisample( GlES3TextureStorageType target, GLsizei samples, GlES3Internal internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations )const;
		inline void TexStorage3DMultisample( GlES3TextureStorageType target, GLsizei samples, GlES3Internal internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations )const;

		//@}
		/**@name Sampler functions */
		//@{

		inline void GenSamplers( int count, uint32_t * samplers )const;
		inline void DeleteSamplers( int count, const uint32_t * samplers )const;
		inline bool IsSampler( uint32_t sampler )const;
		inline void BindSampler( uint32_t unit, uint32_t sampler )const;
		inline void GetSamplerParameter( uint32_t sampler, GlES3SamplerParameter pname, float * params )const;
		inline void GetSamplerParameter( uint32_t sampler, GlES3SamplerParameter pname, int * params )const;
		inline void SetSamplerParameter( uint32_t sampler, GlES3SamplerParameter pname, float param )const;
		inline void SetSamplerParameter( uint32_t sampler, GlES3SamplerParameter pname, const float * params )const;
		inline void SetSamplerParameter( uint32_t sampler, GlES3SamplerParameter pname, int param )const;
		inline void SetSamplerParameter( uint32_t sampler, GlES3SamplerParameter pname, const int * params )const;

		//@}
		/**@name Texture Buffer objects functions */
		//@{

		inline void TexBuffer( GlES3TexDim p_target, GlES3Internal p_internalFormat, uint32_t buffer )const;

		//@}
		/**@name Buffer objects functions */
		//@{

		inline void GenBuffers( int n, uint32_t * buffers )const;
		inline void DeleteBuffers( int n, uint32_t const * buffers )const;
		inline bool IsBuffer( uint32_t buffer )const;
		inline void BindBuffer( GlES3BufferTarget target, uint32_t buffer )const;
		inline void BufferData( GlES3BufferTarget target, ptrdiff_t size, void const * data, GlES3BufferMode usage )const;
		inline void BufferSubData( GlES3BufferTarget target, ptrdiff_t offset, ptrdiff_t size, void const * data )const;
		inline void CopyBufferSubData( GlES3BufferTarget readtarget, GlES3BufferTarget writetarget, ptrdiff_t readoffset, ptrdiff_t writeoffset, ptrdiff_t size )const;
		inline void UnmapBuffer( GlES3BufferTarget target )const;
		inline void * MapBufferRange( GlES3BufferTarget target, ptrdiff_t offset, ptrdiff_t length, uint32_t access )const;
		inline void GetBufferParameter( GlES3BufferTarget target, GlES3BufferParameter pname, int * params )const;
		inline void GetBufferParameter( GlES3BufferTarget target, GlES3BufferParameter pname, uint64_t * params )const;
		inline void FlushMappedBufferRange( GlES3BufferTarget target, ptrdiff_t offset, ptrdiff_t length )const;
		inline void BufferAddressRange( GlES3Address pname, uint32_t index, uint64_t address, size_t length )const;
		inline void VertexFormat( int size, GlES3Type type, int stride )const;
		inline void NormalFormat( GlES3Type type, int stride )const;
		inline void ColorFormat( int size, GlES3Type type, int stride )const;
		inline void IndexFormat( GlES3Type type, int stride )const;
		inline void TexCoordFormat( int size, GlES3Type type, int stride )const;
		inline void EdgeFlagFormat( int stride )const;
		inline void SecondaryColorFormat( int size, GlES3Type type, int stride )const;
		inline void FogCoordFormat( uint32_t type, int stride )const;
		inline void VertexAttribFormat( uint32_t index, int size, GlES3Type type, bool normalized, int stride )const;
		inline void VertexAttribIFormat( uint32_t index, int size, GlES3Type type, int stride )const;
		inline void MakeBufferResident( GlES3BufferTarget target, GlES3AccessType access )const;
		inline void MakeBufferNonResident( GlES3BufferTarget target )const;
		inline bool IsBufferResident( GlES3BufferTarget target )const;
		inline void MakeNamedBufferResident( uint32_t buffer, GlES3AccessType access )const;
		inline void MakeNamedBufferNonResident( uint32_t buffer )const;
		inline bool IsNamedBufferResident( uint32_t buffer )const;
		inline void GetNamedBufferParameter( uint32_t buffer, GlES3BufferParameter pname, uint64_t * params )const;

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
		inline void BindTransformFeedback( GlES3BufferTarget target, uint32_t buffer )const;

		/** see https://www.opengl.org/sdk/docs/man4/html/glIsTransformFeedback.xhtml
		*/
		inline bool IsTransformFeedback( uint32_t buffer )const;

		/** see https://www.opengl.org/sdk/docs/man4/html/glBeginTransformFeedback.xhtml
		*/
		inline void BeginTransformFeedback( GlES3Topology primitive )const;

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
		inline void TransformFeedbackVaryings( uint32_t program, int count, char const * const * varyings, GlES3AttributeLayout bufferMode )const;

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
		inline void BindFramebuffer( GlES3FrameBufferMode p_eBindingMode, uint32_t framebuffer )const;

		/** see https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glFramebufferTexture.xml
		*/
		inline void FramebufferTexture2D( GlES3FrameBufferMode p_eBindingMode, GlES3AttachmentPoint p_eAttachment, GlES3TexDim textarget, uint32_t texture, int level )const;

		/** see https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glFramebufferTextureLayer.xml
		*/
		inline void FramebufferTextureLayer( GlES3FrameBufferMode p_eBindingMode, GlES3AttachmentPoint p_eAttachment, uint32_t texture, int level, int layer )const;

		/** see https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glFramebufferRenderbuffer.xml
		*/
		inline void FramebufferRenderbuffer( GlES3FrameBufferMode p_eBindingMode, GlES3AttachmentPoint p_eAttachment, GlES3RenderBufferMode p_eRboTarget, uint32_t renderbufferId )const;

		/** see https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glCheckFramebufferStatus.xml
		*/
		inline GlES3FramebufferStatus CheckFramebufferStatus( GlES3FrameBufferMode p_eBindingMode )const;

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
		inline void BindRenderbuffer( GlES3RenderBufferMode p_eBindingMode, uint32_t id )const;
		inline void RenderbufferStorage( GlES3RenderBufferMode p_eBindingMode, GlES3Internal internalFormat, int width, int height )const;
		inline void RenderbufferStorageMultisample( GlES3RenderBufferMode p_eBindingMode, int p_iSamples, GlES3Internal internalFormat, int width, int height )const;
		inline void RenderbufferStorage( GlES3RenderBufferMode p_eBindingMode, GlES3Internal internalFormat, Castor::Size const & size )const;
		inline void RenderbufferStorageMultisample( GlES3RenderBufferMode p_eBindingMode, int p_iSamples, GlES3Internal internalFormat, Castor::Size const & size )const;
		inline void GetRenderbufferParameteriv( GlES3RenderBufferMode p_eBindingMode, GlES3RenderBufferParameter param, int * value )const;
		inline void BlitFramebuffer( int srcX0, int srcY0, int srcX1, int srcY1, int dstX0, int dstY0, int dstX1, int dstY1, uint32_t mask, GlES3InterpolationMode filter )const;
		inline void BlitFramebuffer( Castor::Rectangle const & rcSrc, Castor::Rectangle const & rcDst, uint32_t mask, GlES3InterpolationMode filter )const;
		inline void BlitFramebuffer( int srcX0, int srcY0, int srcX1, int srcY1, int dstX0, int dstY0, int dstX1, int dstY1, GlES3Component mask, GlES3InterpolationMode filter )const;
		inline void BlitFramebuffer( Castor::Rectangle const & rcSrc, Castor::Rectangle const & rcDst, GlES3Component mask, GlES3InterpolationMode filter )const;
		inline void DrawBuffers( int n, const uint32_t * bufs )const;

		//@}
		/**@name Uniform variable Functions */
		//@{

		inline int GetUniformLocation( uint32_t program, char const * name )const;
		inline void SetUniform( int location, int v0 )const;
		inline void SetUniform( int location, uint32_t v0 )const;
		inline void SetUniform( int location, float v0 )const;
		inline void SetUniform( int location, int v0, int v1 )const;
		inline void SetUniform( int location, int v0, int v1, int v2 )const;
		inline void SetUniform( int location, int v0, int v1, int v2, int v3 )const;
		inline void SetUniform( int location, uint32_t v0, uint32_t v1 )const;
		inline void SetUniform( int location, uint32_t v0, uint32_t v1, uint32_t v2 )const;
		inline void SetUniform( int location, uint32_t v0, uint32_t v1, uint32_t v2, uint32_t v3 )const;
		inline void SetUniform( int location, float v0, float v1 )const;
		inline void SetUniform( int location, float v0, float v1, float v2 )const;
		inline void SetUniform( int location, float v0, float v1, float v2, float v3 )const;
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
		inline void SetUniform2x2v( int location, int count, bool transpose, float const * value )const;
		inline void SetUniform2x3v( int location, int count, bool transpose, float const * value )const;
		inline void SetUniform2x4v( int location, int count, bool transpose, float const * value )const;
		inline void SetUniform3x3v( int location, int count, bool transpose, float const * value )const;
		inline void SetUniform3x2v( int location, int count, bool transpose, float const * value )const;
		inline void SetUniform3x4v( int location, int count, bool transpose, float const * value )const;
		inline void SetUniform4x4v( int location, int count, bool transpose, float const * value )const;
		inline void SetUniform4x2v( int location, int count, bool transpose, float const * value )const;
		inline void SetUniform4x3v( int location, int count, bool transpose, float const * value )const;

		//@}
		/**@name Uniform Buffer Objects Functions */
		//@{

		inline uint32_t GetUniformBlockIndex( uint32_t shader, char const * uniformBlockName )const;
		inline void BindBufferBase( GlES3BufferTarget target, uint32_t index, uint32_t buffer )const;
		inline void UniformBlockBinding( uint32_t shader, uint32_t uniformBlockIndex, uint32_t uniformBlockBinding )const;
		inline void GetUniformIndices( uint32_t shader, int uniformCount, char const * const * uniformNames, uint32_t * uniformIndices )const;
		inline void GetActiveUniformsiv( uint32_t shader, int uniformCount, uint32_t const * uniformIndices, GlES3UniformValue pname, int * params )const;
		inline void GetActiveUniformBlockiv( uint32_t shader, uint32_t uniformBlockIndex, GlES3UniformBlockValue pname, int * params )const;

		//@}
		/**@name Shader object Functions */
		//@{

		inline uint32_t CreateShader( GlES3ShaderType type )const;
		inline void DeleteShader( uint32_t program )const;
		inline bool IsShader( uint32_t program )const;
		inline void AttachShader( uint32_t program, uint32_t shader )const;
		inline void DetachShader( uint32_t program, uint32_t shader )const;
		inline void CompileShader( uint32_t program )const;
		inline void GetShaderiv( uint32_t program, GlES3ShaderStatus pname, int * param )const;
		inline void GetShaderInfoLog( uint32_t program, int bufSize, int * length, char * infoLog )const;
		inline void ShaderSource( uint32_t program, int count, char const * const * strings, int const * lengths )const;
		inline void GetActiveAttrib( uint32_t program, uint32_t index, int bufSize, int * length, int * size, uint32_t * type, char * name )const;
		inline void GetActiveUniform( uint32_t program, uint32_t index, int bufSize, int * length, int * size, uint32_t * type, char * name )const;

		//@}
		/**@name Shader program Functions */
		//@{

		inline uint32_t CreateProgram()const;
		inline void DeleteProgram( uint32_t program )const;
		inline void LinkProgram( uint32_t program )const;
		inline void UseProgram( uint32_t program )const;
		inline void GetProgramiv( uint32_t program, GlES3ShaderStatus pname, int * param )const;
		inline void GetProgramInfoLog( uint32_t program, int bufSize, int * length, char * infoLog )const;
		inline int GetAttribLocation( uint32_t program, char const * name )const;
		inline bool IsProgram( uint32_t program )const;
		inline void ProgramParameteri( uint32_t program, uint32_t pname, int value )const;

		//@}
		/**@name Vertex Attribute Pointer functions */
		//@{

		inline void EnableVertexAttribArray( uint32_t index )const;
		inline void VertexAttribPointer( uint32_t index, int size, GlES3Type type, bool normalized, int stride, void const * pointer )const;
		inline void VertexAttribPointer( uint32_t index, int size, GlES3Type type, int stride, void const * pointer )const;
		inline void DisableVertexAttribArray( uint32_t index )const;

		//@}
		/**@name Vertex Array Objects */
		//@{

		inline void GenVertexArrays( int n, uint32_t * arrays )const;
		inline bool IsVertexArray( uint32_t array )const;
		inline void BindVertexArray( uint32_t array )const;
		inline void DeleteVertexArrays( int n, uint32_t const * arrays )const;

		//@}
		/**@name Query functions */
		//@{

		inline void GenQueries( int p_n, uint32_t * p_queries )const;
		inline void DeleteQueries( int p_n, uint32_t const * p_queries )const;
		inline bool IsQuery( uint32_t p_query )const;
		inline void BeginQuery( GlES3QueryType p_target, uint32_t p_query )const;
		inline void EndQuery( GlES3QueryType p_target )const;
		inline void GetQueryObjectInfos( uint32_t p_id, GlES3QueryInfo p_name, int32_t * p_params )const;
		inline void GetQueryObjectInfos( uint32_t p_id, GlES3QueryInfo p_name, uint32_t * p_params )const;

		//@}
		/**@name Other functions */
		//@{

		inline GlES3AccessType GetLockFlags( Castor3D::AccessTypes const & p_flags )const;
		inline Castor::FlagCombination< GlES3BufferMappingBit > GetBitfieldFlags( Castor3D::AccessTypes const & p_flags )const;
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
		std::array< GlES3Topology, size_t( Castor3D::Topology::eCount ) > PrimitiveTypes;
		std::array< GlES3TexDim, size_t( Castor3D::TextureType::eCount ) > TextureDimensions;
		std::array< GlES3Comparator, size_t( Castor3D::ComparisonFunc::eCount ) > AlphaFuncs;
		std::array< GlES3WrapMode, size_t( Castor3D::WrapMode::eCount ) > TextureWrapMode;
		std::array< GlES3InterpolationMode, size_t( Castor3D::InterpolationMode::eCount ) > TextureInterpolation;
		std::array< GlES3BlendFactor, size_t( Castor3D::BlendOperand::eCount ) > BlendFactors;
		std::array< GlES3BlendSource, size_t( Castor3D::BlendSource::eCount ) > TextureArguments;
		std::array< GlES3BlendFunc, size_t( Castor3D::ColourBlendFunc::eCount ) > RgbBlendFuncs;
		std::array< GlES3BlendFunc, size_t( Castor3D::AlphaBlendFunc::eCount ) > AlphaBlendFuncs;
		std::array< GlES3BlendOp, size_t( Castor3D::BlendOperation::eCount ) > BlendOps;
		std::array< PixelFmt, size_t( Castor::PixelFormat::eCount ) > PixelFormats;
		std::array< GlES3ShaderType, size_t( Castor3D::ShaderType::eCount ) > ShaderTypes;
		std::array< GlES3Internal, size_t( Castor::PixelFormat::eCount ) > Internals;
		std::array< GlES3AttachmentPoint, size_t( Castor3D::AttachmentPoint::eCount ) > Attachments;
		std::array< GlES3FrameBufferMode, size_t( Castor3D::FrameBufferMode::eCount ) > FramebufferModes;
		std::array< GlES3Internal, size_t( Castor::PixelFormat::eCount ) > RboStorages;
		std::array< GlES3BufferBinding, size_t( Castor3D::WindowBuffer::eCount ) > Buffers;
		std::array< GlES3Face, size_t( Castor3D::Culling::eCount ) > Faces;
		std::array< GlES3FillMode, 3u > FillModes;
		std::array< GlES3StencilOp, size_t( Castor3D::StencilOp::eCount ) > StencilOps;
		std::array< GlES3Comparator, size_t( Castor3D::StencilFunc::eCount ) > StencilFuncs;
		std::array< GlES3QueryType, size_t( Castor3D::QueryType::eCount ) > Queries;
		std::array< GlES3QueryInfo, size_t( Castor3D::QueryInfo::eCount ) > QueryInfos;
		std::array< GlES3TextureStorageType, size_t( Castor3D::TextureStorageType::eCount ) > TextureStorages;
		std::array< GlES3TexDim, size_t( Castor3D::CubeMapFace::eCount ) > CubeMapFaces;
		std::array< GlES3CompareMode, size_t( Castor3D::ComparisonMode::eCount ) > ComparisonModes;
		std::array< bool, size_t( Castor3D::WritingMask::eCount ) > WriteMasks;
		std::array< GlES3Comparator, size_t( Castor3D::DepthFunc::eCount ) > DepthFuncs;
		std::map< GlES3AttachmentPoint, GlES3BufferBinding > BuffersTA;

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
		GlES3RenderSystem & m_renderSystem;
		GlES3Provider m_gpu{ GlES3Provider::eUnknown };

		GlES3Debug m_debug;
	};
}

#include "OpenGlES3.inl"

#endif
