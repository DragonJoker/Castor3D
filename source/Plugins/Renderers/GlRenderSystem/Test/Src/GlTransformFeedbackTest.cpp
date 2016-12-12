#include "GlTransformFeedbackTest.hpp"

#include <Engine.hpp>
#include <Mesh/Buffer/VertexBuffer.hpp>
#include <Mesh/Buffer/GeometryBuffers.hpp>
#include <Miscellaneous/TransformFeedback.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Shader/UniformBuffer.hpp>
#include <Shader/OneUniform.hpp>
#include <Shader/ShaderProgram.hpp>
#include <State/RasteriserState.hpp>

#include <Buffer/GlBuffer.hpp>

using namespace Castor;
using namespace Castor3D;

#ifdef max
#	undef max
#endif

#define Glsl( x ) cuT( "#version 330\n" ) cuT( #x )

namespace Testing
{
	namespace
	{
		ShaderProgramSPtr DoCreateNoopProgram( Engine & p_engine )
		{
			String l_vtx = Glsl(
				void main()\n
				{\n
					gl_Position = vec4( 0, 0, 0, 1 );\n
				}\n
			);
			auto l_program = p_engine.GetRenderSystem()->CreateShaderProgram();
			auto l_model = p_engine.GetRenderSystem()->GetGpuInformations().GetMaxShaderModel();
			l_program->CreateObject( ShaderType::eVertex );
			l_program->SetSource( ShaderType::eVertex, l_model, l_vtx );
			return l_program;
		}

		ShaderProgramSPtr DoCreateBasicComputeProgram( Engine & p_engine )
		{
			String l_vtx = Glsl(
				in float inValue;\n
				out float outValue;\n\n
				void main()\n
				{\n
					outValue = sqrt( inValue );\n
				}\n
			);
			auto l_program = p_engine.GetRenderSystem()->CreateShaderProgram();
			auto l_model = p_engine.GetRenderSystem()->GetGpuInformations().GetMaxShaderModel();
			l_program->CreateObject( ShaderType::eVertex );
			l_program->SetSource( ShaderType::eVertex, l_model, l_vtx );
			return l_program;
		}

		ShaderProgramSPtr DoCreateGeometryShaderProgram( Engine & p_engine )
		{
			String l_vtx = Glsl(
				in float inValue;\n
				out float geoValue;\n\n
				void main()\n
				{\n
					geoValue = sqrt( inValue );\n
				}\n
			);
			String l_geo = Glsl(
				layout( points ) in;\n
				layout( triangle_strip, max_vertices = 3 ) out;\n\n
				in float[] geoValue;\n
				out float outValue;\n\n
				void main()\n
				{\n
					for ( int i = 0; i < 3; i++ )\n
					{\n
						outValue = geoValue[0] + i;\n
						EmitVertex();\n
					}\n
					EndPrimitive();\n
				}\n
			);
			auto l_program = p_engine.GetRenderSystem()->CreateShaderProgram();
			auto l_model = p_engine.GetRenderSystem()->GetGpuInformations().GetMaxShaderModel();
			l_program->CreateObject( ShaderType::eVertex );
			l_program->CreateObject( ShaderType::eGeometry );
			l_program->SetSource( ShaderType::eVertex, l_model, l_vtx );
			l_program->SetSource( ShaderType::eGeometry, l_model, l_geo );
			return l_program;
		}

		ShaderProgramSPtr DoCreateParticleSystemShaderProgram( Engine & p_engine )
		{
			String l_vtx = Glsl(
				layout( location = 0 ) in float Type;\n
				layout( location = 1 ) in vec3 Position;\n
				layout( location = 2 ) in vec3 Velocity;\n
				layout( location = 3 ) in float Age;\n\n
				out float Type0;\n
				out vec3 Position0;\n
				out vec3 Velocity0;\n
				out float Age0;\n\n
				void main()\n
				{\n
					Type0 = Type;\n
					Position0 = Position;\n
					Velocity0 = Velocity;\n
					Age0 = Age;\n
				}\n
			);
			String l_geo = Glsl(
				layout( points ) in;\n
				layout( points ) out;\n
				layout( max_vertices = 30 ) out;\n\n
				in float Type0[];\n
				in vec3 Position0[];\n
				in vec3 Velocity0[];\n
				in float Age0[];\n\n
				out float Type1;\n
				out vec3 Position1;\n
				out vec3 Velocity1;\n
				out float Age1;\n\n
				uniform float gDeltaTimeMillis;\n
				uniform float gTime;\n
				uniform float gLauncherLifetime;\n
				uniform float gShellLifetime;\n
				uniform float gSecondaryShellLifetime;\n\n
				vec3 GetRandomDir( float TexCoord )\n
				{\n
					vec3 Dir = vec3( TexCoord, TexCoord, TexCoord );\n
					Dir -= vec3( 0.5, 0.5, 0.5 );\n
					return Dir;\n
				}\n\n
				void main()\n
				{\n
					float Age = Age0[0] + gDeltaTimeMillis;\n\n
					if ( Type0[0] == 0.0f )\n
					{\n
						if ( Age >= gLauncherLifetime )\n
						{\n
							Type1 = 0.0f;\n
							Position1 = Position0[0];\n
							Velocity1 = Velocity0[0];\n
							Age1 = 0.0f;\n
							EmitVertex();\n
							EndPrimitive();\n\n
							Type1 = 1.0f;\n
							Position1 = Position0[0];\n
							vec3 Dir = GetRandomDir( gTime / 1000.0 );\n
							Dir.y = max( Dir.y, 0.5 );\n
							Velocity1 = normalize( Dir ) / 20.0;\n
							Age1 = 0.0f;\n
							EmitVertex();\n
							EndPrimitive();\n
						}\n
						else\n
						{\n
							Type1 = 0.0f;\n
							Position1 = Position0[0];\n
							Velocity1 = Velocity0[0];\n
							Age1 = Age;\n
							EmitVertex();\n
							EndPrimitive();\n
						}\n
					}\n
					else\n
					{\n
						float DeltaTimeSecs = gDeltaTimeMillis / 1000.0f;\n
						float t1 = Age0[0] / 1000.0;\n
						float t2 = Age / 1000.0;\n
						vec3 DeltaP = DeltaTimeSecs * Velocity0[0];\n
						vec3 DeltaV = vec3( DeltaTimeSecs ) * ( 0.0, -9.81, 0.0 );\n\n
						if ( Type0[0] == 1.0f )\n
						{\n
							if ( Age < gShellLifetime )\n
							{\n
								Type1 = 1.0f;\n
								Position1 = Position0[0] + DeltaP;\n
								Velocity1 = Velocity0[0] + DeltaV;\n
								Age1 = Age;\n
								EmitVertex();\n
								EndPrimitive();\n
							}\n
							else\n
							{\n
								for ( int i = 0; i < 10; i++ )\n
								{\n
									Type1 = 2.0f;\n
									Position1 = Position0[0];\n
									vec3 Dir = GetRandomDir( ( gTime + i ) / 1000.0 );\n
									Velocity1 = normalize( Dir ) / 20.0;\n
									Age1 = 0.0f;\n
									EmitVertex();\n
									EndPrimitive();\n
								}\n
							}\n
						}\n
						else\n
						{\n
							if ( Age < gSecondaryShellLifetime )\n
							{\n
								Type1 = 2.0f;\n
								Position1 = Position0[0] + DeltaP;\n
								Velocity1 = Velocity0[0] + DeltaV;\n
								Age1 = Age;\n
								EmitVertex();\n
								EndPrimitive();\n
							}\n
						}\n
					}\n
				}\n
			);
			auto l_program = p_engine.GetRenderSystem()->CreateShaderProgram();
			auto l_model = p_engine.GetRenderSystem()->GetGpuInformations().GetMaxShaderModel();
			l_program->CreateObject( ShaderType::eVertex );
			l_program->CreateObject( ShaderType::eGeometry );
			l_program->SetSource( ShaderType::eVertex, l_model, l_vtx );
			l_program->SetSource( ShaderType::eGeometry, l_model, l_geo );
			return l_program;
		}
	}

	GlTransformFeedbackTest::GlTransformFeedbackTest( Engine & p_engine )
		: GlTestCase{ "GlTransformFeedbackTest", p_engine }
	{
	}

	GlTransformFeedbackTest::~GlTransformFeedbackTest()
	{
	}

	void GlTransformFeedbackTest::DoRegisterTests()
	{
		DoRegisterTest( "GlTransformFeedbackTest::Creation", std::bind( &GlTransformFeedbackTest::Creation, this ) );
		DoRegisterTest( "GlTransformFeedbackTest::BasicCompute", std::bind( &GlTransformFeedbackTest::BasicCompute, this ) );
		DoRegisterTest( "GlTransformFeedbackTest::GeometryShader", std::bind( &GlTransformFeedbackTest::GeometryShader, this ) );
		DoRegisterTest( "GlTransformFeedbackTest::VariableFeedback", std::bind( &GlTransformFeedbackTest::VariableFeedback, this ) );
		DoRegisterTest( "GlTransformFeedbackTest::ParticleSystem", std::bind( &GlTransformFeedbackTest::ParticleSystem, this ) );
		DoRegisterTest( "GlTransformFeedbackTest::TwoWaysParticleSystem", std::bind( &GlTransformFeedbackTest::TwoWaysParticleSystem, this ) );
	}

	void GlTransformFeedbackTest::Creation()
	{
		auto l_program = DoCreateNoopProgram( m_engine );
		auto l_transformFeedback = m_engine.GetRenderSystem()->CreateTransformFeedback( {}, Topology::ePoints, *l_program );

		m_engine.GetRenderSystem()->GetMainContext()->SetCurrent();
		CT_CHECK( l_program->Initialise() );
		CT_CHECK( l_transformFeedback->Initialise( {} ) );
		l_transformFeedback->Cleanup();
		l_program->Cleanup();
		m_engine.GetRenderSystem()->GetMainContext()->EndCurrent();
	}

	void GlTransformFeedbackTest::BasicCompute()
	{
		BufferDeclaration l_outputs
		{
			{
				BufferElementDeclaration( cuT( "outValue" ), 0, ElementType::eFloat, 0u )
			}
		};
		BufferDeclaration l_inputs
		{
			{
				BufferElementDeclaration( cuT( "inValue" ), 0, ElementType::eFloat, 0u )
			}
		};
		float l_data[] = { 1, 2, 3, 4, 5 };

		// Shader program
		auto l_program = DoCreateBasicComputeProgram( m_engine );

		// Input VBO
		VertexBuffer l_vboIn{ m_engine, l_inputs };
		l_vboIn.Resize( sizeof( l_data ) );
		std::memcpy( l_vboIn.data(), l_data, sizeof( l_data ) );

		// Output VBO
		VertexBuffer l_vboOut{ m_engine, l_outputs };
		l_vboOut.Resize( sizeof( l_data ) );

		// Transform feedback
		auto l_transformFeedback = m_engine.GetRenderSystem()->CreateTransformFeedback( l_outputs, Topology::ePoints, *l_program );
		l_program->SetTransformLayout( l_outputs );

		// VAO
		auto l_geometryBuffers = m_engine.GetRenderSystem()->CreateGeometryBuffers( Topology::ePoints, *l_program );

		// Pipeline
		RasteriserState l_rs;
		l_rs.SetDiscardPrimitives( true );
		auto l_pipeline = m_engine.GetRenderSystem()->CreateRenderPipeline( DepthStencilState{}, std::move( l_rs ), BlendState{}, MultisampleState{}, *l_program, PipelineFlags{} );

		m_engine.GetRenderSystem()->GetMainContext()->SetCurrent();
		CT_CHECK( l_program->Initialise() );
		CT_CHECK( l_vboIn.Initialise( BufferAccessType::eDynamic, BufferAccessNature::eDraw ) );
		CT_CHECK( l_vboOut.Initialise( BufferAccessType::eDynamic, BufferAccessNature::eDraw ) );
		CT_CHECK( l_transformFeedback->Initialise( { l_vboOut } ) );
		CT_CHECK( l_geometryBuffers->Initialise( { l_vboIn }, nullptr ) );

		for ( int j = 0; j < 2; ++j )
		{
			std::array< float, 5 > l_buffer;
			l_vboIn.Download( 0u, sizeof( l_data ), reinterpret_cast< uint8_t * >( l_buffer.data() ) );

			for ( int i = 0; i < 5; ++i )
			{
				CT_EQUAL( l_buffer[i], l_data[i] );
			}

			l_pipeline->Apply();
			l_program->UpdateUbos();
			l_transformFeedback->Bind();
			CT_CHECK( l_geometryBuffers->Draw( 5, 0 ) );
			l_transformFeedback->Unbind();
			CT_EQUAL( l_transformFeedback->GetWrittenPrimitives(), 5 );

			l_vboOut.Download( 0u, sizeof( l_data ), reinterpret_cast< uint8_t * >( l_buffer.data() ) );

			for ( int i = 0; i < 5; ++i )
			{
				CT_EQUAL( l_buffer[i], sqrt( l_data[i] ) );
				l_data[i] = sqrt( l_data[i] );
			}

			l_vboIn.Copy( l_vboOut, sizeof( l_data ) );
			m_engine.GetRenderSystem()->GetMainContext()->SwapBuffers();
		}

		l_geometryBuffers->Cleanup();
		l_transformFeedback->Cleanup();
		l_vboOut.Cleanup();
		l_vboIn.Cleanup();
		l_pipeline->Cleanup();
		m_engine.GetRenderSystem()->GetMainContext()->EndCurrent();
	}

	void GlTransformFeedbackTest::GeometryShader()
	{
		BufferDeclaration l_outputs
		{
			{
				BufferElementDeclaration( cuT( "outValue" ), 0, ElementType::eFloat, 0u )
			}
		};
		BufferDeclaration l_inputs
		{
			{
				BufferElementDeclaration( cuT( "inValue" ), 0, ElementType::eFloat, 0u )
			}
		};
		float l_data[] = { 1, 2, 3, 4, 5 };

		// Shader program
		auto l_program = DoCreateGeometryShaderProgram( m_engine );

		// Transform feedback
		auto l_transformFeedback = m_engine.GetRenderSystem()->CreateTransformFeedback( l_outputs, Topology::eTriangles, *l_program );
		l_program->SetTransformLayout( l_outputs );

		// Pipeline
		RasteriserState l_rs;
		l_rs.SetDiscardPrimitives( true );
		auto l_pipeline = m_engine.GetRenderSystem()->CreateRenderPipeline( DepthStencilState{}, std::move( l_rs ), BlendState{}, MultisampleState{}, *l_program, PipelineFlags{} );

		// Input VBO
		VertexBuffer l_vboIn{ m_engine, l_inputs };
		l_vboIn.Resize( sizeof( l_data ) );
		std::memcpy( l_vboIn.data(), l_data, sizeof( l_data ) );

		// Output VBO
		VertexBuffer l_vboOut{ m_engine, l_outputs };
		l_vboOut.Resize( sizeof( l_data ) * 3 );

		// VAO
		auto l_geometryBuffers = m_engine.GetRenderSystem()->CreateGeometryBuffers( Topology::ePoints, *l_program );

		m_engine.GetRenderSystem()->GetMainContext()->SetCurrent();
		CT_CHECK( l_program->Initialise() );
		CT_CHECK( l_vboIn.Initialise( BufferAccessType::eStatic, BufferAccessNature::eDraw ) );
		CT_CHECK( l_vboOut.Initialise( BufferAccessType::eStatic, BufferAccessNature::eRead ) );
		CT_CHECK( l_transformFeedback->Initialise( { l_vboOut } ) );
		CT_CHECK( l_geometryBuffers->Initialise( { l_vboIn }, nullptr ) );

		l_pipeline->Apply();
		l_transformFeedback->Bind();
		CT_CHECK( l_geometryBuffers->Draw( 5, 0 ) );
		l_transformFeedback->Unbind();
		m_engine.GetRenderSystem()->GetMainContext()->SwapBuffers();

		l_vboOut.Bind();
		auto l_buffer = reinterpret_cast< float * >( l_vboOut.Lock( 0, sizeof( l_data ) * 3, AccessType::eRead ) );

		if ( l_buffer )
		{
			for ( int j = 0; j < 5; ++j )
			{
				auto l_src = sqrt( l_data[j] );

				for ( int i = 0; i < 3; ++i )
				{
					CT_EQUAL( l_buffer[j * 3 + i], l_src + i );
				}
			}

			l_vboOut.Unlock();
		}

		l_vboOut.Unbind();

		l_geometryBuffers->Cleanup();
		l_transformFeedback->Cleanup();
		l_vboOut.Cleanup();
		l_vboIn.Cleanup();
		l_program->Cleanup();
		m_engine.GetRenderSystem()->GetMainContext()->EndCurrent();
	}

	void GlTransformFeedbackTest::VariableFeedback()
	{
		BufferDeclaration l_outputs
		{
			{
				BufferElementDeclaration( cuT( "outValue" ), 0, ElementType::eFloat, 0u )
			}
		};
		BufferDeclaration l_inputs
		{
			{
				BufferElementDeclaration( cuT( "inValue" ), 0, ElementType::eFloat, 0u )
			}
		};
		float l_data[] = { 1, 2, 3, 4, 5 };

		// Shader program
		auto l_program = DoCreateGeometryShaderProgram( m_engine );

		// Transform feedback
		auto l_transformFeedback = m_engine.GetRenderSystem()->CreateTransformFeedback( l_outputs, Topology::eTriangles, *l_program );
		l_program->SetTransformLayout( l_outputs );

		// Pipeline
		RasteriserState l_rs;
		l_rs.SetDiscardPrimitives( true );
		auto l_pipeline = m_engine.GetRenderSystem()->CreateRenderPipeline( DepthStencilState{}, std::move( l_rs ), BlendState{}, MultisampleState{}, *l_program, PipelineFlags{} );

		// Input VBO
		VertexBuffer l_vboIn{ m_engine, l_inputs };
		l_vboIn.Resize( sizeof( l_data ) );
		std::memcpy( l_vboIn.data(), l_data, sizeof( l_data ) );

		// Output VBO
		VertexBuffer l_vboOut{ m_engine, l_outputs };
		l_vboOut.Resize( sizeof( l_data ) * 3 );

		// VAO
		auto l_geometryBuffers = m_engine.GetRenderSystem()->CreateGeometryBuffers( Topology::ePoints, *l_program );

		m_engine.GetRenderSystem()->GetMainContext()->SetCurrent();
		CT_CHECK( l_program->Initialise() );
		CT_CHECK( l_vboIn.Initialise( BufferAccessType::eStatic, BufferAccessNature::eDraw ) );
		CT_CHECK( l_vboOut.Initialise( BufferAccessType::eStatic, BufferAccessNature::eRead ) );
		CT_CHECK( l_transformFeedback->Initialise( { l_vboOut } ) );
		CT_CHECK( l_geometryBuffers->Initialise( { l_vboIn }, nullptr ) );

		l_pipeline->Apply();
		l_transformFeedback->Bind();
		CT_CHECK( l_geometryBuffers->Draw( 5, 0 ) );
		l_transformFeedback->Unbind();
		m_engine.GetRenderSystem()->GetMainContext()->SwapBuffers();
		CT_EQUAL( l_transformFeedback->GetWrittenPrimitives(), 5 );

		l_geometryBuffers->Cleanup();
		l_transformFeedback->Cleanup();
		l_vboOut.Cleanup();
		l_vboIn.Cleanup();
		l_program->Cleanup();
		m_engine.GetRenderSystem()->GetMainContext()->EndCurrent();
	}

	void GlTransformFeedbackTest::ParticleSystem()
	{
		struct Particle
		{
			enum class Type
			{
				Launcher,
				Shell,
				SecondaryShell
			};
			float m_type;
			Castor::Point3f m_position;
			Castor::Point3f m_velocity;
			float m_lifeTime;
		};
		BufferDeclaration l_outputs
		{
			{
				BufferElementDeclaration( cuT( "Type1" ), 0u, ElementType::eFloat ),
				BufferElementDeclaration( cuT( "Position1" ), 0u, ElementType::eVec3 ),
				BufferElementDeclaration( cuT( "Velocity1" ), 0u, ElementType::eVec3 ),
				BufferElementDeclaration( cuT( "Age1" ), 0u, ElementType::eFloat ),
			}
		};
		BufferDeclaration l_inputs
		{
			{
				BufferElementDeclaration( cuT( "Type" ), 0u, ElementType::eFloat ),
				BufferElementDeclaration( cuT( "Position" ), uint32_t( ElementUsage::ePosition ), ElementType::eVec3 ),
				BufferElementDeclaration( cuT( "Velocity" ), 0u, ElementType::eVec3 ),
				BufferElementDeclaration( cuT( "Age" ), 0u, ElementType::eFloat ),
			}
		};
		Particle l_particle
		{
			float( Particle::Type::Launcher ),
			Point3r{ 0, 0, 0 },
			Point3r{ 0.0f, 0.0001f, 0.0f },
			0.0f
		};

		// Shader program
		auto l_program = DoCreateParticleSystemShaderProgram( m_engine );
		auto l_deltaTime = l_program->CreateUniform< Uniform1f >( cuT( "gDeltaTimeMillis" ), ShaderType::eGeometry );
		auto l_total = l_program->CreateUniform< Uniform1f >( cuT( "gTime" ), ShaderType::eGeometry );
		auto l_launcherLifetime = l_program->CreateUniform< Uniform1f >( cuT( "gLauncherLifetime" ), ShaderType::eGeometry );
		auto l_shellLifetime = l_program->CreateUniform< Uniform1f >( cuT( "gShellLifetime" ), ShaderType::eGeometry );
		auto l_secondaryShellLifetime = l_program->CreateUniform< Uniform1f >( cuT( "gSecondaryShellLifetime" ), ShaderType::eGeometry );
		l_launcherLifetime->SetValue( 100.0f );
		l_shellLifetime->SetValue( 10000.0f );
		l_secondaryShellLifetime->SetValue( 2500.0f );

		// Transform feedback
		TransformFeedbackSPtr l_transformFeedback{ m_engine.GetRenderSystem()->CreateTransformFeedback( l_outputs, Topology::ePoints, *l_program ) };
		l_program->SetTransformLayout( l_outputs );

		// Pipeline
		RasteriserState l_rs;
		l_rs.SetDiscardPrimitives( true );
		auto l_pipeline = m_engine.GetRenderSystem()->CreateRenderPipeline( DepthStencilState{}, std::move( l_rs ), BlendState{}, MultisampleState{}, *l_program, PipelineFlags{} );

		// Input VBO
		VertexBuffer l_vboIn{ m_engine, l_inputs };
		l_vboIn.Resize( 10000 * sizeof( l_particle ) );
		std::memcpy( l_vboIn.data(), &l_particle, sizeof( l_particle ) );

		// Output VBO
		VertexBuffer l_vboOut{ m_engine, l_outputs };
		l_vboOut.Resize( 10000 * sizeof( l_particle ) );

		// VAO
		GeometryBuffersSPtr l_geometryBuffers{ m_engine.GetRenderSystem()->CreateGeometryBuffers( Topology::ePoints, *l_program ) };

		m_engine.GetRenderSystem()->GetMainContext()->SetCurrent();
		CT_CHECK( l_program->Initialise() );

		CT_CHECK( l_vboIn.Initialise( BufferAccessType::eStatic, BufferAccessNature::eDraw ) );
		CT_CHECK( l_vboOut.Initialise( BufferAccessType::eStatic, BufferAccessNature::eRead ) );
		CT_CHECK( l_transformFeedback->Initialise( { l_vboOut } ) );
		CT_CHECK( l_geometryBuffers->Initialise( { l_vboIn }, nullptr ) );

		double l_totalTime{ 0u };
		Particle l_particles[10000];

		for ( int i = 0; i < 10000; ++i )
		{
			auto l_time = 40.0f;
			l_totalTime += l_time;
			l_deltaTime->SetValue( float( l_time ) );
			l_total->SetValue( float( l_totalTime ) );

			l_vboOut.Bind();
			auto l_count = uint32_t( sizeof( Particle ) * std::max( 1u, l_transformFeedback->GetWrittenPrimitives() ) );
			auto l_buffer = reinterpret_cast< Particle * >( l_vboOut.Lock( 0, l_count, AccessType::eRead ) );

			if ( l_buffer )
			{
				std::memcpy( l_particles, l_buffer, l_count );
				l_vboOut.Unlock();
			}

			l_vboOut.Unbind();

			l_pipeline->Apply();
			l_transformFeedback->Bind();
			CT_CHECK( l_geometryBuffers->Draw( std::max( 1u, l_transformFeedback->GetWrittenPrimitives() ), 0 ) );
			l_transformFeedback->Unbind();
			m_engine.GetRenderSystem()->GetMainContext()->SwapBuffers();

			l_vboOut.Bind();
			l_count = uint32_t( sizeof( Particle ) * std::max( 1u, l_transformFeedback->GetWrittenPrimitives() ) );
			l_buffer = reinterpret_cast< Particle * >( l_vboOut.Lock( 0, l_count, AccessType::eRead ) );

			if ( l_buffer )
			{
				std::memcpy( l_particles, l_buffer, l_count );
				l_vboOut.Unlock();
			}

			l_vboOut.Unbind();
			l_vboIn.Copy( l_vboOut, l_count );
		}

		l_geometryBuffers->Cleanup();
		l_transformFeedback->Cleanup();
		l_vboOut.Cleanup();
		l_vboIn.Cleanup();
		l_program->Cleanup();
		m_engine.GetRenderSystem()->GetMainContext()->EndCurrent();
	}

	void GlTransformFeedbackTest::TwoWaysParticleSystem()
	{
		struct Particle
		{
			enum class Type
			{
				Launcher,
				Shell,
				SecondaryShell
			};
			float m_type{ float( Type::Launcher ) };
			Castor::Point3f m_position{ 0, 0, 0 };
			Castor::Point3f m_velocity{ 0.0f, 0.0001f, 0.0f };
			float m_lifeTime{ 0.0f };
		};
		BufferDeclaration l_outputs
		{
			{
				BufferElementDeclaration( cuT( "Type1" ), 0u, ElementType::eFloat ),
				BufferElementDeclaration( cuT( "Position1" ), 0u, ElementType::eVec3 ),
				BufferElementDeclaration( cuT( "Velocity1" ), 0u, ElementType::eVec3 ),
				BufferElementDeclaration( cuT( "Age1" ), 0u, ElementType::eFloat ),
			}
		};
		BufferDeclaration l_inputs
		{
			{
				BufferElementDeclaration( cuT( "Type" ), 0u, ElementType::eFloat ),
				BufferElementDeclaration( cuT( "Position" ), uint32_t( ElementUsage::ePosition ), ElementType::eVec3 ),
				BufferElementDeclaration( cuT( "Velocity" ), 0u, ElementType::eVec3 ),
				BufferElementDeclaration( cuT( "Age" ), 0u, ElementType::eFloat ),
			}
		};
		Particle l_particle;

		// Shader program
		auto l_program = DoCreateParticleSystemShaderProgram( m_engine );
		auto l_deltaTime = l_program->CreateUniform< Uniform1f >( cuT( "gDeltaTimeMillis" ), ShaderType::eGeometry );
		auto l_total = l_program->CreateUniform< Uniform1f >( cuT( "gTime" ), ShaderType::eGeometry );
		auto l_launcherLifetime = l_program->CreateUniform< Uniform1f >( cuT( "gLauncherLifetime" ), ShaderType::eGeometry );
		auto l_shellLifetime = l_program->CreateUniform< Uniform1f >( cuT( "gShellLifetime" ), ShaderType::eGeometry );
		auto l_secondaryShellLifetime = l_program->CreateUniform< Uniform1f >( cuT( "gSecondaryShellLifetime" ), ShaderType::eGeometry );
		l_launcherLifetime->SetValue( 100.0f );
		l_shellLifetime->SetValue( 10000.0f );
		l_secondaryShellLifetime->SetValue( 2500.0f );
		l_program->SetTransformLayout( l_outputs );

		// Transform feedback
		TransformFeedbackSPtr l_transformFeedback[]
		{
			{ m_engine.GetRenderSystem()->CreateTransformFeedback( l_outputs, Topology::ePoints, *l_program ) },
			{ m_engine.GetRenderSystem()->CreateTransformFeedback( l_outputs, Topology::ePoints, *l_program ) }
		};

		// Pipeline
		RasteriserState l_rs;
		l_rs.SetDiscardPrimitives( true );
		auto l_pipeline = m_engine.GetRenderSystem()->CreateRenderPipeline( DepthStencilState{}, std::move( l_rs ), BlendState{}, MultisampleState{}, *l_program, PipelineFlags{} );

		// Input/Output VBOs
		VertexBuffer l_vbos[]
		{
			{ m_engine, l_inputs },
			{ m_engine, l_inputs }
		};

		// VAO
		GeometryBuffersSPtr l_geometryBuffers[]
		{
			{ m_engine.GetRenderSystem()->CreateGeometryBuffers( Topology::ePoints, *l_program ) },
			{ m_engine.GetRenderSystem()->CreateGeometryBuffers( Topology::ePoints, *l_program ) }
		};

		m_engine.GetRenderSystem()->GetMainContext()->SetCurrent();
		CT_CHECK( l_program->Initialise() );

		for ( uint32_t i = 0; i < 2; ++i )
		{
			l_vbos[i].Resize( 10000 * sizeof( l_particle ) );
			std::memcpy( l_vbos[i].data(), &l_particle, sizeof( l_particle ) );
			CT_CHECK( l_vbos[i].Initialise( BufferAccessType::eStatic, BufferAccessNature::eDraw ) );
		}

		for ( uint32_t i = 0; i < 2; ++i )
		{
			CT_CHECK( l_transformFeedback[i]->Initialise( { l_vbos[i] } ) );
			CT_CHECK( l_geometryBuffers[i]->Initialise( { l_vbos[i] }, nullptr ) );
		}

		double l_totalTime{ 0u };
		Particle l_particles[10000];
		uint32_t l_vtx{ 0u };
		uint32_t l_tfb{ 1u };

		for ( int i = 0; i < 10000; ++i )
		{
			auto l_time = 40.0f;
			l_totalTime += l_time;
			l_deltaTime->SetValue( float( l_time ) );
			l_total->SetValue( float( l_totalTime ) );

			l_pipeline->Apply();
			l_transformFeedback[l_tfb]->Bind();
			CT_CHECK( l_geometryBuffers[l_vtx]->Draw( std::max( 1u, l_transformFeedback[l_vtx]->GetWrittenPrimitives() ), 0 ) );
			l_transformFeedback[l_tfb]->Unbind();
			m_engine.GetRenderSystem()->GetMainContext()->SwapBuffers();

			l_vbos[l_tfb].Bind();
			auto l_count = uint32_t( sizeof( Particle ) * std::max( 1u, l_transformFeedback[l_tfb]->GetWrittenPrimitives() ) );
			auto l_buffer = reinterpret_cast< Particle * >( l_vbos[l_tfb].Lock( 0, l_count, AccessType::eRead ) );

			if ( l_buffer )
			{
				std::memcpy( l_particles, l_buffer, l_count );
				l_vbos[l_tfb].Unlock();
			}

			l_vbos[l_tfb].Unbind();
			l_vtx = l_tfb;
			l_tfb = 1 - l_tfb;
		}

		for ( uint32_t i = 0; i < 2; ++i )
		{
			l_geometryBuffers[i]->Cleanup();
			l_transformFeedback[i]->Cleanup();
			l_vbos[i].Cleanup();
		}

		l_program->Cleanup();
		m_engine.GetRenderSystem()->GetMainContext()->EndCurrent();
	}
}
