/*
This file belongs to GlRenderer.
See LICENSE file in root folder.
*/
#include "GlBindPipelineCommand.hpp"

#include "Pipeline/GlPipeline.hpp"
#include "Pipeline/GlPipelineLayout.hpp"
#include "Shader/GlShaderProgram.hpp"

namespace gl_renderer
{
	namespace
	{
		void doApply( renderer::ColourBlendState const & state )
		{
			if ( state.isLogicOpEnabled() )
			{
				glLogCall( gl::LogicOp, convert( state.getLogicOp() ) );
			}

			auto & blendConstants = state.getBlendConstants();
			glLogCall( gl::BlendColor
				, blendConstants[0]
				, blendConstants[1]
				, blendConstants[2]
				, blendConstants[3] );
			bool blend = false;
			GLuint buf = 0u;

			for ( auto & blendState : state )
			{
				if ( blendState.isBlendEnabled() )
				{
					blend = true;
					glLogCall( gl::BlendEquationSeparatei
						, buf
						, convert( blendState.getColourBlendOp() )
						, convert( blendState.getAlphaBlendOp() ) );
					glLogCall( gl::BlendFuncSeparatei
						, buf
						, convert( blendState.getSrcColourBlendFactor() )
						, convert( blendState.getDstColourBlendFactor() )
						, convert( blendState.getSrcAlphaBlendFactor() )
						, convert( blendState.getDstAlphaBlendFactor() ) );
				}

				++buf;
			}

			if ( blend )
			{
				glLogCall( gl::Enable, GL_BLEND );
			}
			else
			{
				glLogCall( gl::Disable, GL_BLEND );
			}
		}

		void doApply( renderer::RasterisationState const & state )
		{
			if ( state.getCullMode() != renderer::CullModeFlag::eNone )
			{
				glLogCall( gl::Enable, GL_CULL_FACE );
				glLogCall( gl::CullFace, convert( state.getCullMode() ) );
				glLogCall( gl::FrontFace, convert( state.getFrontFace() ) );
			}
			else
			{
				glLogCall( gl::Disable, GL_CULL_FACE );
			}

			glLogCall( gl::PolygonMode
				, GL_CULL_MODE_FRONT_AND_BACK
				, convert( state.getPolygonMode() ) );

			if ( state.isDepthBiasEnabled() )
			{
				switch ( state.getPolygonMode() )
				{
				case renderer::PolygonMode::eFill:
					glLogCall( gl::Enable, GL_POLYGON_OFFSET_FILL );
					break;

				case renderer::PolygonMode::eLine:
					glLogCall( gl::Enable, GL_POLYGON_OFFSET_LINE );
					break;

				case renderer::PolygonMode::ePoint:
					glLogCall( gl::Enable, GL_POLYGON_OFFSET_POINT );
					break;
				}

				glLogCall( gl::PolygonOffsetClampEXT, state.getDepthBiasConstantFactor()
					, state.getDepthBiasSlopeFactor()
					, state.getDepthBiasClamp() );
			}
			else
			{
				switch ( state.getPolygonMode() )
				{
				case renderer::PolygonMode::eFill:
					glLogCall( gl::Disable, GL_POLYGON_OFFSET_FILL );
					break;

				case renderer::PolygonMode::eLine:
					glLogCall( gl::Disable, GL_POLYGON_OFFSET_LINE );
					break;

				case renderer::PolygonMode::ePoint:
					glLogCall( gl::Disable, GL_POLYGON_OFFSET_POINT );
					break;
				}
			}

			if ( state.isDepthClampEnabled() )
			{
				glLogCall( gl::Enable, GL_DEPTH_CLAMP );
			}
			else
			{
				glLogCall( gl::Disable, GL_DEPTH_CLAMP );
			}

			if ( state.isRasteriserDiscardEnabled() )
			{
				glLogCall( gl::Enable, GL_RASTERIZER_DISCARD );
			}
			else
			{
				glLogCall( gl::Disable, GL_RASTERIZER_DISCARD );
			}

			if ( state.getPolygonMode() == renderer::PolygonMode::eLine )
			{
				glLogCall( gl::LineWidth, state.getLineWidth() );
			}
		}

		void doApply( renderer::MultisampleState const & state )
		{
			if ( state.getRasterisationSamples() != renderer::SampleCountFlag::e1 )
			{
				glLogCall( gl::Enable, GL_MULTISAMPLE );

				if ( state.isAlphaToCoverageEnabled() )
				{
					glLogCall( gl::Enable, GL_SAMPLE_ALPHA_TO_COVERAGE );
				}
				else
				{
					glLogCall( gl::Disable, GL_SAMPLE_ALPHA_TO_COVERAGE );
				}

				if ( state.isAlphaToOneEnabled() )
				{
					glLogCall( gl::Enable, GL_SAMPLE_ALPHA_TO_ONE );
				}
				else
				{
					glLogCall( gl::Disable, GL_SAMPLE_ALPHA_TO_ONE );
				}
			}
			else
			{
				glLogCall( gl::Disable, GL_MULTISAMPLE );
			}
		}

		void doApply( renderer::DepthStencilState const & state )
		{
			if ( state.isDepthWriteEnabled() )
			{
				glLogCall( gl::DepthMask, GL_TRUE );
			}
			else
			{
				glLogCall( gl::DepthMask, GL_FALSE );
			}

			if ( state.isDepthTestEnabled() )
			{
				glLogCall( gl::Enable, GL_DEPTH_TEST );
				glLogCall( gl::DepthFunc, convert( state.getDepthCompareOp() ) );
			}
			else
			{
				glLogCall( gl::Disable, GL_DEPTH_TEST );
			}

			if ( state.isStencilTestEnabled() )
			{
				glLogCall( gl::Enable, GL_STENCIL_TEST );

				glLogCall( gl::StencilMaskSeparate
					, GL_CULL_MODE_BACK
					, state.getBackStencilOp().getWriteMask() );
				glLogCall( gl::StencilFuncSeparate
					, GL_CULL_MODE_BACK
					, convert( state.getBackStencilOp().getCompareOp() )
					, state.getBackStencilOp().getReference()
					, state.getBackStencilOp().getCompareMask() );
				glLogCall( gl::StencilOpSeparate
					, GL_CULL_MODE_BACK
					, convert( state.getBackStencilOp().getFailOp() )
					, convert( state.getBackStencilOp().getDepthFailOp() )
					, convert( state.getBackStencilOp().getPassOp() ) );
				glLogCall( gl::StencilMaskSeparate
					, GL_CULL_MODE_FRONT
					, state.getFrontStencilOp().getWriteMask() );
				glLogCall( gl::StencilFuncSeparate
					, GL_CULL_MODE_FRONT
					, convert( state.getFrontStencilOp().getCompareOp() )
					, state.getFrontStencilOp().getReference()
					, state.getFrontStencilOp().getCompareMask() );
				glLogCall( gl::StencilOpSeparate
					, GL_CULL_MODE_FRONT
					, convert( state.getFrontStencilOp().getFailOp() )
					, convert( state.getFrontStencilOp().getDepthFailOp() )
					, convert( state.getFrontStencilOp().getPassOp() ) );
			}
			else
			{
				glLogCall( gl::Disable, GL_STENCIL_TEST );
			}

			if ( state.isDepthBoundsTestEnabled() )
			{
				glLogCall( gl::Enable, GL_DEPTH_CLAMP );
				glLogCall( gl::DepthRange, state.getMinDepthBounds(), state.getMaxDepthBounds() );
			}
			else
			{
				glLogCall( gl::Disable, GL_DEPTH_CLAMP );
			}
		}

		void doApply( renderer::TessellationState const & state )
		{
			if ( state.getControlPoints() )
			{
				glLogCall( gl::PatchParameteri, GL_PATCH_VERTICES, int( state.getControlPoints() ) );
			}
		}

		void doApply( renderer::Viewport const & state )
		{
			glLogCall( gl::Viewport
				, state.getOffset()[0]
				, state.getOffset()[1]
				, state.getSize()[0]
				, state.getSize()[1] );
		}

		void doApply( renderer::Scissor const & state )
		{
			glLogCall( gl::Scissor
				, state.getOffset()[0]
				, state.getOffset()[1]
				, state.getSize()[0]
				, state.getSize()[1] );
		}
	}

	BindPipelineCommand::BindPipelineCommand( renderer::Pipeline const & pipeline
		, renderer::PipelineBindPoint bindingPoint )
		: m_pipeline{ static_cast< Pipeline const & > ( pipeline ) }
		, m_layout{ static_cast< PipelineLayout const & > ( m_pipeline.getLayout() ) }
		, m_program{ static_cast< ShaderProgram const & > ( m_pipeline.getProgram() ) }
		, m_bindingPoint{ bindingPoint }
	{
	}

	void BindPipelineCommand::apply()const
	{
		glLogCommand( "BindPipelineCommand" );
		doApply( m_pipeline.getColourBlendState() );
		doApply( m_pipeline.getRasterisationState() );
		doApply( m_pipeline.getDepthStencilState() );
		doApply( m_pipeline.getMultisampleState() );
		doApply( m_pipeline.getTessellationState() );

		if ( m_pipeline.hasViewport() )
		{
			doApply( m_pipeline.getViewport() );
		}

		if ( m_pipeline.hasScissor() )
		{
			doApply( m_pipeline.getScissor() );
		}

		glLogCall( gl::UseProgram, m_program.getProgram() );
	}

	CommandPtr BindPipelineCommand::clone()const
	{
		return std::make_unique< BindPipelineCommand >( *this );
	}
}
