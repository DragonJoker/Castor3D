#include "Aria/DatabaseTest.hpp"

#include "Aria/TestDatabase.hpp"
#include "Aria/TestsCounts.hpp"
#include "Aria/Database/DbDateTimeHelpers.hpp"

#include <CastorUtils/Data/File.hpp>

namespace aria
{
	//*********************************************************************************************

	namespace
	{
		void moveResultFile( TestRun const & test
			, TestStatus oldStatus
			, TestStatus newStatus
			, castor::Path work )
		{
			if ( oldStatus == TestStatus::eNotRun
				|| newStatus == TestStatus::eNotRun
				|| oldStatus == newStatus )
			{
				return;
			}

			auto resultFolder = work / getResultFolder( *test.test );
			moveFile( resultFolder / getFolderName( oldStatus )
				, resultFolder / getFolderName( newStatus )
				, getResultName( test ) );
		}
	}

	//*********************************************************************************************

	DatabaseTest::DatabaseTest( TestDatabase & database
		, TestRun test )
		: m_database{ database }
		, m_test{ std::move( test ) }
		, m_outOfCastorDate{ isOutOfCastorDate( database.m_config, m_test ) }
		, m_outOfSceneDate{ isOutOfSceneDate( database.m_config, m_test ) }
		, m_outOfDate{ m_outOfCastorDate || m_outOfSceneDate }
	{
	}

	void DatabaseTest::updateCastorDate( db::DateTime const & castorDate )
	{
		if ( m_test.castorDate < castorDate )
		{
			m_test.castorDate = castorDate;
			m_database.updateRunCastorDate( m_test );
			updateOutOfDate();
		}
	}

	void DatabaseTest::updateCastorDate()
	{
		updateCastorRefDate( m_database.m_config );
		updateCastorDate( m_database.m_config.castorRefDate );
	}

	void DatabaseTest::updateSceneDate( db::DateTime const & sceneDate )
	{
		if ( m_test.sceneDate < sceneDate )
		{
			m_test.sceneDate = sceneDate;
			m_database.updateRunSceneDate( m_test );
			updateOutOfDate();
		}
	}

	void DatabaseTest::updateSceneDate()
	{
		auto sceneDate = aria::getSceneDate( m_database.m_config, m_test );
		updateSceneDate( sceneDate );
	}

	void DatabaseTest::updateStatusNW( TestStatus newStatus )
	{
		m_counts->remove( m_test.status );
		m_counts->add( newStatus );
		updateOutOfDate();
		m_test.status = newStatus;
	}

	void DatabaseTest::updateStatus( TestStatus newStatus
		, bool useAsReference )
	{
		auto & config = m_database.m_config;
		TestStatus oldStatus = m_test.status;
		updateCastorRefDate( config );
		m_test.castorDate = config.castorRefDate;
		updateStatusNW( newStatus );
		m_database.updateRunStatus( m_test );
		moveResultFile( m_test, oldStatus, newStatus, config.work );

		if ( useAsReference )
		{
			updateReference( newStatus );
		}
	}

	void DatabaseTest::createNewRun( TestStatus status
		, db::DateTime const & runDate )
	{
		auto & config = m_database.m_config;
		auto rawStatus = status;
		auto newStatus = rawStatus;

		if ( m_test.test->ignoreResult )
		{
			newStatus = TestStatus::eNegligible;
		}

		m_test.runDate = runDate;
		assert( db::date_time::isValid( m_test.runDate ) );
		updateCastorRefDate( config );
		m_test.castorDate = config.castorRefDate;
		assert( db::date_time::isValid( m_test.castorDate ) );
		m_test.sceneDate = aria::getSceneDate( config, m_test );
		assert( db::date_time::isValid( m_test.sceneDate ) );
		updateStatusNW( newStatus );
		m_database.insertRun( m_test );

		if ( m_test.test->ignoreResult )
		{
			updateReference( rawStatus );
		}
	}

	void DatabaseTest::createNewRun( castor::Path const & match )
	{
		auto path = match.getPath();
		createNewRun( aria::getStatus( path.getFileName() )
			, getFileDate( match ) );
	}

	void DatabaseTest::update( int id )
	{
		m_test.id = id;
	}

	void DatabaseTest::update( int id
		, db::DateTime runDate
		, TestStatus status
		, db::DateTime castorDate
		, db::DateTime sceneDate )
	{
		m_test.id = id;
		m_test.status = status;
		m_test.runDate = std::move( runDate );
		m_test.castorDate = std::move( castorDate );
		m_test.sceneDate = std::move( sceneDate );
		m_outOfCastorDate = isOutOfCastorDate( m_database.m_config, m_test );
		m_outOfSceneDate = isOutOfSceneDate( m_database.m_config, m_test );
		m_outOfDate = m_outOfCastorDate || m_outOfSceneDate;
	}

	void DatabaseTest::updateIgnoreResult( bool ignore
		, db::DateTime castorDate
		, bool useAsReference )
	{
		if ( m_test.test->ignoreResult != ignore )
		{
			if ( ignore )
			{
				m_counts->addIgnored();
			}
			else
			{
				m_counts->removeIgnored();
			}
		}

		m_test.test->ignoreResult = ignore;
		m_test.castorDate = std::move( castorDate );
		assert( db::date_time::isValid( m_test.castorDate ) );
		m_database.updateTestIgnoreResult( *m_test.test, ignore );

		if ( ignore )
		{
			updateStatus( TestStatus::eNegligible, useAsReference );
		}
	}

	void DatabaseTest::updateReference( TestStatus status )
	{
		auto & config = m_database.m_config;
		castor::File::copyFileName( config.work / getResultFolder( *m_test.test ) / getFolderName( status ) / getResultName( m_test )
			, config.test / getReferenceFolder( m_test ) / getReferenceName( m_test ) );
	}

	void DatabaseTest::updateOutOfDate( bool remove )const
	{
		bool outOfCastorDate{ isOutOfCastorDate( m_database.m_config, m_test ) };
		bool outOfSceneDate{ isOutOfSceneDate( m_database.m_config, m_test ) };
		bool outOfDate{ outOfSceneDate || outOfCastorDate };
		std::swap( m_outOfCastorDate, outOfCastorDate );
		std::swap( m_outOfSceneDate, outOfSceneDate );
		std::swap( m_outOfDate, outOfDate );

		if ( outOfDate != m_outOfDate )
		{
			if ( outOfDate )
			{
				if ( remove )
				{
					m_counts->removeOutdated();
				}
			}
			else
			{
				m_counts->addOutdated();
			}
		}
	}

	//*********************************************************************************************
}
