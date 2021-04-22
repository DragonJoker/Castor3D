/*
See LICENSE file in root folder
*/
#ifndef ___CTP_DatabaseTest_HPP___
#define ___CTP_DatabaseTest_HPP___

#include "Prerequisites.hpp"
#include "Database/DbPrerequisites.hpp"

namespace aria
{
	class DatabaseTest
	{
		friend class TestDatabase;
		friend struct CategoryTestsCounts;

	public:
		DatabaseTest( TestDatabase & database
			, TestRun test );

		void updateIgnoreResult( bool ignore
			, db::DateTime castorDate
			, bool useAsReference );
		void updateCastorDate( db::DateTime const & castorDate );
		void updateCastorDate();
		void updateSceneDate( db::DateTime const & castorDate );
		void updateSceneDate();
		void updateStatusNW( TestStatus newStatus );
		void updateStatus( TestStatus newStatus
			, bool useAsReference );
		void createNewRun( castor::Path const & match );

		bool checkOutOfCastorDate()const
		{
			updateOutOfDate();
			return m_outOfCastorDate;
		}

		bool checkOutOfSceneDate()const
		{
			updateOutOfDate();
			return m_outOfSceneDate;
		}

		int getTestId()const
		{
			return m_test.test->id;
		}

		int getRunId()const
		{
			return m_test.id;
		}

		Renderer getRenderer()const
		{
			return m_test.renderer;
		}

		Category getCategory()const
		{
			return m_test.test->category;
		}

		bool getIgnoreResult()const
		{
			return m_test.test->ignoreResult;
		}

		TestStatus getStatus()const
		{
			return m_test.status;
		}

		std::string const & getName()const
		{
			return m_test.test->name;
		}

		db::DateTime const & getRunDate()const
		{
			return m_test.runDate;
		}

		db::DateTime const & getCastorDate()const
		{
			return m_test.castorDate;
		}

		db::DateTime const & getSceneDate()const
		{
			return m_test.sceneDate;
		}

		CategoryTestsCounts const & getCounts()const
		{
			return *m_counts;
		}

		TestRun const * operator->()const
		{
			return &m_test;
		}

		TestRun const & operator*()const
		{
			return m_test;
		}

	private:
		void update( int id );
		void update( int id
			, db::DateTime runDate
			, TestStatus status
			, db::DateTime castorDate
			, db::DateTime sceneDate );
		void updateReference( TestStatus status );
		void updateOutOfDate( bool remove = true )const;

	private:
		TestDatabase & m_database;
		CategoryTestsCounts * m_counts{};
		TestRun m_test;
		mutable bool m_outOfCastorDate;
		mutable bool m_outOfSceneDate;
		mutable bool m_outOfDate;
	};
}

#endif
