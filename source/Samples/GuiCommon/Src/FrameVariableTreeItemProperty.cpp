#include "FrameVariableTreeItemProperty.hpp"

#include <Render/RenderSystem.hpp>
#include <Design/ChangeTracked.hpp>

#include "PointProperties.hpp"
#include "MatrixProperties.hpp"

#include <wx/propgrid/advprops.h>

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	namespace
	{
		static wxString PROPERTY_CATEGORY_VARIABLE = _( "Uniform:" );
		static wxString PROPERTY_NAME = _( "Name" );
		static wxString PROPERTY_VALUE = _( "Value" );
		static wxString PROPERTY_TYPE = _( "Type" );
		static wxString PROPERTY_TYPE_FLOAT = wxT( "float" );
		static wxString PROPERTY_TYPE_INT = wxT( "int" );
		static wxString PROPERTY_TYPE_UINT = wxT( "uint" );
		static wxString PROPERTY_TYPE_VEC2 = wxT( "vec2" );
		static wxString PROPERTY_TYPE_VEC3 = wxT( "vec3" );
		static wxString PROPERTY_TYPE_VEC4 = wxT( "vec4" );
		static wxString PROPERTY_TYPE_IVEC2 = wxT( "ivec2" );
		static wxString PROPERTY_TYPE_IVEC3 = wxT( "ivec3" );
		static wxString PROPERTY_TYPE_IVEC4 = wxT( "ivec4" );
		static wxString PROPERTY_TYPE_UIVEC2 = wxT( "uivec2" );
		static wxString PROPERTY_TYPE_UIVEC3 = wxT( "uivec3" );
		static wxString PROPERTY_TYPE_UIVEC4 = wxT( "uivec4" );
		static wxString PROPERTY_TYPE_MAT4  = wxT( "mat4x4" );

		wxPGProperty * doBuildTrackedValueProperty( wxString const & name
			, UniformValueBase const & uniform )
		{
			wxPGProperty * result = nullptr;

			if ( uniform.isRanged() )
			{
				switch ( uniform.getType() )
				{
				case UniformType::eInt:
					{
						auto rangedUniform = static_cast< UniformValue< castor::ChangeTracked< castor::RangedValue< int32_t > > > const & >( uniform );
						result = new wxIntProperty( name
							, name
							, rangedUniform.getValue().value().value() );
						result->SetEditor( wxT( "SpinCtrl" ) );
						result->SetAttribute( wxT( "Min" ), rangedUniform.getValue().value().range().min() );
						result->SetAttribute( wxT( "Max" ), rangedUniform.getValue().value().range().max() );
					}
					break;

				case UniformType::eUInt:
					{
						auto rangedUniform = static_cast< UniformValue< castor::ChangeTracked< castor::RangedValue< uint32_t > > > const & >( uniform );
						result = new wxUIntProperty( name
							, name
							, rangedUniform.getValue().value().value() );
						result->SetEditor( wxT( "SpinCtrl" ) );
						result->SetAttribute( wxT( "Min" ), int32_t( rangedUniform.getValue().value().range().min() ) );
						result->SetAttribute( wxT( "Max" ), int32_t( rangedUniform.getValue().value().range().max() ) );
					}
					break;

				case UniformType::eFloat:
					{
						auto rangedUniform = static_cast< UniformValue< castor::ChangeTracked< castor::RangedValue< float > > > const & >( uniform );
						result = new wxFloatProperty( name
							, name
							, rangedUniform.getValue().value().value() );
						result->SetEditor( wxT( "SpinCtrl" ) );
						result->SetAttribute( wxT( "Min" ), rangedUniform.getValue().value().range().min() );
						result->SetAttribute( wxT( "Max" ), rangedUniform.getValue().value().range().max() );
					}
					break;
				}
			}
			else
			{
				switch ( uniform.getType() )
				{
				case UniformType::eInt:
					result = new wxIntProperty( name, name, static_cast< UniformValue< castor::ChangeTracked< int32_t > > const & >( uniform ).getValue() );
					break;

				case UniformType::eUInt:
					result = new wxUIntProperty( name, name, static_cast< UniformValue< castor::ChangeTracked< uint32_t > > const & >( uniform ).getValue() );
					break;

				case UniformType::eFloat:
					result = new wxFloatProperty( name, name, static_cast< UniformValue< castor::ChangeTracked< float > > const & >( uniform ).getValue() );
					break;

				case UniformType::eVec2f:
					result = new FloatPointProperty< 2 >( GC_POINT_XY, name, name, static_cast< UniformValue< castor::ChangeTracked< Point2f > > const & >( uniform ).getValue() );
					break;

				case UniformType::eVec3f:
					result = new FloatPointProperty< 3 >( GC_POINT_XYZ, name, name, static_cast< UniformValue< castor::ChangeTracked< Point3f > > const & >( uniform ).getValue() );
					break;

				case UniformType::eVec4f:
					result = new FloatPointProperty< 4 >( GC_POINT_XYZW, name, name, static_cast< UniformValue< castor::ChangeTracked< Point4f > > const & >( uniform ).getValue() );
					break;

				case UniformType::eVec2i:
					result = new IntPointProperty< 2 >( GC_POINT_XY, name, name, static_cast< UniformValue< castor::ChangeTracked< Point2i > > const & >( uniform ).getValue() );
					break;

				case UniformType::eVec3i:
					result = new IntPointProperty< 3 >( GC_POINT_XYZ, name, name, static_cast< UniformValue< castor::ChangeTracked< Point3i > > const & >( uniform ).getValue() );
					break;

				case UniformType::eVec4i:
					result = new IntPointProperty< 4 >( GC_POINT_XYZW, name, name, static_cast< UniformValue< castor::ChangeTracked< Point4i > > const & >( uniform ).getValue() );
					break;

				case UniformType::eVec2ui:
					result = new UIntPointProperty< 2 >( GC_POINT_XY, name, name, static_cast< UniformValue< castor::ChangeTracked< Point2ui > > const & >( uniform ).getValue() );
					break;

				case UniformType::eVec3ui:
					result = new UIntPointProperty< 3 >( GC_POINT_XYZ, name, name, static_cast< UniformValue< castor::ChangeTracked< Point3ui > > const & >( uniform ).getValue() );
					break;

				case UniformType::eVec4ui:
					result = new UIntPointProperty< 4 >( GC_POINT_XYZW, name, name, static_cast< UniformValue< castor::ChangeTracked< Point4ui > > const & >( uniform ).getValue() );
					break;

				case UniformType::eMat4f:
					result = new Matrix4fProperty( GC_POINT_1234, GC_POINT_1234, name, name, static_cast< UniformValue< castor::ChangeTracked< Matrix4x4f > > const & >( uniform ).getValue() );
					break;
				}
			}

			return result;
		}

		wxPGProperty * doBuildUntrackedValueProperty( wxString const & name
			, UniformValueBase const & uniform )
		{
			wxPGProperty * result = nullptr;

			if ( uniform.isRanged() )
			{
				switch ( uniform.getType() )
				{
				case UniformType::eInt:
					{
						auto rangedUniform = static_cast< UniformValue< castor::RangedValue< int32_t > > const & >( uniform );
						result = new wxIntProperty( name
							, name
							, rangedUniform.getValue().value() );
						result->SetEditor( wxT( "SpinCtrl" ) );
						result->SetAttribute( wxT( "Min" ), rangedUniform.getValue().range().min() );
						result->SetAttribute( wxT( "Max" ), rangedUniform.getValue().range().max() );
					}
					break;

				case UniformType::eUInt:
					{
						auto rangedUniform = static_cast< UniformValue< castor::RangedValue< uint32_t > > const & >( uniform );
						result = new wxUIntProperty( name
							, name
							, rangedUniform.getValue().value() );
						result->SetEditor( wxT( "SpinCtrl" ) );
						result->SetAttribute( wxT( "Min" ), int32_t( rangedUniform.getValue().range().min() ) );
						result->SetAttribute( wxT( "Max" ), int32_t( rangedUniform.getValue().range().max() ) );
					}
					break;

				case UniformType::eFloat:
					{
						auto rangedUniform = static_cast< UniformValue< castor::RangedValue< float > > const & >( uniform );
						result = new wxFloatProperty( name
							, name
							, rangedUniform.getValue().value() );
						result->SetEditor( wxT( "SpinCtrl" ) );
						result->SetAttribute( wxT( "Min" ), rangedUniform.getValue().range().min() );
						result->SetAttribute( wxT( "Max" ), rangedUniform.getValue().range().max() );
					}
					break;
				}
				switch ( uniform.getType() )
				{
				case UniformType::eInt:
					result = new wxIntProperty( name
						, name
						, static_cast< UniformValue< castor::RangedValue< int32_t > > const & >( uniform ).getValue() );
					result->SetEditor( wxT( "SpinCtrl" ) );
					break;

				case UniformType::eUInt:
					result = new wxUIntProperty( name
						, name
						, static_cast< UniformValue< castor::RangedValue< uint32_t > > const & >( uniform ).getValue() );
					result->SetEditor( wxT( "SpinCtrl" ) );
					break;

				case UniformType::eFloat:
					result = new wxFloatProperty( name
						, name
						, static_cast< UniformValue< castor::RangedValue< float > > const & >( uniform ).getValue() );
					result->SetEditor( wxT( "SpinCtrl" ) );
					break;
				}
			}
			else
			{
				switch ( uniform.getType() )
				{
				case UniformType::eInt:
					result = new wxIntProperty( name, name, static_cast< UniformValue< int32_t > const & >( uniform ).getValue() );
					break;

				case UniformType::eUInt:
					result = new wxUIntProperty( name, name, static_cast< UniformValue< uint32_t > const & >( uniform ).getValue() );
					break;

				case UniformType::eFloat:
					result = new wxFloatProperty( name, name, static_cast< UniformValue< float > const & >( uniform ).getValue() );
					break;

				case UniformType::eVec2f:
					result = new FloatPointProperty< 2 >( GC_POINT_XY, name, name, static_cast< UniformValue< Point2f > const & >( uniform ).getValue() );
					break;

				case UniformType::eVec3f:
					result = new FloatPointProperty< 3 >( GC_POINT_XYZ, name, name, static_cast< UniformValue< Point3f > const & >( uniform ).getValue() );
					break;

				case UniformType::eVec4f:
					result = new FloatPointProperty< 4 >( GC_POINT_XYZW, name, name, static_cast< UniformValue< Point4f > const & >( uniform ).getValue() );
					break;

				case UniformType::eVec2i:
					result = new IntPointProperty< 2 >( GC_POINT_XY, name, name, static_cast< UniformValue< Point2i > const & >( uniform ).getValue() );
					break;

				case UniformType::eVec3i:
					result = new IntPointProperty< 3 >( GC_POINT_XYZ, name, name, static_cast< UniformValue< Point3i > const & >( uniform ).getValue() );
					break;

				case UniformType::eVec4i:
					result = new IntPointProperty< 4 >( GC_POINT_XYZW, name, name, static_cast< UniformValue< Point4i > const & >( uniform ).getValue() );
					break;

				case UniformType::eVec2ui:
					result = new UIntPointProperty< 2 >( GC_POINT_XY, name, name, static_cast< UniformValue< Point2ui > const & >( uniform ).getValue() );
					break;

				case UniformType::eVec3ui:
					result = new UIntPointProperty< 3 >( GC_POINT_XYZ, name, name, static_cast< UniformValue< Point3ui > const & >( uniform ).getValue() );
					break;

				case UniformType::eVec4ui:
					result = new UIntPointProperty< 4 >( GC_POINT_XYZW, name, name, static_cast< UniformValue< Point4ui > const & >( uniform ).getValue() );
					break;

				case UniformType::eMat4f:
					result = new Matrix4fProperty( GC_POINT_1234, GC_POINT_1234, name, name, static_cast< UniformValue< Matrix4x4f > const & >( uniform ).getValue() );
					break;
				}
			}

			return result;
		}

		wxPGProperty * doBuildValueProperty( wxString const & name
			, UniformValueBase const & uniform )
		{
			wxPGProperty * result = nullptr;

			if ( uniform.isTracked() )
			{
				result = doBuildTrackedValueProperty( name, uniform );
			}
			else
			{
				result = doBuildUntrackedValueProperty( name, uniform );
			}

			return result;
		}

		void doSetTrackedValue( UniformValueBase & uniform
			, wxVariant const & value )
		{
			if ( uniform.isRanged() )
			{
				switch ( uniform.getType() )
				{
				case UniformType::eInt:
					{
						auto & rangedUniform = static_cast< UniformValue< castor::ChangeTracked< castor::RangedValue< int32_t > > > & >( uniform );
						rangedUniform.getValue() = castor::RangedValue< int32_t >( getValue< int32_t >( value ), rangedUniform.getValue().value().range() );
					}
					break;

				case UniformType::eUInt:
					{
						auto & rangedUniform = static_cast< UniformValue< castor::ChangeTracked< castor::RangedValue< uint32_t > > > & >( uniform );
						rangedUniform.getValue() = castor::RangedValue< uint32_t >( getValue< uint32_t >( value ), rangedUniform.getValue().value().range() );
					}
					break;

				case UniformType::eFloat:
					{
						auto & rangedUniform = static_cast< UniformValue< castor::ChangeTracked< castor::RangedValue< float > > > & >( uniform );
						rangedUniform.getValue() = castor::RangedValue< float >( getValue< float >( value ), rangedUniform.getValue().value().range() );
					}
					break;
				}
			}
			else
			{
				switch ( uniform.getType() )
				{
				case UniformType::eInt:
					static_cast< UniformValue< castor::ChangeTracked< int32_t > > & >( uniform ).getValue() = getValue< int32_t >( value );
					break;

				case UniformType::eUInt:
					static_cast< UniformValue< castor::ChangeTracked< uint32_t > > & >( uniform ).getValue() = getValue< uint32_t >( value );
					break;

				case UniformType::eFloat:
					static_cast< UniformValue< castor::ChangeTracked< float > > & >( uniform ).getValue() = getValue< float >( value );
					break;

				case UniformType::eVec2i:
					static_cast< UniformValue< castor::ChangeTracked< castor::Point2i > > & >( uniform ).getValue() = PointRefFromVariant< int, 2 >( value );
					break;

				case UniformType::eVec3i:
					static_cast< UniformValue< castor::ChangeTracked< castor::Point3i > > & >( uniform ).getValue() = PointRefFromVariant< int, 3 >( value );
					break;

				case UniformType::eVec4i:
					static_cast< UniformValue< castor::ChangeTracked< castor::Point4i > > & >( uniform ).getValue() = PointRefFromVariant< int, 4 >( value );
					break;

				case UniformType::eVec2ui:
					static_cast< UniformValue< castor::ChangeTracked< castor::Point2ui > > & >( uniform ).getValue() = PointRefFromVariant< uint32_t, 2 >( value );
					break;

				case UniformType::eVec3ui:
					static_cast< UniformValue< castor::ChangeTracked< castor::Point3ui > > & >( uniform ).getValue() = PointRefFromVariant< uint32_t, 3 >( value );
					break;

				case UniformType::eVec4ui:
					static_cast< UniformValue< castor::ChangeTracked< castor::Point4ui > > & >( uniform ).getValue() = PointRefFromVariant< uint32_t, 4 >( value );
					break;

				case UniformType::eVec2f:
					static_cast< UniformValue< castor::ChangeTracked< castor::Point2f > > & >( uniform ).getValue() = PointRefFromVariant< float, 2 >( value );
					break;

				case UniformType::eVec3f:
					static_cast< UniformValue< castor::ChangeTracked< castor::Point3f > > & >( uniform ).getValue() = PointRefFromVariant< float, 3 >( value );
					break;

				case UniformType::eVec4f:
					static_cast< UniformValue< castor::ChangeTracked< castor::Point4f > > & >( uniform ).getValue() = PointRefFromVariant< float, 4 >( value );
					break;

				case UniformType::eMat4f:
					static_cast< UniformValue< castor::ChangeTracked< castor::Matrix4x4f > > & >( uniform ).getValue() = matrixRefFromVariant< float, 4 >( value );
					break;
				}
			}
		}

		void doSetUntrackedValue( UniformValueBase & uniform
			, wxVariant const & value )
		{
			if ( uniform.isRanged() )
			{
				switch ( uniform.getType() )
				{
				case UniformType::eInt:
					{
						auto & rangedUniform = static_cast< UniformValue< castor::RangedValue< int32_t > > & >( uniform );
						rangedUniform.getValue() = castor::RangedValue< int32_t >( getValue< int32_t >( value ), rangedUniform.getValue().range() );
					}
					break;

				case UniformType::eUInt:
					{
						auto & rangedUniform = static_cast< UniformValue< castor::RangedValue< uint32_t > > & >( uniform );
						rangedUniform.getValue() = castor::RangedValue< uint32_t >( getValue< uint32_t >( value ), rangedUniform.getValue().range() );
					}
					break;

				case UniformType::eFloat:
					{
						auto & rangedUniform = static_cast< UniformValue< castor::RangedValue< float > > & >( uniform );
						rangedUniform.getValue() = castor::RangedValue< float >( getValue< float >( value ), rangedUniform.getValue().range() );
					}
					break;

				}
			}
			else
			{
				switch ( uniform.getType() )
				{
				case UniformType::eInt:
					static_cast< UniformValue< int32_t > & >( uniform ).getValue() = getValue< int32_t >( value );
					break;

				case UniformType::eUInt:
					static_cast< UniformValue< uint32_t > & >( uniform ).getValue() = getValue< uint32_t >( value );
					break;

				case UniformType::eFloat:
					static_cast< UniformValue< float > & >( uniform ).getValue() = getValue< float >( value );
					break;

				case UniformType::eVec2i:
					static_cast< UniformValue< castor::Point2i > & >( uniform ).getValue() = PointRefFromVariant< int, 2 >( value );
					break;

				case UniformType::eVec3i:
					static_cast< UniformValue< castor::Point3i > & >( uniform ).getValue() = PointRefFromVariant< int, 3 >( value );
					break;

				case UniformType::eVec4i:
					static_cast< UniformValue< castor::Point4i > & >( uniform ).getValue() = PointRefFromVariant< int, 4 >( value );
					break;

				case UniformType::eVec2ui:
					static_cast< UniformValue< castor::Point2ui > & >( uniform ).getValue() = PointRefFromVariant< uint32_t, 2 >( value );
					break;

				case UniformType::eVec3ui:
					static_cast< UniformValue< castor::Point3ui > & >( uniform ).getValue() = PointRefFromVariant< uint32_t, 3 >( value );
					break;

				case UniformType::eVec4ui:
					static_cast< UniformValue< castor::Point4ui > & >( uniform ).getValue() = PointRefFromVariant< uint32_t, 4 >( value );
					break;

				case UniformType::eVec2f:
					static_cast< UniformValue< castor::Point2f > & >( uniform ).getValue() = PointRefFromVariant< float, 2 >( value );
					break;

				case UniformType::eVec3f:
					static_cast< UniformValue< castor::Point3f > & >( uniform ).getValue() = PointRefFromVariant< float, 3 >( value );
					break;

				case UniformType::eVec4f:
					static_cast< UniformValue< castor::Point4f > & >( uniform ).getValue() = PointRefFromVariant< float, 4 >( value );
					break;

				case UniformType::eMat4f:
					static_cast< UniformValue< castor::Matrix4x4f > & >( uniform ).getValue() = matrixRefFromVariant< float, 4 >( value );
					break;
				}
			}
		}

		void doSetValue( UniformValueBase & uniform
			, wxVariant const & value )
		{
			if ( uniform.isTracked() )
			{
				doSetTrackedValue( uniform, value );
			}
			else
			{
				doSetUntrackedValue( uniform, value );
			}
		}
	}

	FrameVariableTreeItemProperty::FrameVariableTreeItemProperty( Engine * engine
		, bool editable
		, UniformValueBase & uniform )
		: TreeItemProperty{ engine, editable, ePROPERTY_DATA_TYPE_CAMERA }
		, m_uniform{ uniform }
	{
		PROPERTY_CATEGORY_VARIABLE = _( "Uniform:" );
		PROPERTY_NAME = _( "Name" );
		PROPERTY_VALUE = _( "Value" );
		PROPERTY_TYPE = _( "Type" );
		PROPERTY_TYPE_FLOAT = wxT( "float" );
		PROPERTY_TYPE_INT = wxT( "int" );
		PROPERTY_TYPE_INT = wxT( "uint" );
		PROPERTY_TYPE_VEC2 = wxT( "vec2" );
		PROPERTY_TYPE_VEC3 = wxT( "vec3" );
		PROPERTY_TYPE_VEC4 = wxT( "vec4" );
		PROPERTY_TYPE_IVEC2 = wxT( "ivec2" );
		PROPERTY_TYPE_IVEC3 = wxT( "ivec3" );
		PROPERTY_TYPE_IVEC4 = wxT( "ivec4" );
		PROPERTY_TYPE_UIVEC2 = wxT( "uivec2" );
		PROPERTY_TYPE_UIVEC3 = wxT( "uivec3" );
		PROPERTY_TYPE_UIVEC4 = wxT( "uivec4" );
		PROPERTY_TYPE_MAT4  = wxT( "mat4x4" );

		CreateTreeItemMenu();
	}

	FrameVariableTreeItemProperty::~FrameVariableTreeItemProperty()
	{
	}

	void FrameVariableTreeItemProperty::doCreateProperties( wxPGEditor * editor
		, wxPropertyGrid * grid )
	{
		wxString displayName = m_uniform.getName();

		grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_VARIABLE + wxT( " " ) + displayName ) );
		auto * prop = grid->Append( new wxStringProperty( PROPERTY_TYPE ) );
		prop->SetValue( getName( m_uniform.getType() ) );
		prop->Enable( false );
		grid->Append( doBuildValueProperty( PROPERTY_VALUE, m_uniform ) );
	}

	void FrameVariableTreeItemProperty::doPropertyChange( wxPropertyGridEvent & p_event )
	{
		wxPGProperty * property = p_event.GetProperty();

		if ( property )
		{
			if ( property->GetName() == PROPERTY_VALUE )
			{
				onValueChange( property->GetValue() );
			}
		}
	}

	void FrameVariableTreeItemProperty::onValueChange( wxVariant const & value )
	{
		doApplyChange( [value, this]()
		{
			doSetValue( m_uniform, value );
		} );
	}
}
