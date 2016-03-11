#include "BloomPostEffect.hpp"

#include <BackBuffers.hpp>
#include <BlendStateManager.hpp>
#include <BufferDeclaration.hpp>
#include <BufferElementDeclaration.hpp>
#include <Context.hpp>
#include <DynamicTexture.hpp>
#include <Engine.hpp>
#include <FrameVariableBuffer.hpp>
#include <GeometryBuffers.hpp>
#include <OneFrameVariable.hpp>
#include <Parameter.hpp>
#include <Pipeline.hpp>
#include <RenderSystem.hpp>
#include <RenderTarget.hpp>
#include <RenderWindow.hpp>
#include <SamplerManager.hpp>
#include <ShaderManager.hpp>
#include <TextureUnit.hpp>
#include <Vertex.hpp>
#include <VertexBuffer.hpp>
#include <Viewport.hpp>

#include <numeric>

#include <GlslSource.hpp>

using namespace Castor;
using namespace Castor3D;

namespace Bloom
{
	namespace
	{
		static const String FilterConfig = cuT( "FilterConfig" );
		static const String FilterConfigCoefficients = cuT( "c3d_fCoefficients" );
		static const String FilterConfigOffsetX = cuT( "c3d_fOffsetX" );
		static const String FilterConfigOffsetY = cuT( "c3d_fOffsetY" );
		static const String CombineMapPass0 = cuT( "c3d_mapPass0" );
		static const String CombineMapPass1 = cuT( "c3d_mapPass1" );
		static const String CombineMapPass2 = cuT( "c3d_mapPass2" );
		static const String CombineMapPass3 = cuT( "c3d_mapPass3" );
		static const String CombineMapScene = cuT( "c3d_mapScene" );

		Castor::String GetGlslVertexProgram( RenderSystem * p_renderSystem )
		{
			using namespace GLSL;
			GlslWriter l_writer = p_renderSystem->CreateGlslWriter();

			UBO_MATRIX( l_writer );

			// Shader inputs
			Vec2 position = l_writer.GetAttribute< Vec2 >( ShaderProgram::Position );

			// Shader outputs
			auto vtx_texture = l_writer.GetOutput< Vec2 >( cuT( "vtx_texture" ) );
			auto gl_Position = l_writer.GetBuiltin< Vec4 >( cuT( "gl_Position" ) );

			l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				vtx_texture = position;
				gl_Position = c3d_mtxProjection * vec4( position.XY, 0.0, 1.0 );
			} );
			return l_writer.Finalise();
		}

		Castor::String GetGlslHiPassProgram( RenderSystem * p_renderSystem )
		{
			using namespace GLSL;
			GlslWriter l_writer = p_renderSystem->CreateGlslWriter();

			// Shader inputs
			auto c3d_mapDiffuse = l_writer.GetUniform< Sampler2D >( ShaderProgram::MapDiffuse );
			auto vtx_texture = l_writer.GetInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto plx_v4FragColor = l_writer.GetFragData< Vec4 >( cuT( "plx_v4FragColor" ), 0 );

			l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				plx_v4FragColor = vec4( texture2D( c3d_mapDiffuse, vec2( vtx_texture.X, vtx_texture.Y ) ).XYZ, 1.0 );

				plx_v4FragColor.X = TERNARY( l_writer, Float, plx_v4FragColor.X > 1.0, Float( 1 ), Float( 0 ) );
				plx_v4FragColor.Y = TERNARY( l_writer, Float, plx_v4FragColor.Y > 1.0, Float( 1 ), Float( 0 ) );
				plx_v4FragColor.Z = TERNARY( l_writer, Float, plx_v4FragColor.Z > 1.0, Float( 1 ), Float( 0 ) );
			} );
			return l_writer.Finalise();
		}

		Castor::String GetGlslBlurProgram( RenderSystem * p_renderSystem )
		{
			using namespace GLSL;
			GlslWriter l_writer = p_renderSystem->CreateGlslWriter();

			// Shader inputs
			auto c3d_mapDiffuse = l_writer.GetUniform< Sampler2D >( ShaderProgram::MapDiffuse );
			auto vtx_texture = l_writer.GetInput< Vec2 >( cuT( "vtx_texture" ) );

			Ubo l_config = l_writer.GetUbo( FilterConfig );
			auto c3d_fCoefficients = l_writer.GetUniform< Float >( FilterConfigCoefficients, 3u );
			auto c3d_fOffsetX = l_writer.GetUniform< Float >( FilterConfigOffsetX );
			auto c3d_fOffsetY = l_writer.GetUniform< Float >( FilterConfigOffsetY );
			l_config.End();

			// Shader outputs
			auto plx_v4FragColor = l_writer.GetFragData< Vec4 >( cuT( "plx_v4FragColor" ), 0 );

			l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				LOCALE_ASSIGN( l_writer, Vec2, l_offset, vec2( c3d_fOffsetX, c3d_fOffsetY ) );
				plx_v4FragColor  = c3d_fCoefficients[0] * texture2D( c3d_mapDiffuse, vtx_texture - l_offset );
				plx_v4FragColor += c3d_fCoefficients[0] * texture2D( c3d_mapDiffuse, vtx_texture );
				plx_v4FragColor += c3d_fCoefficients[0] * texture2D( c3d_mapDiffuse, vtx_texture + l_offset );
			} );
			return l_writer.Finalise();
		}

		Castor::String GetGlslCombineProgram( RenderSystem * p_renderSystem )
		{
			using namespace GLSL;
			GlslWriter l_writer = p_renderSystem->CreateGlslWriter();

			// Shader inputs
			auto c3d_mapPass0 = l_writer.GetUniform< Sampler2D >( CombineMapPass0 );
			auto c3d_mapPass1 = l_writer.GetUniform< Sampler2D >( CombineMapPass1 );
			auto c3d_mapPass2 = l_writer.GetUniform< Sampler2D >( CombineMapPass2 );
			auto c3d_mapPass3 = l_writer.GetUniform< Sampler2D >( CombineMapPass3 );
			auto c3d_mapScene = l_writer.GetUniform< Sampler2D >( CombineMapScene );
			auto vtx_texture = l_writer.GetInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto plx_v4FragColor = l_writer.GetFragData< Vec4 >( cuT( "plx_v4FragColor" ), 0 );

			l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				plx_v4FragColor  = texture2D( c3d_mapPass0, vtx_texture );
				plx_v4FragColor += texture2D( c3d_mapPass1, vtx_texture );
				plx_v4FragColor += texture2D( c3d_mapPass2, vtx_texture );
				plx_v4FragColor += texture2D( c3d_mapPass3, vtx_texture );
				plx_v4FragColor += texture2D( c3d_mapScene, vtx_texture );
			} );
			return l_writer.Finalise();
		}
	}

	//*********************************************************************************************

	BloomPostEffect::BloomPostEffectSurface::BloomPostEffectSurface()
	{
	}

	bool BloomPostEffect::BloomPostEffectSurface::Initialise( RenderTarget & p_renderTarget, Size const & p_size, uint32_t p_index, bool p_linear )
	{
		bool l_return = false;
		m_size = p_size;
		m_colourTexture = std::make_shared< TextureUnit >( *p_renderTarget.GetEngine() );
		m_colourTexture->SetIndex( p_index );

		String l_name = cuT( "BloomSampler_" );
		eINTERPOLATION_MODE l_mode;

		if ( p_linear )
		{
			l_name += cuT( "Linear" );
			l_mode = eINTERPOLATION_MODE_ANISOTROPIC;
		}
		else
		{
			l_name += cuT( "Nearest" );
			l_mode = eINTERPOLATION_MODE_NEAREST;
		}

		SamplerSPtr l_sampler;

		if ( !p_renderTarget.GetEngine()->GetSamplerManager().Has( l_name ) )
		{
			l_sampler = p_renderTarget.GetEngine()->GetSamplerManager().Create( l_name );
			l_sampler->SetInterpolationMode( eINTERPOLATION_FILTER_MIN, l_mode );
			l_sampler->SetInterpolationMode( eINTERPOLATION_FILTER_MAG, l_mode );
			l_sampler->SetWrappingMode( eTEXTURE_UVW_U, eWRAP_MODE_CLAMP_TO_BORDER );
			l_sampler->SetWrappingMode( eTEXTURE_UVW_V, eWRAP_MODE_CLAMP_TO_BORDER );
			l_sampler->SetWrappingMode( eTEXTURE_UVW_W, eWRAP_MODE_CLAMP_TO_BORDER );
		}
		else
		{
			l_sampler = p_renderTarget.GetEngine()->GetSamplerManager().Find( l_name );
		}

		m_fbo = p_renderTarget.GetEngine()->GetRenderSystem()->CreateFrameBuffer();
		auto l_colourTexture = p_renderTarget.CreateDynamicTexture( eACCESS_TYPE_READ, eACCESS_TYPE_READ | eACCESS_TYPE_WRITE );

		m_colourTexture->SetSampler( l_sampler );
		l_colourTexture->SetRenderTarget( p_renderTarget.shared_from_this() );
		m_colourAttach = m_fbo->CreateAttachment( l_colourTexture );

		l_colourTexture->SetType( eTEXTURE_TYPE_2D );
		l_colourTexture->SetImage( p_size, ePIXEL_FORMAT_R8G8B8 );
		m_fbo->Create( 0 );
		m_colourTexture->SetTexture( l_colourTexture );
		m_colourTexture->Initialise();
		m_fbo->Initialise( p_size );

		if ( m_fbo->Bind( eFRAMEBUFFER_MODE_CONFIG ) )
		{
			m_fbo->Attach( eATTACHMENT_POINT_COLOUR, 0, m_colourAttach, eTEXTURE_TARGET_2D );
			l_return = m_fbo->IsComplete();
			m_fbo->Unbind();
		}

		return l_return;
	}

	void BloomPostEffect::BloomPostEffectSurface::Cleanup()
	{
		m_fbo->Bind( eFRAMEBUFFER_MODE_CONFIG );
		m_fbo->DetachAll();
		m_fbo->Unbind();
		m_fbo->Cleanup();

		m_colourTexture->Cleanup();
		m_fbo->Destroy();

		m_fbo.reset();
		m_colourAttach.reset();
		m_colourTexture.reset();
	}

	//*********************************************************************************************

	BloomPostEffect::BloomPostEffect( RenderSystem * p_renderSystem, RenderTarget & p_renderTarget, Parameters const & p_param )
		: PostEffect( p_renderSystem, p_renderTarget, p_param )
		, m_viewport( Viewport::Ortho( *p_renderSystem->GetEngine(), 0, 1, 0, 1, 0, 1 ) )
		, m_offsetX( 1.2f )
		, m_offsetY( 1.2f )
		, m_kernel( { 5, 6, 5 } )
		, m_declaration(
		{
			BufferElementDeclaration( ShaderProgram::Position, eELEMENT_USAGE_POSITION, eELEMENT_TYPE_2FLOATS ),
		} )
	{
		String l_count;

		if ( p_param.Get( cuT( "OffsetX" ), l_count ) )
		{
			m_offsetX = string::to_float( l_count );
		}

		if ( p_param.Get( cuT( "OffsetX" ), l_count ) )
		{
			m_offsetY = string::to_float( l_count );
		}

		// Normalize kernel coefficients
		float l_sum = std::accumulate( std::begin( m_kernel ), std::end( m_kernel ), 0.0f );
		std::transform( std::begin( m_kernel ), std::end( m_kernel ), std::begin( m_kernel ), [&l_sum]( float & p_value )
		{
			return p_value /= l_sum;
		} );

		real l_pBuffer[] =
		{
			0, 0,
			1, 1,
			0, 1,
			0, 0,
			1, 0,
			1, 1,
		};

		std::memcpy( m_buffer, l_pBuffer, sizeof( l_pBuffer ) );
		uint32_t i = 0;

		for ( auto & l_vertex : m_vertices )
		{
			l_vertex = std::make_shared< BufferElementGroup >( &reinterpret_cast< uint8_t * >( m_buffer )[i++ * m_declaration.GetStride()] );
		}
	}

	BloomPostEffect::~BloomPostEffect()
	{
	}

	bool BloomPostEffect::Initialise()
	{
		bool l_return = false;
		ShaderManager & l_manager = m_renderSystem->GetEngine()->GetShaderManager();
		eSHADER_MODEL l_model = m_renderSystem->GetMaxShaderModel();
		Size l_size = m_renderTarget.GetSize();
		String l_vertex;
		String l_hipass;
		String l_blur;
		String l_combine;

		if ( m_renderSystem->GetRendererType() == eRENDERER_TYPE_OPENGL )
		{
			l_vertex = GetGlslVertexProgram( m_renderSystem );
			l_hipass = GetGlslHiPassProgram( m_renderSystem );
			l_blur = GetGlslBlurProgram( m_renderSystem );
			l_combine = GetGlslCombineProgram( m_renderSystem );
		}
		else
		{
			CASTOR_EXCEPTION( "Unsupported renderer type" );
		}

		if ( !l_vertex.empty() && !l_hipass.empty() )
		{
			ShaderProgramSPtr l_program = l_manager.GetNewProgram();
			m_hiPassMapDiffuse = l_program->CreateFrameVariable( ShaderProgram::MapDiffuse, eSHADER_TYPE_PIXEL );
			l_manager.CreateMatrixBuffer( *l_program, MASK_SHADER_TYPE_VERTEX );
			l_program->SetSource( eSHADER_TYPE_VERTEX, l_model, l_vertex );
			l_program->SetSource( eSHADER_TYPE_PIXEL, l_model, l_hipass );
			l_program->Initialise();
			m_hiPassProgram = l_program;
		}

		if ( !l_vertex.empty() && !l_blur.empty() )
		{
			ShaderProgramSPtr l_program = l_manager.GetNewProgram();
			m_filterMapDiffuse = l_program->CreateFrameVariable( ShaderProgram::MapDiffuse, eSHADER_TYPE_PIXEL );
			auto l_filterConfig = m_renderSystem->CreateFrameVariableBuffer( FilterConfig );
			m_filterCoefficients = std::static_pointer_cast< OneFloatFrameVariable >( l_filterConfig->CreateVariable( *l_program, eFRAME_VARIABLE_TYPE_FLOAT, FilterConfigCoefficients, 3 ) );
			m_filterOffsetX = std::static_pointer_cast< OneFloatFrameVariable >( l_filterConfig->CreateVariable( *l_program, eFRAME_VARIABLE_TYPE_FLOAT, FilterConfigOffsetX ) );
			m_filterOffsetY = std::static_pointer_cast< OneFloatFrameVariable >( l_filterConfig->CreateVariable( *l_program, eFRAME_VARIABLE_TYPE_FLOAT, FilterConfigOffsetY ) );
			l_program->AddFrameVariableBuffer( l_filterConfig, MASK_SHADER_TYPE_PIXEL );
			l_manager.CreateMatrixBuffer( *l_program, MASK_SHADER_TYPE_VERTEX );

			l_program->SetSource( eSHADER_TYPE_VERTEX, l_model, l_vertex );
			l_program->SetSource( eSHADER_TYPE_PIXEL, l_model, l_blur );
			l_program->Initialise();
			m_filterProgram = l_program;
		}

		if ( !l_vertex.empty() && !l_combine.empty() )
		{
			ShaderProgramSPtr l_program = l_manager.GetNewProgram();
			l_program->CreateFrameVariable( CombineMapPass0, eSHADER_TYPE_PIXEL )->SetValue( 0 );
			l_program->CreateFrameVariable( CombineMapPass1, eSHADER_TYPE_PIXEL )->SetValue( 1 );
			l_program->CreateFrameVariable( CombineMapPass2, eSHADER_TYPE_PIXEL )->SetValue( 2 );
			l_program->CreateFrameVariable( CombineMapPass3, eSHADER_TYPE_PIXEL )->SetValue( 3 );
			l_program->CreateFrameVariable( CombineMapScene, eSHADER_TYPE_PIXEL )->SetValue( 4 );

			l_manager.CreateMatrixBuffer( *l_program, MASK_SHADER_TYPE_VERTEX );

			l_program->SetSource( eSHADER_TYPE_VERTEX, l_model, l_vertex );
			l_program->SetSource( eSHADER_TYPE_PIXEL, l_model, l_combine );
			l_program->Initialise();
			m_combineProgram = l_program;

			m_vertexBuffer = std::make_unique< VertexBuffer >( *m_renderSystem->GetEngine(), m_declaration );
			m_vertexBuffer->Resize( uint32_t( m_vertices.size() * m_declaration.GetStride() ) );
			m_vertexBuffer->LinkCoords( m_vertices.begin(), m_vertices.end() );
			m_vertexBuffer->Create();
			m_vertexBuffer->Initialise( eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_DRAW );
			m_geometryBuffers = m_renderSystem->CreateGeometryBuffers( eTOPOLOGY_TRIANGLES, *l_program, m_vertexBuffer.get(), nullptr, nullptr, nullptr );
		}

		uint32_t l_index = 0;

		for ( auto && l_surface : m_hiPassSurfaces )
		{
			l_surface.Initialise( m_renderTarget, l_size, l_index++, true );
			l_size.width() >>= 1;
			l_size.height() >>= 1;
		}

		l_size = m_renderTarget.GetSize();
		l_index = 0;

		for ( auto && l_surface : m_blurSurfaces )
		{
			l_surface.Initialise( m_renderTarget, l_size, l_index++, false );
			l_size.width() >>= 1;
			l_size.height() >>= 1;
		}

		return l_return;
	}

	void BloomPostEffect::Cleanup()
	{
		m_hiPassMapDiffuse.reset();
		m_filterMapDiffuse.reset();
		m_filterCoefficients.reset();
		m_filterOffsetX.reset();
		m_filterOffsetY.reset();

		m_vertexBuffer->Cleanup();
		m_vertexBuffer->Destroy();
		m_vertexBuffer.reset();
		m_geometryBuffers.reset();

		for ( auto && l_surface : m_blurSurfaces )
		{
			l_surface.Cleanup();
		}

		for ( auto && l_surface : m_hiPassSurfaces )
		{
			l_surface.Cleanup();
		}
	}

	bool BloomPostEffect::Apply()
	{
		m_renderTarget.GetFrameBuffer()->Unbind();

		if ( DoHiPassFilter() )
		{
			DoDownSample();
			DoBlur( m_hiPassSurfaces, m_blurSurfaces, FILTER_COUNT, m_filterOffsetX, m_offsetX );
			DoBlur( m_blurSurfaces, m_hiPassSurfaces, FILTER_COUNT, m_filterOffsetY, m_offsetY );
			DoCombine();
		}

		if ( m_renderTarget.GetFrameBuffer()->Bind( eFRAMEBUFFER_MODE_AUTOMATIC, eFRAMEBUFFER_TARGET_DRAW ) )
		{
			m_renderSystem->GetCurrentContext()->RenderTexture( m_renderTarget.GetSize(), *m_blurSurfaces[0].m_colourTexture->GetTexture() );
			m_renderTarget.GetFrameBuffer()->Unbind();
		}

		return m_renderTarget.GetFrameBuffer()->Bind();
	}

	bool BloomPostEffect::DoHiPassFilter()
	{
		bool l_return = false;
		auto l_source = &m_hiPassSurfaces[0];

		if ( l_source->m_fbo->Bind( eFRAMEBUFFER_MODE_AUTOMATIC, eFRAMEBUFFER_TARGET_DRAW ) )
		{
			l_return = true;
			auto l_context = m_renderSystem->GetCurrentContext();
			l_source->m_fbo->Clear();
			m_hiPassMapDiffuse->SetValue( 0 );
			l_context->RenderTexture( l_source->m_size, *m_renderTarget.GetTexture().GetTexture(), m_hiPassProgram.lock() );
			l_source->m_fbo->Unbind();
		}

		return l_return;
	}

	void BloomPostEffect::DoDownSample()
	{
		auto l_context = m_renderSystem->GetCurrentContext();
		auto l_source = &m_hiPassSurfaces[0];

		for ( uint32_t i = 1; i < m_hiPassSurfaces.size(); ++i )
		{
			auto l_destination = &m_hiPassSurfaces[i];

			if ( l_destination->m_fbo->Bind( eFRAMEBUFFER_MODE_AUTOMATIC, eFRAMEBUFFER_TARGET_DRAW ) )
			{
				l_context->RenderTexture( l_destination->m_size, *l_source->m_colourTexture->GetTexture() );
				l_destination->m_fbo->Unbind();
			}

			l_source = l_destination;
		}
	}

	void BloomPostEffect::DoBlur( SurfaceArray & p_sources, SurfaceArray & p_destinations, uint32_t p_count, Castor3D::OneFloatFrameVariableSPtr p_offset, float p_offsetValue )
	{
		auto l_context = m_renderSystem->GetCurrentContext();

		m_filterCoefficients->SetValue( m_kernel[0], 0 );
		m_filterCoefficients->SetValue( m_kernel[1], 1 );
		m_filterCoefficients->SetValue( m_kernel[2], 2 );
		m_filterOffsetX->SetValue( 0 );
		m_filterOffsetY->SetValue( 0 );

		for ( uint32_t i = 0; i < p_count; ++i )
		{
			auto l_source = &p_sources[i];
			auto l_destination = &p_destinations[i];

			if ( l_destination->m_fbo->Bind( eFRAMEBUFFER_MODE_AUTOMATIC, eFRAMEBUFFER_TARGET_DRAW ) )
			{
				p_offset->SetValue( p_offsetValue / l_source->m_size.width() );
				l_context->RenderTexture( l_source->m_size, *l_source->m_colourTexture->GetTexture(), m_filterProgram.lock() );
				l_destination->m_fbo->Unbind();
			}
		}
	}

	void BloomPostEffect::DoCombine()
	{
		if ( m_blurSurfaces[0].m_fbo->Bind( eFRAMEBUFFER_MODE_AUTOMATIC, eFRAMEBUFFER_TARGET_DRAW ) )
		{
			ShaderProgramSPtr l_program = m_combineProgram.lock();

			if ( l_program && l_program->GetStatus() == ePROGRAM_STATUS_LINKED )
			{
				m_viewport.SetSize( m_renderTarget.GetSize() );
				m_viewport.Render( m_renderSystem->GetPipeline() );

				auto const & l_texture0 = *m_hiPassSurfaces[0].m_colourTexture;
				auto const & l_texture1 = *m_hiPassSurfaces[1].m_colourTexture;
				auto const & l_texture2 = *m_hiPassSurfaces[2].m_colourTexture;
				auto const & l_texture3 = *m_hiPassSurfaces[3].m_colourTexture;
				auto const & l_texture4 = m_renderTarget.GetTexture();
				FrameVariableBufferSPtr l_matrices = l_program->FindFrameVariableBuffer( ShaderProgram::BufferMatrix );

				if ( l_matrices )
				{
					m_renderSystem->GetPipeline().ApplyProjection( *l_matrices );
				}

				l_program->Bind();

				l_texture0.Bind();
				l_texture1.Bind();
				l_texture2.Bind();
				l_texture3.Bind();
				l_texture4.GetTexture()->Bind( 4 );
				l_texture4.GetSampler()->Bind( 4 );

				m_geometryBuffers->Draw( uint32_t( m_vertices.size() ), 0 );

				l_texture0.Unbind();
				l_texture1.Unbind();
				l_texture2.Unbind();
				l_texture3.Unbind();
				l_texture4.GetTexture()->Unbind( 4 );
				l_texture4.GetSampler()->Unbind( 4 );

				l_program->Unbind();
			}

			m_blurSurfaces[0].m_fbo->Unbind();

			m_blurSurfaces[0].m_colourTexture->Bind();
			m_blurSurfaces[0].m_colourTexture->GetTexture()->GenerateMipmaps();
			m_blurSurfaces[0].m_colourTexture->Unbind();
		}
	}
}
