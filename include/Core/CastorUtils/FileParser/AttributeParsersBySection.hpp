/*
See LICENSE file in root folder
*/
#ifndef ___CU_AttributeParsersBySection_H___
#define ___CU_AttributeParsersBySection_H___

#include "CastorUtils/FileParser/FileParserModule.hpp"

namespace castor
{
#if defined( CU_CompilerMSVC )

	class AttributeParserMap
	{
	private:
		using Map = StringMap< ParserFunctionAndParams >;
		Map m_map;

	public:
		using iterator = Map::iterator;
		using const_iterator = Map::const_iterator;

		ParserFunctionAndParams & operator []( String const & name )
		{
			auto [it, res] = m_map.try_emplace( name );
			return it->second;
		}

		iterator find( String const & name )
		{
			return m_map.find( name );
		}

		const_iterator find( String const & name )const
		{
			return m_map.find( name );
		}

		iterator begin()
		{
			return m_map.begin();
		}

		const_iterator begin()const
		{
			return m_map.begin();
		}

		iterator end()
		{
			return m_map.end();
		}

		const_iterator end()const
		{
			return m_map.end();
		}
	};

#endif
}

#endif
