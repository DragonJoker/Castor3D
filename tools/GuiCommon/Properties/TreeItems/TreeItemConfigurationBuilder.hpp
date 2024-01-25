/*
See LICENSE file in root folder
*/
#ifndef ___GC_TreeItemConfigurationBuilder_HPP___
#define ___GC_TreeItemConfigurationBuilder_HPP___

#include "GuiCommon/Properties/TreeItems/TreeItemProperty.hpp"

#include <Castor3D/Miscellaneous/ConfigurationVisitor.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual"

namespace GuiCommon
{
	struct TreeItemConfigurationBuilder
		: public castor3d::ConfigurationVisitor
	{
	private:
		explicit TreeItemConfigurationBuilder( wxPropertyGrid * grid
			, TreeItemProperty & prop
			, wxPGProperty * subgrid = nullptr )
			: castor3d::ConfigurationVisitor{}
			, m_grid{ grid }
			, m_subgrid{ subgrid }
			, m_prop{ prop }
		{
		}

	public:
		template< typename ConfigT, typename ... ParamsT >
		static void submit( wxPropertyGrid * grid
			, TreeItemProperty & prop
			, ConfigT & config
			, ParamsT && ... params )
		{
			TreeItemConfigurationBuilder vis{ grid, prop };
			config.accept( vis, castor::forward< ParamsT >( params )... );
		}

	private:
		template< typename ValueT, typename ControlT >
		void doVisit( castor::String const & name
			, ValueT & value
			, ControlT controls )
		{
			if ( m_subgrid )
			{
				m_prop.addPropertyT( m_subgrid, name, &value, castor::move( controls ) );
			}
			else
			{
				m_prop.addPropertyT( m_grid, name, &value, castor::move( controls ) );
			}
		}

		template< typename ControlT >
		void doVisit( castor::String const & name
			, castor3d::ColourWrapper value
			, ControlT controls )
		{
			if ( m_subgrid )
			{
				m_prop.addProperty( m_subgrid, name, value, castor::move( controls ) );
			}
			else
			{
				m_prop.addProperty( m_grid, name, value, castor::move( controls ) );
			}
		}

		template< typename ValueT, typename ControlT >
		void doVisit( castor::String const & name
			, ValueT & enumValue
			, castor::StringArray const & enumNames
			, OnEnumValueChangeT< ValueT > onChange
			, ControlT controls )
		{
			if ( m_subgrid )
			{
				m_prop.addPropertyET( m_subgrid, name, make_wxArrayString( enumNames ), &enumValue, castor::move( controls ), onChange );
			}
			else
			{
				m_prop.addPropertyET( m_grid, name, make_wxArrayString( enumNames ), &enumValue, castor::move( controls ), onChange );
			}
		}

		void visit( castor::String const & name
			, bool & value
			, ControlsList controls )override
		{
			doVisit( name, value, castor::move( controls ) );
		}

		void visit( castor::String const & name
			, int16_t & value
			, ControlsList controls )override
		{
			doVisit( name, value, castor::move( controls ) );
		}

		void visit( castor::String const & name
			, uint16_t & value
			, ControlsList controls )override
		{
			doVisit( name, value, castor::move( controls ) );
		}

		void visit( castor::String const & name
			, int32_t & value
			, ControlsList controls )override
		{
			doVisit( name, value, castor::move( controls ) );
		}

		void visit( castor::String const & name
			, uint32_t & value
			, ControlsList controls )override
		{
			doVisit( name, value, castor::move( controls ) );
		}

		void visit( castor::String const & name
			, int64_t & value
			, ControlsList controls )override
		{
			doVisit( name, value, castor::move( controls ) );
		}

		void visit( castor::String const & name
			, uint64_t & value
			, ControlsList controls )override
		{
			doVisit( name, value, castor::move( controls ) );
		}

		void visit( castor::String const & name
			, float & value
			, ControlsList controls )override
		{
			doVisit( name, value, castor::move( controls ) );
		}

		void visit( castor::String const & name
			, double & value
			, ControlsList controls )override
		{
			doVisit( name, value, castor::move( controls ) );
		}

		void visit( castor::String const & name
			, castor::Angle & value
			, ControlsList controls )override
		{
			doVisit( name, value, castor::move( controls ) );
		}

		void visit( castor::String const & name
			, castor3d::ColourWrapper value
			, ControlsList controls )override
		{
			doVisit( name, value, castor::move( controls ) );
		}

		void visit( castor::String const & name
			, castor::Point2f & value
			, ControlsList controls )override
		{
			doVisit( name, value, castor::move( controls ) );
		}

		void visit( castor::String const & name
			, castor::Point2i & value
			, ControlsList controls )override
		{
			doVisit( name, value, castor::move( controls ) );
		}

		void visit( castor::String const & name
			, castor::Point2ui & value
			, ControlsList controls )override
		{
			doVisit( name, value, castor::move( controls ) );
		}

		void visit( castor::String const & name
			, castor::Point3f & value
			, ControlsList controls )override
		{
			doVisit( name, value, castor::move( controls ) );
		}

		void visit( castor::String const & name
			, castor::Point3i & value
			, ControlsList controls )override
		{
			doVisit( name, value, castor::move( controls ) );
		}

		void visit( castor::String const & name
			, castor::Point3ui & value
			, ControlsList controls )override
		{
			doVisit( name, value, castor::move( controls ) );
		}

		void visit( castor::String const & name
			, castor::Point4f & value
			, ControlsList controls )override
		{
			doVisit( name, value, castor::move( controls ) );
		}

		void visit( castor::String const & name
			, castor::Point4i & value
			, ControlsList controls )override
		{
			doVisit( name, value, castor::move( controls ) );
		}

		void visit( castor::String const & name
			, castor::Point4ui & value
			, ControlsList controls )override
		{
			doVisit( name, value, castor::move( controls ) );
		}

		void visit( castor::String const & name
			, castor::Matrix4x4f & value
			, ControlsList controls )override
		{
			doVisit( name, value, castor::move( controls ) );
		}

		void visit( castor::String const & name
			, castor::RangedValue< float > & value
			, ControlsList controls )override
		{
			doVisit( name, value, castor::move( controls ) );
		}

		void visit( castor::String const & name
			, castor::RangedValue< int32_t > & value
			, ControlsList controls )override
		{
			doVisit( name, value, castor::move( controls ) );
		}

		void visit( castor::String const & name
			, castor::RangedValue< uint32_t > & value
			, ControlsList controls )override
		{
			doVisit( name, value, castor::move( controls ) );
		}

		void visit( castor::String const & name
			, int32_t & enumValue
			, castor::StringArray const & enumNames
			, OnSEnumValueChange onChange
			, ControlsList controls )override
		{
			doVisit( name, enumValue, enumNames, onChange, castor::move( controls ) );
		}

		void visit( castor::String const & name
			, uint32_t & enumValue
			, castor::StringArray const & enumNames
			, OnUEnumValueChange onChange
			, ControlsList controls )override
		{
			doVisit( name, enumValue, enumNames, onChange, castor::move( controls ) );
		}

		void visit( castor::String const & name
			, bool & value
			, AtomicControlsList controls )override
		{
			doVisit( name, value, castor::move( controls ) );
		}

		void visit( castor::String const & name
			, int16_t & value
			, AtomicControlsList controls )override
		{
			doVisit( name, value, castor::move( controls ) );
		}

		void visit( castor::String const & name
			, uint16_t & value
			, AtomicControlsList controls )override
		{
			doVisit( name, value, castor::move( controls ) );
		}

		void visit( castor::String const & name
			, int32_t & value
			, AtomicControlsList controls )override
		{
			doVisit( name, value, castor::move( controls ) );
		}

		void visit( castor::String const & name
			, uint32_t & value
			, AtomicControlsList controls )override
		{
			doVisit( name, value, castor::move( controls ) );
		}

		void visit( castor::String const & name
			, int64_t & value
			, AtomicControlsList controls )override
		{
			doVisit( name, value, castor::move( controls ) );
		}

		void visit( castor::String const & name
			, uint64_t & value
			, AtomicControlsList controls )override
		{
			doVisit( name, value, castor::move( controls ) );
		}

		void visit( castor::String const & name
			, float & value
			, AtomicControlsList controls )override
		{
			doVisit( name, value, castor::move( controls ) );
		}

		void visit( castor::String const & name
			, double & value
			, AtomicControlsList controls )override
		{
			doVisit( name, value, castor::move( controls ) );
		}

		void visit( castor::String const & name
			, castor::Angle & value
			, AtomicControlsList controls )override
		{
			doVisit( name, value, castor::move( controls ) );
		}

		void visit( castor::String const & name
			, castor3d::ColourWrapper value
			, AtomicControlsList controls )override
		{
			doVisit( name, value, castor::move( controls ) );
		}

		void visit( castor::String const & name
			, castor::Point2f & value
			, AtomicControlsList controls )override
		{
			doVisit( name, value, castor::move( controls ) );
		}

		void visit( castor::String const & name
			, castor::Point2i & value
			, AtomicControlsList controls )override
		{
			doVisit( name, value, castor::move( controls ) );
		}

		void visit( castor::String const & name
			, castor::Point2ui & value
			, AtomicControlsList controls )override
		{
			doVisit( name, value, castor::move( controls ) );
		}

		void visit( castor::String const & name
			, castor::Point3f & value
			, AtomicControlsList controls )override
		{
			doVisit( name, value, castor::move( controls ) );
		}

		void visit( castor::String const & name
			, castor::Point3i & value
			, AtomicControlsList controls )override
		{
			doVisit( name, value, castor::move( controls ) );
		}

		void visit( castor::String const & name
			, castor::Point3ui & value
			, AtomicControlsList controls )override
		{
			doVisit( name, value, castor::move( controls ) );
		}

		void visit( castor::String const & name
			, castor::Point4f & value
			, AtomicControlsList controls )override
		{
			doVisit( name, value, castor::move( controls ) );
		}

		void visit( castor::String const & name
			, castor::Point4i & value
			, AtomicControlsList controls )override
		{
			doVisit( name, value, castor::move( controls ) );
		}

		void visit( castor::String const & name
			, castor::Point4ui & value
			, AtomicControlsList controls )override
		{
			doVisit( name, value, castor::move( controls ) );
		}

		void visit( castor::String const & name
			, castor::Matrix4x4f & value
			, AtomicControlsList controls )override
		{
			doVisit( name, value, castor::move( controls ) );
		}

		void visit( castor::String const & name
			, castor::RangedValue< float > & value
			, AtomicControlsList controls )override
		{
			doVisit( name, value, castor::move( controls ) );
		}

		void visit( castor::String const & name
			, castor::RangedValue< int32_t > & value
			, AtomicControlsList controls )override
		{
			doVisit( name, value, castor::move( controls ) );
		}

		void visit( castor::String const & name
			, castor::RangedValue< uint32_t > & value
			, AtomicControlsList controls )override
		{
			doVisit( name, value, castor::move( controls ) );
		}

		void visit( castor::String const & name
			, int32_t & enumValue
			, castor::StringArray const & enumNames
			, OnSEnumValueChange onChange
			, AtomicControlsList controls )override
		{
			doVisit( name, enumValue, enumNames, onChange, castor::move( controls ) );
		}

		void visit( castor::String const & name
			, uint32_t & enumValue
			, castor::StringArray const & enumNames
			, OnUEnumValueChange onChange
			, AtomicControlsList controls )override
		{
			doVisit( name, enumValue, enumNames, onChange, castor::move( controls ) );
		}

	private:
		castor::RawUniquePtr< ConfigurationVisitorBase > doGetSubConfiguration( castor::String const & category )override
		{
			return castor::RawUniquePtr< ConfigurationVisitorBase >( new TreeItemConfigurationBuilder{ m_grid
				, m_prop
				, ( m_subgrid
					? m_prop.addProperty( m_subgrid, category )
					: m_prop.addProperty( m_grid, category ) ) } );
		}

	private:
		wxPropertyGrid * m_grid{};
		wxPGProperty * m_subgrid{};
		TreeItemProperty & m_prop;
	};
}

#pragma GCC diagnostic pop

#endif
