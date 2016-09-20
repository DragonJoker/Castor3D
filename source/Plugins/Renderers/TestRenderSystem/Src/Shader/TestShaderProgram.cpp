#include "Shader/TestShaderProgram.hpp"

#include "Render/TestRenderSystem.hpp"
#include "Shader/TestFrameVariableBuffer.hpp"
#include "Shader/TestOneFrameVariable.hpp"
#include "Shader/TestPointFrameVariable.hpp"
#include "Shader/TestMatrixFrameVariable.hpp"
#include "Shader/TestShaderObject.hpp"

#include <Log/Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace TestRender
{
	TestShaderProgram::TestShaderProgram( TestRenderSystem & p_renderSystem )
		: ShaderProgram( p_renderSystem )
		, m_layout( p_renderSystem )
	{
		CreateObject( ShaderType::Vertex );
		CreateObject( ShaderType::Pixel );
	}

	TestShaderProgram::~TestShaderProgram()
	{
	}

	void TestShaderProgram::Cleanup()
	{
		m_layout.Cleanup();
		DoCleanup();
	}

	bool TestShaderProgram::Initialise()
	{
		bool l_return = DoInitialise();

		if ( l_return )
		{
			m_layout.Initialise( *this );
		}

		return l_return;
	}

	bool TestShaderProgram::Link()
	{
		m_status = ePROGRAM_STATUS_LINKED;
		return true;
	}

	void TestShaderProgram::Bind( bool p_bindUbo )const
	{
	}

	void TestShaderProgram::Unbind()const
	{
	}

	ShaderObjectSPtr TestShaderProgram::DoCreateObject( ShaderType p_type )
	{
		return std::make_shared< TestShaderObject >( this, p_type );
	}

	FrameVariableBufferSPtr TestShaderProgram::DoCreateFrameVariableBuffer( Castor::String const & p_name )
	{
		return std::make_shared< TestFrameVariableBuffer >( p_name, *this, *GetRenderSystem() );
	}

	std::shared_ptr< FrameVariable > TestShaderProgram::DoCreateVariable( FrameVariableType p_type, int p_occurences )
	{
		switch ( p_type )
		{
		case FrameVariableType::Bool:
			return std::make_shared< TestOneFrameVariable< bool > >( p_occurences, *this );

		case FrameVariableType::Int:
			return std::make_shared< TestOneFrameVariable< int > >( p_occurences, *this );

		case FrameVariableType::UInt:
			return std::make_shared< TestOneFrameVariable< unsigned int > >( p_occurences, *this );

		case FrameVariableType::Float:
			return std::make_shared< TestOneFrameVariable< float > >( p_occurences, *this );

		case FrameVariableType::Double:
			return std::make_shared< TestOneFrameVariable< double > >( p_occurences, *this );

		case FrameVariableType::Sampler:
			return std::make_shared< TestOneFrameVariable< int > >( p_occurences, *this );

		case FrameVariableType::Vec2b:
			return std::make_shared< TestPointFrameVariable< bool, 2 > >( p_occurences, *this );

		case FrameVariableType::Vec3b:
			return std::make_shared< TestPointFrameVariable< bool, 3 > >( p_occurences, *this );

		case FrameVariableType::Vec4b:
			return std::make_shared< TestPointFrameVariable< bool, 4 > >( p_occurences, *this );

		case FrameVariableType::Vec2i:
			return std::make_shared< TestPointFrameVariable< int, 2 > >( p_occurences, *this );

		case FrameVariableType::Vec3i:
			return std::make_shared< TestPointFrameVariable< int, 3 > >( p_occurences, *this );

		case FrameVariableType::Vec4i:
			return std::make_shared< TestPointFrameVariable< int, 4 > >( p_occurences, *this );

		case FrameVariableType::Vec2ui:
			return std::make_shared< TestPointFrameVariable< unsigned int, 2 > >( p_occurences, *this );

		case FrameVariableType::Vec3ui:
			return std::make_shared< TestPointFrameVariable< unsigned int, 3 > >( p_occurences, *this );

		case FrameVariableType::Vec4ui:
			return std::make_shared< TestPointFrameVariable< unsigned int, 4 > >( p_occurences, *this );

		case FrameVariableType::Vec2f:
			return std::make_shared< TestPointFrameVariable< float, 2 > >( p_occurences, *this );

		case FrameVariableType::Vec3f:
			return std::make_shared< TestPointFrameVariable< float, 3 > >( p_occurences, *this );

		case FrameVariableType::Vec4f:
			return std::make_shared< TestPointFrameVariable< float, 4 > >( p_occurences, *this );

		case FrameVariableType::Vec2d:
			return std::make_shared< TestPointFrameVariable< double, 2 > >( p_occurences, *this );

		case FrameVariableType::Vec3d:
			return std::make_shared< TestPointFrameVariable< double, 3 > >( p_occurences, *this );

		case FrameVariableType::Vec4d:
			return std::make_shared< TestPointFrameVariable< double, 4 > >( p_occurences, *this );

		case FrameVariableType::Mat2x2b:
			return std::make_shared< TestMatrixFrameVariable< bool, 2, 2 > >( p_occurences, *this );

		case FrameVariableType::Mat2x3b:
			return std::make_shared< TestMatrixFrameVariable< bool, 2, 3 > >( p_occurences, *this );

		case FrameVariableType::Mat2x4b:
			return std::make_shared< TestMatrixFrameVariable< bool, 2, 4 > >( p_occurences, *this );

		case FrameVariableType::Mat3x2b:
			return std::make_shared< TestMatrixFrameVariable< bool, 3, 2 > >( p_occurences, *this );

		case FrameVariableType::Mat3x3b:
			return std::make_shared< TestMatrixFrameVariable< bool, 3, 3 > >( p_occurences, *this );

		case FrameVariableType::Mat3x4b:
			return std::make_shared< TestMatrixFrameVariable< bool, 3, 4 > >( p_occurences, *this );

		case FrameVariableType::Mat4x2b:
			return std::make_shared< TestMatrixFrameVariable< bool, 4, 2 > >( p_occurences, *this );

		case FrameVariableType::Mat4x3b:
			return std::make_shared< TestMatrixFrameVariable< bool, 4, 3 > >( p_occurences, *this );

		case FrameVariableType::Mat4x4b:
			return std::make_shared< TestMatrixFrameVariable< bool, 4, 4 > >( p_occurences, *this );

		case FrameVariableType::Mat2x2i:
			return std::make_shared< TestMatrixFrameVariable< int, 2, 2 > >( p_occurences, *this );

		case FrameVariableType::Mat2x3i:
			return std::make_shared< TestMatrixFrameVariable< int, 2, 3 > >( p_occurences, *this );

		case FrameVariableType::Mat2x4i:
			return std::make_shared< TestMatrixFrameVariable< int, 2, 4 > >( p_occurences, *this );

		case FrameVariableType::Mat3x2i:
			return std::make_shared< TestMatrixFrameVariable< int, 3, 2 > >( p_occurences, *this );

		case FrameVariableType::Mat3x3i:
			return std::make_shared< TestMatrixFrameVariable< int, 3, 3 > >( p_occurences, *this );

		case FrameVariableType::Mat3x4i:
			return std::make_shared< TestMatrixFrameVariable< int, 3, 4 > >( p_occurences, *this );

		case FrameVariableType::Mat4x2i:
			return std::make_shared< TestMatrixFrameVariable< int, 4, 2 > >( p_occurences, *this );

		case FrameVariableType::Mat4x3i:
			return std::make_shared< TestMatrixFrameVariable< int, 4, 3 > >( p_occurences, *this );

		case FrameVariableType::Mat4x4i:
			return std::make_shared< TestMatrixFrameVariable< int, 4, 4 > >( p_occurences, *this );

		case FrameVariableType::Mat2x2ui:
			return std::make_shared< TestMatrixFrameVariable< unsigned int, 2, 2 > >( p_occurences, *this );

		case FrameVariableType::Mat2x3ui:
			return std::make_shared< TestMatrixFrameVariable< unsigned int, 2, 3 > >( p_occurences, *this );

		case FrameVariableType::Mat2x4ui:
			return std::make_shared< TestMatrixFrameVariable< unsigned int, 2, 4 > >( p_occurences, *this );

		case FrameVariableType::Mat3x2ui:
			return std::make_shared< TestMatrixFrameVariable< unsigned int, 3, 2 > >( p_occurences, *this );

		case FrameVariableType::Mat3x3ui:
			return std::make_shared< TestMatrixFrameVariable< unsigned int, 3, 3 > >( p_occurences, *this );

		case FrameVariableType::Mat3x4ui:
			return std::make_shared< TestMatrixFrameVariable< unsigned int, 3, 4 > >( p_occurences, *this );

		case FrameVariableType::Mat4x2ui:
			return std::make_shared< TestMatrixFrameVariable< unsigned int, 4, 2 > >( p_occurences, *this );

		case FrameVariableType::Mat4x3ui:
			return std::make_shared< TestMatrixFrameVariable< unsigned int, 4, 3 > >( p_occurences, *this );

		case FrameVariableType::Mat4x4ui:
			return std::make_shared< TestMatrixFrameVariable< unsigned int, 4, 4 > >( p_occurences, *this );

		case FrameVariableType::Mat2x2f:
			return std::make_shared< TestMatrixFrameVariable< float, 2, 2 > >( p_occurences, *this );

		case FrameVariableType::Mat2x3f:
			return std::make_shared< TestMatrixFrameVariable< float, 2, 3 > >( p_occurences, *this );

		case FrameVariableType::Mat2x4f:
			return std::make_shared< TestMatrixFrameVariable< float, 2, 4 > >( p_occurences, *this );

		case FrameVariableType::Mat3x2f:
			return std::make_shared< TestMatrixFrameVariable< float, 3, 2 > >( p_occurences, *this );

		case FrameVariableType::Mat3x3f:
			return std::make_shared< TestMatrixFrameVariable< float, 3, 3 > >( p_occurences, *this );

		case FrameVariableType::Mat3x4f:
			return std::make_shared< TestMatrixFrameVariable< float, 3, 4 > >( p_occurences, *this );

		case FrameVariableType::Mat4x2f:
			return std::make_shared< TestMatrixFrameVariable< float, 4, 2 > >( p_occurences, *this );

		case FrameVariableType::Mat4x3f:
			return std::make_shared< TestMatrixFrameVariable< float, 4, 3 > >( p_occurences, *this );

		case FrameVariableType::Mat4x4f:
			return std::make_shared< TestMatrixFrameVariable< float, 4, 4 > >( p_occurences, *this );

		case FrameVariableType::Mat2x2d:
			return std::make_shared< TestMatrixFrameVariable< double, 2, 2 > >( p_occurences, *this );

		case FrameVariableType::Mat2x3d:
			return std::make_shared< TestMatrixFrameVariable< double, 2, 3 > >( p_occurences, *this );

		case FrameVariableType::Mat2x4d:
			return std::make_shared< TestMatrixFrameVariable< double, 2, 4 > >( p_occurences, *this );

		case FrameVariableType::Mat3x2d:
			return std::make_shared< TestMatrixFrameVariable< double, 3, 2 > >( p_occurences, *this );

		case FrameVariableType::Mat3x3d:
			return std::make_shared< TestMatrixFrameVariable< double, 3, 3 > >( p_occurences, *this );

		case FrameVariableType::Mat3x4d:
			return std::make_shared< TestMatrixFrameVariable< double, 3, 4 > >( p_occurences, *this );

		case FrameVariableType::Mat4x2d:
			return std::make_shared< TestMatrixFrameVariable< double, 4, 2 > >( p_occurences, *this );

		case FrameVariableType::Mat4x3d:
			return std::make_shared< TestMatrixFrameVariable< double, 4, 3 > >( p_occurences, *this );

		case FrameVariableType::Mat4x4d:
			return std::make_shared< TestMatrixFrameVariable< double, 4, 4 > >( p_occurences, *this );

		default:
			FAILURE( cuT( "Unsupported frame variable type" ) );
			return nullptr;
		}
	}
}
