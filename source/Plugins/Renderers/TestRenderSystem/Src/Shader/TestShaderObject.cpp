#include "Shader/TestShaderObject.hpp"

#include "Render/TestRenderSystem.hpp"
#include "Shader/TestShaderProgram.hpp"
#include "Shader/TestPushUniform.hpp"

#include <Log/Logger.hpp>

using namespace castor3d;
using namespace castor;

namespace TestRender
{
	TestShaderObject::TestShaderObject( TestShaderProgram * p_parent, ShaderType p_type )
		: ShaderObject( *p_parent, p_type )
	{
	}

	TestShaderObject::~TestShaderObject()
	{
	}

	bool TestShaderObject::create()
	{
		return true;
	}

	void TestShaderObject::destroy()
	{
		detach();
	}

	bool TestShaderObject::compile()
	{
		return true;
	}

	void TestShaderObject::detach()
	{
	}

	void TestShaderObject::attachTo( ShaderProgram & p_program )
	{
		detach();
	}

	String TestShaderObject::doRetrieveCompilerLog()
	{
		return String{};
	}


	std::shared_ptr< PushUniform > TestShaderObject::doCreateUniform( UniformType p_type, int p_occurences )
	{
		auto & program = static_cast< TestShaderProgram & >( m_parent );

		switch ( p_type )
		{
		case UniformType::eBool:
			return std::make_shared< TestPushUniform< UniformType::eBool > >( program, p_occurences );

		case UniformType::eInt:
			return std::make_shared< TestPushUniform< UniformType::eInt > >( program, p_occurences );

		case UniformType::eUInt:
			return std::make_shared< TestPushUniform< UniformType::eUInt > >( program, p_occurences );

		case UniformType::eFloat:
			return std::make_shared< TestPushUniform< UniformType::eFloat > >( program, p_occurences );

		case UniformType::eDouble:
			return std::make_shared< TestPushUniform< UniformType::eDouble > >( program, p_occurences );

		case UniformType::eSampler:
			return std::make_shared< TestPushUniform< UniformType::eSampler > >( program, p_occurences );

		case UniformType::eVec2b:
			return std::make_shared< TestPushUniform< UniformType::eVec2b > >( program, p_occurences );

		case UniformType::eVec3b:
			return std::make_shared< TestPushUniform< UniformType::eVec3b > >( program, p_occurences );

		case UniformType::eVec4b:
			return std::make_shared< TestPushUniform< UniformType::eVec4b > >( program, p_occurences );

		case UniformType::eVec2i:
			return std::make_shared< TestPushUniform< UniformType::eVec2i > >( program, p_occurences );

		case UniformType::eVec3i:
			return std::make_shared< TestPushUniform< UniformType::eVec3i > >( program, p_occurences );

		case UniformType::eVec4i:
			return std::make_shared< TestPushUniform< UniformType::eVec4i > >( program, p_occurences );

		case UniformType::eVec2ui:
			return std::make_shared< TestPushUniform< UniformType::eVec2ui > >( program, p_occurences );

		case UniformType::eVec3ui:
			return std::make_shared< TestPushUniform< UniformType::eVec3ui > >( program, p_occurences );

		case UniformType::eVec4ui:
			return std::make_shared< TestPushUniform< UniformType::eVec4ui > >( program, p_occurences );

		case UniformType::eVec2f:
			return std::make_shared< TestPushUniform< UniformType::eVec2f > >( program, p_occurences );

		case UniformType::eVec3f:
			return std::make_shared< TestPushUniform< UniformType::eVec3f > >( program, p_occurences );

		case UniformType::eVec4f:
			return std::make_shared< TestPushUniform< UniformType::eVec4f > >( program, p_occurences );

		case UniformType::eVec2d:
			return std::make_shared< TestPushUniform< UniformType::eVec2d > >( program, p_occurences );

		case UniformType::eVec3d:
			return std::make_shared< TestPushUniform< UniformType::eVec3d > >( program, p_occurences );

		case UniformType::eVec4d:
			return std::make_shared< TestPushUniform< UniformType::eVec4d > >( program, p_occurences );

		case UniformType::eMat2x2b:
			return std::make_shared< TestPushUniform< UniformType::eMat2x2b > >( program, p_occurences );

		case UniformType::eMat2x3b:
			return std::make_shared< TestPushUniform< UniformType::eMat2x3b > >( program, p_occurences );

		case UniformType::eMat2x4b:
			return std::make_shared< TestPushUniform< UniformType::eMat2x4b > >( program, p_occurences );

		case UniformType::eMat3x2b:
			return std::make_shared< TestPushUniform< UniformType::eMat3x2b > >( program, p_occurences );

		case UniformType::eMat3x3b:
			return std::make_shared< TestPushUniform< UniformType::eMat3x3b > >( program, p_occurences );

		case UniformType::eMat3x4b:
			return std::make_shared< TestPushUniform< UniformType::eMat3x4b > >( program, p_occurences );

		case UniformType::eMat4x2b:
			return std::make_shared< TestPushUniform< UniformType::eMat4x2b > >( program, p_occurences );

		case UniformType::eMat4x3b:
			return std::make_shared< TestPushUniform< UniformType::eMat4x3b > >( program, p_occurences );

		case UniformType::eMat4x4b:
			return std::make_shared< TestPushUniform< UniformType::eMat4x4b > >( program, p_occurences );

		case UniformType::eMat2x2i:
			return std::make_shared< TestPushUniform< UniformType::eMat2x2i > >( program, p_occurences );

		case UniformType::eMat2x3i:
			return std::make_shared< TestPushUniform< UniformType::eMat2x3i > >( program, p_occurences );

		case UniformType::eMat2x4i:
			return std::make_shared< TestPushUniform< UniformType::eMat2x4i > >( program, p_occurences );

		case UniformType::eMat3x2i:
			return std::make_shared< TestPushUniform< UniformType::eMat3x2i > >( program, p_occurences );

		case UniformType::eMat3x3i:
			return std::make_shared< TestPushUniform< UniformType::eMat3x3i > >( program, p_occurences );

		case UniformType::eMat3x4i:
			return std::make_shared< TestPushUniform< UniformType::eMat3x4i > >( program, p_occurences );

		case UniformType::eMat4x2i:
			return std::make_shared< TestPushUniform< UniformType::eMat4x2i > >( program, p_occurences );

		case UniformType::eMat4x3i:
			return std::make_shared< TestPushUniform< UniformType::eMat4x3i > >( program, p_occurences );

		case UniformType::eMat4x4i:
			return std::make_shared< TestPushUniform< UniformType::eMat4x4i > >( program, p_occurences );

		case UniformType::eMat2x2ui:
			return std::make_shared< TestPushUniform< UniformType::eMat2x2ui > >( program, p_occurences );

		case UniformType::eMat2x3ui:
			return std::make_shared< TestPushUniform< UniformType::eMat2x3ui > >( program, p_occurences );

		case UniformType::eMat2x4ui:
			return std::make_shared< TestPushUniform< UniformType::eMat2x4ui > >( program, p_occurences );

		case UniformType::eMat3x2ui:
			return std::make_shared< TestPushUniform< UniformType::eMat3x2ui > >( program, p_occurences );

		case UniformType::eMat3x3ui:
			return std::make_shared< TestPushUniform< UniformType::eMat3x3ui > >( program, p_occurences );

		case UniformType::eMat3x4ui:
			return std::make_shared< TestPushUniform< UniformType::eMat3x4ui > >( program, p_occurences );

		case UniformType::eMat4x2ui:
			return std::make_shared< TestPushUniform< UniformType::eMat4x2ui > >( program, p_occurences );

		case UniformType::eMat4x3ui:
			return std::make_shared< TestPushUniform< UniformType::eMat4x3ui > >( program, p_occurences );

		case UniformType::eMat4x4ui:
			return std::make_shared< TestPushUniform< UniformType::eMat4x4ui > >( program, p_occurences );

		case UniformType::eMat2x2f:
			return std::make_shared< TestPushUniform< UniformType::eMat2x2f > >( program, p_occurences );

		case UniformType::eMat2x3f:
			return std::make_shared< TestPushUniform< UniformType::eMat2x3f > >( program, p_occurences );

		case UniformType::eMat2x4f:
			return std::make_shared< TestPushUniform< UniformType::eMat2x4f > >( program, p_occurences );

		case UniformType::eMat3x2f:
			return std::make_shared< TestPushUniform< UniformType::eMat3x2f > >( program, p_occurences );

		case UniformType::eMat3x3f:
			return std::make_shared< TestPushUniform< UniformType::eMat3x3f > >( program, p_occurences );

		case UniformType::eMat3x4f:
			return std::make_shared< TestPushUniform< UniformType::eMat3x4f > >( program, p_occurences );

		case UniformType::eMat4x2f:
			return std::make_shared< TestPushUniform< UniformType::eMat4x2f > >( program, p_occurences );

		case UniformType::eMat4x3f:
			return std::make_shared< TestPushUniform< UniformType::eMat4x3f > >( program, p_occurences );

		case UniformType::eMat4x4f:
			return std::make_shared< TestPushUniform< UniformType::eMat4x4f > >( program, p_occurences );

		case UniformType::eMat2x2d:
			return std::make_shared< TestPushUniform< UniformType::eMat2x2d > >( program, p_occurences );

		case UniformType::eMat2x3d:
			return std::make_shared< TestPushUniform< UniformType::eMat2x3d > >( program, p_occurences );

		case UniformType::eMat2x4d:
			return std::make_shared< TestPushUniform< UniformType::eMat2x4d > >( program, p_occurences );

		case UniformType::eMat3x2d:
			return std::make_shared< TestPushUniform< UniformType::eMat3x2d > >( program, p_occurences );

		case UniformType::eMat3x3d:
			return std::make_shared< TestPushUniform< UniformType::eMat3x3d > >( program, p_occurences );

		case UniformType::eMat3x4d:
			return std::make_shared< TestPushUniform< UniformType::eMat3x4d > >( program, p_occurences );

		case UniformType::eMat4x2d:
			return std::make_shared< TestPushUniform< UniformType::eMat4x2d > >( program, p_occurences );

		case UniformType::eMat4x3d:
			return std::make_shared< TestPushUniform< UniformType::eMat4x3d > >( program, p_occurences );

		case UniformType::eMat4x4d:
			return std::make_shared< TestPushUniform< UniformType::eMat4x4d > >( program, p_occurences );

		default:
			FAILURE( cuT( "Unsupported frame variable type" ) );
			return nullptr;
		}
	}
}
