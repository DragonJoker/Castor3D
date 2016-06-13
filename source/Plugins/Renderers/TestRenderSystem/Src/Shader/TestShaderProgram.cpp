#include "Shader/TestShaderProgram.hpp"

#include "Render/TestRenderSystem.hpp"
#include "Shader/TestFrameVariableBuffer.hpp"
#include "Shader/TestOneFrameVariable.hpp"
#include "Shader/TestShaderObject.hpp"

#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace TestRender
{
	TestShaderProgram::TestShaderProgram( TestRenderSystem & p_renderSystem )
		: ShaderProgram( p_renderSystem )
		, m_layout( p_renderSystem )
	{
		CreateObject( eSHADER_TYPE_VERTEX );
		CreateObject( eSHADER_TYPE_PIXEL );
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

	ShaderObjectSPtr TestShaderProgram::DoCreateObject( eSHADER_TYPE p_type )
	{
		return std::make_shared< TestShaderObject >( this, p_type );
	}

	std::shared_ptr< OneIntFrameVariable > TestShaderProgram::DoCreateTextureVariable( int p_occurences )
	{
		return std::make_shared< TestOneFrameVariable< int > >( p_occurences, this );
	}

	String TestShaderProgram::DoRetrieveLinkerLog()
	{
		return String{};
	}
}
