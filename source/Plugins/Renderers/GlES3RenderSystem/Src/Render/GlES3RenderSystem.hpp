/* See LICENSE file in root folder */
#ifndef ___C3DGLES3_RENDER_SYSTEM_H___
#define ___C3DGLES3_RENDER_SYSTEM_H___

#include "GlES3RenderSystemPrerequisites.hpp"
#include "Render/GlES3RenderPipeline.hpp"
#include "Render/GlES3Context.hpp"

#include <Render/RenderSystem.hpp>

namespace GlES3Render
{
	class GlES3RenderSystem
		: public Castor3D::RenderSystem
	{
	public:
		GlES3RenderSystem( Castor3D::Engine & p_engine );
		virtual ~GlES3RenderSystem();

		static Castor3D::RenderSystemUPtr Create( Castor3D::Engine & p_engine );

		//!< Initialize OpenGL Extensions
		bool InitOpenGlES3Extensions();
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateContext
		 */
		Castor3D::ContextSPtr CreateContext()override;
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateGeometryBuffers
		 */
		Castor3D::GeometryBuffersSPtr CreateGeometryBuffers( Castor3D::Topology p_topology
			, Castor3D::ShaderProgram const & p_program )override;
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateRenderPipeline
		 */
		Castor3D::RenderPipelineUPtr CreateRenderPipeline( Castor3D::DepthStencilState && p_dsState
			, Castor3D::RasteriserState && p_rsState
			, Castor3D::BlendState && p_bdState
			, Castor3D::MultisampleState && p_msState
			, Castor3D::ShaderProgram & p_program
			, Castor3D::PipelineFlags const & p_flags )override;
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateComputePipeline
		 */
		Castor3D::ComputePipelineUPtr CreateComputePipeline( Castor3D::ShaderProgram & p_program )override;
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateSampler
		 */
		Castor3D::SamplerSPtr CreateSampler( Castor::String const & p_name )override;
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateShaderProgram
		 */
		Castor3D::ShaderProgramSPtr CreateShaderProgram()override;
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateUniformBufferBinding
		 */
		Castor3D::UniformBufferBindingUPtr CreateUniformBufferBinding(Castor3D::UniformBuffer & p_ubo
			, Castor3D::ShaderProgram const & p_program )override;
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateUInt8Buffer
		 */
		std::unique_ptr< Castor3D::GpuBuffer< uint8_t > > CreateUInt8Buffer( Castor3D::BufferType p_type )override;
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateUInt16Buffer
		 */
		std::unique_ptr< Castor3D::GpuBuffer< uint16_t > > CreateUInt16Buffer( Castor3D::BufferType p_type )override;
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateUInt32Buffer
		 */
		std::unique_ptr< Castor3D::GpuBuffer< uint32_t > > CreateUInt32Buffer( Castor3D::BufferType p_type )override;
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateFloatBuffer
		 */
		std::unique_ptr< Castor3D::GpuBuffer< float > > CreateFloatBuffer( Castor3D::BufferType p_type )override;
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateTransformFeedback
		 */
		Castor3D::TransformFeedbackUPtr CreateTransformFeedback( Castor3D::BufferDeclaration const & p_computed
			, Castor3D::Topology p_topology
			, Castor3D::ShaderProgram & p_program )override;
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateTexture
		 */
		Castor3D::TextureLayoutSPtr CreateTexture( Castor3D::TextureType p_type
			, Castor3D::AccessTypes const & p_cpuAccess
			, Castor3D::AccessTypes const & p_gpuAccess )override;
		/**
		*\copydoc		Castor3D::RenderSystem::CreateTexture
		*/
		Castor3D::TextureLayoutSPtr CreateTexture( Castor3D::TextureType p_type
			, Castor3D::AccessTypes const & p_cpuAccess
			, Castor3D::AccessTypes const & p_gpuAccess
			, Castor::PixelFormat p_format
			, Castor::Size const & p_size )override;
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateTexture
		 */
		Castor3D::TextureLayoutSPtr CreateTexture( Castor3D::TextureType p_type
			, Castor3D::AccessTypes const & p_cpuAccess
			, Castor3D::AccessTypes const & p_gpuAccess
			, Castor::PixelFormat p_format
			, Castor::Point3ui const & p_size )override;
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateTextureStorage
		 */
		Castor3D::TextureStorageUPtr CreateTextureStorage( Castor3D::TextureStorageType p_type
			, Castor3D::TextureLayout & p_layout
			, Castor3D::AccessTypes const & p_cpuAccess
			, Castor3D::AccessTypes const & p_gpuAccess )override;
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateFrameBuffer
		 */
		Castor3D::FrameBufferSPtr CreateFrameBuffer()override;
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateBackBuffers
		 */
		Castor3D::BackBuffersSPtr CreateBackBuffers()override;
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateQuery
		 */
		Castor3D::GpuQueryUPtr CreateQuery( Castor3D::QueryType p_type )override;
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateViewport
		 */
		Castor3D::IViewportImplUPtr CreateViewport( Castor3D::Viewport & p_viewport )override;

		inline bool IsExtensionInit()
		{
			return m_extensionsInit;
		}

		inline int GetOpenGlES3Major()
		{
			return m_openGlES3Major;
		}

		inline int GetOpenGlES3Minor()
		{
			return m_openGlES3Minor;
		}

		inline void SetOpenGlES3Version( int p_iMajor, int p_iMinor )
		{
			m_openGlES3Major = p_iMajor;
			m_openGlES3Minor = p_iMinor;
		}

		inline OpenGlES3 & GetOpenGlES3()
		{
			return m_openGlES3;
		}

	private:
		/**
		 *\copydoc		Castor3D::RenderSystem::DoInitialise
		 */
		void DoInitialise()override;
		/**
		 *\copydoc		Castor3D::RenderSystem::DoCleanup
		 */
		void DoCleanup()override;

	public:
		static Castor::String Name;
		static Castor::String Type;

	protected:
		bool m_useVertexBufferObjects;
		bool m_extensionsInit;
		int m_openGlES3Major;
		int m_openGlES3Minor;
		OpenGlES3 m_openGlES3;

#if C3D_TRACE_OBJECTS

	public:
		bool Track( void * p_object, std::string const & p_name, std::string const & p_file, int line );
		bool Untrack( void * p_object );

#else

	public:
		bool Track( void * p_object, std::string const & p_name, std::string const & p_file, int line )
		{
		}
		bool Untrack( void * p_object )
		{
		}

#endif
	};
}

#endif
