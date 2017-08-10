#include "GlAtomicCounterBufferTest.hpp"

#include <Engine.hpp>
#include <Mesh/Buffer/VertexBuffer.hpp>
#include <Miscellaneous/ComputePipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Shader/AtomicCounterBuffer.hpp>
#include <Shader/ShaderProgram.hpp>

#include <Buffer/GlBuffer.hpp>

using namespace castor;
using namespace castor3d;

#ifdef max
#	undef max
#endif

#define Glsl( x ) cuT( "#version 430\n#extension GL_ARB_compute_variable_group_size : enable\n" ) cuT( #x )

namespace Testing
{
	namespace
	{
		ShaderProgramSPtr doCreateNoopProgram( Engine & engine )
		{
			String vtx = Glsl(
				layout( binding = 0, offset = 0 ) uniform atomic_uint counter;\n
				layout( local_size_variable ) in;\n
				void main()\n
				{\n
				}\n
			);
			auto program = engine.getRenderSystem()->createShaderProgram();
			program->createObject( ShaderType::eCompute );
			program->setSource( ShaderType::eCompute, vtx );
			return program;
		}

		ShaderProgramSPtr doCreateInOutProgram( Engine & engine )
		{
			String vtx = Glsl(
				layout( binding=0, offset=0 ) uniform atomic_uint counter;\n
				layout( local_size_variable ) in;\n
				void main()\n
				{\n
					atomicCounterIncrement( counter );\n
				}\n
			);
			auto program = engine.getRenderSystem()->createShaderProgram();
			program->createObject( ShaderType::eCompute );
			program->setSource( ShaderType::eCompute, vtx );
			return program;
		}
	}

	GlAtomicCounterBufferTest::GlAtomicCounterBufferTest( Engine & engine )
		: GlTestCase{ "GlAtomicCounterBufferTest", engine }
	{
	}

	GlAtomicCounterBufferTest::~GlAtomicCounterBufferTest()
	{
	}

	void GlAtomicCounterBufferTest::doRegisterTests()
	{
		doRegisterTest( "GlAtomicCounterBufferTest::Creation", std::bind( &GlAtomicCounterBufferTest::Creation, this ) );
		doRegisterTest( "GlAtomicCounterBufferTest::InOut", std::bind( &GlAtomicCounterBufferTest::InOut, this ) );
	}

	void GlAtomicCounterBufferTest::Creation()
	{
		auto program = doCreateNoopProgram( m_engine );
		auto & atomicCounterBuffer = program->createAtomicCounterBuffer( cuT( "Dummy" ), ShaderTypeFlag::eCompute );

		m_engine.getRenderSystem()->getMainContext()->setCurrent();
		CT_CHECK( atomicCounterBuffer.initialise( 1u, 0u ) );
		atomicCounterBuffer.cleanup();
		m_engine.getRenderSystem()->getMainContext()->endCurrent();
	}

	void GlAtomicCounterBufferTest::InOut()
	{
		auto program = doCreateInOutProgram( m_engine );
		auto & atomicCounterBuffer = program->createAtomicCounterBuffer( cuT( "Dummy" ), ShaderTypeFlag::eCompute );
		auto pipeline = m_engine.getRenderSystem()->createComputePipeline( *program );

		m_engine.getRenderSystem()->getMainContext()->setCurrent();
		CT_CHECK( atomicCounterBuffer.initialise( 1u, 0u ) );
		CT_CHECK( program->initialise() );
		uint32_t count{ 0u };
		atomicCounterBuffer.upload( 0u, 1u, &count );
		pipeline->run( Point3ui{ 1u, 1u, 1u }, Point3ui{ 1u, 1u, 1u }, MemoryBarrier::eAtomicCounterBuffer );
		atomicCounterBuffer.download( 0u, 1u, &count );
		CT_EQUAL( count, 1u );
		atomicCounterBuffer.cleanup();
		program->cleanup();
		m_engine.getRenderSystem()->getMainContext()->endCurrent();
	}
}
