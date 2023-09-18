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
			, TreeItemProperty & prop )
			: castor3d::ConfigurationVisitor{}
			, m_grid{ grid }
			, m_prop{ prop }
		{
		}

	public:
		template< typename ConfigT >
		static void submit( wxPropertyGrid * grid
			, TreeItemProperty & prop
			, ConfigT & config )
		{
			TreeItemConfigurationBuilder vis{ grid, prop };
			config.accept( vis );
		}

	private:
		void visit( castor::String const & name
			, ControlsList controls )override
		{
			m_prop.addProperty( m_grid, name );
		}

		void visit( castor::String const & name
			, bool & value
			, ControlsList controls )override
		{
			m_prop.addPropertyT( m_grid, name, &value, std::move( controls ) );
		}

		void visit( castor::String const & name
			, float & value
			, ControlsList controls )override
		{
			m_prop.addPropertyT( m_grid, name, &value, std::move( controls ) );
		}

		void visit( castor::String const & name
			, int32_t & value
			, ControlsList controls )override
		{
			m_prop.addPropertyT( m_grid, name, &value, std::move( controls ) );
		}

		void visit( castor::String const & name
			, uint32_t & value
			, ControlsList controls )override
		{
			m_prop.addPropertyT( m_grid, name, &value, std::move( controls ) );
		}

		void visit( castor::String const & name
			, castor::Point2f & value
			, ControlsList controls )override
		{
			m_prop.addPropertyT( m_grid, name, &value, std::move( controls ) );
		}

		void visit( castor::String const & name
			, castor::Point2i & value
			, ControlsList controls )override
		{
			m_prop.addPropertyT( m_grid, name, &value, std::move( controls ) );
		}

		void visit( castor::String const & name
			, castor::Point2ui & value
			, ControlsList controls )override
		{
			m_prop.addPropertyT( m_grid, name, &value, std::move( controls ) );
		}

		void visit( castor::String const & name
			, castor::Point3f & value
			, ControlsList controls )override
		{
			m_prop.addPropertyT( m_grid, name, &value, std::move( controls ) );
		}

		void visit( castor::String const & name
			, castor::Point3i & value
			, ControlsList controls )override
		{
			m_prop.addPropertyT( m_grid, name, &value, std::move( controls ) );
		}

		void visit( castor::String const & name
			, castor::Point3ui & value
			, ControlsList controls )override
		{
			m_prop.addPropertyT( m_grid, name, &value, std::move( controls ) );
		}

		void visit( castor::String const & name
			, castor::Point4f & value
			, ControlsList controls )override
		{
			m_prop.addPropertyT( m_grid, name, &value, std::move( controls ) );
		}

		void visit( castor::String const & name
			, castor::Point4i & value
			, ControlsList controls )override
		{
			m_prop.addPropertyT( m_grid, name, &value, std::move( controls ) );
		}

		void visit( castor::String const & name
			, castor::Point4ui & value
			, ControlsList controls )override
		{
			m_prop.addPropertyT( m_grid, name, &value, std::move( controls ) );
		}

		void visit( castor::String const & name
			, castor::Matrix4x4f & value
			, ControlsList controls )override
		{
			m_prop.addPropertyT( m_grid, name, &value, std::move( controls ) );
		}

		void visit( castor::String const & name
			, castor::RangedValue< float > & value
			, ControlsList controls )override
		{
			m_prop.addPropertyT( m_grid, name, &value, std::move( controls ) );
		}

		void visit( castor::String const & name
			, castor::RangedValue< int32_t > & value
			, ControlsList controls )override
		{
			m_prop.addPropertyT( m_grid, name, &value, std::move( controls ) );
		}

		void visit( castor::String const & name
			, castor::RangedValue< uint32_t > & value
			, ControlsList controls )override
		{
			m_prop.addPropertyT( m_grid, name, &value, std::move( controls ) );
		}

		void visit( castor::String const & name
			, int32_t & enumValue
			, castor::StringArray const & enumNames
			, OnSEnumValueChange onChange
			, ControlsList controls )override
		{
			m_prop.addPropertyET( m_grid, name, make_wxArrayString( enumNames ), &enumValue, std::move( controls ), onChange );
		}

		void visit( castor::String const & name
			, uint32_t & enumValue
			, castor::StringArray const & enumNames
			, OnUEnumValueChange onChange
			, ControlsList controls )override
		{
			m_prop.addPropertyET( m_grid, name, make_wxArrayString( enumNames ), &enumValue, std::move( controls ), onChange );
		}

		void visit( castor::String const & name
			, AtomicControlsList controls )override
		{
			m_prop.addProperty( m_grid, name );
		}

		void visit( castor::String const & name
			, bool & value
			, AtomicControlsList controls )override
		{
			m_prop.addPropertyT( m_grid, name, &value, std::move( controls ) );
		}

		void visit( castor::String const & name
			, float & value
			, AtomicControlsList controls )override
		{
			m_prop.addPropertyT( m_grid, name, &value, std::move( controls ) );
		}

		void visit( castor::String const & name
			, int32_t & value
			, AtomicControlsList controls )override
		{
			m_prop.addPropertyT( m_grid, name, &value, std::move( controls ) );
		}

		void visit( castor::String const & name
			, uint32_t & value
			, AtomicControlsList controls )override
		{
			m_prop.addPropertyT( m_grid, name, &value, std::move( controls ) );
		}

		void visit( castor::String const & name
			, castor::Point2f & value
			, AtomicControlsList controls )override
		{
			m_prop.addPropertyT( m_grid, name, &value, std::move( controls ) );
		}

		void visit( castor::String const & name
			, castor::Point2i & value
			, AtomicControlsList controls )override
		{
			m_prop.addPropertyT( m_grid, name, &value, std::move( controls ) );
		}

		void visit( castor::String const & name
			, castor::Point2ui & value
			, AtomicControlsList controls )override
		{
			m_prop.addPropertyT( m_grid, name, &value, std::move( controls ) );
		}

		void visit( castor::String const & name
			, castor::Point3f & value
			, AtomicControlsList controls )override
		{
			m_prop.addPropertyT( m_grid, name, &value, std::move( controls ) );
		}

		void visit( castor::String const & name
			, castor::Point3i & value
			, AtomicControlsList controls )override
		{
			m_prop.addPropertyT( m_grid, name, &value, std::move( controls ) );
		}

		void visit( castor::String const & name
			, castor::Point3ui & value
			, AtomicControlsList controls )override
		{
			m_prop.addPropertyT( m_grid, name, &value, std::move( controls ) );
		}

		void visit( castor::String const & name
			, castor::Point4f & value
			, AtomicControlsList controls )override
		{
			m_prop.addPropertyT( m_grid, name, &value, std::move( controls ) );
		}

		void visit( castor::String const & name
			, castor::Point4i & value
			, AtomicControlsList controls )override
		{
			m_prop.addPropertyT( m_grid, name, &value, std::move( controls ) );
		}

		void visit( castor::String const & name
			, castor::Point4ui & value
			, AtomicControlsList controls )override
		{
			m_prop.addPropertyT( m_grid, name, &value, std::move( controls ) );
		}

		void visit( castor::String const & name
			, castor::Matrix4x4f & value
			, AtomicControlsList controls )override
		{
			m_prop.addPropertyT( m_grid, name, &value, std::move( controls ) );
		}

		void visit( castor::String const & name
			, castor::RangedValue< float > & value
			, AtomicControlsList controls )override
		{
			m_prop.addPropertyT( m_grid, name, &value, std::move( controls ) );
		}

		void visit( castor::String const & name
			, castor::RangedValue< int32_t > & value
			, AtomicControlsList controls )override
		{
			m_prop.addPropertyT( m_grid, name, &value, std::move( controls ) );
		}

		void visit( castor::String const & name
			, castor::RangedValue< uint32_t > & value
			, AtomicControlsList controls )override
		{
			m_prop.addPropertyT( m_grid, name, &value, std::move( controls ) );
		}

		void visit( castor::String const & name
			, int32_t & enumValue
			, castor::StringArray const & enumNames
			, OnSEnumValueChange onChange
			, AtomicControlsList controls )override
		{
			m_prop.addPropertyET( m_grid, name, make_wxArrayString( enumNames ), &enumValue, std::move( controls ), onChange );
		}

		void visit( castor::String const & name
			, uint32_t & enumValue
			, castor::StringArray const & enumNames
			, OnUEnumValueChange onChange
			, AtomicControlsList controls )override
		{
			m_prop.addPropertyET( m_grid, name, make_wxArrayString( enumNames ), &enumValue, std::move( controls ), onChange );
		}

	private:
		wxPropertyGrid * m_grid;
		TreeItemProperty & m_prop;
	};
}

#pragma GCC diagnostic pop

#endif
