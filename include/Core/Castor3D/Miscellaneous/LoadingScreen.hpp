/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LoadingScreen_H___
#define ___C3D_LoadingScreen_H___

#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/Culling/CullingModule.hpp"
#include "Castor3D/Render/Passes/PassesModule.hpp"
#include "Castor3D/Render/Technique/TechniqueModule.hpp"
#include "Castor3D/Scene/SceneModule.hpp"
#include "Castor3D/Scene/Background/BackgroundModule.hpp"

#include "Castor3D/Miscellaneous/ProgressBar.hpp"
#include "Castor3D/Shader/Ubos/HdrConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelUbo.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

#include <CastorUtils/Math/RangedValue.hpp>

#include <RenderGraph/FrameGraph.hpp>
#include <RenderGraph/RunnablePass.hpp>
#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#pragma warning( push )
#pragma warning( disable:4365 )
#include <atomic>
#pragma warning( pop )

namespace castor3d
{
	class LoadingScreen
	{
	public:
		C3D_API LoadingScreen( ProgressBar & progressBar
			, RenderDevice const & device
			, crg::ResourceHandler & handler
			, SceneSPtr scene
			, VkRenderPass renderPass
			, castor::Size const & size );
		C3D_API ~LoadingScreen();

		C3D_API void enable();
		C3D_API void disable();
		C3D_API void update( CpuUpdater & updater );
		C3D_API void update( GpuUpdater & updater );
		C3D_API void setRenderPass( VkRenderPass renderPass
			, castor::Size const & renderSize );
		C3D_API crg::SemaphoreWaitArray render( ashes::Queue const & queue
			, ashes::FrameBuffer const & framebuffer
			, crg::SemaphoreWaitArray const & toWait
			, crg::Fence *& fence );

		crg::SemaphoreWaitArray render( ashes::Queue const & queue
			, ashes::FrameBuffer const & framebuffer
			, crg::SemaphoreWait const & toWait
			, crg::Fence *& fence )
		{
			return render( queue
				, framebuffer
				, crg::SemaphoreWaitArray{ toWait }
				, fence );
		}

		void step( castor::String const & label )
		{
			m_progressBar.step( label );
		}

		void setRange( uint32_t max )
		{
			m_progressBar.setRange( max );
		}

		void incRange( uint32_t mod )
		{
			m_progressBar.incRange( mod );
		}

		uint32_t getIndex()const
		{
			return m_progressBar.getIndex();
		}

		bool isEnabled()const
		{
			return m_enabled;
		}

		ProgressBar & getProgressBar()
		{
			return m_progressBar;
		}

	private:
		crg::FramePass & doCreateOpaquePass( crg::FramePass const * previousPass );
		crg::FramePass & doCreateTransparentPass( crg::FramePass const * previousPass );
		crg::FramePass & doCreateOverlayPass( crg::FramePass const * previousPass );
		crg::FramePass & doCreateWindowPass( crg::FramePass const * previousPass );

	public:
		C3D_API static castor::String const SceneName;

	private:
		class WindowPass
			: public crg::RunnablePass
		{
		public:
			WindowPass( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph
				, RenderDevice const & device
				, VkRenderPass renderPass
				, VkExtent2D const & renderSize );

			void setRenderPass( VkRenderPass renderPass
				, VkExtent2D const & renderSize );
			void setTarget( VkFramebuffer framebuffer
				, std::vector< VkClearValue > clearValues );

			crg::Fence & getFence()
			{
				return m_fence;
			}

		private:
			void doInitialise();
			void doRecordInto( VkCommandBuffer commandBuffer
				, uint32_t index );

		private:
			VkExtent2D m_renderSize;
			VkRenderPass m_renderPass{};
			ShaderModule m_vertexShader;
			ShaderModule m_pixelShader;
			ashes::PipelineShaderStageCreateInfoArray m_stages;
			crg::RenderQuadHolder m_renderQuad;
			VkFramebuffer m_framebuffer{};
			std::vector< VkClearValue > m_clearValues;
		};

	private:
		RenderDevice const & m_device;
		ProgressBar & m_progressBar;
		crg::FrameGraph m_graph;
		std::atomic_bool m_enabled{};
		SceneSPtr m_scene;
		SceneBackground & m_background;
		VkRenderPass m_renderPass;
		castor::Size m_renderSize;
		CameraSPtr m_camera;
		SceneCullerUPtr m_culler;
		Texture m_colour;
		Texture m_depth;
		MatrixUbo m_matrixUbo;
		HdrConfigUbo m_hdrConfigUbo;
		SceneUbo m_sceneUbo;
		BackgroundRendererUPtr m_backgroundRenderer;
		crg::FramePass * m_opaquePassDesc{};
		RenderTechniquePass * m_opaquePass{};
		crg::FramePass * m_transparentPassDesc{};
		RenderTechniquePass * m_transparentPass{};
		crg::FramePass * m_overlayPassDesc{};
		OverlayPass * m_overlayPass{};
		crg::FramePass * m_windowPassDesc{};
		WindowPass * m_windowPass{};
		crg::RunnableGraphPtr m_runnable;
	};
}

#endif
