/*
See LICENSE file in root folder
*/
#ifndef ___MotionBlur_Parsers_H___
#define ___MotionBlur_Parsers_H___

#include <CastorUtils/FileParser/FileParserModule.hpp>

namespace motion_blur
{
	struct Configuration
	{
		uint32_t samplesCount{ 4u };
		float vectorDivider = 1.0f;
		float blurScale = 1.0f;
	};

	castor::AttributeParsers createParsers();
	castor::StrUInt32Map createSections();
}

#endif
