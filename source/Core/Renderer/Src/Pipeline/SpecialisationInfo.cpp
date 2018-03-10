/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#include "SpecialisationInfo.hpp"

#include <algorithm>

namespace renderer
{
	namespace
	{
		uint32_t deduceSize( SpecialisationMapEntryArray const & entries )
		{
			auto offset = 0u;

			for ( auto & constant : entries )
			{
				if ( offset > constant.offset )
				{
					throw std::runtime_error{ "Inconsistency detected between given and computed offsets" };
				}

				offset = constant.offset + getSize( constant.format ) * constant.arraySize;
			}

			return offset;
		}
	}

	SpecialisationInfoBase::SpecialisationInfoBase( SpecialisationMapEntryArray const & entries )
		: m_entries{ entries }
		, m_data( deduceSize( m_entries ) )
	{
		assert( ( m_data.size() % 4 ) == 0 );
	}
}
