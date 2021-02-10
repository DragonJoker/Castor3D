/*
See LICENSE file in root folder
*/
#ifndef ___C3D_HdrConfig_H___
#define ___C3D_HdrConfig_H___

#include "ToneMappingModule.hpp"

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor3d
{
	class HdrConfig
	{
	public:
		//!\~english	The exposure value.
		//!\~french		La valeur d'exposition.
		float exposure{ 1.0f };
		//!\~english	The gamma correction value.
		//!\~french		La valeur de correction gamma.
		float gamma{ 2.2f };
	};
}

#endif
