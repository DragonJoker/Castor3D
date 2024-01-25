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
		 *\param[in]	resources	The graph resources handler.
		 *\param[in]	device		The GPU device.
		 *\param[in]	queueData	The queue receiving the GPU commands.
		 *\param[in]	size		The render area dimensions.
		 *\param[in]	cameraUbo	The scene matrices UBO.
		 *\param[in]	sceneUbo	The scene UBO.
		 *\param[in]	culler		The culler for this pass.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	resources	Le gestionnaire de ressources du graphe.
		 *\param[in]	device		Le device GPU.
		 *\param[in]	queueData	La queue recevant les commandes GPU.
		 *\param[in]	size		Les dimensions de la zone de rendu
		 *\param[in]	cameraUbo	L'UBO de matrices de la scène.
		 *\param[in]	sceneUbo	L'UBO de scène.
		 *\param[in]	culler		Le culler pour cette passe.
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
		 *\param[in]	toWait	The semaphores to wait.
		 *\return		The semaphores signaled by this render.
		 *\~french
		 *\brief		Dessine les objets ajoutés.
		 *\param[in]	queue	The queue recevant les commandes de dessin.
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
