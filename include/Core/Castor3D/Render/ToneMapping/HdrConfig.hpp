/*
See LICENSE file in root folder
*/
#ifndef ___C3D_HdrConfig_H___
#define ___C3D_HdrConfig_H___

#include "ToneMappingModule.hpp"

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor3d
{
	struct HdrConfig
	{
		C3D_API void accept( ConfigurationVisitorBase & visitor );

		C3D_API static void addParsers( castor::AttributeParsers & result );

		//!\~english	The exposure value.
		//!\~french		La valeur d'exposition.
		float exposure{ 1.0f };
		//!\~english	The gamma correction value.
		//!\~french		La valeur de correction gamma.
		float gamma{ 2.2f };
	};

	inline bool operator==( HdrConfig const & lhs, HdrConfig const & rhs )noexcept
	{
		return lhs.exposure == rhs.exposure
			&& lhs.gamma == rhs.gamma;
	}
}

#endif
