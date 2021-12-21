/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BackgroundPass_H___
#define ___C3D_BackgroundPass_H___

#include "PassesModule.hpp"
#include "Castor3D/Scene/Background/BackgroundModule.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"

#include "Castor3D/Render/Viewport.hpp"

#include <RenderGraph/RunnablePasses/RenderPass.hpp>

#include <ashespp/Pipeline/GraphicsPipeline.hpp>

namespace castor3d
{
	class BackgroundPass
		: public crg::RenderPass
	{
	public:
		C3D_API BackgroundPass( crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, RenderDevice const & device
			, SceneBackground & background
			, VkExtent2D const & size
			, bool usesDepth );
		/**
		 *\~english
		 *\brief			Updates the render pass, CPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau CPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( CpuUpdater & updater );
		/**
		 *\~english
		 *\brief			Updates the render pass, GPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau GPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( GpuUpdater & updater );

	private:
		void doSubInitialise();
		void doSubRecordInto( crg::RecordContext & context
			, VkCommandBuffer commandBuffer
			, uint32_t index );
		bool doIsEnabled()const;

		void doInitialiseVertexBuffer();
		ashes::PipelineShaderStageCreateInfoArray doInitialiseShader();
		void doFillDescriptorBindings();
		void doCreateDescriptorSet();
		void doCreatePipeline();
		ashes::PipelineViewportStateCreateInfo doCreateViewportState();
		void doResetPipeline();

	private:
		struct Cube
		{
			struct Quad
			{
				struct Vertex
				{
					castor::Point3f position;
				};

				Vertex vertex[4];
			};

			Quad faces[6];
		};

	private:
		RenderDevice const & m_device;
		SceneBackground const * m_background;
		VkExtent2D m_size;
		bool m_usesDepth;
		Viewport m_viewport;
		OnBackgroundChangedConnection m_onBackgroundChanged;
		ashes::VertexBufferPtr< Cube > m_vertexBuffer;
		ashes::BufferPtr< uint16_t > m_indexBuffer;
		crg::WriteDescriptorSetArray m_descriptorWrites;
		crg::VkDescriptorSetLayoutBindingArray m_descriptorBindings;
		ashes::DescriptorSetLayoutPtr m_descriptorSetLayout;
		ashes::PipelineLayoutPtr m_pipelineLayout;
		ashes::GraphicsPipelinePtr m_pipeline;
		ashes::DescriptorSetPoolPtr m_descriptorSetPool;
		ashes::DescriptorSetPtr m_descriptorSet;
	};

	class BackgroundRenderer
	{
	private:
		C3D_API BackgroundRenderer( crg::FrameGraph & graph
			, crg::FramePassArray previousPasses
			, RenderDevice const & device
			, ProgressBar * progress
			, castor::String const & name
			, SceneBackground & background
			, HdrConfigUbo const & hdrConfigUbo
			, SceneUbo const & sceneUbo
			, crg::ImageViewId const & colour
			, bool clearColour
			, crg::ImageViewId const * depth );

	public:
		BackgroundRenderer( crg::FrameGraph & graph
			, crg::FramePass const * previousPass
			, RenderDevice const & device
			, ProgressBar * progress
			, castor::String const & name
			, SceneBackground & background
			, HdrConfigUbo const & hdrConfigUbo
			, SceneUbo const & sceneUbo
			, crg::ImageViewId const & colour
			, bool clearColour = true )
			: BackgroundRenderer{ graph
				, ( previousPass
					? crg::FramePassArray{ previousPass }
					: crg::FramePassArray{} )
				, device
				, progress
				, name
				, background
				, hdrConfigUbo
				, sceneUbo
				, colour
				, clearColour
				, nullptr }
		{
		}

		BackgroundRenderer( crg::FrameGraph & graph
			, crg::FramePass const * previousPass
			, RenderDevice const & device
			, ProgressBar * progress
			, castor::String const & name
			, SceneBackground & background
			, HdrConfigUbo const & hdrConfigUbo
			, SceneUbo const & sceneUbo
			, crg::ImageViewId const & colour
			, bool clearColour
			, crg::ImageViewId const & depth )
			: BackgroundRenderer{ graph
				, ( previousPass
					? crg::FramePassArray{ previousPass }
					: crg::FramePassArray{} )
				, device
				, progress
				, name
				, background
				, hdrConfigUbo
				, sceneUbo
				, colour
				, clearColour
				, &depth }
		{
		}

		BackgroundRenderer( crg::FrameGraph & graph
			, crg::FramePass const * previousPass
			, RenderDevice const & device
			, castor::String const & name
			, SceneBackground & background
			, HdrConfigUbo const & hdrConfigUbo
			, SceneUbo const & sceneUbo
			, crg::ImageViewId const & colour
			, bool clearColour = true )
			: BackgroundRenderer{ graph
				, ( previousPass
					? crg::FramePassArray{ previousPass }
					: crg::FramePassArray{} )
				, device
				, nullptr
				, name
				, background
				, hdrConfigUbo
				, sceneUbo
				, colour
				, clearColour
				, nullptr }
		{
		}

		BackgroundRenderer( crg::FrameGraph & graph
			, crg::FramePass const * previousPass
			, RenderDevice const & device
			, castor::String const & name
			, SceneBackground & background
			, HdrConfigUbo const & hdrConfigUbo
			, SceneUbo const & sceneUbo
			, crg::ImageViewId const & colour
			, bool clearColour
			, crg::ImageViewId const & depth )
			: BackgroundRenderer{ graph
				, ( previousPass
					? crg::FramePassArray{ previousPass }
					: crg::FramePassArray{} )
				, device
				, nullptr
				, name
				, background
				, hdrConfigUbo
				, sceneUbo
				, colour
				, clearColour
				, &depth }
		{
		}

		BackgroundRenderer( crg::FrameGraph & graph
			, crg::FramePassArray previousPasses
			, RenderDevice const & device
			, ProgressBar * progress
			, castor::String const & name
			, SceneBackground & background
			, HdrConfigUbo const & hdrConfigUbo
			, SceneUbo const & sceneUbo
			, crg::ImageViewId const & colour
			, bool clearColour = true )
			: BackgroundRenderer{ graph
				, std::move( previousPasses )
				, device
				, progress
				, name
				, background
				, hdrConfigUbo
				, sceneUbo
				, colour
				, clearColour
				, nullptr }
		{
		}

		BackgroundRenderer( crg::FrameGraph & graph
			, crg::FramePassArray previousPasses
			, RenderDevice const & device
			, ProgressBar * progress
			, castor::String const & name
			, SceneBackground & background
			, HdrConfigUbo const & hdrConfigUbo
			, SceneUbo const & sceneUbo
			, crg::ImageViewId const & colour
			, bool clearColour
			, crg::ImageViewId const & depth )
			: BackgroundRenderer{ graph
				, std::move( previousPasses )
				, device
				, progress
				, name
				, background
				, hdrConfigUbo
				, sceneUbo
				, colour
				, clearColour
				, &depth }
		{
		}

		BackgroundRenderer( crg::FrameGraph & graph
			, crg::FramePassArray previousPasses
			, RenderDevice const & device
			, castor::String const & name
			, SceneBackground & background
			, HdrConfigUbo const & hdrConfigUbo
			, SceneUbo const & sceneUbo
			, crg::ImageViewId const & colour
			, bool clearColour = true )
			: BackgroundRenderer{ graph
				, std::move( previousPasses )
				, device
				, nullptr
				, name
				, background
				, hdrConfigUbo
				, sceneUbo
				, colour
				, clearColour
				, nullptr }
		{
		}

		BackgroundRenderer( crg::FrameGraph & graph
			, crg::FramePassArray previousPasses
			, RenderDevice const & device
			, castor::String const & name
			, SceneBackground & background
			, HdrConfigUbo const & hdrConfigUbo
			, SceneUbo const & sceneUbo
			, crg::ImageViewId const & colour
			, bool clearColour
			, crg::ImageViewId const & depth )
			: BackgroundRenderer{ graph
				, std::move( previousPasses )
				, device
				, nullptr
				, name
				, background
				, hdrConfigUbo
				, sceneUbo
				, colour
				, clearColour
				, &depth }
		{
		}

		C3D_API ~BackgroundRenderer();
		/**
		 *\~english
		 *\brief			Updates the render pass, CPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau CPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( CpuUpdater & updater );
		/**
		 *\~english
		 *\brief			Updates the render pass, GPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau GPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( GpuUpdater & updater );

		crg::FramePass const & getPass()const
		{
			return *m_backgroundPassDesc;
		}

	private:
		crg::FramePass const & doCreatePass( crg::FrameGraph & graph
			, crg::FramePassArray previousPasses
			, castor::String const & name
			, SceneBackground & background
			, HdrConfigUbo const & hdrConfigUbo
			, SceneUbo const & sceneUbo
			, crg::ImageViewId const & colour
			, bool clearColour
			, crg::ImageViewId const * depth
			, ProgressBar * progress );

	private:
		RenderDevice const & m_device;
		MatrixUbo m_matrixUbo;
		UniformBufferOffsetT< ModelUboConfiguration > m_modelUbo;
		crg::FramePass const * m_backgroundPassDesc{};
		BackgroundPass * m_backgroundPass{};
	};
}

#endif
