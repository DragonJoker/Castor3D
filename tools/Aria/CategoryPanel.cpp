#include "Aria/CategoryPanel.hpp"

#include "Aria/TestDatabase.hpp"
#include "Aria/TestsCounts.hpp"
#include "Aria/Database/DbDateTimeHelpers.hpp"

#include <wx/sizer.h>
#include <wx/stattext.h>

#include <algorithm>
#include <numeric>

namespace aria
{
	//*********************************************************************************************

	namespace
	{
		std::string displayPercent( float percent )
		{
			auto stream = castor::makeStringStream();
			stream << std::setprecision( 2 ) << std::fixed << percent << "%";
			return stream.str();
		}

		std::string displayPercent( uint32_t value, uint32_t max )
		{
			return displayPercent( ( 100.0f * value ) / max );
		}

		std::string getName( TestsCountsType type )
		{
			switch ( type )
			{
			case aria::TestsCountsType::eNotRun:
				return "Not run";
			case aria::TestsCountsType::eNegligible:
				return "Negligible";
			case aria::TestsCountsType::eAcceptable:
				return "Acceptable";
			case aria::TestsCountsType::eUnacceptable:
				return "Unacceptable";
			case aria::TestsCountsType::eUnprocessed:
				return "Unprocessed";
			case aria::TestsCountsType::ePending:
				return "Pending";
			case aria::TestsCountsType::eRunning:
				return "Running";
			case aria::TestsCountsType::eIgnored:
				return "Ignored";
			case aria::TestsCountsType::eOutdated:
				return "Out of date";
			case aria::TestsCountsType::eAll:
				return "All";
			default:
				return "Unknown";
			}
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
		wxBoxSizer * sizer{ new wxBoxSizer{ wxVERTICAL } };
		sizer->AddStretchSpacer();

		m_values[TestsCountsType::eAll] = new wxStaticText{ this, wxID_ANY, wxEmptyString };
		m_values[TestsCountsType::eAll]->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_values[TestsCountsType::eAll]->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		sizer->Add( m_values[TestsCountsType::eAll], wxSizerFlags{}.Left() );

		for ( uint32_t i = 1; i < TestsCountsType::eAll; ++i )
		{
			m_values[i] = new wxStaticText{ this, wxID_ANY, wxEmptyString };
			m_values[i]->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
			m_values[i]->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
			sizer->Add( m_values[i], wxSizerFlags{}.Left() );
		}

		m_values[TestsCountsType::eNotRun] = new wxStaticText{ this, wxID_ANY, wxEmptyString };
		m_values[TestsCountsType::eNotRun]->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_values[TestsCountsType::eNotRun]->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		sizer->Add( m_values[TestsCountsType::eNotRun], wxSizerFlags{}.Left() );

		sizer->AddStretchSpacer();

		sizer->SetSizeHints( this );
		SetSizer( sizer );
	}

	void CategoryPanel::update( wxString const & name
		, AllTestsCounts & counts )
	{
		m_name = name;
		m_allCounts = &counts;
		m_rendererCounts = nullptr;
		m_categoryCounts = nullptr;

#if CTP_UseCountedValue

		for ( uint32_t i = 0; i < TestsCountsType::eCount; ++i )
		{
			auto type = TestsCountsType( i );
			m_connections[i] = m_allCounts->getCount( type ).onValueChange.connect( [type, this]( CountedUInt const & v )
				{
					if ( type == TestsCountsType::eAll )
					{
						m_values[type]->SetLabel( m_name << ": "
							<< m_allCounts->getValue( type ) << " tests" );
					}
					else
					{
						m_values[type]->SetLabel( wxString{ "- " }
							<< getName( type ) << ": "
							<< m_allCounts->getValue( type ) << " ("
							<< displayPercent( m_allCounts->getPercent( type ) ) << ")." );
					}
				} );
		}

#else

		refresh();

#endif
	}

	void CategoryPanel::update( wxString const & name
		, RendererTestsCounts & counts )
	{
		m_name = name;
		m_allCounts = nullptr;
		m_rendererCounts = &counts;
		m_categoryCounts = nullptr;

#if CTP_UseCountedValue

		for ( uint32_t i = 0; i < TestsCountsType::eCount; ++i )
		{
			auto type = TestsCountsType( i );
			m_connections[i] = m_rendererCounts->getCount( type ).onValueChange.connect( [type, this]( CountedUInt const & v )
				{
					if ( type == TestsCountsType::eAll )
					{
						m_values[type]->SetLabel( m_name << ": "
							<< m_rendererCounts->getValue( type ) << " tests" );
					}
					else
					{
						m_values[type]->SetLabel( wxString{ "- " }
							<< getName( type ) << ": "
							<< m_rendererCounts->getValue( type ) << " ("
							<< displayPercent( m_rendererCounts->getPercent( type ) ) << ")." );
					}
				} );
		}

#else

		refresh();

#endif
	}

	void CategoryPanel::update( wxString const & name
		, CategoryTestsCounts & counts )
	{
		m_name = name;
		m_allCounts = nullptr;
		m_rendererCounts = nullptr;
		m_categoryCounts = &counts;

#if CTP_UseCountedValue

		for ( uint32_t i = 0; i < TestsCountsType::eCount; ++i )
		{
			auto type = TestsCountsType( i );
			m_connections[i] = m_categoryCounts->getCount( type ).onValueChange.connect( [type, this]( CountedUInt const & v )
				{
					if ( type == TestsCountsType::eAll )
					{
						m_values[type]->SetLabel( m_name << ": "
							<< m_categoryCounts->getValue( type ) << " tests" );
					}
					else
					{
						m_values[type]->SetLabel( wxString{ "- " }
							<< getName( type ) << ": "
							<< m_categoryCounts->getValue( type ) << " ("
							<< displayPercent( m_categoryCounts->getPercent( type ) ) << ")." );
					}
				} );
		}

#else

		refresh();

#endif
	}

	void CategoryPanel::refresh()
	{
#if !CTP_UseCountedValue

		if ( m_allCounts )
		{
			for ( uint32_t i = 0; i < TestsCountsType::eCount; ++i )
			{
				auto type = TestsCountsType( i );

				if ( type == TestsCountsType::eAll )
				{
					m_values[type]->SetLabel( wxString{ m_name } << ": "
						<< m_allCounts->getValue( type ) << " tests" );
				}
				else
				{
					m_values[type]->SetLabel( wxString{ "- " }
						<< getName( type ) << ": "
						<< m_allCounts->getValue( type ) << " ("
						<< displayPercent( m_allCounts->getPercent( type ) ) << ")." );
				}
			}
		}
		else if ( m_rendererCounts )
		{
			for ( uint32_t i = 0; i < TestsCountsType::eCount; ++i )
			{
				auto type = TestsCountsType( i );

				if ( type == TestsCountsType::eAll )
				{
					m_values[type]->SetLabel( wxString{ m_name } << ": "
						<< m_rendererCounts->getValue( type ) << " tests" );
				}
				else
				{
					m_values[type]->SetLabel( wxString{ "- " }
						<< getName( type ) << ": "
						<< m_rendererCounts->getValue( type ) << " ("
						<< displayPercent( m_rendererCounts->getPercent( type ) ) << ")." );
				}
			}
		}
		else if ( m_categoryCounts )
		{
			for ( uint32_t i = 0; i < TestsCountsType::eCount; ++i )
			{
				auto type = TestsCountsType( i );

				if ( type == TestsCountsType::eAll )
				{
					m_values[type]->SetLabel( wxString{ m_name } << ": "
						<< m_categoryCounts->getValue( type ) << " tests" );
				}
				else
				{
					m_values[type]->SetLabel( wxString{ "- " }
						<< getName( type ) << ": "
						<< m_categoryCounts->getValue( type ) << " ("
						<< displayPercent( m_categoryCounts->getPercent( type ) ) << ")." );
				}
			}
		}

#endif
	}
}
