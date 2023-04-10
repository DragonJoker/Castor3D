/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderCube_H___
#define ___C3D_RenderCube_H___
#pragma once

#include "RenderToTextureModule.hpp"

#include "Castor3D/Buffer/UniformBuffer.hpp"

#include <ashespp/Buffer/VertexBuffer.hpp>
#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/Pipeline/GraphicsPipeline.hpp>
#include <ashespp/Pipeline/PipelineLayout.hpp>

#include <array>

namespace castor3d
{
	class RenderCube
	{
	public:
		C3D_API virtual ~RenderCube();
		/**
		*\~english
		*\brief
		*	Constructor.
		*\param[in] device
		*	The GPU device.
		*\param[in] nearest
		*	Tells if the sampler needs a Nearest filter or not, if \p sampler is nullptr.
		*\param[in] sampler
		*	The sampler to use.
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le device GPU.
		*\param[in] nearest
		*	Dit si le sampler doit filtrer en Nearest ou pas, si \p sampler est nullptr.
		*\param[in] sampler
		*	Le sampler à utiliser.
		*/
		C3D_API explicit RenderCube( RenderDevice const & device
			, bool nearest
			, SamplerObs sampler = {} );
		/**
		*\~english
		*\brief
		*	Creates the rendering pipelines.
		*\param[in] size
		*	The render size.
		*\param[in] position
		*	The render position.
		*\param[in] program
		*	The shader program.
		*\param[in] view
		*	The source view.
		*\param[in] renderPass
		*	The render pass to use.
		*\param[in] pushRanges
		*	The push constant ranges.
		*\~french
		*\brief
		*	Crée les pipelines de rendu.
		*\param[in] size
		*	Les dimensions de rendu.
		*\param[in] position
		*	La position du rendu.
		*\param[in] program
		*	Le programme shader.
		*\param[in] view
		*	La vue cible.
		*\param[in] renderPass
		*	La passe de rendu à utiliser.
		*\param[in] pushRanges
		*	Les intervalles de push constants.
		*/
		C3D_API void createPipelines( VkExtent2D const & size
			, castor::Position const & position
			, ashes::PipelineShaderStageCreateInfoArray const & program
			, ashes::ImageView const & view
			, ashes::RenderPass const & renderPass
			, ashes::VkPushConstantRangeArray const & pushRanges );
		/**
		*\~english
		*\brief
		*	Creates the rendering pipelines.
		*\param[in] size
		*	The render size.
		*\param[in] position
		*	The render position.
		*\param[in] program
		*	The shader program.
		*\param[in] view
		*	The source view.
		*\param[in] renderPass
		*	The render pass to use.
		*\param[in] pushRanges
		*	The push constant ranges.
		*\param[in] dsState
		*	The depth stencil state to use.
		*\~french
		*\brief
		*	Crée les pipelines de rendu.
		*\param[in] size
		*	Les dimensions de rendu.
		*\param[in] position
		*	La position du rendu.
		*\param[in] program
		*	Le programme shader.
		*\param[in] view
		*	La vue cible.
		*\param[in] renderPass
		*	La passe de rendu à utiliser.
		*\param[in] pushRanges
		*	Les intervalles de push constants.
		*\param[in] dsState
		*	L'état de profondeur et stencil à utiliser.
		*/
		C3D_API void createPipelines( VkExtent2D const & size
			, castor::Position const & position
			, ashes::PipelineShaderStageCreateInfoArray const & program
			, ashes::ImageView const & view
			, ashes::RenderPass const & renderPass
			, ashes::VkPushConstantRangeArray const & pushRanges
			, ashes::PipelineDepthStencilStateCreateInfo const & dsState );
		/**
		*\~english
		*\brief
		*	Cleans up GPU objects.
		*\~french
		*\brief
		*	Nettoie les objets GPU.
		*/
		C3D_API void cleanup();
		/**
		*\~english
		*\brief
		*	Prepares the commands to render one face from the cube.
		*\param[in] renderPass
		*	The render pass to use.
		*\param[in] subpassIndex
		*	The render subpass index.
		*\param[in] face
		*	The face index.
		*\~french
		*\brief
		*	Prépare les commandes de dessin d'une face du cube.
		*\param[in] renderPass
		*	La passe de rendu à utiliser.
		*\param[in] subpassIndex
		*	L'indice de la sous passe de rendu.
		*\param[in] face
		*	L'indice de la face.
		*/
		C3D_API void prepareFrame( ashes::RenderPass const & renderPass
			, uint32_t subpassIndex
			, uint32_t face );
		/**
		*\~english
		*\brief
		*	Prepares the commands to render one face from the cube, inside given command buffer.
		*\param[in,out] commandBuffer
		*	The command buffer.
		*\param[in] face
		*	The face index.
		*\~french
		*\brief
		*	Prépare les commandes de dessin d'une face du cube, dans le tampon de commandes donné.
		*\param[in,out] commandBuffer
		*	Le tampon de commandes.
		*\param[in] face
		*	L'indice de la face.
		*/
		C3D_API void registerFrame( ashes::CommandBuffer & commandBuffer
			, uint32_t face )const;

	private:
		C3D_API virtual void doFillDescriptorLayoutBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings );
		C3D_API virtual void doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
			, ashes::DescriptorSet & descriptorSet
			, uint32_t face );
		C3D_API virtual void doRegisterFrame( ashes::CommandBuffer & commandBuffer
			, uint32_t face )const;

	protected:
		RenderDevice const & m_device;
		SamplerObs m_sampler{};
		ashes::PipelineLayoutPtr m_pipelineLayout;

	private:
		struct FacePipeline
		{
			ashes::GraphicsPipelinePtr pipeline;
			ashes::DescriptorSetPtr descriptorSet;
		};

		ashes::CommandBufferPtr m_commandBuffer;
		UniformBufferUPtrT< castor::Matrix4x4f > m_matrixUbo;
		ashes::VertexBufferPtr< castor::Point4f > m_vertexBuffer;
		ashes::PipelineVertexInputStateCreateInfoPtr m_vertexLayout;
		ashes::DescriptorSetLayoutPtr m_descriptorLayout;
		ashes::DescriptorSetPoolPtr m_descriptorPool;
		std::array< FacePipeline, 6u > m_faces;
	};
}

#endif
