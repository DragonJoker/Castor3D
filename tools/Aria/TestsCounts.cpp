#include "Aria/TestsCounts.hpp"

#include "Aria/DatabaseTest.hpp"

namespace aria
{
	//*********************************************************************************************

	TestsCounts::TestsCounts( TestsCounts & all
		, Config const & config
		, TestMap const & tests
		, TestRunCategoryMap const & runs )
		: m_config{ config }
		, m_rendererTests{ &tests }
		, m_rendererRuns{ &runs }
		, m_parent{ &all }
	{
#if CTP_UseCountedValue

		for ( uint32_t i = 0; i < TestsCounts::Type::eAll; ++i )
		{
			m_connections[i] = m_values[i].onValueChange.connect( [i, this]( CountedUInt const & v )
				{
					m_parent->m_values[i].onValueChange( v );
				} );
		}

#endif
	}

	TestsCounts::TestsCounts( TestsCounts & renderer
		, Config const & config
		, TestArray const & tests
		, TestRunArray const & runs )
		: m_config{ config }
		, m_categoryTests{ &tests }
		, m_categoryRuns{ &runs }
		, m_parent{ &renderer }
	{
#if CTP_UseCountedValue

		for ( uint32_t i = 0; i < TestsCounts::Type::eAll; ++i )
		{
			m_connections[i] = m_values[i].onValueChange.connect( [i, this]( CountedUInt const & v )
				{
					m_parent->m_values[i].onValueChange( v );
				} );
		}

#endif
	}

	TestsCounts::TestsCounts( Config const & config
		, TestMap const & tests
		, TestRunMap const & runs )
		: m_config{ config }
		, m_allTests{ &tests }
		, m_allRuns{ &runs }
	{
	}

	TestsCountsPtr TestsCounts::addChild( TestMap const & tests
		, TestRunCategoryMap const & runs )
	{
		TestsCountsPtr result{ new TestsCounts{ *this
			, m_config
			, tests
			, runs } };
		m_children.push_back( result.get() );
		return result;
	}

	TestsCountsPtr TestsCounts::addChild( TestArray const & tests
		, TestRunArray const & runs )
	{
		TestsCountsPtr result{ new TestsCounts{ *this
			, m_config
			, tests
			, runs } };
		m_children.push_back( result.get() );
		return result;
	}

	void TestsCounts::addTest( DatabaseTest & test )
	{
		test.m_counts = this;
		add( test.getStatus() );

		if ( test.getIgnoreResult() )
		{
			addIgnored();
		}

		if ( isOutOfDate( m_config, *test ) )
		{
			addOutdated();
		}
	}

	void TestsCounts::add( TestStatus status )
	{
		assert( m_children.empty()
			&& "Only Category test counts can unit count tests" );
		++getCount( Type::eAll );
		++getCount( getType( status ) );
	}

	void TestsCounts::remove( TestStatus status )
	{
		assert( m_children.empty()
			&& "Only Category test counts can unit count tests" );
		--getCount( getType( status ) );
		--getCount( Type::eAll );
	}

	CountedUInt & TestsCounts::getCount( Type type )
	{
		return m_values[type];
	}

	CountedUInt const & TestsCounts::getCount( Type type )const
	{
		return m_values[type];
	}

	uint32_t TestsCounts::getValue( Type type )const
	{
		if ( m_children.empty() )
		{
			return uint32_t( getCount( type ) );
		}

		uint32_t result{};

		for ( auto & child : m_children )
		{
			result += child->getValue( type );
		}

		return result;
	}

	uint32_t TestsCounts::getStatusValue( TestStatus status )const
	{
		return getValue( getType( status ) );
	}

	uint32_t TestsCounts::getIgnoredValue()const
	{
		return getValue( Type::eIgnored );
	}

	uint32_t TestsCounts::getOutdatedValue()const
	{
		return getValue( Type::eOutdated );
	}

	uint32_t TestsCounts::getAllValue()const
	{
		return getValue( Type::eAll );
	}

	uint32_t TestsCounts::getAllRunStatus()const
	{
		uint32_t result{};

		for ( auto i = 1u; i < Type::eCountedInAllEnd; ++i )
		{
			result += getValue( Type( i ) );
		}

		return result;
	}

	//*********************************************************************************************
}
