/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderPipeline_H___
#define ___C3D_RenderPipeline_H___

#include "Castor3DPrerequisites.hpp"

#include <Pipeline/ColourBlendState.hpp>
#include <Pipeline/DepthStencilState.hpp>
#include <Pipeline/MultisampleState.hpp>
#include <Pipeline/RasterisationState.hpp>

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
			, renderer::DepthStencilState && dsState
			, renderer::RasterisationState && rsState
			, renderer::ColourBlendState && blState
			, renderer::MultisampleState && msState
			, renderer::ShaderProgram & program
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
		*	Cleans up the pipeline.
		*\~french
		*\brief
		*	Nettoie le pipeline.
		*/
		C3D_API void cleanup();
		/**
		*\~english
		*\brief
		*	Initialises the pipeline.
		*\param[in] renderPass
		*	The render pass to which this pipeline is bound.
		*\param[in] topology
		*	The rendering topology.
		*\~french
		*\brief
		*	Initialise le pipeline.
		*\param[in] renderPass
		*	La passe de rendu à laquelle ce pipeline est lié.
		*\param[in] topology
		*	La topology de dessin.
		*/
		C3D_API void initialise( renderer::RenderPass const & renderPass
			, renderer::PrimitiveTopology topology );
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
		inline void setVertexLayouts( renderer::VertexLayoutCRefArray const & layouts )
		{
			REQUIRE( !m_pipeline );
			m_vertexLayouts = layouts;
		}

		inline void setDescriptorSetLayouts( renderer::DescriptorSetLayoutCRefArray const & layouts )
		{
			REQUIRE( !m_pipeline );
			m_descriptorLayouts = layouts;
		}

		inline void setPushConstantRanges( renderer::PushConstantRangeCRefArray const & pushConstantRanges )
		{
			REQUIRE( !m_pipeline );
			m_pushConstantRanges = pushConstantRanges;
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

		inline renderer::Pipeline const & getPipeline()const
		{
			REQUIRE( m_pipeline );
			return *m_pipeline;
		}

		inline renderer::PipelineLayout const & getPipelineLayout()const
		{
			REQUIRE( m_pipelineLayout );
			return *m_pipelineLayout;
		}
		/**@}*/

	private:
		renderer::DepthStencilState m_dsState;
		renderer::RasterisationState m_rsState;
		renderer::ColourBlendState m_blState;
		renderer::MultisampleState m_msState;
		renderer::ShaderProgram & m_program;
		PipelineFlags m_flags;
		renderer::VertexLayoutCRefArray m_vertexLayouts;
		renderer::DescriptorSetLayoutCRefArray m_descriptorLayouts;
		renderer::PushConstantRangeCRefArray m_pushConstantRanges;
		renderer::PipelineLayoutPtr m_pipelineLayout;
		renderer::PipelinePtr m_pipeline;
	};
}

#endif
