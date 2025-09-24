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
		: public c3d::ConfigurationVisitor
	{
		struct Token
		{
		private:
			friend struct TreeItemConfigurationBuilder;
			explicit Token()noexcept = default;
		};

	public:
		explicit TreeItemConfigurationBuilder( [[maybe_unused]] Token token
			, wxPropertyGrid * grid
			, TreeItemProperty & prop
			, wxPGProperty * subgrid = nullptr )
			: c3d::ConfigurationVisitor{}
			, m_grid{ grid }
			, m_subgrid{ subgrid }
			, m_prop{ prop }
		{
		}

		template< typename ConfigT, typename ... ParamsT >
		static void submit( wxPropertyGrid * grid
			, TreeItemProperty & prop
			, ConfigT & config
			, ParamsT && ... params )
		{
			TreeItemConfigurationBuilder vis{ Token{}, grid, prop };
			config.accept( vis, c3d::forward< ParamsT >( params )... );
		}

	private:
		template< typename ValueT, typename ControlT >
		void doVisit( c3d::String const & name
			, ValueT & value
			, ControlT controls )
		{
			if ( m_subgrid )
			{
				m_prop.addPropertyT( m_subgrid, name, &value, c3d::move( controls ) );
			}
			else
			{
				m_prop.addPropertyT( m_grid, name, &value, c3d::move( controls ) );
			}
		}

		template< typename ControlT >
		void doVisit( c3d::String const & name
			, c3d::ColourWrapper value
			, ControlT controls )
		{
			if ( m_subgrid )
			{
				m_prop.addProperty( m_subgrid, name, value, c3d::move( controls ) );
			}
			else
			{
				m_prop.addProperty( m_grid, name, value, c3d::move( controls ) );
			}
		}

		template< typename ValueT, typename ControlT >
		void doVisit( c3d::String const & name
			, ValueT & enumValue
			, c3d::StringArray const & enumNames
			, OnEnumValueChangeT< ValueT > onChange
			, ControlT controls )
		{
			if ( m_subgrid )
			{
				m_prop.addPropertyET( m_subgrid, name, make_wxArrayString( enumNames ), &enumValue, c3d::move( controls ), c3d::move( onChange ) );
			}
			else
			{
				m_prop.addPropertyET( m_grid, name, make_wxArrayString( enumNames ), &enumValue, c3d::move( controls ), c3d::move( onChange ) );
			}
		}

		void visit( c3d::String const & name
			, bool & value
			, ControlsList controls )override
		{
			doVisit( name, value, c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, int16_t & value
			, ControlsList controls )override
		{
			doVisit( name, value, c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, uint16_t & value
			, ControlsList controls )override
		{
			doVisit( name, value, c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, int32_t & value
			, ControlsList controls )override
		{
			doVisit( name, value, c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, uint32_t & value
			, ControlsList controls )override
		{
			doVisit( name, value, c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, int64_t & value
			, ControlsList controls )override
		{
			doVisit( name, value, c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, uint64_t & value
			, ControlsList controls )override
		{
			doVisit( name, value, c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, float & value
			, ControlsList controls )override
		{
			doVisit( name, value, c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, double & value
			, ControlsList controls )override
		{
			doVisit( name, value, c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, c3d::Angle & value
			, ControlsList controls )override
		{
			doVisit( name, value, c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, c3d::LuminousIntensity & value
			, ControlsList controls )override
		{
			doVisit( name, value, c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, c3d::Illumination & value
			, ControlsList controls )override
		{
			doVisit( name, value, c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, c3d::ColourWrapper value
			, ControlsList controls )override
		{
			doVisit( name, value, c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, c3d::Point2f & value
			, ControlsList controls )override
		{
			doVisit( name, value, c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, c3d::Point2i & value
			, ControlsList controls )override
		{
			doVisit( name, value, c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, c3d::Point2ui & value
			, ControlsList controls )override
		{
			doVisit( name, value, c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, c3d::Point3f & value
			, ControlsList controls )override
		{
			doVisit( name, value, c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, c3d::Point3i & value
			, ControlsList controls )override
		{
			doVisit( name, value, c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, c3d::Point3ui & value
			, ControlsList controls )override
		{
			doVisit( name, value, c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, c3d::Point4f & value
			, ControlsList controls )override
		{
			doVisit( name, value, c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, c3d::Point4i & value
			, ControlsList controls )override
		{
			doVisit( name, value, c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, c3d::Point4ui & value
			, ControlsList controls )override
		{
			doVisit( name, value, c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, c3d::Matrix4x4f & value
			, ControlsList controls )override
		{
			doVisit( name, value, c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, c3d::RangedValue< float > & value
			, ControlsList controls )override
		{
			doVisit( name, value, c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, c3d::RangedValue< int32_t > & value
			, ControlsList controls )override
		{
			doVisit( name, value, c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, c3d::RangedValue< uint32_t > & value
			, ControlsList controls )override
		{
			doVisit( name, value, c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, int32_t & enumValue
			, c3d::StringArray const & enumNames
			, OnSEnumValueChange onChange
			, ControlsList controls )override
		{
			doVisit( name, enumValue, enumNames, c3d::move( onChange ), c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, uint32_t & enumValue
			, c3d::StringArray const & enumNames
			, OnUEnumValueChange onChange
			, ControlsList controls )override
		{
			doVisit( name, enumValue, enumNames, c3d::move( onChange ), c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, bool & value
			, AtomicControlsList controls )override
		{
			doVisit( name, value, c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, int16_t & value
			, AtomicControlsList controls )override
		{
			doVisit( name, value, c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, uint16_t & value
			, AtomicControlsList controls )override
		{
			doVisit( name, value, c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, int32_t & value
			, AtomicControlsList controls )override
		{
			doVisit( name, value, c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, uint32_t & value
			, AtomicControlsList controls )override
		{
			doVisit( name, value, c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, int64_t & value
			, AtomicControlsList controls )override
		{
			doVisit( name, value, c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, uint64_t & value
			, AtomicControlsList controls )override
		{
			doVisit( name, value, c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, float & value
			, AtomicControlsList controls )override
		{
			doVisit( name, value, c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, double & value
			, AtomicControlsList controls )override
		{
			doVisit( name, value, c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, c3d::Angle & value
			, AtomicControlsList controls )override
		{
			doVisit( name, value, c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, c3d::LuminousIntensity & value
			, AtomicControlsList controls )override
		{
			doVisit( name, value, c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, c3d::Illumination & value
			, AtomicControlsList controls )override
		{
			doVisit( name, value, c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, c3d::ColourWrapper value
			, AtomicControlsList controls )override
		{
			doVisit( name, value, c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, c3d::Point2f & value
			, AtomicControlsList controls )override
		{
			doVisit( name, value, c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, c3d::Point2i & value
			, AtomicControlsList controls )override
		{
			doVisit( name, value, c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, c3d::Point2ui & value
			, AtomicControlsList controls )override
		{
			doVisit( name, value, c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, c3d::Point3f & value
			, AtomicControlsList controls )override
		{
			doVisit( name, value, c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, c3d::Point3i & value
			, AtomicControlsList controls )override
		{
			doVisit( name, value, c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, c3d::Point3ui & value
			, AtomicControlsList controls )override
		{
			doVisit( name, value, c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, c3d::Point4f & value
			, AtomicControlsList controls )override
		{
			doVisit( name, value, c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, c3d::Point4i & value
			, AtomicControlsList controls )override
		{
			doVisit( name, value, c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, c3d::Point4ui & value
			, AtomicControlsList controls )override
		{
			doVisit( name, value, c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, c3d::Matrix4x4f & value
			, AtomicControlsList controls )override
		{
			doVisit( name, value, c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, c3d::RangedValue< float > & value
			, AtomicControlsList controls )override
		{
			doVisit( name, value, c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, c3d::RangedValue< int32_t > & value
			, AtomicControlsList controls )override
		{
			doVisit( name, value, c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, c3d::RangedValue< uint32_t > & value
			, AtomicControlsList controls )override
		{
			doVisit( name, value, c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, int32_t & enumValue
			, c3d::StringArray const & enumNames
			, OnSEnumValueChange onChange
			, AtomicControlsList controls )override
		{
			doVisit( name, enumValue, enumNames, c3d::move( onChange ), c3d::move( controls ) );
		}

		void visit( c3d::String const & name
			, uint32_t & enumValue
			, c3d::StringArray const & enumNames
			, OnUEnumValueChange onChange
			, AtomicControlsList controls )override
		{
			doVisit( name, enumValue, enumNames, c3d::move( onChange ), c3d::move( controls ) );
		}

	private:
		c3d::RawUniquePtr< ConfigurationVisitorBase > doGetSubConfiguration( c3d::String const & category )override
		{
			return c3d::makeRawUnique< TreeItemConfigurationBuilder >( Token{}
				, m_grid, m_prop
				, ( m_subgrid
					? m_prop.addProperty( m_subgrid, category )
					: m_prop.addProperty( m_grid, category ) ) );
		}

	private:
		wxPropertyGrid * m_grid{};
		wxPGProperty * m_subgrid{};
		TreeItemProperty & m_prop;
	};
}

#pragma GCC diagnostic pop

#endif
