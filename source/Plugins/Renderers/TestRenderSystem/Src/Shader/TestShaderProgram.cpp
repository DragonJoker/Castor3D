#include "Shader/TestShaderProgram.hpp"

#include "Render/TestRenderSystem.hpp"
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
}
