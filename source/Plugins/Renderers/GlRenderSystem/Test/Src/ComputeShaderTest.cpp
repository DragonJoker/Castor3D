#include "ComputeShaderTest.hpp"

#include <Engine.hpp>
#include <Mesh/Buffer/VertexBuffer.hpp>
#include <Miscellaneous/ComputePipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Shader/AtomicCounterBuffer.hpp>
#include <Shader/ShaderProgram.hpp>
#include <Shader/ShaderStorageBuffer.hpp>

#include <Buffer/GlBuffer.hpp>

using namespace Castor;
using namespace Castor3D;

#ifdef max
#	undef max
#endif

#define Glsl( x ) cuT( "#version 430\n#extension GL_ARB_compute_variable_group_size : enable\n" ) cuT( #x )

template< typename T, size_t Count >
std::ostream & operator<<( std::ostream & p_stream, std::array< T, Count > const & p_array )
{
	p_stream << "array( " << Count << " ): { ";
	std::string sep;

	for ( auto & value : p_array )
	{
		p_stream << sep << value;
		sep = ", ";
	}

	p_stream << " }";
	return p_stream;
}

namespace Testing
{
	namespace
	{
		ShaderProgramSPtr DoCreateSimpleComputeProgram( Engine & engine )
		{
			String code = Glsl(
				layout( std430, binding = 0 ) buffer Storage\n
				{ \n
					ivec4 ids[];\n
				};\n
				layout( local_size_variable ) in;\n
				void main()\n
				{ \n
					uint gid = gl_GlobalInvocationID.x;\n
					ids[gid].x = int( gid );\n
					ids[gid].y = int( gid * 2 );\n
					ids[gid].z = int( gid * 3 );\n
					ids[gid].w = int( gid * 4 );\n
				}\n
			);
			auto program = engine.GetRenderSystem()->CreateShaderProgram();
			program->CreateObject( ShaderType::eCompute );
			program->SetSource( ShaderType::eCompute, code );
			return program;
		}

		ShaderProgramSPtr DoCreateTwoStoragesProgram( Engine & engine )
		{
			String code = Glsl(
				layout( std430, binding = 1 ) buffer Storage1\n
				{\n
					ivec4 ids1[];\n
				};\n
				layout( std430, binding = 2 ) buffer Storage2\n
				{\n
					ivec4 ids2[];\n
				};\n
				layout( local_size_variable ) in;\n
				void main()\n
				{\n
					uint gid = gl_GlobalInvocationID.x;\n
					ids2[gid].x = ids1[gid].x + int( gid );\n
					ids2[gid].y = ids1[gid].y + int( gid * 2 );\n
					ids2[gid].z = ids1[gid].z + int( gid * 3 );\n
					ids2[gid].w = ids1[gid].w + int( gid * 4 );\n
				}\n
			);
			auto program = engine.GetRenderSystem()->CreateShaderProgram();
			program->CreateObject( ShaderType::eCompute );
			program->SetSource( ShaderType::eCompute, code );
			return program;
		}

		ShaderProgramSPtr DoCreateAtomicCounterProgram( Engine & engine )
		{
			String code = Glsl(
				layout( binding = 0, offset = 0 ) uniform atomic_uint out_index;\n
				layout( std430, binding = 1 ) buffer Storage\n
				{\n
					ivec4 ids[];\n
				};\n
				layout( local_size_variable ) in;\n
				void main()\n
				{\n
					uint gid = atomicCounterIncrement( out_index );\n
					ids[gid].x = int( gid );\n
					ids[gid].y = int( gid * 2 );\n
					ids[gid].z = int( gid * 3 );\n
					ids[gid].w = int( gid * 4 );\n
				}\n
			);
			auto program = engine.GetRenderSystem()->CreateShaderProgram();
			program->CreateObject( ShaderType::eCompute );
			program->SetSource( ShaderType::eCompute, code );
			return program;
		}

		ShaderProgramSPtr DoCreateTwoStoragesAndAtomicCounterProgram( Engine & engine )
		{
			String code = Glsl(
				layout( binding = 0, offset = 0 ) uniform atomic_uint out_index;\n
				layout( std430, binding = 1 ) buffer Storage1\n
				{\n
					ivec4 ids1[];\n
				};\n
				layout( std430, binding = 2 ) buffer Storage2\n
				{\n
					ivec4 ids2[];\n
				};\n
				layout( local_size_variable ) in;\n
				void main()\n
				{\n
					uint gid = atomicCounterIncrement( out_index );\n
					ids2[gid].x = ids1[gid].x + int( gid );\n
					ids2[gid].y = ids1[gid].y + int( gid * 2 );\n
					ids2[gid].z = ids1[gid].z + int( gid * 3 );\n
					ids2[gid].w = ids1[gid].w + int( gid * 4 );\n
				}\n
			);
			auto program = engine.GetRenderSystem()->CreateShaderProgram();
			program->CreateObject( ShaderType::eCompute );
			program->SetSource( ShaderType::eCompute, code );
			return program;
		}
	}

	GlComputeShaderTest::GlComputeShaderTest( Engine & engine )
		: GlTestCase{ "GlComputeShaderTest", engine }
	{
	}

	GlComputeShaderTest::~GlComputeShaderTest()
	{
	}

	void GlComputeShaderTest::DoRegisterTests()
	{
		DoRegisterTest( "GlComputeShaderTest::SimpleCompute", std::bind( &GlComputeShaderTest::SimpleCompute, this ) );
		DoRegisterTest( "GlComputeShaderTest::TwoStorages", std::bind( &GlComputeShaderTest::TwoStorages, this ) );
		DoRegisterTest( "GlComputeShaderTest::AtomicCounter", std::bind( &GlComputeShaderTest::AtomicCounter, this ) );
		DoRegisterTest( "GlComputeShaderTest::TwoStoragesAndAtomicCounter", std::bind( &GlComputeShaderTest::TwoStoragesAndAtomicCounter, this ) );
	}

	void GlComputeShaderTest::SimpleCompute()
	{
		auto program = DoCreateSimpleComputeProgram( m_engine );
		ShaderStorageBuffer storageBuffer( m_engine );
		auto pipeline = m_engine.GetRenderSystem()->CreateComputePipeline( *program );

		m_engine.GetRenderSystem()->GetMainContext()->SetCurrent();
		storageBuffer.Resize( 5u * sizeof( Point4ui ) );
		CT_CHECK( storageBuffer.Initialise( BufferAccessType::eDynamic, BufferAccessNature::eDraw ) );
		CT_CHECK( program->Initialise() );
		pipeline->Run( Point3ui{ 5u, 1u, 1u }, Point3ui{ 1u, 1u, 1u }, MemoryBarrier::eShaderStorageBuffer );
		std::array< uint32_t, 5 * 4 > results;
		storageBuffer.Download( 0u, uint32_t( results.size() * sizeof( uint32_t ) ), reinterpret_cast< uint8_t * >( results.data() ) );
		std::array< uint32_t, 5 * 4 > expect{
			0, 0, 0, 0,
			1, 2, 3, 4,
			2, 4, 6, 8,
			3, 6, 9, 12,
			4, 8, 12, 16,
		};
		CT_EQUAL( results, expect );
		storageBuffer.Cleanup();
		program->Cleanup();
		m_engine.GetRenderSystem()->GetMainContext()->EndCurrent();
	}

	void GlComputeShaderTest::TwoStorages()
	{
		auto program = DoCreateTwoStoragesProgram( m_engine );
		ShaderStorageBuffer storage1( m_engine );
		ShaderStorageBuffer storage2( m_engine );
		auto pipeline = m_engine.GetRenderSystem()->CreateComputePipeline( *program );
		std::array< uint32_t, 5 * 4 > init{ 0 };
		auto size = uint32_t( sizeof( uint32_t ) * init.size() );
		storage1.Resize( size );
		storage2.Resize( size );

		m_engine.GetRenderSystem()->GetMainContext()->SetCurrent();
		CT_CHECK( storage1.Initialise( BufferAccessType::eDynamic, BufferAccessNature::eDraw ) );
		storage1.Upload( 0u, size, reinterpret_cast< uint8_t * >( init.data() ) );
		CT_CHECK( storage2.Initialise( BufferAccessType::eDynamic, BufferAccessNature::eDraw ) );
		storage2.Upload( 0u, size, reinterpret_cast< uint8_t * >( init.data() ) );
		CT_CHECK( program->Initialise() );
		storage1.BindTo( 1u );
		storage2.BindTo( 2u );
		pipeline->Run( Point3ui{ 5u, 1u, 1u }, Point3ui{ 1u, 1u, 1u }, MemoryBarrier::eShaderStorageBuffer );
		std::array< uint32_t, 5 * 4 > results1{ 0 };
		storage2.Download( 0u, size, reinterpret_cast< uint8_t * >( results1.data() ) );
		std::array< uint32_t, 5 * 4 > expect1{
			0, 0, 0, 0,
			1, 2, 3, 4,
			2, 4, 6, 8,
			3, 6, 9, 12,
			4, 8, 12, 16,
		};
		CT_EQUAL( results1, expect1 );
		storage1.BindTo( 2u );
		storage2.BindTo( 1u );
		pipeline->Run( Point3ui{ 5u, 1u, 1u }, Point3ui{ 1u, 1u, 1u }, MemoryBarrier::eShaderStorageBuffer );
		std::array< uint32_t, 5 * 4 > results2{ 0 };
		storage1.Download( 0u, size, reinterpret_cast< uint8_t * >( results2.data() ) );
		std::array< uint32_t, 5 * 4 > expect2{
			0, 0, 0, 0,
			2, 4, 6, 8,
			4, 8, 12, 16,
			6, 12, 18, 24,
			8, 16, 24, 32,
		};
		CT_EQUAL( results2, expect2 );
		storage1.Cleanup();
		storage2.Cleanup();
		program->Cleanup();
		m_engine.GetRenderSystem()->GetMainContext()->EndCurrent();
	}

	void GlComputeShaderTest::AtomicCounter()
	{
		auto program = DoCreateAtomicCounterProgram( m_engine );
		auto & atomicCounterBuffer = program->CreateAtomicCounterBuffer( cuT( "Counter" ), ShaderTypeFlag::eCompute );
		ShaderStorageBuffer storageBuffer( m_engine );
		auto pipeline = m_engine.GetRenderSystem()->CreateComputePipeline( *program );
		uint32_t count = 0u;

		m_engine.GetRenderSystem()->GetMainContext()->SetCurrent();
		CT_CHECK( atomicCounterBuffer.Initialise( 1u, 0u ) );
		atomicCounterBuffer.Upload( 0u, 1u, &count );
		storageBuffer.Resize( 5u * sizeof( Point4ui ) );
		CT_CHECK( storageBuffer.Initialise( BufferAccessType::eDynamic, BufferAccessNature::eDraw ) );
		CT_CHECK( program->Initialise() );
		storageBuffer.BindTo( 1u );
		pipeline->Run( Point3ui{ 5u, 1u, 1u }, Point3ui{ 1u, 1u, 1u }, MemoryBarrier::eShaderStorageBuffer );
		std::array< uint32_t, 5 * 4 > results;
		storageBuffer.Download( 0u, uint32_t( results.size() * sizeof( uint32_t ) ), reinterpret_cast< uint8_t * >( results.data() ) );
		std::array< uint32_t, 5 * 4 > expect{
			0, 0, 0, 0,
			1, 2, 3, 4,
			2, 4, 6, 8,
			3, 6, 9, 12,
			4, 8, 12, 16,
		};
		CT_EQUAL( results, expect );
		storageBuffer.Cleanup();
		atomicCounterBuffer.Cleanup();
		program->Cleanup();
		m_engine.GetRenderSystem()->GetMainContext()->EndCurrent();
	}

	void GlComputeShaderTest::TwoStoragesAndAtomicCounter()
	{
		auto program = DoCreateTwoStoragesAndAtomicCounterProgram( m_engine );
		auto & atomicCounterBuffer = program->CreateAtomicCounterBuffer( cuT( "Counter" ), ShaderTypeFlag::eCompute );
		ShaderStorageBuffer storage1( m_engine );
		ShaderStorageBuffer storage2( m_engine );
		auto pipeline = m_engine.GetRenderSystem()->CreateComputePipeline( *program );
		uint32_t count = 0u;
		std::array< uint32_t, 5 * 4 > init{ 0 };
		auto size = uint32_t( sizeof( uint32_t ) * init.size() );
		storage1.Resize( size );
		storage2.Resize( size );

		m_engine.GetRenderSystem()->GetMainContext()->SetCurrent();
		CT_CHECK( atomicCounterBuffer.Initialise( 1u, 0u ) );
		atomicCounterBuffer.Upload( 0u, 1u, &count );
		CT_CHECK( storage1.Initialise( BufferAccessType::eDynamic, BufferAccessNature::eDraw ) );
		storage1.Upload( 0u, size, reinterpret_cast< uint8_t * >( init.data() ) );
		CT_CHECK( storage2.Initialise( BufferAccessType::eDynamic, BufferAccessNature::eDraw ) );
		storage2.Upload( 0u, size, reinterpret_cast< uint8_t * >( init.data() ) );
		CT_CHECK( program->Initialise() );
		storage1.BindTo( 1u );
		storage2.BindTo( 2u );
		pipeline->Run( Point3ui{ 5u, 1u, 1u }, Point3ui{ 1u, 1u, 1u }, MemoryBarrier::eShaderStorageBuffer );
		std::array< uint32_t, 5 * 4 > results1{ 0 };
		storage2.Download( 0u, size, reinterpret_cast< uint8_t * >( results1.data() ) );
		std::array< uint32_t, 5 * 4 > expect1{
			0, 0, 0, 0,
			1, 2, 3, 4,
			2, 4, 6, 8,
			3, 6, 9, 12,
			4, 8, 12, 16,
		};
		CT_EQUAL( results1, expect1 );
		atomicCounterBuffer.Upload( 0u, 1u, &count );
		storage1.BindTo( 2u );
		storage2.BindTo( 1u );
		pipeline->Run( Point3ui{ 5u, 1u, 1u }, Point3ui{ 1u, 1u, 1u }, MemoryBarrier::eShaderStorageBuffer );
		std::array< uint32_t, 5 * 4 > results2{ 0 };
		storage1.Download( 0u, size, reinterpret_cast< uint8_t * >( results2.data() ) );
		std::array< uint32_t, 5 * 4 > expect2{
			0, 0, 0, 0,
			2, 4, 6, 8,
			4, 8, 12, 16,
			6, 12, 18, 24,
			8, 16, 24, 32,
		};
		CT_EQUAL( results2, expect2 );
		storage1.Cleanup();
		storage2.Cleanup();
		atomicCounterBuffer.Cleanup();
		program->Cleanup();
		m_engine.GetRenderSystem()->GetMainContext()->EndCurrent();
	}
}
