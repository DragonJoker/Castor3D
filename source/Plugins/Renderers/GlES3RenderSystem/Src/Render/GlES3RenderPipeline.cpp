#include "Render/GlES3RenderPipeline.hpp"

#include "Common/OpenGlES3.hpp"
#include "Render/GlES3RenderSystem.hpp"

#include <Log/Logger.hpp>
#include <Shader/ShaderProgram.hpp>
#include <Shader/UniformBufferBinding.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlES3Render
{
	namespace
	{
		void DoApply( BlendState const p_state, OpenGlES3 const & p_gl )
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
						p_gl.BlendFunc( i, GlES3BlendFactor::eOne, GlES3BlendFactor::eZero, GlES3BlendFactor::eOne, GlES3BlendFactor::eZero );
						p_gl.BlendEquation( i, GlES3BlendOp::eAdd );
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
					p_gl.BlendFunc( GlES3BlendFactor::eOne, GlES3BlendFactor::eZero, GlES3BlendFactor::eOne, GlES3BlendFactor::eZero );
					p_gl.BlendEquation( GlES3BlendOp::eAdd );
				}
			}

			if ( l_enabled )
			{
				p_gl.BlendColor( p_state.GetBlendFactors() );
				p_gl.Enable( GlES3Tweak::eBlend );
			}
			else
			{
				p_gl.Disable( GlES3Tweak::eBlend );
			}
		}

		void DoApply( DepthStencilState const & p_state, OpenGlES3 const & p_gl )
		{
			p_gl.DepthMask( p_gl.Get( p_state.GetDepthMask() ) );

			if ( p_state.GetDepthTest() )
			{
				p_gl.Enable( GlES3Tweak::eDepthTest );
				p_gl.DepthFunc( p_gl.Get( p_state.GetDepthFunc() ) );
			}
			else
			{
				p_gl.Disable( GlES3Tweak::eDepthTest );
			}

			if ( p_state.GetStencilTest() )
			{
				p_gl.Enable( GlES3Tweak::eStencilTest );
				p_gl.StencilMaskSeparate( GlES3Face::eBoth, p_state.GetStencilWriteMask() );
				p_gl.StencilFuncSeparate( GlES3Face::eBack, p_gl.Get( p_state.GetStencilBackFunc() ), p_state.GetStencilBackRef(), p_state.GetStencilReadMask() );
				p_gl.StencilFuncSeparate( GlES3Face::eFront, p_gl.Get( p_state.GetStencilFrontFunc() ), p_state.GetStencilFrontRef(), p_state.GetStencilReadMask() );
				p_gl.StencilOpSeparate( GlES3Face::eBack, p_gl.Get( p_state.GetStencilBackFailOp() ), p_gl.Get( p_state.GetStencilBackDepthFailOp() ), p_gl.Get( p_state.GetStencilBackPassOp() ) );
				p_gl.StencilOpSeparate( GlES3Face::eFront, p_gl.Get( p_state.GetStencilFrontFailOp() ), p_gl.Get( p_state.GetStencilFrontDepthFailOp() ), p_gl.Get( p_state.GetStencilFrontPassOp() ) );
			}
			else
			{
				p_gl.Disable( GlES3Tweak::eStencilTest );
			}
		}

		void DoApply( MultisampleState const & p_state, OpenGlES3 const & p_gl )
		{
			if ( p_state.GetMultisample() )
			{
				p_gl.Enable( GlES3Tweak::eMultisample );
			}
			else
			{
				p_gl.Disable( GlES3Tweak::eMultisample );
			}

			if ( p_state.IsAlphaToCoverageEnabled() )
			{
				p_gl.Enable( GlES3Tweak::eAlphaToCoverage );
			}
			else
			{
				p_gl.Disable( GlES3Tweak::eAlphaToCoverage );
			}
		}

		void DoApply( RasteriserState const & p_state, OpenGlES3 const & p_gl )
		{
			p_gl.PolygonMode( GlES3Face::eBoth, p_gl.Get( p_state.GetFillMode() ) );

			if ( p_state.GetCulledFaces() != Culling::eNone )
			{
				p_gl.Enable( GlES3Tweak::eCullFace );
				p_gl.CullFace( p_gl.Get( p_state.GetCulledFaces() ) );

				if ( p_state.GetFrontCCW() )
				{
					p_gl.FrontFace( GlES3FrontFaceDirection::eCounterClockWise );
				}
				else
				{
					p_gl.FrontFace( GlES3FrontFaceDirection::eClockWise );
				}
			}
			else
			{
				p_gl.Disable( GlES3Tweak::eCullFace );
			}

			if ( p_state.GetAntialiasedLines() )
			{
				p_gl.Enable( GlES3Tweak::eLineSmooth );
				p_gl.Hint( GlES3Hint::eLineSmooth, GlES3HintValue::eNicest );
			}
			else
			{
				p_gl.Disable( GlES3Tweak::eLineSmooth );
				p_gl.Hint( GlES3Hint::eLineSmooth, GlES3HintValue::eDontCare );
			}

			if ( p_state.GetScissor() )
			{
				p_gl.Enable( GlES3Tweak::eScissorTest );
			}
			else
			{
				p_gl.Disable( GlES3Tweak::eScissorTest );
			}

			if ( p_state.GetDepthClipping() )
			{
				p_gl.Disable( GlES3Tweak::eDepthClamp );
			}
			else
			{
				p_gl.Enable( GlES3Tweak::eDepthClamp );
			}

			if ( p_state.GetDepthBiasFactor() != 0 || p_state.GetDepthBiasUnits() != 0 )
			{
				p_gl.Enable( GlES3Tweak::eOffsetFill );
				p_gl.PolygonOffset( p_state.GetDepthBiasFactor(), p_state.GetDepthBiasUnits() );
			}
			else
			{
				p_gl.Disable( GlES3Tweak::eOffsetFill );
			}

			if ( p_state.GetDiscardPrimitives() )
			{
				p_gl.Enable( GlES3Tweak::eRasterizerDiscard );
			}
			else
			{
				p_gl.Disable( GlES3Tweak::eRasterizerDiscard );
			}
		}
	}

	GlES3RenderPipeline::GlES3RenderPipeline( OpenGlES3 & p_gl
		, GlES3RenderSystem & p_renderSystem
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

	GlES3RenderPipeline::~GlES3RenderPipeline()
	{
	}

	void GlES3RenderPipeline::Apply()const
	{
		DoApply( m_rsState, GetOpenGlES3() );
		DoApply( m_dsState, GetOpenGlES3() );
		DoApply( m_blState, GetOpenGlES3() );
		DoApply( m_msState, GetOpenGlES3() );
		m_program.Bind();
		auto l_count{ 0u };

		for ( auto & l_binding : m_bindings )
		{
			l_binding.get().Bind( l_count++ );
		}
	}
}
