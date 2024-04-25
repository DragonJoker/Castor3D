/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DebugDrawer_H___
#define ___C3D_DebugDrawer_H___

#include "Castor3D/Buffer/ObjectBufferOffset.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"

#include <CastorUtils/Design/OwnedBy.hpp>

#include <RenderGraph/RunnablePasses/RenderPass.hpp>

namespace castor3d
{
	class DebugDrawer
		: public castor::OwnedBy< RenderTarget >
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
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	graph		Le graphe.
		 *\param[in]	previous	La passe précédente.
		 *\param[in]	device		Le device GPU.
		 *\param[in]	parent		La render target parente.
		 *\param[in]	colour		L'image couleur cible.
		 *\param[in]	depth		L'image profondeur cible.
		 */
		C3D_API DebugDrawer( crg::FramePassGroup & graph
			, crg::FramePass const * previous
			, RenderDevice const & device
			, RenderTarget & parent
			, crg::ImageViewIdArray colour
			, Texture const & depth
			, uint32_t const * passIndex );
		/**
		 *\~english
		 *\brief		Adds a buffer containing AABBs to draw.
		 *\param[in]	bindings	The shader data bindings.
		 *\param[in]	writes		The shader data.
		 *\param[in]	count		The number of AABB to draw.
		 *\param[in]	shader		The shader used to draw the AABB.
		 *\~french
		 *\brief		Ajoute un buffer d'AABB à dessiner.
		 *\param[in]	bindings	Les bindings des données à passer au shader.
		 *\param[in]	writes		Les données à passer au shader.
		 *\param[in]	count		Le nombre d'AABB à dessiner.
		 *\param[in]	shader		Le shader utilisé pour dessiner les AABB.
		 */
		C3D_API void addAabbs( ashes::VkDescriptorSetLayoutBindingArray const & bindings
			, ashes::WriteDescriptorSetArray const & writes
			, VkDeviceSize count
			, ashes::PipelineShaderStageCreateInfoArray const & shader
			, bool enableDepthTest );

		crg::FramePass const & getLastPass()const noexcept
		{
			return *m_lastPass;
		}

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

			castor::UnorderedMap< size_t, Instance > instances;
		};

		struct AABBBuffer
		{
			AABBBuffer( VkDeviceSize count
				, Pipeline * pipeline
				, ashes::DescriptorSet * descriptorSet
				, ashes::WriteDescriptorSetArray writes );

			VkDeviceSize count;
			Pipeline * pipeline;
			ashes::DescriptorSet * descriptorSet;
			ashes::WriteDescriptorSetArray writes;
		};

		using PipelinePtr = castor::RawUniquePtr< Pipeline >;

		class FramePass
			: public crg::RenderPass
		{
		public:
			FramePass( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph
				, RenderDevice const & device
				, VkExtent2D dimensions
				, uint32_t const * passIndex );
			~FramePass()noexcept override;

			void addAabbs( ashes::VkDescriptorSetLayoutBindingArray const & bindings
			, ashes::WriteDescriptorSetArray const & writes
			, VkDeviceSize count
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
			castor::UnorderedMap< size_t, PipelinePtr > m_pipelines;
			castor::Vector< AABBBuffer > m_aabbs;
			Object m_aabb;
			bool m_pending{};
		};

	private:
		RenderDevice const & m_device;
		crg::FramePass const * m_lastPass{};
		FramePass * m_framePass{};
	};
}

#endif
