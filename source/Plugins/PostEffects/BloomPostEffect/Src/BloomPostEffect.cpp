#include "BloomPostEffect.hpp"

#include <BackBuffers.hpp>
#include <BufferDeclaration.hpp>
#include <BufferElementDeclaration.hpp>
#include <Context.hpp>
#include <DynamicTexture.hpp>
#include <Engine.hpp>
#include <FrameVariableBuffer.hpp>
#include <GeometryBuffers.hpp>
#include <OneFrameVariable.hpp>
#include <Parameter.hpp>
#include <RenderSystem.hpp>
#include <RenderTarget.hpp>
#include <RenderWindow.hpp>
#include <SamplerManager.hpp>
#include <ShaderManager.hpp>
#include <Vertex.hpp>
#include <VertexBuffer.hpp>
#include <Viewport.hpp>

#if C3D_HAS_GL_RENDERER
#	include <GlslSource.hpp>
#	include <GlRenderSystem.hpp>
using namespace GlRender;
#endif

#if C3D_HAS_D3D11_RENDERER
#	include <Dx11RenderSystem.hpp>
using namespace Dx11Render;
#endif

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

#if defined( C3D_HAS_GL_RENDERER )

		Castor::String GetGlslVertexProgram( RenderSystem * p_renderSystem )
		{
			using namespace GLSL;
			GlslWriter l_writer( static_cast< GlRenderSystem * >( p_renderSystem )->GetOpenGl(), eSHADER_TYPE_VERTEX );
			l_writer << GLSL::Version() << Endl();

			UBO_MATRIX( l_writer );

			// Shader inputs
			ATTRIBUTE( l_writer, Vec2, vertex );

			// Shader outputs
			OUT( l_writer, Vec2, vtx_texture );

			l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				vtx_texture = vertex;
				BUILTIN( l_writer, Vec4, gl_Position ) = c3d_mtxProjection * vec4( vertex.XY, 0.0, 1.0 );
			} );
			return l_writer.Finalise();
		}

		Castor::String GetGlslHiPassProgram( RenderSystem * p_renderSystem )
		{
			using namespace GLSL;
			GlslWriter l_writer( static_cast< GlRenderSystem * >( p_renderSystem )->GetOpenGl(), eSHADER_TYPE_PIXEL );
			l_writer << GLSL::Version() << Endl();

			// Shader inputs
			UNIFORM( l_writer, Sampler2D, c3d_mapDiffuse );
			IN( l_writer, Vec2, vtx_texture );

			// Shader outputs
			FRAG_OUTPUT( l_writer, Vec4, plx_v4FragColor, 0 );

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
			GlslWriter l_writer( static_cast< GlRenderSystem * >( p_renderSystem )->GetOpenGl(), eSHADER_TYPE_PIXEL );
			l_writer << GLSL::Version() << Endl();

			// Shader inputs
			UNIFORM( l_writer, Sampler2D, c3d_mapDiffuse );
			IN( l_writer, Vec2, vtx_texture );

			Ubo l_config = l_writer.GetUbo( cuT( "FilterConfig" ) );
			UNIFORM_ARRAY( l_config, Float, c3d_fCoefficients, 3 );
			UNIFORM( l_config, Float, c3d_fOffsetX );
			UNIFORM( l_config, Float, c3d_fOffsetY );
			l_config.End();

			// Shader outputs
			FRAG_OUTPUT( l_writer, Vec4, plx_v4FragColor, 0 );

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
			GlslWriter l_writer( static_cast< GlRenderSystem * >( p_renderSystem )->GetOpenGl(), eSHADER_TYPE_PIXEL );
			l_writer << GLSL::Version() << Endl();

			// Shader inputs
			UNIFORM( l_writer, Sampler2D, c3d_mapPass0 );
			UNIFORM( l_writer, Sampler2D, c3d_mapPass1 );
			UNIFORM( l_writer, Sampler2D, c3d_mapPass2 );
			UNIFORM( l_writer, Sampler2D, c3d_mapPass3 );
			UNIFORM( l_writer, Sampler2D, c3d_mapScene );

			IN( l_writer, Vec2, vtx_texture );

			// Shader outputs
			FRAG_OUTPUT( l_writer, Vec4, plx_v4FragColor, 0 );

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

#endif
#if defined( C3D_HAS_D3D11_RENDERER )

		Castor::String GetHlslVertexProgram( RenderSystem * p_renderSystem )
		{
			String l_return;
			DxRenderSystem * l_rs = static_cast< DxRenderSystem * >( p_renderSystem );
			std::unique_ptr< UniformsBase > l_uniforms = UniformsBase::Get( *l_rs );

			if ( l_uniforms )
			{
				l_return += l_uniforms->GetVertexInMatrices( 0 );
				l_return +=
					cuT( "struct VtxInput\n" )
					cuT( "{\n" )
					cuT( "	float2 Position: POSITION0;\n" )
					cuT( "};\n" )
					cuT( "struct VtxOutput\n" )
					cuT( "{\n" )
					cuT( "	float4 Position: SV_POSITION;\n" )
					cuT( "	float3 TextureUV: TEXCOORD0;\n" )
					cuT( "};\n" )
					cuT( "VtxOutput mainVx( in VtxInput p_input )\n" )
					cuT( "{\n" )
					cuT( "	VtxOutput l_output;\n" )
					cuT( "	l_output.Position = mul( float4( p_input.Position, 0.0, 1.0 ), c3d_mtxProjectionModelView );\n" )
					cuT( "	l_output.TextureUV = p_input.Position;\n" )
					cuT( "	return l_output;\n" )
					cuT( "}\n" );
			}

			return l_return;
		}

		Castor::String GetHlslHiPassProgram( RenderSystem * p_renderSystem )
		{
			String l_return;
			DxRenderSystem * l_rs = static_cast< DxRenderSystem * >( p_renderSystem );
			std::unique_ptr< UniformsBase > l_uniforms = UniformsBase::Get( *l_rs );

			if ( l_uniforms )
			{
				l_return +=
					cuT( "SamplerState DefaultSampler\n" )
					cuT( "{\n" )
					cuT( "	AddressU = WRAP;\n" )
					cuT( "	AddressV = WRAP;\n" )
					cuT( "	AddressW = WRAP;\n" )
					cuT( "	MipFilter = NONE;\n" )
					cuT( "	MinFilter = LINEAR;\n" )
					cuT( "	MagFilter = LINEAR;\n" )
					cuT( "};\n" )
					cuT( "struct PxlInput\n" )
					cuT( "{\n" )
					cuT( "	float4 Position: SV_POSITION;\n" )
					cuT( "	float3 TextureUV: TEXCOORD0;\n" )
					cuT( "};\n" )
					cuT( "Texture2D c3d_mapDiffuse: register( t0 );\n" )
					cuT( "float4 mainPx( in PxlInput p_input ): SV_TARGET\n" )
					cuT( "{\n" )
					cuT( "	float4 pxl_v4FragColor = float4( c3d_mapDiffuse.Sample( DefaultSampler, p_input.TextureUV ).rgb, 1.0 );\n" )
 					cuT( "	plx_v4FragColor.x = plx_v4FragColor.x > 1.0 ? 1.0 : 0.0;\n" )
					cuT( "	plx_v4FragColor.y = plx_v4FragColor.y > 1.0 ? 1.0 : 0.0;\n" )
					cuT( "	plx_v4FragColor.z = plx_v4FragColor.z > 1.0 ? 1.0 : 0.0;\n" )
					cuT( "	return plx_v4FragColor;\n" )
					cuT( "}\n" );
			}

			return l_return;
		}

		Castor::String GetHlslBlurProgram( RenderSystem * p_renderSystem )
		{
			String l_return;
			DxRenderSystem * l_rs = static_cast< DxRenderSystem * >( p_renderSystem );
			std::unique_ptr< UniformsBase > l_uniforms = UniformsBase::Get( *l_rs );

			if ( l_uniforms )
			{
				l_return +=
					cuT( "SamplerState DefaultSampler\n" )
					cuT( "{\n" )
					cuT( "	AddressU = WRAP;\n" )
					cuT( "	AddressV = WRAP;\n" )
					cuT( "	AddressW = WRAP;\n" )
					cuT( "	MipFilter = NONE;\n" )
					cuT( "	MinFilter = LINEAR;\n" )
					cuT( "	MagFilter = LINEAR;\n" )
					cuT( "};\n" )
					cuT( "struct PxlInput\n" )
					cuT( "{\n" )
					cuT( "	float4 Position: SV_POSITION;\n" )
					cuT( "	float3 TextureUV: TEXCOORD0;\n" )
					cuT( "};\n" )
					cuT( "cbuffer FilterConfig: register( cb0 )\n" )
					cuT( "{\n" )
					cuT( "	float c3d_fCoefficients[3];\n" )
					cuT( "	float c3d_fOffsetX;\n" )
					cuT( "	float c3d_fOffsetY;\n" )
					cuT( "};\n" )
					cuT( "Texture2D c3d_mapDiffuse: register( t0 );\n" )
					cuT( "float4 mainPx( in PxlInput p_input ): SV_TARGET\n" )
					cuT( "{\n" )
					cuT( "	float2 l_offset = float2( c3d_fOffsetX, c3d_fOffsetY );\n" )
					cuT( "	float4 plx_v4FragColor = c3d_fCoefficients[0] * c3d_mapDiffuse.Sample( DefaultSampler, p_input.TextureUV - l_offset );\n" )
					cuT( "	plx_v4FragColor += c3d_fCoefficients[0] * c3d_mapDiffuse.Sample( DefaultSampler, p_input.TextureUV );\n" )
					cuT( "	plx_v4FragColor += c3d_fCoefficients[0] * c3d_mapDiffuse.Sample( DefaultSampler, p_input.TextureUV + l_offset );\n" )
					cuT( "	return plx_v4FragColor;\n" )
					cuT( "}\n" );
			}

			return l_return;
		}

		Castor::String GetHlslCombineProgram( RenderSystem * p_renderSystem )
		{
			String l_return;
			DxRenderSystem * l_rs = static_cast< DxRenderSystem * >( p_renderSystem );
			std::unique_ptr< UniformsBase > l_uniforms = UniformsBase::Get( *l_rs );

			if ( l_uniforms )
			{
				l_return +=
					cuT( "SamplerState DefaultSampler\n" )
					cuT( "{\n" )
					cuT( "	AddressU = WRAP;\n" )
					cuT( "	AddressV = WRAP;\n" )
					cuT( "	AddressW = WRAP;\n" )
					cuT( "	MipFilter = NONE;\n" )
					cuT( "	MinFilter = LINEAR;\n" )
					cuT( "	MagFilter = LINEAR;\n" )
					cuT( "};\n" )
					cuT( "struct PxlInput\n" )
					cuT( "{\n" )
					cuT( "	float4 Position: SV_POSITION;\n" )
					cuT( "	float3 TextureUV: TEXCOORD0;\n" )
					cuT( "};\n" )
					cuT( "Texture2D c3d_mapPass0: register( t0 );\n" )
					cuT( "Texture2D c3d_mapPass1: register( t1 );\n" )
					cuT( "Texture2D c3d_mapPass2: register( t2 );\n" )
					cuT( "Texture2D c3d_mapPass3: register( t3 );\n" )
					cuT( "Texture2D c3d_mapScene: register( t4 );\n" )
					cuT( "float4 mainPx( in PxlInput p_input ): SV_TARGET\n" )
					cuT( "{\n" )
					cuT( "	float4 plx_v4FragColor  = texture2D( c3d_mapPass0, vtx_texture );\n" )
					cuT( "	plx_v4FragColor += c3d_mapPass1.texture2D( DefaultSampler, p_input.TextureUV );\n" )
					cuT( "	plx_v4FragColor += c3d_mapPass2.texture2D( DefaultSampler, p_input.TextureUV );\n" )
					cuT( "	plx_v4FragColor += c3d_mapPass3.texture2D( DefaultSampler, p_input.TextureUV );\n" )
					cuT( "	plx_v4FragColor += c3d_mapScene.texture2D( DefaultSampler, p_input.TextureUV );\n" )
					cuT( "	return plx_v4FragColor;\n" )
					cuT( "}\n" );
			}

			return l_return;
		}

#endif

	}

	//*********************************************************************************************

	BloomPostEffect::BloomPostEffectSurface::BloomPostEffectSurface()
	{
	}

	bool BloomPostEffect::BloomPostEffectSurface::Initialise( RenderTarget & p_renderTarget, Size const & p_size, bool p_linear )
	{
		bool l_return = false;
		m_size = p_size;

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

		if ( !p_renderTarget.GetOwner()->GetSamplerManager().Has( l_name ) )
		{
			l_sampler = p_renderTarget.GetOwner()->GetSamplerManager().Create( l_name );
			l_sampler->SetInterpolationMode( eINTERPOLATION_FILTER_MIN, l_mode );
			l_sampler->SetInterpolationMode( eINTERPOLATION_FILTER_MAG, l_mode );
			l_sampler->SetWrappingMode( eTEXTURE_UVW_U, eWRAP_MODE_CLAMP_TO_BORDER );
			l_sampler->SetWrappingMode( eTEXTURE_UVW_V, eWRAP_MODE_CLAMP_TO_BORDER );
			l_sampler->SetWrappingMode( eTEXTURE_UVW_W, eWRAP_MODE_CLAMP_TO_BORDER );
		}
		else
		{
			l_sampler = p_renderTarget.GetOwner()->GetSamplerManager().Find( l_name );
		}

		m_fbo = p_renderTarget.CreateFrameBuffer();
		m_colourTexture = p_renderTarget.CreateDynamicTexture( eACCESS_TYPE_READ, eACCESS_TYPE_READ | eACCESS_TYPE_WRITE );

		m_colourTexture->SetSampler( l_sampler );
		m_colourTexture->SetRenderTarget( p_renderTarget.shared_from_this() );
		m_colourAttach = p_renderTarget.CreateAttachment( m_colourTexture );

		m_colourTexture->SetType( eTEXTURE_TYPE_2D );
		m_colourTexture->SetImage( p_size, ePIXEL_FORMAT_R8G8B8 );
		m_fbo->Create( 0 );
		m_colourTexture->Create();

		m_colourTexture->Initialise( 0 );
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
		m_colourTexture->Destroy();
		m_fbo->Destroy();

		m_colourTexture.reset();
		m_fbo.reset();
		m_colourAttach.reset();
	}

	//*********************************************************************************************

	BloomPostEffect::BloomPostEffect( RenderSystem * p_renderSystem, RenderTarget & p_renderTarget, Parameters const & p_param )
		: PostEffect( p_renderSystem, p_renderTarget, p_param )
		, m_viewport( Viewport::Ortho( *p_renderSystem->GetOwner(), 0, 1, 0, 1, 0, 1 ) )
		, m_offsetX( 1.2f )
		, m_offsetY( 1.2f )
	{
		m_kernel[0] = 5;
		m_kernel[1] = 6;
		m_kernel[2] = 5;

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
		float l_sum = 0;

		for ( auto l_value : m_kernel )
		{
			l_sum += l_value;
		}

		for ( auto & l_value : m_kernel )
		{
			l_value /= l_sum;
		}

		BufferElementDeclaration l_vertexDeclarationElements[] =
		{
			BufferElementDeclaration( 0, eELEMENT_USAGE_POSITION, eELEMENT_TYPE_2FLOATS ),
		};
		m_declaration = std::make_shared< BufferDeclaration >( l_vertexDeclarationElements );

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
			l_vertex = std::make_shared< BufferElementGroup >( &reinterpret_cast< uint8_t * >( m_buffer )[i++ * m_declaration->GetStride()] );
		}
	}

	BloomPostEffect::~BloomPostEffect()
	{
	}

	bool BloomPostEffect::Initialise()
	{
		bool l_return = false;
		ShaderManager & l_manager = m_renderSystem->GetOwner()->GetShaderManager();
		eSHADER_MODEL l_model = m_renderSystem->GetMaxShaderModel();
		Size l_size = m_renderTarget.GetSize();
		String l_vertex;
		String l_hipass;
		String l_blur;
		String l_combine;
#if defined( C3D_HAS_GL_RENDERER )

		if ( m_renderSystem->GetRendererType() == eRENDERER_TYPE_OPENGL )
		{
			l_vertex = GetGlslVertexProgram( m_renderSystem );
			l_hipass = GetGlslHiPassProgram( m_renderSystem );
			l_blur = GetGlslBlurProgram( m_renderSystem );
			l_combine = GetGlslCombineProgram( m_renderSystem );
		}
		else

#endif
#if defined( DC3D_HAS_D3D11_RENDERER )

		if ( m_renderSystem->GetRendererType() == eRENDERER_TYPE_DIRECT3D )
		{
			l_vertex = GetHlslVertexProgram( m_renderSystem );
			l_hipass = GetHlslHiPassProgram( m_renderSystem );
			l_blur = GetHlslBlurProgram( m_renderSystem );
			l_combine = GetHlslCombineProgram( m_renderSystem );
		}
		else

#endif

		{
			CASTOR_EXCEPTION( "Unsupported renderer type" );
		}

		if ( !l_vertex.empty() && !l_hipass.empty() )
		{
			ShaderProgramBaseSPtr l_program = l_manager.GetNewProgram();
			m_hiPassMapDiffuse = l_program->CreateFrameVariable( ShaderProgramBase::MapDiffuse, eSHADER_TYPE_PIXEL );
			l_manager.CreateMatrixBuffer( *l_program, MASK_SHADER_TYPE_VERTEX );
			l_program->SetSource( eSHADER_TYPE_VERTEX, l_model, l_vertex );
			l_program->SetSource( eSHADER_TYPE_PIXEL, l_model, l_hipass );
			l_program->Initialise();
			m_hiPassProgram = l_program;
		}

		if ( !l_vertex.empty() && !l_blur.empty() )
		{
			ShaderProgramBaseSPtr l_program = l_manager.GetNewProgram();
			m_filterMapDiffuse = l_program->CreateFrameVariable( ShaderProgramBase::MapDiffuse, eSHADER_TYPE_PIXEL );
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
			ShaderProgramBaseSPtr l_program = l_manager.GetNewProgram();
			m_combineMapPass0 = l_program->CreateFrameVariable( CombineMapPass0, eSHADER_TYPE_PIXEL );
			m_combineMapPass1 = l_program->CreateFrameVariable( CombineMapPass1, eSHADER_TYPE_PIXEL );
			m_combineMapPass2 = l_program->CreateFrameVariable( CombineMapPass2, eSHADER_TYPE_PIXEL );
			m_combineMapPass3 = l_program->CreateFrameVariable( CombineMapPass3, eSHADER_TYPE_PIXEL );
			m_combineMapScene = l_program->CreateFrameVariable( CombineMapScene, eSHADER_TYPE_PIXEL );
			l_manager.CreateMatrixBuffer( *l_program, MASK_SHADER_TYPE_VERTEX );

			l_program->SetSource( eSHADER_TYPE_VERTEX, l_model, l_vertex );
			l_program->SetSource( eSHADER_TYPE_PIXEL, l_model, l_combine );
			l_program->Initialise();
			m_combineProgram = l_program;

			VertexBufferUPtr l_vtxBuffer = std::make_unique< VertexBuffer >( *m_renderSystem->GetOwner(), &( *m_declaration )[0], m_declaration->Size() );
			l_vtxBuffer->Resize( m_vertices.size() * m_declaration->GetStride() );
			l_vtxBuffer->LinkCoords( m_vertices.begin(), m_vertices.end() );
			m_geometryBuffers = m_renderSystem->CreateGeometryBuffers( std::move( l_vtxBuffer ), nullptr, nullptr, eTOPOLOGY_TRIANGLES );
			m_geometryBuffers->Create();
			m_geometryBuffers->Initialise( l_program, eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_DRAW );
		}

		for ( auto && l_surface : m_hiPassSurfaces )
		{
			l_surface.Initialise( m_renderTarget, l_size, true );
			l_size.width() >>= 1;
			l_size.height() >>= 1;
		}

		l_size = m_renderTarget.GetSize();

		for ( auto && l_surface : m_blurSurfaces )
		{
			l_surface.Initialise( m_renderTarget, l_size, false );
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
		m_combineMapPass0.reset();
		m_combineMapPass1.reset();
		m_combineMapPass2.reset();
		m_combineMapPass3.reset();
		m_combineMapScene.reset();
		m_declaration.reset();

		m_geometryBuffers->Cleanup();
		m_geometryBuffers->Destroy();
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
			m_renderSystem->GetCurrentContext()->RenderTextureToCurrentBuffer( m_renderTarget.GetSize(), m_blurSurfaces[0].m_colourTexture );
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
			m_hiPassMapDiffuse->SetValue( m_renderTarget.GetTexture().get() );
			l_context->RenderTexture( l_source->m_size, m_renderTarget.GetTexture(), m_hiPassProgram.lock() );
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
				l_context->RenderTextureToCurrentBuffer( l_destination->m_size, l_source->m_colourTexture );
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
				l_context->RenderTexture( l_source->m_size, l_source->m_colourTexture, m_filterProgram.lock() );
				l_destination->m_fbo->Unbind();
			}
		}
	}

	void BloomPostEffect::DoCombine()
	{
		if ( m_blurSurfaces[0].m_fbo->Bind( eFRAMEBUFFER_MODE_AUTOMATIC, eFRAMEBUFFER_TARGET_DRAW ) )
		{
			ShaderProgramBaseSPtr l_program = m_combineProgram.lock();
			m_viewport.SetSize( m_renderTarget.GetSize() );
			m_viewport.Render( m_renderSystem->GetPipeline() );

			auto l_texture0 = m_hiPassSurfaces[0].m_colourTexture;
			auto l_texture1 = m_hiPassSurfaces[1].m_colourTexture;
			auto l_texture2 = m_hiPassSurfaces[2].m_colourTexture;
			auto l_texture3 = m_hiPassSurfaces[3].m_colourTexture;

			l_texture0->SetIndex( 0 );
			l_texture1->SetIndex( 1 );
			l_texture2->SetIndex( 2 );
			l_texture3->SetIndex( 3 );

			uint32_t l_id = m_renderTarget.GetTexture()->GetIndex();
			m_renderTarget.GetTexture()->SetIndex( 4 );

			if ( l_program && l_program->GetStatus() == ePROGRAM_STATUS_LINKED )
			{
				FrameVariableBufferSPtr l_matrices = l_program->FindFrameVariableBuffer( ShaderProgramBase::BufferMatrix );
				m_combineMapPass0->SetValue( l_texture0.get() );
				m_combineMapPass1->SetValue( l_texture1.get() );
				m_combineMapPass2->SetValue( l_texture2.get() );
				m_combineMapPass3->SetValue( l_texture3.get() );
				m_combineMapScene->SetValue( m_renderTarget.GetTexture().get() );

				if ( l_matrices )
				{
					m_renderSystem->GetPipeline().ApplyProjection( *l_matrices );
				}
	
				l_program->Bind( 0, 1 );
			}

			l_texture0->BindAt( 0 );
			l_texture1->BindAt( 1 );
			l_texture2->BindAt( 2 );
			l_texture3->BindAt( 3 );
			m_renderTarget.GetTexture()->BindAt( 4 );

			m_geometryBuffers->Draw( l_program, m_vertices.size(), 0 );

			l_texture0->UnbindFrom( 0 );
			l_texture1->UnbindFrom( 1 );
			l_texture2->UnbindFrom( 2 );
			l_texture3->UnbindFrom( 3 );
			m_renderTarget.GetTexture()->UnbindFrom( 4 );

			if ( l_program && l_program->GetStatus() == ePROGRAM_STATUS_LINKED )
			{
				l_program->Unbind();
			}
	
			m_renderTarget.GetTexture()->SetIndex( l_id );
			m_blurSurfaces[0].m_fbo->Unbind();
		}
	}
}
