#include "Render/GlRenderPipeline.hpp"

#include "Common/OpenGl.hpp"
#include "Render/GlRenderSystem.hpp"

#include <Log/Logger.hpp>
#include <Shader/ShaderProgram.hpp>
#include <Shader/UniformBufferBinding.hpp>

#define CHECK_GL_STATES 0

using namespace Castor3D;
using namespace Castor;

#if CHECK_GL_STATES

namespace Castor3D
{
	OutputStream & operator<<( OutputStream & p_stream, DepthStencilState const & p_state )
	{
		static std::map< WritingMask, String > const depthMasks
		{
			{ WritingMask::eAll, "1" },
			{ WritingMask::eZero, "0" },
		};
		static std::map< DepthFunc, String > const depthFuncs
		{
			{ DepthFunc::eNever, "Nv" },
			{ DepthFunc::eLess, "Ls" },
			{ DepthFunc::eEqual, "Eq" },
			{ DepthFunc::eLEqual, "Le" },
			{ DepthFunc::eGreater, "Gr" },
			{ DepthFunc::eNEqual, "Ne" },
			{ DepthFunc::eGEqual, "Ge" },
			{ DepthFunc::eAlways, "Al" },
		};
		static std::map< StencilFunc, String > const stencilFuncs
		{
			{ StencilFunc::eNever, "Nv" },
			{ StencilFunc::eLess, "Ls" },
			{ StencilFunc::eEqual, "Eq" },
			{ StencilFunc::eLEqual, "Le" },
			{ StencilFunc::eGreater, "Gr" },
			{ StencilFunc::eNEqual, "Ne" },
			{ StencilFunc::eGEqual, "Ge" },
			{ StencilFunc::eAlways, "Al" },
		};
		static std::map< StencilOp, String > const stencilOps
		{
			{ StencilOp::eKeep, "Keep" },
			{ StencilOp::eZero, "Zero" },
			{ StencilOp::eReplace, "Repl" },
			{ StencilOp::eIncrement, "Incr" },
			{ StencilOp::eIncrWrap, "IncW" },
			{ StencilOp::eDecrement, "Decr" },
			{ StencilOp::eDecrWrap, "DecW" },
			{ StencilOp::eInvert, "Invt" },
		};
		p_stream << cuT( "Depth - mask: " ) << depthMasks.at( p_state.GetDepthMask() )
			<< cuT( ", test: " ) << ( p_state.GetDepthTest() ? "1" : "0" );

		if ( p_state.GetDepthTest() )
		{
			p_stream << cuT( ", func: " ) << depthFuncs.at( p_state.GetDepthFunc() );
		}

		p_stream << cuT( ", Stencil - test: " ) << ( p_state.GetStencilTest() ? "1" : "0" );

		if ( p_state.GetDepthTest() )
		{
			p_stream << cuT( ", Back - ref: " ) << p_state.GetStencilBackRef()
				<< cuT( ", func: " ) << stencilFuncs.at( p_state.GetStencilBackFunc() )
				<< cuT( ", fail op: " ) << stencilOps.at( p_state.GetStencilBackFailOp() )
				<< cuT( ", depth fail op: " ) << stencilOps.at( p_state.GetStencilBackDepthFailOp() )
				<< cuT( ", pass op: " ) << stencilOps.at( p_state.GetStencilBackPassOp() )
				<< cuT( " - Front ref: " ) << p_state.GetStencilFrontRef()
				<< cuT( ", func: " ) << stencilFuncs.at( p_state.GetStencilFrontFunc() )
				<< cuT( ", fail op: " ) << stencilOps.at( p_state.GetStencilFrontFailOp() )
				<< cuT( ", depth fail op: " ) << stencilOps.at( p_state.GetStencilFrontDepthFailOp() )
				<< cuT( ", pass op: " ) << stencilOps.at( p_state.GetStencilFrontPassOp() );
		}

		return p_stream;
	}
}

#endif

namespace GlRender
{
	namespace
	{
#if CHECK_GL_STATES

		void DoLoad( DepthStencilState & p_state )
		{
			static GLint const GL_INCR_WRAP = 0x8507;
			static GLint const GL_DECR_WRAP = 0x8508;
			static GLint const GL_STENCIL_BACK_REF = 0x8CA3;
			static GLint const GL_STENCIL_BACK_FUNC = 0x8800;
			static GLint const GL_STENCIL_BACK_FAIL = 0x8801;
			static GLint const GL_STENCIL_BACK_PASS_DEPTH_FAIL = 0x8802;
			static GLint const GL_STENCIL_BACK_PASS_DEPTH_PASS = 0x8803;
			static std::map< GLint, DepthFunc > const depthFuncs
			{
				{ GL_NEVER, DepthFunc::eNever },
				{ GL_LESS, DepthFunc::eLess },
				{ GL_EQUAL, DepthFunc::eEqual },
				{ GL_LEQUAL, DepthFunc::eLEqual },
				{ GL_GREATER, DepthFunc::eGreater },
				{ GL_NOTEQUAL, DepthFunc::eNEqual },
				{ GL_GEQUAL, DepthFunc::eGEqual },
				{ GL_ALWAYS, DepthFunc::eAlways },
			};
			static std::map< GLint, StencilFunc > const stencilFuncs
			{
				{ GL_NEVER, StencilFunc::eNever },
				{ GL_LESS, StencilFunc::eLess },
				{ GL_EQUAL, StencilFunc::eEqual },
				{ GL_LEQUAL, StencilFunc::eLEqual },
				{ GL_GREATER, StencilFunc::eGreater },
				{ GL_NOTEQUAL, StencilFunc::eNEqual },
				{ GL_GEQUAL, StencilFunc::eGEqual },
				{ GL_ALWAYS, StencilFunc::eAlways },
			};
			static std::map< GLint, StencilOp > const stencilOps
			{
				{ GL_KEEP, StencilOp::eKeep },
				{ GL_ZERO, StencilOp::eZero },
				{ GL_REPLACE, StencilOp::eReplace },
				{ GL_INCR, StencilOp::eIncrement },
				{ GL_INCR_WRAP, StencilOp::eIncrWrap },
				{ GL_DECR, StencilOp::eDecrement },
				{ GL_DECR_WRAP, StencilOp::eDecrWrap },
				{ GL_INVERT, StencilOp::eInvert },
			};
			GLint value;
			glGetIntegerv( GL_DEPTH_TEST, &value );
			p_state.SetDepthTest( value != 0 );
			glGetIntegerv( GL_DEPTH_WRITEMASK, &value );
			p_state.SetDepthMask( value ? WritingMask::eAll : WritingMask::eZero );
			glGetIntegerv( GL_DEPTH_FUNC, &value );
			p_state.SetDepthFunc( depthFuncs.at( value ) );
			glGetIntegerv( GL_STENCIL_TEST, &value );
			p_state.SetStencilTest( value != 0 );
			glGetIntegerv( GL_STENCIL_BACK_REF, &value );
			p_state.SetStencilBackRef( value );
			glGetIntegerv( GL_STENCIL_BACK_FUNC, &value );
			p_state.SetStencilBackFunc( stencilFuncs.at( value ) );
			glGetIntegerv( GL_STENCIL_BACK_FAIL, &value );
			p_state.SetStencilBackFailOp( stencilOps.at( value ) );
			glGetIntegerv( GL_STENCIL_BACK_PASS_DEPTH_FAIL, &value );
			p_state.SetStencilBackDepthFailOp( stencilOps.at( value ) );
			glGetIntegerv( GL_STENCIL_BACK_PASS_DEPTH_PASS, &value );
			p_state.SetStencilBackPassOp( stencilOps.at( value ) );
			glGetIntegerv( GL_STENCIL_REF, &value );
			p_state.SetStencilFrontRef( value );
			glGetIntegerv( GL_STENCIL_FUNC, &value );
			p_state.SetStencilFrontFunc( stencilFuncs.at( value ) );
			glGetIntegerv( GL_STENCIL_FAIL, &value );
			p_state.SetStencilFrontFailOp( stencilOps.at( value ) );
			glGetIntegerv( GL_STENCIL_PASS_DEPTH_FAIL, &value );
			p_state.SetStencilFrontDepthFailOp( stencilOps.at( value ) );
			glGetIntegerv( GL_STENCIL_PASS_DEPTH_PASS, &value );
			p_state.SetStencilFrontPassOp( stencilOps.at( value ) );
		}

		bool DoCompare( DepthStencilState const & p_gl
			, DepthStencilState const & p_save
			, String const & p_stack )
		{
			bool result = p_gl.GetDepthTest() == p_save.GetDepthTest()
				&& p_gl.GetDepthMask() == p_save.GetDepthMask()
				&& ( !p_gl.GetDepthTest()
					|| p_gl.GetDepthFunc() == p_save.GetDepthFunc() )
				&& p_gl.GetStencilTest() == p_save.GetStencilTest()
				&& ( !p_gl.GetStencilTest()
					|| ( p_gl.GetStencilBackRef() == p_save.GetStencilBackRef()
						&& p_gl.GetStencilBackFunc() == p_save.GetStencilBackFunc()
						&& p_gl.GetStencilBackFailOp() == p_save.GetStencilBackFailOp()
						&& p_gl.GetStencilBackDepthFailOp() == p_save.GetStencilBackDepthFailOp()
						&& p_gl.GetStencilBackPassOp() == p_save.GetStencilBackPassOp()
						&& p_gl.GetStencilFrontRef() == p_save.GetStencilFrontRef()
						&& p_gl.GetStencilFrontFunc() == p_save.GetStencilFrontFunc()
						&& p_gl.GetStencilFrontFailOp() == p_save.GetStencilFrontFailOp()
						&& p_gl.GetStencilFrontDepthFailOp() == p_save.GetStencilFrontDepthFailOp()
						&& p_gl.GetStencilFrontPassOp() == p_save.GetStencilFrontPassOp() ) );

			if ( !result )
			{
				Logger::LogDebug( StringStream{} << cuT( "DepthStencilState comparison failed" ) );
				Logger::LogDebug( StringStream{} << cuT( "GL  :\n" ) << p_gl );
				Logger::LogDebug( StringStream{} << cuT( "SAVE:\n" ) << p_save );
				Logger::LogDebug( p_stack );
			}

			return result;
		}

		struct StateCheck
		{
			StateCheck( DepthStencilState const & p_state )
			{
				DepthStencilState gl;
				DoLoad( gl );
				REQUIRE( DoCompare( gl, save, stack ) );
				save = p_state;
				StringStream stream;
				stream << Debug::Backtrace{};
				stack = stream.str();
			}

			~StateCheck()
			{
				DepthStencilState gl;
				DoLoad( gl );
				REQUIRE( DoCompare( gl, save, stack ) );
			}

			static DepthStencilState save;
			static String stack;
		};

		DepthStencilState StateCheck::save;
		String StateCheck::stack;

#endif
		
		template< typename StateType >
		uint64_t DoEncode( StateType const & p_state );

		template<>
		uint64_t DoEncode< DepthStencilState >( DepthStencilState const & p_state )
		{
			return ( uint64_t( p_state.GetDepthTest() ? 1u : 0u ) << 63 )
				| ( uint64_t( p_state.GetDepthMask() == WritingMask::eMax ? 1u : 0u ) << 62 )
				| ( uint64_t( p_state.GetStencilTest() ? 1u : 0u ) << 61 )
				| ( uint64_t( uint8_t( p_state.GetStencilWriteMask() ) ) << 53 )
				| ( uint64_t( uint8_t( p_state.GetStencilReadMask() ) ) << 45 )
				| ( uint64_t( uint8_t( p_state.GetStencilBackFunc() ) ) << 42 )
				| ( uint64_t( uint8_t( p_state.GetStencilBackRef() ) ) << 34 )
				| ( uint64_t( uint8_t( p_state.GetStencilBackFailOp() ) ) << 31 )
				| ( uint64_t( uint8_t( p_state.GetStencilBackDepthFailOp() ) ) << 28 )
				| ( uint64_t( uint8_t( p_state.GetStencilBackPassOp() ) ) << 25 )
				| ( uint64_t( uint8_t( p_state.GetStencilFrontFunc() ) ) << 22 )
				| ( uint64_t( uint8_t( p_state.GetStencilFrontRef() ) ) << 14 )
				| ( uint64_t( uint8_t( p_state.GetStencilFrontFailOp() ) ) << 11 )
				| ( uint64_t( uint8_t( p_state.GetStencilFrontDepthFailOp() ) ) << 8 )
				| ( uint64_t( uint8_t( p_state.GetStencilFrontPassOp() ) ) << 5 );
			
		}
		
		void DoApply( BlendState const p_state, OpenGl const & p_gl )
		{
			bool enabled{ false };

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
						enabled = true;
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
					enabled = true;
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

			if ( enabled )
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
#if CHECK_GL_STATES

			StateCheck check( p_state );

#endif
			static uint64_t save = 0ull;
			uint64_t current = DoEncode( p_state );

			if ( current != save )
			{
				save = current;
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
		uint32_t count{ 0u };

		for ( auto & binding : m_bindings )
		{
			binding.get().Bind( binding.get().GetOwner()->GetBindingPoint() );
		}
	}
}
