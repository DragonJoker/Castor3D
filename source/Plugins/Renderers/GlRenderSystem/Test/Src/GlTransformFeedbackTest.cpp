#include "GlTransformFeedbackTest.hpp"

#include <Engine.hpp>
#include <Mesh/Buffer/VertexBuffer.hpp>
#include <Mesh/Buffer/GeometryBuffers.hpp>
#include <Miscellaneous/TransformFeedback.hpp>
#include <Render/Pipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Shader/FrameVariableBuffer.hpp>
#include <Shader/OneFrameVariable.hpp>
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
				void main()
				{
					gl_Position = vec4( 0, 0, 0, 1 );
				}
			);
			auto l_program = p_engine.GetRenderSystem()->CreateShaderProgram();
			auto l_model = p_engine.GetRenderSystem()->GetGpuInformations().GetMaxShaderModel();
			l_program->SetSource( ShaderType::Vertex, l_model, l_vtx );
			return l_program;
		}

		ShaderProgramSPtr DoCreateBasicComputeProgram( Engine & p_engine )
		{
			String l_vtx = Glsl(
				in float inValue;
				out float outValue;

				void main()
				{
					outValue = sqrt( inValue );
				}
			);
			auto l_program = p_engine.GetRenderSystem()->CreateShaderProgram();
			auto l_model = p_engine.GetRenderSystem()->GetGpuInformations().GetMaxShaderModel();
			l_program->SetSource( ShaderType::Vertex, l_model, l_vtx );
			return l_program;
		}

		ShaderProgramSPtr DoCreateGeometryShaderProgram( Engine & p_engine )
		{
			String l_vtx = Glsl(
				in float inValue;
				out float geoValue;

				void main()
				{
					geoValue = sqrt( inValue );
				}
			);
			String l_geo = Glsl(
				layout( points ) in;
				layout( triangle_strip, max_vertices = 3 ) out;

				in float[] geoValue;
				out float outValue;

				void main()
				{
					for ( int i = 0; i < 3; i++ )
					{
						outValue = geoValue[0] + i;
						EmitVertex();
					}

					EndPrimitive();
				}
			);
			auto l_program = p_engine.GetRenderSystem()->CreateShaderProgram();
			auto l_model = p_engine.GetRenderSystem()->GetGpuInformations().GetMaxShaderModel();
			l_program->CreateObject( ShaderType::Geometry );
			l_program->SetSource( ShaderType::Vertex, l_model, l_vtx );
			l_program->SetSource( ShaderType::Geometry, l_model, l_geo );
			return l_program;
		}

		ShaderProgramSPtr DoCreateParticleSystemShaderProgram( Engine & p_engine )
		{
			String l_vtx = Glsl(
				layout( location = 0 ) in float Type;
				layout( location = 1 ) in vec3 Position;
				layout( location = 2 ) in vec3 Velocity;
				layout( location = 3 ) in float Age;

				out float Type0;
				out vec3 Position0;
				out vec3 Velocity0;
				out float Age0;

				void main()
				{
					Type0 = Type;
					Position0 = Position;
					Velocity0 = Velocity;
					Age0 = Age;
				}
			);
			String l_geo = Glsl(
				layout( points ) in;
				layout( points ) out;
				layout( max_vertices = 30 ) out;

				in float Type0[];
				in vec3 Position0[];
				in vec3 Velocity0[];
				in float Age0[];

				out float Type1;
				out vec3 Position1;
				out vec3 Velocity1;
				out float Age1;

				uniform float gDeltaTimeMillis;
				uniform float gTime;
				uniform float gLauncherLifetime;
				uniform float gShellLifetime;
				uniform float gSecondaryShellLifetime;

				vec3 GetRandomDir( float TexCoord )
				{
					vec3 Dir = vec3( TexCoord, TexCoord, TexCoord );
					Dir -= vec3( 0.5, 0.5, 0.5 );
					return Dir;
				}

				void main()
				{
					float Age = Age0[0] + gDeltaTimeMillis;

					if ( Type0[0] == 0.0f )
					{
						if ( Age >= gLauncherLifetime )
						{
							Type1 = 0.0f;
							Position1 = Position0[0];
							Velocity1 = Velocity0[0];
							Age1 = 0.0f;
							EmitVertex();
							EndPrimitive();

							Type1 = 1.0f;
							Position1 = Position0[0];
							vec3 Dir = GetRandomDir( gTime / 1000.0 );
							Dir.y = max( Dir.y, 0.5 );
							Velocity1 = normalize( Dir ) / 20.0;
							Age1 = 0.0f;
							EmitVertex();
							EndPrimitive();
						}
						else
						{
							Type1 = 0.0f;
							Position1 = Position0[0];
							Velocity1 = Velocity0[0];
							Age1 = Age;
							EmitVertex();
							EndPrimitive();
						}
					}
					else
					{
						float DeltaTimeSecs = gDeltaTimeMillis / 1000.0f;
						float t1 = Age0[0] / 1000.0;
						float t2 = Age / 1000.0;
						vec3 DeltaP = DeltaTimeSecs * Velocity0[0];
						vec3 DeltaV = vec3( DeltaTimeSecs ) * ( 0.0, -9.81, 0.0 );

						if ( Type0[0] == 1.0f )
						{
							if ( Age < gShellLifetime )
							{
								Type1 = 1.0f;
								Position1 = Position0[0] + DeltaP;
								Velocity1 = Velocity0[0] + DeltaV;
								Age1 = Age;
								EmitVertex();
								EndPrimitive();
							}
							else
							{
								for ( int i = 0; i < 10; i++ )
								{
									Type1 = 2.0f;
									Position1 = Position0[0];
									vec3 Dir = GetRandomDir( ( gTime + i ) / 1000.0 );
									Velocity1 = normalize( Dir ) / 20.0;
									Age1 = 0.0f;
									EmitVertex();
									EndPrimitive();
								}
							}
						}
						else
						{
							if ( Age < gSecondaryShellLifetime )
							{
								Type1 = 2.0f;
								Position1 = Position0[0] + DeltaP;
								Velocity1 = Velocity0[0] + DeltaV;
								Age1 = Age;
								EmitVertex();
								EndPrimitive();
							}
						}
					}
				}
			);
			auto l_program = p_engine.GetRenderSystem()->CreateShaderProgram();
			auto l_model = p_engine.GetRenderSystem()->GetGpuInformations().GetMaxShaderModel();
			l_program->CreateObject( ShaderType::Geometry );
			l_program->SetSource( ShaderType::Vertex, l_model, l_vtx );
			l_program->SetSource( ShaderType::Geometry, l_model, l_geo );
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
		auto l_transformFeedback = m_engine.GetRenderSystem()->CreateTransformFeedback( {}, Topology::Points, *l_program );

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
				BufferElementDeclaration( cuT( "outValue" ), 0, ElementType::Float, 0u )
			}
		};
		BufferDeclaration l_inputs
		{
			{
				BufferElementDeclaration( cuT( "inValue" ), 0, ElementType::Float, 0u )
			}
		};
		float l_data[] = { 1, 2, 3, 4, 5 };

		// Shader program
		auto l_program = DoCreateBasicComputeProgram( m_engine );

		// Transform feedback
		auto l_transformFeedback = m_engine.GetRenderSystem()->CreateTransformFeedback( l_outputs, Topology::Points, *l_program );
		l_program->SetTransformLayout( l_outputs );

		// Pipeline
		RasteriserState l_rs;
		l_rs.SetDiscardPrimitives( true );
		auto l_pipeline = m_engine.GetRenderSystem()->CreatePipeline( DepthStencilState{}, std::move( l_rs ), BlendState{}, MultisampleState{}, *l_program, PipelineFlags{} );

		// Input VBO
		VertexBuffer l_vboIn{ m_engine, l_inputs };
		l_vboIn.Resize( sizeof( l_data ) );
		std::memcpy( l_vboIn.data(), l_data, sizeof( l_data ) );

		// Output VBO
		VertexBuffer l_vboOut{ m_engine, l_outputs };
		l_vboOut.Resize( sizeof( l_data ) );

		// VAO
		auto l_geometryBuffers = m_engine.GetRenderSystem()->CreateGeometryBuffers( Topology::Points, *l_program );

		m_engine.GetRenderSystem()->GetMainContext()->SetCurrent();
		CT_CHECK( l_program->Initialise() );
		CT_CHECK( l_vboIn.Create() );
		CT_CHECK( l_vboOut.Create() );
		CT_CHECK( l_vboIn.Upload( BufferAccessType::Static, BufferAccessNature::Draw ) );
		CT_CHECK( l_vboOut.Upload( BufferAccessType::Static, BufferAccessNature::Read ) );
		CT_CHECK( l_transformFeedback->Initialise( { l_vboOut } ) );
		CT_CHECK( l_geometryBuffers->Initialise( { l_vboIn }, nullptr ) );

		for ( int j = 0; j < 2; ++j )
		{
			l_vboIn.Bind();
			auto l_buffer = reinterpret_cast< float * >( l_vboIn.Lock( 0, sizeof( l_data ), AccessType::Read ) );

			if ( l_buffer )
			{
				for ( int i = 0; i < 5; ++i )
				{
					CT_EQUAL( l_buffer[i], l_data[i] );
				}

				l_vboIn.Unlock();
			}

			l_vboIn.Unbind();

			l_pipeline->Apply();
			CT_CHECK( l_transformFeedback->Bind() );
			CT_CHECK( l_geometryBuffers->Draw( 5, 0 ) );
			l_transformFeedback->Unbind();
			m_engine.GetRenderSystem()->GetMainContext()->SwapBuffers();

			l_vboOut.Bind();
			l_buffer = reinterpret_cast< float * >( l_vboOut.Lock( 0, sizeof( l_data ), AccessType::Read ) );

			if ( l_buffer )
			{
				for ( int i = 0; i < 5; ++i )
				{
					CT_EQUAL( l_buffer[i], sqrt( l_data[i] ) );
					l_data[i] = sqrt( l_data[i] );
				}

				l_vboOut.Unlock();
			}

			l_vboOut.Unbind();
			l_vboIn.Copy( l_vboOut, sizeof( l_data ) );
		}

		l_geometryBuffers->Cleanup();
		l_transformFeedback->Cleanup();
		l_vboOut.Destroy();
		l_vboIn.Destroy();
		l_program->Cleanup();
		m_engine.GetRenderSystem()->GetMainContext()->EndCurrent();
	}

	void GlTransformFeedbackTest::GeometryShader()
	{
		BufferDeclaration l_outputs
		{
			{
				BufferElementDeclaration( cuT( "outValue" ), 0, ElementType::Float, 0u )
			}
		};
		BufferDeclaration l_inputs
		{
			{
				BufferElementDeclaration( cuT( "inValue" ), 0, ElementType::Float, 0u )
			}
		};
		float l_data[] = { 1, 2, 3, 4, 5 };

		// Shader program
		auto l_program = DoCreateGeometryShaderProgram( m_engine );

		// Transform feedback
		auto l_transformFeedback = m_engine.GetRenderSystem()->CreateTransformFeedback( l_outputs, Topology::Triangles, *l_program );
		l_program->SetTransformLayout( l_outputs );

		// Pipeline
		RasteriserState l_rs;
		l_rs.SetDiscardPrimitives( true );
		auto l_pipeline = m_engine.GetRenderSystem()->CreatePipeline( DepthStencilState{}, std::move( l_rs ), BlendState{}, MultisampleState{}, *l_program, PipelineFlags{} );

		// Input VBO
		VertexBuffer l_vboIn{ m_engine, l_inputs };
		l_vboIn.Resize( sizeof( l_data ) );
		std::memcpy( l_vboIn.data(), l_data, sizeof( l_data ) );

		// Output VBO
		VertexBuffer l_vboOut{ m_engine, l_outputs };
		l_vboOut.Resize( sizeof( l_data ) * 3 );

		// VAO
		auto l_geometryBuffers = m_engine.GetRenderSystem()->CreateGeometryBuffers( Topology::Points, *l_program );

		m_engine.GetRenderSystem()->GetMainContext()->SetCurrent();
		CT_CHECK( l_program->Initialise() );
		CT_CHECK( l_vboIn.Create() );
		CT_CHECK( l_vboOut.Create() );
		CT_CHECK( l_vboIn.Upload( BufferAccessType::Static, BufferAccessNature::Draw ) );
		CT_CHECK( l_vboOut.Upload( BufferAccessType::Static, BufferAccessNature::Read ) );
		CT_CHECK( l_transformFeedback->Initialise( { l_vboOut } ) );
		CT_CHECK( l_geometryBuffers->Initialise( { l_vboIn }, nullptr ) );

		l_pipeline->Apply();
		CT_CHECK( l_transformFeedback->Bind() );
		CT_CHECK( l_geometryBuffers->Draw( 5, 0 ) );
		l_transformFeedback->Unbind();
		m_engine.GetRenderSystem()->GetMainContext()->SwapBuffers();

		l_vboOut.Bind();
		auto l_buffer = reinterpret_cast< float * >( l_vboOut.Lock( 0, sizeof( l_data ) * 3, AccessType::Read ) );

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
		l_vboOut.Destroy();
		l_vboIn.Destroy();
		l_program->Cleanup();
		m_engine.GetRenderSystem()->GetMainContext()->EndCurrent();
	}

	void GlTransformFeedbackTest::VariableFeedback()
	{
		BufferDeclaration l_outputs
		{
			{
				BufferElementDeclaration( cuT( "outValue" ), 0, ElementType::Float, 0u )
			}
		};
		BufferDeclaration l_inputs
		{
			{
				BufferElementDeclaration( cuT( "inValue" ), 0, ElementType::Float, 0u )
			}
		};
		float l_data[] = { 1, 2, 3, 4, 5 };

		// Shader program
		auto l_program = DoCreateGeometryShaderProgram( m_engine );

		// Transform feedback
		auto l_transformFeedback = m_engine.GetRenderSystem()->CreateTransformFeedback( l_outputs, Topology::Triangles, *l_program );
		l_program->SetTransformLayout( l_outputs );

		// Pipeline
		RasteriserState l_rs;
		l_rs.SetDiscardPrimitives( true );
		auto l_pipeline = m_engine.GetRenderSystem()->CreatePipeline( DepthStencilState{}, std::move( l_rs ), BlendState{}, MultisampleState{}, *l_program, PipelineFlags{} );

		// Input VBO
		VertexBuffer l_vboIn{ m_engine, l_inputs };
		l_vboIn.Resize( sizeof( l_data ) );
		std::memcpy( l_vboIn.data(), l_data, sizeof( l_data ) );

		// Output VBO
		VertexBuffer l_vboOut{ m_engine, l_outputs };
		l_vboOut.Resize( sizeof( l_data ) * 3 );

		// VAO
		auto l_geometryBuffers = m_engine.GetRenderSystem()->CreateGeometryBuffers( Topology::Points, *l_program );

		m_engine.GetRenderSystem()->GetMainContext()->SetCurrent();
		CT_CHECK( l_program->Initialise() );
		CT_CHECK( l_vboIn.Create() );
		CT_CHECK( l_vboOut.Create() );
		CT_CHECK( l_vboIn.Upload( BufferAccessType::Static, BufferAccessNature::Draw ) );
		CT_CHECK( l_vboOut.Upload( BufferAccessType::Static, BufferAccessNature::Read ) );
		CT_CHECK( l_transformFeedback->Initialise( { l_vboOut } ) );
		CT_CHECK( l_geometryBuffers->Initialise( { l_vboIn }, nullptr ) );

		l_pipeline->Apply();
		CT_CHECK( l_transformFeedback->Bind() );
		CT_CHECK( l_geometryBuffers->Draw( 5, 0 ) );
		l_transformFeedback->Unbind();
		m_engine.GetRenderSystem()->GetMainContext()->SwapBuffers();

		CT_EQUAL( l_transformFeedback->GetWrittenPrimitives(), 5 );


		l_geometryBuffers->Cleanup();
		l_transformFeedback->Cleanup();
		l_vboOut.Destroy();
		l_vboIn.Destroy();
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
				BufferElementDeclaration( cuT( "Type1" ), 0u, ElementType::Float ),
				BufferElementDeclaration( cuT( "Position1" ), 0u, ElementType::Vec3 ),
				BufferElementDeclaration( cuT( "Velocity1" ), 0u, ElementType::Vec3 ),
				BufferElementDeclaration( cuT( "Age1" ), 0u, ElementType::Float ),
			}
		};
		BufferDeclaration l_inputs
		{
			{
				BufferElementDeclaration( cuT( "Type" ), 0u, ElementType::Float ),
				BufferElementDeclaration( cuT( "Position" ), uint32_t( ElementUsage::Position ), ElementType::Vec3 ),
				BufferElementDeclaration( cuT( "Velocity" ), 0u, ElementType::Vec3 ),
				BufferElementDeclaration( cuT( "Age" ), 0u, ElementType::Float ),
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
		auto l_deltaTime = l_program->CreateFrameVariable< OneFloatFrameVariable >( cuT( "gDeltaTimeMillis" ), ShaderType::Geometry );
		auto l_total = l_program->CreateFrameVariable< OneFloatFrameVariable >( cuT( "gTime" ), ShaderType::Geometry );
		auto l_launcherLifetime = l_program->CreateFrameVariable< OneFloatFrameVariable >( cuT( "gLauncherLifetime" ), ShaderType::Geometry );
		auto l_shellLifetime = l_program->CreateFrameVariable< OneFloatFrameVariable >( cuT( "gShellLifetime" ), ShaderType::Geometry );
		auto l_secondaryShellLifetime = l_program->CreateFrameVariable< OneFloatFrameVariable >( cuT( "gSecondaryShellLifetime" ), ShaderType::Geometry );
		l_launcherLifetime->SetValue( 100.0f );
		l_shellLifetime->SetValue( 10000.0f );
		l_secondaryShellLifetime->SetValue( 2500.0f );

		// Transform feedback
		TransformFeedbackSPtr l_transformFeedback{ m_engine.GetRenderSystem()->CreateTransformFeedback( l_outputs, Topology::Points, *l_program ) };
		l_program->SetTransformLayout( l_outputs );

		// Pipeline
		RasteriserState l_rs;
		l_rs.SetDiscardPrimitives( true );
		auto l_pipeline = m_engine.GetRenderSystem()->CreatePipeline( DepthStencilState{}, std::move( l_rs ), BlendState{}, MultisampleState{}, *l_program, PipelineFlags{} );

		// Input VBO
		VertexBuffer l_vboIn{ m_engine, l_inputs };
		l_vboIn.Resize( 10000 * sizeof( l_particle ) );
		std::memcpy( l_vboIn.data(), &l_particle, sizeof( l_particle ) );

		// Output VBO
		VertexBuffer l_vboOut{ m_engine, l_outputs };
		l_vboOut.Resize( 10000 * sizeof( l_particle ) );

		// VAO
		GeometryBuffersSPtr l_geometryBuffers{ m_engine.GetRenderSystem()->CreateGeometryBuffers( Topology::Points, *l_program ) };

		m_engine.GetRenderSystem()->GetMainContext()->SetCurrent();
		CT_CHECK( l_program->Initialise() );

		CT_CHECK( l_vboIn.Create() );
		CT_CHECK( l_vboOut.Create() );
		CT_CHECK( l_vboIn.Upload( BufferAccessType::Static, BufferAccessNature::Draw ) );
		CT_CHECK( l_vboOut.Upload( BufferAccessType::Static, BufferAccessNature::Read ) );
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
			auto l_buffer = reinterpret_cast< Particle * >( l_vboOut.Lock( 0, l_count, AccessType::Read ) );

			if ( l_buffer )
			{
				std::memcpy( l_particles, l_buffer, l_count );
				l_vboOut.Unlock();
			}

			l_vboOut.Unbind();

			l_pipeline->Apply();
			CT_CHECK( l_transformFeedback->Bind() );
			CT_CHECK( l_geometryBuffers->Draw( std::max( 1u, l_transformFeedback->GetWrittenPrimitives() ), 0 ) );
			l_transformFeedback->Unbind();
			m_engine.GetRenderSystem()->GetMainContext()->SwapBuffers();

			l_vboOut.Bind();
			l_count = uint32_t( sizeof( Particle ) * std::max( 1u, l_transformFeedback->GetWrittenPrimitives() ) );
			l_buffer = reinterpret_cast< Particle * >( l_vboOut.Lock( 0, l_count, AccessType::Read ) );

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
		l_vboOut.Destroy();
		l_vboIn.Destroy();
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
				BufferElementDeclaration( cuT( "Type1" ), 0u, ElementType::Float ),
				BufferElementDeclaration( cuT( "Position1" ), 0u, ElementType::Vec3 ),
				BufferElementDeclaration( cuT( "Velocity1" ), 0u, ElementType::Vec3 ),
				BufferElementDeclaration( cuT( "Age1" ), 0u, ElementType::Float ),
			}
		};
		BufferDeclaration l_inputs
		{
			{
				BufferElementDeclaration( cuT( "Type" ), 0u, ElementType::Float ),
				BufferElementDeclaration( cuT( "Position" ), uint32_t( ElementUsage::Position ), ElementType::Vec3 ),
				BufferElementDeclaration( cuT( "Velocity" ), 0u, ElementType::Vec3 ),
				BufferElementDeclaration( cuT( "Age" ), 0u, ElementType::Float ),
			}
		};
		Particle l_particle;

		// Shader program
		auto l_program = DoCreateParticleSystemShaderProgram( m_engine );
		auto l_deltaTime = l_program->CreateFrameVariable< OneFloatFrameVariable >( cuT( "gDeltaTimeMillis" ), ShaderType::Geometry );
		auto l_total = l_program->CreateFrameVariable< OneFloatFrameVariable >( cuT( "gTime" ), ShaderType::Geometry );
		auto l_launcherLifetime = l_program->CreateFrameVariable< OneFloatFrameVariable >( cuT( "gLauncherLifetime" ), ShaderType::Geometry );
		auto l_shellLifetime = l_program->CreateFrameVariable< OneFloatFrameVariable >( cuT( "gShellLifetime" ), ShaderType::Geometry );
		auto l_secondaryShellLifetime = l_program->CreateFrameVariable< OneFloatFrameVariable >( cuT( "gSecondaryShellLifetime" ), ShaderType::Geometry );
		l_launcherLifetime->SetValue( 100.0f );
		l_shellLifetime->SetValue( 10000.0f );
		l_secondaryShellLifetime->SetValue( 2500.0f );
		l_program->SetTransformLayout( l_outputs );

		// Transform feedback
		TransformFeedbackSPtr l_transformFeedback[]
		{
			{ m_engine.GetRenderSystem()->CreateTransformFeedback( l_outputs, Topology::Points, *l_program ) },
			{ m_engine.GetRenderSystem()->CreateTransformFeedback( l_outputs, Topology::Points, *l_program ) }
		};

		// Pipeline
		RasteriserState l_rs;
		l_rs.SetDiscardPrimitives( true );
		auto l_pipeline = m_engine.GetRenderSystem()->CreatePipeline( DepthStencilState{}, std::move( l_rs ), BlendState{}, MultisampleState{}, *l_program, PipelineFlags{} );

		// Input/Output VBOs
		VertexBuffer l_vbos[]
		{
			{ m_engine, l_inputs },
			{ m_engine, l_inputs }
		};

		// VAO
		GeometryBuffersSPtr l_geometryBuffers[]
		{
			{ m_engine.GetRenderSystem()->CreateGeometryBuffers( Topology::Points, *l_program ) },
			{ m_engine.GetRenderSystem()->CreateGeometryBuffers( Topology::Points, *l_program ) }
		};

		m_engine.GetRenderSystem()->GetMainContext()->SetCurrent();
		CT_CHECK( l_program->Initialise() );

		for ( uint32_t i = 0; i < 2; ++i )
		{
			CT_CHECK( l_vbos[i].Create() );
			l_vbos[i].Resize( 10000 * sizeof( l_particle ) );
			std::memcpy( l_vbos[i].data(), &l_particle, sizeof( l_particle ) );
			CT_CHECK( l_vbos[i].Upload( BufferAccessType::Static, BufferAccessNature::Draw ) );
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
			CT_CHECK( l_transformFeedback[l_tfb]->Bind() );
			CT_CHECK( l_geometryBuffers[l_vtx]->Draw( std::max( 1u, l_transformFeedback[l_vtx]->GetWrittenPrimitives() ), 0 ) );
			l_transformFeedback[l_tfb]->Unbind();
			m_engine.GetRenderSystem()->GetMainContext()->SwapBuffers();

			l_vbos[l_tfb].Bind();
			auto l_count = uint32_t( sizeof( Particle ) * std::max( 1u, l_transformFeedback[l_tfb]->GetWrittenPrimitives() ) );
			auto l_buffer = reinterpret_cast< Particle * >( l_vbos[l_tfb].Lock( 0, l_count, AccessType::Read ) );

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
			l_vbos[i].Destroy();
		}

		l_program->Cleanup();
		m_engine.GetRenderSystem()->GetMainContext()->EndCurrent();
	}
}
