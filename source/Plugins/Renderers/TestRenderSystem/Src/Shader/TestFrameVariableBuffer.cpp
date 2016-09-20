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

		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::Int >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestOneFrameVariable< int > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::UInt >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestOneFrameVariable< uint32_t > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::Float >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestOneFrameVariable< float > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::Double >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestOneFrameVariable< double > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::Sampler >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestOneFrameVariable< int > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::Vec2i >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestPointFrameVariable< int, 2 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::Vec3i >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestPointFrameVariable< int, 3 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::Vec4i >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestPointFrameVariable< int, 4 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::Vec2ui >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestPointFrameVariable< uint32_t, 2 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::Vec3ui >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestPointFrameVariable< uint32_t, 3 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::Vec4ui >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestPointFrameVariable< uint32_t, 4 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::Vec2f >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestPointFrameVariable< float, 2 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::Vec3f >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestPointFrameVariable< float, 3 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::Vec4f >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestPointFrameVariable< float, 4 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::Vec2d >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestPointFrameVariable< double, 2 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::Vec3d >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestPointFrameVariable< double, 3 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::Vec4d >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestPointFrameVariable< double, 4 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::Mat2x2f >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixFrameVariable< float, 2, 2 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::Mat2x3f >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixFrameVariable< float, 2, 3 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::Mat2x4f >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixFrameVariable< float, 2, 4 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::Mat3x2f >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixFrameVariable< float, 3, 2 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::Mat3x3f >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixFrameVariable< float, 3, 3 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::Mat3x4f >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixFrameVariable< float, 3, 4 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::Mat4x2f >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixFrameVariable< float, 4, 2 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::Mat4x3f >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixFrameVariable< float, 4, 3 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::Mat4x4f >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixFrameVariable< float, 4, 4 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::Mat2x2d >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixFrameVariable< double, 2, 2 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::Mat2x3d >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixFrameVariable< double, 2, 3 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::Mat2x4d >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixFrameVariable< double, 2, 4 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::Mat3x2d >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixFrameVariable< double, 3, 2 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::Mat3x3d >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixFrameVariable< double, 3, 3 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::Mat3x4d >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixFrameVariable< double, 3, 4 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::Mat4x2d >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixFrameVariable< double, 4, 2 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::Mat4x3d >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixFrameVariable< double, 4, 3 > >( p_occurences, p_program );
		}
		template<> FrameVariableSPtr TestFrameVariableCreator< FrameVariableType::Mat4x4d >( TestShaderProgram & p_program, uint32_t p_occurences )
		{
			return std::make_shared< TestMatrixFrameVariable< double, 4, 4 > >( p_occurences, p_program );
		}
	}

	TestFrameVariableBuffer::TestFrameVariableBuffer( String const & p_name, ShaderProgram & p_program, RenderSystem & p_renderSystem )
		: FrameVariableBuffer( p_name, p_program, p_renderSystem )
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
		case FrameVariableType::Int:
			l_return = TestFrameVariableCreator< FrameVariableType::Int >( l_program, p_occurences );
			break;

		case FrameVariableType::UInt:
			l_return = TestFrameVariableCreator< FrameVariableType::UInt >( l_program, p_occurences );
			break;

		case FrameVariableType::Float:
			l_return = TestFrameVariableCreator< FrameVariableType::Float >( l_program, p_occurences );
			break;

		case FrameVariableType::Double:
			l_return = TestFrameVariableCreator< FrameVariableType::Double >( l_program, p_occurences );
			break;

		case FrameVariableType::Sampler:
			l_return = TestFrameVariableCreator< FrameVariableType::Sampler >( l_program, p_occurences );
			break;

		case FrameVariableType::Vec2i:
			l_return = TestFrameVariableCreator< FrameVariableType::Vec2i >( l_program, p_occurences );
			break;

		case FrameVariableType::Vec3i:
			l_return = TestFrameVariableCreator< FrameVariableType::Vec3i >( l_program, p_occurences );
			break;

		case FrameVariableType::Vec4i:
			l_return = TestFrameVariableCreator< FrameVariableType::Vec4i >( l_program, p_occurences );
			break;

		case FrameVariableType::Vec2ui:
			l_return = TestFrameVariableCreator< FrameVariableType::Vec2ui >( l_program, p_occurences );
			break;

		case FrameVariableType::Vec3ui:
			l_return = TestFrameVariableCreator< FrameVariableType::Vec3ui >( l_program, p_occurences );
			break;

		case FrameVariableType::Vec4ui:
			l_return = TestFrameVariableCreator< FrameVariableType::Vec4ui >( l_program, p_occurences );
			break;

		case FrameVariableType::Vec2f:
			l_return = TestFrameVariableCreator< FrameVariableType::Vec2f >( l_program, p_occurences );
			break;

		case FrameVariableType::Vec3f:
			l_return = TestFrameVariableCreator< FrameVariableType::Vec3f >( l_program, p_occurences );
			break;

		case FrameVariableType::Vec4f:
			l_return = TestFrameVariableCreator< FrameVariableType::Vec4f >( l_program, p_occurences );
			break;

		case FrameVariableType::Vec2d:
			l_return = TestFrameVariableCreator< FrameVariableType::Vec2d >( l_program, p_occurences );
			break;

		case FrameVariableType::Vec3d:
			l_return = TestFrameVariableCreator< FrameVariableType::Vec3d >( l_program, p_occurences );
			break;

		case FrameVariableType::Vec4d:
			l_return = TestFrameVariableCreator< FrameVariableType::Vec4d >( l_program, p_occurences );
			break;

		case FrameVariableType::Mat2x2f:
			l_return = TestFrameVariableCreator< FrameVariableType::Mat2x2f >( l_program, p_occurences );
			break;

		case FrameVariableType::Mat2x3f:
			l_return = TestFrameVariableCreator< FrameVariableType::Mat2x3f >( l_program, p_occurences );
			break;

		case FrameVariableType::Mat2x4f:
			l_return = TestFrameVariableCreator< FrameVariableType::Mat2x4f >( l_program, p_occurences );
			break;

		case FrameVariableType::Mat3x2f:
			l_return = TestFrameVariableCreator< FrameVariableType::Mat3x2f >( l_program, p_occurences );
			break;

		case FrameVariableType::Mat3x3f:
			l_return = TestFrameVariableCreator< FrameVariableType::Mat3x3f >( l_program, p_occurences );
			break;

		case FrameVariableType::Mat3x4f:
			l_return = TestFrameVariableCreator< FrameVariableType::Mat3x4f >( l_program, p_occurences );
			break;

		case FrameVariableType::Mat4x2f:
			l_return = TestFrameVariableCreator< FrameVariableType::Mat4x2f >( l_program, p_occurences );
			break;

		case FrameVariableType::Mat4x3f:
			l_return = TestFrameVariableCreator< FrameVariableType::Mat4x3f >( l_program, p_occurences );
			break;

		case FrameVariableType::Mat4x4f:
			l_return = TestFrameVariableCreator< FrameVariableType::Mat4x4f >( l_program, p_occurences );
			break;

		case FrameVariableType::Mat2x2d:
			l_return = TestFrameVariableCreator< FrameVariableType::Mat2x2d >( l_program, p_occurences );
			break;

		case FrameVariableType::Mat2x3d:
			l_return = TestFrameVariableCreator< FrameVariableType::Mat2x3d >( l_program, p_occurences );
			break;

		case FrameVariableType::Mat2x4d:
			l_return = TestFrameVariableCreator< FrameVariableType::Mat2x4d >( l_program, p_occurences );
			break;

		case FrameVariableType::Mat3x2d:
			l_return = TestFrameVariableCreator< FrameVariableType::Mat3x2d >( l_program, p_occurences );
			break;

		case FrameVariableType::Mat3x3d:
			l_return = TestFrameVariableCreator< FrameVariableType::Mat3x3d >( l_program, p_occurences );
			break;

		case FrameVariableType::Mat3x4d:
			l_return = TestFrameVariableCreator< FrameVariableType::Mat3x4d >( l_program, p_occurences );
			break;

		case FrameVariableType::Mat4x2d:
			l_return = TestFrameVariableCreator< FrameVariableType::Mat4x2d >( l_program, p_occurences );
			break;

		case FrameVariableType::Mat4x3d:
			l_return = TestFrameVariableCreator< FrameVariableType::Mat4x3d >( l_program, p_occurences );
			break;

		case FrameVariableType::Mat4x4d:
			l_return = TestFrameVariableCreator< FrameVariableType::Mat4x4d >( l_program, p_occurences );
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
