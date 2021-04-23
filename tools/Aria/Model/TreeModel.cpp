#include "Aria/Model/TreeModel.hpp"

#include "Aria/DatabaseTest.hpp"
#include "Aria/TestDatabase.hpp"
#include "Aria/TestsCounts.hpp"
#include "Aria/Database/DbDateTimeHelpers.hpp"
#include "Aria/Model/DataViewTestStatusRenderer.hpp"
#include "Aria/Model/TreeModelNode.hpp"

#include <CastorUtils/Exception/Assertion.hpp>
#include <CastorUtils/Log/Logger.hpp>

#include <wx/dc.h>

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

		int getColumnMinSize( TreeModel::Column col
			, int maxWidth )
		{
			switch ( col )
			{
			case aria::TreeModel::Column::eStatusName:
				return maxWidth
					- getColumnMinSize( TreeModel::Column::eRunDate, maxWidth )
					- getColumnMinSize( TreeModel::Column::eRunTime, maxWidth );
			case aria::TreeModel::Column::eRunDate:
				return 80;
			case aria::TreeModel::Column::eRunTime:
				return 90;
			default:
				return 100;
			}
		}

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
		, RendererTestsCounts & counts )
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
		, CategoryTestsCounts & counts )
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

	void TreeModel::resize( wxDataViewCtrl * view
		, wxSize const & size )
	{
		for ( int i = 0; i < int( Column::eCount ); ++i )
		{
			auto column = Column( i );
			auto col = view->GetColumn( i );
			col->SetMinWidth( getColumnMinSize( column, size.GetWidth() ) );
			col->SetWidth( getColumnMinSize( column, size.GetWidth() ) );
		}

		view->Refresh();
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
				node->statusName.categoryCounts = &node->test->getCounts();
				node->statusName.status = node->test->getStatus();
				node->statusName.outOfCastorDate = node->test->checkOutOfCastorDate();
				node->statusName.outOfSceneDate = node->test->checkOutOfSceneDate();
				node->statusName.ignored = node->test->getIgnoreResult();
				node->statusName.name = getName( item );
				variant = reinterpret_cast< void * >( &node->statusName );
				break;

			case Column::eRunDate:
				variant = ( db::date_time::isValid( node->test->getRunDate() ) && db::date::isValid( node->test->getRunDate().date() ) )
					? makeWxString( db::date::print( node->test->getRunDate().date(), DISPLAY_DATETIME_DATE ) )
					: wxString{};
				break;

			case Column::eRunTime:
				variant = ( db::date_time::isValid( node->test->getRunDate() ) && db::time::isValid( node->test->getRunDate().time_of_day() ) )
					? makeWxString( db::time::print( node->test->getRunDate().time_of_day(), DISPLAY_DATETIME_TIME ) )
					: wxString{};
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
				if ( node->category )
				{
					node->statusName.status = ( ( node->categoryCounts->getValue( TestsCountsType::eRunning ) != 0 )
						? TestStatus::eRunning_Begin
						: ( ( node->categoryCounts->getValue( TestsCountsType::ePending ) != 0 )
							? TestStatus::ePending
							: ( ( node->categoryCounts->getValue( TestsCountsType::eUnacceptable ) != 0 )
								? TestStatus::eUnacceptable
								: ( ( node->categoryCounts->getValue( TestsCountsType::eAcceptable ) != 0 )
									? TestStatus::eAcceptable
									: ( ( node->categoryCounts->getValue( TestsCountsType::eNegligible ) > node->categoryCounts->getValue( TestsCountsType::eNotRun ) )
										? TestStatus::eNegligible
										: TestStatus::eNotRun ) ) ) ) );
					node->statusName.outOfCastorDate = ( node->categoryCounts->getValue( TestsCountsType::eOutdated ) != 0 );
				}
				else if ( node->renderer )
				{
					node->statusName.status = ( ( node->rendererCounts->getValue( TestsCountsType::eRunning ) != 0 )
						? TestStatus::eRunning_Begin
						: ( ( node->rendererCounts->getValue( TestsCountsType::ePending ) != 0 )
							? TestStatus::ePending
							: ( ( node->rendererCounts->getValue( TestsCountsType::eUnacceptable ) != 0 )
								? TestStatus::eUnacceptable
								: ( ( node->rendererCounts->getValue( TestsCountsType::eAcceptable ) != 0 )
									? TestStatus::eAcceptable
									: ( ( node->rendererCounts->getValue( TestsCountsType::eNegligible ) > node->rendererCounts->getValue( TestsCountsType::eNotRun ) )
										? TestStatus::eNegligible
										: TestStatus::eNotRun ) ) ) ) );
					node->statusName.outOfCastorDate = ( node->rendererCounts->getValue( TestsCountsType::eOutdated ) != 0 );
				}
				else
				{
					node->statusName.status = ( ( node->allCounts->getValue( TestsCountsType::eRunning ) != 0 )
						? TestStatus::eRunning_Begin
						: ( ( node->allCounts->getValue( TestsCountsType::ePending ) != 0 )
							? TestStatus::ePending
							: ( ( node->allCounts->getValue( TestsCountsType::eUnacceptable ) != 0 )
								? TestStatus::eUnacceptable
								: ( ( node->allCounts->getValue( TestsCountsType::eAcceptable ) != 0 )
									? TestStatus::eAcceptable
									: ( ( node->allCounts->getValue( TestsCountsType::eNegligible ) > node->allCounts->getValue( TestsCountsType::eNotRun ) )
										? TestStatus::eNegligible
										: TestStatus::eNotRun ) ) ) ) );
					node->statusName.outOfCastorDate = ( node->allCounts->getValue( TestsCountsType::eOutdated ) != 0 );
				}
				node->statusName.outOfSceneDate = false;
				node->statusName.ignored = false;
				node->statusName.name = getName( item );
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
