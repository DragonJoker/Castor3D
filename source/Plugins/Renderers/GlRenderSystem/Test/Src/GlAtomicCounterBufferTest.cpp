#include "GlAtomicCounterBufferTest.hpp"

#include <Engine.hpp>
#include <Mesh/Buffer/VertexBuffer.hpp>
#include <Miscellaneous/ComputePipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Shader/AtomicCounterBuffer.hpp>
#include <Shader/ShaderProgram.hpp>

#include <Buffer/GlBuffer.hpp>

using namespace Castor;
using namespace Castor3D;

#ifdef max
#	undef max
#endif

#define Glsl( x ) cuT( "#version 430\n#extension GL_ARB_compute_variable_group_size : enable\n" ) cuT( #x )

namespace Testing
{
	namespace
	{
		ShaderProgramSPtr DoCreateNoopProgram( Engine & p_engine )
		{
			String vtx = Glsl(
				layout( binding = 0, offset = 0 ) uniform atomic_uint counter;\n
				layout( local_size_variable ) in;\n
				void main()\n
				{\n
				}\n
			);
			auto program = p_engine.GetRenderSystem()->CreateShaderProgram();
			program->CreateObject( ShaderType::eCompute );
			program->SetSource( ShaderType::eCompute, vtx );
			return program;
		}

		ShaderProgramSPtr DoCreateInOutProgram( Engine & p_engine )
		{
			String vtx = Glsl(
				layout( binding=0, offset=0 ) uniform atomic_uint counter;\n
				layout( local_size_variable ) in;\n
				void main()\n
				{\n
					atomicCounterIncrement( counter );\n
				}\n
			);
			auto program = p_engine.GetRenderSystem()->CreateShaderProgram();
			program->CreateObject( ShaderType::eCompute );
			program->SetSource( ShaderType::eCompute, vtx );
			return program;
		}
	}

	GlAtomicCounterBufferTest::GlAtomicCounterBufferTest( Engine & p_engine )
		: GlTestCase{ "GlAtomicCounterBufferTest", p_engine }
	{
	}

	GlAtomicCounterBufferTest::~GlAtomicCounterBufferTest()
	{
	}

	void GlAtomicCounterBufferTest::DoRegisterTests()
	{
		DoRegisterTest( "GlAtomicCounterBufferTest::Creation", std::bind( &GlAtomicCounterBufferTest::Creation, this ) );
		DoRegisterTest( "GlAtomicCounterBufferTest::InOut", std::bind( &GlAtomicCounterBufferTest::InOut, this ) );
	}

	void GlAtomicCounterBufferTest::Creation()
	{
		auto program = DoCreateNoopProgram( m_engine );
		auto & atomicCounterBuffer = program->CreateAtomicCounterBuffer( cuT( "Dummy" ), ShaderTypeFlag::eCompute );

		m_engine.GetRenderSystem()->GetMainContext()->SetCurrent();
		CT_CHECK( atomicCounterBuffer.Initialise( 1u, 0u ) );
		atomicCounterBuffer.Cleanup();
		m_engine.GetRenderSystem()->GetMainContext()->EndCurrent();
	}

	void GlAtomicCounterBufferTest::InOut()
	{
		auto program = DoCreateInOutProgram( m_engine );
		auto & atomicCounterBuffer = program->CreateAtomicCounterBuffer( cuT( "Dummy" ), ShaderTypeFlag::eCompute );
		auto pipeline = m_engine.GetRenderSystem()->CreateComputePipeline( *program );

		m_engine.GetRenderSystem()->GetMainContext()->SetCurrent();
		CT_CHECK( atomicCounterBuffer.Initialise( 1u, 0u ) );
		CT_CHECK( program->Initialise() );
		uint32_t count{ 0u };
		atomicCounterBuffer.Upload( 0u, 1u, &count );
		pipeline->Run( Point3ui{ 1u, 1u, 1u }, Point3ui{ 1u, 1u, 1u }, MemoryBarrier::eAtomicCounterBuffer );
		atomicCounterBuffer.Download( 0u, 1u, &count );
		CT_EQUAL( count, 1u );
		atomicCounterBuffer.Cleanup();
		program->Cleanup();
		m_engine.GetRenderSystem()->GetMainContext()->EndCurrent();
	}
}
