#include "FrameVariableTreeItemProperty.hpp"

#include <FrameVariableBuffer.hpp>
#include <FunctorEvent.hpp>
#include <MatrixFrameVariable.hpp>
#include <OneFrameVariable.hpp>
#include <PointFrameVariable.hpp>

#include "PointProperties.hpp"
#include "MatrixProperties.hpp"

#include <wx/propgrid/advprops.h>

using namespace Castor3D;
using namespace Castor;

namespace GuiCommon
{
	namespace
	{
		static const wxString PROPERTY_CATEGORY_VARIABLE = _( "Frame Variable: " );
		static const wxString PROPERTY_NAME = _( "Name" );
		static const wxString PROPERTY_VALUE = _( "Value" );
		static const wxString PROPERTY_TYPE = _( "Type" );
		static const wxString PROPERTY_TYPE_BOOL = wxT( "bool" );
		static const wxString PROPERTY_TYPE_INT = wxT( "int" );
		static const wxString PROPERTY_TYPE_FLOAT = wxT( "float" );
		static const wxString PROPERTY_TYPE_DOUBLE = wxT( "double" );
		static const wxString PROPERTY_TYPE_BVEC2 = wxT( "bvec2" );
		static const wxString PROPERTY_TYPE_BVEC3 = wxT( "bvec3" );
		static const wxString PROPERTY_TYPE_BVEC4 = wxT( "bvec4" );
		static const wxString PROPERTY_TYPE_IVEC2 = wxT( "ivec2" );
		static const wxString PROPERTY_TYPE_IVEC3 = wxT( "ivec3" );
		static const wxString PROPERTY_TYPE_IVEC4 = wxT( "ivec4" );
		static const wxString PROPERTY_TYPE_UIVEC2 = wxT( "uivec2" );
		static const wxString PROPERTY_TYPE_UIVEC3 = wxT( "uivec3" );
		static const wxString PROPERTY_TYPE_UIVEC4 = wxT( "uivec4" );
		static const wxString PROPERTY_TYPE_VEC2  = wxT( "vec2" );
		static const wxString PROPERTY_TYPE_VEC3  = wxT( "vec3" );
		static const wxString PROPERTY_TYPE_VEC4  = wxT( "vec4" );
		static const wxString PROPERTY_TYPE_DVEC2 = wxT( "dvec2" );
		static const wxString PROPERTY_TYPE_DVEC3 = wxT( "dvec3" );
		static const wxString PROPERTY_TYPE_DVEC4 = wxT( "dvec4" );
		static const wxString PROPERTY_TYPE_BMAT2 = wxT( "bmat2x2" );
		static const wxString PROPERTY_TYPE_BMAT3 = wxT( "bmat3x3" );
		static const wxString PROPERTY_TYPE_BMAT4 = wxT( "bmat4x4" );
		static const wxString PROPERTY_TYPE_IMAT2 = wxT( "imat2x2" );
		static const wxString PROPERTY_TYPE_IMAT3 = wxT( "imat3x3" );
		static const wxString PROPERTY_TYPE_IMAT4 = wxT( "imat4x4" );
		static const wxString PROPERTY_TYPE_UIMAT2 = wxT( "uimat2x2" );
		static const wxString PROPERTY_TYPE_UIMAT3 = wxT( "uimat3x3" );
		static const wxString PROPERTY_TYPE_UIMAT4 = wxT( "uimat4x4" );
		static const wxString PROPERTY_TYPE_MAT2  = wxT( "mat2x2" );
		static const wxString PROPERTY_TYPE_MAT3  = wxT( "mat3x3" );
		static const wxString PROPERTY_TYPE_MAT4  = wxT( "mat4x4" );
		static const wxString PROPERTY_TYPE_DMAT2 = wxT( "dmat2x2" );
		static const wxString PROPERTY_TYPE_DMAT3 = wxT( "dmat3x3" );
		static const wxString PROPERTY_TYPE_DMAT4 = wxT( "dmat4x4" );
		static const wxString PROPERTY_TYPE_SAMPLER = wxT( "sampler" );

		wxPGProperty * DoBuildValueProperty( wxString const & p_name, FrameVariableSPtr p_variable )
		{
			wxPGProperty * l_return = NULL;

			switch ( p_variable->GetFullType() )
			{
			case eFRAME_VARIABLE_TYPE_BOOL:
				l_return = new wxBoolProperty( p_name, p_name, std::static_pointer_cast< OneBoolFrameVariable >( p_variable )->GetValue() );
				break;
			case eFRAME_VARIABLE_TYPE_INT:
				l_return = new wxIntProperty( p_name, p_name, std::static_pointer_cast< OneIntFrameVariable >( p_variable )->GetValue() );
				break;
			case eFRAME_VARIABLE_TYPE_UINT:
				l_return = new wxUIntProperty( p_name, p_name, std::static_pointer_cast< OneUIntFrameVariable >( p_variable )->GetValue() );
				break;
			case eFRAME_VARIABLE_TYPE_FLOAT:
				l_return = new wxFloatProperty( p_name, p_name, std::static_pointer_cast< OneFloatFrameVariable >( p_variable )->GetValue() );
				break;
			case eFRAME_VARIABLE_TYPE_DOUBLE:
				l_return = new wxFloatProperty( p_name, p_name, std::static_pointer_cast< OneDoubleFrameVariable >( p_variable )->GetValue() );
				break;
			case eFRAME_VARIABLE_TYPE_SAMPLER:
				l_return = new wxFloatProperty( p_name );
				if ( std::static_pointer_cast< OneTextureFrameVariable >( p_variable )->GetValue() )
				{
					int l_index = std::static_pointer_cast< OneTextureFrameVariable >( p_variable )->GetValue()->GetIndex();
					l_return->SetValue( l_index );
				}
				break;
			case eFRAME_VARIABLE_TYPE_VEC2B:
				l_return = new BoolPointProperty< 2 >( p_name, p_name, std::static_pointer_cast< Point2bFrameVariable >( p_variable )->GetValue() );
				break;
			case eFRAME_VARIABLE_TYPE_VEC3B:
				l_return = new BoolPointProperty< 3 >( p_name, p_name, std::static_pointer_cast< Point3bFrameVariable >( p_variable )->GetValue() );
				break;
			case eFRAME_VARIABLE_TYPE_VEC4B:
				l_return = new BoolPointProperty< 4 >( p_name, p_name, std::static_pointer_cast< Point4bFrameVariable >( p_variable )->GetValue() );
				break;
			case eFRAME_VARIABLE_TYPE_VEC2I:
				l_return = new IntPointProperty< 2 >( p_name, p_name, std::static_pointer_cast< Point2iFrameVariable >( p_variable )->GetValue() );
				break;
			case eFRAME_VARIABLE_TYPE_VEC3I:
				l_return = new IntPointProperty< 3 >( p_name, p_name, std::static_pointer_cast< Point3iFrameVariable >( p_variable )->GetValue() );
				break;
			case eFRAME_VARIABLE_TYPE_VEC4I:
				l_return = new IntPointProperty< 4 >( p_name, p_name, std::static_pointer_cast< Point4iFrameVariable >( p_variable )->GetValue() );
				break;
			case eFRAME_VARIABLE_TYPE_VEC2UI:
				l_return = new UIntPointProperty< 2 >( p_name, p_name, std::static_pointer_cast< Point2uiFrameVariable >( p_variable )->GetValue() );
				break;
			case eFRAME_VARIABLE_TYPE_VEC3UI:
				l_return = new UIntPointProperty< 3 >( p_name, p_name, std::static_pointer_cast< Point3uiFrameVariable >( p_variable )->GetValue() );
				break;
			case eFRAME_VARIABLE_TYPE_VEC4UI:
				l_return = new UIntPointProperty< 4 >( p_name, p_name, std::static_pointer_cast< Point4uiFrameVariable >( p_variable )->GetValue() );
				break;
			case eFRAME_VARIABLE_TYPE_VEC2F:
				l_return = new FloatPointProperty< 2 >( p_name, p_name, std::static_pointer_cast< Point2fFrameVariable >( p_variable )->GetValue() );
				break;
			case eFRAME_VARIABLE_TYPE_VEC3F:
				l_return = new FloatPointProperty< 3 >( p_name, p_name, std::static_pointer_cast< Point3fFrameVariable >( p_variable )->GetValue() );
				break;
			case eFRAME_VARIABLE_TYPE_VEC4F:
				l_return = new FloatPointProperty< 4 >( p_name, p_name, std::static_pointer_cast< Point4fFrameVariable >( p_variable )->GetValue() );
				break;
			case eFRAME_VARIABLE_TYPE_VEC2D:
				l_return = new DoublePointProperty< 2 >( p_name, p_name, std::static_pointer_cast< Point2dFrameVariable >( p_variable )->GetValue() );
				break;
			case eFRAME_VARIABLE_TYPE_VEC3D:
				l_return = new DoublePointProperty< 3 >( p_name, p_name, std::static_pointer_cast< Point3dFrameVariable >( p_variable )->GetValue() );
				break;
			case eFRAME_VARIABLE_TYPE_VEC4D:
				l_return = new DoublePointProperty< 4 >( p_name, p_name, std::static_pointer_cast< Point4dFrameVariable >( p_variable )->GetValue() );
				break;
			case eFRAME_VARIABLE_TYPE_MAT2X2F:
				l_return = new FloatMatrixProperty< 2 >( p_name, p_name, std::static_pointer_cast< Matrix2x2fFrameVariable >( p_variable )->GetValue() );
				break;
			case eFRAME_VARIABLE_TYPE_MAT3X3F:
				l_return = new FloatMatrixProperty< 3 >( p_name, p_name, std::static_pointer_cast< Matrix3x3fFrameVariable >( p_variable )->GetValue() );
				break;
			case eFRAME_VARIABLE_TYPE_MAT4X4F:
				l_return = new FloatMatrixProperty< 4 >( p_name, p_name, std::static_pointer_cast< Matrix4x4fFrameVariable >( p_variable )->GetValue() );
				break;
			case eFRAME_VARIABLE_TYPE_MAT2X2D:
				l_return = new DoubleMatrixProperty< 2 >( p_name, p_name, std::static_pointer_cast< Matrix2x2dFrameVariable >( p_variable )->GetValue() );
				break;
			case eFRAME_VARIABLE_TYPE_MAT3X3D:
				l_return = new DoubleMatrixProperty< 3 >( p_name, p_name, std::static_pointer_cast< Matrix3x3dFrameVariable >( p_variable )->GetValue() );
				break;
			case eFRAME_VARIABLE_TYPE_MAT4X4D:
				l_return = new DoubleMatrixProperty< 4 >( p_name, p_name, std::static_pointer_cast< Matrix4x4dFrameVariable >( p_variable )->GetValue() );
				break;
			}

			return l_return;
		}
	}

	FrameVariableTreeItemProperty::FrameVariableTreeItemProperty( bool p_editable, Castor3D::FrameVariableSPtr p_variable, FrameVariableBufferSPtr p_buffer )
		: TreeItemProperty( p_editable, ePROPERTY_DATA_TYPE_CAMERA )
		, m_variable( p_variable )
		, m_type( eSHADER_TYPE_COUNT )
		, m_buffer( p_buffer )
	{
	}

	FrameVariableTreeItemProperty::FrameVariableTreeItemProperty( bool p_editable, Castor3D::FrameVariableSPtr p_variable, eSHADER_TYPE p_type )
		: TreeItemProperty( p_editable, ePROPERTY_DATA_TYPE_CAMERA )
		, m_variable( p_variable )
		, m_type( p_type )
	{
	}

	FrameVariableTreeItemProperty::~FrameVariableTreeItemProperty()
	{
	}

	void FrameVariableTreeItemProperty::CreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid )
	{
		FrameVariableSPtr l_variable = GetVariable();

		if ( l_variable )
		{
			p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_VARIABLE + l_variable->GetName() ) );
			p_grid->Append( new wxStringProperty( PROPERTY_NAME ) )->SetValue( l_variable->GetName() );

			wxArrayString l_choices;
			l_choices.push_back( PROPERTY_TYPE_BOOL );
			l_choices.push_back( PROPERTY_TYPE_INT );
			l_choices.push_back( PROPERTY_TYPE_FLOAT );
			l_choices.push_back( PROPERTY_TYPE_DOUBLE );
			l_choices.push_back( PROPERTY_TYPE_BVEC2 );
			l_choices.push_back( PROPERTY_TYPE_BVEC3 );
			l_choices.push_back( PROPERTY_TYPE_BVEC4 );
			l_choices.push_back( PROPERTY_TYPE_IVEC2 );
			l_choices.push_back( PROPERTY_TYPE_IVEC3 );
			l_choices.push_back( PROPERTY_TYPE_IVEC4 );
			l_choices.push_back( PROPERTY_TYPE_UIVEC2 );
			l_choices.push_back( PROPERTY_TYPE_UIVEC3 );
			l_choices.push_back( PROPERTY_TYPE_UIVEC4 );
			l_choices.push_back( PROPERTY_TYPE_VEC2 );
			l_choices.push_back( PROPERTY_TYPE_VEC3 );
			l_choices.push_back( PROPERTY_TYPE_VEC4 );
			l_choices.push_back( PROPERTY_TYPE_DVEC2 );
			l_choices.push_back( PROPERTY_TYPE_DVEC3 );
			l_choices.push_back( PROPERTY_TYPE_DVEC4 );
			l_choices.push_back( PROPERTY_TYPE_MAT2 );
			l_choices.push_back( PROPERTY_TYPE_MAT3 );
			l_choices.push_back( PROPERTY_TYPE_MAT4 );
			l_choices.push_back( PROPERTY_TYPE_DMAT2 );
			l_choices.push_back( PROPERTY_TYPE_DMAT3 );
			l_choices.push_back( PROPERTY_TYPE_DMAT4 );
			l_choices.push_back( PROPERTY_TYPE_SAMPLER );
			wxString l_selected = make_wxString( l_variable->GetFullTypeName() );
			wxEnumProperty * l_type = new wxEnumProperty( PROPERTY_TYPE, PROPERTY_TYPE, l_choices );
			l_type->Enable( !m_buffer.expired() );
			l_type->SetValue( l_selected );
			p_grid->Append( l_type );

			p_grid->Append( DoBuildValueProperty( PROPERTY_VALUE, l_variable ) );
		}
	}

	void FrameVariableTreeItemProperty::OnPropertyChange( wxPropertyGridEvent & p_event )
	{
		wxPGProperty * l_property = p_event.GetProperty();
		FrameVariableSPtr l_variable = GetVariable();

		if ( l_property && l_variable )
		{
			if ( l_property->GetName() == PROPERTY_CATEGORY_VARIABLE )
			{
				if ( l_property->GetValueAsString() == PROPERTY_TYPE_BOOL )
				{
					OnTypeChange( eFRAME_VARIABLE_TYPE_BOOL );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_INT )
				{
					OnTypeChange( eFRAME_VARIABLE_TYPE_INT );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_FLOAT )
				{
					OnTypeChange( eFRAME_VARIABLE_TYPE_FLOAT );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_DOUBLE )
				{
					OnTypeChange( eFRAME_VARIABLE_TYPE_DOUBLE );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_BVEC2 )
				{
					OnTypeChange( eFRAME_VARIABLE_TYPE_VEC2B );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_BVEC3 )
				{
					OnTypeChange( eFRAME_VARIABLE_TYPE_VEC3B );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_BVEC4 )
				{
					OnTypeChange( eFRAME_VARIABLE_TYPE_VEC4B );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_IVEC2 )
				{
					OnTypeChange( eFRAME_VARIABLE_TYPE_VEC2I );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_IVEC3 )
				{
					OnTypeChange( eFRAME_VARIABLE_TYPE_VEC3I );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_IVEC4 )
				{
					OnTypeChange( eFRAME_VARIABLE_TYPE_VEC4I );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_UIVEC2 )
				{
					OnTypeChange( eFRAME_VARIABLE_TYPE_VEC2UI );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_UIVEC3 )
				{
					OnTypeChange( eFRAME_VARIABLE_TYPE_VEC3UI );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_UIVEC4 )
				{
					OnTypeChange( eFRAME_VARIABLE_TYPE_VEC4UI );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_VEC2 )
				{
					OnTypeChange( eFRAME_VARIABLE_TYPE_VEC2F );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_VEC3 )
				{
					OnTypeChange( eFRAME_VARIABLE_TYPE_VEC3F );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_VEC4 )
				{
					OnTypeChange( eFRAME_VARIABLE_TYPE_VEC4F );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_DVEC2 )
				{
					OnTypeChange( eFRAME_VARIABLE_TYPE_VEC2D );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_DVEC3 )
				{
					OnTypeChange( eFRAME_VARIABLE_TYPE_VEC3D );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_DVEC4 )
				{
					OnTypeChange( eFRAME_VARIABLE_TYPE_VEC4D );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_MAT2 )
				{
					OnTypeChange( eFRAME_VARIABLE_TYPE_MAT2X2F );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_MAT3 )
				{
					OnTypeChange( eFRAME_VARIABLE_TYPE_MAT3X3F );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_MAT4 )
				{
					OnTypeChange( eFRAME_VARIABLE_TYPE_MAT4X4F );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_DMAT2 )
				{
					OnTypeChange( eFRAME_VARIABLE_TYPE_MAT2X2D );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_DMAT3 )
				{
					OnTypeChange( eFRAME_VARIABLE_TYPE_MAT3X3D );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_DMAT4 )
				{
					OnTypeChange( eFRAME_VARIABLE_TYPE_MAT4X4D );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_SAMPLER )
				{
					OnTypeChange( eFRAME_VARIABLE_TYPE_SAMPLER );
				}
			}
		}
	}

	void FrameVariableTreeItemProperty::OnTypeChange( Castor3D::eFRAME_VARIABLE_TYPE p_value )
	{
		FrameVariableSPtr l_variable = GetVariable();
		FrameVariableBufferSPtr l_buffer = GetBuffer();

		if ( l_buffer )
		{
			l_variable->GetProgram()->GetRenderSystem()->GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [&p_value, l_variable, &l_buffer, this]()
			{
				l_buffer->RemoveVariable( l_variable->GetName() );
				m_variable = l_buffer->CreateVariable( *l_variable->GetProgram(), p_value, l_variable->GetName(), 1 );
			} ) );
		}
	}
}
