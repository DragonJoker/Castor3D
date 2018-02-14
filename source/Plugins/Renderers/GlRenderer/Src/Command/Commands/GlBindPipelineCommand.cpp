/*
This file belongs to GlRenderer.
See LICENSE file in root folder.
*/
#include "GlBindPipelineCommand.hpp"

#include "Core/GlDevice.hpp"
#include "Pipeline/GlPipeline.hpp"
#include "Pipeline/GlPipelineLayout.hpp"
#include "Shader/GlShaderProgram.hpp"

namespace gl_renderer
{
	void apply( Device const & device
		, renderer::ColourBlendState const & state )
	{
		auto & save = device.getCurrentBlendState();
		static bool hadBlend = false;

		if ( state != save )
		{
			if ( state.isLogicOpEnabled()
				&& !save.isLogicOpEnabled()
				&& state.getLogicOp() != save.getLogicOp() )
			{
				glLogCall( gl::LogicOp, convert( state.getLogicOp() ) );
			}

			if ( state.getBlendConstants() != save.getBlendConstants() )
			{
				auto & blendConstants = state.getBlendConstants();
				glLogCall( gl::BlendColor
					, blendConstants[0]
					, blendConstants[1]
					, blendConstants[2]
					, blendConstants[3] );
			}

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

			if ( hadBlend != blend )
			{
				if ( blend )
				{
					glLogCall( gl::Enable, GL_BLEND );
				}
				else
				{
					glLogCall( gl::Disable, GL_BLEND );
				}
			}

			save = state;
			hadBlend = blend;
		}
	}

	void apply( Device const & device
		, renderer::RasterisationState const & state )
	{
		auto & save = device.getCurrentRasterisationState();

		if ( state != save )
		{
			if ( state.getCullMode() != save.getCullMode()
				|| state.getFrontFace() != save.getFrontFace() )
			{
				if ( state.getCullMode() != renderer::CullModeFlag::eNone )
				{
					if ( save.getCullMode() == renderer::CullModeFlag::eNone )
					{
						glLogCall( gl::Enable, GL_CULL_FACE );
					}

					if ( state.getCullMode() != save.getCullMode() )
					{
						glLogCall( gl::CullFace, convert( state.getCullMode() ) );
					}

					if ( state.getFrontFace() != save.getFrontFace() )
					{
						glLogCall( gl::FrontFace, convert( state.getFrontFace() ) );
					}
				}
				else if ( save.getCullMode() != renderer::CullModeFlag::eNone )
				{
					glLogCall( gl::Disable, GL_CULL_FACE );
				}
			}

			if ( state.getPolygonMode() != save.getPolygonMode() )
			{
				glLogCall( gl::PolygonMode
					, GL_CULL_MODE_FRONT_AND_BACK
					, convert( state.getPolygonMode() ) );
			}

			if ( state.isDepthBiasEnabled() != save.isDepthBiasEnabled()
				|| state.getPolygonMode() != save.getPolygonMode() )
			{
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
			}

			if ( state.isDepthClampEnabled() != save.isDepthClampEnabled() )
			{
				if ( state.isDepthClampEnabled() )
				{
					glLogCall( gl::Enable, GL_DEPTH_CLAMP );
				}
				else
				{
					glLogCall( gl::Disable, GL_DEPTH_CLAMP );
				}
			}

			if ( state.isRasteriserDiscardEnabled() != save.isRasteriserDiscardEnabled() )
			{
				if ( state.isRasteriserDiscardEnabled() )
				{
					glLogCall( gl::Enable, GL_RASTERIZER_DISCARD );
				}
				else
				{
					glLogCall( gl::Disable, GL_RASTERIZER_DISCARD );
				}
			}

			if ( state.getPolygonMode() == renderer::PolygonMode::eLine )
			{
				glLogCall( gl::LineWidth, state.getLineWidth() );
			}

			save = state;
		}
	}

	void apply( Device const & device
		, renderer::MultisampleState const & state )
	{
		auto & save = device.getCurrentMultisampleState();

		if ( state != save )
		{
			if ( state.getRasterisationSamples() != renderer::SampleCountFlag::e1 )
			{
				glLogCall( gl::Enable, GL_MULTISAMPLE );

				if ( state.isAlphaToCoverageEnabled() != save.isAlphaToCoverageEnabled() )
				{
					if ( state.isAlphaToCoverageEnabled() )
					{
						glLogCall( gl::Enable, GL_SAMPLE_ALPHA_TO_COVERAGE );
					}
					else
					{
						glLogCall( gl::Disable, GL_SAMPLE_ALPHA_TO_COVERAGE );
					}
				}

				if ( state.isAlphaToOneEnabled() != save.isAlphaToOneEnabled() )
				{
					if ( state.isAlphaToOneEnabled() )
					{
						glLogCall( gl::Enable, GL_SAMPLE_ALPHA_TO_ONE );
					}
					else
					{
						glLogCall( gl::Disable, GL_SAMPLE_ALPHA_TO_ONE );
					}
				}
			}
			else
			{
				glLogCall( gl::Disable, GL_MULTISAMPLE );
			}

			save = state;
		}
	}

	void apply( Device const & device
		, renderer::DepthStencilState const & state )
	{
		auto & save = device.getCurrentDepthStencilState();

		if ( state != save )
		{
			if ( state.isDepthWriteEnabled() != save.isDepthWriteEnabled() )
			{
				if ( state.isDepthWriteEnabled() )
				{
					glLogCall( gl::DepthMask, GL_TRUE );
				}
				else
				{
					glLogCall( gl::DepthMask, GL_FALSE );
				}
			}

			if ( state.isDepthTestEnabled() != save.isDepthTestEnabled() )
			{
				if ( state.isDepthTestEnabled() )
				{
					glLogCall( gl::Enable, GL_DEPTH_TEST );
					glLogCall( gl::DepthFunc, convert( state.getDepthCompareOp() ) );
				}
				else
				{
					glLogCall( gl::Disable, GL_DEPTH_TEST );
				}
			}

			if ( state.isStencilTestEnabled() != save.isStencilTestEnabled()
				|| ( state.isStencilTestEnabled()
					&& ( state.getBackStencilOp() != save.getBackStencilOp()
						|| state.getFrontStencilOp() != save.getFrontStencilOp() ) ) )
			{
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
			}

			if ( state.isDepthBoundsTestEnabled() != save.isDepthBoundsTestEnabled()
				|| ( state.isDepthBoundsTestEnabled()
					&& ( state.getMinDepthBounds() != save.getMinDepthBounds()
						|| state.getMaxDepthBounds() != save.getMaxDepthBounds() ) ) )
			{
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

			save = state;
		}
	}

	void apply( Device const & device
		, renderer::TessellationState const & state )
	{
		auto & save = device.getCurrentTessellationState();

		if ( state != save )
		{
			if ( state.getControlPoints() )
			{
				glLogCall( gl::PatchParameteri, GL_PATCH_VERTICES, int( state.getControlPoints() ) );
			}

			save = state;
		}
	}

	void apply( Device const & device
		, renderer::Viewport const & state )
	{
		auto & save = device.getCurrentViewport();

		if ( state != save )
		{
			glLogCall( gl::Viewport
				, state.getOffset()[0]
				, state.getOffset()[1]
				, state.getSize()[0]
				, state.getSize()[1] );
		}
	}

	void apply( Device const & device
		, renderer::Scissor const & state )
	{
		auto & save = device.getCurrentScissor();

		if ( state != save )
		{
			glLogCall( gl::Scissor
				, state.getOffset()[0]
				, state.getOffset()[1]
				, state.getSize()[0]
				, state.getSize()[1] );
		}
	}

	BindPipelineCommand::BindPipelineCommand( Device const & device
		, renderer::Pipeline const & pipeline
		, renderer::PipelineBindPoint bindingPoint )
		: m_device{ device }
		, m_pipeline{ static_cast< Pipeline const & > ( pipeline ) }
		, m_layout{ static_cast< PipelineLayout const & > ( m_pipeline.getLayout() ) }
		, m_program{ static_cast< ShaderProgram const & > ( m_pipeline.getProgram() ) }
		, m_bindingPoint{ bindingPoint }
	{
	}

	void BindPipelineCommand::apply()const
	{
		glLogCommand( "BindPipelineCommand" );
		gl_renderer::apply( m_device, m_pipeline.getColourBlendState() );
		gl_renderer::apply( m_device, m_pipeline.getRasterisationState() );
		gl_renderer::apply( m_device, m_pipeline.getDepthStencilState() );
		gl_renderer::apply( m_device, m_pipeline.getMultisampleState() );
		gl_renderer::apply( m_device, m_pipeline.getTessellationState() );

		if ( m_pipeline.hasViewport() )
		{
			gl_renderer::apply( m_device, m_pipeline.getViewport() );
		}

		if ( m_pipeline.hasScissor() )
		{
			gl_renderer::apply( m_device, m_pipeline.getScissor() );
		}

		auto & save = m_device.getCurrentProgram();

		if ( m_program.getProgram() != save )
		{
			glLogCall( gl::UseProgram, m_program.getProgram() );
			save = m_program.getProgram();
		}
	}

	CommandPtr BindPipelineCommand::clone()const
	{
		return std::make_unique< BindPipelineCommand >( *this );
	}
}
