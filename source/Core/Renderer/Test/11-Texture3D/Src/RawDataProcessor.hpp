#pragma once

#include "Prerequisites.hpp"

namespace vkapp
{
	void readFile( std::string const & name
		, renderer::UIVec3 const & size
		, renderer::ByteArray & data );
}
