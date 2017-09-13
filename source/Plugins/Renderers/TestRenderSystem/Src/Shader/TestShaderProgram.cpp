#include "Shader/TestShaderProgram.hpp"

#include "Render/TestRenderSystem.hpp"
#include "Shader/TestPushUniform.hpp"
#include "Shader/TestShaderObject.hpp"

#include <Log/Logger.hpp>

using namespace castor3d;
using namespace castor;

namespace TestRender
{
	TestShaderProgram::TestShaderProgram( TestRenderSystem & p_renderSystem )
		: ShaderProgram( p_renderSystem )
		, m_layout( p_renderSystem )
	{
		createObject( ShaderType::eVertex );
		createObject( ShaderType::ePixel );
	}

	TestShaderProgram::~TestShaderProgram()
	{
	}

	void TestShaderProgram::cleanup()
	{
		m_layout.cleanup();
		doCleanup();
	}

	bool TestShaderProgram::initialise()
	{
		bool result = doInitialise();

		if ( result )
		{
			m_layout.initialise( *this );
		}

		return result;
	}

	bool TestShaderProgram::link()
	{
		m_status = ProgramStatus::eLinked;
		return true;
	}

	void TestShaderProgram::bind()const
	{
	}

	void TestShaderProgram::unbind()const
	{
	}

	ShaderObjectSPtr TestShaderProgram::doCreateObject( ShaderType p_type )
	{
		return std::make_shared< TestShaderObject >( this, p_type );
	}

	std::shared_ptr< PushUniform > TestShaderProgram::doCreateUniform( UniformType p_type, int p_occurences )
	{
		switch ( p_type )
		{
		case UniformType::eBool:
			return std::make_shared< TestPushUniform< UniformType::eBool > >( *this, p_occurences );

		case UniformType::eInt:
			return std::make_shared< TestPushUniform< UniformType::eInt > >( *this, p_occurences );

		case UniformType::eUInt:
			return std::make_shared< TestPushUniform< UniformType::eUInt > >( *this, p_occurences );

		case UniformType::eFloat:
			return std::make_shared< TestPushUniform< UniformType::eFloat > >( *this, p_occurences );

		case UniformType::eDouble:
			return std::make_shared< TestPushUniform< UniformType::eDouble > >( *this, p_occurences );

		case UniformType::eSampler:
			return std::make_shared< TestPushUniform< UniformType::eSampler > >( *this, p_occurences );

		case UniformType::eVec2b:
			return std::make_shared< TestPushUniform< UniformType::eVec2b > >( *this, p_occurences );

		case UniformType::eVec3b:
			return std::make_shared< TestPushUniform< UniformType::eVec3b > >( *this, p_occurences );

		case UniformType::eVec4b:
			return std::make_shared< TestPushUniform< UniformType::eVec4b > >( *this, p_occurences );

		case UniformType::eVec2i:
			return std::make_shared< TestPushUniform< UniformType::eVec2i > >( *this, p_occurences );

		case UniformType::eVec3i:
			return std::make_shared< TestPushUniform< UniformType::eVec3i > >( *this, p_occurences );

		case UniformType::eVec4i:
			return std::make_shared< TestPushUniform< UniformType::eVec4i > >( *this, p_occurences );

		case UniformType::eVec2ui:
			return std::make_shared< TestPushUniform< UniformType::eVec2ui > >( *this, p_occurences );

		case UniformType::eVec3ui:
			return std::make_shared< TestPushUniform< UniformType::eVec3ui > >( *this, p_occurences );

		case UniformType::eVec4ui:
			return std::make_shared< TestPushUniform< UniformType::eVec4ui > >( *this, p_occurences );

		case UniformType::eVec2f:
			return std::make_shared< TestPushUniform< UniformType::eVec2f > >( *this, p_occurences );

		case UniformType::eVec3f:
			return std::make_shared< TestPushUniform< UniformType::eVec3f > >( *this, p_occurences );

		case UniformType::eVec4f:
			return std::make_shared< TestPushUniform< UniformType::eVec4f > >( *this, p_occurences );

		case UniformType::eVec2d:
			return std::make_shared< TestPushUniform< UniformType::eVec2d > >( *this, p_occurences );

		case UniformType::eVec3d:
			return std::make_shared< TestPushUniform< UniformType::eVec3d > >( *this, p_occurences );

		case UniformType::eVec4d:
			return std::make_shared< TestPushUniform< UniformType::eVec4d > >( *this, p_occurences );

		case UniformType::eMat2x2b:	
			return std::make_shared< TestPushUniform< UniformType::eMat2x2b > >( *this, p_occurences );

		case UniformType::eMat2x3b:
			return std::make_shared< TestPushUniform< UniformType::eMat2x3b > >( *this, p_occurences );

		case UniformType::eMat2x4b:
			return std::make_shared< TestPushUniform< UniformType::eMat2x4b > >( *this, p_occurences );

		case UniformType::eMat3x2b:
			return std::make_shared< TestPushUniform< UniformType::eMat3x2b > >( *this, p_occurences );

		case UniformType::eMat3x3b:
			return std::make_shared< TestPushUniform< UniformType::eMat3x3b > >( *this, p_occurences );

		case UniformType::eMat3x4b:
			return std::make_shared< TestPushUniform< UniformType::eMat3x4b > >( *this, p_occurences );

		case UniformType::eMat4x2b:
			return std::make_shared< TestPushUniform< UniformType::eMat4x2b > >( *this, p_occurences );

		case UniformType::eMat4x3b:
			return std::make_shared< TestPushUniform< UniformType::eMat4x3b > >( *this, p_occurences );

		case UniformType::eMat4x4b:
			return std::make_shared< TestPushUniform< UniformType::eMat4x4b > >( *this, p_occurences );

		case UniformType::eMat2x2i:
			return std::make_shared< TestPushUniform< UniformType::eMat2x2i > >( *this, p_occurences );

		case UniformType::eMat2x3i:
			return std::make_shared< TestPushUniform< UniformType::eMat2x3i > >( *this, p_occurences );

		case UniformType::eMat2x4i:
			return std::make_shared< TestPushUniform< UniformType::eMat2x4i > >( *this, p_occurences );

		case UniformType::eMat3x2i:
			return std::make_shared< TestPushUniform< UniformType::eMat3x2i > >( *this, p_occurences );

		case UniformType::eMat3x3i:
			return std::make_shared< TestPushUniform< UniformType::eMat3x3i > >( *this, p_occurences );

		case UniformType::eMat3x4i:
			return std::make_shared< TestPushUniform< UniformType::eMat3x4i > >( *this, p_occurences );

		case UniformType::eMat4x2i:
			return std::make_shared< TestPushUniform< UniformType::eMat4x2i > >( *this, p_occurences );

		case UniformType::eMat4x3i:
			return std::make_shared< TestPushUniform< UniformType::eMat4x3i > >( *this, p_occurences );

		case UniformType::eMat4x4i:
			return std::make_shared< TestPushUniform< UniformType::eMat4x4i > >( *this, p_occurences );

		case UniformType::eMat2x2ui:
			return std::make_shared< TestPushUniform< UniformType::eMat2x2ui > >( *this, p_occurences );

		case UniformType::eMat2x3ui:
			return std::make_shared< TestPushUniform< UniformType::eMat2x3ui > >( *this, p_occurences );

		case UniformType::eMat2x4ui:
			return std::make_shared< TestPushUniform< UniformType::eMat2x4ui > >( *this, p_occurences );

		case UniformType::eMat3x2ui:
			return std::make_shared< TestPushUniform< UniformType::eMat3x2ui > >( *this, p_occurences );

		case UniformType::eMat3x3ui:
			return std::make_shared< TestPushUniform< UniformType::eMat3x3ui > >( *this, p_occurences );

		case UniformType::eMat3x4ui:
			return std::make_shared< TestPushUniform< UniformType::eMat3x4ui > >( *this, p_occurences );

		case UniformType::eMat4x2ui:
			return std::make_shared< TestPushUniform< UniformType::eMat4x2ui > >( *this, p_occurences );

		case UniformType::eMat4x3ui:
			return std::make_shared< TestPushUniform< UniformType::eMat4x3ui > >( *this, p_occurences );

		case UniformType::eMat4x4ui:
			return std::make_shared< TestPushUniform< UniformType::eMat4x4ui > >( *this, p_occurences );

		case UniformType::eMat2x2f:
			return std::make_shared< TestPushUniform< UniformType::eMat2x2f > >( *this, p_occurences );

		case UniformType::eMat2x3f:
			return std::make_shared< TestPushUniform< UniformType::eMat2x3f > >( *this, p_occurences );

		case UniformType::eMat2x4f:
			return std::make_shared< TestPushUniform< UniformType::eMat2x4f > >( *this, p_occurences );

		case UniformType::eMat3x2f:
			return std::make_shared< TestPushUniform< UniformType::eMat3x2f > >( *this, p_occurences );

		case UniformType::eMat3x3f:
			return std::make_shared< TestPushUniform< UniformType::eMat3x3f > >( *this, p_occurences );

		case UniformType::eMat3x4f:
			return std::make_shared< TestPushUniform< UniformType::eMat3x4f > >( *this, p_occurences );

		case UniformType::eMat4x2f:
			return std::make_shared< TestPushUniform< UniformType::eMat4x2f > >( *this, p_occurences );

		case UniformType::eMat4x3f:
			return std::make_shared< TestPushUniform< UniformType::eMat4x3f > >( *this, p_occurences );

		case UniformType::eMat4x4f:
			return std::make_shared< TestPushUniform< UniformType::eMat4x4f > >( *this, p_occurences );

		case UniformType::eMat2x2d:
			return std::make_shared< TestPushUniform< UniformType::eMat2x2d > >( *this, p_occurences );

		case UniformType::eMat2x3d:
			return std::make_shared< TestPushUniform< UniformType::eMat2x3d > >( *this, p_occurences );

		case UniformType::eMat2x4d:
			return std::make_shared< TestPushUniform< UniformType::eMat2x4d > >( *this, p_occurences );

		case UniformType::eMat3x2d:
			return std::make_shared< TestPushUniform< UniformType::eMat3x2d > >( *this, p_occurences );

		case UniformType::eMat3x3d:
			return std::make_shared< TestPushUniform< UniformType::eMat3x3d > >( *this, p_occurences );

		case UniformType::eMat3x4d:
			return std::make_shared< TestPushUniform< UniformType::eMat3x4d > >( *this, p_occurences );

		case UniformType::eMat4x2d:
			return std::make_shared< TestPushUniform< UniformType::eMat4x2d > >( *this, p_occurences );

		case UniformType::eMat4x3d:
			return std::make_shared< TestPushUniform< UniformType::eMat4x3d > >( *this, p_occurences );

		case UniformType::eMat4x4d:
			return std::make_shared< TestPushUniform< UniformType::eMat4x4d > >( *this, p_occurences );

		default:
			FAILURE( cuT( "Unsupported frame variable type" ) );
			return nullptr;
		}
	}
}
