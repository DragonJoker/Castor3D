#include "Render/GlRenderPipeline.hpp"

#include "Common/OpenGl.hpp"
#include "Render/GlRenderSystem.hpp"

#include <Log/Logger.hpp>
#include <Shader/ShaderProgram.hpp>
#include <Shader/UniformBufferBinding.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	namespace
	{
		void DoApply( BlendState const p_state, OpenGl const & p_gl )
		{
			bool l_enabled{ false };

			p_gl.ColorMask( p_gl.Get( p_state.GetColourMaskR() )
							, p_gl.Get( p_state.GetColourMaskG() )
							, p_gl.Get( p_state.GetColourMaskB() )
							, p_gl.Get( p_state.GetColourMaskA() ) );

			if ( p_state.IsIndependantBlendEnabled() )
			{
				for ( int i = 0; i < 8; i++ )
				{
					if ( p_state.IsBlendEnabled( i ) )
					{
						l_enabled = true;
						p_gl.BlendFunc( i
							, p_gl.Get( p_state.GetRgbSrcBlend( i ) )
							, p_gl.Get( p_state.GetRgbDstBlend( i ) )
							, p_gl.Get( p_state.GetAlphaSrcBlend( i ) )
							, p_gl.Get( p_state.GetAlphaDstBlend( i ) ) );
						p_gl.BlendEquation( i, p_gl.Get( p_state.GetRgbBlendOp( i ) ) );
					}
					else
					{
						p_gl.BlendFunc( i, GlBlendFactor::eOne, GlBlendFactor::eZero, GlBlendFactor::eOne, GlBlendFactor::eZero );
						p_gl.BlendEquation( i, GlBlendOp::eAdd );
					}
				}
			}
			else
			{
				if ( p_state.IsBlendEnabled( 0 ) )
				{
					l_enabled = true;
					p_gl.BlendFunc( p_gl.Get( p_state.GetRgbSrcBlend() )
						, p_gl.Get( p_state.GetRgbDstBlend() )
						, p_gl.Get( p_state.GetAlphaSrcBlend() )
						, p_gl.Get( p_state.GetAlphaDstBlend() ) );
					p_gl.BlendEquation( p_gl.Get( p_state.GetRgbBlendOp() ) );
				}
				else
				{
					p_gl.BlendFunc( GlBlendFactor::eOne, GlBlendFactor::eZero, GlBlendFactor::eOne, GlBlendFactor::eZero );
					p_gl.BlendEquation( GlBlendOp::eAdd );
				}
			}

			if ( l_enabled )
			{
				p_gl.BlendColor( p_state.GetBlendFactors() );
				p_gl.Enable( GlTweak::eBlend );
			}
			else
			{
				p_gl.Disable( GlTweak::eBlend );
			}
		}

		void DoApply( DepthStencilState const & p_state, OpenGl const & p_gl )
		{
			p_gl.DepthMask( p_gl.Get( p_state.GetDepthMask() ) );

			if ( p_state.GetDepthTest() )
			{
				p_gl.Enable( GlTweak::eDepthTest );
				p_gl.DepthFunc( p_gl.Get( p_state.GetDepthFunc() ) );
			}
			else
			{
				p_gl.Disable( GlTweak::eDepthTest );
			}

			if ( p_state.GetStencilTest() )
			{
				p_gl.Enable( GlTweak::eStencilTest );
				p_gl.StencilMaskSeparate( GlFace::eBoth, p_state.GetStencilWriteMask() );
				p_gl.StencilFuncSeparate( GlFace::eBack, p_gl.Get( p_state.GetStencilBackFunc() ), p_state.GetStencilBackRef(), p_state.GetStencilReadMask() );
				p_gl.StencilFuncSeparate( GlFace::eFront, p_gl.Get( p_state.GetStencilFrontFunc() ), p_state.GetStencilFrontRef(), p_state.GetStencilReadMask() );
				p_gl.StencilOpSeparate( GlFace::eBack, p_gl.Get( p_state.GetStencilBackFailOp() ), p_gl.Get( p_state.GetStencilBackDepthFailOp() ), p_gl.Get( p_state.GetStencilBackPassOp() ) );
				p_gl.StencilOpSeparate( GlFace::eFront, p_gl.Get( p_state.GetStencilFrontFailOp() ), p_gl.Get( p_state.GetStencilFrontDepthFailOp() ), p_gl.Get( p_state.GetStencilFrontPassOp() ) );
			}
			else
			{
				p_gl.Disable( GlTweak::eStencilTest );
			}
		}

		void DoApply( MultisampleState const & p_state, OpenGl const & p_gl )
		{
			if ( p_state.GetMultisample() )
			{
				p_gl.Enable( GlTweak::eMultisample );
			}
			else
			{
				p_gl.Disable( GlTweak::eMultisample );
			}

			if ( p_state.IsAlphaToCoverageEnabled() )
			{
				p_gl.Enable( GlTweak::eAlphaToCoverage );
			}
			else
			{
				p_gl.Disable( GlTweak::eAlphaToCoverage );
			}
		}

		void DoApply( RasteriserState const & p_state, OpenGl const & p_gl )
		{
			p_gl.PolygonMode( GlFace::eBoth, p_gl.Get( p_state.GetFillMode() ) );

			if ( p_state.GetCulledFaces() != Culling::eNone )
			{
				p_gl.Enable( GlTweak::eCullFace );
				p_gl.CullFace( p_gl.Get( p_state.GetCulledFaces() ) );

				if ( p_state.GetFrontCCW() )
				{
					p_gl.FrontFace( GlFrontFaceDirection::eCounterClockWise );
				}
				else
				{
					p_gl.FrontFace( GlFrontFaceDirection::eClockWise );
				}
			}
			else
			{
				p_gl.Disable( GlTweak::eCullFace );
			}

			if ( p_state.GetAntialiasedLines() )
			{
				p_gl.Enable( GlTweak::eLineSmooth );
				p_gl.Hint( GlHint::eLineSmooth, GlHintValue::eNicest );
			}
			else
			{
				p_gl.Disable( GlTweak::eLineSmooth );
				p_gl.Hint( GlHint::eLineSmooth, GlHintValue::eDontCare );
			}

			if ( p_state.GetScissor() )
			{
				p_gl.Enable( GlTweak::eScissorTest );
			}
			else
			{
				p_gl.Disable( GlTweak::eScissorTest );
			}

			if ( p_state.GetDepthClipping() )
			{
				p_gl.Disable( GlTweak::eDepthClamp );
			}
			else
			{
				p_gl.Enable( GlTweak::eDepthClamp );
			}

			if ( p_state.GetDepthBiasFactor() != 0 || p_state.GetDepthBiasUnits() != 0 )
			{
				p_gl.Enable( GlTweak::eOffsetFill );
				p_gl.PolygonOffset( p_state.GetDepthBiasFactor(), p_state.GetDepthBiasUnits() );
			}
			else
			{
				p_gl.Disable( GlTweak::eOffsetFill );
			}

			if ( p_state.GetDiscardPrimitives() )
			{
				p_gl.Enable( GlTweak::eRasterizerDiscard );
			}
			else
			{
				p_gl.Disable( GlTweak::eRasterizerDiscard );
			}
		}
	}

	GlRenderPipeline::GlRenderPipeline( OpenGl & p_gl
		, GlRenderSystem & p_renderSystem
		, DepthStencilState && p_dsState
		, RasteriserState && p_rsState
		, BlendState && p_bdState
		, MultisampleState && p_msState
		, ShaderProgram & p_program
		, PipelineFlags const & p_flags )
		: RenderPipeline{ p_renderSystem, std::move( p_dsState ), std::move( p_rsState ), std::move( p_bdState ), std::move( p_msState ), p_program, p_flags }
		, Holder{ p_gl }
	{
	}

	GlRenderPipeline::~GlRenderPipeline()
	{
	}

	void GlRenderPipeline::Apply()const
	{
		DoApply( m_rsState, GetOpenGl() );
		DoApply( m_dsState, GetOpenGl() );
		DoApply( m_blState, GetOpenGl() );
		DoApply( m_msState, GetOpenGl() );
		m_program.Bind();
		auto l_count{ 0u };

		for ( auto & l_binding : m_bindings )
		{
			l_binding.get().Bind( l_count++ );
		}
	}
}
