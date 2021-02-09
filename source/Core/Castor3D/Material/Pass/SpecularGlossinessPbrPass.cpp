#include "Castor3D/Material/Pass/SpecularGlossinessPbrPass.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Shader/PassBuffer/PassBuffer.hpp"

using namespace castor;

namespace castor3d
{
	SpecularGlossinessPbrPass::SpecularGlossinessPbrPass( Material & parent )
		: Pass{ parent }
		, m_diffuse{ RgbColour::fromRGBA( 0xFFFFFFFF ) }
		, m_specular{ RgbColour::fromRGBA( 0xFFFFFFFF ) }
	{
	}

	SpecularGlossinessPbrPass::~SpecularGlossinessPbrPass()
	{
	}

	void SpecularGlossinessPbrPass::accept( PassBuffer & buffer )const
	{
		buffer.visit( *this );
	}

	void SpecularGlossinessPbrPass::doInitialise()
	{
	}

	void SpecularGlossinessPbrPass::doCleanup()
	{
	}

	void SpecularGlossinessPbrPass::doSetOpacity( float value )
	{
	}
}
