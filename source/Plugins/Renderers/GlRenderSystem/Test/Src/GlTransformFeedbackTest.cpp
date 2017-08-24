#include "GlTransformFeedbackTest.hpp"

#include <Engine.hpp>
#include <Mesh/Buffer/VertexBuffer.hpp>
#include <Mesh/Buffer/GeometryBuffers.hpp>
#include <Miscellaneous/TransformFeedback.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Shader/UniformBuffer.hpp>
#include <Shader/ShaderProgram.hpp>
#include <Shader/Uniform/PushUniform.hpp>
#include <State/RasteriserState.hpp>

#include <Buffer/GlBuffer.hpp>

using namespace castor;
using namespace castor3d;

#ifdef max
#	undef max
#endif

#define Glsl( x ) cuT( "#version 330\n" ) cuT( #x )

namespace Testing
{
	namespace
	{
		ShaderProgramSPtr doCreateNoopProgram( Engine & engine )
		{
			String vtx = Glsl(
				void main()\n
				{\n
					gl_Position = vec4( 0, 0, 0, 1 );\n
				}\n
			);
			auto program = engine.getRenderSystem()->createShaderProgram();
			program->createObject( ShaderType::eVertex );
			program->setSource( ShaderType::eVertex, vtx );
			return program;
		}

		ShaderProgramSPtr doCreateBasicComputeProgram( Engine & engine )
		{
			String vtx = Glsl(
				in float inValue;\n
				out float outValue;\n\n
				void main()\n
				{\n
					outValue = sqrt( inValue );\n
				}\n
			);
			auto program = engine.getRenderSystem()->createShaderProgram();
			program->createObject( ShaderType::eVertex );
			program->setSource( ShaderType::eVertex, vtx );
			return program;
		}

		ShaderProgramSPtr doCreateGeometryShaderProgram( Engine & engine )
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
						emitVertex();\n
					}\n
					endPrimitive();\n
				}\n
			);
			auto program = engine.getRenderSystem()->createShaderProgram();
			program->createObject( ShaderType::eVertex );
			program->createObject( ShaderType::eGeometry );
			program->setSource( ShaderType::eVertex, vtx );
			program->setSource( ShaderType::eGeometry, geo );
			return program;
		}

		ShaderProgramSPtr doCreateParticleSystemShaderProgram( Engine & engine )
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
				vec3 getRandomDir( float TexCoord )\n
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
							emitVertex();\n
							endPrimitive();\n\n
							Type1 = 1.0f;\n
							Position1 = Position0[0];\n
							vec3 Dir = getRandomDir( gTime / 1000.0 );\n
							Dir.y = max( Dir.y, 0.5 );\n
							Velocity1 = normalize( Dir ) / 20.0;\n
							Age1 = 0.0f;\n
							emitVertex();\n
							endPrimitive();\n
						}\n
						else\n
						{\n
							Type1 = 0.0f;\n
							Position1 = Position0[0];\n
							Velocity1 = Velocity0[0];\n
							Age1 = Age;\n
							emitVertex();\n
							endPrimitive();\n
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
								emitVertex();\n
								endPrimitive();\n
							}\n
							else\n
							{\n
								for ( int i = 0; i < 10; i++ )\n
								{\n
									Type1 = 2.0f;\n
									Position1 = Position0[0];\n
									vec3 Dir = getRandomDir( ( gTime + i ) / 1000.0 );\n
									Velocity1 = normalize( Dir ) / 20.0;\n
									Age1 = 0.0f;\n
									emitVertex();\n
									endPrimitive();\n
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
								emitVertex();\n
								endPrimitive();\n
							}\n
						}\n
					}\n
				}\n
			);
			auto program = engine.getRenderSystem()->createShaderProgram();
			program->createObject( ShaderType::eVertex );
			program->createObject( ShaderType::eGeometry );
			program->setSource( ShaderType::eVertex, vtx );
			program->setSource( ShaderType::eGeometry, geo );
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

	void GlTransformFeedbackTest::doRegisterTests()
	{
		doRegisterTest( "GlTransformFeedbackTest::Creation", std::bind( &GlTransformFeedbackTest::Creation, this ) );
		doRegisterTest( "GlTransformFeedbackTest::BasicCompute", std::bind( &GlTransformFeedbackTest::BasicCompute, this ) );
		doRegisterTest( "GlTransformFeedbackTest::GeometryShader", std::bind( &GlTransformFeedbackTest::GeometryShader, this ) );
		doRegisterTest( "GlTransformFeedbackTest::VariableFeedback", std::bind( &GlTransformFeedbackTest::VariableFeedback, this ) );
		doRegisterTest( "GlTransformFeedbackTest::ParticleSystem", std::bind( &GlTransformFeedbackTest::ParticleSystem, this ) );
		doRegisterTest( "GlTransformFeedbackTest::TwoWaysParticleSystem", std::bind( &GlTransformFeedbackTest::TwoWaysParticleSystem, this ) );
	}

	void GlTransformFeedbackTest::Creation()
	{
		auto program = doCreateNoopProgram( m_engine );
		auto transformFeedback = m_engine.getRenderSystem()->createTransformFeedback( {}, Topology::ePoints, *program );

		m_engine.getRenderSystem()->getMainContext()->setCurrent();
		CT_CHECK( program->initialise() );
		CT_CHECK( transformFeedback->initialise( {} ) );
		transformFeedback->cleanup();
		program->cleanup();
		m_engine.getRenderSystem()->getMainContext()->endCurrent();
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
		auto program = doCreateBasicComputeProgram( m_engine );

		// Input VBO
		VertexBuffer vboIn{ m_engine, inputs };
		vboIn.resize( sizeof( data ) );
		std::memcpy( vboIn.getData(), data, sizeof( data ) );

		// Output VBO
		VertexBuffer vboOut{ m_engine, outputs };
		vboOut.resize( sizeof( data ) );

		// Transform feedback
		auto transformFeedback = m_engine.getRenderSystem()->createTransformFeedback( outputs, Topology::ePoints, *program );
		program->setTransformLayout( outputs );

		// VAO
		auto geometryBuffers = m_engine.getRenderSystem()->createGeometryBuffers( Topology::ePoints, *program );

		// Pipeline
		RasteriserState rs;
		rs.setDiscardPrimitives( true );
		auto pipeline = m_engine.getRenderSystem()->createRenderPipeline( DepthStencilState{}, std::move( rs ), BlendState{}, MultisampleState{}, *program, PipelineFlags{} );

		m_engine.getRenderSystem()->getMainContext()->setCurrent();
		CT_CHECK( program->initialise() );
		CT_CHECK( vboIn.initialise( BufferAccessType::eDynamic, BufferAccessNature::eDraw ) );
		CT_CHECK( vboOut.initialise( BufferAccessType::eDynamic, BufferAccessNature::eDraw ) );
		CT_CHECK( transformFeedback->initialise( { vboOut } ) );
		CT_CHECK( geometryBuffers->initialise( { vboIn }, nullptr ) );

		for ( int j = 0; j < 2; ++j )
		{
			std::array< float, 5 > buffer;
			vboIn.download( 0u, sizeof( data ), reinterpret_cast< uint8_t * >( buffer.data() ) );

			for ( int i = 0; i < 5; ++i )
			{
				CT_EQUAL( buffer[i], data[i] );
			}

			pipeline->apply();
			transformFeedback->bind();
			CT_CHECK( geometryBuffers->draw( 5, 0u ) );
			transformFeedback->unbind();
			CT_EQUAL( transformFeedback->getWrittenPrimitives(), 5 );

			vboOut.download( 0u, sizeof( data ), reinterpret_cast< uint8_t * >( buffer.data() ) );

			for ( int i = 0; i < 5; ++i )
			{
				CT_EQUAL( buffer[i], sqrt( data[i] ) );
				data[i] = sqrt( data[i] );
			}

			vboIn.copy( vboOut, sizeof( data ) );
			m_engine.getRenderSystem()->getMainContext()->swapBuffers();
		}

		geometryBuffers->cleanup();
		transformFeedback->cleanup();
		vboOut.cleanup();
		vboIn.cleanup();
		pipeline->cleanup();
		m_engine.getRenderSystem()->getMainContext()->endCurrent();
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
		auto program = doCreateGeometryShaderProgram( m_engine );

		// Transform feedback
		auto transformFeedback = m_engine.getRenderSystem()->createTransformFeedback( outputs, Topology::eTriangles, *program );
		program->setTransformLayout( outputs );

		// Pipeline
		RasteriserState rs;
		rs.setDiscardPrimitives( true );
		auto pipeline = m_engine.getRenderSystem()->createRenderPipeline( DepthStencilState{}, std::move( rs ), BlendState{}, MultisampleState{}, *program, PipelineFlags{} );

		// Input VBO
		VertexBuffer vboIn{ m_engine, inputs };
		vboIn.resize( sizeof( data ) );
		std::memcpy( vboIn.getData(), data, sizeof( data ) );

		// Output VBO
		VertexBuffer vboOut{ m_engine, outputs };
		vboOut.resize( sizeof( data ) * 3 );

		// VAO
		auto geometryBuffers = m_engine.getRenderSystem()->createGeometryBuffers( Topology::ePoints, *program );

		m_engine.getRenderSystem()->getMainContext()->setCurrent();
		CT_CHECK( program->initialise() );
		CT_CHECK( vboIn.initialise( BufferAccessType::eStatic, BufferAccessNature::eDraw ) );
		CT_CHECK( vboOut.initialise( BufferAccessType::eStatic, BufferAccessNature::eRead ) );
		CT_CHECK( transformFeedback->initialise( { vboOut } ) );
		CT_CHECK( geometryBuffers->initialise( { vboIn }, nullptr ) );

		pipeline->apply();
		transformFeedback->bind();
		CT_CHECK( geometryBuffers->draw( 5, 0u ) );
		transformFeedback->unbind();
		m_engine.getRenderSystem()->getMainContext()->swapBuffers();

		vboOut.bind();
		auto buffer = reinterpret_cast< float * >( vboOut.lock( 0, sizeof( data ) * 3, AccessType::eRead ) );

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

			vboOut.unlock();
		}

		vboOut.unbind();

		geometryBuffers->cleanup();
		transformFeedback->cleanup();
		vboOut.cleanup();
		vboIn.cleanup();
		program->cleanup();
		m_engine.getRenderSystem()->getMainContext()->endCurrent();
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
		auto program = doCreateGeometryShaderProgram( m_engine );

		// Transform feedback
		auto transformFeedback = m_engine.getRenderSystem()->createTransformFeedback( outputs, Topology::eTriangles, *program );
		program->setTransformLayout( outputs );

		// Pipeline
		RasteriserState rs;
		rs.setDiscardPrimitives( true );
		auto pipeline = m_engine.getRenderSystem()->createRenderPipeline( DepthStencilState{}, std::move( rs ), BlendState{}, MultisampleState{}, *program, PipelineFlags{} );

		// Input VBO
		VertexBuffer vboIn{ m_engine, inputs };
		vboIn.resize( sizeof( data ) );
		std::memcpy( vboIn.getData(), data, sizeof( data ) );

		// Output VBO
		VertexBuffer vboOut{ m_engine, outputs };
		vboOut.resize( sizeof( data ) * 3 );

		// VAO
		auto geometryBuffers = m_engine.getRenderSystem()->createGeometryBuffers( Topology::ePoints, *program );

		m_engine.getRenderSystem()->getMainContext()->setCurrent();
		CT_CHECK( program->initialise() );
		CT_CHECK( vboIn.initialise( BufferAccessType::eStatic, BufferAccessNature::eDraw ) );
		CT_CHECK( vboOut.initialise( BufferAccessType::eStatic, BufferAccessNature::eRead ) );
		CT_CHECK( transformFeedback->initialise( { vboOut } ) );
		CT_CHECK( geometryBuffers->initialise( { vboIn }, nullptr ) );

		pipeline->apply();
		transformFeedback->bind();
		CT_CHECK( geometryBuffers->draw( 5, 0u ) );
		transformFeedback->unbind();
		m_engine.getRenderSystem()->getMainContext()->swapBuffers();
		CT_EQUAL( transformFeedback->getWrittenPrimitives(), 5 );

		geometryBuffers->cleanup();
		transformFeedback->cleanup();
		vboOut.cleanup();
		vboIn.cleanup();
		program->cleanup();
		m_engine.getRenderSystem()->getMainContext()->endCurrent();
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
			castor::Point3f m_position;
			castor::Point3f m_velocity;
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
		auto program = doCreateParticleSystemShaderProgram( m_engine );
		auto deltaTime = program->createUniform< UniformType::eFloat >( cuT( "gDeltaTimeMillis" ), ShaderType::eGeometry );
		auto total = program->createUniform< UniformType::eFloat >( cuT( "gTime" ), ShaderType::eGeometry );
		auto launcherLifetime = program->createUniform< UniformType::eFloat >( cuT( "gLauncherLifetime" ), ShaderType::eGeometry );
		auto shellLifetime = program->createUniform< UniformType::eFloat >( cuT( "gShellLifetime" ), ShaderType::eGeometry );
		auto secondaryShellLifetime = program->createUniform< UniformType::eFloat >( cuT( "gSecondaryShellLifetime" ), ShaderType::eGeometry );
		launcherLifetime->setValue( 100.0f );
		shellLifetime->setValue( 10000.0f );
		secondaryShellLifetime->setValue( 2500.0f );

		// Transform feedback
		TransformFeedbackSPtr transformFeedback{ m_engine.getRenderSystem()->createTransformFeedback( outputs, Topology::ePoints, *program ) };
		program->setTransformLayout( outputs );

		// Pipeline
		RasteriserState rs;
		rs.setDiscardPrimitives( true );
		auto pipeline = m_engine.getRenderSystem()->createRenderPipeline( DepthStencilState{}, std::move( rs ), BlendState{}, MultisampleState{}, *program, PipelineFlags{} );

		// Input VBO
		VertexBuffer vboIn{ m_engine, inputs };
		vboIn.resize( 10000 * sizeof( particle ) );
		std::memcpy( vboIn.getData(), &particle, sizeof( particle ) );

		// Output VBO
		VertexBuffer vboOut{ m_engine, outputs };
		vboOut.resize( 10000 * sizeof( particle ) );

		// VAO
		GeometryBuffersSPtr geometryBuffers{ m_engine.getRenderSystem()->createGeometryBuffers( Topology::ePoints, *program ) };

		m_engine.getRenderSystem()->getMainContext()->setCurrent();
		CT_CHECK( program->initialise() );

		CT_CHECK( vboIn.initialise( BufferAccessType::eStatic, BufferAccessNature::eDraw ) );
		CT_CHECK( vboOut.initialise( BufferAccessType::eStatic, BufferAccessNature::eRead ) );
		CT_CHECK( transformFeedback->initialise( { vboOut } ) );
		CT_CHECK( geometryBuffers->initialise( { vboIn }, nullptr ) );

		double totalTime{ 0u };
		Particle particles[10000];

		for ( int i = 0; i < 10000; ++i )
		{
			auto time = 40.0f;
			totalTime += time;
			deltaTime->setValue( float( time ) );
			total->setValue( float( totalTime ) );

			vboOut.bind();
			auto count = uint32_t( sizeof( Particle ) * std::max( 1u, transformFeedback->getWrittenPrimitives() ) );
			auto buffer = reinterpret_cast< Particle * >( vboOut.lock( 0, count, AccessType::eRead ) );

			if ( buffer )
			{
				std::memcpy( particles, buffer, count );
				vboOut.unlock();
			}

			vboOut.unbind();

			pipeline->apply();
			transformFeedback->bind();
			CT_CHECK( geometryBuffers->draw( std::max( 1u, transformFeedback->getWrittenPrimitives() ), 0u ) );
			transformFeedback->unbind();
			m_engine.getRenderSystem()->getMainContext()->swapBuffers();

			vboOut.bind();
			count = uint32_t( sizeof( Particle ) * std::max( 1u, transformFeedback->getWrittenPrimitives() ) );
			buffer = reinterpret_cast< Particle * >( vboOut.lock( 0, count, AccessType::eRead ) );

			if ( buffer )
			{
				std::memcpy( particles, buffer, count );
				vboOut.unlock();
			}

			vboOut.unbind();
			vboIn.copy( vboOut, count );
		}

		geometryBuffers->cleanup();
		transformFeedback->cleanup();
		vboOut.cleanup();
		vboIn.cleanup();
		program->cleanup();
		m_engine.getRenderSystem()->getMainContext()->endCurrent();
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
			castor::Point3f m_position{ 0, 0, 0 };
			castor::Point3f m_velocity{ 0.0f, 0.0001f, 0.0f };
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
		auto program = doCreateParticleSystemShaderProgram( m_engine );
		auto deltaTime = program->createUniform< UniformType::eFloat >( cuT( "gDeltaTimeMillis" ), ShaderType::eGeometry );
		auto total = program->createUniform< UniformType::eFloat >( cuT( "gTime" ), ShaderType::eGeometry );
		auto launcherLifetime = program->createUniform< UniformType::eFloat >( cuT( "gLauncherLifetime" ), ShaderType::eGeometry );
		auto shellLifetime = program->createUniform< UniformType::eFloat >( cuT( "gShellLifetime" ), ShaderType::eGeometry );
		auto secondaryShellLifetime = program->createUniform< UniformType::eFloat >( cuT( "gSecondaryShellLifetime" ), ShaderType::eGeometry );
		launcherLifetime->setValue( 100.0f );
		shellLifetime->setValue( 10000.0f );
		secondaryShellLifetime->setValue( 2500.0f );
		program->setTransformLayout( outputs );

		// Transform feedback
		TransformFeedbackSPtr transformFeedback[]
		{
			{ m_engine.getRenderSystem()->createTransformFeedback( outputs, Topology::ePoints, *program ) },
			{ m_engine.getRenderSystem()->createTransformFeedback( outputs, Topology::ePoints, *program ) }
		};

		// Pipeline
		RasteriserState rs;
		rs.setDiscardPrimitives( true );
		auto pipeline = m_engine.getRenderSystem()->createRenderPipeline( DepthStencilState{}, std::move( rs ), BlendState{}, MultisampleState{}, *program, PipelineFlags{} );

		// Input/Output VBOs
		VertexBuffer vbos[]
		{
			{ m_engine, inputs },
			{ m_engine, inputs }
		};

		// VAO
		GeometryBuffersSPtr geometryBuffers[]
		{
			{ m_engine.getRenderSystem()->createGeometryBuffers( Topology::ePoints, *program ) },
			{ m_engine.getRenderSystem()->createGeometryBuffers( Topology::ePoints, *program ) }
		};

		m_engine.getRenderSystem()->getMainContext()->setCurrent();
		CT_CHECK( program->initialise() );

		for ( uint32_t i = 0; i < 2; ++i )
		{
			vbos[i].resize( 10000 * sizeof( particle ) );
			std::memcpy( vbos[i].getData(), &particle, sizeof( particle ) );
			CT_CHECK( vbos[i].initialise( BufferAccessType::eStatic, BufferAccessNature::eDraw ) );
		}

		for ( uint32_t i = 0; i < 2; ++i )
		{
			CT_CHECK( transformFeedback[i]->initialise( { vbos[i] } ) );
			CT_CHECK( geometryBuffers[i]->initialise( { vbos[i] }, nullptr ) );
		}

		double totalTime{ 0u };
		Particle particles[10000];
		uint32_t vtx{ 0u };
		uint32_t tfb{ 1u };

		for ( int i = 0; i < 10000; ++i )
		{
			auto time = 40.0f;
			totalTime += time;
			deltaTime->setValue( float( time ) );
			total->setValue( float( totalTime ) );

			pipeline->apply();
			transformFeedback[tfb]->bind();
			CT_CHECK( geometryBuffers[vtx]->draw( std::max( 1u, transformFeedback[vtx]->getWrittenPrimitives() ), 0u ) );
			transformFeedback[tfb]->unbind();
			m_engine.getRenderSystem()->getMainContext()->swapBuffers();

			vbos[tfb].bind();
			auto count = uint32_t( sizeof( Particle ) * std::max( 1u, transformFeedback[tfb]->getWrittenPrimitives() ) );
			auto buffer = reinterpret_cast< Particle * >( vbos[tfb].lock( 0, count, AccessType::eRead ) );

			if ( buffer )
			{
				std::memcpy( particles, buffer, count );
				vbos[tfb].unlock();
			}

			vbos[tfb].unbind();
			vtx = tfb;
			tfb = 1 - tfb;
		}

		for ( uint32_t i = 0; i < 2; ++i )
		{
			geometryBuffers[i]->cleanup();
			transformFeedback[i]->cleanup();
			vbos[i].cleanup();
		}

		program->cleanup();
		m_engine.getRenderSystem()->getMainContext()->endCurrent();
	}
}
