/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderGrid_H___
#define ___C3D_RenderGrid_H___

#include "RenderQuad.hpp"
#include "Castor3D/Buffer/UniformBufferOffset.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"

#include <CastorUtils/Design/Named.hpp>

#include <ashespp/Buffer/VertexBuffer.hpp>
#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/Pipeline/GraphicsPipeline.hpp>
#include <ashespp/Pipeline/PipelineLayout.hpp>
#include <ashespp/Pipeline/PipelineVertexInputStateCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineViewportStateCreateInfo.hpp>

#include <type_traits>

namespace castor3d
{
	class RenderGrid
		: public castor::Named
	{
		template< typename ConfigT, typename BuilderT >
		friend class RenderGridBuilderT;

	protected:
		/**
		*\~english
		*\brief
		*	Constructor.
		*\param[in] device
		*	The RenderDevice.
		*\param[in] name
		*	The pass name.
		*\param[in] samplerFilter
		*	The sampler filter for the source texture.
		*\param[in] config
		*	The configuration.
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le RenderDevice.
		*\param[in] name
		*	Le nom de la passe.
		*\param[in] samplerFilter
		*	Le filtre d'échantillonnage pour la texture source.
		*\param[in] config
		*	La configuration.
		*/
		C3D_API RenderGrid( RenderDevice const & device
			, castor::String const & name
			, VkFilter samplerFilter
			, uint32_t gridSize
			, rq::Config config );

	public:
		C3D_API virtual ~RenderGrid();
		C3D_API explicit RenderGrid( RenderGrid && rhs )noexcept;
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
		*	Creates the rendering pipeline.
		*\param[in] program
		*	The shader program.
		*\param[in] renderPass
		*	The render pass to use.
		*\param[in] pushRanges
		*	The push constant ranges.
		*\param[in] dsState
		*	The depth stencil state to use.
		*\~french
		*\brief
		*	Crée le pipeline de rendu.
		*\param[in] program
		*	Le programme shader.
		*\param[in] renderPass
		*	La passe de rendu à utiliser.
		*\param[in] pushRanges
		*	Les intervalles de push constants.
		*\param[in] dsState
		*	L'état de profondeur et stencil à utiliser.
		*/
		C3D_API void createPipeline( ashes::PipelineShaderStageCreateInfoArray const & program
			, ashes::RenderPass const & renderPass
			, ashes::VkPushConstantRangeArray const & pushRanges = ashes::VkPushConstantRangeArray{}
			, ashes::PipelineDepthStencilStateCreateInfo dsState = ashes::PipelineDepthStencilStateCreateInfo{ 0u, VK_FALSE, VK_FALSE } );
		/**
		*\~english
		*\brief
		*	Creates the entries for one pass.
		*\param[in] writes
		*	The pass descriptor writes.
		*\~french
		*\brief
		*	Crée les entrées pour une passe.
		*\param[in] writes
		*	Les descriptor writes de la passe.
		*/
		C3D_API void registerPassInputs( ashes::WriteDescriptorSetArray const & writes );
		/**
		*\~english
		*\brief
		*	Initialises the descriptor sets for all registered passes.
		*\~french
		*\brief
		*	Crée les descriptor sets pour toute les passes enregistrées.
		*/
		C3D_API void initialisePasses();
		/**
		*\~english
		*\brief
		*	Creates the rendering pipeline and initialises the quad for one pass.
		*\param[in] program
		*	The shader program.
		*\param[in] writes
		*	The pass descriptor writes.
		*\param[in] renderPass
		*	The render pass to use.
		*\param[in] writes
		*	The pass' descriptor writes.
		*\param[in] pushRanges
		*	The push constant ranges.
		*\param[in] dsState
		*	The depth stencil state to use.
		*\~french
		*\brief
		*	Crée le pipeline de rendu et initialise le quad pour une passe.
		*\param[in] program
		*	Le programme shader.
		*\param[in] writes
		*	Les descriptor writes de la passe.
		*\param[in] renderPass
		*	La passe de rendu à utiliser.
		*\param[in] writes
		*	Les descriptor writes de la passe.
		*\param[in] pushRanges
		*	Les intervalles de push constants.
		*\param[in] dsState
		*	L'état de profondeur et stencil à utiliser.
		*/
		C3D_API void createPipelineAndPass( ashes::PipelineShaderStageCreateInfoArray const & program
			, ashes::RenderPass const & renderPass
			, ashes::WriteDescriptorSetArray const & writes
			, ashes::VkPushConstantRangeArray const & pushRanges = ashes::VkPushConstantRangeArray{}
			, ashes::PipelineDepthStencilStateCreateInfo dsState = ashes::PipelineDepthStencilStateCreateInfo{ 0u, false, false } );
		/**
		*\~english
		*\brief
		*	Prpares the commands to render the quad, inside given command buffer.
		*\param[in,out] commandBuffer
		*	The command buffer.
		*\param[in] descriptorSetIndex
		*	The render descriptor set index.
		*\~french
		*\brief
		*	Prépare les commandes de dessin du quad, dans le tampon de commandes donné.
		*\param[in,out] commandBuffer
		*	Le tampon de commandes.
		*\param[in] descriptorSetIndex
		*	L'indice du descriptor set.
		*/
		C3D_API void registerPass( ashes::CommandBuffer & commandBuffer
			, uint32_t descriptorSetIndex )const;
		/**
		*\~english
		*\brief
		*	Prpares the commands to render the quad, inside given command buffer.
		*\param[in,out] commandBuffer
		*	The command buffer.
		*\param[in] descriptorSetIndex
		*	The render descriptor set index.
		*\~french
		*\brief
		*	Prépare les commandes de dessin du quad, dans le tampon de commandes donné.
		*\param[in,out] commandBuffer
		*	Le tampon de commandes.
		*\param[in] descriptorSetIndex
		*	L'indice du descriptor set.
		*/
		inline void registerPass( ashes::CommandBuffer & commandBuffer )const
		{
			registerPass( commandBuffer, 0u );
		}
		/**
		*\~english
		*\brief
		*	Creates a descriptor write for combined image sampler.
		*\param[in] view
		*	The image view.
		*\param[in] sampler
		*	The sampler.
		*\param[in] dstBinding
		*	The binding inside the descriptor set.
		*\param[in] dstArrayElement
		*	The array element index.
		*\~french
		*\brief
		*	Crée un descriptor write pour un sampler et une image combinés.
		*\param[in] view
		*	La vue sur l'image.
		*\param[in] sampler
		*	Le sampler.
		*\param[in] dstBinding
		*	Le binding dans le descriptor set.
		*\param[in] dstArrayElement
		*	L'indice dans le tableau d'éléments.
		*/
		static ashes::WriteDescriptorSet makeDescriptorWrite( ashes::ImageView const & view
			, ashes::Sampler const & sampler
			, uint32_t dstBinding
			, uint32_t dstArrayElement = 0u )
		{
			return RenderQuad::makeDescriptorWrite( view
				, sampler
				, dstBinding
				, dstArrayElement );
		}
		/**
		*\~english
		*\brief
		*	Creates a descriptor write for uniform buffer.
		*\param[in] buffer
		*	The uniform buffer.
		*\param[in] elemOffset
		*	The offset, expressed in element count.
		*\param[in] elemRange
		*	The range, expressed in element count.
		*\param[in] dstBinding
		*	The binding inside the descriptor set.
		*\param[in] dstArrayElement
		*	The array element index.
		*\~french
		*\brief
		*	Crée un descriptor write pour un uniform buffer.
		*\param[in] buffer
		*	L'uniform buffer.
		*\param[in] elemOffset
		*	L'offset, exprimé en nombre d'éléments.
		*\param[in] elemRange
		*	L'intervalle, exprimé en nombre d'éléments.
		*\param[in] dstBinding
		*	Le binding dans le descriptor set.
		*\param[in] dstArrayElement
		*	L'indice dans le tableau d'éléments.
		*/
		static ashes::WriteDescriptorSet makeDescriptorWrite( ashes::UniformBuffer const & buffer
			, uint32_t dstBinding
			, uint32_t elemOffset
			, uint32_t elemRange
			, uint32_t dstArrayElement = 0u )
		{
			return RenderQuad::makeDescriptorWrite( buffer
				, dstBinding
				, elemOffset
				, elemRange
				, dstArrayElement );
		}
		/**
		*\~english
		*\brief
		*	Creates a descriptor write for uniform buffer range.
		*\param[in] buffer
		*	The uniform buffer range.
		*\param[in] dstBinding
		*	The binding inside the descriptor set.
		*\param[in] dstArrayElement
		*	The array element index.
		*\~french
		*\brief
		*	Crée un descriptor write pour un intervalle d'uniform buffer.
		*\param[in] buffer
		*	L'intervalle d'uniform buffer.
		*\param[in] dstBinding
		*	Le binding dans le descriptor set.
		*\param[in] dstArrayElement
		*	L'indice dans le tableau d'éléments.
		*/
		template< typename DataT >
		static ashes::WriteDescriptorSet makeDescriptorWrite( UniformBufferOffsetT< DataT > const & buffer
			, uint32_t dstBinding
			, uint32_t dstArrayElement = 0u )
		{
			return RenderQuad::makeDescriptorWrite( buffer
				, dstBinding
				, dstArrayElement );
		}
		/**
		*\~english
		*\brief
		*	Creates a descriptor write for storage buffer.
		*\param[in] storageBuffer
		*	The storage buffer.
		*\param[in] elemOffset
		*	The offset, expressed in element count.
		*\param[in] elemRange
		*	The range, expressed in element count.
		*\param[in] dstBinding
		*	The binding inside the descriptor set.
		*\param[in] dstArrayElement
		*	The array element index.
		*\~french
		*\brief
		*	Crée un descriptor write pour un storage buffer.
		*\param[in] storageBuffer
		*	Le storage buffer.
		*\param[in] elemOffset
		*	L'offset, exprimé en nombre d'éléments.
		*\param[in] elemRange
		*	L'intervalle, exprimé en nombre d'éléments.
		*\param[in] dstBinding
		*	Le binding dans le descriptor set.
		*\param[in] dstArrayElement
		*	L'indice dans le tableau d'éléments.
		*/
		C3D_API static ashes::WriteDescriptorSet makeDescriptorWrite( ashes::BufferBase const & storageBuffer
			, uint32_t dstBinding
			, uint32_t byteOffset
			, uint32_t byteRange
			, uint32_t dstArrayElement = 0u )
		{
			return RenderQuad::makeDescriptorWrite( storageBuffer
				, dstBinding
				, byteOffset
				, byteRange
				, dstArrayElement );
		}
		/**
		*\~english
		*\brief
		*	Creates a descriptor write for storage buffer.
		*\param[in] storageBuffer
		*	The storage buffer.
		*\param[in] elemOffset
		*	The offset, expressed in element count.
		*\param[in] elemRange
		*	The range, expressed in element count.
		*\param[in] dstBinding
		*	The binding inside the descriptor set.
		*\param[in] dstArrayElement
		*	The array element index.
		*\~french
		*\brief
		*	Crée un descriptor write pour un storage buffer.
		*\param[in] storageBuffer
		*	Le storage buffer.
		*\param[in] elemOffset
		*	L'offset, exprimé en nombre d'éléments.
		*\param[in] elemRange
		*	L'intervalle, exprimé en nombre d'éléments.
		*\param[in] dstBinding
		*	Le binding dans le descriptor set.
		*\param[in] dstArrayElement
		*	L'indice dans le tableau d'éléments.
		*/
		template< typename DataT >
		static ashes::WriteDescriptorSet makeDescriptorWrite( ashes::Buffer< DataT > const & storageBuffer
			, uint32_t dstBinding
			, uint32_t elemOffset
			, uint32_t elemRange
			, uint32_t dstArrayElement = 0u )
		{
			return RenderQuad::makeDescriptorWrite( storageBuffer
				, dstBinding
				, elemOffset
				, elemRange
				, dstArrayElement );
		}
		/**
		*\~english
		*\brief
		*	Creates a descriptor write for buffer texel view.
		*\param[in] buffer
		*	The buffer.
		*\param[in] view
		*	The texel view.
		*\param[in] dstBinding
		*	The binding inside the descriptor set.
		*\param[in] dstArrayElement
		*	The array element index.
		*\~french
		*\brief
		*	Crée un descriptor write pour une texel view sur un buffer.
		*\param[in] buffer
		*	Le buffer.
		*\param[in] view
		*	La texel view.
		*\param[in] dstBinding
		*	Le binding dans le descriptor set.
		*\param[in] dstArrayElement
		*	L'indice dans le tableau d'éléments.
		*/
		C3D_API static ashes::WriteDescriptorSet makeDescriptorWrite( ashes::BufferBase const & buffer
			, ashes::BufferView const & view
			, uint32_t dstBinding
			, uint32_t dstArrayElement = 0u )
		{
			return RenderQuad::makeDescriptorWrite( buffer
				, view
				, dstBinding
				, dstArrayElement );
		}
		/**
		*\~english
		*\brief
		*	Creates a descriptor write for buffer texel view.
		*\param[in] buffer
		*	The buffer.
		*\param[in] view
		*	The texel view.
		*\param[in] dstBinding
		*	The binding inside the descriptor set.
		*\param[in] dstArrayElement
		*	The array element index.
		*\~french
		*\brief
		*	Crée un descriptor write pour une texel view sur un buffer.
		*\param[in] buffer
		*	Le buffer.
		*\param[in] view
		*	La texel view.
		*\param[in] dstBinding
		*	Le binding dans le descriptor set.
		*\param[in] dstArrayElement
		*	L'indice dans le tableau d'éléments.
		*/
		template< typename DataT >
		static ashes::WriteDescriptorSet makeDescriptorWrite( ashes::Buffer< DataT > const & buffer
			, ashes::BufferView const & view
			, uint32_t dstBinding
			, uint32_t dstArrayElement = 0u )
		{
			return RenderQuad::makeDescriptorWrite( buffer
				, view
				, dstBinding
				, dstArrayElement );
		}

		inline RenderSystem * getRenderSystem()const
		{
			return &m_renderSystem;
		}

		inline RenderDevice const & getDevice()const
		{
			return m_device;
		}

		inline Sampler const & getSampler()const
		{
			return *m_sampler;
		}

		inline uint32_t getGridSize()const
		{
			return m_gridSize;
		}
		/**@}*/

	private:
		C3D_API virtual void doRegisterPass( ashes::CommandBuffer & commandBuffer )const;

	protected:
		RenderSystem & m_renderSystem;
		RenderDevice const & m_device;
		SamplerSPtr m_sampler;
		rq::ConfigData m_config;

	private:
		bool m_useTexCoord{ true };
		uint32_t m_gridSize{ 0u };
		uint32_t m_count{ 0u };
		ashes::DescriptorSetLayoutPtr m_descriptorSetLayout;
		ashes::PipelineLayoutPtr m_pipelineLayout;
		ashes::GraphicsPipelinePtr m_pipeline;
		ashes::DescriptorSetPoolPtr m_descriptorSetPool;
		std::vector< ashes::WriteDescriptorSetArray > m_passes;
		std::vector< ashes::DescriptorSetPtr > m_descriptorSets;
		ashes::VertexBufferPtr< castor::Point3f > m_vertexBuffer;
	};

	template< typename ConfigT, typename BuilderT >
	class RenderGridBuilderT
		: public RenderQuadBuilderT< ConfigT, BuilderT >
	{
	public:
		inline RenderGridBuilderT()
		{
		}
		/**
		*\~english
		*	Creates the RenderGrid.
		*\param[in] device
		*	The RenderDevice.
		*\param[in] name
		*	The pass name.
		*\param[in] samplerFilter
		*	The sampler filter for the source texture.
		*\~french
		*	Crée le RenderGrid.
		*\param[in] device
		*	Le RenderDevice.
		*\param[in] name
		*	Le nom de la passe.
		*\param[in] samplerFilter
		*	Le filtre d'échantillonnage pour la texture source.
		*/
		inline RenderGridUPtr build( RenderDevice const & device
			, castor::String const & name
			, VkFilter samplerFilter
			, uint32_t gridSize )
		{
			return castor::UniquePtr< RenderGrid >( new RenderGrid{ device
				, name
				, samplerFilter
				, gridSize
				, this->m_config } );
		}
	};

	class RenderGridBuilder
		: public RenderGridBuilderT< rq::Config, RenderGridBuilder >
	{
	};
}

#endif
