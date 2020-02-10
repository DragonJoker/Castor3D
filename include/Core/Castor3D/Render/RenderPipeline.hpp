/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderPipeline_H___
#define ___C3D_RenderPipeline_H___

#include "Castor3D/Castor3DPrerequisites.hpp"

#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/Pipeline/PipelineColorBlendStateCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineDepthStencilStateCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineMultisampleStateCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineRasterizationStateCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineVertexInputStateCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineViewportStateCreateInfo.hpp>

#include <CastorUtils/Design/OwnedBy.hpp>

namespace castor3d
{
	class RenderPipeline
		: public castor::OwnedBy< RenderSystem >
	{
	public:
		/**
		*\~english
		*\brief
		*	Constructor.
		*\param[in] renderSystem
		*	The parent RenderSystem.
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
		*\param[in] renderSystem
		*	Le RenderSystem parent.
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
		C3D_API explicit RenderPipeline( RenderSystem & renderSystem
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
		*\param[in] renderPass
		*	The render pass to which this pipeline is bound.
		*\~french
		*\brief
		*	Initialise le pipeline.
		*\param[in] renderPass
		*	La passe de rendu à laquelle ce pipeline est lié.
		*/
		C3D_API void initialise( ashes::RenderPass const & renderPass );
		/**
		*\~english
		*\brief
		*	Cleans up the pipeline.
		*\~french
		*\brief
		*	Nettoie le pipeline.
		*/
		C3D_API void cleanup();
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
		C3D_API void setDescriptorSetLayouts( std::vector< ashes::DescriptorSetLayoutPtr > && layouts );

		inline void setVertexLayouts( std::vector< ashes::PipelineVertexInputStateCreateInfo > layouts )
		{
			CU_Require( !m_pipeline );
			m_vertexLayouts = std::move( layouts );
		}

		inline void setPushConstantRanges( ashes::VkPushConstantRangeArray const & pushConstantRanges )
		{
			CU_Require( !m_pipeline );
			m_pushConstantRanges = pushConstantRanges;
		}

		inline void setViewport( VkViewport const & viewport )
		{
			CU_Require( !m_pipeline );
			m_viewport = std::make_unique< VkViewport >( viewport );
		}

		inline void setScissor( VkRect2D const & scissor )
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
		inline PipelineFlags const & getFlags()const
		{
			return m_flags;
		}

		inline ashes::GraphicsPipeline const & getPipeline()const
		{
			CU_Require( m_pipeline );
			return *m_pipeline;
		}

		inline ashes::PipelineLayout const & getPipelineLayout()const
		{
			CU_Require( m_pipelineLayout );
			return *m_pipelineLayout;
		}

		inline ashes::DescriptorSetLayout const & getDescriptorSetLayout( uint32_t index )const
		{
			CU_Require( index < m_descriptorLayouts.size() );
			return *m_descriptorLayouts[index];
		}

		inline bool hasDescriptorPool( uint32_t index )const
		{
			return index < m_descriptorPools.size();
		}

		inline ashes::DescriptorSetPool const & getDescriptorPool( uint32_t index )const
		{
			CU_Require( index < m_descriptorPools.size() );
			return *m_descriptorPools[index];
		}
		/**@}*/

	private:
		ashes::PipelineDepthStencilStateCreateInfo m_dsState;
		ashes::PipelineRasterizationStateCreateInfo m_rsState;
		ashes::PipelineColorBlendStateCreateInfo m_blState;
		ashes::PipelineMultisampleStateCreateInfo m_msState;
		ShaderProgramSPtr m_program;
		PipelineFlags m_flags;
		std::vector< ashes::PipelineVertexInputStateCreateInfo > m_vertexLayouts;
		std::vector< ashes::DescriptorSetLayoutPtr > m_descriptorLayouts;
		std::vector< ashes::DescriptorSetPoolPtr > m_descriptorPools;
		ashes::VkPushConstantRangeArray m_pushConstantRanges;
		std::unique_ptr< VkViewport > m_viewport;
		std::unique_ptr< VkRect2D > m_scissor;
		ashes::PipelineLayoutPtr m_pipelineLayout;
		ashes::GraphicsPipelinePtr m_pipeline;
	};
}

#endif
