/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BackgroundPass_H___
#define ___C3D_BackgroundPass_H___

#include "PassesModule.hpp"
#include "Castor3D/Scene/Background/BackgroundModule.hpp"

#include <RenderGraph/RenderPass.hpp>

#include <ashespp/Pipeline/GraphicsPipeline.hpp>

namespace castor3d
{
	class BackgroundPass
		: public crg::RenderPass
	{
	public:
		C3D_API BackgroundPass( crg::FramePass const & pass
			, crg::GraphContext const & context
			, crg::RunnableGraph & graph
			, RenderDevice const & device
			, crg::ImageId const & colourImage
			, crg::ImageId const & depthImage
			, SceneBackground & background );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~BackgroundPass();
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
		void doSubInitialise()override;
		void doSubRecordInto( VkCommandBuffer commandBuffer )const override;

		void doInitialiseVertexBuffer();
		ashes::PipelineShaderStageCreateInfoArray doInitialiseShader();
		void doFillDescriptorBindings();
		void doCreateDescriptorSet();
		void doCreatePipeline();
		ashes::PipelineViewportStateCreateInfo doCreateViewportState();

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
		crg::ImageId const & m_colourImage;
		crg::ImageId const & m_depthImage;
		SceneBackground & m_background;
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
}

#endif
