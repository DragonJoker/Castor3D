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

#define Glsl( x ) cuT( "#version 430\n" ) cuT( #x )

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
			l_program->CreateObject( ShaderType::eVertex );
			l_program->SetSource( ShaderType::eVertex, l_model, l_vtx );
			return l_program;
		}

		ShaderProgramSPtr DoCreateInOutProgram( Engine & p_engine )
		{
			String l_vtx = Glsl(
				layout( binding=0, offset=0 ) uniform atomic_uint counter;
				layout( local_size_x = 1, local_size_y = 1, local_size_z = 1 ) in;
				void main()
				{
					atomicCounterIncrement( counter );
				}
			);
			auto l_program = p_engine.GetRenderSystem()->CreateShaderProgram();
			auto l_model = p_engine.GetRenderSystem()->GetGpuInformations().GetMaxShaderModel();
			l_program->CreateObject( ShaderType::eCompute );
			l_program->SetSource( ShaderType::eCompute, l_model, l_vtx );
			return l_program;
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
		auto l_program = DoCreateNoopProgram( m_engine );
		auto & l_atomicCounterBuffer = l_program->CreateAtomicCounterBuffer( cuT( "Dummy" ), ShaderTypeFlag::eCompute );

		m_engine.GetRenderSystem()->GetMainContext()->SetCurrent();
		CT_CHECK( l_atomicCounterBuffer.Initialise( 1u, 0u ) );
		l_atomicCounterBuffer.Cleanup();
		m_engine.GetRenderSystem()->GetMainContext()->EndCurrent();
	}

	void GlAtomicCounterBufferTest::InOut()
	{
		auto l_program = DoCreateInOutProgram( m_engine );
		auto & l_atomicCounterBuffer = l_program->CreateAtomicCounterBuffer( cuT( "Dummy" ), ShaderTypeFlag::eCompute );
		auto l_pipeline = m_engine.GetRenderSystem()->CreateComputePipeline( *l_program );

		m_engine.GetRenderSystem()->GetMainContext()->SetCurrent();
		CT_CHECK( l_atomicCounterBuffer.Initialise( 1u, 0u ) );
		CT_CHECK( l_program->Initialise() );
		uint32_t l_count{ 0u };
		CT_CHECK( l_atomicCounterBuffer.Upload( 0u, 1u, &l_count ) );
		l_pipeline->Run( Point3ui{ 1u, 1u, 1u }, Point3ui{ 1u, 1u, 1u }, MemoryBarrier::eAtomicCounterBuffer );
		CT_CHECK( l_atomicCounterBuffer.Download( 0u, 1u, &l_count ) );
		CT_EQUAL( l_count, 1u );
		l_atomicCounterBuffer.Cleanup();
		m_engine.GetRenderSystem()->GetMainContext()->EndCurrent();
	}
}
