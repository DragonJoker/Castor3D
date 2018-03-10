/*
This file belongs to GlRenderer.
See LICENSE file in root folder.
*/
#include "GlBindPipelineCommand.hpp"

#include "Core/GlDevice.hpp"
#include "Pipeline/GlPipeline.hpp"
#include "Pipeline/GlPipelineLayout.hpp"

namespace gl_renderer
{
	void apply( Device const & device
		, renderer::InputAssemblyState const & state )
	{
		auto & save = device.getCurrentInputAssemblyState();

		if ( state != save )
		{
			if ( state.topology != save.topology )
			{
				if ( state.topology == renderer::PrimitiveTopology::ePointList )
				{
					glLogCall( gl::Enable, GL_PROGRAM_POINT_SIZE );
				}
				else
				{
					glLogCall( gl::Disable, GL_PROGRAM_POINT_SIZE );
				}
			}

			if ( state.primitiveRestartEnable != save.primitiveRestartEnable )
			{
				if ( state.primitiveRestartEnable )
				{
					glLogCall( gl::Enable, GL_PRIMITIVE_RESTART );
				}
				else
				{
					glLogCall( gl::Disable, GL_PRIMITIVE_RESTART );
				}
			}
		}
	}

	void apply( Device const & device
		, renderer::ColourBlendState const & state )
	{
		auto & save = device.getCurrentBlendState();
		static bool hadBlend = false;

		if ( state != save )
		{
			if ( state.logicOpEnable
				&& !save.logicOpEnable
				&& state.logicOp != save.logicOp )
			{
				glLogCall( gl::LogicOp, convert( state.logicOp ) );
			}

			if ( state.blendConstants != save.blendConstants )
			{
				auto & blendConstants = state.blendConstants;
				glLogCall( gl::BlendColor
					, blendConstants[0]
					, blendConstants[1]
					, blendConstants[2]
					, blendConstants[3] );
			}

			bool blend = false;
			GLuint buf = 0u;

			for ( auto & blendState : state.attachs )
			{
				if ( blendState.blendEnable )
				{
					blend = true;
					glLogCall( gl::BlendEquationSeparatei
						, buf
						, convert( blendState.colorBlendOp )
						, convert( blendState.alphaBlendOp ) );
					glLogCall( gl::BlendFuncSeparatei
						, buf
						, convert( blendState.srcColorBlendFactor )
						, convert( blendState.dstColorBlendFactor )
						, convert( blendState.srcAlphaBlendFactor )
						, convert( blendState.dstAlphaBlendFactor ) );
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
		, renderer::RasterisationState const & state
		, bool dynamicLineWidth
		, bool dynamicDepthBias )
	{
		auto & save = device.getCurrentRasterisationState();

		if ( state != save )
		{
			if ( state.cullMode != save.cullMode
				|| state.frontFace != save.frontFace )
			{
				if ( state.cullMode != renderer::CullModeFlag::eNone )
				{
					if ( save.cullMode == renderer::CullModeFlag::eNone )
					{
						glLogCall( gl::Enable, GL_CULL_FACE );
					}

					if ( state.cullMode != save.cullMode )
					{
						glLogCall( gl::CullFace, convert( state.cullMode ) );
					}

					if ( state.frontFace != save.frontFace )
					{
						glLogCall( gl::FrontFace, convert( state.frontFace ) );
					}
				}
				else if ( save.cullMode != renderer::CullModeFlag::eNone )
				{
					glLogCall( gl::Disable, GL_CULL_FACE );
				}
			}

			if ( state.polygonMode != save.polygonMode )
			{
				glLogCall( gl::PolygonMode
					, GL_CULL_MODE_FRONT_AND_BACK
					, convert( state.polygonMode ) );
			}

			if ( state.depthBiasEnable != save.depthBiasEnable
				|| state.polygonMode != save.polygonMode )
			{
				if ( state.depthBiasEnable )
				{
					switch ( state.polygonMode )
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

					if ( !dynamicDepthBias )
					{
						glLogCall( gl::PolygonOffsetClampEXT, state.depthBiasConstantFactor
							, state.depthBiasSlopeFactor
							, state.depthBiasClamp );
					}
				}
				else
				{
					switch ( state.polygonMode )
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

			if ( state.depthClampEnable != save.depthClampEnable )
			{
				if ( state.depthClampEnable )
				{
					glLogCall( gl::Enable, GL_DEPTH_CLAMP );
				}
				else
				{
					glLogCall( gl::Disable, GL_DEPTH_CLAMP );
				}
			}

			if ( state.rasteriserDiscardEnable != save.rasteriserDiscardEnable )
			{
				if ( state.rasteriserDiscardEnable )
				{
					glLogCall( gl::Enable, GL_RASTERIZER_DISCARD );
				}
				else
				{
					glLogCall( gl::Disable, GL_RASTERIZER_DISCARD );
				}
			}

			if ( state.polygonMode == renderer::PolygonMode::eLine
				&& !dynamicLineWidth )
			{
				glLogCall( gl::LineWidth, state.lineWidth );
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
			if ( state.rasterisationSamples != renderer::SampleCountFlag::e1 )
			{
				glLogCall( gl::Enable, GL_MULTISAMPLE );

				if ( state.alphaToCoverageEnable != save.alphaToCoverageEnable )
				{
					if ( state.alphaToCoverageEnable )
					{
						glLogCall( gl::Enable, GL_SAMPLE_ALPHA_TO_COVERAGE );
					}
					else
					{
						glLogCall( gl::Disable, GL_SAMPLE_ALPHA_TO_COVERAGE );
					}
				}

				if ( state.alphaToOneEnable != save.alphaToOneEnable )
				{
					if ( state.alphaToOneEnable )
					{
						glLogCall( gl::Enable, GL_SAMPLE_ALPHA_TO_ONE );
					}
					else
					{
						glLogCall( gl::Disable, GL_SAMPLE_ALPHA_TO_ONE );
					}
				}

				if ( state.sampleShadingEnable )
				{
					glLogCall( gl::Enable, GL_SAMPLE_SHADING );
					glLogCall( gl::MinSampleShading, state.minSampleShading );
				}
				else
				{
					glLogCall( gl::Disable, GL_SAMPLE_SHADING );
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
			if ( state.depthWriteEnable != save.depthWriteEnable )
			{
				if ( state.depthWriteEnable )
				{
					glLogCall( gl::DepthMask, GL_TRUE );
				}
				else
				{
					glLogCall( gl::DepthMask, GL_FALSE );
				}
			}

			if ( state.depthTestEnable != save.depthTestEnable )
			{
				if ( state.depthTestEnable )
				{
					glLogCall( gl::Enable, GL_DEPTH_TEST );
					glLogCall( gl::DepthFunc, convert( state.depthCompareOp ) );
				}
				else
				{
					glLogCall( gl::Disable, GL_DEPTH_TEST );
				}
			}

			if ( state.stencilTestEnable != save.stencilTestEnable
				|| ( state.stencilTestEnable
					&& ( state.back != save.back
						|| state.front != save.front ) ) )
			{
				if ( state.stencilTestEnable )
				{
					glLogCall( gl::Enable, GL_STENCIL_TEST );

					glLogCall( gl::StencilMaskSeparate
						, GL_CULL_MODE_BACK
						, state.back.writeMask );
					glLogCall( gl::StencilFuncSeparate
						, GL_CULL_MODE_BACK
						, convert( state.back.compareOp )
						, state.back.reference
						, state.back.compareMask );
					glLogCall( gl::StencilOpSeparate
						, GL_CULL_MODE_BACK
						, convert( state.back.failOp )
						, convert( state.back.depthFailOp )
						, convert( state.back.passOp ) );
					glLogCall( gl::StencilMaskSeparate
						, GL_CULL_MODE_FRONT
						, state.front.writeMask );
					glLogCall( gl::StencilFuncSeparate
						, GL_CULL_MODE_FRONT
						, convert( state.front.compareOp )
						, state.front.reference
						, state.front.compareMask );
					glLogCall( gl::StencilOpSeparate
						, GL_CULL_MODE_FRONT
						, convert( state.front.failOp )
						, convert( state.front.depthFailOp )
						, convert( state.front.passOp ) );
				}
				else
				{
					glLogCall( gl::Disable, GL_STENCIL_TEST );
				}
			}

			if ( state.depthBoundsTestEnable != save.depthBoundsTestEnable
				|| ( state.depthBoundsTestEnable
					&& ( state.minDepthBounds != save.minDepthBounds
						|| state.maxDepthBounds != save.maxDepthBounds ) ) )
			{
				if ( state.depthBoundsTestEnable )
				{
					glLogCall( gl::Enable, GL_DEPTH_CLAMP );
					glLogCall( gl::DepthRange, state.minDepthBounds, state.maxDepthBounds );
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
			if ( state.patchControlPoints )
			{
				glLogCall( gl::PatchParameteri, GL_PATCH_VERTICES, int( state.patchControlPoints ) );
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
				, state.getOffset().x
				, state.getOffset().y
				, state.getSize().width
				, state.getSize().height );
		}
	}

	void apply( Device const & device
		, renderer::Scissor const & state )
	{
		auto & save = device.getCurrentScissor();

		if ( state != save )
		{
			glLogCall( gl::Scissor
				, state.getOffset().x
				, state.getOffset().y
				, state.getSize().width
				, state.getSize().height );
		}
	}

	BindPipelineCommand::BindPipelineCommand( Device const & device
		, renderer::Pipeline const & pipeline
		, renderer::PipelineBindPoint bindingPoint )
		: m_device{ device }
		, m_pipeline{ static_cast< Pipeline const & > ( pipeline ) }
		, m_layout{ static_cast< PipelineLayout const & > ( m_pipeline.getLayout() ) }
		, m_program{ m_pipeline.getProgram() }
		, m_bindingPoint{ bindingPoint }
		, m_dynamicLineWidth{ m_pipeline.hasDynamicState( renderer::DynamicState::eLineWidth ) }
		, m_dynamicDepthBias{ m_pipeline.hasDynamicState( renderer::DynamicState::eDepthBias ) }
		, m_dynamicScissor{ m_pipeline.hasDynamicState( renderer::DynamicState::eScissor ) }
		, m_dynamicViewport{ m_pipeline.hasDynamicState( renderer::DynamicState::eViewport ) }
	{
	}

	void BindPipelineCommand::apply()const
	{
		glLogCommand( "BindPipelineCommand" );
		gl_renderer::apply( m_device, m_pipeline.getInputAssemblyState() );
		gl_renderer::apply( m_device, m_pipeline.getColourBlendState() );
		gl_renderer::apply( m_device, m_pipeline.getRasterisationState()
			, m_dynamicLineWidth
			, m_dynamicDepthBias );
		gl_renderer::apply( m_device, m_pipeline.getDepthStencilState() );
		gl_renderer::apply( m_device, m_pipeline.getMultisampleState() );
		gl_renderer::apply( m_device, m_pipeline.getTessellationState() );

		if ( !m_dynamicViewport )
		{
			assert( m_pipeline.hasViewport() );
			gl_renderer::apply( m_device, m_pipeline.getViewport() );
		}

		if ( !m_dynamicScissor )
		{
			assert( m_pipeline.hasScissor() );
			gl_renderer::apply( m_device, m_pipeline.getScissor() );
		}

		auto & save = m_device.getCurrentProgram();

		if ( m_program != save )
		{
			glLogCall( gl::UseProgram, m_program );
			save = m_program;
		}
	}

	CommandPtr BindPipelineCommand::clone()const
	{
		return std::make_unique< BindPipelineCommand >( *this );
	}
}
