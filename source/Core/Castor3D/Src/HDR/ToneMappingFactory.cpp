#include "ToneMappingFactory.hpp"

#include "HaarmPeterDuikerToneMapping.hpp"
#include "HejlBurgessDawsonToneMapping.hpp"
#include "LinearToneMapping.hpp"
#include "ReinhardToneMapping.hpp"

using namespace Castor;

namespace Castor3D
{
	void ToneMappingFactory::Initialise()
	{
		Register( HaarmPeterDuikerToneMapping::Name, &HaarmPeterDuikerToneMapping::Create );
		Register( HejlBurgessDawsonToneMapping::Name, &HejlBurgessDawsonToneMapping::Create );
		Register( LinearToneMapping::Name, &LinearToneMapping::Create );
		Register( ReinhardToneMapping::Name, &ReinhardToneMapping::Create );
	}
}
