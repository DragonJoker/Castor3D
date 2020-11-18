#include "CastorTestParser/CategoryPanel.hpp"

#include "CastorTestParser/Database/DbDateTimeHelpers.hpp"

#include <wx/sizer.h>
#include <wx/stattext.h>

#include <algorithm>

namespace test_parser
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

		uint32_t getTestsCount( TestMap const & tests )
		{
			uint32_t result{};

			for ( auto & catTtests : tests )
			{
				result += getTestsCount( catTtests.second );
			}

			return result;
		}

		uint32_t getStatusTests( TestMap const & tests
			, TestStatus status )
		{
			uint32_t result{};

			for ( auto & catTtests : tests )
			{
				result += getStatusTests( catTtests.second, status );
			}

			return result;
		}

		uint32_t getIgnoredTests( TestMap const & tests )
		{
			uint32_t result{};

			for ( auto & catTtests : tests )
			{
				result += getIgnoredTests( catTtests.second );
			}

			return result;
		}

		uint32_t getOutdatedTests( TestMap const & tests
			, Config const & config )
		{
			uint32_t result{};
			auto date = getFileDate( config.castor );
			assert( db::date_time::isValid( date ) );

			for ( auto & catTtests : tests )
			{
				result += getOutdatedTests( catTtests.second, date );
			}

			return result;
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

	void CategoryPanel::setCategory( wxString const & category
		, TestArray const & tests )
	{
		m_category = category;
		m_tests = &tests;
		m_status->SetLabel( wxString{ category } << ": " << getTestsCount( tests ) << " tests" );
		m_negligible->SetLabel( wxString{ "- " } << getStatusTests( tests, TestStatus::eNegligible ) << " negligible." );
		m_acceptable->SetLabel( wxString{ "- " } << getStatusTests( tests, TestStatus::eAcceptable ) << " acceptable." );
		m_unacceptable->SetLabel( wxString{ "- " } << getStatusTests( tests, TestStatus::eUnacceptable ) << " unacceptable." );
		m_unprocessed->SetLabel( wxString{ "- " } << getStatusTests( tests, TestStatus::eUnprocessed ) << " unprocessed." );
		m_ignored->SetLabel( wxString{ "- " } << getIgnoredTests( tests ) << " ignored." );
		m_outdated->SetLabel( wxString{ "- " } << getOutdatedTests( tests, m_config ) << " outdated." );
		m_running->SetLabel( wxEmptyString );
	}

	void CategoryPanel::setAll( TestMap const & tests
		, std::list< TestNode > const & running )
	{
		m_status->SetLabel( wxString{ "All" } << ": " << getTestsCount( tests ) << " tests" );
		m_negligible->SetLabel( wxString{ "- " } << getStatusTests( tests, TestStatus::eNegligible ) << " negligible." );
		m_acceptable->SetLabel( wxString{ "- " } << getStatusTests( tests, TestStatus::eAcceptable ) << " acceptable." );
		m_unacceptable->SetLabel( wxString{ "- " } << getStatusTests( tests, TestStatus::eUnacceptable ) << " unacceptable." );
		m_unprocessed->SetLabel( wxString{ "- " } << getStatusTests( tests, TestStatus::eUnprocessed ) << " unprocessed." );
		m_ignored->SetLabel( wxString{ "- " } << getIgnoredTests( tests ) << " ignored." );
		m_outdated->SetLabel( wxString{ "- " } << getOutdatedTests( tests, m_config ) << " outdated." );
		m_running->SetLabel( wxString{ "- " } << running.size() << " running." );
	}
}