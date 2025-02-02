/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LpvConfig_H___
#define ___C3D_LpvConfig_H___

#include "LightPropagationVolumesModule.hpp"

#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"

#include <CastorUtils/Design/ChangeTracked.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>

namespace castor3d
{
	struct LpvConfig
	{
		C3D_API void accept( ConfigurationVisitorBase & visitor );

		C3D_API static void addParsers( castor::AttributeParsers & result
			, CSCNSection shadows, CSCNSection lightLpv
			, castor::RawParserFunctionT< ShadowContext > parserConfig );

		castor::ChangeTracked< float > indirectAttenuation;
		castor::ChangeTracked< float > texelAreaModifier;
	};

	inline bool operator==( LpvConfig const & lhs, LpvConfig const & rhs )noexcept
	{
		return lhs.indirectAttenuation == rhs.indirectAttenuation
			&& lhs.texelAreaModifier == rhs.texelAreaModifier;
	}
}

#endif
