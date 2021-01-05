#include "Aria/CategoryPanel.hpp"

#include "Aria/Database/DbDateTimeHelpers.hpp"
#include "Aria/TestDatabase.hpp"

#include <wx/sizer.h>
#include <wx/stattext.h>

#include <algorithm>

namespace aria
{
	namespace
	{
		uint32_t getTestsCount( TestArray const & tests
			, TestRunArray const & runs )
		{
			return uint32_t( tests.size() );
		}

		uint32_t getStatusTests( TestArray const & tests
			, TestRunArray const & runs
			, std::list< TestNode > const & running
			, TestStatus status )
		{
			return uint32_t( std::count_if( runs.begin()
				, runs.end()
				, [status]( DatabaseTest const & lookup )
				{
					return lookup->status == status;
				} ) );
		}

		uint32_t getIgnoredTests( TestArray const & tests
			, TestRunArray const & runs
			, std::list< TestNode > const & running )
		{
			return uint32_t( std::count_if( tests.begin()
				, tests.end()
				, []( TestPtr const & lookup )
				{
					return lookup->ignoreResult;
				} ) );
		}

		uint32_t getOutdatedTests( TestArray const & tests
			, TestRunArray const & runs
			, std::list< TestNode > const & running
			, db::DateTime const & date )
		{
			assert( db::date_time::isValid( date ) );
			return uint32_t( std::count_if( runs.begin()
				, runs.end()
				, [&date]( DatabaseTest const & lookup )
				{
					return lookup->castorDate != date;
				} ) );
		}

		uint32_t getOutdatedTests( TestArray const & tests
			, TestRunArray const & runs
			, std::list< TestNode > const & running
			, Config const & config )
		{
			return getOutdatedTests( tests, runs, {}, getFileDate( config.castor ) );
		}

		uint32_t getTestsCount( TestMap const & tests
			, TestRunCategoryMap const & runs )
		{
			uint32_t result{};

			for ( auto & catTests : tests )
			{
				result += getTestsCount( catTests.second, {} );
			}

			return result;
		}

		uint32_t getStatusTests( TestMap const & tests
			, TestRunCategoryMap const & runs
			, std::list< TestNode > const & running
			, TestStatus status )
		{
			uint32_t result{};

			for ( auto & catRuns : runs )
			{
				result += getStatusTests( {}, catRuns.second, running, status );
			}

			return result;
		}

		uint32_t getIgnoredTests( TestMap const & tests
			, TestRunCategoryMap const & runs
			, std::list< TestNode > const & running )
		{
			uint32_t result{};

			for ( auto & catTests : tests )
			{
				result += getIgnoredTests( catTests.second, {}, running );
			}

			return result;
		}

		uint32_t getOutdatedTests( TestMap const & tests
			, TestRunCategoryMap const & runs
			, std::list< TestNode > const & running
			, Config const & config )
		{
			uint32_t result{};
			auto date = getFileDate( config.castor );
			assert( db::date_time::isValid( date ) );

			for ( auto & catTests : runs )
			{
				result += getOutdatedTests( {}, catTests.second, running, date );
			}

			return result;
		}

		uint32_t getTestsCount( TestMap const & tests
			, TestRunMap const & runs )
		{
			uint32_t result{};

			for ( auto & catTests : tests )
			{
				result += getTestsCount( catTests.second, {} );
			}

			return uint32_t( result * runs.size() );
		}

		uint32_t getStatusTests( TestMap const & tests
			, TestRunMap const & runs
			, std::list< TestNode > const & running
			, TestStatus status )
		{
			uint32_t result{};

			for ( auto & rendRuns : runs )
			{
				result += getStatusTests( {}, rendRuns.second, running, status );
			}

			return result;
		}

		uint32_t getIgnoredTests( TestMap const & tests
			, TestRunMap const & runs
			, std::list< TestNode > const & running )
		{
			uint32_t result{};

			for ( auto & catTests : tests )
			{
				result += getIgnoredTests( catTests.second, {}, running );
			}

			return uint32_t( result * runs.size() );
		}

		uint32_t getOutdatedTests( TestMap const & tests
			, TestRunMap const & runs
			, std::list< TestNode > const & running
			, Config const & config )
		{
			uint32_t result{};

			for ( auto & rendRuns : runs )
			{
				result += getOutdatedTests( {}, rendRuns.second, running, config );
			}

			return result;
		}

		template< typename TestContT, typename RunContT >
		void updateCounts( TestContT const & tests
			, RunContT const & runs
			, std::list< TestNode > const & running
			, Config const & config
			, CategoryPanel::Counts & counts )
		{
			counts.all = getTestsCount( tests, runs );
			counts.negligible = getStatusTests( tests, runs, running, TestStatus::eNegligible );
			counts.acceptable = getStatusTests( tests, runs, running, TestStatus::eAcceptable );
			counts.unacceptable = getStatusTests( tests, runs, running, TestStatus::eUnacceptable );
			counts.unprocessed = getStatusTests( tests, runs, running, TestStatus::eUnprocessed );
			counts.ignored = getIgnoredTests( tests, runs, running );
			counts.outdated = getOutdatedTests( tests, runs, running, config );
			counts.running = uint32_t( running.size() );
		}
	}

	CategoryPanel::CategoryPanel( Config const & config
		, wxWindow * parent
		, wxPoint const & position
		, wxSize const & size )
		: wxPanel{ parent, wxID_ANY, position, size }
		, m_config{ config }
	{
		SetBackgroundColour( PANEL_BACKGROUND_COLOUR);
		SetForegroundColour( PANEL_FOREGROUND_COLOUR );

		m_status = new wxStaticText{ this, wxID_ANY, wxEmptyString };
		m_status->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_status->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_negligible = new wxStaticText{ this, wxID_ANY, wxEmptyString };
		m_negligible->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_negligible->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_acceptable = new wxStaticText{ this, wxID_ANY, wxEmptyString };
		m_acceptable->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_acceptable->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_unacceptable = new wxStaticText{ this, wxID_ANY, wxEmptyString };
		m_unacceptable->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_unacceptable->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_unprocessed = new wxStaticText{ this, wxID_ANY, wxEmptyString };
		m_unprocessed->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_unprocessed->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_ignored = new wxStaticText{ this, wxID_ANY, wxEmptyString };
		m_ignored->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_ignored->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_running = new wxStaticText{ this, wxID_ANY, wxEmptyString };
		m_running->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_running->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_outdated = new wxStaticText{ this, wxID_ANY, wxEmptyString };
		m_outdated->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_outdated->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		wxBoxSizer * sizer{ new wxBoxSizer{ wxVERTICAL } };
		sizer->AddStretchSpacer();
		sizer->Add( m_status, wxSizerFlags{}.Left() );
		sizer->Add( m_negligible, wxSizerFlags{}.Left() );
		sizer->Add( m_acceptable, wxSizerFlags{}.Left() );
		sizer->Add( m_unacceptable, wxSizerFlags{}.Left() );
		sizer->Add( m_unprocessed, wxSizerFlags{}.Left() );
		sizer->Add( m_ignored, wxSizerFlags{}.Left() );
		sizer->Add( m_outdated, wxSizerFlags{}.Left() );
		sizer->Add( m_running, wxSizerFlags{}.Left() );
		sizer->AddStretchSpacer();

		SetSizer( sizer );
		sizer->SetSizeHints( this );
	}

	void CategoryPanel::setAll( TestMap const & tests
		, TestRunMap const & runs
		, std::list< TestNode > const & running )
	{
		m_allTests = &tests;
		m_allRuns = &runs;
		m_rendererTests = nullptr;
		m_rendererRuns = nullptr;
		m_categoryTests = nullptr;
		m_categoryRuns = nullptr;
		m_counts.name = "All";
		update( running );
	}

	void CategoryPanel::setRenderer( wxString const & renderer
		, TestMap const & tests
		, TestRunCategoryMap const & runs
		, std::list< TestNode > const & running )
	{
		m_allTests = nullptr;
		m_allRuns = nullptr;
		m_rendererTests = &tests;
		m_rendererRuns = &runs;
		m_categoryTests = nullptr;
		m_categoryRuns = nullptr;
		m_counts.name = renderer;
		update( running );
	}

	void CategoryPanel::setCategory( wxString const & category
		, TestArray const & tests
		, TestRunArray const & runs
		, std::list< TestNode > const & running )
	{
		m_allTests = nullptr;
		m_allRuns = nullptr;
		m_rendererTests = nullptr;
		m_rendererRuns = nullptr;
		m_categoryTests = &tests;
		m_categoryRuns = &runs;
		m_counts.name = category;
		update( running );
	}

	void CategoryPanel::update( std::list< TestNode > const & running )
	{
		if ( m_allTests )
		{
			updateCounts( *m_allTests, *m_allRuns, running, m_config, m_counts );
		}
		else if ( m_rendererTests )
		{
			updateCounts( *m_rendererTests, *m_rendererRuns, running, m_config, m_counts );
		}
		else
		{
			updateCounts( *m_categoryTests, *m_categoryRuns, running, m_config, m_counts );
		}

		m_status->SetLabel( wxString{ m_counts.name } << ": " << m_counts.all << " tests" );
		m_negligible->SetLabel( wxString{ "- " } << m_counts.negligible << " (" << 100.0f * m_counts.negligible / float( m_counts.all ) << "%) negligible." );
		m_acceptable->SetLabel( wxString{ "- " } << m_counts.acceptable << " (" << 100.0f * m_counts.acceptable / float( m_counts.all ) << "%) acceptable." );
		m_unacceptable->SetLabel( wxString{ "- " } << m_counts.unacceptable << " (" << 100.0f * m_counts.unacceptable / float( m_counts.all ) << "%) unacceptable." );
		m_unprocessed->SetLabel( wxString{ "- " } << m_counts.unprocessed << " (" << 100.0f * m_counts.unprocessed / float( m_counts.all ) << "%) unprocessed." );
		m_ignored->SetLabel( wxString{ "- " } << m_counts.ignored << " (" << 100.0f * m_counts.ignored / float( m_counts.all ) << "%) ignored." );
		m_outdated->SetLabel( wxString{ "- " } << m_counts.outdated << " (" << 100.0f * m_counts.outdated / float( m_counts.all ) << "%) outdated." );
		m_running->SetLabel( wxString{ "- " } << m_counts.running << " running." );
	}
}
