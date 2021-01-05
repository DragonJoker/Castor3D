#include "Aria/Model/TreeModel.hpp"
#include "Aria/Model/TreeModelNode.hpp"

#include "Aria/TestDatabase.hpp"
#include "Aria/Database/DbDateTimeHelpers.hpp"

#include <CastorUtils/Exception/Assertion.hpp>
#include <CastorUtils/Log/Logger.hpp>

#include <wx/dc.h>

#include "Aria/xpms/acceptable.xpm"
#include "Aria/xpms/ignored.xpm"
#include "Aria/xpms/negligible.xpm"
#include "Aria/xpms/none.xpm"
#include "Aria/xpms/notrun.xpm"
#include "Aria/xpms/outofdate.xpm"
#include "Aria/xpms/outofdate2.xpm"
#include "Aria/xpms/unacceptable.xpm"
#include "Aria/xpms/unprocessed.xpm"
#include "Aria/xpms/progress_1.xpm"
#include "Aria/xpms/progress_2.xpm"
#include "Aria/xpms/progress_3.xpm"
#include "Aria/xpms/progress_4.xpm"
#include "Aria/xpms/progress_5.xpm"
#include "Aria/xpms/progress_6.xpm"
#include "Aria/xpms/progress_7.xpm"
#include "Aria/xpms/progress_8.xpm"
#include "Aria/xpms/progress_9.xpm"
#include "Aria/xpms/progress_10.xpm"
#include "Aria/xpms/progress_11.xpm"
#include "Aria/xpms/progress_12.xpm"

namespace aria
{
	namespace
	{
		class DataViewTestStatusRenderer
			: public wxDataViewCustomRenderer
		{
		private:
			using Clock = std::chrono::high_resolution_clock;

		public:
			static wxString GetDefaultType()
			{
				return wxT( "long" );
			}

			DataViewTestStatusRenderer( const wxString & varianttype
				, wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT
				, int align = wxDVR_DEFAULT_ALIGNMENT )
				: wxDataViewCustomRenderer{ varianttype, mode, align }
				, m_size{ 20, 20 }
				, m_outOfCastorDateBmp{ createImage( outofdate_xpm ) }
				, m_outOfSceneDateBmp{ createImage( outofdate2_xpm ) }
				, m_bitmaps{ createImage( ignored_xpm )
				, createImage( notrun_xpm )
				, createImage( negligible_xpm )
				, createImage( acceptable_xpm )
				, createImage( unacceptable_xpm )
				, createImage( unprocessed_xpm )
				, createImage( progress_1_xpm )
				, createImage( progress_2_xpm )
				, createImage( progress_3_xpm )
				, createImage( progress_4_xpm )
				, createImage( progress_5_xpm )
				, createImage( progress_6_xpm )
				, createImage( progress_7_xpm )
				, createImage( progress_8_xpm )
				, createImage( progress_9_xpm )
				, createImage( progress_10_xpm )
				, createImage( progress_11_xpm )
				, createImage( progress_12_xpm ) }
			{
			}

			bool SetValue( const wxVariant & value ) override
			{
				m_value = uint32_t( value.GetLong() );
				m_index = ( m_value >> 2 );
				m_outOfCastorDate = ( m_value & 0x01u );
				m_outOfSceneDate = ( m_value & 0x02u );
				return true;
			}

			bool GetValue( wxVariant & value ) const override
			{
				value = long( m_value );
				return true;
			}

			bool Render( wxRect cell, wxDC * dc, int state ) override
			{
				dc->DrawBitmap( m_bitmaps[m_index]
					, cell.x
					, cell.y
					, true );

				if ( m_index < 6 )
				{
					if ( m_outOfCastorDate )
					{
						dc->DrawBitmap( m_outOfCastorDateBmp
							, cell.x
							, cell.y
							, true );
					}

					if ( m_outOfSceneDate )
					{
						dc->DrawBitmap( m_outOfSceneDateBmp
							, cell.x
							, cell.y
							, true );
					}
				}

				return false;
			}

			wxSize GetSize() const override
			{
				return m_size;
			}

		private:
			wxImage createImage( char const * const * xpmData )
			{
				wxImage result{ xpmData };
				return result.Scale( m_size.x, m_size.y );
			}

		private:
			wxSize m_size;
			wxBitmap m_outOfCastorDateBmp;
			wxBitmap m_outOfSceneDateBmp;
			std::array< wxBitmap, size_t( TestStatus::eCount ) + AdditionalIndices > m_bitmaps;
			uint32_t m_value;
			uint32_t m_index;
			bool m_outOfCastorDate{};
			bool m_outOfSceneDate{};
		};
	}

	TreeModel::TreeModel( Config const & config
		, Renderer renderer )
		: m_config{ config }
		, m_renderer{ renderer }
		, m_root( new TreeModelNode{ nullptr, renderer, nullptr } )
	{
	}

	TreeModel::~TreeModel()
	{
		delete m_root;
	}

	TreeModelNode * TreeModel::addCategory( Category category )
	{
		TreeModelNode * node = new TreeModelNode{ m_root, m_renderer, category };
		m_categories[category->name] = node;
		m_root->Append( node );
		return node;
	}

	TreeModelNode * TreeModel::addTest( DatabaseTest & test )
	{
		auto it = m_categories.find( test->test->category->name );
		CU_Require( m_categories.end() != it );
		TreeModelNode * node = new TreeModelNode{ it->second, test };
		it->second->Append( node );
		return node;
	}

	void TreeModel::expandRoots( wxDataViewCtrl * view )
	{
		view->Expand( wxDataViewItem{ m_root } );
	}

	void TreeModel::instantiate( wxDataViewCtrl * view )
	{
		uint32_t flags = wxCOL_SORTABLE | wxCOL_RESIZABLE;
		auto renCategory = new wxDataViewTextRenderer{ GetColumnType( unsigned( TreeModel::Column::eCategory ) ) };
		wxDataViewColumn * colCategory = new wxDataViewColumn( _( "Category" ), renCategory, int( TreeModel::Column::eCategory ), 150, wxALIGN_LEFT, flags );
		colCategory->SetMinWidth( 150 );
		view->AppendColumn( colCategory );
		auto renScene = new wxDataViewTextRenderer{ GetColumnType( unsigned( TreeModel::Column::eName ) ) };
		wxDataViewColumn * colScene = new wxDataViewColumn( _( "Name" ), renScene, int( TreeModel::Column::eName ), 400, wxALIGN_LEFT, flags );
		colScene->SetMinWidth( 400 );
		view->AppendColumn( colScene );
		auto renRunDate = new wxDataViewTextRenderer{ GetColumnType( unsigned( TreeModel::Column::eRunDate ) ) };
		wxDataViewColumn * colRunDate = new wxDataViewColumn( _( "Run Date" ), renRunDate, int( TreeModel::Column::eRunDate ), 80, wxALIGN_LEFT, flags );
		colRunDate->SetMinWidth( 80 );
		view->AppendColumn( colRunDate );
		auto renRunTime = new wxDataViewTextRenderer{ GetColumnType( unsigned( TreeModel::Column::eRunTime ) ) };
		wxDataViewColumn * colRunTime = new wxDataViewColumn( _( "Run Time" ), renRunDate, int( TreeModel::Column::eRunTime ), 80, wxALIGN_LEFT, flags );
		colRunTime->SetMinWidth( 80 );
		view->AppendColumn( colRunTime );
		auto renStatus = new DataViewTestStatusRenderer{ GetColumnType( unsigned( TreeModel::Column::eStatus ) ) };
		wxDataViewColumn * colStatus = new wxDataViewColumn( _( "Status" ), renStatus, int( TreeModel::Column::eStatus ), 30, wxALIGN_LEFT, flags );
		colStatus->SetMinWidth( 30 );
		view->AppendColumn( colStatus );
	}

	uint32_t TreeModel::getTestId( wxDataViewItem const & item )const
	{
		uint32_t result{};
		auto node = static_cast< TreeModelNode * >( item.GetID() );

		if ( node && node->test )
		{
			result = node->test->getTestId();
		}

		return result;
	}

	uint32_t TreeModel::getRunId( wxDataViewItem const & item )const
	{
		uint32_t result{};
		auto node = static_cast< TreeModelNode * >( item.GetID() );

		if ( node && node->test )
		{
			result = node->test->getRunId();
		}

		return result;
	}

	std::string TreeModel::getName( wxDataViewItem const & item )const
	{
		std::string result{};
		auto node = static_cast< TreeModelNode * >( item.GetID() );

		if ( node && node->test )
		{
			result = node->test->getName();
		}

		return result;
	}

	db::Date TreeModel::getRunDate( wxDataViewItem const & item )const
	{
		db::Date result{ boost::posix_time::not_a_date_time };
		auto node = static_cast< TreeModelNode * >( item.GetID() );

		if ( node && node->test )
		{
			result = node->test->getRunDate().date();
		}

		return result;
	}

	db::Time TreeModel::getRunTime( wxDataViewItem const & item )const
	{
		db::Time result{ boost::posix_time::not_a_date_time };
		auto node = static_cast< TreeModelNode * >( item.GetID() );

		if ( node && node->test )
		{
			result = node->test->getRunDate().time_of_day();
		}

		return result;
	}

	TestStatus TreeModel::getStatus( wxDataViewItem const & item )const
	{
		TestStatus result{};
		auto node = static_cast< TreeModelNode * >( item.GetID() );

		if ( node && node->test )
		{
			result = node->test->getStatus();
		}

		return result;
	}

	std::string TreeModel::getRenderer( wxDataViewItem const & item )const
	{
		std::string result{};
		auto node = static_cast< TreeModelNode * >( item.GetID() );

		if ( node && node->test )
		{
			result = node->test->getRenderer()->name;
		}

		return result;
	}

	std::string TreeModel::getCategory( wxDataViewItem const & item )const
	{
		std::string result{};
		auto node = static_cast< TreeModelNode * >( item.GetID() );

		if ( node )
		{
			if ( node->test )
			{
				result = node->test->getCategory()->name;
			}
			else
			{
				result = node->category->name;
			}
		}

		return result;
	}

	void TreeModel::Delete( wxDataViewItem const & item )
	{
		auto node = static_cast< TreeModelNode * >( item.GetID() );

		if ( node )
		{
			wxDataViewItem parent( node->GetParent() );

			if ( parent.IsOk() )
			{
				// first remove the node from the parent's array of children;
				// NOTE: MyMusicTreeModelNodePtrArray is only an array of _pointers_
				//       thus removing the node from it doesn't result in freeing it
				node->GetParent()->Remove( node );

				// free the node
				delete node;

				// notify control
				ItemDeleted( parent, item );
			}
			else
			{
				wxASSERT( node == m_root );
				// don't make the control completely empty:
				castor::Logger::logError( "Cannot remove the root item!" );
			}
		}
	}

	int TreeModel::Compare( wxDataViewItem const & item1
		, wxDataViewItem const & item2
		, unsigned int column
		, bool ascending )const
	{
		int result = 0;
		wxASSERT( item1.IsOk() && item2.IsOk() );

		if ( IsContainer( item1 ) && IsContainer( item2 ) )
		{
			wxVariant value1, value2;
			GetValue( value1, item1, 0 );
			GetValue( value2, item2, 0 );

			wxString str1 = value1.GetString();
			wxString str2 = value2.GetString();
			int res = str1.Cmp( str2 );

			if ( res )
			{
				result = res;
			}
			else
			{
				// items must be different
				auto litem1 = wxUIntPtr( item1.GetID() );
				auto litem2 = wxUIntPtr( item2.GetID() );
				result = litem1 - litem2;
			}
		}
		else if ( column == int( Column::eStatus ) )
		{
			auto node1 = static_cast< TreeModelNode const * >( item1.GetID() );
			auto node2 = static_cast< TreeModelNode const * >( item2.GetID() );
			result = ( node1->test->getStatus() < node2->test->getStatus()
				? -1
				: ( node1->test->getStatus() == node2->test->getStatus()
					? 0
					: 1 ) );
			result = ascending
				? result
				: -result;
		}
		else
		{
			result = wxDataViewModel::Compare( item1, item2, column, ascending );
		}

		return result;
	}

	unsigned int TreeModel::GetColumnCount()const
	{
		return uint32_t( Column::eCount );
	}

	wxString TreeModel::GetColumnType( unsigned int col )const
	{
		switch ( Column( col ) )
		{
		case Column::eStatus:
			return DataViewTestStatusRenderer::GetDefaultType();
		default:
			return wxT( "string" );
		}
	}

	void TreeModel::GetValue( wxVariant & variant
		, wxDataViewItem const & item
		, unsigned int col )const
	{
		wxASSERT( item.IsOk() );
		auto node = static_cast< TreeModelNode * >( item.GetID() );

		if ( !node )
		{
			return;
		}

		if ( node->test )
		{
			switch ( Column( col ) )
			{
			case Column::eCategory:
				variant = makeWxString( node->test->getCategory()->name );
				break;

			case Column::eName:
				variant = makeWxString( node->test->getName() );
				break;

			case Column::eRunDate:
				variant = ( db::date_time::isValid( node->test->getRunDate() ) && db::date::isValid( node->test->getRunDate().date() ) )
					? makeWxString( db::date::print( node->test->getRunDate().date(), DISPLAY_DATETIME_DATE ) )
					: wxEmptyString;
				break;

			case Column::eRunTime:
				variant = ( db::date_time::isValid( node->test->getRunDate() ) && db::time::isValid( node->test->getRunDate().time_of_day() ) )
					? makeWxString( db::time::print( node->test->getRunDate().time_of_day(), DISPLAY_DATETIME_TIME ) )
					: wxEmptyString;
				break;

			case Column::eStatus:
				variant = long( getTestStatusIndex( m_config, **node->test ) );
				break;

			default:
				castor::Logger::logError( "TreeModel::GetValue: wrong column " + castor::string::toString( col ) );
				break;
			}
		}
		else
		{
			switch ( Column( col ) )
			{
			case Column::eCategory:
				variant = node->category
					? makeWxString( node->category->name )
					: wxT( "All" );
				break;

			case Column::eName:
			case Column::eRunDate:
			case Column::eRunTime:
			case Column::eStatus:
				break;

			default:
				castor::Logger::logError( "TreeModel::GetValue: wrong column " + castor::string::toString( col ) );
				break;
			}
		}
	}

	bool TreeModel::SetValue( const wxVariant & variant
		, wxDataViewItem const & item
		, unsigned int col )
	{
		bool result = false;
		wxASSERT( item.IsOk() );
		auto node = static_cast< TreeModelNode * >( item.GetID() );

		if ( !node )
		{
			return result;
		}

		switch ( Column( col ) )
		{
		case Column::eCategory:
		case Column::eName:
		case Column::eRunDate:
		case Column::eRunTime:
		case Column::eStatus:
			break;

		default:
			castor::Logger::logError( "TreeModel::SetValue: wrong column " + castor::string::toString( col ) );
			break;
		}

		return result;
	}

	wxDataViewItem TreeModel::GetParent( wxDataViewItem const & item )const
	{
		wxDataViewItem result( 0 );

		if ( item.IsOk() )
		{
			auto node = static_cast< TreeModelNode * >( item.GetID() );

			if ( node && m_root != node )
			{
				result = wxDataViewItem{ node->GetParent() };
			}
		}

		return result;
	}

	bool TreeModel::IsContainer( wxDataViewItem const & item )const
	{
		bool result = false;

		if ( !item.IsOk() )
		{
			result = true;
		}
		else
		{
			auto node = static_cast< TreeModelNode * >( item.GetID() );

			if ( node )
			{
				result = node->IsContainer();
			}
		}

		return result;
	}

	unsigned int TreeModel::GetChildren( const wxDataViewItem & parent
		, wxDataViewItemArray & array )const
	{
		unsigned int result = 0;
		auto node = static_cast< TreeModelNode * >( parent.GetID() );

		if ( !node )
		{
			array.Add( wxDataViewItem{ m_root } );
			result = 1;
		}
		else
		{
			if ( node->GetChildCount() > 0 )
			{
				auto count = static_cast< unsigned int >( node->GetChildCount() );

				for ( unsigned int pos = 0; pos < count; pos++ )
				{
					auto child = node->GetNthChild( pos );
					array.Add( wxDataViewItem{ child } );
				}

				result = count;
			}
		}

		return result;
	}
}
