#include "Render/GlPipeline.hpp"

#include "Common/OpenGl.hpp"
#include "Render/GlRenderSystem.hpp"

#include <Log/Logger.hpp>
#include <Shader/ShaderProgram.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	namespace
	{
		bool DoApply( BlendState const p_state, OpenGl const & p_gl )
		{
			bool l_return{ true };
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
						l_return &= p_gl.BlendFunc( i
													, p_gl.Get( p_state.GetRgbSrcBlend( i ) )
													, p_gl.Get( p_state.GetRgbDstBlend( i ) )
													, p_gl.Get( p_state.GetAlphaSrcBlend( i ) )
													, p_gl.Get( p_state.GetAlphaDstBlend( i ) ) );
						l_return &= p_gl.BlendEquation( i, p_gl.Get( p_state.GetRgbBlendOp( i ) ) );
					}
					else
					{
						l_return &= p_gl.BlendFunc( i, eGL_BLEND_FACTOR_ONE, eGL_BLEND_FACTOR_ZERO, eGL_BLEND_FACTOR_ONE, eGL_BLEND_FACTOR_ZERO );
						l_return &= p_gl.BlendEquation( i, eGL_BLEND_OP_ADD );
					}
				}
			}
			else
			{
				if ( p_state.IsBlendEnabled( 0 ) )
				{
					l_enabled = true;
					l_return &= p_gl.BlendFunc( p_gl.Get( p_state.GetRgbSrcBlend() )
												, p_gl.Get( p_state.GetRgbDstBlend() )
												, p_gl.Get( p_state.GetAlphaSrcBlend() )
												, p_gl.Get( p_state.GetAlphaDstBlend() ) );
					l_return &= p_gl.BlendEquation( p_gl.Get( p_state.GetRgbBlendOp() ) );
				}
				else
				{
					l_return &= p_gl.BlendFunc( eGL_BLEND_FACTOR_ONE, eGL_BLEND_FACTOR_ZERO, eGL_BLEND_FACTOR_ONE, eGL_BLEND_FACTOR_ZERO );
					l_return &= p_gl.BlendEquation( eGL_BLEND_OP_ADD );
				}
			}

			if ( l_enabled )
			{
				l_return &= p_gl.BlendColor( p_state.GetBlendFactors() );
				l_return &= p_gl.Enable( eGL_TWEAK_BLEND );
			}
			else
			{
				l_return &= p_gl.Disable( eGL_TWEAK_BLEND );
			}

			return l_return;
		}

		bool DoApply( DepthStencilState const & p_state, OpenGl const & p_gl )
		{
			bool l_return = true;
			p_gl.DepthMask( p_gl.Get( p_state.GetDepthMask() ) );

			if ( p_state.GetDepthTest() )
			{
				l_return &= p_gl.Enable( eGL_TWEAK_DEPTH_TEST );
				l_return &= p_gl.DepthFunc( p_gl.Get( p_state.GetDepthFunc() ) );
			}
			else
			{
				l_return &= p_gl.Disable( eGL_TWEAK_DEPTH_TEST );
			}

			if ( p_state.GetStencilTest() )
			{
				l_return &= p_gl.Enable( eGL_TWEAK_STENCIL_TEST );
				l_return &= p_gl.StencilMaskSeparate( eGL_FACE_FRONT_AND_BACK, p_state.GetStencilWriteMask() );
				l_return &= p_gl.StencilFuncSeparate( eGL_FACE_BACK, p_gl.Get( p_state.GetStencilBackFunc() ), p_state.GetStencilBackRef(), p_state.GetStencilReadMask() );
				l_return &= p_gl.StencilFuncSeparate( eGL_FACE_FRONT, p_gl.Get( p_state.GetStencilFrontFunc() ), p_state.GetStencilFrontRef(), p_state.GetStencilReadMask() );
				l_return &= p_gl.StencilOpSeparate( eGL_FACE_BACK, p_gl.Get( p_state.GetStencilBackFailOp() ), p_gl.Get( p_state.GetStencilBackDepthFailOp() ), p_gl.Get( p_state.GetStencilBackPassOp() ) );
				l_return &= p_gl.StencilOpSeparate( eGL_FACE_FRONT, p_gl.Get( p_state.GetStencilFrontFailOp() ), p_gl.Get( p_state.GetStencilFrontDepthFailOp() ), p_gl.Get( p_state.GetStencilFrontPassOp() ) );
			}
			else
			{
				l_return &= p_gl.Disable( eGL_TWEAK_STENCIL_TEST );
			}

			return l_return;
		}

		bool DoApply( MultisampleState const & p_state, OpenGl const & p_gl )
		{
			bool l_return = true;

			if ( p_state.GetMultisample() )
			{
				l_return &= p_gl.Enable( eGL_TWEAK_MULTISAMPLE );
			}
			else
			{
				l_return &= p_gl.Disable( eGL_TWEAK_MULTISAMPLE );
			}

			if ( p_state.IsAlphaToCoverageEnabled() )
			{
				l_return &= p_gl.Enable( eGL_TWEAK_ALPHA_TO_COVERAGE );
			}
			else
			{
				l_return &= p_gl.Disable( eGL_TWEAK_ALPHA_TO_COVERAGE );
			}

			return l_return;
		}

		bool DoApply( RasteriserState const & p_state, OpenGl const & p_gl )
		{
			bool l_return = true;

			l_return &= p_gl.PolygonMode( eGL_FACE_FRONT_AND_BACK, p_gl.Get( p_state.GetFillMode() ) );

			if ( p_state.GetCulledFaces() != Culling::None )
			{
				l_return &= p_gl.Enable( eGL_TWEAK_CULL_FACE );
				l_return &= p_gl.CullFace( p_gl.Get( p_state.GetCulledFaces() ) );

				if ( p_state.GetFrontCCW() )
				{
					l_return &= p_gl.FrontFace( eGL_FRONT_FACE_DIRECTION_CCW );
				}
				else
				{
					l_return &= p_gl.FrontFace( eGL_FRONT_FACE_DIRECTION_CW );
				}
			}
			else
			{
				l_return &= p_gl.Disable( eGL_TWEAK_CULL_FACE );
			}

			if ( p_state.GetAntialiasedLines() )
			{
				l_return &= p_gl.Enable( eGL_TWEAK_LINE_SMOOTH );
				l_return &= p_gl.Hint( eGL_HINT_LINE_SMOOTH, eGL_HINT_VALUE_NICEST );
			}
			else
			{
				l_return &= p_gl.Disable( eGL_TWEAK_LINE_SMOOTH );
				l_return &= p_gl.Hint( eGL_HINT_LINE_SMOOTH, eGL_HINT_VALUE_DONTCARE );
			}

			if ( p_state.GetScissor() )
			{
				l_return &= p_gl.Enable( eGL_TWEAK_SCISSOR_TEST );
			}
			else
			{
				l_return &= p_gl.Disable( eGL_TWEAK_SCISSOR_TEST );
			}

			if ( p_state.GetDepthClipping() )
			{
				l_return &= p_gl.Disable( eGL_TWEAK_DEPTH_CLAMP );
			}
			else
			{
				l_return &= p_gl.Enable( eGL_TWEAK_DEPTH_CLAMP );
			}

			l_return &= p_gl.PolygonOffset( p_state.GetDepthBias(), 4096.0 );
			return l_return;
		}
	}

	GlPipeline::GlPipeline( OpenGl & p_gl
							, GlRenderSystem & p_renderSystem
							, DepthStencilState && p_dsState
							, RasteriserState && p_rsState
							, BlendState && p_bdState
							, MultisampleState && p_msState
							, ShaderProgram & p_program
							, PipelineFlags const & p_flags )
		: Pipeline{ p_renderSystem, std::move( p_dsState ), std::move( p_rsState ), std::move( p_bdState ), std::move( p_msState ), p_program, p_flags }
		, Holder{ p_gl }
	{
	}

	GlPipeline::~GlPipeline()
	{
	}

	void GlPipeline::Apply()const
	{
		DoApply( m_rsState, GetOpenGl() );
		DoApply( m_dsState, GetOpenGl() );
		DoApply( m_blState, GetOpenGl() );
		DoApply( m_msState, GetOpenGl() );
		m_program.Bind( false );
	}
}
