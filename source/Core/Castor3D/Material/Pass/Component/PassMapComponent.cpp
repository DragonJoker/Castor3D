#include "Castor3D/Material/Pass/Component/PassMapComponent.hpp"

CU_ImplementSmartPtr( castor3d, PassMapComponent )

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
		, castor::String type
		, TextureFlags textureFlags
		, castor::StringArray deps )
		: PassComponent{ pass, std::move( type ), std::move( deps ) }
		, m_textureFlags{ makeTextureFlag( getId(), textureFlags ) }
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
