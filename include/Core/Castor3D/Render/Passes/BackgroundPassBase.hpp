/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BackgroundPassBase_H___
#define ___C3D_BackgroundPassBase_H___

#include "PassesModule.hpp"
#include "Castor3D/Scene/Background/BackgroundModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include "Castor3D/Render/Viewport.hpp"

#include <RenderGraph/RunnablePasses/RenderPass.hpp>

#include <ashespp/Pipeline/GraphicsPipeline.hpp>

namespace castor3d
{
	class BackgroundPassBase
		: public crg::RenderPass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	pass		The parent frame pass.
		 *\param[in]	context		The rendering context.
		 *\param[in]	graph		The runnable graph.
		 *\param[in]	device		The GPU device.
		 *\param[in]	background	The scene background.
		 *\param[in]	size		The render area dimensions.
		 *\param[in]	usesDepth	\p true to account for depth buffer.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	pass		La frame pass parente.
		 *\param[in]	context		Le contexte de rendu.
		 *\param[in]	graph		Le runnable graph.
		 *\param[in]	device		Le device GPU.
		 *\param[in]	background	Le fond de la scène.
		 *\param[in]	size		Les dimensions de la zone de rendu.
		 *\param[in]	usesDepth	\p true pour prendre en compte le depth buffer.
		 */
		C3D_API BackgroundPassBase( crg::FramePass const & pass
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

	protected:
		C3D_API bool doIsEnabled()const;
		C3D_API ashes::PipelineViewportStateCreateInfo doCreateViewportState();

	private:
		void doSubInitialise();
		void doSubRecordInto( crg::RecordContext & context
			, VkCommandBuffer commandBuffer
			, uint32_t index );
		void doCreateDescriptorSet();
		void doResetPipeline();

		virtual void doInitialiseVertexBuffer() = 0;
		virtual ashes::PipelineShaderStageCreateInfoArray doInitialiseShader() = 0;
		virtual void doFillDescriptorBindings() = 0;
		virtual void doCreatePipeline() = 0;

	protected:
		struct Vertex
		{
			castor::Point3f position;
		};

		RenderDevice const & m_device;
		SceneBackground const * m_background;
		VkExtent2D m_size;
		bool m_usesDepth;
		Viewport m_viewport;
		OnBackgroundChangedConnection m_onBackgroundChanged;
		ashes::VertexBufferPtr< Vertex > m_vertexBuffer;
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
