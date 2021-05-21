/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderPipeline_H___
#define ___C3D_RenderPipeline_H___

#include "RenderModule.hpp"
#include "Castor3D/Render/Node/RenderNodeModule.hpp"

#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/Pipeline/PipelineColorBlendStateCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineDepthStencilStateCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineMultisampleStateCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineRasterizationStateCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineVertexInputStateCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineViewportStateCreateInfo.hpp>

#include <unordered_map>

namespace castor3d
{
	class RenderPipeline
		: public castor::OwnedBy< SceneRenderPass >
	{
	public:
		enum Descriptor
		{
			eBuffers,
			eTextures,
			eAdditional,
			CU_EnumBounds( Descriptor, eBuffers )
		};

	public:
		/**
		*\~english
		*\brief
		*	Constructor.
		*\param[in] owner
		*	The owning render pass.
		*\param[in] renderSystem
		*	The RenderSystem.
		*\param[in] dsState
		*	The depth stencil state.
		*\param[in] rsState
		*	The rateriser state.
		*\param[in] blState
		*	The blend state.
		*\param[in] msState
		*	The multisample state.
		*\param[in] program
		*	The shader program.
		*\param[in] flags
		*	The creation flags.
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] owner
		*	La passe de rendu parente.
		*\param[in] renderSystem
		*	Le RenderSystem.
		*\param[in] dsState
		*	L'état de stencil et profondeur.
		*\param[in] rsState
		*	L'état de rastériseur.
		*\param[in] blState
		*	L'état de mélange.
		*\param[in] msState
		*	L'état de multi-échantillonnage.
		*\param[in] program
		*	Le programme shader.
		*\param[in] flags
		*	Les indicateurs de création.
		*/
		C3D_API explicit RenderPipeline( SceneRenderPass & owner
			, RenderSystem & renderSystem
			, ashes::PipelineDepthStencilStateCreateInfo dsState
			, ashes::PipelineRasterizationStateCreateInfo rsState
			, ashes::PipelineColorBlendStateCreateInfo blState
			, ashes::PipelineMultisampleStateCreateInfo msState
			, ShaderProgramSPtr program
			, PipelineFlags const & flags );
		/**
		*\~english
		*\brief
		*	Denstructor.
		*\~french
		*\brief
		*	Destructeur.
		*/
		C3D_API ~RenderPipeline();
		/**
		*\~english
		*\brief
		*	Initialises the pipeline.
		*\param[in] device
		*	The GPU device.
		*\param[in] renderPass
		*	The render pass to which this pipeline is bound.
		*\~french
		*\brief
		*	Initialise le pipeline.
		*\param[in] device
		*	Le device GPU.
		*\param[in] renderPass
		*	La passe de rendu à laquelle ce pipeline est lié.
		*/
		C3D_API void initialise( RenderDevice const & device
			, VkRenderPass renderPass
			, ashes::DescriptorSetLayoutCRefArray descriptorLayouts );
		/**
		*\~english
		*\brief
		*	Cleans up the pipeline.
		*\param[in] device
		*	The GPU device.
		*\~french
		*\brief
		*	Nettoie le pipeline.
		*\param[in] device
		*	Le device GPU.
		*/
		C3D_API void cleanup( RenderDevice const & device );
		/**
		*\~english
		*\brief
		*	Creates the descriptor pools for \p maxSets descriptor sets per descriptor set layout.
		*\param[in] maxSets
		*	The number of descriptor sets to be allocatable by the pools, per layout.
		*\~french
		*\brief
		*	Crée les pools de descripteurs pour \p maxSets ensembles de descripteurs par layout d'ensemble de descripteurs.
		*\param[in] maxSets
		*	Le nombre d'ensembles de descripteurs allouables par les pools, par layout.
		*/
		C3D_API void createDescriptorPools( uint32_t maxSets );
		/**
		*\~english
		*name
		*	Mutators.
		*\remarks
		*	They must be called before any call to initialise().
		*\~french
		*name
		*	Mutateurs.
		*\remarks
		*	Ils doivent être appelés avant l'appel à initialise().
		**/
		/**@{*/
		C3D_API void setVertexLayouts( ashes::PipelineVertexInputStateCreateInfoCRefArray const & layouts );
		C3D_API void setDescriptorSetLayout( ashes::DescriptorSetLayoutPtr layout );
		C3D_API void setAdditionalDescriptorSet( SubmeshRenderNode const & node
			, ashes::DescriptorSetPtr descriptorSet );
		C3D_API void setAdditionalDescriptorSet( BillboardRenderNode const & node
			, ashes::DescriptorSetPtr descriptorSet );

		void setVertexLayouts( std::vector< ashes::PipelineVertexInputStateCreateInfo > layouts )
		{
			CU_Require( !m_pipeline );
			m_vertexLayouts = std::move( layouts );
		}

		void setPushConstantRanges( ashes::VkPushConstantRangeArray const & pushConstantRanges )
		{
			CU_Require( !m_pipeline );
			m_pushConstantRanges = pushConstantRanges;
		}

		void setViewport( VkViewport const & viewport )
		{
			CU_Require( !m_pipeline );
			m_viewport = std::make_unique< VkViewport >( viewport );
		}

		void setScissor( VkRect2D const & scissor )
		{
			CU_Require( !m_pipeline );
			m_scissor = std::make_unique< VkRect2D >( scissor );
		}
		/**@}*/
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		**/
		/**@{*/
		C3D_API ashes::DescriptorSet const & getAdditionalDescriptorSet( SubmeshRenderNode const & node )const;
		C3D_API ashes::DescriptorSet const & getAdditionalDescriptorSet( BillboardRenderNode const & node )const;

		PipelineFlags const & getFlags()const
		{
			return m_flags;
		}

		bool hasPipeline()const
		{
			return m_pipeline != nullptr;
		}

		ashes::GraphicsPipeline const & getPipeline()const
		{
			hasPipeline();
			return *m_pipeline;
		}

		ashes::PipelineLayout const & getPipelineLayout()const
		{
			CU_Require( m_pipelineLayout );
			return *m_pipelineLayout;
		}

		ashes::DescriptorSetLayout const & getDescriptorSetLayout()const
		{
			return *m_descriptorLayout;
		}

		bool hasDescriptorSetLayout()const
		{
			return m_descriptorLayout != nullptr;
		}

		ashes::DescriptorSetPool const & getDescriptorPool()const
		{
			return *m_descriptorPool;
		}

		RenderSystem & getRenderSystem()const
		{
			return m_renderSystem;
		}
		/**@}*/

	private:
		RenderSystem & m_renderSystem;
		ashes::PipelineDepthStencilStateCreateInfo m_dsState;
		ashes::PipelineRasterizationStateCreateInfo m_rsState;
		ashes::PipelineColorBlendStateCreateInfo m_blState;
		ashes::PipelineMultisampleStateCreateInfo m_msState;
		ShaderProgramSPtr m_program;
		PipelineFlags m_flags;
		std::vector< ashes::PipelineVertexInputStateCreateInfo > m_vertexLayouts;
		ashes::DescriptorSetLayoutPtr m_descriptorLayout;
		ashes::DescriptorSetPoolPtr m_descriptorPool;
		ashes::VkPushConstantRangeArray m_pushConstantRanges;
		std::unique_ptr< VkViewport > m_viewport;
		std::unique_ptr< VkRect2D > m_scissor;
		ashes::PipelineLayoutPtr m_pipelineLayout;
		ashes::GraphicsPipelinePtr m_pipeline;
		std::unordered_map< SubmeshRenderNode const *, ashes::DescriptorSetPtr > m_submeshAddDescriptors;
		std::unordered_map< BillboardRenderNode const *, ashes::DescriptorSetPtr > m_billboardAddDescriptors;
	};
}

#endif
