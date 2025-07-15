/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DebugConfig_HPP___
#define ___C3D_DebugConfig_HPP___

#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"

namespace c3d
{
	struct DebugConfig
	{
		uint32_t intermediateShaderValueIndex{ 0u };

		DebugConfig()
		{
			registerValue( cuT( "Default" ), cuT( "Result" ) );
		}

		uint32_t registerValue( String const & category
			, String const & name )
		{
			auto fullName = category + cuT( "/" ) + name;
			auto it = std::find( m_intermediateValueNames.begin()
				, m_intermediateValueNames.end()
				, fullName );

			if ( it == m_intermediateValueNames.end() )
			{
				m_intermediateValueNames.emplace_back( fullName );
				it = std::next( m_intermediateValueNames.begin(), ptrdiff_t( m_intermediateValueNames.size() - 1u ) );
			}

			return uint32_t( std::distance( m_intermediateValueNames.begin(), it ) );
		}

		StringArray const & getIntermediateValues()const noexcept
		{
			return m_intermediateValueNames;
		}

	private:
		StringArray m_intermediateValueNames;
	};
}

#endif
