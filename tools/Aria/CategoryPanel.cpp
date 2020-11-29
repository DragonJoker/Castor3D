#include "Aria/CategoryPanel.hpp"

#include "Aria/Database/DbDateTimeHelpers.hpp"

#include <wx/sizer.h>
#include <wx/stattext.h>

#include <algorithm>

namespace aria
{
	namespace
	{
		uint32_t getTestsCount( TestArray const & tests )
		{
			return uint32_t( tests.size() );
		}

		uint32_t getStatusTests( TestArray const & tests
			, TestStatus status )
		{
			return uint32_t( std::count_if( tests.begin()
				, tests.end()
				, [&status]( Test const & lookup )
				{
					return status == lookup.status
						&& !lookup.ignoreResult;
				} ) );
		}

		uint32_t getIgnoredTests( TestArray const & tests )
		{
			return uint32_t( std::count_if( tests.begin()
				, tests.end()
				, []( Test const & lookup )
				{
					return lookup.ignoreResult;
				} ) );
		}

		uint32_t getOutdatedTests( TestArray const & tests
			, db::DateTime const & date )
		{
			assert( db::date_time::isValid( date ) );
			return uint32_t( std::count_if( tests.begin()
				, tests.end()
				, [&date]( Test const & lookup )
				{
					return lookup.castorDate != date;
				} ) );
		}

		uint32_t getOutdatedTests( TestArray const & tests
			, Config const & config )
		{
			return getOutdatedTests( tests, getFileDate( config.castor ) );
		}

		uint32_t getTestsCount( TestCategoryMap const & tests )
		{
			uint32_t result{};

			for ( auto & catTests : tests )
			{
				result += getTestsCount( catTests.second );
			}

			return result;
		}

		uint32_t getStatusTests( TestCategoryMap const & tests
			, TestStatus status )
		{
			uint32_t result{};

			for ( auto & catTests : tests )
			{
				result += getStatusTests( catTests.second, status );
			}

			return result;
		}

		uint32_t getIgnoredTests( TestCategoryMap const & tests )
		{
			uint32_t result{};

			for ( auto & catTests : tests )
			{
				result += getIgnoredTests( catTests.second );
			}

			return result;
		}

		uint32_t getOutdatedTests( TestCategoryMap const & tests
			, db::DateTime const & date )
		{
			uint32_t result{};

			for ( auto & catTests : tests )
			{
				result += getOutdatedTests( catTests.second, date );
			}

			return result;
		}

		uint32_t getOutdatedTests( TestCategoryMap const & tests
			, Config const & config )
		{
			return getOutdatedTests( tests, getFileDate( config.castor ) );
		}

		uint32_t getTestsCount( TestMap const & tests )
		{
			uint32_t result{};

			for ( auto & rendTests : tests )
			{
				result += getTestsCount( rendTests.second );
			}

			return result;
		}

		uint32_t getStatusTests( TestMap const & tests
			, TestStatus status )
		{
			uint32_t result{};

			for ( auto & rendTests : tests )
			{
				result += getStatusTests( rendTests.second, status );
			}

			return result;
		}

		uint32_t getIgnoredTests( TestMap const & tests )
		{
			uint32_t result{};

			for ( auto & rendTests : tests )
			{
				result += getIgnoredTests( rendTests.second );
			}

			return result;
		}

		uint32_t getOutdatedTests( TestMap const & tests
			, Config const & config )
		{
			uint32_t result{};
			auto date = getFileDate( config.castor );
			assert( db::date_time::isValid( date ) );

			for ( auto & rendTests : tests )
			{
				result += getOutdatedTests( rendTests.second, date );
			}

			return result;
		}

		template< typename TestContT >
		void updateCounts( TestContT const & tests
			, std::list< TestNode > const & running
			, Config const & config
			, CategoryPanel::Counts & counts )
		{
			counts.all = getTestsCount( tests );
			counts.negligible = getStatusTests( tests, TestStatus::eNegligible );
			counts.acceptable = getStatusTests( tests, TestStatus::eAcceptable );
			counts.unacceptable = getStatusTests( tests, TestStatus::eUnacceptable );
			counts.unprocessed = getStatusTests( tests, TestStatus::eUnprocessed );
			counts.ignored = getIgnoredTests( tests );
			counts.outdated = getOutdatedTests( tests, config );
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

	void CategoryPanel::setRenderer( wxString const & renderer
			, TestCategoryMap const & tests
		, std::list< TestNode > const & running )
	{
		m_allTests = nullptr;
		m_rendererTests = &tests;
		m_categoryTests = nullptr;
		m_counts.name = renderer;
		update( running );
	}

	void CategoryPanel::setCategory( wxString const & category
		, TestArray const & tests
		, std::list< TestNode > const & running )
	{
		m_allTests = nullptr;
		m_rendererTests = nullptr;
		m_categoryTests = &tests;
		m_counts.name = category;
		update( running );
	}

	void CategoryPanel::setAll( TestMap const & tests
		, std::list< TestNode > const & running )
	{
		m_allTests = &tests;
		m_rendererTests = nullptr;
		m_categoryTests = nullptr;
		m_counts.name = wxT( "All" );
		update( running );
	}

	void CategoryPanel::update( std::list< TestNode > const & running )
	{
		if ( m_allTests )
		{
			updateCounts( *m_allTests, running, m_config, m_counts );
		}
		else if ( m_rendererTests )
		{
			updateCounts( *m_rendererTests, running, m_config, m_counts );
		}
		else
		{
			updateCounts( *m_categoryTests, running, m_config, m_counts );
		}

		m_status->SetLabel( wxString{ m_counts.name } << ": " << m_counts.all << " tests" );
		m_negligible->SetLabel( wxString{ "- " } << m_counts.negligible << " negligible." );
		m_acceptable->SetLabel( wxString{ "- " } << m_counts.acceptable << " acceptable." );
		m_unacceptable->SetLabel( wxString{ "- " } << m_counts.unacceptable << " unacceptable." );
		m_unprocessed->SetLabel( wxString{ "- " } << m_counts.unprocessed << " unprocessed." );
		m_ignored->SetLabel( wxString{ "- " } << m_counts.ignored << " ignored." );
		m_outdated->SetLabel( wxString{ "- " } << m_counts.outdated << " outdated." );
		m_running->SetLabel( wxString{ "- " } << m_counts.running << " running." );
	}
}
