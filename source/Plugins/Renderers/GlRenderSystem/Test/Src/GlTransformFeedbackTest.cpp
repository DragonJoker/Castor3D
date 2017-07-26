#include "GlTransformFeedbackTest.hpp"

#include <Engine.hpp>
#include <Mesh/Buffer/VertexBuffer.hpp>
#include <Mesh/Buffer/GeometryBuffers.hpp>
#include <Miscellaneous/TransformFeedback.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Shader/UniformBuffer.hpp>
#include <Shader/PushUniform.hpp>
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
		ShaderProgramSPtr DoCreateNoopProgram( Engine & engine )
		{
			String vtx = Glsl(
				void main()\n
				{\n
					gl_Position = vec4( 0, 0, 0, 1 );\n
				}\n
			);
			auto program = engine.GetRenderSystem()->CreateShaderProgram();
			program->CreateObject( ShaderType::eVertex );
			program->SetSource( ShaderType::eVertex, vtx );
			return program;
		}

		ShaderProgramSPtr DoCreateBasicComputeProgram( Engine & engine )
		{
			String vtx = Glsl(
				in float inValue;\n
				out float outValue;\n\n
				void main()\n
				{\n
					outValue = sqrt( inValue );\n
				}\n
			);
			auto program = engine.GetRenderSystem()->CreateShaderProgram();
			program->CreateObject( ShaderType::eVertex );
			program->SetSource( ShaderType::eVertex, vtx );
			return program;
		}

		ShaderProgramSPtr DoCreateGeometryShaderProgram( Engine & engine )
		{
			String vtx = Glsl(
				in float inValue;\n
				out float geoValue;\n\n
				void main()\n
				{\n
					geoValue = sqrt( inValue );\n
				}\n
			);
			String geo = Glsl(
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
			auto program = engine.GetRenderSystem()->CreateShaderProgram();
			program->CreateObject( ShaderType::eVertex );
			program->CreateObject( ShaderType::eGeometry );
			program->SetSource( ShaderType::eVertex, vtx );
			program->SetSource( ShaderType::eGeometry, geo );
			return program;
		}

		ShaderProgramSPtr DoCreateParticleSystemShaderProgram( Engine & engine )
		{
			String vtx = Glsl(
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
			String geo = Glsl(
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
			auto program = engine.GetRenderSystem()->CreateShaderProgram();
			program->CreateObject( ShaderType::eVertex );
			program->CreateObject( ShaderType::eGeometry );
			program->SetSource( ShaderType::eVertex, vtx );
			program->SetSource( ShaderType::eGeometry, geo );
			return program;
		}
	}

	GlTransformFeedbackTest::GlTransformFeedbackTest( Engine & engine )
		: GlTestCase{ "GlTransformFeedbackTest", engine }
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
		auto program = DoCreateNoopProgram( m_engine );
		auto transformFeedback = m_engine.GetRenderSystem()->CreateTransformFeedback( {}, Topology::ePoints, *program );

		m_engine.GetRenderSystem()->GetMainContext()->SetCurrent();
		CT_CHECK( program->Initialise() );
		CT_CHECK( transformFeedback->Initialise( {} ) );
		transformFeedback->Cleanup();
		program->Cleanup();
		m_engine.GetRenderSystem()->GetMainContext()->EndCurrent();
	}

	void GlTransformFeedbackTest::BasicCompute()
	{
		BufferDeclaration outputs
		{
			{
				BufferElementDeclaration( cuT( "outValue" ), 0, ElementType::eFloat, 0u )
			}
		};
		BufferDeclaration inputs
		{
			{
				BufferElementDeclaration( cuT( "inValue" ), 0, ElementType::eFloat, 0u )
			}
		};
		float data[] = { 1, 2, 3, 4, 5 };

		// Shader program
		auto program = DoCreateBasicComputeProgram( m_engine );

		// Input VBO
		VertexBuffer vboIn{ m_engine, inputs };
		vboIn.Resize( sizeof( data ) );
		std::memcpy( vboIn.GetData(), data, sizeof( data ) );

		// Output VBO
		VertexBuffer vboOut{ m_engine, outputs };
		vboOut.Resize( sizeof( data ) );

		// Transform feedback
		auto transformFeedback = m_engine.GetRenderSystem()->CreateTransformFeedback( outputs, Topology::ePoints, *program );
		program->SetTransformLayout( outputs );

		// VAO
		auto geometryBuffers = m_engine.GetRenderSystem()->CreateGeometryBuffers( Topology::ePoints, *program );

		// Pipeline
		RasteriserState rs;
		rs.SetDiscardPrimitives( true );
		auto pipeline = m_engine.GetRenderSystem()->CreateRenderPipeline( DepthStencilState{}, std::move( rs ), BlendState{}, MultisampleState{}, *program, PipelineFlags{} );

		m_engine.GetRenderSystem()->GetMainContext()->SetCurrent();
		CT_CHECK( program->Initialise() );
		CT_CHECK( vboIn.Initialise( BufferAccessType::eDynamic, BufferAccessNature::eDraw ) );
		CT_CHECK( vboOut.Initialise( BufferAccessType::eDynamic, BufferAccessNature::eDraw ) );
		CT_CHECK( transformFeedback->Initialise( { vboOut } ) );
		CT_CHECK( geometryBuffers->Initialise( { vboIn }, nullptr ) );

		for ( int j = 0; j < 2; ++j )
		{
			std::array< float, 5 > buffer;
			vboIn.Download( 0u, sizeof( data ), reinterpret_cast< uint8_t * >( buffer.data() ) );

			for ( int i = 0; i < 5; ++i )
			{
				CT_EQUAL( buffer[i], data[i] );
			}

			pipeline->Apply();
			transformFeedback->Bind();
			CT_CHECK( geometryBuffers->Draw( 5, 0 ) );
			transformFeedback->Unbind();
			CT_EQUAL( transformFeedback->GetWrittenPrimitives(), 5 );

			vboOut.Download( 0u, sizeof( data ), reinterpret_cast< uint8_t * >( buffer.data() ) );

			for ( int i = 0; i < 5; ++i )
			{
				CT_EQUAL( buffer[i], sqrt( data[i] ) );
				data[i] = sqrt( data[i] );
			}

			vboIn.Copy( vboOut, sizeof( data ) );
			m_engine.GetRenderSystem()->GetMainContext()->SwapBuffers();
		}

		geometryBuffers->Cleanup();
		transformFeedback->Cleanup();
		vboOut.Cleanup();
		vboIn.Cleanup();
		pipeline->Cleanup();
		m_engine.GetRenderSystem()->GetMainContext()->EndCurrent();
	}

	void GlTransformFeedbackTest::GeometryShader()
	{
		BufferDeclaration outputs
		{
			{
				BufferElementDeclaration( cuT( "outValue" ), 0, ElementType::eFloat, 0u )
			}
		};
		BufferDeclaration inputs
		{
			{
				BufferElementDeclaration( cuT( "inValue" ), 0, ElementType::eFloat, 0u )
			}
		};
		float data[] = { 1, 2, 3, 4, 5 };

		// Shader program
		auto program = DoCreateGeometryShaderProgram( m_engine );

		// Transform feedback
		auto transformFeedback = m_engine.GetRenderSystem()->CreateTransformFeedback( outputs, Topology::eTriangles, *program );
		program->SetTransformLayout( outputs );

		// Pipeline
		RasteriserState rs;
		rs.SetDiscardPrimitives( true );
		auto pipeline = m_engine.GetRenderSystem()->CreateRenderPipeline( DepthStencilState{}, std::move( rs ), BlendState{}, MultisampleState{}, *program, PipelineFlags{} );

		// Input VBO
		VertexBuffer vboIn{ m_engine, inputs };
		vboIn.Resize( sizeof( data ) );
		std::memcpy( vboIn.GetData(), data, sizeof( data ) );

		// Output VBO
		VertexBuffer vboOut{ m_engine, outputs };
		vboOut.Resize( sizeof( data ) * 3 );

		// VAO
		auto geometryBuffers = m_engine.GetRenderSystem()->CreateGeometryBuffers( Topology::ePoints, *program );

		m_engine.GetRenderSystem()->GetMainContext()->SetCurrent();
		CT_CHECK( program->Initialise() );
		CT_CHECK( vboIn.Initialise( BufferAccessType::eStatic, BufferAccessNature::eDraw ) );
		CT_CHECK( vboOut.Initialise( BufferAccessType::eStatic, BufferAccessNature::eRead ) );
		CT_CHECK( transformFeedback->Initialise( { vboOut } ) );
		CT_CHECK( geometryBuffers->Initialise( { vboIn }, nullptr ) );

		pipeline->Apply();
		transformFeedback->Bind();
		CT_CHECK( geometryBuffers->Draw( 5, 0 ) );
		transformFeedback->Unbind();
		m_engine.GetRenderSystem()->GetMainContext()->SwapBuffers();

		vboOut.Bind();
		auto buffer = reinterpret_cast< float * >( vboOut.Lock( 0, sizeof( data ) * 3, AccessType::eRead ) );

		if ( buffer )
		{
			for ( int j = 0; j < 5; ++j )
			{
				auto src = sqrt( data[j] );

				for ( int i = 0; i < 3; ++i )
				{
					CT_EQUAL( buffer[j * 3 + i], src + i );
				}
			}

			vboOut.Unlock();
		}

		vboOut.Unbind();

		geometryBuffers->Cleanup();
		transformFeedback->Cleanup();
		vboOut.Cleanup();
		vboIn.Cleanup();
		program->Cleanup();
		m_engine.GetRenderSystem()->GetMainContext()->EndCurrent();
	}

	void GlTransformFeedbackTest::VariableFeedback()
	{
		BufferDeclaration outputs
		{
			{
				BufferElementDeclaration( cuT( "outValue" ), 0, ElementType::eFloat, 0u )
			}
		};
		BufferDeclaration inputs
		{
			{
				BufferElementDeclaration( cuT( "inValue" ), 0, ElementType::eFloat, 0u )
			}
		};
		float data[] = { 1, 2, 3, 4, 5 };

		// Shader program
		auto program = DoCreateGeometryShaderProgram( m_engine );

		// Transform feedback
		auto transformFeedback = m_engine.GetRenderSystem()->CreateTransformFeedback( outputs, Topology::eTriangles, *program );
		program->SetTransformLayout( outputs );

		// Pipeline
		RasteriserState rs;
		rs.SetDiscardPrimitives( true );
		auto pipeline = m_engine.GetRenderSystem()->CreateRenderPipeline( DepthStencilState{}, std::move( rs ), BlendState{}, MultisampleState{}, *program, PipelineFlags{} );

		// Input VBO
		VertexBuffer vboIn{ m_engine, inputs };
		vboIn.Resize( sizeof( data ) );
		std::memcpy( vboIn.GetData(), data, sizeof( data ) );

		// Output VBO
		VertexBuffer vboOut{ m_engine, outputs };
		vboOut.Resize( sizeof( data ) * 3 );

		// VAO
		auto geometryBuffers = m_engine.GetRenderSystem()->CreateGeometryBuffers( Topology::ePoints, *program );

		m_engine.GetRenderSystem()->GetMainContext()->SetCurrent();
		CT_CHECK( program->Initialise() );
		CT_CHECK( vboIn.Initialise( BufferAccessType::eStatic, BufferAccessNature::eDraw ) );
		CT_CHECK( vboOut.Initialise( BufferAccessType::eStatic, BufferAccessNature::eRead ) );
		CT_CHECK( transformFeedback->Initialise( { vboOut } ) );
		CT_CHECK( geometryBuffers->Initialise( { vboIn }, nullptr ) );

		pipeline->Apply();
		transformFeedback->Bind();
		CT_CHECK( geometryBuffers->Draw( 5, 0 ) );
		transformFeedback->Unbind();
		m_engine.GetRenderSystem()->GetMainContext()->SwapBuffers();
		CT_EQUAL( transformFeedback->GetWrittenPrimitives(), 5 );

		geometryBuffers->Cleanup();
		transformFeedback->Cleanup();
		vboOut.Cleanup();
		vboIn.Cleanup();
		program->Cleanup();
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
		BufferDeclaration outputs
		{
			{
				BufferElementDeclaration( cuT( "Type1" ), 0u, ElementType::eFloat ),
				BufferElementDeclaration( cuT( "Position1" ), 0u, ElementType::eVec3 ),
				BufferElementDeclaration( cuT( "Velocity1" ), 0u, ElementType::eVec3 ),
				BufferElementDeclaration( cuT( "Age1" ), 0u, ElementType::eFloat ),
			}
		};
		BufferDeclaration inputs
		{
			{
				BufferElementDeclaration( cuT( "Type" ), 0u, ElementType::eFloat ),
				BufferElementDeclaration( cuT( "Position" ), uint32_t( ElementUsage::ePosition ), ElementType::eVec3 ),
				BufferElementDeclaration( cuT( "Velocity" ), 0u, ElementType::eVec3 ),
				BufferElementDeclaration( cuT( "Age" ), 0u, ElementType::eFloat ),
			}
		};
		Particle particle
		{
			float( Particle::Type::Launcher ),
			Point3r{ 0, 0, 0 },
			Point3r{ 0.0f, 0.0001f, 0.0f },
			0.0f
		};

		// Shader program
		auto program = DoCreateParticleSystemShaderProgram( m_engine );
		auto deltaTime = program->CreateUniform< UniformType::eFloat >( cuT( "gDeltaTimeMillis" ), ShaderType::eGeometry );
		auto total = program->CreateUniform< UniformType::eFloat >( cuT( "gTime" ), ShaderType::eGeometry );
		auto launcherLifetime = program->CreateUniform< UniformType::eFloat >( cuT( "gLauncherLifetime" ), ShaderType::eGeometry );
		auto shellLifetime = program->CreateUniform< UniformType::eFloat >( cuT( "gShellLifetime" ), ShaderType::eGeometry );
		auto secondaryShellLifetime = program->CreateUniform< UniformType::eFloat >( cuT( "gSecondaryShellLifetime" ), ShaderType::eGeometry );
		launcherLifetime->SetValue( 100.0f );
		shellLifetime->SetValue( 10000.0f );
		secondaryShellLifetime->SetValue( 2500.0f );

		// Transform feedback
		TransformFeedbackSPtr transformFeedback{ m_engine.GetRenderSystem()->CreateTransformFeedback( outputs, Topology::ePoints, *program ) };
		program->SetTransformLayout( outputs );

		// Pipeline
		RasteriserState rs;
		rs.SetDiscardPrimitives( true );
		auto pipeline = m_engine.GetRenderSystem()->CreateRenderPipeline( DepthStencilState{}, std::move( rs ), BlendState{}, MultisampleState{}, *program, PipelineFlags{} );

		// Input VBO
		VertexBuffer vboIn{ m_engine, inputs };
		vboIn.Resize( 10000 * sizeof( particle ) );
		std::memcpy( vboIn.GetData(), &particle, sizeof( particle ) );

		// Output VBO
		VertexBuffer vboOut{ m_engine, outputs };
		vboOut.Resize( 10000 * sizeof( particle ) );

		// VAO
		GeometryBuffersSPtr geometryBuffers{ m_engine.GetRenderSystem()->CreateGeometryBuffers( Topology::ePoints, *program ) };

		m_engine.GetRenderSystem()->GetMainContext()->SetCurrent();
		CT_CHECK( program->Initialise() );

		CT_CHECK( vboIn.Initialise( BufferAccessType::eStatic, BufferAccessNature::eDraw ) );
		CT_CHECK( vboOut.Initialise( BufferAccessType::eStatic, BufferAccessNature::eRead ) );
		CT_CHECK( transformFeedback->Initialise( { vboOut } ) );
		CT_CHECK( geometryBuffers->Initialise( { vboIn }, nullptr ) );

		double totalTime{ 0u };
		Particle particles[10000];

		for ( int i = 0; i < 10000; ++i )
		{
			auto time = 40.0f;
			totalTime += time;
			deltaTime->SetValue( float( time ) );
			total->SetValue( float( totalTime ) );

			vboOut.Bind();
			auto count = uint32_t( sizeof( Particle ) * std::max( 1u, transformFeedback->GetWrittenPrimitives() ) );
			auto buffer = reinterpret_cast< Particle * >( vboOut.Lock( 0, count, AccessType::eRead ) );

			if ( buffer )
			{
				std::memcpy( particles, buffer, count );
				vboOut.Unlock();
			}

			vboOut.Unbind();

			pipeline->Apply();
			transformFeedback->Bind();
			CT_CHECK( geometryBuffers->Draw( std::max( 1u, transformFeedback->GetWrittenPrimitives() ), 0 ) );
			transformFeedback->Unbind();
			m_engine.GetRenderSystem()->GetMainContext()->SwapBuffers();

			vboOut.Bind();
			count = uint32_t( sizeof( Particle ) * std::max( 1u, transformFeedback->GetWrittenPrimitives() ) );
			buffer = reinterpret_cast< Particle * >( vboOut.Lock( 0, count, AccessType::eRead ) );

			if ( buffer )
			{
				std::memcpy( particles, buffer, count );
				vboOut.Unlock();
			}

			vboOut.Unbind();
			vboIn.Copy( vboOut, count );
		}

		geometryBuffers->Cleanup();
		transformFeedback->Cleanup();
		vboOut.Cleanup();
		vboIn.Cleanup();
		program->Cleanup();
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
		BufferDeclaration outputs
		{
			{
				BufferElementDeclaration( cuT( "Type1" ), 0u, ElementType::eFloat ),
				BufferElementDeclaration( cuT( "Position1" ), 0u, ElementType::eVec3 ),
				BufferElementDeclaration( cuT( "Velocity1" ), 0u, ElementType::eVec3 ),
				BufferElementDeclaration( cuT( "Age1" ), 0u, ElementType::eFloat ),
			}
		};
		BufferDeclaration inputs
		{
			{
				BufferElementDeclaration( cuT( "Type" ), 0u, ElementType::eFloat ),
				BufferElementDeclaration( cuT( "Position" ), uint32_t( ElementUsage::ePosition ), ElementType::eVec3 ),
				BufferElementDeclaration( cuT( "Velocity" ), 0u, ElementType::eVec3 ),
				BufferElementDeclaration( cuT( "Age" ), 0u, ElementType::eFloat ),
			}
		};
		Particle particle;

		// Shader program
		auto program = DoCreateParticleSystemShaderProgram( m_engine );
		auto deltaTime = program->CreateUniform< UniformType::eFloat >( cuT( "gDeltaTimeMillis" ), ShaderType::eGeometry );
		auto total = program->CreateUniform< UniformType::eFloat >( cuT( "gTime" ), ShaderType::eGeometry );
		auto launcherLifetime = program->CreateUniform< UniformType::eFloat >( cuT( "gLauncherLifetime" ), ShaderType::eGeometry );
		auto shellLifetime = program->CreateUniform< UniformType::eFloat >( cuT( "gShellLifetime" ), ShaderType::eGeometry );
		auto secondaryShellLifetime = program->CreateUniform< UniformType::eFloat >( cuT( "gSecondaryShellLifetime" ), ShaderType::eGeometry );
		launcherLifetime->SetValue( 100.0f );
		shellLifetime->SetValue( 10000.0f );
		secondaryShellLifetime->SetValue( 2500.0f );
		program->SetTransformLayout( outputs );

		// Transform feedback
		TransformFeedbackSPtr transformFeedback[]
		{
			{ m_engine.GetRenderSystem()->CreateTransformFeedback( outputs, Topology::ePoints, *program ) },
			{ m_engine.GetRenderSystem()->CreateTransformFeedback( outputs, Topology::ePoints, *program ) }
		};

		// Pipeline
		RasteriserState rs;
		rs.SetDiscardPrimitives( true );
		auto pipeline = m_engine.GetRenderSystem()->CreateRenderPipeline( DepthStencilState{}, std::move( rs ), BlendState{}, MultisampleState{}, *program, PipelineFlags{} );

		// Input/Output VBOs
		VertexBuffer vbos[]
		{
			{ m_engine, inputs },
			{ m_engine, inputs }
		};

		// VAO
		GeometryBuffersSPtr geometryBuffers[]
		{
			{ m_engine.GetRenderSystem()->CreateGeometryBuffers( Topology::ePoints, *program ) },
			{ m_engine.GetRenderSystem()->CreateGeometryBuffers( Topology::ePoints, *program ) }
		};

		m_engine.GetRenderSystem()->GetMainContext()->SetCurrent();
		CT_CHECK( program->Initialise() );

		for ( uint32_t i = 0; i < 2; ++i )
		{
			vbos[i].Resize( 10000 * sizeof( particle ) );
			std::memcpy( vbos[i].GetData(), &particle, sizeof( particle ) );
			CT_CHECK( vbos[i].Initialise( BufferAccessType::eStatic, BufferAccessNature::eDraw ) );
		}

		for ( uint32_t i = 0; i < 2; ++i )
		{
			CT_CHECK( transformFeedback[i]->Initialise( { vbos[i] } ) );
			CT_CHECK( geometryBuffers[i]->Initialise( { vbos[i] }, nullptr ) );
		}

		double totalTime{ 0u };
		Particle particles[10000];
		uint32_t vtx{ 0u };
		uint32_t tfb{ 1u };

		for ( int i = 0; i < 10000; ++i )
		{
			auto time = 40.0f;
			totalTime += time;
			deltaTime->SetValue( float( time ) );
			total->SetValue( float( totalTime ) );

			pipeline->Apply();
			transformFeedback[tfb]->Bind();
			CT_CHECK( geometryBuffers[vtx]->Draw( std::max( 1u, transformFeedback[vtx]->GetWrittenPrimitives() ), 0 ) );
			transformFeedback[tfb]->Unbind();
			m_engine.GetRenderSystem()->GetMainContext()->SwapBuffers();

			vbos[tfb].Bind();
			auto count = uint32_t( sizeof( Particle ) * std::max( 1u, transformFeedback[tfb]->GetWrittenPrimitives() ) );
			auto buffer = reinterpret_cast< Particle * >( vbos[tfb].Lock( 0, count, AccessType::eRead ) );

			if ( buffer )
			{
				std::memcpy( particles, buffer, count );
				vbos[tfb].Unlock();
			}

			vbos[tfb].Unbind();
			vtx = tfb;
			tfb = 1 - tfb;
		}

		for ( uint32_t i = 0; i < 2; ++i )
		{
			geometryBuffers[i]->Cleanup();
			transformFeedback[i]->Cleanup();
			vbos[i].Cleanup();
		}

		program->Cleanup();
		m_engine.GetRenderSystem()->GetMainContext()->EndCurrent();
	}
}
