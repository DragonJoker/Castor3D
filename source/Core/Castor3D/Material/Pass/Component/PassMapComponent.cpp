#include "Castor3D/Material/Pass/Component/PassMapComponent.hpp"

namespace castor3d
{
	PassMapComponent::PassMapComponent( Pass & pass
		, castor::String const & type )
		: PassComponent{ pass, type }
	{
	}
}
