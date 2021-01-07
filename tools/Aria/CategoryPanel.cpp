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
			, TestStatus status )
		{
			return uint32_t( std::count_if( runs.begin()
				, runs.end()
				, [status]( DatabaseTest const & lookup )
				{
					return lookup->status == status;
				} ) );
		}

		uint32_t getStatusTests( TestArray const & tests
			, TestRunArray const & runs
			, TestStatus min
			, TestStatus max )
		{
			return uint32_t( std::count_if( runs.begin()
				, runs.end()
				, [min, max]( DatabaseTest const & lookup )
				{
					return lookup->status >= min
						&& lookup->status <= max;
				} ) );
		}

		uint32_t getIgnoredTests( TestArray const & tests
			, TestRunArray const & runs )
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
			, Config const & config )
		{
			return uint32_t( std::count_if( runs.begin()
				, runs.end()
				, [&config]( DatabaseTest const & lookup )
				{
					return isOutOfDate( config, *lookup );
				} ) );
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
			, TestStatus status )
		{
			uint32_t result{};

			for ( auto & catRuns : runs )
			{
				result += getStatusTests( {}, catRuns.second, status );
			}

			return result;
		}

		uint32_t getStatusTests( TestMap const & tests
			, TestRunCategoryMap const & runs
			, TestStatus min
			, TestStatus max )
		{
			uint32_t result{};

			for ( auto & catRuns : runs )
			{
				result += getStatusTests( {}, catRuns.second, min, max );
			}

			return result;
		}

		uint32_t getIgnoredTests( TestMap const & tests
			, TestRunCategoryMap const & runs )
		{
			uint32_t result{};

			for ( auto & catTests : tests )
			{
				result += getIgnoredTests( catTests.second, {} );
			}

			return result;
		}

		uint32_t getOutdatedTests( TestMap const & tests
			, TestRunCategoryMap const & runs
			, Config const & config )
		{
			uint32_t result{};

			for ( auto & catTests : runs )
			{
				result += getOutdatedTests( {}, catTests.second, config );
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
			, TestStatus status )
		{
			uint32_t result{};

			for ( auto & rendRuns : runs )
			{
				result += getStatusTests( {}, rendRuns.second, status );
			}

			return result;
		}

		uint32_t getStatusTests( TestMap const & tests
			, TestRunMap const & runs
			, TestStatus min
			, TestStatus max )
		{
			uint32_t result{};

			for ( auto & rendRuns : runs )
			{
				result += getStatusTests( {}, rendRuns.second, min, max );
			}

			return result;
		}

		uint32_t getIgnoredTests( TestMap const & tests
			, TestRunMap const & runs )
		{
			uint32_t result{};

			for ( auto & catTests : tests )
			{
				result += getIgnoredTests( catTests.second, {} );
			}

			return uint32_t( result * runs.size() );
		}

		uint32_t getOutdatedTests( TestMap const & tests
			, TestRunMap const & runs
			, Config const & config )
		{
			uint32_t result{};

			for ( auto & rendRuns : runs )
			{
				result += getOutdatedTests( {}, rendRuns.second, config );
			}

			return result;
		}

		template< typename TestContT, typename RunContT >
		void updateCounts( TestContT const & tests
			, RunContT const & runs
			, Config const & config
			, TestsCounts & counts )
		{
			counts.all = getTestsCount( tests, runs );
			counts.negligible = getStatusTests( tests, runs, TestStatus::eNegligible );
			counts.acceptable = getStatusTests( tests, runs, TestStatus::eAcceptable );
			counts.unacceptable = getStatusTests( tests, runs, TestStatus::eUnacceptable );
			counts.unprocessed = getStatusTests( tests, runs, TestStatus::eUnprocessed );
			counts.ignored = getIgnoredTests( tests, runs );
			counts.outdated = getOutdatedTests( tests, runs, config );
			counts.pending = getStatusTests( tests, runs, TestStatus::ePending );
			counts.running = getStatusTests( tests, runs, TestStatus::eRunning_Begin, TestStatus::eRunning_End );
		}
	}

	//*********************************************************************************************

	TestsCounts::TestsCounts( Config const & config
		, TestMap const & tests
		, TestRunMap const & runs )
		: m_config{ config }
		, m_allTests{ &tests }
		, m_allRuns{ &runs }
	{
	}

	TestsCounts::TestsCounts( Config const & config
		, TestMap const & tests
		, TestRunCategoryMap const & runs )
		: m_config{ config }
		, m_rendererTests{ &tests }
		, m_rendererRuns{ &runs }
	{
	}

	TestsCounts::TestsCounts( Config const & config
		, TestArray const & tests
		, TestRunArray const & runs )
		: m_config{ config }
		, m_categoryTests{ &tests }
		, m_categoryRuns{ &runs }
	{
	}

	void TestsCounts::update()
	{
		if ( m_allTests )
		{
			updateCounts( *m_allTests, *m_allRuns, m_config, *this );
		}
		else if ( m_rendererTests )
		{
			updateCounts( *m_rendererTests, *m_rendererRuns, m_config, *this );
		}
		else
		{
			updateCounts( *m_categoryTests, *m_categoryRuns, m_config, *this );
		}
	}

	//*********************************************************************************************

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
		m_pending = new wxStaticText{ this, wxID_ANY, wxEmptyString };
		m_pending->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_pending->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_running = new wxStaticText{ this, wxID_ANY, wxEmptyString };
		m_running->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_running->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_outdated = new wxStaticText{ this, wxID_ANY, wxEmptyString };
		m_outdated->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_outdated->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_ignored = new wxStaticText{ this, wxID_ANY, wxEmptyString };
		m_ignored->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_ignored->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		wxBoxSizer * sizer{ new wxBoxSizer{ wxVERTICAL } };
		sizer->AddStretchSpacer();
		sizer->Add( m_status, wxSizerFlags{}.Left() );
		sizer->Add( m_negligible, wxSizerFlags{}.Left() );
		sizer->Add( m_acceptable, wxSizerFlags{}.Left() );
		sizer->Add( m_unacceptable, wxSizerFlags{}.Left() );
		sizer->Add( m_unprocessed, wxSizerFlags{}.Left() );
		sizer->Add( m_pending, wxSizerFlags{}.Left() );
		sizer->Add( m_running, wxSizerFlags{}.Left() );
		sizer->Add( m_outdated, wxSizerFlags{}.Left() );
		sizer->Add( m_ignored, wxSizerFlags{}.Left() );
		sizer->AddStretchSpacer();

		sizer->SetSizeHints( this );
		SetSizer( sizer );
	}

	std::string displayPercent( uint32_t value, uint32_t max )
	{
		auto percent = ( 100.0f * value ) / max;
		auto stream = castor::makeStringStream();
		stream << std::setprecision( 2 ) << std::fixed << percent << "%";
		return stream.str();
	}

	void CategoryPanel::update( wxString const & name
		, TestsCounts & counts )
	{
		m_counts = &counts;
		m_status->SetLabel( wxString{ name } << ": " << m_counts->all << " tests" );
		refresh();
	}

	void CategoryPanel::refresh()
	{
		if ( m_counts )
		{
			m_negligible->SetLabel( wxString{ "- " } << m_counts->negligible << " (" << displayPercent( m_counts->negligible, m_counts->all ) << ") negligible." );
			m_acceptable->SetLabel( wxString{ "- " } << m_counts->acceptable << " (" << displayPercent( m_counts->acceptable, m_counts->all ) << ") acceptable." );
			m_unacceptable->SetLabel( wxString{ "- " } << m_counts->unacceptable << " (" << displayPercent( m_counts->unacceptable, m_counts->all ) << ") unacceptable." );
			m_unprocessed->SetLabel( wxString{ "- " } << m_counts->unprocessed << " (" << displayPercent( m_counts->unprocessed, m_counts->all ) << ") unprocessed." );
			m_pending->SetLabel( wxString{ "- " } << m_counts->pending << " (" << displayPercent( m_counts->pending, m_counts->all ) << ") pending." );
			m_running->SetLabel( wxString{ "- " } << m_counts->running << " (" << displayPercent( m_counts->running, m_counts->all ) << ") running." );
			m_outdated->SetLabel( wxString{ "- " } << m_counts->outdated << " (" << displayPercent( m_counts->outdated, m_counts->all ) << ") outdated." );
			m_ignored->SetLabel( wxString{ "- " } << m_counts->ignored << " (" << displayPercent( m_counts->ignored, m_counts->all ) << ") ignored." );
		}
	}
}
