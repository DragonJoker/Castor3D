/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderPipeline_H___
#define ___C3D_RenderPipeline_H___

#include "Castor3DPrerequisites.hpp"

#include <Descriptor/DescriptorSetLayout.hpp>
#include <Descriptor/DescriptorSetPool.hpp>
#include <Pipeline/ColourBlendState.hpp>
#include <Pipeline/DepthStencilState.hpp>
#include <Pipeline/MultisampleState.hpp>
#include <Pipeline/RasterisationState.hpp>
#include <Pipeline/Scissor.hpp>
#include <Pipeline/VertexLayout.hpp>
#include <Pipeline/Viewport.hpp>

#include <Design/OwnedBy.hpp>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		The rendering pipeline.
	\remark		Defines the various matrices, applies the transformations it can support.
	\~french
	\brief		Le pipeline de rendu.
	\remark		Définit les diverses matrices, applique les transformations supportées.
	*/
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
			, ashes::DepthStencilState && dsState
			, ashes::RasterisationState && rsState
			, ashes::ColourBlendState && blState
			, ashes::MultisampleState && msState
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
		inline void setVertexLayouts( std::vector< ashes::VertexLayout > layouts )
		{
			REQUIRE( !m_pipeline );
			m_vertexLayouts = std::move( layouts );
		}

		inline void setVertexLayouts( ashes::VertexLayoutCRefArray const & layouts )
		{
			REQUIRE( !m_pipeline );
			for ( auto & layout : layouts )
			{
				m_vertexLayouts.push_back( layout.get() );
			}
		}

		inline void setDescriptorSetLayouts( std::vector< ashes::DescriptorSetLayoutPtr > && layouts )
		{
			REQUIRE( !m_pipeline );
			m_descriptorLayouts = std::move( layouts );
		}

		inline void setPushConstantRanges( ashes::PushConstantRangeArray const & pushConstantRanges )
		{
			REQUIRE( !m_pipeline );
			m_pushConstantRanges = pushConstantRanges;
		}

		inline void setViewport( ashes::Viewport const & viewport )
		{
			REQUIRE( !m_pipeline );
			m_viewport = std::make_unique< ashes::Viewport >( viewport );
		}

		inline void setScissor( ashes::Scissor const & scissor )
		{
			REQUIRE( !m_pipeline );
			m_scissor = std::make_unique< ashes::Scissor >( scissor );
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

		inline ashes::Pipeline const & getPipeline()const
		{
			REQUIRE( m_pipeline );
			return *m_pipeline;
		}

		inline ashes::PipelineLayout const & getPipelineLayout()const
		{
			REQUIRE( m_pipelineLayout );
			return *m_pipelineLayout;
		}

		inline ashes::DescriptorSetLayout const & getDescriptorSetLayout( uint32_t index )const
		{
			REQUIRE( index < m_descriptorLayouts.size() );
			return *m_descriptorLayouts[index];
		}

		inline bool hasDescriptorPool( uint32_t index )const
		{
			return index < m_descriptorPools.size();
		}

		inline ashes::DescriptorSetPool const & getDescriptorPool( uint32_t index )const
		{
			REQUIRE( index < m_descriptorPools.size() );
			return *m_descriptorPools[index];
		}
		/**@}*/

	private:
		ashes::DepthStencilState m_dsState;
		ashes::RasterisationState m_rsState;
		ashes::ColourBlendState m_blState;
		ashes::MultisampleState m_msState;
		ShaderProgramSPtr m_program;
		PipelineFlags m_flags;
		std::vector< ashes::VertexLayout > m_vertexLayouts;
		std::vector< ashes::DescriptorSetLayoutPtr > m_descriptorLayouts;
		std::vector< ashes::DescriptorSetPoolPtr > m_descriptorPools;
		ashes::PushConstantRangeArray m_pushConstantRanges;
		std::unique_ptr< ashes::Viewport > m_viewport;
		std::unique_ptr< ashes::Scissor > m_scissor;
		ashes::PipelineLayoutPtr m_pipelineLayout;
		ashes::PipelinePtr m_pipeline;
	};
}

#endif
