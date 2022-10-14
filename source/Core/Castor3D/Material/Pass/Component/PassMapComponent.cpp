#include "Castor3D/Material/Pass/Component/PassMapComponent.hpp"

namespace castor3d
{
	//*********************************************************************************************

	bool PassMapComponentPlugin::writeTextureConfig( TextureConfiguration const & configuration
		, castor::String const & tabs
		, castor::StringStream & file )const
	{
		bool result = true;
		auto it = checkFlag( configuration.components, getTextureFlags() );

		if ( it != configuration.components.end() )
		{
			result = doWriteTextureConfig( configuration, it->componentsMask, tabs, file );
		}

		return result;
	}

	//*********************************************************************************************

	PassMapComponent::PassMapComponent( Pass & pass
		, castor::String const & type )
		: PassComponent{ pass, type }
	{
	}

	void PassMapComponent::fillConfig( TextureConfiguration & configuration
		, PassVisitorBase & vis )const
	{
		if ( hasAny( configuration.components, getTextureFlags() ) )
		{
			doFillConfig( configuration, vis );
		}
	}

	//*********************************************************************************************
}
