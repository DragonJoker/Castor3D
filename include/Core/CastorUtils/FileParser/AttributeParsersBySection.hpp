/*
See LICENSE file in root folder
*/
#ifndef ___CU_AttributeParsersBySection_H___
#define ___CU_AttributeParsersBySection_H___

#include "CastorUtils/FileParser/FileParserModule.hpp"

namespace castor
{
	struct ParserFunctionAndParams
	{
		ParserFunction function;
		ParserParameterArray params;
	};

#if defined( CU_CompilerMSVC )

	class AttributeParserMap
	{
	private:
		CU_DeclareMap( String, ParserFunctionAndParams, );
		Map m_map;

	public:
		typedef MapIt iterator;
		typedef MapConstIt const_iterator;

		ParserFunctionAndParams & operator []( String const & name )
		{
			auto ires = m_map.emplace( name, ParserFunctionAndParams{} );
			return ires.first->second;
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
