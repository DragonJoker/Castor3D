/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#include "GlRendererPrerequisites.hpp"

#include <Buffer/PushConstantsBuffer.hpp>

#include <algorithm>

namespace renderer
{
	renderer::PushConstantArray convert( renderer::ShaderStageFlag stage
		, renderer::SpecialisationMapEntryArray::const_iterator const & begin
		, renderer::SpecialisationMapEntryArray::const_iterator const & end )
	{
		renderer::PushConstantArray result;

		for ( auto it = begin; it != end; ++it )
		{
			result.push_back( {
				it->constantID,
				it->offset,
				it->format,
				it->arraySize
			} );
		}

		return result;
	}

	renderer::PushConstantsBufferBase convert( renderer::ShaderStageFlag stage
		, renderer::SpecialisationInfoBase const & specialisationInfo )
	{
		renderer::PushConstantsBufferBase result{ stage
			, convert( stage, specialisationInfo.begin(), specialisationInfo.end() ) };
		assert( result.getSize() == specialisationInfo.getSize() );
		std::memcpy( result.getData()
			, specialisationInfo.getData()
			, specialisationInfo.getSize() );
		return result;
	}
}
