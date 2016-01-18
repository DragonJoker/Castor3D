#include "GlRenderSystem.hpp"

#include "GlBackBuffers.hpp"
#include "GlFrameBuffer.hpp"
#include "GlShaderProgram.hpp"
#include "GlShaderObject.hpp"
#include "OpenGl.hpp"
#include "GlPipeline.hpp"
#include "GlContext.hpp"
#include "GlVertexBufferObject.hpp"
#include "GlIndexBufferObject.hpp"
#include "GlMatrixBufferObject.hpp"
#include "GlGeometryBuffers.hpp"
#include "GlDynamicTexture.hpp"
#include "GlStaticTexture.hpp"
#include "GlBlendState.hpp"
#include "GlDepthStencilState.hpp"
#include "GlRasteriserState.hpp"
#include "GlFrameVariableBuffer.hpp"
#include "GlSampler.hpp"

#include "GlslSource.hpp"

#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlRenderSystem::GlRenderSystem( Engine & p_engine )
		: RenderSystem( p_engine, eRENDERER_TYPE_OPENGL )
		, m_openGlMajor( 0 )
		, m_openGlMinor( 0 )
		, m_useVertexBufferObjects( false )
		, m_extensionsInit( false )
		, m_openGl( *this )
	{
		m_instancing = true;
		m_accumBuffer = true;
		m_pipelineImpl = std::make_shared< GlPipelineImpl >( GetOpenGl(), *m_pipeline );
		m_pipeline->UpdateImpl();
	}

	GlRenderSystem::~GlRenderSystem()
	{
		m_pipelineImpl.reset();
	}

	bool GlRenderSystem::InitOpenGlExtensions()
	{
		if ( !m_extensionsInit )
		{
			if ( !GetOpenGl().Initialise() )
			{
				m_extensionsInit = false;
			}
			else
			{
				Logger::LogInfo( cuT( "Vendor : " ) + GetOpenGl().GetVendor()	);
				Logger::LogInfo( cuT( "Renderer : " ) + GetOpenGl().GetRenderer()	);
				Logger::LogInfo( cuT( "OpenGL Version : " ) + GetOpenGl().GetStrVersion()	);
				m_extensionsInit = true;
				m_instancing = GetOpenGl().HasInstancing();

				m_openGlMajor = GetOpenGl().GetVersion() / 10;
				m_openGlMinor = GetOpenGl().GetVersion() % 10;

				Logger::LogInfo( StringStream() << cuT( "Using version " ) << m_openGlMajor << cuT( "." ) << m_openGlMinor << cuT( " core functions" ) );
				m_useShader[eSHADER_TYPE_COMPUTE] = GetOpenGl().HasCSh();
				m_useShader[eSHADER_TYPE_HULL] = GetOpenGl().HasTSh();
				m_useShader[eSHADER_TYPE_DOMAIN] = GetOpenGl().HasTSh();
				m_useShader[eSHADER_TYPE_GEOMETRY] = GetOpenGl().HasGSh();
				m_useShader[eSHADER_TYPE_PIXEL] = GetOpenGl().HasPSh();
				m_useShader[eSHADER_TYPE_VERTEX] = GetOpenGl().HasVSh();

				if ( m_useShader[eSHADER_TYPE_COMPUTE] )
				{
					m_maxShaderModel = eSHADER_MODEL_5;
				}
				else if ( m_useShader[eSHADER_TYPE_HULL] )
				{
					m_maxShaderModel = eSHADER_MODEL_4;
				}
				else if ( m_useShader[eSHADER_TYPE_GEOMETRY] )
				{
					m_maxShaderModel = eSHADER_MODEL_3;
				}
				else if ( m_useShader[eSHADER_TYPE_PIXEL] )
				{
					m_maxShaderModel = eSHADER_MODEL_2;
				}
				else
				{
					m_maxShaderModel = eSHADER_MODEL_1;
				}

				m_nonPowerOfTwoTextures = GetOpenGl().HasNonPowerOfTwoTextures();
				REQUIRE( m_maxShaderModel >= eSHADER_MODEL_2 );
			}
		}

		return m_extensionsInit;
	}

	ContextSPtr GlRenderSystem::CreateContext()
	{
		return std::make_shared< GlContext >( *this, GetOpenGl() );
	}

	GeometryBuffersSPtr GlRenderSystem::CreateGeometryBuffers( VertexBufferUPtr p_pVertexBuffer, IndexBufferUPtr p_pIndexBuffer, MatrixBufferUPtr p_pMatrixBuffer, eTOPOLOGY p_topology )
	{
		return std::make_shared< GlGeometryBuffers >( GetOpenGl(), std::move( p_pVertexBuffer ), std::move( p_pIndexBuffer ), std::move( p_pMatrixBuffer ), p_topology );
	}

	DepthStencilStateSPtr GlRenderSystem::CreateDepthStencilState()
	{
		return std::make_shared< GlDepthStencilState >( this, GetOpenGl() );
	}

	RasteriserStateSPtr GlRenderSystem::CreateRasteriserState()
	{
		return std::make_shared< GlRasteriserState >( this, GetOpenGl() );
	}

	BlendStateSPtr GlRenderSystem::CreateBlendState()
	{
		return std::make_shared< GlBlendState >( this, GetOpenGl() );
	}

	FrameVariableBufferSPtr GlRenderSystem::CreateFrameVariableBuffer( Castor::String const & p_name )
	{
		return std::make_shared< GlFrameVariableBuffer >( GetOpenGl(), p_name, *this );
	}

	SamplerSPtr GlRenderSystem::CreateSampler( Castor::String const & p_name )
	{
		return std::make_shared< GlSampler >( GetOpenGl(), this, p_name );
	}

	ShaderProgramBaseSPtr GlRenderSystem::CreateShaderProgram()
	{
		return std::make_shared< GlShaderProgram >( GetOpenGl(), *this );
	}

	std::shared_ptr< Castor3D::GpuBuffer< uint32_t > > GlRenderSystem::CreateIndexBuffer( CpuBuffer< uint32_t > * p_buffer )
	{
		return std::make_shared< GlIndexBufferObject >( *this, GetOpenGl(), p_buffer );
	}

	std::shared_ptr< Castor3D::GpuBuffer< uint8_t > > GlRenderSystem::CreateVertexBuffer( BufferDeclaration const & p_declaration, CpuBuffer< uint8_t > * p_buffer )
	{
		return std::make_shared< GlVertexBufferObject >( *this, GetOpenGl(), p_declaration, p_buffer );
	}

	std::shared_ptr< Castor3D::GpuBuffer< real > > GlRenderSystem::CreateMatrixBuffer( CpuBuffer< real > * p_buffer )
	{
		return std::make_shared< GlMatrixBufferObject >( *this, GetOpenGl(), p_buffer );
	}

	StaticTextureSPtr GlRenderSystem::CreateStaticTexture()
	{
		return std::make_shared< GlStaticTexture >( GetOpenGl(), *this );
	}

	DynamicTextureSPtr GlRenderSystem::CreateDynamicTexture( uint8_t p_cpuAccess, uint8_t p_gpuAccess )
	{
		return std::make_shared< GlDynamicTexture >( GetOpenGl(), *this, p_cpuAccess, p_gpuAccess );
	}

	FrameBufferSPtr GlRenderSystem::CreateFrameBuffer()
	{
		return std::make_shared< GlFrameBuffer >( GetOpenGl(), *GetOwner() );
	}

	Castor3D::BackBuffersSPtr GlRenderSystem::CreateBackBuffers()
	{
		return std::make_shared< GlBackBuffers >( GetOpenGl(), *GetOwner() );
	}
	
	Castor3D::ShaderProgramBaseSPtr GlRenderSystem::CreateOverlayProgram( uint32_t p_flags )
	{
		using namespace GLSL;

		// Shader program
		ShaderManager & l_manager = GetOwner()->GetShaderManager();
		ShaderProgramBaseSPtr l_program = l_manager.GetNewProgram();
		l_manager.CreateMatrixBuffer( *l_program, MASK_SHADER_TYPE_VERTEX | MASK_SHADER_TYPE_PIXEL );
		l_manager.CreatePassBuffer( *l_program, MASK_SHADER_TYPE_PIXEL );

		// Vertex shader
		String l_strVs;
		{
			GlslWriter l_writer( GetOpenGl(), eSHADER_TYPE_VERTEX );
			l_writer << Version() << Endl();

			UBO_MATRIX( l_writer );

			// Shader inputs
			ATTRIBUTE( l_writer, IVec2, vertex );
			ATTRIBUTE( l_writer, Vec2, texture );

			// Shader outputs
			OUT( l_writer, Vec2, vtx_texture );

			l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				vtx_texture = texture;
				BUILTIN( l_writer, Vec4, gl_Position ) = c3d_mtxProjection * vec4( vertex.X, vertex.Y, 0.0, 1.0 );
			} );

			l_strVs = l_writer.Finalise();
		}

		// Pixel shader
		String l_strPs;
		{
			// Vertex shader
			GlslWriter l_writer( GetOpenGl(), eSHADER_TYPE_VERTEX );
			l_writer << Version() << Endl();

			UBO_PASS( l_writer );

			// Shader inputs
			IN( l_writer, Vec2, vtx_texture );
			UNIFORM( l_writer, Sampler2D, c3d_mapText );
			UNIFORM( l_writer, Sampler2D, c3d_mapColour );
			UNIFORM( l_writer, Sampler2D, c3d_mapOpacity );

			// Shader outputs
			FRAG_OUTPUT( l_writer, Vec4, pxl_v4FragColor, 0 );

			l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				LOCALE_ASSIGN( l_writer, Vec4, l_v4Ambient, c3d_v4MatAmbient );
				LOCALE_ASSIGN( l_writer, Float, l_fAlpha, c3d_fMatOpacity );

				if ( ( p_flags & eTEXTURE_CHANNEL_TEXT ) == eTEXTURE_CHANNEL_TEXT )
				{
					l_fAlpha *= texture2D( c3d_mapText, vec2( vtx_texture.X, vtx_texture.Y ) ).R;
				}

				if ( ( p_flags & eTEXTURE_CHANNEL_COLOUR ) == eTEXTURE_CHANNEL_COLOUR )
				{
					l_v4Ambient = texture2D( c3d_mapColour, vec2( vtx_texture.X, vtx_texture.Y ) );
				}

				if ( ( p_flags & eTEXTURE_CHANNEL_OPACITY ) == eTEXTURE_CHANNEL_OPACITY )
				{
					l_fAlpha *= texture2D( c3d_mapOpacity, vec2( vtx_texture.X, vtx_texture.Y ) ).R;
				}

				pxl_v4FragColor = vec4( l_v4Ambient.XYZ, l_fAlpha );
			} );

			l_strPs = l_writer.Finalise();
		}

		if ( ( p_flags & eTEXTURE_CHANNEL_TEXT ) == eTEXTURE_CHANNEL_TEXT )
		{
			l_program->CreateFrameVariable( ShaderProgramBase::MapText, eSHADER_TYPE_PIXEL );
		}

		if ( ( p_flags & eTEXTURE_CHANNEL_COLOUR ) == eTEXTURE_CHANNEL_COLOUR )
		{
			l_program->CreateFrameVariable( ShaderProgramBase::MapColour, eSHADER_TYPE_PIXEL );
		}

		if ( ( p_flags & eTEXTURE_CHANNEL_OPACITY ) == eTEXTURE_CHANNEL_OPACITY )
		{
			l_program->CreateFrameVariable( ShaderProgramBase::MapOpacity, eSHADER_TYPE_PIXEL );
		}

		eSHADER_MODEL l_model = GetMaxShaderModel();
		l_program->SetSource( eSHADER_TYPE_VERTEX, l_model, l_strVs );
		l_program->SetSource( eSHADER_TYPE_PIXEL, l_model, l_strPs );

		return l_program;
	}

	Castor3D::ShaderProgramBaseSPtr GlRenderSystem::CreateBillboardsProgram( RenderTechniqueBase const & p_technique, uint32_t p_flags )
	{
		using namespace GLSL;

		static String PRIMITIVES[] =
		{
			cuT( "points" ),//eTOPOLOGY_POINTS
			cuT( "lines" ),//eTOPOLOGY_LINES
			cuT( "line_loop" ),//eTOPOLOGY_LINE_LOOP
			cuT( "line_strip" ),//eTOPOLOGY_LINE_STRIP
			cuT( "triangles" ),//eTOPOLOGY_TRIANGLES
			cuT( "triangle_strip" ),//eTOPOLOGY_TRIANGLE_STRIPS
			cuT( "triangle_fan" ),//eTOPOLOGY_TRIANGLE_FAN
			cuT( "quads" ),//eTOPOLOGY_QUADS
			cuT( "quad_strip" ),//eTOPOLOGY_QUAD_STRIPS
			cuT( "polygon" ),//eTOPOLOGY_POLYGON
		};

		ShaderManager & l_manager = GetOwner()->GetShaderManager();
		ShaderProgramBaseSPtr l_program = l_manager.GetNewProgram();
		l_manager.CreateMatrixBuffer( *l_program, MASK_SHADER_TYPE_GEOMETRY | MASK_SHADER_TYPE_PIXEL );
		l_manager.CreateSceneBuffer( *l_program, MASK_SHADER_TYPE_VERTEX | MASK_SHADER_TYPE_GEOMETRY | MASK_SHADER_TYPE_PIXEL );
		l_manager.CreatePassBuffer( *l_program, MASK_SHADER_TYPE_PIXEL );
		l_manager.CreateTextureVariables( *l_program, p_flags );
		FrameVariableBufferSPtr l_billboardUbo = GetOwner()->GetRenderSystem()->CreateFrameVariableBuffer( cuT( "Billboard" ) );
		l_program->AddFrameVariableBuffer( l_billboardUbo, MASK_SHADER_TYPE_GEOMETRY );

		ShaderObjectBaseSPtr l_object = l_program->CreateObject( eSHADER_TYPE_GEOMETRY );
		l_object->SetInputType( eTOPOLOGY_POINTS );
		l_object->SetOutputType( eTOPOLOGY_TRIANGLE_STRIPS );
		l_object->SetOutputVtxCount( 4 );

		String l_strVtxShader;
		{
			GlslWriter l_writer( GetOpenGl(), eSHADER_TYPE_VERTEX );
			l_writer << Version() << Endl();

			// Shader inputs
			ATTRIBUTE( l_writer, IVec4, vertex );

			l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				BUILTIN( l_writer, Vec4, gl_Position ) = vec4( vertex.XYZ, 1.0 );
			} );

			l_strVtxShader = l_writer.Finalise();
		}

		String l_strGeoShader;
		{
			GlslWriter l_writer( GetOpenGl(), eSHADER_TYPE_GEOMETRY );
			l_writer << Version() << Endl();

			l_writer << cuT( "layout( " ) << PRIMITIVES[l_object->GetInputType()] << cuT( " ) in;" ) << Endl();
			l_writer << cuT( "layout( " ) << PRIMITIVES[l_object->GetOutputType()] << cuT( " ) out;" ) << Endl();
			l_writer << cuT( "layout( max_vertices = " ) << l_object->GetOutputVtxCount() << cuT( " ) out;" ) << Endl();

			UBO_MATRIX( l_writer );
			UBO_SCENE( l_writer );
			UBO_BILLBOARD( l_writer );

			OUT( l_writer, Vec3, vtx_vertex );
			OUT( l_writer, Vec3, vtx_normal );
			OUT( l_writer, Vec3, vtx_tangent );
			OUT( l_writer, Vec3, vtx_bitangent );
			OUT( l_writer, Vec3, vtx_texture );

			BUILTIN( l_writer, Vec4, gl_Position );
			BUILTIN_ARRAY( l_writer, gl_PerVertex, gl_in, 8u );

			l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
			{
				LOCALE_ASSIGN( l_writer, Vec3, l_position, l_writer.Paren( c3d_mtxProjectionModelView * gl_in[0].gl_Position() ).XYZ );
				l_position.Y = c3d_v3CameraPosition.Y;
				LOCALE_ASSIGN( l_writer, Vec3, l_toCamera, c3d_v3CameraPosition - l_position );
				LOCALE_ASSIGN( l_writer, Vec3, l_up, vec3( Float( 0.0f ), 1.0, 0.0 ) );
				LOCALE_ASSIGN( l_writer, Vec3, l_right, cross( l_toCamera, l_up ) );
				LOCALE_ASSIGN( l_writer, Vec3, l_v3Normal, l_writer.Paren( c3d_mtxProjectionModelView * vec4( Float( 0.0f ), 0.0, -1.0, 0.0 ) ).XYZ );
				l_v3Normal = l_writer.Paren( c3d_mtxProjectionModelView * vec4( l_v3Normal, 0.0 ) ).XYZ;

				LOCALE_ASSIGN( l_writer, Vec3, l_position0, l_position - ( l_right * 0.5 ) );
				LOCALE_ASSIGN( l_writer, Vec3, l_v2Texture0, vec3( Float( 0.0f ), 0.0, 0.0 ) );
				LOCALE_ASSIGN( l_writer, Vec3, l_position1, l_position0 + vec3( Float( 0.0f ), CAST( l_writer, Float, c3d_v2iDimensions.Y ), 0.0 ) );
				LOCALE_ASSIGN( l_writer, Vec3, l_v2Texture1, vec3( Float( 0.0f ), 1.0, 0.0 ) );
				LOCALE_ASSIGN( l_writer, Vec3, l_position2, l_position0 + l_right );
				LOCALE_ASSIGN( l_writer, Vec3, l_v2Texture2, vec3( Float( 1.0f ), 0.0, 0.0 ) );
				LOCALE_ASSIGN( l_writer, Vec3, l_position3, l_position2 + vec3( Float( 0.0f ), CAST( l_writer, Float, c3d_v2iDimensions.Y ), 0.0 ) );
				LOCALE_ASSIGN( l_writer, Vec3, l_v2Texture3, vec3( Float( 1.0f ), 1.0, 0.0 ) );

				LOCALE_ASSIGN( l_writer, Vec3, l_vec2m1, l_position1 - l_position0 );
				LOCALE_ASSIGN( l_writer, Vec3, l_vec3m1, l_position2 - l_position0 );
				LOCALE_ASSIGN( l_writer, Vec3, l_tex2m1, l_v2Texture1 - l_v2Texture0 );
				LOCALE_ASSIGN( l_writer, Vec3, l_tex3m1, l_v2Texture2 - l_v2Texture0 );
				LOCALE_ASSIGN( l_writer, Vec3, l_v3Tangent, normalize( ( l_vec2m1 * l_tex3m1.X ) - ( l_vec3m1 * l_tex2m1.Y ) ) );
				LOCALE_ASSIGN( l_writer, Vec3, l_v3Bitangent, cross( l_v3Tangent, l_v3Normal ) );

				{
					IndentBlock l_block( l_writer );
					l_writer << Endl();
					gl_Position = vec4( l_position0, 1.0 );
					vtx_vertex = l_position0;
					vtx_normal = l_v3Normal;
					vtx_tangent = l_v3Tangent;
					vtx_bitangent = l_v3Bitangent;
					vtx_texture = l_v2Texture0;
					l_writer.EmitVertex();
				}
				l_writer << Endl();

				{
					IndentBlock l_block( l_writer );
					l_writer << Endl();
					gl_Position = vec4( l_position1, 1.0 );
					vtx_vertex = l_position1;
					vtx_normal = l_v3Normal;
					vtx_tangent = l_v3Tangent;
					vtx_bitangent = l_v3Bitangent;
					vtx_texture = l_v2Texture1;
					l_writer.EmitVertex();
				}
				l_writer << Endl();

				{
					IndentBlock l_block( l_writer );
					l_writer << Endl();
					gl_Position = vec4( l_position2, 1.0 );
					vtx_vertex = l_position2;
					vtx_normal = l_v3Normal;
					vtx_tangent = l_v3Tangent;
					vtx_bitangent = l_v3Bitangent;
					vtx_texture = l_v2Texture2;
					l_writer.EmitVertex();
				}
				l_writer << Endl();

				{
					IndentBlock l_block( l_writer );
					l_writer << Endl();
					gl_Position = vec4( l_position3, 1.0 );
					vtx_vertex = l_position3;
					vtx_normal = l_v3Normal;
					vtx_tangent = l_v3Tangent;
					vtx_bitangent = l_v3Bitangent;
					vtx_texture = l_v2Texture3;
					l_writer.EmitVertex();
				}
				l_writer << Endl();
				l_writer.EndPrimitive();
			} );

			l_strGeoShader = l_writer.Finalise();
		}

		String l_strPxlShader = p_technique.GetPixelShaderSource( p_flags );

		std::static_pointer_cast< Point2iFrameVariable >( l_billboardUbo->CreateVariable( *l_program.get(), eFRAME_VARIABLE_TYPE_VEC2I, cuT( "c3d_v2iDimensions" ) ) );
		l_program->SetSource( eSHADER_TYPE_VERTEX, eSHADER_MODEL_3, l_strVtxShader );
		l_program->SetSource( eSHADER_TYPE_GEOMETRY, eSHADER_MODEL_3, l_strGeoShader );
		l_program->SetSource( eSHADER_TYPE_PIXEL, eSHADER_MODEL_3, l_strPxlShader );
		l_program->SetSource( eSHADER_TYPE_VERTEX, eSHADER_MODEL_4, l_strVtxShader );
		l_program->SetSource( eSHADER_TYPE_GEOMETRY, eSHADER_MODEL_4, l_strGeoShader );
		l_program->SetSource( eSHADER_TYPE_PIXEL, eSHADER_MODEL_4, l_strPxlShader );

		return l_program;
	}

	void GlRenderSystem::DoInitialise()
	{
		if ( !m_initialised )
		{
			InitOpenGlExtensions();

			m_useVertexBufferObjects = GetOpenGl().HasVbo();
			m_initialised = true;
			m_useShader[eSHADER_TYPE_COMPUTE] = GetOpenGl().HasCSh();
			m_useShader[eSHADER_TYPE_HULL] = GetOpenGl().HasTSh();
			m_useShader[eSHADER_TYPE_DOMAIN] = GetOpenGl().HasTSh();
			m_useShader[eSHADER_TYPE_GEOMETRY] = GetOpenGl().HasGSh();
			m_useShader[eSHADER_TYPE_PIXEL] = GetOpenGl().HasPSh();
			m_useShader[eSHADER_TYPE_VERTEX] = GetOpenGl().HasVSh();
			m_pipeline->Initialise();
		}
	}

	void GlRenderSystem::DoCleanup()
	{
		m_extensionsInit = false;
		m_initialised = false;
		GetOpenGl().Cleanup();
	}

#if C3D_TRACE_OBJECTS

	void GlRenderSystem::Track( void * p_object, std::string const & p_name, std::string const & p_file, int p_line )
	{
		std::string l_name;

		if ( DoTrack( p_object, p_name, p_file, p_line, l_name ) )
		{
		}
	}

	void GlRenderSystem::Untrack( void * p_object )
	{
		ObjectDeclaration l_decl;

		if ( DoUntrack( p_object, l_decl ) )
		{
		}
	}

#endif

}
