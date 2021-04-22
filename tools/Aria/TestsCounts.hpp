/*
See LICENSE file in root folder
*/
#ifndef ___CTP_TestsCounts_HPP___
#define ___CTP_TestsCounts_HPP___

#include "CountedValue.hpp"

#include <array>

namespace aria
{
	struct CategoryTestsCounts
	{
	public:
		CategoryTestsCounts( Config const & config
			, TestArray const & tests
			, TestRunArray const & runs );
		void addTest( DatabaseTest & test );

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
			++getCount( TestsCountsType::eIgnored );
		}

		void removeIgnored()
		{
			--getCount( TestsCountsType::eIgnored );
		}

		void addOutdated()
		{
			++getCount( TestsCountsType::eOutdated );
		}

		void removeOutdated()
		{
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
		TestArray const * m_tests{};
		TestRunArray const * m_runs{};
	};

	struct RendererTestsCounts
	{
		RendererTestsCounts( Config const & config );

		CategoryTestsCounts & addCategory( Category category
			, TestArray const & tests
			, TestRunArray const & runs );
		CategoryTestsCounts & getCounts( Category category );
		uint32_t getValue( TestsCountsType type )const;
		uint32_t getAllValue()const;

		float getPercent( TestsCountsType type )const
		{
			return ( 100.0f * getValue( type ) ) / getAllValue();
		}

		TestsCountsCategoryMap & getCategories()
		{
			return categories;
		}

	private:
		Config const & config;
		TestsCountsCategoryMap categories;
	};

	struct AllTestsCounts
	{
		AllTestsCounts( Config const & config );

		RendererTestsCounts & addRenderer( Renderer renderer );
		RendererTestsCounts & getRenderer( Renderer renderer );

		CategoryTestsCounts & addCategory( Renderer renderer
			, Category category
			, TestArray const & tests
			, TestRunArray const & runs );

		uint32_t getValue( TestsCountsType type )const;
		uint32_t getAllValue()const;

		float getPercent( TestsCountsType type )const
		{
			return ( 100.0f * getValue( type ) ) / getAllValue();
		}

	private:
		Config const & config;
		TestsCountsRendererMap renderers;
	};
}

#endif
