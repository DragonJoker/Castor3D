#include "CastorTestParser/Model/TreeModel.hpp"
#include "CastorTestParser/Model/TreeModelNode.hpp"

#include "CastorTestParser/xpms/acceptable.xpm"
#include "CastorTestParser/xpms/negligible.xpm"
#include "CastorTestParser/xpms/notrun.xpm"
#include "CastorTestParser/xpms/running.xpm"
#include "CastorTestParser/xpms/unacceptable.xpm"
#include "CastorTestParser/xpms/unprocessed.xpm"

#include <CastorUtils/Log/Logger.hpp>

namespace test_parser
{
	namespace
	{
		wxImage createImage( char const * const * xpmData )
		{
			wxImage result{ xpmData };
			return result;
		}
	}

	TreeModel::TreeModel()
		: m_root( new TreeModelNode{ nullptr, _( "Tests database" ) } )
		, m_statusImages
		{
			createImage( notrun_xpm ),
			createImage( negligible_xpm ),
			createImage( acceptable_xpm ),
			createImage( unacceptable_xpm ),
			createImage( unprocessed_xpm ),
			createImage( running_xpm ),
		}
	{
	}

	TreeModel::~TreeModel()
	{
		delete m_root;
	}

	uint32_t TreeModel::getId( wxDataViewItem const & item )const
	{
		uint32_t result{};
		auto node = static_cast< TreeModelNode * >( item.GetID() );

		if ( node && node->test )
		{
			result = node->test->id;
		}

		return result;
	}

	std::string TreeModel::getName( wxDataViewItem const & item )const
	{
		std::string result{};
		auto node = static_cast< TreeModelNode * >( item.GetID() );

		if ( node && node->test )
		{
			result = node->test->name;
		}

		return result;
	}

	db::DateTime TreeModel::getRunDate( wxDataViewItem const & item )const
	{
		db::DateTime result{ boost::posix_time::not_a_date_time };
		auto node = static_cast< TreeModelNode * >( item.GetID() );

		if ( node && node->test )
		{
			result = node->test->runDate;
		}

		return result;
	}

	TestStatus TreeModel::getStatus( wxDataViewItem const & item )const
	{
		TestStatus result{};
		auto node = static_cast< TreeModelNode * >( item.GetID() );

		if ( node && node->test )
		{
			result = node->test->status;
		}

		return result;
	}

	std::string TreeModel::getRenderer( wxDataViewItem const & item )const
	{
		std::string result{};
		auto node = static_cast< TreeModelNode * >( item.GetID() );

		if ( node && node->test )
		{
			result = node->test->renderer;
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
				result = node->test->category;
			}
			else
			{
				result = makeStdString( node->category );
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
			result = ( node1->test->status < node2->test->status
				? -1
				: ( node1->test->status == node2->test->status
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
		return 5;
	}

	wxString TreeModel::GetColumnType( unsigned int col )const
	{
		switch ( Column( col ) )
		{
		case Column::eRunDate:
			return wxT( "datetime" );
		case Column::eStatus:
			return wxT( "wxBitmap" );
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
				variant = makeWxString( node->test->category );
				break;

			case Column::eName:
				variant = makeWxString( node->test->name );
				break;

			case Column::eRenderer:
				variant = makeWxString( node->test->renderer );
				break;

			case Column::eRunDate:
				variant = makeWxDateTime( node->test->runDate );
				break;

			case Column::eStatus:
				variant << m_statusImages[std::min( long( TestStatus::eCount ) - 1, long( node->test->status ) )];
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
				variant = node->category;
				break;

			case Column::eName:
			case Column::eRenderer:
			case Column::eRunDate:
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

		if ( node->test )
		{
			switch ( Column( col ) )
			{
			case Column::eCategory:
				node->test->category = makeStdString( variant.GetString() );
				result = true;
				break;

			case Column::eName:
				node->test->name = makeStdString( variant.GetString() );
				result = true;
				break;

			case Column::eRenderer:
				node->test->renderer = makeStdString( variant.GetString() );
				result = true;
				break;

			case Column::eRunDate:
				node->test->runDate = makeDbDateTime( variant.GetDateTime() );
				result = true;
				break;

			case Column::eStatus:
				//node->test->status = int( variant.GetLong() );
				result = true;
				break;

			default:
				castor::Logger::logError( "TreeModel::SetValue: wrong column " + castor::string::toString( col ) );
				break;
			}
		}
		else
		{
			switch ( Column( col ) )
			{
			case Column::eCategory:
				node->category = variant.GetString();
				result = true;
				break;

			case Column::eName:
			case Column::eRenderer:
			case Column::eRunDate:
			case Column::eStatus:
				break;

			default:
				castor::Logger::logError( "TreeModel::SetValue: wrong column " + castor::string::toString( col ) );
				break;
			}
		}

		return result;
	}

	wxDataViewItem TreeModel::GetParent( wxDataViewItem const & item )const
	{
		wxDataViewItem result( 0 );

		// the invisible root node has no parent
		if ( item.IsOk() )
		{
			auto node = static_cast< TreeModelNode * >( item.GetID() );

			if ( node && node != m_root )
			{
				result = wxDataViewItem{ node->GetParent() };
			}
		}

		return result;
	}

	bool TreeModel::IsContainer( wxDataViewItem const & item )const
	{
		bool result = true;
		// the invisble root node can have children
		// (in our model always "MyMusic")
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
