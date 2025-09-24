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

namespace c3d
{
	struct LpvConfig
	{
		C3D_API void accept( ConfigurationVisitorBase & visitor );

		C3D_API static void addParsers( AttributeParsers & result
			, CSCNSection shadows, CSCNSection lightLpv
			, RawParserFunctionT< ShadowContext > parserConfig );

		ChangeTracked< float > indirectAttenuation;
		ChangeTracked< float > texelAreaModifier;

	private:
		friend bool operator==( LpvConfig const & lhs, LpvConfig const & rhs )noexcept
		{
			return lhs.indirectAttenuation == rhs.indirectAttenuation
				&& lhs.texelAreaModifier == rhs.texelAreaModifier;
		}
	};
}

#endif
