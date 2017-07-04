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
	std::string l_sep;

	for ( auto & l_value : p_array )
	{
		p_stream << l_sep << l_value;
		l_sep = ", ";
	}

	p_stream << " }";
	return p_stream;
}

namespace Testing
{
	namespace
	{
		ShaderProgramSPtr DoCreateSimpleComputeProgram( Engine & p_engine )
		{
			String l_code = Glsl(
				layout( std430, binding = 0 ) buffer Storage\n
				{ \n
					ivec4 ids[];\n
				};\n
				layout( local_size_variable ) in;\n
				void main()\n
				{ \n
					uint l_gid = gl_GlobalInvocationID.x;\n
					ids[l_gid].x = int( l_gid );\n
					ids[l_gid].y = int( l_gid * 2 );\n
					ids[l_gid].z = int( l_gid * 3 );\n
					ids[l_gid].w = int( l_gid * 4 );\n
				}\n
			);
			auto l_program = p_engine.GetRenderSystem()->CreateShaderProgram();
			l_program->CreateObject( ShaderType::eCompute );
			l_program->SetSource( ShaderType::eCompute, l_code );
			return l_program;
		}

		ShaderProgramSPtr DoCreateTwoStoragesProgram( Engine & p_engine )
		{
			String l_code = Glsl(
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
					uint l_gid = gl_GlobalInvocationID.x;\n
					ids2[l_gid].x = ids1[l_gid].x + int( l_gid );\n
					ids2[l_gid].y = ids1[l_gid].y + int( l_gid * 2 );\n
					ids2[l_gid].z = ids1[l_gid].z + int( l_gid * 3 );\n
					ids2[l_gid].w = ids1[l_gid].w + int( l_gid * 4 );\n
				}\n
			);
			auto l_program = p_engine.GetRenderSystem()->CreateShaderProgram();
			l_program->CreateObject( ShaderType::eCompute );
			l_program->SetSource( ShaderType::eCompute, l_code );
			return l_program;
		}

		ShaderProgramSPtr DoCreateAtomicCounterProgram( Engine & p_engine )
		{
			String l_code = Glsl(
				layout( binding = 0, offset = 0 ) uniform atomic_uint out_index;\n
				layout( std430, binding = 1 ) buffer Storage\n
				{\n
					ivec4 ids[];\n
				};\n
				layout( local_size_variable ) in;\n
				void main()\n
				{\n
					uint l_gid = atomicCounterIncrement( out_index );\n
					ids[l_gid].x = int( l_gid );\n
					ids[l_gid].y = int( l_gid * 2 );\n
					ids[l_gid].z = int( l_gid * 3 );\n
					ids[l_gid].w = int( l_gid * 4 );\n
				}\n
			);
			auto l_program = p_engine.GetRenderSystem()->CreateShaderProgram();
			l_program->CreateObject( ShaderType::eCompute );
			l_program->SetSource( ShaderType::eCompute, l_code );
			return l_program;
		}

		ShaderProgramSPtr DoCreateTwoStoragesAndAtomicCounterProgram( Engine & p_engine )
		{
			String l_code = Glsl(
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
					uint l_gid = atomicCounterIncrement( out_index );\n
					ids2[l_gid].x = ids1[l_gid].x + int( l_gid );\n
					ids2[l_gid].y = ids1[l_gid].y + int( l_gid * 2 );\n
					ids2[l_gid].z = ids1[l_gid].z + int( l_gid * 3 );\n
					ids2[l_gid].w = ids1[l_gid].w + int( l_gid * 4 );\n
				}\n
			);
			auto l_program = p_engine.GetRenderSystem()->CreateShaderProgram();
			l_program->CreateObject( ShaderType::eCompute );
			l_program->SetSource( ShaderType::eCompute, l_code );
			return l_program;
		}
	}

	GlComputeShaderTest::GlComputeShaderTest( Engine & p_engine )
		: GlTestCase{ "GlComputeShaderTest", p_engine }
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
		auto l_program = DoCreateSimpleComputeProgram( m_engine );
		ShaderStorageBuffer l_storageBuffer( m_engine );
		auto l_pipeline = m_engine.GetRenderSystem()->CreateComputePipeline( *l_program );

		m_engine.GetRenderSystem()->GetMainContext()->SetCurrent();
		l_storageBuffer.Resize( 5u * sizeof( Point4ui ) );
		CT_CHECK( l_storageBuffer.Initialise( BufferAccessType::eDynamic, BufferAccessNature::eDraw ) );
		CT_CHECK( l_program->Initialise() );
		l_pipeline->Run( Point3ui{ 5u, 1u, 1u }, Point3ui{ 1u, 1u, 1u }, MemoryBarrier::eShaderStorageBuffer );
		std::array< uint32_t, 5 * 4 > l_results;
		l_storageBuffer.Download( 0u, uint32_t( l_results.size() * sizeof( uint32_t ) ), reinterpret_cast< uint8_t * >( l_results.data() ) );
		std::array< uint32_t, 5 * 4 > l_expect{
			0, 0, 0, 0,
			1, 2, 3, 4,
			2, 4, 6, 8,
			3, 6, 9, 12,
			4, 8, 12, 16,
		};
		CT_EQUAL( l_results, l_expect );
		l_storageBuffer.Cleanup();
		l_program->Cleanup();
		m_engine.GetRenderSystem()->GetMainContext()->EndCurrent();
	}

	void GlComputeShaderTest::TwoStorages()
	{
		auto l_program = DoCreateTwoStoragesProgram( m_engine );
		ShaderStorageBuffer l_storage1( m_engine );
		ShaderStorageBuffer l_storage2( m_engine );
		auto l_pipeline = m_engine.GetRenderSystem()->CreateComputePipeline( *l_program );
		std::array< uint32_t, 5 * 4 > l_init{ 0 };
		auto l_size = uint32_t( sizeof( uint32_t ) * l_init.size() );
		l_storage1.Resize( l_size );
		l_storage2.Resize( l_size );

		m_engine.GetRenderSystem()->GetMainContext()->SetCurrent();
		CT_CHECK( l_storage1.Initialise( BufferAccessType::eDynamic, BufferAccessNature::eDraw ) );
		l_storage1.Upload( 0u, l_size, reinterpret_cast< uint8_t * >( l_init.data() ) );
		CT_CHECK( l_storage2.Initialise( BufferAccessType::eDynamic, BufferAccessNature::eDraw ) );
		l_storage2.Upload( 0u, l_size, reinterpret_cast< uint8_t * >( l_init.data() ) );
		CT_CHECK( l_program->Initialise() );
		l_storage1.BindTo( 1u );
		l_storage2.BindTo( 2u );
		l_pipeline->Run( Point3ui{ 5u, 1u, 1u }, Point3ui{ 1u, 1u, 1u }, MemoryBarrier::eShaderStorageBuffer );
		std::array< uint32_t, 5 * 4 > l_results1{ 0 };
		l_storage2.Download( 0u, l_size, reinterpret_cast< uint8_t * >( l_results1.data() ) );
		std::array< uint32_t, 5 * 4 > l_expect1{
			0, 0, 0, 0,
			1, 2, 3, 4,
			2, 4, 6, 8,
			3, 6, 9, 12,
			4, 8, 12, 16,
		};
		CT_EQUAL( l_results1, l_expect1 );
		l_storage1.BindTo( 2u );
		l_storage2.BindTo( 1u );
		l_pipeline->Run( Point3ui{ 5u, 1u, 1u }, Point3ui{ 1u, 1u, 1u }, MemoryBarrier::eShaderStorageBuffer );
		std::array< uint32_t, 5 * 4 > l_results2{ 0 };
		l_storage1.Download( 0u, l_size, reinterpret_cast< uint8_t * >( l_results2.data() ) );
		std::array< uint32_t, 5 * 4 > l_expect2{
			0, 0, 0, 0,
			2, 4, 6, 8,
			4, 8, 12, 16,
			6, 12, 18, 24,
			8, 16, 24, 32,
		};
		CT_EQUAL( l_results2, l_expect2 );
		l_storage1.Cleanup();
		l_storage2.Cleanup();
		l_program->Cleanup();
		m_engine.GetRenderSystem()->GetMainContext()->EndCurrent();
	}

	void GlComputeShaderTest::AtomicCounter()
	{
		auto l_program = DoCreateAtomicCounterProgram( m_engine );
		auto & l_atomicCounterBuffer = l_program->CreateAtomicCounterBuffer( cuT( "Counter" ), ShaderTypeFlag::eCompute );
		ShaderStorageBuffer l_storageBuffer( m_engine );
		auto l_pipeline = m_engine.GetRenderSystem()->CreateComputePipeline( *l_program );
		uint32_t l_count = 0u;

		m_engine.GetRenderSystem()->GetMainContext()->SetCurrent();
		CT_CHECK( l_atomicCounterBuffer.Initialise( 1u, 0u ) );
		l_atomicCounterBuffer.Upload( 0u, 1u, &l_count );
		l_storageBuffer.Resize( 5u * sizeof( Point4ui ) );
		CT_CHECK( l_storageBuffer.Initialise( BufferAccessType::eDynamic, BufferAccessNature::eDraw ) );
		CT_CHECK( l_program->Initialise() );
		l_storageBuffer.BindTo( 1u );
		l_pipeline->Run( Point3ui{ 5u, 1u, 1u }, Point3ui{ 1u, 1u, 1u }, MemoryBarrier::eShaderStorageBuffer );
		std::array< uint32_t, 5 * 4 > l_results;
		l_storageBuffer.Download( 0u, uint32_t( l_results.size() * sizeof( uint32_t ) ), reinterpret_cast< uint8_t * >( l_results.data() ) );
		std::array< uint32_t, 5 * 4 > l_expect{
			0, 0, 0, 0,
			1, 2, 3, 4,
			2, 4, 6, 8,
			3, 6, 9, 12,
			4, 8, 12, 16,
		};
		CT_EQUAL( l_results, l_expect );
		l_storageBuffer.Cleanup();
		l_atomicCounterBuffer.Cleanup();
		l_program->Cleanup();
		m_engine.GetRenderSystem()->GetMainContext()->EndCurrent();
	}

	void GlComputeShaderTest::TwoStoragesAndAtomicCounter()
	{
		auto l_program = DoCreateTwoStoragesAndAtomicCounterProgram( m_engine );
		auto & l_atomicCounterBuffer = l_program->CreateAtomicCounterBuffer( cuT( "Counter" ), ShaderTypeFlag::eCompute );
		ShaderStorageBuffer l_storage1( m_engine );
		ShaderStorageBuffer l_storage2( m_engine );
		auto l_pipeline = m_engine.GetRenderSystem()->CreateComputePipeline( *l_program );
		uint32_t l_count = 0u;
		std::array< uint32_t, 5 * 4 > l_init{ 0 };
		auto l_size = uint32_t( sizeof( uint32_t ) * l_init.size() );
		l_storage1.Resize( l_size );
		l_storage2.Resize( l_size );

		m_engine.GetRenderSystem()->GetMainContext()->SetCurrent();
		CT_CHECK( l_atomicCounterBuffer.Initialise( 1u, 0u ) );
		l_atomicCounterBuffer.Upload( 0u, 1u, &l_count );
		CT_CHECK( l_storage1.Initialise( BufferAccessType::eDynamic, BufferAccessNature::eDraw ) );
		l_storage1.Upload( 0u, l_size, reinterpret_cast< uint8_t * >( l_init.data() ) );
		CT_CHECK( l_storage2.Initialise( BufferAccessType::eDynamic, BufferAccessNature::eDraw ) );
		l_storage2.Upload( 0u, l_size, reinterpret_cast< uint8_t * >( l_init.data() ) );
		CT_CHECK( l_program->Initialise() );
		l_storage1.BindTo( 1u );
		l_storage2.BindTo( 2u );
		l_pipeline->Run( Point3ui{ 5u, 1u, 1u }, Point3ui{ 1u, 1u, 1u }, MemoryBarrier::eShaderStorageBuffer );
		std::array< uint32_t, 5 * 4 > l_results1{ 0 };
		l_storage2.Download( 0u, l_size, reinterpret_cast< uint8_t * >( l_results1.data() ) );
		std::array< uint32_t, 5 * 4 > l_expect1{
			0, 0, 0, 0,
			1, 2, 3, 4,
			2, 4, 6, 8,
			3, 6, 9, 12,
			4, 8, 12, 16,
		};
		CT_EQUAL( l_results1, l_expect1 );
		l_atomicCounterBuffer.Upload( 0u, 1u, &l_count );
		l_storage1.BindTo( 2u );
		l_storage2.BindTo( 1u );
		l_pipeline->Run( Point3ui{ 5u, 1u, 1u }, Point3ui{ 1u, 1u, 1u }, MemoryBarrier::eShaderStorageBuffer );
		std::array< uint32_t, 5 * 4 > l_results2{ 0 };
		l_storage1.Download( 0u, l_size, reinterpret_cast< uint8_t * >( l_results2.data() ) );
		std::array< uint32_t, 5 * 4 > l_expect2{
			0, 0, 0, 0,
			2, 4, 6, 8,
			4, 8, 12, 16,
			6, 12, 18, 24,
			8, 16, 24, 32,
		};
		CT_EQUAL( l_results2, l_expect2 );
		l_storage1.Cleanup();
		l_storage2.Cleanup();
		l_atomicCounterBuffer.Cleanup();
		l_program->Cleanup();
		m_engine.GetRenderSystem()->GetMainContext()->EndCurrent();
	}
}
