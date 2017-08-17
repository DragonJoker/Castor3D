#include "PushUniform.hpp"

using namespace castor;

namespace castor3d
{
	PushUniform::PushUniform( ShaderProgram & p_program )
		: m_program{ p_program }
	{
	}

	PushUniform::~PushUniform()
	{
	}

	bool PushUniform::initialise()
	{
		return doInitialise();
	}

	void PushUniform::update()
	{
		if ( getBaseUniform().isChanged() )
		{
			doUpdate();
			getBaseUniform().setChanged( false );
		}
	}
}
