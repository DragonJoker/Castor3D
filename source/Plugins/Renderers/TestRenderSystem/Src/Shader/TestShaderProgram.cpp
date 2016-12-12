#include "Shader/TestShaderProgram.hpp"

#include "Render/TestRenderSystem.hpp"
#include "Shader/TestUniformBuffer.hpp"
#include "Shader/TestOneUniform.hpp"
#include "Shader/TestPointUniform.hpp"
#include "Shader/TestMatrixUniform.hpp"
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
		CreateObject( ShaderType::eVertex );
		CreateObject( ShaderType::ePixel );
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
		m_status = ProgramStatus::eLinked;
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

	UniformBufferSPtr TestShaderProgram::DoCreateUniformBuffer( Castor::String const & p_name, FlagCombination< ShaderTypeFlag > const & p_flags )
	{
		return std::make_shared< TestUniformBuffer >( p_name, *this, p_flags, *GetRenderSystem() );
	}

	std::shared_ptr< Uniform > TestShaderProgram::DoCreateVariable( UniformType p_type, int p_occurences )
	{
		switch ( p_type )
		{
		case UniformType::eBool:
			return std::make_shared< TestOneUniform< bool > >( p_occurences, *this );

		case UniformType::eInt:
			return std::make_shared< TestOneUniform< int > >( p_occurences, *this );

		case UniformType::eUInt:
			return std::make_shared< TestOneUniform< unsigned int > >( p_occurences, *this );

		case UniformType::eFloat:
			return std::make_shared< TestOneUniform< float > >( p_occurences, *this );

		case UniformType::eDouble:
			return std::make_shared< TestOneUniform< double > >( p_occurences, *this );

		case UniformType::eSampler:
			return std::make_shared< TestOneUniform< int > >( p_occurences, *this );

		case UniformType::eVec2b:
			return std::make_shared< TestPointUniform< bool, 2 > >( p_occurences, *this );

		case UniformType::eVec3b:
			return std::make_shared< TestPointUniform< bool, 3 > >( p_occurences, *this );

		case UniformType::eVec4b:
			return std::make_shared< TestPointUniform< bool, 4 > >( p_occurences, *this );

		case UniformType::eVec2i:
			return std::make_shared< TestPointUniform< int, 2 > >( p_occurences, *this );

		case UniformType::eVec3i:
			return std::make_shared< TestPointUniform< int, 3 > >( p_occurences, *this );

		case UniformType::eVec4i:
			return std::make_shared< TestPointUniform< int, 4 > >( p_occurences, *this );

		case UniformType::eVec2ui:
			return std::make_shared< TestPointUniform< unsigned int, 2 > >( p_occurences, *this );

		case UniformType::eVec3ui:
			return std::make_shared< TestPointUniform< unsigned int, 3 > >( p_occurences, *this );

		case UniformType::eVec4ui:
			return std::make_shared< TestPointUniform< unsigned int, 4 > >( p_occurences, *this );

		case UniformType::eVec2f:
			return std::make_shared< TestPointUniform< float, 2 > >( p_occurences, *this );

		case UniformType::eVec3f:
			return std::make_shared< TestPointUniform< float, 3 > >( p_occurences, *this );

		case UniformType::eVec4f:
			return std::make_shared< TestPointUniform< float, 4 > >( p_occurences, *this );

		case UniformType::eVec2d:
			return std::make_shared< TestPointUniform< double, 2 > >( p_occurences, *this );

		case UniformType::eVec3d:
			return std::make_shared< TestPointUniform< double, 3 > >( p_occurences, *this );

		case UniformType::eVec4d:
			return std::make_shared< TestPointUniform< double, 4 > >( p_occurences, *this );

		case UniformType::eMat2x2b:
			return std::make_shared< TestMatrixUniform< bool, 2, 2 > >( p_occurences, *this );

		case UniformType::eMat2x3b:
			return std::make_shared< TestMatrixUniform< bool, 2, 3 > >( p_occurences, *this );

		case UniformType::eMat2x4b:
			return std::make_shared< TestMatrixUniform< bool, 2, 4 > >( p_occurences, *this );

		case UniformType::eMat3x2b:
			return std::make_shared< TestMatrixUniform< bool, 3, 2 > >( p_occurences, *this );

		case UniformType::eMat3x3b:
			return std::make_shared< TestMatrixUniform< bool, 3, 3 > >( p_occurences, *this );

		case UniformType::eMat3x4b:
			return std::make_shared< TestMatrixUniform< bool, 3, 4 > >( p_occurences, *this );

		case UniformType::eMat4x2b:
			return std::make_shared< TestMatrixUniform< bool, 4, 2 > >( p_occurences, *this );

		case UniformType::eMat4x3b:
			return std::make_shared< TestMatrixUniform< bool, 4, 3 > >( p_occurences, *this );

		case UniformType::eMat4x4b:
			return std::make_shared< TestMatrixUniform< bool, 4, 4 > >( p_occurences, *this );

		case UniformType::eMat2x2i:
			return std::make_shared< TestMatrixUniform< int, 2, 2 > >( p_occurences, *this );

		case UniformType::eMat2x3i:
			return std::make_shared< TestMatrixUniform< int, 2, 3 > >( p_occurences, *this );

		case UniformType::eMat2x4i:
			return std::make_shared< TestMatrixUniform< int, 2, 4 > >( p_occurences, *this );

		case UniformType::eMat3x2i:
			return std::make_shared< TestMatrixUniform< int, 3, 2 > >( p_occurences, *this );

		case UniformType::eMat3x3i:
			return std::make_shared< TestMatrixUniform< int, 3, 3 > >( p_occurences, *this );

		case UniformType::eMat3x4i:
			return std::make_shared< TestMatrixUniform< int, 3, 4 > >( p_occurences, *this );

		case UniformType::eMat4x2i:
			return std::make_shared< TestMatrixUniform< int, 4, 2 > >( p_occurences, *this );

		case UniformType::eMat4x3i:
			return std::make_shared< TestMatrixUniform< int, 4, 3 > >( p_occurences, *this );

		case UniformType::eMat4x4i:
			return std::make_shared< TestMatrixUniform< int, 4, 4 > >( p_occurences, *this );

		case UniformType::eMat2x2ui:
			return std::make_shared< TestMatrixUniform< unsigned int, 2, 2 > >( p_occurences, *this );

		case UniformType::eMat2x3ui:
			return std::make_shared< TestMatrixUniform< unsigned int, 2, 3 > >( p_occurences, *this );

		case UniformType::eMat2x4ui:
			return std::make_shared< TestMatrixUniform< unsigned int, 2, 4 > >( p_occurences, *this );

		case UniformType::eMat3x2ui:
			return std::make_shared< TestMatrixUniform< unsigned int, 3, 2 > >( p_occurences, *this );

		case UniformType::eMat3x3ui:
			return std::make_shared< TestMatrixUniform< unsigned int, 3, 3 > >( p_occurences, *this );

		case UniformType::eMat3x4ui:
			return std::make_shared< TestMatrixUniform< unsigned int, 3, 4 > >( p_occurences, *this );

		case UniformType::eMat4x2ui:
			return std::make_shared< TestMatrixUniform< unsigned int, 4, 2 > >( p_occurences, *this );

		case UniformType::eMat4x3ui:
			return std::make_shared< TestMatrixUniform< unsigned int, 4, 3 > >( p_occurences, *this );

		case UniformType::eMat4x4ui:
			return std::make_shared< TestMatrixUniform< unsigned int, 4, 4 > >( p_occurences, *this );

		case UniformType::eMat2x2f:
			return std::make_shared< TestMatrixUniform< float, 2, 2 > >( p_occurences, *this );

		case UniformType::eMat2x3f:
			return std::make_shared< TestMatrixUniform< float, 2, 3 > >( p_occurences, *this );

		case UniformType::eMat2x4f:
			return std::make_shared< TestMatrixUniform< float, 2, 4 > >( p_occurences, *this );

		case UniformType::eMat3x2f:
			return std::make_shared< TestMatrixUniform< float, 3, 2 > >( p_occurences, *this );

		case UniformType::eMat3x3f:
			return std::make_shared< TestMatrixUniform< float, 3, 3 > >( p_occurences, *this );

		case UniformType::eMat3x4f:
			return std::make_shared< TestMatrixUniform< float, 3, 4 > >( p_occurences, *this );

		case UniformType::eMat4x2f:
			return std::make_shared< TestMatrixUniform< float, 4, 2 > >( p_occurences, *this );

		case UniformType::eMat4x3f:
			return std::make_shared< TestMatrixUniform< float, 4, 3 > >( p_occurences, *this );

		case UniformType::eMat4x4f:
			return std::make_shared< TestMatrixUniform< float, 4, 4 > >( p_occurences, *this );

		case UniformType::eMat2x2d:
			return std::make_shared< TestMatrixUniform< double, 2, 2 > >( p_occurences, *this );

		case UniformType::eMat2x3d:
			return std::make_shared< TestMatrixUniform< double, 2, 3 > >( p_occurences, *this );

		case UniformType::eMat2x4d:
			return std::make_shared< TestMatrixUniform< double, 2, 4 > >( p_occurences, *this );

		case UniformType::eMat3x2d:
			return std::make_shared< TestMatrixUniform< double, 3, 2 > >( p_occurences, *this );

		case UniformType::eMat3x3d:
			return std::make_shared< TestMatrixUniform< double, 3, 3 > >( p_occurences, *this );

		case UniformType::eMat3x4d:
			return std::make_shared< TestMatrixUniform< double, 3, 4 > >( p_occurences, *this );

		case UniformType::eMat4x2d:
			return std::make_shared< TestMatrixUniform< double, 4, 2 > >( p_occurences, *this );

		case UniformType::eMat4x3d:
			return std::make_shared< TestMatrixUniform< double, 4, 3 > >( p_occurences, *this );

		case UniformType::eMat4x4d:
			return std::make_shared< TestMatrixUniform< double, 4, 4 > >( p_occurences, *this );

		default:
			FAILURE( cuT( "Unsupported frame variable type" ) );
			return nullptr;
		}
	}
}
