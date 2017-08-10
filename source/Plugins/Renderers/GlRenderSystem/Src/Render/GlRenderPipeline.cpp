#include "Render/GlRenderPipeline.hpp"

#include "Common/OpenGl.hpp"
#include "Render/GlRenderSystem.hpp"

#include <Log/Logger.hpp>
#include <Shader/ShaderProgram.hpp>
#include <Shader/UniformBufferBinding.hpp>

#define CHECK_GL_STATES 0

using namespace castor3d;
using namespace castor;

#if CHECK_GL_STATES

namespace castor3d
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
		p_stream << cuT( "Depth - mask: " ) << depthMasks.at( p_state.getDepthMask() )
			<< cuT( ", test: " ) << ( p_state.getDepthTest() ? "1" : "0" );

		if ( p_state.getDepthTest() )
		{
			p_stream << cuT( ", func: " ) << depthFuncs.at( p_state.getDepthFunc() );
		}

		p_stream << cuT( ", Stencil - test: " ) << ( p_state.getStencilTest() ? "1" : "0" );

		if ( p_state.getDepthTest() )
		{
			p_stream << cuT( ", Back - ref: " ) << p_state.getStencilBackRef()
				<< cuT( ", func: " ) << stencilFuncs.at( p_state.getStencilBackFunc() )
				<< cuT( ", fail op: " ) << stencilOps.at( p_state.getStencilBackFailOp() )
				<< cuT( ", depth fail op: " ) << stencilOps.at( p_state.getStencilBackDepthFailOp() )
				<< cuT( ", pass op: " ) << stencilOps.at( p_state.getStencilBackPassOp() )
				<< cuT( " - Front ref: " ) << p_state.getStencilFrontRef()
				<< cuT( ", func: " ) << stencilFuncs.at( p_state.getStencilFrontFunc() )
				<< cuT( ", fail op: " ) << stencilOps.at( p_state.getStencilFrontFailOp() )
				<< cuT( ", depth fail op: " ) << stencilOps.at( p_state.getStencilFrontDepthFailOp() )
				<< cuT( ", pass op: " ) << stencilOps.at( p_state.getStencilFrontPassOp() );
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

		void doLoad( DepthStencilState & p_state )
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
			glgetIntegerv( GL_DEPTH_TEST, &value );
			p_state.setDepthTest( value != 0 );
			glgetIntegerv( GL_DEPTH_WRITEMASK, &value );
			p_state.setDepthMask( value ? WritingMask::eAll : WritingMask::eZero );
			glgetIntegerv( GL_DEPTH_FUNC, &value );
			p_state.setDepthFunc( depthFuncs.at( value ) );
			glgetIntegerv( GL_STENCIL_TEST, &value );
			p_state.setStencilTest( value != 0 );
			glgetIntegerv( GL_STENCIL_BACK_REF, &value );
			p_state.setStencilBackRef( value );
			glgetIntegerv( GL_STENCIL_BACK_FUNC, &value );
			p_state.setStencilBackFunc( stencilFuncs.at( value ) );
			glgetIntegerv( GL_STENCIL_BACK_FAIL, &value );
			p_state.setStencilBackFailOp( stencilOps.at( value ) );
			glgetIntegerv( GL_STENCIL_BACK_PASS_DEPTH_FAIL, &value );
			p_state.setStencilBackDepthFailOp( stencilOps.at( value ) );
			glgetIntegerv( GL_STENCIL_BACK_PASS_DEPTH_PASS, &value );
			p_state.setStencilBackPassOp( stencilOps.at( value ) );
			glgetIntegerv( GL_STENCIL_REF, &value );
			p_state.setStencilFrontRef( value );
			glgetIntegerv( GL_STENCIL_FUNC, &value );
			p_state.setStencilFrontFunc( stencilFuncs.at( value ) );
			glgetIntegerv( GL_STENCIL_FAIL, &value );
			p_state.setStencilFrontFailOp( stencilOps.at( value ) );
			glgetIntegerv( GL_STENCIL_PASS_DEPTH_FAIL, &value );
			p_state.setStencilFrontDepthFailOp( stencilOps.at( value ) );
			glgetIntegerv( GL_STENCIL_PASS_DEPTH_PASS, &value );
			p_state.setStencilFrontPassOp( stencilOps.at( value ) );
		}

		bool doCompare( DepthStencilState const & p_gl
			, DepthStencilState const & p_save
			, String const & p_stack )
		{
			bool result = p_gl.getDepthTest() == p_save.getDepthTest()
				&& p_gl.getDepthMask() == p_save.getDepthMask()
				&& ( !p_gl.getDepthTest()
					|| p_gl.getDepthFunc() == p_save.getDepthFunc() )
				&& p_gl.getStencilTest() == p_save.getStencilTest()
				&& ( !p_gl.getStencilTest()
					|| ( p_gl.getStencilBackRef() == p_save.getStencilBackRef()
						&& p_gl.getStencilBackFunc() == p_save.getStencilBackFunc()
						&& p_gl.getStencilBackFailOp() == p_save.getStencilBackFailOp()
						&& p_gl.getStencilBackDepthFailOp() == p_save.getStencilBackDepthFailOp()
						&& p_gl.getStencilBackPassOp() == p_save.getStencilBackPassOp()
						&& p_gl.getStencilFrontRef() == p_save.getStencilFrontRef()
						&& p_gl.getStencilFrontFunc() == p_save.getStencilFrontFunc()
						&& p_gl.getStencilFrontFailOp() == p_save.getStencilFrontFailOp()
						&& p_gl.getStencilFrontDepthFailOp() == p_save.getStencilFrontDepthFailOp()
						&& p_gl.getStencilFrontPassOp() == p_save.getStencilFrontPassOp() ) );

			if ( !result )
			{
				Logger::logDebug( StringStream{} << cuT( "DepthStencilState comparison failed" ) );
				Logger::logDebug( StringStream{} << cuT( "GL  :\n" ) << p_gl );
				Logger::logDebug( StringStream{} << cuT( "SAVE:\n" ) << p_save );
				Logger::logDebug( p_stack );
			}

			return result;
		}

		struct StateCheck
		{
			StateCheck( DepthStencilState const & p_state )
			{
				DepthStencilState gl;
				doLoad( gl );
				REQUIRE( doCompare( gl, save, stack ) );
				save = p_state;
				StringStream stream;
				stream << Debug::Backtrace{};
				stack = stream.str();
			}

			~StateCheck()
			{
				DepthStencilState gl;
				doLoad( gl );
				REQUIRE( doCompare( gl, save, stack ) );
			}

			static DepthStencilState save;
			static String stack;
		};

		DepthStencilState StateCheck::save;
		String StateCheck::stack;

#endif
		
		template< typename StateType >
		uint64_t doEncode( StateType const & p_state );

		template<>
		uint64_t doEncode< DepthStencilState >( DepthStencilState const & p_state )
		{
			return ( uint64_t( p_state.getDepthTest() ? 1u : 0u ) << 63 )
				| ( uint64_t( p_state.getDepthMask() == WritingMask::eMax ? 1u : 0u ) << 62 )
				| ( uint64_t( p_state.getStencilTest() ? 1u : 0u ) << 61 )
				| ( uint64_t( uint8_t( p_state.getStencilWriteMask() ) ) << 53 )
				| ( uint64_t( uint8_t( p_state.getStencilReadMask() ) ) << 45 )
				| ( uint64_t( uint8_t( p_state.getStencilBackFunc() ) ) << 42 )
				| ( uint64_t( uint8_t( p_state.getStencilBackRef() ) ) << 34 )
				| ( uint64_t( uint8_t( p_state.getStencilBackFailOp() ) ) << 31 )
				| ( uint64_t( uint8_t( p_state.getStencilBackDepthFailOp() ) ) << 28 )
				| ( uint64_t( uint8_t( p_state.getStencilBackPassOp() ) ) << 25 )
				| ( uint64_t( uint8_t( p_state.getStencilFrontFunc() ) ) << 22 )
				| ( uint64_t( uint8_t( p_state.getStencilFrontRef() ) ) << 14 )
				| ( uint64_t( uint8_t( p_state.getStencilFrontFailOp() ) ) << 11 )
				| ( uint64_t( uint8_t( p_state.getStencilFrontDepthFailOp() ) ) << 8 )
				| ( uint64_t( uint8_t( p_state.getStencilFrontPassOp() ) ) << 5 );
			
		}
		
		void doApply( BlendState const p_state, OpenGl const & p_gl )
		{
			bool enabled{ false };

			p_gl.ColorMask( p_gl.get( p_state.getColourMaskR() )
							, p_gl.get( p_state.getColourMaskG() )
							, p_gl.get( p_state.getColourMaskB() )
							, p_gl.get( p_state.getColourMaskA() ) );

			if ( p_state.isIndependantBlendEnabled() )
			{
				for ( int i = 0; i < 8; i++ )
				{
					if ( p_state.isBlendEnabled( i ) )
					{
						enabled = true;
						p_gl.BlendFunc( i
							, p_gl.get( p_state.getRgbSrcBlend( i ) )
							, p_gl.get( p_state.getRgbDstBlend( i ) )
							, p_gl.get( p_state.getAlphaSrcBlend( i ) )
							, p_gl.get( p_state.getAlphaDstBlend( i ) ) );
						p_gl.BlendEquation( i, p_gl.get( p_state.getRgbBlendOp( i ) ) );
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
				if ( p_state.isBlendEnabled( 0 ) )
				{
					enabled = true;
					p_gl.BlendFunc( p_gl.get( p_state.getRgbSrcBlend() )
						, p_gl.get( p_state.getRgbDstBlend() )
						, p_gl.get( p_state.getAlphaSrcBlend() )
						, p_gl.get( p_state.getAlphaDstBlend() ) );
					p_gl.BlendEquation( p_gl.get( p_state.getRgbBlendOp() ) );
				}
				else
				{
					p_gl.BlendFunc( GlBlendFactor::eOne, GlBlendFactor::eZero, GlBlendFactor::eOne, GlBlendFactor::eZero );
					p_gl.BlendEquation( GlBlendOp::eAdd );
				}
			}

			if ( enabled )
			{
				p_gl.BlendColor( p_state.getBlendFactors() );
				p_gl.Enable( GlTweak::eBlend );
			}
			else
			{
				p_gl.Disable( GlTweak::eBlend );
			}
		}

		void doApply( DepthStencilState const & p_state, OpenGl const & p_gl )
		{
#if CHECK_GL_STATES

			StateCheck check( p_state );

#endif
			static uint64_t save = 0ull;
			uint64_t current = doEncode( p_state );

			if ( current != save )
			{
				save = current;
				p_gl.DepthMask( p_gl.get( p_state.getDepthMask() ) );

				if ( p_state.getDepthTest() )
				{
					p_gl.Enable( GlTweak::eDepthTest );
					p_gl.DepthFunc( p_gl.get( p_state.getDepthFunc() ) );
				}
				else
				{
					p_gl.Disable( GlTweak::eDepthTest );
				}

				if ( p_state.getStencilTest() )
				{
					p_gl.Enable( GlTweak::eStencilTest );
					p_gl.StencilMaskSeparate( GlFace::eBoth, p_state.getStencilWriteMask() );
					p_gl.StencilFuncSeparate( GlFace::eBack, p_gl.get( p_state.getStencilBackFunc() ), p_state.getStencilBackRef(), p_state.getStencilReadMask() );
					p_gl.StencilFuncSeparate( GlFace::eFront, p_gl.get( p_state.getStencilFrontFunc() ), p_state.getStencilFrontRef(), p_state.getStencilReadMask() );
					p_gl.StencilOpSeparate( GlFace::eBack, p_gl.get( p_state.getStencilBackFailOp() ), p_gl.get( p_state.getStencilBackDepthFailOp() ), p_gl.get( p_state.getStencilBackPassOp() ) );
					p_gl.StencilOpSeparate( GlFace::eFront, p_gl.get( p_state.getStencilFrontFailOp() ), p_gl.get( p_state.getStencilFrontDepthFailOp() ), p_gl.get( p_state.getStencilFrontPassOp() ) );
				}
				else
				{
					p_gl.Disable( GlTweak::eStencilTest );
				}
			}
		}

		void doApply( MultisampleState const & p_state, OpenGl const & p_gl )
		{
			if ( p_state.getMultisample() )
			{
				p_gl.Enable( GlTweak::eMultisample );
			}
			else
			{
				p_gl.Disable( GlTweak::eMultisample );
			}

			if ( p_state.isAlphaToCoverageEnabled() )
			{
				p_gl.Enable( GlTweak::eAlphaToCoverage );
			}
			else
			{
				p_gl.Disable( GlTweak::eAlphaToCoverage );
			}
		}

		void doApply( RasteriserState const & p_state, OpenGl const & p_gl )
		{
			p_gl.PolygonMode( GlFace::eBoth, p_gl.get( p_state.getFillMode() ) );

			if ( p_state.getCulledFaces() != Culling::eNone )
			{
				p_gl.Enable( GlTweak::eCullFace );
				p_gl.CullFace( p_gl.get( p_state.getCulledFaces() ) );

				if ( p_state.getFrontCCW() )
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

			if ( p_state.getAntialiasedLines() )
			{
				p_gl.Enable( GlTweak::eLineSmooth );
				p_gl.Hint( GlHint::eLineSmooth, GlHintValue::eNicest );
			}
			else
			{
				p_gl.Disable( GlTweak::eLineSmooth );
				p_gl.Hint( GlHint::eLineSmooth, GlHintValue::eDontCare );
			}

			if ( p_state.getScissor() )
			{
				p_gl.Enable( GlTweak::eScissorTest );
			}
			else
			{
				p_gl.Disable( GlTweak::eScissorTest );
			}

			if ( p_state.getDepthClipping() )
			{
				p_gl.Disable( GlTweak::eDepthClamp );
			}
			else
			{
				p_gl.Enable( GlTweak::eDepthClamp );
			}

			if ( p_state.getDepthBiasFactor() != 0 || p_state.getDepthBiasUnits() != 0 )
			{
				p_gl.Enable( GlTweak::eOffsetFill );
				p_gl.PolygonOffset( p_state.getDepthBiasFactor(), p_state.getDepthBiasUnits() );
			}
			else
			{
				p_gl.Disable( GlTweak::eOffsetFill );
			}

			if ( p_state.getDiscardPrimitives() )
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
		, GlRenderSystem & renderSystem
		, DepthStencilState && p_dsState
		, RasteriserState && p_rsState
		, BlendState && p_bdState
		, MultisampleState && p_msState
		, ShaderProgram & p_program
		, PipelineFlags const & p_flags )
		: RenderPipeline{ renderSystem, std::move( p_dsState ), std::move( p_rsState ), std::move( p_bdState ), std::move( p_msState ), p_program, p_flags }
		, Holder{ p_gl }
	{
	}

	GlRenderPipeline::~GlRenderPipeline()
	{
	}

	void GlRenderPipeline::apply()const
	{
		doApply( m_rsState, getOpenGl() );
		doApply( m_dsState, getOpenGl() );
		doApply( m_blState, getOpenGl() );
		doApply( m_msState, getOpenGl() );
		m_program.bind();
		uint32_t count{ 0u };

		for ( auto & binding : m_bindings )
		{
			binding.get().bind( binding.get().getOwner()->getBindingPoint() );
		}
	}
}
