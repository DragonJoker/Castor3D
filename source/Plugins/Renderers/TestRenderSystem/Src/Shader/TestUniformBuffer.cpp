#include "Shader/TestUniformBuffer.hpp"

#include "Render/TestRenderSystem.hpp"
#include "Shader/TestMatrixUniform.hpp"
#include "Shader/TestOneUniform.hpp"
#include "Shader/TestPointUniform.hpp"
#include "Shader/TestShaderProgram.hpp"

using namespace Castor3D;
using namespace Castor;

namespace TestRender
{
	namespace
	{
		template< UniformType Type > UniformSPtr TestUniformCreator( TestShaderProgram & p_program, uint32_t p_occurences );

		template<> UniformSPtr TestUniformCreator< UniformType::eInt >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestOneUniform< int > >( p_occurences, p_program );
		}
		template<> UniformSPtr TestUniformCreator< UniformType::eUInt >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestOneUniform< uint32_t > >( p_occurences, p_program );
		}
		template<> UniformSPtr TestUniformCreator< UniformType::eFloat >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestOneUniform< float > >( p_occurences, p_program );
		}
		template<> UniformSPtr TestUniformCreator< UniformType::eDouble >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestOneUniform< double > >( p_occurences, p_program );
		}
		template<> UniformSPtr TestUniformCreator< UniformType::eSampler >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestOneUniform< int > >( p_occurences, p_program );
		}
		template<> UniformSPtr TestUniformCreator< UniformType::eVec2i >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestPointUniform< int, 2 > >( p_occurences, p_program );
		}
		template<> UniformSPtr TestUniformCreator< UniformType::eVec3i >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestPointUniform< int, 3 > >( p_occurences, p_program );
		}
		template<> UniformSPtr TestUniformCreator< UniformType::eVec4i >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestPointUniform< int, 4 > >( p_occurences, p_program );
		}
		template<> UniformSPtr TestUniformCreator< UniformType::eVec2ui >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestPointUniform< uint32_t, 2 > >( p_occurences, p_program );
		}
		template<> UniformSPtr TestUniformCreator< UniformType::eVec3ui >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestPointUniform< uint32_t, 3 > >( p_occurences, p_program );
		}
		template<> UniformSPtr TestUniformCreator< UniformType::eVec4ui >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestPointUniform< uint32_t, 4 > >( p_occurences, p_program );
		}
		template<> UniformSPtr TestUniformCreator< UniformType::eVec2f >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestPointUniform< float, 2 > >( p_occurences, p_program );
		}
		template<> UniformSPtr TestUniformCreator< UniformType::eVec3f >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestPointUniform< float, 3 > >( p_occurences, p_program );
		}
		template<> UniformSPtr TestUniformCreator< UniformType::eVec4f >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestPointUniform< float, 4 > >( p_occurences, p_program );
		}
		template<> UniformSPtr TestUniformCreator< UniformType::eVec2d >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestPointUniform< double, 2 > >( p_occurences, p_program );
		}
		template<> UniformSPtr TestUniformCreator< UniformType::eVec3d >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestPointUniform< double, 3 > >( p_occurences, p_program );
		}
		template<> UniformSPtr TestUniformCreator< UniformType::eVec4d >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestPointUniform< double, 4 > >( p_occurences, p_program );
		}
		template<> UniformSPtr TestUniformCreator< UniformType::eMat2x2f >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixUniform< float, 2, 2 > >( p_occurences, p_program );
		}
		template<> UniformSPtr TestUniformCreator< UniformType::eMat2x3f >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixUniform< float, 2, 3 > >( p_occurences, p_program );
		}
		template<> UniformSPtr TestUniformCreator< UniformType::eMat2x4f >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixUniform< float, 2, 4 > >( p_occurences, p_program );
		}
		template<> UniformSPtr TestUniformCreator< UniformType::eMat3x2f >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixUniform< float, 3, 2 > >( p_occurences, p_program );
		}
		template<> UniformSPtr TestUniformCreator< UniformType::eMat3x3f >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixUniform< float, 3, 3 > >( p_occurences, p_program );
		}
		template<> UniformSPtr TestUniformCreator< UniformType::eMat3x4f >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixUniform< float, 3, 4 > >( p_occurences, p_program );
		}
		template<> UniformSPtr TestUniformCreator< UniformType::eMat4x2f >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixUniform< float, 4, 2 > >( p_occurences, p_program );
		}
		template<> UniformSPtr TestUniformCreator< UniformType::eMat4x3f >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixUniform< float, 4, 3 > >( p_occurences, p_program );
		}
		template<> UniformSPtr TestUniformCreator< UniformType::eMat4x4f >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixUniform< float, 4, 4 > >( p_occurences, p_program );
		}
		template<> UniformSPtr TestUniformCreator< UniformType::eMat2x2d >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixUniform< double, 2, 2 > >( p_occurences, p_program );
		}
		template<> UniformSPtr TestUniformCreator< UniformType::eMat2x3d >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixUniform< double, 2, 3 > >( p_occurences, p_program );
		}
		template<> UniformSPtr TestUniformCreator< UniformType::eMat2x4d >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixUniform< double, 2, 4 > >( p_occurences, p_program );
		}
		template<> UniformSPtr TestUniformCreator< UniformType::eMat3x2d >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixUniform< double, 3, 2 > >( p_occurences, p_program );
		}
		template<> UniformSPtr TestUniformCreator< UniformType::eMat3x3d >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixUniform< double, 3, 3 > >( p_occurences, p_program );
		}
		template<> UniformSPtr TestUniformCreator< UniformType::eMat3x4d >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixUniform< double, 3, 4 > >( p_occurences, p_program );
		}
		template<> UniformSPtr TestUniformCreator< UniformType::eMat4x2d >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixUniform< double, 4, 2 > >( p_occurences, p_program );
		}
		template<> UniformSPtr TestUniformCreator< UniformType::eMat4x3d >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixUniform< double, 4, 3 > >( p_occurences, p_program );
		}
		template<> UniformSPtr TestUniformCreator< UniformType::eMat4x4d >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixUniform< double, 4, 4 > >( p_occurences, p_program );
		}
	}

	TestUniformBuffer::TestUniformBuffer( String const & p_name
		, ShaderProgram & p_program
		, FlagCombination< ShaderTypeFlag > const & p_flags
		, RenderSystem & p_renderSystem )
		: UniformBuffer( p_name, p_program, p_flags, p_renderSystem )
	{
	}

	TestUniformBuffer::~TestUniformBuffer()
	{
	}

	UniformSPtr TestUniformBuffer::DoCreateVariable( UniformType p_type, Castor::String const & p_name, uint32_t p_occurences )
	{
		UniformSPtr l_return;
		TestShaderProgram & l_program = static_cast< TestShaderProgram & >( m_program );

		switch ( p_type )
		{
		case UniformType::eInt:
			l_return = TestUniformCreator< UniformType::eInt >( l_program, p_occurences );
			break;

		case UniformType::eUInt:
			l_return = TestUniformCreator< UniformType::eUInt >( l_program, p_occurences );
			break;

		case UniformType::eFloat:
			l_return = TestUniformCreator< UniformType::eFloat >( l_program, p_occurences );
			break;

		case UniformType::eDouble:
			l_return = TestUniformCreator< UniformType::eDouble >( l_program, p_occurences );
			break;

		case UniformType::eSampler:
			l_return = TestUniformCreator< UniformType::eSampler >( l_program, p_occurences );
			break;

		case UniformType::eVec2i:
			l_return = TestUniformCreator< UniformType::eVec2i >( l_program, p_occurences );
			break;

		case UniformType::eVec3i:
			l_return = TestUniformCreator< UniformType::eVec3i >( l_program, p_occurences );
			break;

		case UniformType::eVec4i:
			l_return = TestUniformCreator< UniformType::eVec4i >( l_program, p_occurences );
			break;

		case UniformType::eVec2ui:
			l_return = TestUniformCreator< UniformType::eVec2ui >( l_program, p_occurences );
			break;

		case UniformType::eVec3ui:
			l_return = TestUniformCreator< UniformType::eVec3ui >( l_program, p_occurences );
			break;

		case UniformType::eVec4ui:
			l_return = TestUniformCreator< UniformType::eVec4ui >( l_program, p_occurences );
			break;

		case UniformType::eVec2f:
			l_return = TestUniformCreator< UniformType::eVec2f >( l_program, p_occurences );
			break;

		case UniformType::eVec3f:
			l_return = TestUniformCreator< UniformType::eVec3f >( l_program, p_occurences );
			break;

		case UniformType::eVec4f:
			l_return = TestUniformCreator< UniformType::eVec4f >( l_program, p_occurences );
			break;

		case UniformType::eVec2d:
			l_return = TestUniformCreator< UniformType::eVec2d >( l_program, p_occurences );
			break;

		case UniformType::eVec3d:
			l_return = TestUniformCreator< UniformType::eVec3d >( l_program, p_occurences );
			break;

		case UniformType::eVec4d:
			l_return = TestUniformCreator< UniformType::eVec4d >( l_program, p_occurences );
			break;

		case UniformType::eMat2x2f:
			l_return = TestUniformCreator< UniformType::eMat2x2f >( l_program, p_occurences );
			break;

		case UniformType::eMat2x3f:
			l_return = TestUniformCreator< UniformType::eMat2x3f >( l_program, p_occurences );
			break;

		case UniformType::eMat2x4f:
			l_return = TestUniformCreator< UniformType::eMat2x4f >( l_program, p_occurences );
			break;

		case UniformType::eMat3x2f:
			l_return = TestUniformCreator< UniformType::eMat3x2f >( l_program, p_occurences );
			break;

		case UniformType::eMat3x3f:
			l_return = TestUniformCreator< UniformType::eMat3x3f >( l_program, p_occurences );
			break;

		case UniformType::eMat3x4f:
			l_return = TestUniformCreator< UniformType::eMat3x4f >( l_program, p_occurences );
			break;

		case UniformType::eMat4x2f:
			l_return = TestUniformCreator< UniformType::eMat4x2f >( l_program, p_occurences );
			break;

		case UniformType::eMat4x3f:
			l_return = TestUniformCreator< UniformType::eMat4x3f >( l_program, p_occurences );
			break;

		case UniformType::eMat4x4f:
			l_return = TestUniformCreator< UniformType::eMat4x4f >( l_program, p_occurences );
			break;

		case UniformType::eMat2x2d:
			l_return = TestUniformCreator< UniformType::eMat2x2d >( l_program, p_occurences );
			break;

		case UniformType::eMat2x3d:
			l_return = TestUniformCreator< UniformType::eMat2x3d >( l_program, p_occurences );
			break;

		case UniformType::eMat2x4d:
			l_return = TestUniformCreator< UniformType::eMat2x4d >( l_program, p_occurences );
			break;

		case UniformType::eMat3x2d:
			l_return = TestUniformCreator< UniformType::eMat3x2d >( l_program, p_occurences );
			break;

		case UniformType::eMat3x3d:
			l_return = TestUniformCreator< UniformType::eMat3x3d >( l_program, p_occurences );
			break;

		case UniformType::eMat3x4d:
			l_return = TestUniformCreator< UniformType::eMat3x4d >( l_program, p_occurences );
			break;

		case UniformType::eMat4x2d:
			l_return = TestUniformCreator< UniformType::eMat4x2d >( l_program, p_occurences );
			break;

		case UniformType::eMat4x3d:
			l_return = TestUniformCreator< UniformType::eMat4x3d >( l_program, p_occurences );
			break;

		case UniformType::eMat4x4d:
			l_return = TestUniformCreator< UniformType::eMat4x4d >( l_program, p_occurences );
			break;
		}

		if ( l_return )
		{
			l_return->SetName( p_name );
		}

		return l_return;
	}

	bool TestUniformBuffer::DoInitialise()
	{
		return true;
	}

	void TestUniformBuffer::DoCleanup()
	{
	}

	void TestUniformBuffer::DoBindTo( uint32_t p_index )
	{
	}

	void TestUniformBuffer::DoUpdate()
	{
	}
}
