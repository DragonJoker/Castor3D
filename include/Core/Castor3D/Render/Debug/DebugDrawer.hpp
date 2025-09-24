/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DebugDrawer_H___
#define ___C3D_DebugDrawer_H___

#include "Castor3D/Render/Debug/DebugModule.hpp"

#include "Castor3D/Buffer/ObjectBufferOffset.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"

#include <CastorUtils/Design/OwnedBy.hpp>

#include <RenderGraph/RunnablePasses/RenderPass.hpp>

namespace c3d
{
	class DebugDrawer
		: public OwnedBy< RenderTarget >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	graph		The graph.
		 *\param[in]	previous	The previous pass.
		 *\param[in]	device		The GPU device.
		 *\param[in]	parent		The parent render target.
		 *\param[in]	colour		The target colour image.
		 *\param[in]	depth		The target depth image.
		 *\param[in]	passIndex	The pass index, to select the appropriate shader.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	graph		Le graphe.
		 *\param[in]	previous	La passe précédente.
		 *\param[in]	device		Le device GPU.
		 *\param[in]	parent		La render target parente.
		 *\param[in]	colour		L'image couleur cible.
		 *\param[in]	depth		L'image profondeur cible.
		 *\param[in]	passIndex	L'index de la passe, pour sélectionner le shader approprié.
		 */
		C3D_API DebugDrawer( crg::FramePassGroup & graph
			, RenderDevice const & device
			, RenderTarget & parent
			, Texture & colour
			, Texture const & depth
			, uint32_t const * passIndex );
		/**
		 *\~english
		 *\brief		Adds a buffer containing AABBs to draw.
		 *\param[in]	descriptorBindings	The shader data bindings.
		 *\param[in]	descriptorWrites	The shader data.
		 *\param[in]	instanceCount		The number of instances to draw.
		 *\param[in]	shader				The shader used to draw the AABB.
		 *\param[in]	enableDepthTest		\p true to enable depth test.
		 *\~french
		 *\brief		Ajoute un buffer d'AABB à dessiner.
		 *\param[in]	descriptorBindings	Les bindings des données à passer au shader.
		 *\param[in]	descriptorWrites	Les données à passer au shader.
		 *\param[in]	instanceCount		Le nombre d'instances à dessiner.
		 *\param[in]	shader				Le shader utilisé pour dessiner les AABB.
		 *\param[in]	enableDepthTest		\p true pour activer le depth test.
		 */
		C3D_API void addAabbs( ashes::VkDescriptorSetLayoutBindingArray const & descriptorBindings
			, ashes::WriteDescriptorSetArray const & descriptorWrites
			, VkDeviceSize instanceCount
			, ashes::PipelineShaderStageCreateInfoArray const & shader
			, bool enableDepthTest );
		/**
		 *\~english
		 *\brief		Adds a buffer containing AABBs to draw.
		 *\param[in]	vertexBuffers		The geometry vertex buffers.
		 *\param[in]	indexBuffer			The geometry index buffers.
		 *\param[in]	vertexAttributes	The geometry vertex attributes.
		 *\param[in]	vertexBindings		The geometry vertex bindings.
		 *\param[in]	descriptorBindings	The shader data bindings.
		 *\param[in]	descriptorWrites	The shader data.
		 *\param[in]	instanceCount		The number of instances to draw.
		 *\param[in]	shader				The shader used to draw the geometries.
		 *\param[in]	enableDepthTest		\p true to enable depth test.
		 *\~french
		 *\brief		Ajoute un buffer d'AABB à dessiner.
		 *\param[in]	vertexBuffers		Les vertex buffers de la géométrie.
		 *\param[in]	indexBuffer			L'index buffer de la géométrie.
		 *\param[in]	vertexAttributes	Les attributs de sommet de la géométrie.
		 *\param[in]	vertexBindings		Les attaches de sommet de la géométrie.
		 *\param[in]	descriptorBindings	Les bindings des données à passer au shader.
		 *\param[in]	descriptorWrites	Les données à passer au shader.
		 *\param[in]	instanceCount		Le nombre d'instances à dessiner.
		 *\param[in]	shader				Le shader utilisé pour dessiner les géometries.
		 *\param[in]	enableDepthTest		\p true pour activer le depth test.
		 */
		C3D_API void addDrawable( DebugVertexBuffers const & vertexBuffers
			, DebugIndexBuffer const & indexBuffer
			, ashes::VkVertexInputAttributeDescriptionArray const & vertexAttributes
			, ashes::VkVertexInputBindingDescriptionArray const & vertexBindings
			, ashes::VkDescriptorSetLayoutBindingArray const & descriptorBindings
			, ashes::WriteDescriptorSetArray const & descriptorWrites
			, VkDeviceSize instanceCount
			, ashes::PipelineShaderStageCreateInfoArray const & shader
			, bool enableDepthTest );

	private:
		struct Object
		{
			ObjectBufferOffset vertices;
			ObjectBufferOffset indices;
		};

		struct Pipeline
		{
			ashes::DescriptorSetLayoutPtr descriptorLayout;
			ashes::PipelineLayoutPtr pipelineLayout;
			ashes::GraphicsPipelinePtr pipeline;

			struct Instance
			{
				ashes::DescriptorSetPoolPtr descriptorPool;
				ashes::DescriptorSetPtr descriptorSet;
			};

			HashMap< size_t, Instance > instances;
		};

		struct AABBBuffer
		{
			AABBBuffer( DebugVertexBuffers vertexBuffers
				, DebugIndexBuffer indexBuffer
				, VkDeviceSize instanceCount
				, Pipeline * pipeline
				, ashes::DescriptorSet * descriptorSet
				, ashes::WriteDescriptorSetArray writes );

			DebugVertexBuffers vertexBuffers;
			DebugIndexBuffer indexBuffer;
			VkDeviceSize instanceCount;
			Pipeline * pipeline;
			ashes::DescriptorSet * descriptorSet;
			ashes::WriteDescriptorSetArray writes;
		};

		using PipelinePtr = RawUniquePtr< Pipeline >;

		class FramePass
			: public crg::RenderPass
		{
		public:
			FramePass( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph
				, RenderDevice const & device
				, Extent2D dimensions
				, uint32_t const * passIndex );
			~FramePass()noexcept override;

			void addDrawable( DebugVertexBuffers vertexBuffers
				, DebugIndexBuffer indexBuffer
				, ashes::VkVertexInputAttributeDescriptionArray const & vertexAttributes
				, ashes::VkVertexInputBindingDescriptionArray const & vertexBindings
				, ashes::VkDescriptorSetLayoutBindingArray const & bindings
				, ashes::WriteDescriptorSetArray const & writes
				, VkDeviceSize instanceCount
				, ashes::PipelineShaderStageCreateInfoArray const & shader
				, bool enableDepthTest );
			void addAabbs( ashes::VkDescriptorSetLayoutBindingArray const & bindings
				, ashes::WriteDescriptorSetArray const & writes
				, VkDeviceSize instanceCount
				, ashes::PipelineShaderStageCreateInfoArray const & shader
				, bool enableDepthTest );

		private:
			void doSubRecordInto( crg::RecordContext const & context
				, VkCommandBuffer commandBuffer );
			bool doIsEnabled()const noexcept
			{
				return m_pending || !m_aabbs.empty();
			}

		private:
			RenderDevice const & m_device;
			HashMap< size_t, PipelinePtr > m_pipelines;
			Vector< AABBBuffer > m_aabbs;
			Object m_aabb;
			bool m_pending{};
		};

	private:
		FramePass * m_framePass{};
	};
}

#endif
