/*
See LICENSE file in root folder
*/
#ifndef ___C3DSMAA_SMAA_H___
#define ___C3DSMAA_SMAA_H___

#include <Castor3DPrerequisites.hpp>

#include "SmaaConfig.hpp"

namespace smaa
{
	castor::String const & getSmaaShader();
	void writeConstants( glsl::GlslWriter & writer
		, SmaaConfig const & config
		, castor::Point4f const & renderTargetMetrics
		, bool vertexShader );
}

#endif
