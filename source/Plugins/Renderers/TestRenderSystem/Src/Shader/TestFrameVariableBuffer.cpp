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
		template< eFRAME_VARIABLE_TYPE Type > FrameVariableSPtr TestFrameVariableCreator( TestShaderProgram * p_program, uint32_t p_occurences );
		template<> FrameVariableSPtr TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_INT >( TestShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestOneFrameVariable< int > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_UINT >( TestShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestOneFrameVariable< uint32_t > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_FLOAT >( TestShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestOneFrameVariable< float > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_DOUBLE >( TestShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestOneFrameVariable< double > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_SAMPLER >( TestShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestOneFrameVariable< int > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC2I >( TestShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestPointFrameVariable< int, 2 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC3I >( TestShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestPointFrameVariable< int, 3 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC4I >( TestShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestPointFrameVariable< int, 4 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC2UI >( TestShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestPointFrameVariable< uint32_t, 2 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC3UI >( TestShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestPointFrameVariable< uint32_t, 3 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC4UI >( TestShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestPointFrameVariable< uint32_t, 4 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC2F >( TestShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestPointFrameVariable< float, 2 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC3F >( TestShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestPointFrameVariable< float, 3 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC4F >( TestShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestPointFrameVariable< float, 4 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC2D >( TestShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestPointFrameVariable< double, 2 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC3D >( TestShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestPointFrameVariable< double, 3 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC4D >( TestShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestPointFrameVariable< double, 4 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT2X2F >( TestShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixFrameVariable< float, 2, 2 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT2X3F >( TestShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixFrameVariable< float, 2, 3 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT2X4F >( TestShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixFrameVariable< float, 2, 4 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT3X2F >( TestShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixFrameVariable< float, 3, 2 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT3X3F >( TestShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixFrameVariable< float, 3, 3 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT3X4F >( TestShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixFrameVariable< float, 3, 4 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT4X2F >( TestShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixFrameVariable< float, 4, 2 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT4X3F >( TestShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixFrameVariable< float, 4, 3 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT4X4F >( TestShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixFrameVariable< float, 4, 4 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT2X2D >( TestShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixFrameVariable< double, 2, 2 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT2X3D >( TestShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixFrameVariable< double, 2, 3 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT2X4D >( TestShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixFrameVariable< double, 2, 4 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT3X2D >( TestShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixFrameVariable< double, 3, 2 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT3X3D >( TestShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixFrameVariable< double, 3, 3 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT3X4D >( TestShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixFrameVariable< double, 3, 4 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT4X2D >( TestShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixFrameVariable< double, 4, 2 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT4X3D >( TestShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixFrameVariable< double, 4, 3 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT4X4D >( TestShaderProgram * p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixFrameVariable< double, 4, 4 > >( p_occurences, p_program );
		}
	}

	TestFrameVariableBuffer::TestFrameVariableBuffer( String const & p_name, TestRenderSystem & p_renderSystem )
		: FrameVariableBuffer( p_name, p_renderSystem )
	{
	}

	TestFrameVariableBuffer::~TestFrameVariableBuffer()
	{
	}

	FrameVariableSPtr TestFrameVariableBuffer::DoCreateVariable( ShaderProgram * p_program, eFRAME_VARIABLE_TYPE p_type, Castor::String const & p_name, uint32_t p_occurences )
	{
		FrameVariableSPtr l_return;
		TestShaderProgram * l_program = static_cast< TestShaderProgram * >( p_program );

		switch ( p_type )
		{
		case eFRAME_VARIABLE_TYPE_INT:
			l_return = TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_INT >( l_program, p_occurences );
			break;

		case eFRAME_VARIABLE_TYPE_UINT:
			l_return = TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_UINT >( l_program, p_occurences );
			break;

		case eFRAME_VARIABLE_TYPE_FLOAT:
			l_return = TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_FLOAT >( l_program, p_occurences );
			break;

		case eFRAME_VARIABLE_TYPE_DOUBLE:
			l_return = TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_DOUBLE >( l_program, p_occurences );
			break;

		case eFRAME_VARIABLE_TYPE_SAMPLER:
			l_return = TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_SAMPLER >( l_program, p_occurences );
			break;

		case eFRAME_VARIABLE_TYPE_VEC2I:
			l_return = TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC2I >( l_program, p_occurences );
			break;

		case eFRAME_VARIABLE_TYPE_VEC3I:
			l_return = TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC3I >( l_program, p_occurences );
			break;

		case eFRAME_VARIABLE_TYPE_VEC4I:
			l_return = TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC4I >( l_program, p_occurences );
			break;

		case eFRAME_VARIABLE_TYPE_VEC2UI:
			l_return = TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC2UI >( l_program, p_occurences );
			break;

		case eFRAME_VARIABLE_TYPE_VEC3UI:
			l_return = TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC3UI >( l_program, p_occurences );
			break;

		case eFRAME_VARIABLE_TYPE_VEC4UI:
			l_return = TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC4UI >( l_program, p_occurences );
			break;

		case eFRAME_VARIABLE_TYPE_VEC2F:
			l_return = TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC2F >( l_program, p_occurences );
			break;

		case eFRAME_VARIABLE_TYPE_VEC3F:
			l_return = TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC3F >( l_program, p_occurences );
			break;

		case eFRAME_VARIABLE_TYPE_VEC4F:
			l_return = TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC4F >( l_program, p_occurences );
			break;

		case eFRAME_VARIABLE_TYPE_VEC2D:
			l_return = TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC2D >( l_program, p_occurences );
			break;

		case eFRAME_VARIABLE_TYPE_VEC3D:
			l_return = TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC3D >( l_program, p_occurences );
			break;

		case eFRAME_VARIABLE_TYPE_VEC4D:
			l_return = TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC4D >( l_program, p_occurences );
			break;

		case eFRAME_VARIABLE_TYPE_MAT2X2F:
			l_return = TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT2X2F >( l_program, p_occurences );
			break;

		case eFRAME_VARIABLE_TYPE_MAT2X3F:
			l_return = TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT2X3F >( l_program, p_occurences );
			break;

		case eFRAME_VARIABLE_TYPE_MAT2X4F:
			l_return = TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT2X4F >( l_program, p_occurences );
			break;

		case eFRAME_VARIABLE_TYPE_MAT3X2F:
			l_return = TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT3X2F >( l_program, p_occurences );
			break;

		case eFRAME_VARIABLE_TYPE_MAT3X3F:
			l_return = TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT3X3F >( l_program, p_occurences );
			break;

		case eFRAME_VARIABLE_TYPE_MAT3X4F:
			l_return = TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT3X4F >( l_program, p_occurences );
			break;

		case eFRAME_VARIABLE_TYPE_MAT4X2F:
			l_return = TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT4X2F >( l_program, p_occurences );
			break;

		case eFRAME_VARIABLE_TYPE_MAT4X3F:
			l_return = TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT4X3F >( l_program, p_occurences );
			break;

		case eFRAME_VARIABLE_TYPE_MAT4X4F:
			l_return = TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT4X4F >( l_program, p_occurences );
			break;

		case eFRAME_VARIABLE_TYPE_MAT2X2D:
			l_return = TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT2X2D >( l_program, p_occurences );
			break;

		case eFRAME_VARIABLE_TYPE_MAT2X3D:
			l_return = TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT2X3D >( l_program, p_occurences );
			break;

		case eFRAME_VARIABLE_TYPE_MAT2X4D:
			l_return = TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT2X4D >( l_program, p_occurences );
			break;

		case eFRAME_VARIABLE_TYPE_MAT3X2D:
			l_return = TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT3X2D >( l_program, p_occurences );
			break;

		case eFRAME_VARIABLE_TYPE_MAT3X3D:
			l_return = TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT3X3D >( l_program, p_occurences );
			break;

		case eFRAME_VARIABLE_TYPE_MAT3X4D:
			l_return = TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT3X4D >( l_program, p_occurences );
			break;

		case eFRAME_VARIABLE_TYPE_MAT4X2D:
			l_return = TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT4X2D >( l_program, p_occurences );
			break;

		case eFRAME_VARIABLE_TYPE_MAT4X3D:
			l_return = TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT4X3D >( l_program, p_occurences );
			break;

		case eFRAME_VARIABLE_TYPE_MAT4X4D:
			l_return = TestFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT4X4D >( l_program, p_occurences );
			break;
		}

		if ( l_return )
		{
			l_return->SetName( p_name );
		}

		return l_return;
	}

	bool TestFrameVariableBuffer::DoInitialise( ShaderProgram * p_program )
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
