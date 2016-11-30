#include "Shader/TestFrameVariableBuffer.hpp"

#include "Render/TestRenderSystem.hpp"
#include "Shader/TestMatrixFrameVariable.hpp"
#include "Shader/TestOneFrameVariable.hpp"
#include "Shader/TestPointFrameVariable.hpp"
#include "Shader/TestShaderProgram.hpp"

using namespace Castor3D;
using namespace Castor;

namespace TestRender
{
	namespace
	{
		template< FrameVariableType Type > FrameVariableSPtr TestFrameVariableCreator( TestShaderProgram & p_program, uint32_t p_occurences );

		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::eInt >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestOneFrameVariable< int > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::eUInt >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestOneFrameVariable< uint32_t > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::eFloat >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestOneFrameVariable< float > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::eDouble >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestOneFrameVariable< double > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::eSampler >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestOneFrameVariable< int > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::eVec2i >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestPointFrameVariable< int, 2 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::eVec3i >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestPointFrameVariable< int, 3 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::eVec4i >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestPointFrameVariable< int, 4 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::eVec2ui >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestPointFrameVariable< uint32_t, 2 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::eVec3ui >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestPointFrameVariable< uint32_t, 3 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::eVec4ui >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestPointFrameVariable< uint32_t, 4 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::eVec2f >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestPointFrameVariable< float, 2 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::eVec3f >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestPointFrameVariable< float, 3 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::eVec4f >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestPointFrameVariable< float, 4 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::eVec2d >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestPointFrameVariable< double, 2 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::eVec3d >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestPointFrameVariable< double, 3 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::eVec4d >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestPointFrameVariable< double, 4 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::eMat2x2f >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixFrameVariable< float, 2, 2 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::eMat2x3f >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixFrameVariable< float, 2, 3 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::eMat2x4f >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixFrameVariable< float, 2, 4 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::eMat3x2f >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixFrameVariable< float, 3, 2 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::eMat3x3f >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixFrameVariable< float, 3, 3 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::eMat3x4f >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixFrameVariable< float, 3, 4 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::eMat4x2f >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixFrameVariable< float, 4, 2 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::eMat4x3f >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixFrameVariable< float, 4, 3 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::eMat4x4f >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixFrameVariable< float, 4, 4 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::eMat2x2d >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixFrameVariable< double, 2, 2 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::eMat2x3d >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixFrameVariable< double, 2, 3 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::eMat2x4d >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixFrameVariable< double, 2, 4 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::eMat3x2d >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixFrameVariable< double, 3, 2 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::eMat3x3d >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixFrameVariable< double, 3, 3 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::eMat3x4d >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixFrameVariable< double, 3, 4 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::eMat4x2d >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixFrameVariable< double, 4, 2 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::eMat4x3d >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixFrameVariable< double, 4, 3 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::eMat4x4d >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixFrameVariable< double, 4, 4 > >( p_occurences, p_program );
		}
	}

	TestFrameVariableBuffer::TestFrameVariableBuffer(
		String const & p_name,
		ShaderProgram & p_program,
		FlagCombination< ShaderTypeFlag > const & p_flags,
		RenderSystem & p_renderSystem )
		: FrameVariableBuffer( p_name, p_program, p_flags, p_renderSystem )
	{
	}

	TestFrameVariableBuffer::~TestFrameVariableBuffer()
	{
	}

	FrameVariableSPtr TestFrameVariableBuffer::DoCreateVariable( FrameVariableType p_type, Castor::String const & p_name, uint32_t p_occurences )
	{
		FrameVariableSPtr l_return;
		TestShaderProgram & l_program = static_cast< TestShaderProgram & >( m_program );

		switch ( p_type )
		{
		case FrameVariableType::eInt:
			l_return = TestFrameVariableCreator< FrameVariableType::eInt >( l_program, p_occurences );
			break;

		case FrameVariableType::eUInt:
			l_return = TestFrameVariableCreator< FrameVariableType::eUInt >( l_program, p_occurences );
			break;

		case FrameVariableType::eFloat:
			l_return = TestFrameVariableCreator< FrameVariableType::eFloat >( l_program, p_occurences );
			break;

		case FrameVariableType::eDouble:
			l_return = TestFrameVariableCreator< FrameVariableType::eDouble >( l_program, p_occurences );
			break;

		case FrameVariableType::eSampler:
			l_return = TestFrameVariableCreator< FrameVariableType::eSampler >( l_program, p_occurences );
			break;

		case FrameVariableType::eVec2i:
			l_return = TestFrameVariableCreator< FrameVariableType::eVec2i >( l_program, p_occurences );
			break;

		case FrameVariableType::eVec3i:
			l_return = TestFrameVariableCreator< FrameVariableType::eVec3i >( l_program, p_occurences );
			break;

		case FrameVariableType::eVec4i:
			l_return = TestFrameVariableCreator< FrameVariableType::eVec4i >( l_program, p_occurences );
			break;

		case FrameVariableType::eVec2ui:
			l_return = TestFrameVariableCreator< FrameVariableType::eVec2ui >( l_program, p_occurences );
			break;

		case FrameVariableType::eVec3ui:
			l_return = TestFrameVariableCreator< FrameVariableType::eVec3ui >( l_program, p_occurences );
			break;

		case FrameVariableType::eVec4ui:
			l_return = TestFrameVariableCreator< FrameVariableType::eVec4ui >( l_program, p_occurences );
			break;

		case FrameVariableType::eVec2f:
			l_return = TestFrameVariableCreator< FrameVariableType::eVec2f >( l_program, p_occurences );
			break;

		case FrameVariableType::eVec3f:
			l_return = TestFrameVariableCreator< FrameVariableType::eVec3f >( l_program, p_occurences );
			break;

		case FrameVariableType::eVec4f:
			l_return = TestFrameVariableCreator< FrameVariableType::eVec4f >( l_program, p_occurences );
			break;

		case FrameVariableType::eVec2d:
			l_return = TestFrameVariableCreator< FrameVariableType::eVec2d >( l_program, p_occurences );
			break;

		case FrameVariableType::eVec3d:
			l_return = TestFrameVariableCreator< FrameVariableType::eVec3d >( l_program, p_occurences );
			break;

		case FrameVariableType::eVec4d:
			l_return = TestFrameVariableCreator< FrameVariableType::eVec4d >( l_program, p_occurences );
			break;

		case FrameVariableType::eMat2x2f:
			l_return = TestFrameVariableCreator< FrameVariableType::eMat2x2f >( l_program, p_occurences );
			break;

		case FrameVariableType::eMat2x3f:
			l_return = TestFrameVariableCreator< FrameVariableType::eMat2x3f >( l_program, p_occurences );
			break;

		case FrameVariableType::eMat2x4f:
			l_return = TestFrameVariableCreator< FrameVariableType::eMat2x4f >( l_program, p_occurences );
			break;

		case FrameVariableType::eMat3x2f:
			l_return = TestFrameVariableCreator< FrameVariableType::eMat3x2f >( l_program, p_occurences );
			break;

		case FrameVariableType::eMat3x3f:
			l_return = TestFrameVariableCreator< FrameVariableType::eMat3x3f >( l_program, p_occurences );
			break;

		case FrameVariableType::eMat3x4f:
			l_return = TestFrameVariableCreator< FrameVariableType::eMat3x4f >( l_program, p_occurences );
			break;

		case FrameVariableType::eMat4x2f:
			l_return = TestFrameVariableCreator< FrameVariableType::eMat4x2f >( l_program, p_occurences );
			break;

		case FrameVariableType::eMat4x3f:
			l_return = TestFrameVariableCreator< FrameVariableType::eMat4x3f >( l_program, p_occurences );
			break;

		case FrameVariableType::eMat4x4f:
			l_return = TestFrameVariableCreator< FrameVariableType::eMat4x4f >( l_program, p_occurences );
			break;

		case FrameVariableType::eMat2x2d:
			l_return = TestFrameVariableCreator< FrameVariableType::eMat2x2d >( l_program, p_occurences );
			break;

		case FrameVariableType::eMat2x3d:
			l_return = TestFrameVariableCreator< FrameVariableType::eMat2x3d >( l_program, p_occurences );
			break;

		case FrameVariableType::eMat2x4d:
			l_return = TestFrameVariableCreator< FrameVariableType::eMat2x4d >( l_program, p_occurences );
			break;

		case FrameVariableType::eMat3x2d:
			l_return = TestFrameVariableCreator< FrameVariableType::eMat3x2d >( l_program, p_occurences );
			break;

		case FrameVariableType::eMat3x3d:
			l_return = TestFrameVariableCreator< FrameVariableType::eMat3x3d >( l_program, p_occurences );
			break;

		case FrameVariableType::eMat3x4d:
			l_return = TestFrameVariableCreator< FrameVariableType::eMat3x4d >( l_program, p_occurences );
			break;

		case FrameVariableType::eMat4x2d:
			l_return = TestFrameVariableCreator< FrameVariableType::eMat4x2d >( l_program, p_occurences );
			break;

		case FrameVariableType::eMat4x3d:
			l_return = TestFrameVariableCreator< FrameVariableType::eMat4x3d >( l_program, p_occurences );
			break;

		case FrameVariableType::eMat4x4d:
			l_return = TestFrameVariableCreator< FrameVariableType::eMat4x4d >( l_program, p_occurences );
			break;
		}

		if ( l_return )
		{
			l_return->SetName( p_name );
		}

		return l_return;
	}

	bool TestFrameVariableBuffer::DoInitialise()
	{
		return true;
	}

	void TestFrameVariableBuffer::DoCleanup()
	{
	}

	bool TestFrameVariableBuffer::DoBind( uint32_t p_index )
	{
		return true;
	}

	void TestFrameVariableBuffer::DoUnbind( uint32_t p_index )
	{
	}
}
