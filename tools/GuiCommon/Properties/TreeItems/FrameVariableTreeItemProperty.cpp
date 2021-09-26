#include "GuiCommon/Properties/TreeItems/FrameVariableTreeItemProperty.hpp"

#include "GuiCommon/Properties/Math/PointProperties.hpp"
#include "GuiCommon/Properties/Math/MatrixProperties.hpp"

#include <Castor3D/Render/RenderSystem.hpp>

#include <CastorUtils/Design/ChangeTracked.hpp>

#include <wx/propgrid/advprops.h>

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	namespace
	{
		template< typename ValueT >
		using RawValueT = ValueT;

		template< template <typename ValueT> typename HolderT >
		void addRangedValueProperty( TreeItemProperty * property
			, wxPropertyGrid * grid
			, wxString const & name
			, UniformValueBase & uniform )
		{
			switch ( uniform.getType() )
			{
			case UniformType::eInt:
				property->addPropertyT( grid, name, &static_cast< UniformValue< HolderT< castor::RangedValue< int32_t > > > & >( uniform ).getValue() );
				break;
			case UniformType::eUInt:
				property->addPropertyT( grid, name, &static_cast< UniformValue< HolderT< castor::RangedValue< uint32_t > > > & >( uniform ).getValue() );
				break;
			case UniformType::eFloat:
				property->addPropertyT( grid, name, &static_cast< UniformValue< HolderT< castor::RangedValue< float > > > & >( uniform ).getValue() );
				break;
			default:
				CU_Failure( "Unsupported UniformType" );
				break;
			}
		}

		template< template <typename ValueT> typename HolderT >
		void addValueProperty( TreeItemProperty * property
			, wxPropertyGrid * grid
			, wxString const & name
			, UniformValueBase & uniform )
		{
			switch ( uniform.getType() )
			{
			case UniformType::eInt:
				property->addPropertyT( grid, name, &static_cast< UniformValue< HolderT< int32_t > > & >( uniform ).getValue() );
				break;
			case UniformType::eUInt:
				property->addPropertyT( grid, name, &static_cast< UniformValue< HolderT< uint32_t > > & >( uniform ).getValue() );
				break;
			case UniformType::eFloat:
				property->addPropertyT( grid, name, &static_cast< UniformValue< HolderT< float > > & >( uniform ).getValue() );
				break;
			case UniformType::eVec2f:
				property->addPropertyT( grid, name, &static_cast< UniformValue< HolderT< Point2f > > & >( uniform ).getValue() );
				break;
			case UniformType::eVec3f:
				property->addPropertyT( grid, name, &static_cast< UniformValue< HolderT< Point3f > > & >( uniform ).getValue() );
				break;
			case UniformType::eVec4f:
				property->addPropertyT( grid, name, &static_cast< UniformValue< HolderT< Point4f > > & >( uniform ).getValue() );
				break;
			case UniformType::eVec2i:
				property->addPropertyT( grid, name, &static_cast< UniformValue< HolderT< Point2i > > & >( uniform ).getValue() );
				break;
			case UniformType::eVec3i:
				property->addPropertyT( grid, name, &static_cast< UniformValue< HolderT< Point3i > > & >( uniform ).getValue() );
				break;
			case UniformType::eVec4i:
				property->addPropertyT( grid, name, &static_cast< UniformValue< HolderT< Point4i > > & >( uniform ).getValue() );
				break;
			case UniformType::eVec2ui:
				property->addPropertyT( grid, name, &static_cast< UniformValue< HolderT< Point2ui > > & >( uniform ).getValue() );
				break;
			case UniformType::eVec3ui:
				property->addPropertyT( grid, name, &static_cast< UniformValue< HolderT< Point3ui > > & >( uniform ).getValue() );
				break;
			case UniformType::eVec4ui:
				property->addPropertyT( grid, name, &static_cast< UniformValue< HolderT< Point4ui > > & >( uniform ).getValue() );
				break;
			case UniformType::eMat4f:
				property->addPropertyT( grid, name, &static_cast< UniformValue< HolderT< Matrix4x4f > > & >( uniform ).getValue() );
				break;
			default:
				CU_Failure( "Unsupported UniformType" );
				break;
			}
		}
	}

	FrameVariableTreeItemProperty::FrameVariableTreeItemProperty( Engine * engine
		, bool editable
		, UniformValueBase & uniform )
		: TreeItemProperty{ engine, editable }
		, m_uniform{ uniform }
	{
		CreateTreeItemMenu();
	}

	void FrameVariableTreeItemProperty::doCreateProperties( wxPGEditor * editor
		, wxPropertyGrid * grid )
	{
		static wxString PROPERTY_CATEGORY_VARIABLE = _( "Uniform:" );
		static wxString PROPERTY_VALUE = _( "Value" );
		static wxString PROPERTY_TYPE = _( "Type" );

		wxString displayName = m_uniform.getName();

		addProperty( grid, PROPERTY_CATEGORY_VARIABLE + wxT( " " ) + displayName );
		auto * prop = grid->Append( new wxStringProperty( PROPERTY_TYPE ) );
		prop->SetValue( getName( m_uniform.getType() ) );
		prop->Enable( false );

		if ( m_uniform.isTracked() )
		{
			if ( m_uniform.isRanged() )
			{
				addRangedValueProperty< castor::ChangeTracked >( this, grid, PROPERTY_VALUE, m_uniform );
			}
			else
			{
				addValueProperty< castor::ChangeTracked >( this, grid, PROPERTY_VALUE, m_uniform );
			}
		}
		else
		{
			if ( m_uniform.isRanged() )
			{
				addRangedValueProperty< RawValueT >( this, grid, PROPERTY_VALUE, m_uniform );
			}
			else
			{
				addValueProperty< RawValueT >( this, grid, PROPERTY_VALUE, m_uniform );
			}
		}
	}
}
