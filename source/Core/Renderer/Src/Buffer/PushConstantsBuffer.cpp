/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "PushConstantsBuffer.hpp"

#include <algorithm>

namespace renderer
{
	namespace
	{
		uint32_t deduceBaseOffset( PushConstantArray & variables )
		{
			std::sort( variables.begin()
				, variables.end()
				, []( PushConstant const & lhs, PushConstant const & rhs )
			{
				return lhs.offset < rhs.offset;
			} );
			return variables[0].offset;
		}

		uint32_t deduceSize( PushConstantArray const & variables
			, uint32_t baseOffset )
		{
			auto offset = baseOffset;

			for ( auto & constant : variables )
			{
				if ( offset > constant.offset )
				{
					throw std::runtime_error{ "Inconsistency detected between given and computed offsets" };
				}

				offset = constant.offset + getSize( constant.format );
			}

			return offset - baseOffset;
		}
	}

	PushConstantsBuffer::PushConstantsBuffer( ShaderStageFlags stageFlags
		, PushConstantArray const & variables )
		: m_stageFlags{ stageFlags }
		, m_variables{ variables }
		, m_offset{ deduceBaseOffset( m_variables ) }
		, m_data( deduceSize( m_variables, m_offset ) )
	{
		assert( ( m_offset % 4 ) == 0 );
		assert( ( m_data.size() % 4 ) == 0 );
	}
}
