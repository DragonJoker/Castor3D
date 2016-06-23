#include "FrameVariableTreeItemProperty.hpp"

#include <Render/RenderSystem.hpp>
#include <Shader/FrameVariableBuffer.hpp>
#include <Shader/MatrixFrameVariable.hpp>
#include <Shader/OneFrameVariable.hpp>
#include <Shader/PointFrameVariable.hpp>
#include <Shader/ShaderProgram.hpp>

#include "PointProperties.hpp"
#include "MatrixProperties.hpp"

#include <wx/propgrid/advprops.h>

using namespace Castor3D;
using namespace Castor;

namespace GuiCommon
{
	namespace
	{
		static wxString PROPERTY_CATEGORY_VARIABLE = _( "Frame Variable: " );
		static wxString PROPERTY_NAME = _( "Name" );
		static wxString PROPERTY_VALUE = _( "Value" );
		static wxString PROPERTY_TYPE = _( "Type" );
		static wxString PROPERTY_OCCURENCES = _( "Occurences" );
		static wxString PROPERTY_TYPE_BOOL = wxT( "bool" );
		static wxString PROPERTY_TYPE_INT = wxT( "int" );
		static wxString PROPERTY_TYPE_FLOAT = wxT( "float" );
		static wxString PROPERTY_TYPE_DOUBLE = wxT( "double" );
		static wxString PROPERTY_TYPE_BVEC2 = wxT( "bvec2" );
		static wxString PROPERTY_TYPE_BVEC3 = wxT( "bvec3" );
		static wxString PROPERTY_TYPE_BVEC4 = wxT( "bvec4" );
		static wxString PROPERTY_TYPE_IVEC2 = wxT( "ivec2" );
		static wxString PROPERTY_TYPE_IVEC3 = wxT( "ivec3" );
		static wxString PROPERTY_TYPE_IVEC4 = wxT( "ivec4" );
		static wxString PROPERTY_TYPE_UIVEC2 = wxT( "uivec2" );
		static wxString PROPERTY_TYPE_UIVEC3 = wxT( "uivec3" );
		static wxString PROPERTY_TYPE_UIVEC4 = wxT( "uivec4" );
		static wxString PROPERTY_TYPE_VEC2  = wxT( "vec2" );
		static wxString PROPERTY_TYPE_VEC3  = wxT( "vec3" );
		static wxString PROPERTY_TYPE_VEC4  = wxT( "vec4" );
		static wxString PROPERTY_TYPE_DVEC2 = wxT( "dvec2" );
		static wxString PROPERTY_TYPE_DVEC3 = wxT( "dvec3" );
		static wxString PROPERTY_TYPE_DVEC4 = wxT( "dvec4" );
		static wxString PROPERTY_TYPE_BMAT2 = wxT( "bmat2x2" );
		static wxString PROPERTY_TYPE_BMAT3 = wxT( "bmat3x3" );
		static wxString PROPERTY_TYPE_BMAT4 = wxT( "bmat4x4" );
		static wxString PROPERTY_TYPE_IMAT2 = wxT( "imat2x2" );
		static wxString PROPERTY_TYPE_IMAT3 = wxT( "imat3x3" );
		static wxString PROPERTY_TYPE_IMAT4 = wxT( "imat4x4" );
		static wxString PROPERTY_TYPE_UIMAT2 = wxT( "uimat2x2" );
		static wxString PROPERTY_TYPE_UIMAT3 = wxT( "uimat3x3" );
		static wxString PROPERTY_TYPE_UIMAT4 = wxT( "uimat4x4" );
		static wxString PROPERTY_TYPE_MAT2  = wxT( "mat2x2" );
		static wxString PROPERTY_TYPE_MAT3  = wxT( "mat3x3" );
		static wxString PROPERTY_TYPE_MAT4  = wxT( "mat4x4" );
		static wxString PROPERTY_TYPE_DMAT2 = wxT( "dmat2x2" );
		static wxString PROPERTY_TYPE_DMAT3 = wxT( "dmat3x3" );
		static wxString PROPERTY_TYPE_DMAT4 = wxT( "dmat4x4" );
		static wxString PROPERTY_TYPE_SAMPLER = wxT( "sampler" );

		wxPGProperty * DoBuildValueProperty( wxString const & p_name, FrameVariableSPtr p_variable )
		{
			wxPGProperty * l_return = nullptr;

			switch ( p_variable->GetFullType() )
			{
			case FrameVariableType::Bool:
				l_return = new wxBoolProperty( p_name, p_name, std::static_pointer_cast< OneBoolFrameVariable >( p_variable )->GetValue() );
				break;

			case FrameVariableType::Int:
				l_return = new wxIntProperty( p_name, p_name, std::static_pointer_cast< OneIntFrameVariable >( p_variable )->GetValue() );
				break;

			case FrameVariableType::UInt:
				l_return = new wxUIntProperty( p_name, p_name, std::static_pointer_cast< OneUIntFrameVariable >( p_variable )->GetValue() );
				break;

			case FrameVariableType::Float:
				l_return = new wxFloatProperty( p_name, p_name, std::static_pointer_cast< OneFloatFrameVariable >( p_variable )->GetValue() );
				break;

			case FrameVariableType::Double:
				l_return = new wxFloatProperty( p_name, p_name, std::static_pointer_cast< OneDoubleFrameVariable >( p_variable )->GetValue() );
				break;

			case FrameVariableType::Sampler:
				l_return = new wxFloatProperty( p_name, p_name, std::static_pointer_cast< OneIntFrameVariable >( p_variable )->GetValue() );
				break;

			case FrameVariableType::Vec2b:
				l_return = new BoolPointProperty< 2 >( GC_POINT_XY, p_name, p_name, std::static_pointer_cast< Point2bFrameVariable >( p_variable )->GetValue() );
				break;

			case FrameVariableType::Vec3b:
				l_return = new BoolPointProperty< 3 >( GC_POINT_XYZ, p_name, p_name, std::static_pointer_cast< Point3bFrameVariable >( p_variable )->GetValue() );
				break;

			case FrameVariableType::Vec4b:
				l_return = new BoolPointProperty< 4 >( GC_POINT_XYZW, p_name, p_name, std::static_pointer_cast< Point4bFrameVariable >( p_variable )->GetValue() );
				break;

			case FrameVariableType::Vec2i:
				l_return = new IntPointProperty< 2 >( GC_POINT_XY, p_name, p_name, std::static_pointer_cast< Point2iFrameVariable >( p_variable )->GetValue() );
				break;

			case FrameVariableType::Vec3i:
				l_return = new IntPointProperty< 3 >( GC_POINT_XYZ, p_name, p_name, std::static_pointer_cast< Point3iFrameVariable >( p_variable )->GetValue() );
				break;

			case FrameVariableType::Vec4i:
				l_return = new IntPointProperty< 4 >( GC_POINT_XYZW, p_name, p_name, std::static_pointer_cast< Point4iFrameVariable >( p_variable )->GetValue() );
				break;

			case FrameVariableType::Vec2ui:
				l_return = new UIntPointProperty< 2 >( GC_POINT_XY, p_name, p_name, std::static_pointer_cast< Point2uiFrameVariable >( p_variable )->GetValue() );
				break;

			case FrameVariableType::Vec3ui:
				l_return = new UIntPointProperty< 3 >( GC_POINT_XYZ, p_name, p_name, std::static_pointer_cast< Point3uiFrameVariable >( p_variable )->GetValue() );
				break;

			case FrameVariableType::Vec4ui:
				l_return = new UIntPointProperty< 4 >( GC_POINT_XYZW, p_name, p_name, std::static_pointer_cast< Point4uiFrameVariable >( p_variable )->GetValue() );
				break;

			case FrameVariableType::Vec2f:
				l_return = new FloatPointProperty< 2 >( GC_POINT_XY, p_name, p_name, std::static_pointer_cast< Point2fFrameVariable >( p_variable )->GetValue() );
				break;

			case FrameVariableType::Vec3f:
				l_return = new FloatPointProperty< 3 >( GC_POINT_XYZ, p_name, p_name, std::static_pointer_cast< Point3fFrameVariable >( p_variable )->GetValue() );
				break;

			case FrameVariableType::Vec4f:
				l_return = new FloatPointProperty< 4 >( GC_POINT_XYZW, p_name, p_name, std::static_pointer_cast< Point4fFrameVariable >( p_variable )->GetValue() );
				break;

			case FrameVariableType::Vec2d:
				l_return = new DoublePointProperty< 2 >( GC_POINT_XY, p_name, p_name, std::static_pointer_cast< Point2dFrameVariable >( p_variable )->GetValue() );
				break;

			case FrameVariableType::Vec3d:
				l_return = new DoublePointProperty< 3 >( GC_POINT_XYZ, p_name, p_name, std::static_pointer_cast< Point3dFrameVariable >( p_variable )->GetValue() );
				break;

			case FrameVariableType::Vec4d:
				l_return = new DoublePointProperty< 4 >( GC_POINT_XYZW, p_name, p_name, std::static_pointer_cast< Point4dFrameVariable >( p_variable )->GetValue() );
				break;

			case FrameVariableType::Mat2x2f:
				l_return = new FloatMatrixProperty< 2 >( GC_POINT_12, GC_POINT_12, p_name, p_name, Matrix2x2f{ std::static_pointer_cast< Matrix2x2fFrameVariable >( p_variable )->GetValue() } );
				break;

			case FrameVariableType::Mat3x3f:
				l_return = new FloatMatrixProperty< 3 >( GC_POINT_123, GC_POINT_123, p_name, p_name, Matrix3x3f{ std::static_pointer_cast< Matrix3x3fFrameVariable >( p_variable )->GetValue() } );
				break;

			case FrameVariableType::Mat4x4f:
				l_return = new FloatMatrixProperty< 4 >( GC_POINT_1234, GC_POINT_1234, p_name, p_name, Matrix4x4f{ std::static_pointer_cast< Matrix4x4fFrameVariable >( p_variable )->GetValue() } );
				break;

			case FrameVariableType::Mat2x2d:
				l_return = new DoubleMatrixProperty< 2 >( GC_POINT_12, GC_POINT_12, p_name, p_name, Matrix2x2d{ std::static_pointer_cast< Matrix2x2dFrameVariable >( p_variable )->GetValue() } );
				break;

			case FrameVariableType::Mat3x3d:
				l_return = new DoubleMatrixProperty< 3 >( GC_POINT_123, GC_POINT_123, p_name, p_name, Matrix3x3d{ std::static_pointer_cast< Matrix3x3dFrameVariable >( p_variable )->GetValue() } );
				break;

			case FrameVariableType::Mat4x4d:
				l_return = new DoubleMatrixProperty< 4 >( GC_POINT_1234, GC_POINT_1234, p_name, p_name, Matrix4x4d{ std::static_pointer_cast< Matrix4x4dFrameVariable >( p_variable )->GetValue() } );
				break;
			}

			return l_return;
		}

		void DoSetValue( FrameVariableSPtr p_variable, wxVariant const & p_value, int p_index = 0 )
		{
			switch ( p_variable->GetFullType() )
			{
			case FrameVariableType::Bool:
				std::static_pointer_cast< OneBoolFrameVariable >( p_variable )->SetValue( GetValue< bool >( p_value ), p_index );
				break;

			case FrameVariableType::Int:
				std::static_pointer_cast< OneIntFrameVariable >( p_variable )->SetValue( GetValue< int >( p_value ), p_index );
				break;

			case FrameVariableType::UInt:
				std::static_pointer_cast< OneUIntFrameVariable >( p_variable )->SetValue( GetValue< uint32_t >( p_value ), p_index );
				break;

			case FrameVariableType::Float:
				std::static_pointer_cast< OneFloatFrameVariable >( p_variable )->SetValue( GetValue< float >( p_value ), p_index );
				break;

			case FrameVariableType::Double:
				std::static_pointer_cast< OneDoubleFrameVariable >( p_variable )->SetValue( GetValue< double >( p_value ), p_index );
				break;

			case FrameVariableType::Sampler:
				// TODO
				break;

			case FrameVariableType::Vec2b:
				std::static_pointer_cast< Point2bFrameVariable >( p_variable )->SetValue( PointRefFromVariant< bool, 2 >( p_value ), p_index );
				break;

			case FrameVariableType::Vec3b:
				std::static_pointer_cast< Point3bFrameVariable >( p_variable )->SetValue( PointRefFromVariant< bool, 3 >( p_value ), p_index );
				break;

			case FrameVariableType::Vec4b:
				std::static_pointer_cast< Point4bFrameVariable >( p_variable )->SetValue( PointRefFromVariant< bool, 4 >( p_value ), p_index );
				break;

			case FrameVariableType::Vec2i:
				std::static_pointer_cast< Point2iFrameVariable >( p_variable )->SetValue( PointRefFromVariant< int, 2 >( p_value ), p_index );
				break;

			case FrameVariableType::Vec3i:
				std::static_pointer_cast< Point3iFrameVariable >( p_variable )->SetValue( PointRefFromVariant< int, 3 >( p_value ), p_index );
				break;

			case FrameVariableType::Vec4i:
				std::static_pointer_cast< Point4iFrameVariable >( p_variable )->SetValue( PointRefFromVariant< int, 4 >( p_value ), p_index );
				break;

			case FrameVariableType::Vec2ui:
				std::static_pointer_cast< Point2uiFrameVariable >( p_variable )->SetValue( PointRefFromVariant< uint32_t, 2 >( p_value ), p_index );
				break;

			case FrameVariableType::Vec3ui:
				std::static_pointer_cast< Point3uiFrameVariable >( p_variable )->SetValue( PointRefFromVariant< uint32_t, 3 >( p_value ), p_index );
				break;

			case FrameVariableType::Vec4ui:
				std::static_pointer_cast< Point4uiFrameVariable >( p_variable )->SetValue( PointRefFromVariant< uint32_t, 4 >( p_value ), p_index );
				break;

			case FrameVariableType::Vec2f:
				std::static_pointer_cast< Point2fFrameVariable >( p_variable )->SetValue( PointRefFromVariant< float, 2 >( p_value ), p_index );
				break;

			case FrameVariableType::Vec3f:
				std::static_pointer_cast< Point3fFrameVariable >( p_variable )->SetValue( PointRefFromVariant< float, 3 >( p_value ), p_index );
				break;

			case FrameVariableType::Vec4f:
				std::static_pointer_cast< Point4fFrameVariable >( p_variable )->SetValue( PointRefFromVariant< float, 4 >( p_value ), p_index );
				break;

			case FrameVariableType::Vec2d:
				std::static_pointer_cast< Point2dFrameVariable >( p_variable )->SetValue( PointRefFromVariant< double, 2 >( p_value ), p_index );
				break;

			case FrameVariableType::Vec3d:
				std::static_pointer_cast< Point3dFrameVariable >( p_variable )->SetValue( PointRefFromVariant< double, 3 >( p_value ), p_index );
				break;

			case FrameVariableType::Vec4d:
				std::static_pointer_cast< Point4dFrameVariable >( p_variable )->SetValue( PointRefFromVariant< double, 4 >( p_value ), p_index );
				break;

			case FrameVariableType::Mat2x2f:
				std::static_pointer_cast< Matrix2x2fFrameVariable >( p_variable )->SetValue( MatrixRefFromVariant< float, 2 >( p_value ), p_index );
				break;

			case FrameVariableType::Mat3x3f:
				std::static_pointer_cast< Matrix3x3fFrameVariable >( p_variable )->SetValue( MatrixRefFromVariant< float, 3 >( p_value ), p_index );
				break;

			case FrameVariableType::Mat4x4f:
				std::static_pointer_cast< Matrix4x4fFrameVariable >( p_variable )->SetValue( MatrixRefFromVariant< float, 4 >( p_value ), p_index );
				break;

			case FrameVariableType::Mat2x2d:
				std::static_pointer_cast< Matrix2x2dFrameVariable >( p_variable )->SetValue( MatrixRefFromVariant< double, 2 >( p_value ), p_index );
				break;

			case FrameVariableType::Mat3x3d:
				std::static_pointer_cast< Matrix3x3dFrameVariable >( p_variable )->SetValue( MatrixRefFromVariant< double, 3 >( p_value ), p_index );
				break;

			case FrameVariableType::Mat4x4d:
				std::static_pointer_cast< Matrix4x4dFrameVariable >( p_variable )->SetValue( MatrixRefFromVariant< double, 4 >( p_value ), p_index );
				break;
			}
		}
	}

	FrameVariableTreeItemProperty::FrameVariableTreeItemProperty( bool p_editable, Castor3D::FrameVariableSPtr p_variable, FrameVariableBufferSPtr p_buffer )
		: TreeItemProperty( p_variable->GetProgram()->GetRenderSystem()->GetEngine(), p_editable, ePROPERTY_DATA_TYPE_CAMERA )
		, m_variable( p_variable )
		, m_type( eSHADER_TYPE_COUNT )
		, m_buffer( p_buffer )
	{
		PROPERTY_CATEGORY_VARIABLE = _( "Frame Variable: " );
		PROPERTY_NAME = _( "Name" );
		PROPERTY_VALUE = _( "Value" );
		PROPERTY_TYPE = _( "Type" );
		PROPERTY_OCCURENCES = _( "Occurences" );
		PROPERTY_TYPE_BOOL = wxT( "bool" );
		PROPERTY_TYPE_INT = wxT( "int" );
		PROPERTY_TYPE_FLOAT = wxT( "float" );
		PROPERTY_TYPE_DOUBLE = wxT( "double" );
		PROPERTY_TYPE_BVEC2 = wxT( "bvec2" );
		PROPERTY_TYPE_BVEC3 = wxT( "bvec3" );
		PROPERTY_TYPE_BVEC4 = wxT( "bvec4" );
		PROPERTY_TYPE_IVEC2 = wxT( "ivec2" );
		PROPERTY_TYPE_IVEC3 = wxT( "ivec3" );
		PROPERTY_TYPE_IVEC4 = wxT( "ivec4" );
		PROPERTY_TYPE_UIVEC2 = wxT( "uivec2" );
		PROPERTY_TYPE_UIVEC3 = wxT( "uivec3" );
		PROPERTY_TYPE_UIVEC4 = wxT( "uivec4" );
		PROPERTY_TYPE_VEC2  = wxT( "vec2" );
		PROPERTY_TYPE_VEC3  = wxT( "vec3" );
		PROPERTY_TYPE_VEC4  = wxT( "vec4" );
		PROPERTY_TYPE_DVEC2 = wxT( "dvec2" );
		PROPERTY_TYPE_DVEC3 = wxT( "dvec3" );
		PROPERTY_TYPE_DVEC4 = wxT( "dvec4" );
		PROPERTY_TYPE_BMAT2 = wxT( "bmat2x2" );
		PROPERTY_TYPE_BMAT3 = wxT( "bmat3x3" );
		PROPERTY_TYPE_BMAT4 = wxT( "bmat4x4" );
		PROPERTY_TYPE_IMAT2 = wxT( "imat2x2" );
		PROPERTY_TYPE_IMAT3 = wxT( "imat3x3" );
		PROPERTY_TYPE_IMAT4 = wxT( "imat4x4" );
		PROPERTY_TYPE_UIMAT2 = wxT( "uimat2x2" );
		PROPERTY_TYPE_UIMAT3 = wxT( "uimat3x3" );
		PROPERTY_TYPE_UIMAT4 = wxT( "uimat4x4" );
		PROPERTY_TYPE_MAT2  = wxT( "mat2x2" );
		PROPERTY_TYPE_MAT3  = wxT( "mat3x3" );
		PROPERTY_TYPE_MAT4  = wxT( "mat4x4" );
		PROPERTY_TYPE_DMAT2 = wxT( "dmat2x2" );
		PROPERTY_TYPE_DMAT3 = wxT( "dmat3x3" );
		PROPERTY_TYPE_DMAT4 = wxT( "dmat4x4" );
		PROPERTY_TYPE_SAMPLER = wxT( "sampler" );

		CreateTreeItemMenu();
	}

	FrameVariableTreeItemProperty::FrameVariableTreeItemProperty( bool p_editable, Castor3D::FrameVariableSPtr p_variable, eSHADER_TYPE p_type )
		: TreeItemProperty( p_variable->GetProgram()->GetRenderSystem()->GetEngine(), p_editable, ePROPERTY_DATA_TYPE_CAMERA )
		, m_variable( p_variable )
		, m_type( p_type )
	{
	}

	FrameVariableTreeItemProperty::~FrameVariableTreeItemProperty()
	{
	}

	void FrameVariableTreeItemProperty::DoCreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid )
	{
		FrameVariableSPtr l_variable = GetVariable();

		if ( l_variable )
		{
			wxString l_displayName = l_variable->GetName();

			if ( l_variable->GetOccCount() > 1 )
			{
				l_displayName << wxT( "[" ) << l_variable->GetOccCount() << wxT( "]" );
			}

			p_grid->Append( new wxPropertyCategory( PROPERTY_CATEGORY_VARIABLE + l_displayName ) );
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
			p_grid->Append( new wxIntProperty( PROPERTY_OCCURENCES, wxPG_LABEL, l_variable->GetOccCount() ) )->Enable( false );
		}
	}

	void FrameVariableTreeItemProperty::DoPropertyChange( wxPropertyGridEvent & p_event )
	{
		wxPGProperty * l_property = p_event.GetProperty();
		FrameVariableSPtr l_variable = GetVariable();

		if ( l_property && l_variable )
		{
			if ( l_property->GetName() == PROPERTY_TYPE )
			{
				if ( l_property->GetValueAsString() == PROPERTY_TYPE_BOOL )
				{
					OnTypeChange( FrameVariableType::Bool );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_INT )
				{
					OnTypeChange( FrameVariableType::Int );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_FLOAT )
				{
					OnTypeChange( FrameVariableType::Float );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_DOUBLE )
				{
					OnTypeChange( FrameVariableType::Double );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_BVEC2 )
				{
					OnTypeChange( FrameVariableType::Vec2b );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_BVEC3 )
				{
					OnTypeChange( FrameVariableType::Vec3b );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_BVEC4 )
				{
					OnTypeChange( FrameVariableType::Vec4b );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_IVEC2 )
				{
					OnTypeChange( FrameVariableType::Vec2i );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_IVEC3 )
				{
					OnTypeChange( FrameVariableType::Vec3i );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_IVEC4 )
				{
					OnTypeChange( FrameVariableType::Vec4i );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_UIVEC2 )
				{
					OnTypeChange( FrameVariableType::Vec2ui );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_UIVEC3 )
				{
					OnTypeChange( FrameVariableType::Vec3ui );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_UIVEC4 )
				{
					OnTypeChange( FrameVariableType::Vec4ui );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_VEC2 )
				{
					OnTypeChange( FrameVariableType::Vec2f );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_VEC3 )
				{
					OnTypeChange( FrameVariableType::Vec3f );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_VEC4 )
				{
					OnTypeChange( FrameVariableType::Vec4f );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_DVEC2 )
				{
					OnTypeChange( FrameVariableType::Vec2d );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_DVEC3 )
				{
					OnTypeChange( FrameVariableType::Vec3d );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_DVEC4 )
				{
					OnTypeChange( FrameVariableType::Vec4d );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_MAT2 )
				{
					OnTypeChange( FrameVariableType::Mat2x2f );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_MAT3 )
				{
					OnTypeChange( FrameVariableType::Mat3x3f );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_MAT4 )
				{
					OnTypeChange( FrameVariableType::Mat4x4f );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_DMAT2 )
				{
					OnTypeChange( FrameVariableType::Mat2x2d );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_DMAT3 )
				{
					OnTypeChange( FrameVariableType::Mat3x3d );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_DMAT4 )
				{
					OnTypeChange( FrameVariableType::Mat4x4d );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_SAMPLER )
				{
					OnTypeChange( FrameVariableType::Sampler );
				}
			}
			else if ( l_property->GetName() == PROPERTY_NAME )
			{
				OnNameChange( String( l_property->GetValueAsString() ) );
			}
			else if ( l_property->GetName() == PROPERTY_VALUE )
			{
				OnValueChange( l_property->GetValue() );
			}
		}
	}

	void FrameVariableTreeItemProperty::OnTypeChange( FrameVariableType p_value )
	{
		FrameVariableSPtr l_variable = GetVariable();
		FrameVariableBufferSPtr l_buffer = GetBuffer();

		if ( l_buffer )
		{
			DoApplyChange( [&p_value, l_variable, &l_buffer, this]()
			{
				l_buffer->RemoveVariable( l_variable->GetName() );
				m_variable = l_buffer->CreateVariable( *l_variable->GetProgram(), p_value, l_variable->GetName(), l_variable->GetOccCount() );
			} );
		}
	}

	void FrameVariableTreeItemProperty::OnNameChange( String const & p_value )
	{
		FrameVariableSPtr l_variable = GetVariable();
		FrameVariableBufferSPtr l_buffer = GetBuffer();

		if ( l_buffer )
		{
			DoApplyChange( [&p_value, l_variable, &l_buffer, this]()
			{
				l_buffer->RemoveVariable( l_variable->GetName() );
				m_variable = l_buffer->CreateVariable( *l_variable->GetProgram(), l_variable->GetFullType(), p_value, l_variable->GetOccCount() );
			} );
		}
	}

	void FrameVariableTreeItemProperty::OnValueChange( wxVariant const & p_value )
	{
		FrameVariableSPtr l_variable = GetVariable();

		DoApplyChange( [&p_value, l_variable]()
		{
			DoSetValue( l_variable, p_value );
		} );
	}
}
