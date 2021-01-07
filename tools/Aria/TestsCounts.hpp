/*
See LICENSE file in root folder
*/
#ifndef ___CTP_TestsCounts_HPP___
#define ___CTP_TestsCounts_HPP___

#include "CountedValue.hpp"

#include <array>

namespace aria
{
	struct TestsCounts
	{
	private:
		// Renderer
		TestsCounts( TestsCounts & all
			, Config const & config
			, TestMap const & tests
			, TestRunCategoryMap const & runs );
		// Category
		TestsCounts( TestsCounts & renderer
			, Config const & config
			, TestArray const & tests
			, TestRunArray const & runs );

	public:
		// All
		TestsCounts( Config const & config
			, TestMap const & tests
			, TestRunMap const & runs );
		// Renderer
		TestsCountsPtr addChild( TestMap const & tests
			, TestRunCategoryMap const & runs );
		// Category
		TestsCountsPtr addChild( TestArray const & tests
			, TestRunArray const & runs );
		// Test
		void addTest( DatabaseTest & test );
;
		void add( TestStatus status );
		void remove( TestStatus status );
		CountedUInt & getCount( TestsCountsType type );
		CountedUInt const & getCount( TestsCountsType type )const;
		uint32_t getValue( TestsCountsType type )const;
		uint32_t getStatusValue( TestStatus status )const;
		uint32_t getIgnoredValue()const;
		uint32_t getOutdatedValue()const;
		uint32_t getAllValue()const;
		uint32_t getAllRunStatus()const;

		void addIgnored()
		{
			assert( m_children.empty()
				&& "Only Category test counts can unit count tests" );
			++getCount( TestsCountsType::eIgnored );
		}

		void removeIgnored()
		{
			assert( m_children.empty()
				&& "Only Category test counts can unit count tests" );
			--getCount( TestsCountsType::eIgnored );
		}

		void addOutdated()
		{
			assert( m_children.empty()
				&& "Only Category test counts can unit count tests" );
			++getCount( TestsCountsType::eOutdated );
		}

		void removeOutdated()
		{
			assert( m_children.empty()
				&& "Only Category test counts can unit count tests" );
			--getCount( TestsCountsType::eOutdated );
		}

		uint32_t getNotRunValue()const
		{
			return getValue( TestsCountsType::eNotRun );
		}

		float getPercent( TestsCountsType type )const
		{
			return ( 100.0f * getValue( type ) ) / getAllValue();
		}

		float getIgnoredPercent()const
		{
			return ( 100.0f * getIgnoredValue() ) / getAllValue();
		}

		float getOutdatedPercent()const
		{
			return ( 100.0f * getOutdatedValue() ) / getAllValue();
		}

		float getAllPercent()const
		{
			return ( 100.0f * getAllValue() ) / getAllValue();
		}

		float getStatusPercent( TestStatus status )const
		{
			return ( 100.0f * getStatusValue( status ) ) / getAllValue();
		}

		uint32_t getNotRunPercent()const
		{
			return ( 100.0f * getNotRunValue() ) / getAllValue();
		}

	private:
		std::array< CountedUInt, TestsCountsType::eCount > m_values{};
		std::array< CountedUIntConnection, TestsCountsType::eCount > m_connections{};
		Config const & m_config;
		TestMap const * m_allTests{};
		TestRunMap const * m_allRuns{};
		TestMap const * m_rendererTests{};
		TestRunCategoryMap const * m_rendererRuns{};
		TestArray const * m_categoryTests{};
		TestRunArray const * m_categoryRuns{};
		TestsCounts * m_parent{};
		std::vector< TestsCounts * > m_children;
	};
}

#endif
