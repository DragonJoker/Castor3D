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
		Register( eTONE_MAPPING_TYPE_HAARM_PETER_DUIKER, &HaarmPeterDuikerToneMapping::Create );
		Register( eTONE_MAPPING_TYPE_HEJL_BURGESS_DAWSON, &HejlBurgessDawsonToneMapping::Create );
		Register( eTONE_MAPPING_TYPE_LINEAR, &LinearToneMapping::Create );
		Register( eTONE_MAPPING_TYPE_REINHARD, &ReinhardToneMapping::Create );
	}
}
