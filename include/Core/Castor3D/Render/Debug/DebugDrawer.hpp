/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DebugDrawer_H___
#define ___C3D_DebugDrawer_H___

#include "Castor3D/Buffer/ObjectBufferOffset.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"

#include <CastorUtils/Design/OwnedBy.hpp>

namespace castor3d
{
	class DebugDrawer
		: public castor::OwnedBy< RenderTarget >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	parent	The parent render target.
		 *\param[in]	device	The GPU device.
		 *\param[in]	colour	The target colour image.
		 *\param[in]	depth	The target depth image.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	parent	La render target parente.
		 *\param[in]	device	Le device GPU.
		 *\param[in]	colour	L'image couleur cible.
		 *\param[in]	depth	L'image profondeur cible.
		 */
		C3D_API DebugDrawer( RenderTarget & parent
			, RenderDevice const & device
			, Texture const & colour
			, Texture const & depth );
		/**
		 *\~english
		 *\brief		Adds a buffer containing AABBs to draw.
		 *\param[in]	buffer	The GPU buffer.
		 *\param[in]	offset	The binary offset.
		 *\param[in]	size	The binary size.
		 *\param[in]	count	The AABB count.
		 *\param[in]	shader	The shader stages.
		 *\~french
		 *\brief		Ajoute un buffer d'AABB à dessiner.
		 *\param[in]	buffer	Le buffer GPU.
		 *\param[in]	offset	L'offset binaire.
		 *\param[in]	size	La taille binaire.
		 *\param[in]	count	Le nombre d'AABB.
		 *\param[in]	shader	Les shaders.
		 */
		C3D_API void addAabbs( VkBuffer buffer
			, VkDeviceSize offset
			, VkDeviceSize size
			, VkDeviceSize count
			, ashes::PipelineShaderStageCreateInfoArray shader );
		/**
		 *\~english
		 *\brief		Renders added objects.
		 *\param[in]	queue	The queue receiving the render commands.
		 *\param[in]	toWait	The semaphores to wait.
		 *\return		The semaphores signaled by this render.
		 *\~french
		 *\brief		Dessine les objets ajoutés.
		 *\param[in]	queue	La file recevant les commandes de dessin.
		 *\param[in]	toWait	Les sémaphores à attendre.
		 *\return		Les sémaphores signalés par ce dessin.
		 */
		C3D_API crg::SemaphoreWaitArray render( ashes::Queue const & queue
			, crg::SemaphoreWaitArray toWait = {} );

	private:
		struct Pipeline
		{
			ashes::DescriptorSetLayoutPtr descriptorLayout;
			ashes::PipelineLayoutPtr pipelineLayout;
			ashes::GraphicsPipelinePtr pipeline;
			ashes::DescriptorSetPoolPtr descriptorPool;
			ashes::DescriptorSetPtr descriptorSet;
			ObjectBufferOffset vertices;
			ObjectBufferOffset indices;
		};

		struct AABBBuffer
		{
			AABBBuffer( VkBuffer pbuffer
				, VkDeviceSize poffset
				, VkDeviceSize psize
				, VkDeviceSize pcount
				, Pipeline * ppipeline )
				: buffer{ pbuffer }
				, offset{ poffset }
				, size{ psize }
				, count{ pcount }
				, pipeline{ ppipeline }

			{
			}

			VkBuffer buffer;
			VkDeviceSize offset;
			VkDeviceSize size;
			VkDeviceSize count;
			Pipeline * pipeline;
		};

		using PipelinePtr = castor::RawUniquePtr< Pipeline >;

	private:
		RenderDevice const & m_device;
		castor::UnorderedMap< size_t, PipelinePtr > m_pipelines;
		ashes::RenderPassPtr m_renderPass;
		ashes::FrameBufferPtr m_framebuffer;
		ashes::CommandPoolPtr m_commandPool;
		castor::Array< CommandsSemaphore, 2u > m_commandBuffers;
		castor::Vector< AABBBuffer > m_aabbs;
		uint32_t m_index{};
	};
}

#endif
