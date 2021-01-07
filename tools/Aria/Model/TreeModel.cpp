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
#include "Aria/xpms/pending.xpm"
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
	//*********************************************************************************************

	namespace
	{
		int getColumnSize( TreeModel::Column col )
		{
			switch ( col )
			{
			case aria::TreeModel::Column::eStatusName:
				return 400;
			case aria::TreeModel::Column::eRunDate:
				return 80;
			case aria::TreeModel::Column::eRunTime:
				return 90;
			default:
				return 100;
			}
		}

		wxColourBase::ChannelType mix( float percent
			, wxColourBase::ChannelType const & lhs
			, wxColourBase::ChannelType const & rhs )
		{
			return wxColourBase::ChannelType( ( lhs * ( 1.0f - percent ) ) + ( rhs * percent ) );
		}

		wxColour mix( float percent
			, wxColour const & lhs
			, wxColour const & rhs )
		{
			return wxColour{ mix( percent, lhs.Red(), rhs.Red() )
				, mix( percent, lhs.Green(), rhs.Green() )
				, mix( percent, lhs.Blue(), rhs.Blue() )
				, mix( percent, lhs.Alpha(), rhs.Alpha() ) };
		}

		uint32_t getStatusCount( TestStatus status
			, TestsCounts const & counts )
		{
			switch ( status )
			{
			case aria::TestStatus::eNotRun:
				return counts.getNotRun();
			case aria::TestStatus::eNegligible:
				return counts.negligible;
			case aria::TestStatus::eAcceptable:
				return counts.acceptable;
			case aria::TestStatus::eUnacceptable:
				return counts.unacceptable;
			case aria::TestStatus::eUnprocessed:
				return counts.unprocessed;
			case aria::TestStatus::ePending:
				return counts.pending;
			default:
				// Running.
				return counts.running;
			}
		}

		int getStatusSize( TestStatus status
			, int maxWidth
			, TestsCounts const & counts )
		{
			return int( maxWidth * getStatusCount( status, counts ) / float( counts.all ) );
		}

		wxColour getStatusColor( TestStatus status )
		{
			switch ( status )
			{
			case aria::TestStatus::eNotRun:
				return wxColour{ 0xFF, 0xFF, 0xFF, 0x00 };
			case aria::TestStatus::eNegligible:
				return wxColour{ 0x00, 0x8B, 0x19, 0xFF };
			case aria::TestStatus::eAcceptable:
				return wxColour{ 0xDC, 0xAE, 0x00, 0xFF };
			case aria::TestStatus::eUnacceptable:
				return wxColour{ 0xFF, 0x00, 0x00, 0xFF };
			case aria::TestStatus::eUnprocessed:
				return wxColour{ 0x8A, 0x8A, 0x8A, 0xFF };
			case aria::TestStatus::ePending:
				return wxColour{ 0x00, 0x80, 0xC0, 0xFF };
			default:
				return wxColour{ 0x00, 0x00, 0xFF, 0xFF };
			}
		}

		class DataViewTestStatusRenderer
			: public wxDataViewCustomRenderer
		{
		private:
			using Clock = std::chrono::high_resolution_clock;

		public:
			static wxString GetDefaultType()
			{
				return wxT( "void*" );
			}

			DataViewTestStatusRenderer( wxDataViewCtrl * parent
				, const wxString & varianttype
				, wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT
				, int align = wxDVR_DEFAULT_ALIGNMENT )
				: wxDataViewCustomRenderer{ varianttype, mode, align }
				, m_parent{ parent }
				, m_size{ 20, 20 }
				, m_outOfCastorDateBmp{ createImage( outofdate_xpm ) }
				, m_outOfSceneDateBmp{ createImage( outofdate2_xpm ) }
				, m_bitmaps{ createImage( ignored_xpm )
					, createImage( notrun_xpm )
					, createImage( negligible_xpm )
					, createImage( acceptable_xpm )
					, createImage( unacceptable_xpm )
					, createImage( unprocessed_xpm )
					, createImage( pending_xpm )
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

			~DataViewTestStatusRenderer()
			{
			}

			bool SetValue( const wxVariant & value ) override
			{
				m_statusName = reinterpret_cast< StatusName * >( value.GetVoidPtr() );

				if ( m_statusName )
				{
					m_index = ( m_statusName->statusIndex >> 2 );
					m_outOfCastorDate = ( m_statusName->statusIndex & 0x01u );
					m_outOfSceneDate = ( m_statusName->statusIndex & 0x02u );
					m_name = makeWxString( m_statusName->name );
					m_isTest = ( m_statusName->type == NodeType::eTestRun );
				}
				else
				{
					m_index = 0;
					m_outOfCastorDate = false;
					m_outOfSceneDate = false;
					m_name = wxEmptyString;
					m_isTest = true;
				}

				return true;
			}

			bool GetValue( wxVariant & value ) const override
			{
				value = reinterpret_cast< void * >( m_statusName );
				return true;
			}

			bool Render( wxRect cell, wxDC * dc, int state ) override
			{
				if ( m_isTest )
				{
					renderTest( cell, dc, state );
				}
				else
				{
					renderCategory( cell, dc, state );
				}

				if ( m_index < 6 && m_index )
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

				auto size = dc->GetTextExtent( m_name );
				wxPoint offset{ cell.x + 10 + m_size.x, cell.y + ( cell.height - size.y ) / 2 };
				auto foreground = dc->GetTextForeground();
				dc->SetTextForeground( *wxBLACK );
				dc->DrawText( m_name, offset );
				dc->SetTextForeground( foreground );
				return false;
			}

			wxSize GetSize() const override
			{
				return { m_parent->GetColumn( 0u )->GetWidth(), m_size.y };
			}

		private:
			wxImage createImage( char const * const * xpmData )
			{
				wxImage result{ xpmData };
				return result.Scale( m_size.x, m_size.y );
			}

			void renderTest( wxRect cell, wxDC * dc, int state )
			{
				dc->DrawBitmap( m_bitmaps[m_index]
					, cell.x
					, cell.y
					, true );
			}

			void renderCategory( wxRect cell, wxDC * dc, int state )
			{
				int xOffset = 0;
				std::vector< TestStatus > valid;

				for ( int i = 1; i <= int( aria::TestStatus::eRunning_Begin ); ++i )
				{
					auto status = TestStatus( i );

					if ( getStatusCount( status, *m_statusName->counts ) > 0 )
					{
						valid.push_back( status );
					}
				}
				{
					auto status = TestStatus::eNotRun;

					if ( getStatusCount( status, *m_statusName->counts ) > 0 )
					{
						valid.push_back( status );
					}
				}

				if ( !valid.empty() )
				{
					auto nxt = valid.begin();
					auto cur = nxt++;
					auto curColor = state
						? wxSystemSettings::GetColour( wxSystemColour::wxSYS_COLOUR_GRADIENTACTIVECAPTION )
						: PANEL_FOREGROUND_COLOUR;
					auto nxtColor = getStatusColor( *cur );
					auto curWidth = cell.GetSize().x / 4;
					auto gradWidth = std::min( 50, curWidth / 4 );
					auto totalWidth = cell.width;

					curWidth -= gradWidth;
					dc->GradientFillLinear( wxRect{ cell.x + xOffset, cell.y, curWidth, cell.height }, curColor, curColor );
					xOffset += curWidth;
					auto halfColour = mix( 0.5f, curColor, nxtColor );
					dc->GradientFillLinear( wxRect{ cell.x + xOffset, cell.y, gradWidth, cell.height }, curColor, halfColour );
					xOffset += gradWidth;
					totalWidth -= xOffset;

					while ( nxt != valid.end() )
					{
						curColor = getStatusColor( *cur );
						nxtColor = getStatusColor( *nxt );
						curWidth = getStatusSize( *cur, totalWidth, *m_statusName->counts );
						gradWidth = std::min( 50, curWidth / 4 );
						curWidth -= gradWidth * 2;

						dc->GradientFillLinear( wxRect{ cell.x + xOffset, cell.y, gradWidth, cell.height }, halfColour, curColor );
						xOffset += gradWidth;
						dc->GradientFillLinear( wxRect{ cell.x + xOffset, cell.y, curWidth, cell.height }, curColor, curColor );
						xOffset += curWidth;
						halfColour = mix( 0.5f, curColor, nxtColor );
						dc->GradientFillLinear( wxRect{ cell.x + xOffset, cell.y, gradWidth, cell.height }, curColor, halfColour );
						xOffset += gradWidth;

						cur = nxt++;
					}

					curColor = getStatusColor( *cur );
					curWidth = cell.width - xOffset;
					gradWidth = std::min( 50, curWidth / 4 );
					curWidth -= gradWidth;

					dc->GradientFillLinear( wxRect{ cell.x + xOffset, cell.y, gradWidth, cell.height }, halfColour, curColor );
					xOffset += gradWidth;
					dc->GradientFillLinear( wxRect{ cell.x + xOffset, cell.y, curWidth, cell.height }, curColor, curColor );
					xOffset += curWidth;
				}
			}

		private:
			wxDataViewCtrl * m_parent;
			wxSize m_size;
			wxBitmap m_outOfCastorDateBmp;
			wxBitmap m_outOfSceneDateBmp;
			std::array< wxBitmap, size_t( TestStatus::eCount ) + AdditionalIndices > m_bitmaps;
			StatusName * m_statusName{};
			bool m_isTest{ true };
			uint32_t m_index{};
			wxString m_name;
			bool m_outOfCastorDate{};
			bool m_outOfSceneDate{};
		};

		wxString getColumnName( TreeModel::Column col )
		{
			switch ( col )
			{
			case aria::TreeModel::Column::eStatusName:
				return wxT( "Name" );
			case aria::TreeModel::Column::eRunDate:
				return wxT( "Run Date" );
			case aria::TreeModel::Column::eRunTime:
				return wxT( "Run Time" );
			default:
				return wxT( "string" );
			}
		}

		wxString getColumnType( TreeModel::Column col )
		{
			switch ( col )
			{
			case TreeModel::Column::eStatusName:
				return DataViewTestStatusRenderer::GetDefaultType();
			default:
				return wxT( "string" );
			}
		}

		wxDataViewRenderer * getColumnRenderer( TreeModel::Column col
			, wxDataViewCtrl * view )
		{
			switch ( col )
			{
			case aria::TreeModel::Column::eStatusName:
				return new DataViewTestStatusRenderer{ view, getColumnType( col ) };
			default:
				return new wxDataViewTextRenderer{ getColumnType( col ) };
			}
		}
	}

	//*********************************************************************************************

	TreeModel::TreeModel( Config const & config
		, Renderer renderer
		, TestsCounts & counts )
		: m_config{ config }
		, m_renderer{ renderer }
		, m_root( new TreeModelNode{ renderer, counts } )
	{
	}

	TreeModel::~TreeModel()
	{
		delete m_root;
	}

	TreeModelNode * TreeModel::addCategory( Category category
		, TestsCounts & counts )
	{
		TreeModelNode * node = new TreeModelNode{ m_root, m_renderer, category, counts };
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

		for ( int i = 0; i < int( Column::eCount ); ++i )
		{
			auto column = Column( i );
			wxDataViewColumn * col = new wxDataViewColumn( getColumnName( column )
				, getColumnRenderer( column, view )
				, i
				, getColumnSize( column )
				, wxALIGN_LEFT, flags );
			col->SetMinWidth( getColumnSize( column ) );
			view->AppendColumn( col );
		}
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

		if ( node )
		{
			if ( node->test )
			{
				result = node->test->getName();
			}
			else if ( node->category )
			{
				result = node->category->name;
			}
			else if ( node->renderer )
			{
				result = node->renderer->name;
			}
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

		if ( node )
		{
			if ( node->renderer )
			{
				result = node->renderer->name;
			}
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
			wxString str1 = getName( item1 );
			wxString str2 = getName( item2 );
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

			result = ascending
				? result
				: -result;
		}
		else if ( column == int( Column::eStatusName ) )
		{
			auto node1 = static_cast< TreeModelNode const * >( item1.GetID() );
			auto node2 = static_cast< TreeModelNode const * >( item2.GetID() );
			result = ( node1->test->getStatus() < node2->test->getStatus()
				? -1
				: ( node1->test->getStatus() == node2->test->getStatus()
					? makeWxString( node1->test->getName() ).Cmp( node2->test->getName() )
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
		return getColumnType( Column( col ) );
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
			case Column::eStatusName:
				node->statusName.statusIndex = StatusName::getTestStatusIndex( m_config, **node->test );
				node->statusName.name = getName( item );
				variant = reinterpret_cast< void * >( &node->statusName );
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

			default:
				castor::Logger::logError( "TreeModel::GetValue: wrong column " + castor::string::toString( col ) );
				break;
			}
		}
		else
		{
			switch ( Column( col ) )
			{
			case Column::eStatusName:
				node->statusName.statusIndex = StatusName::getStatusIndex( true, TestStatus::eNotRun );
				node->statusName.name = getName( item );
				//node->statusName.counts->update( {}, m_config );
				variant = reinterpret_cast< void * >( &node->statusName );
				break;

			case Column::eRunDate:
			case Column::eRunTime:
				variant = wxEmptyString;
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
		case Column::eStatusName:
		case Column::eRunDate:
		case Column::eRunTime:
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

	bool TreeModel::HasContainerColumns( const wxDataViewItem & item )const
	{
		return false;
	}

	//*********************************************************************************************
}
