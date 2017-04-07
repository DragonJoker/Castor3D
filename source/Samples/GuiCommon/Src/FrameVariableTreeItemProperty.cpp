#include "FrameVariableTreeItemProperty.hpp"

#include <Render/RenderSystem.hpp>
#include <Shader/UniformBuffer.hpp>
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

		wxPGProperty * DoBuildValueProperty( wxString const & p_name
			, UniformSPtr p_variable )
		{
			wxPGProperty * l_return = nullptr;

			switch ( p_variable->GetFullType() )
			{
			case UniformType::eBool:
				l_return = new wxBoolProperty( p_name, p_name, std::static_pointer_cast< Uniform1b >( p_variable )->GetValue() );
				break;

			case UniformType::eInt:
				l_return = new wxIntProperty( p_name, p_name, std::static_pointer_cast< Uniform1i >( p_variable )->GetValue() );
				break;

			case UniformType::eUInt:
				l_return = new wxUIntProperty( p_name, p_name, std::static_pointer_cast< Uniform1ui >( p_variable )->GetValue() );
				break;

			case UniformType::eFloat:
				l_return = new wxFloatProperty( p_name, p_name, std::static_pointer_cast< Uniform1f >( p_variable )->GetValue() );
				break;

			case UniformType::eDouble:
				l_return = new wxFloatProperty( p_name, p_name, std::static_pointer_cast< Uniform1d >( p_variable )->GetValue() );
				break;

			case UniformType::eSampler:
				l_return = new wxFloatProperty( p_name, p_name, std::static_pointer_cast< Uniform1i >( p_variable )->GetValue() );
				break;

			case UniformType::eVec2b:
				l_return = new BoolPointProperty< 2 >( GC_POINT_XY, p_name, p_name, std::static_pointer_cast< Uniform2b >( p_variable )->GetValue() );
				break;

			case UniformType::eVec3b:
				l_return = new BoolPointProperty< 3 >( GC_POINT_XYZ, p_name, p_name, std::static_pointer_cast< Uniform3b >( p_variable )->GetValue() );
				break;

			case UniformType::eVec4b:
				l_return = new BoolPointProperty< 4 >( GC_POINT_XYZW, p_name, p_name, std::static_pointer_cast< Uniform4b >( p_variable )->GetValue() );
				break;

			case UniformType::eVec2i:
				l_return = new IntPointProperty< 2 >( GC_POINT_XY, p_name, p_name, std::static_pointer_cast< Uniform2i >( p_variable )->GetValue() );
				break;

			case UniformType::eVec3i:
				l_return = new IntPointProperty< 3 >( GC_POINT_XYZ, p_name, p_name, std::static_pointer_cast< Uniform3i >( p_variable )->GetValue() );
				break;

			case UniformType::eVec4i:
				l_return = new IntPointProperty< 4 >( GC_POINT_XYZW, p_name, p_name, std::static_pointer_cast< Uniform4i >( p_variable )->GetValue() );
				break;

			case UniformType::eVec2ui:
				l_return = new UIntPointProperty< 2 >( GC_POINT_XY, p_name, p_name, std::static_pointer_cast< Uniform2ui >( p_variable )->GetValue() );
				break;

			case UniformType::eVec3ui:
				l_return = new UIntPointProperty< 3 >( GC_POINT_XYZ, p_name, p_name, std::static_pointer_cast< Uniform3ui >( p_variable )->GetValue() );
				break;

			case UniformType::eVec4ui:
				l_return = new UIntPointProperty< 4 >( GC_POINT_XYZW, p_name, p_name, std::static_pointer_cast< Uniform4ui >( p_variable )->GetValue() );
				break;

			case UniformType::eVec2f:
				l_return = new FloatPointProperty< 2 >( GC_POINT_XY, p_name, p_name, std::static_pointer_cast< Uniform2f >( p_variable )->GetValue() );
				break;

			case UniformType::eVec3f:
				l_return = new FloatPointProperty< 3 >( GC_POINT_XYZ, p_name, p_name, std::static_pointer_cast< Uniform3f >( p_variable )->GetValue() );
				break;

			case UniformType::eVec4f:
				l_return = new FloatPointProperty< 4 >( GC_POINT_XYZW, p_name, p_name, std::static_pointer_cast< Uniform4f >( p_variable )->GetValue() );
				break;

			case UniformType::eVec2d:
				l_return = new DoublePointProperty< 2 >( GC_POINT_XY, p_name, p_name, std::static_pointer_cast< Uniform2d >( p_variable )->GetValue() );
				break;

			case UniformType::eVec3d:
				l_return = new DoublePointProperty< 3 >( GC_POINT_XYZ, p_name, p_name, std::static_pointer_cast< Uniform3d >( p_variable )->GetValue() );
				break;

			case UniformType::eVec4d:
				l_return = new DoublePointProperty< 4 >( GC_POINT_XYZW, p_name, p_name, std::static_pointer_cast< Uniform4d >( p_variable )->GetValue() );
				break;

			case UniformType::eMat2x2f:
				l_return = new FloatMatrixProperty< 2 >( GC_POINT_12, GC_POINT_12, p_name, p_name, Matrix2x2f{ std::static_pointer_cast< Uniform2x2f >( p_variable )->GetValue() } );
				break;

			case UniformType::eMat3x3f:
				l_return = new FloatMatrixProperty< 3 >( GC_POINT_123, GC_POINT_123, p_name, p_name, Matrix3x3f{ std::static_pointer_cast< Uniform3x3f >( p_variable )->GetValue() } );
				break;

			case UniformType::eMat4x4f:
				l_return = new FloatMatrixProperty< 4 >( GC_POINT_1234, GC_POINT_1234, p_name, p_name, Matrix4x4f{ std::static_pointer_cast< Uniform4x4f >( p_variable )->GetValue() } );
				break;

			case UniformType::eMat2x2d:
				l_return = new DoubleMatrixProperty< 2 >( GC_POINT_12, GC_POINT_12, p_name, p_name, Matrix2x2d{ std::static_pointer_cast< Uniform2x2d >( p_variable )->GetValue() } );
				break;

			case UniformType::eMat3x3d:
				l_return = new DoubleMatrixProperty< 3 >( GC_POINT_123, GC_POINT_123, p_name, p_name, Matrix3x3d{ std::static_pointer_cast< Uniform3x3d >( p_variable )->GetValue() } );
				break;

			case UniformType::eMat4x4d:
				l_return = new DoubleMatrixProperty< 4 >( GC_POINT_1234, GC_POINT_1234, p_name, p_name, Matrix4x4d{ std::static_pointer_cast< Uniform4x4d >( p_variable )->GetValue() } );
				break;
			}

			return l_return;
		}

		void DoSetValue( UniformSPtr p_variable, wxVariant const & p_value, int p_index = 0 )
		{
			switch ( p_variable->GetFullType() )
			{
			case UniformType::eBool:
				std::static_pointer_cast< Uniform1b >( p_variable )->SetValue( GetValue< bool >( p_value ), p_index );
				break;

			case UniformType::eInt:
				std::static_pointer_cast< Uniform1i >( p_variable )->SetValue( GetValue< int >( p_value ), p_index );
				break;

			case UniformType::eUInt:
				std::static_pointer_cast< Uniform1ui >( p_variable )->SetValue( GetValue< uint32_t >( p_value ), p_index );
				break;

			case UniformType::eFloat:
				std::static_pointer_cast< Uniform1f >( p_variable )->SetValue( GetValue< float >( p_value ), p_index );
				break;

			case UniformType::eDouble:
				std::static_pointer_cast< Uniform1d >( p_variable )->SetValue( GetValue< double >( p_value ), p_index );
				break;

			case UniformType::eSampler:
				// TODO
				break;

			case UniformType::eVec2b:
				std::static_pointer_cast< Uniform2b >( p_variable )->SetValue( PointRefFromVariant< bool, 2 >( p_value ), p_index );
				break;

			case UniformType::eVec3b:
				std::static_pointer_cast< Uniform3b >( p_variable )->SetValue( PointRefFromVariant< bool, 3 >( p_value ), p_index );
				break;

			case UniformType::eVec4b:
				std::static_pointer_cast< Uniform4b >( p_variable )->SetValue( PointRefFromVariant< bool, 4 >( p_value ), p_index );
				break;

			case UniformType::eVec2i:
				std::static_pointer_cast< Uniform2i >( p_variable )->SetValue( PointRefFromVariant< int, 2 >( p_value ), p_index );
				break;

			case UniformType::eVec3i:
				std::static_pointer_cast< Uniform3i >( p_variable )->SetValue( PointRefFromVariant< int, 3 >( p_value ), p_index );
				break;

			case UniformType::eVec4i:
				std::static_pointer_cast< Uniform4i >( p_variable )->SetValue( PointRefFromVariant< int, 4 >( p_value ), p_index );
				break;

			case UniformType::eVec2ui:
				std::static_pointer_cast< Uniform2ui >( p_variable )->SetValue( PointRefFromVariant< uint32_t, 2 >( p_value ), p_index );
				break;

			case UniformType::eVec3ui:
				std::static_pointer_cast< Uniform3ui >( p_variable )->SetValue( PointRefFromVariant< uint32_t, 3 >( p_value ), p_index );
				break;

			case UniformType::eVec4ui:
				std::static_pointer_cast< Uniform4ui >( p_variable )->SetValue( PointRefFromVariant< uint32_t, 4 >( p_value ), p_index );
				break;

			case UniformType::eVec2f:
				std::static_pointer_cast< Uniform2f >( p_variable )->SetValue( PointRefFromVariant< float, 2 >( p_value ), p_index );
				break;

			case UniformType::eVec3f:
				std::static_pointer_cast< Uniform3f >( p_variable )->SetValue( PointRefFromVariant< float, 3 >( p_value ), p_index );
				break;

			case UniformType::eVec4f:
				std::static_pointer_cast< Uniform4f >( p_variable )->SetValue( PointRefFromVariant< float, 4 >( p_value ), p_index );
				break;

			case UniformType::eVec2d:
				std::static_pointer_cast< Uniform2d >( p_variable )->SetValue( PointRefFromVariant< double, 2 >( p_value ), p_index );
				break;

			case UniformType::eVec3d:
				std::static_pointer_cast< Uniform3d >( p_variable )->SetValue( PointRefFromVariant< double, 3 >( p_value ), p_index );
				break;

			case UniformType::eVec4d:
				std::static_pointer_cast< Uniform4d >( p_variable )->SetValue( PointRefFromVariant< double, 4 >( p_value ), p_index );
				break;

			case UniformType::eMat2x2f:
				std::static_pointer_cast< Uniform2x2f >( p_variable )->SetValue( MatrixRefFromVariant< float, 2 >( p_value ), p_index );
				break;

			case UniformType::eMat3x3f:
				std::static_pointer_cast< Uniform3x3f >( p_variable )->SetValue( MatrixRefFromVariant< float, 3 >( p_value ), p_index );
				break;

			case UniformType::eMat4x4f:
				std::static_pointer_cast< Uniform4x4f >( p_variable )->SetValue( MatrixRefFromVariant< float, 4 >( p_value ), p_index );
				break;

			case UniformType::eMat2x2d:
				std::static_pointer_cast< Uniform2x2d >( p_variable )->SetValue( MatrixRefFromVariant< double, 2 >( p_value ), p_index );
				break;

			case UniformType::eMat3x3d:
				std::static_pointer_cast< Uniform3x3d >( p_variable )->SetValue( MatrixRefFromVariant< double, 3 >( p_value ), p_index );
				break;

			case UniformType::eMat4x4d:
				std::static_pointer_cast< Uniform4x4d >( p_variable )->SetValue( MatrixRefFromVariant< double, 4 >( p_value ), p_index );
				break;
			}
		}
	}

	FrameVariableTreeItemProperty::FrameVariableTreeItemProperty( bool p_editable
		, UniformSPtr p_variable
		, UniformBuffer & p_buffer )
		: TreeItemProperty( p_buffer.GetRenderSystem()->GetEngine(), p_editable, ePROPERTY_DATA_TYPE_CAMERA )
		, m_variable( p_variable )
		, m_type( ShaderType::eCount )
		, m_buffer( &p_buffer )
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

	FrameVariableTreeItemProperty::FrameVariableTreeItemProperty( bool p_editable
		, PushUniformSPtr p_variable
		, ShaderType p_type )
		: TreeItemProperty( p_variable->GetProgram().GetRenderSystem()->GetEngine(), p_editable, ePROPERTY_DATA_TYPE_CAMERA )
		, m_pushVariable( p_variable )
		, m_type( p_type )
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
		PROPERTY_TYPE_VEC2 = wxT( "vec2" );
		PROPERTY_TYPE_VEC3 = wxT( "vec3" );
		PROPERTY_TYPE_VEC4 = wxT( "vec4" );
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
		PROPERTY_TYPE_MAT2 = wxT( "mat2x2" );
		PROPERTY_TYPE_MAT3 = wxT( "mat3x3" );
		PROPERTY_TYPE_MAT4 = wxT( "mat4x4" );
		PROPERTY_TYPE_DMAT2 = wxT( "dmat2x2" );
		PROPERTY_TYPE_DMAT3 = wxT( "dmat3x3" );
		PROPERTY_TYPE_DMAT4 = wxT( "dmat4x4" );
		PROPERTY_TYPE_SAMPLER = wxT( "sampler" );
	}

	FrameVariableTreeItemProperty::~FrameVariableTreeItemProperty()
	{
	}

	void FrameVariableTreeItemProperty::DoCreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid )
	{
		UniformSPtr l_variable = GetVariable();

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
			l_type->Enable( m_buffer != nullptr );
			l_type->SetValue( l_selected );
			p_grid->Append( l_type );

			p_grid->Append( DoBuildValueProperty( PROPERTY_VALUE, l_variable ) );
			p_grid->Append( new wxIntProperty( PROPERTY_OCCURENCES, wxPG_LABEL, l_variable->GetOccCount() ) )->Enable( false );
		}
	}

	void FrameVariableTreeItemProperty::DoPropertyChange( wxPropertyGridEvent & p_event )
	{
		wxPGProperty * l_property = p_event.GetProperty();
		UniformSPtr l_variable = GetVariable();

		if ( l_property && l_variable )
		{
			if ( l_property->GetName() == PROPERTY_TYPE )
			{
				if ( l_property->GetValueAsString() == PROPERTY_TYPE_BOOL )
				{
					OnTypeChange( UniformType::eBool );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_INT )
				{
					OnTypeChange( UniformType::eInt );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_FLOAT )
				{
					OnTypeChange( UniformType::eFloat );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_DOUBLE )
				{
					OnTypeChange( UniformType::eDouble );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_BVEC2 )
				{
					OnTypeChange( UniformType::eVec2b );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_BVEC3 )
				{
					OnTypeChange( UniformType::eVec3b );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_BVEC4 )
				{
					OnTypeChange( UniformType::eVec4b );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_IVEC2 )
				{
					OnTypeChange( UniformType::eVec2i );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_IVEC3 )
				{
					OnTypeChange( UniformType::eVec3i );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_IVEC4 )
				{
					OnTypeChange( UniformType::eVec4i );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_UIVEC2 )
				{
					OnTypeChange( UniformType::eVec2ui );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_UIVEC3 )
				{
					OnTypeChange( UniformType::eVec3ui );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_UIVEC4 )
				{
					OnTypeChange( UniformType::eVec4ui );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_VEC2 )
				{
					OnTypeChange( UniformType::eVec2f );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_VEC3 )
				{
					OnTypeChange( UniformType::eVec3f );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_VEC4 )
				{
					OnTypeChange( UniformType::eVec4f );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_DVEC2 )
				{
					OnTypeChange( UniformType::eVec2d );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_DVEC3 )
				{
					OnTypeChange( UniformType::eVec3d );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_DVEC4 )
				{
					OnTypeChange( UniformType::eVec4d );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_MAT2 )
				{
					OnTypeChange( UniformType::eMat2x2f );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_MAT3 )
				{
					OnTypeChange( UniformType::eMat3x3f );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_MAT4 )
				{
					OnTypeChange( UniformType::eMat4x4f );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_DMAT2 )
				{
					OnTypeChange( UniformType::eMat2x2d );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_DMAT3 )
				{
					OnTypeChange( UniformType::eMat3x3d );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_DMAT4 )
				{
					OnTypeChange( UniformType::eMat4x4d );
				}
				else if ( l_property->GetValueAsString() == PROPERTY_TYPE_SAMPLER )
				{
					OnTypeChange( UniformType::eSampler );
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

	void FrameVariableTreeItemProperty::OnTypeChange( UniformType p_value )
	{
		auto l_variable = GetVariable();
		auto l_buffer = GetBuffer();

		if ( l_buffer )
		{
			DoApplyChange( [&p_value, l_variable, &l_buffer, this]()
			{
				l_buffer->RemoveUniform( l_variable->GetName() );
				m_variable = l_buffer->CreateUniform( p_value, l_variable->GetName(), l_variable->GetOccCount() );
			} );
		}
	}

	void FrameVariableTreeItemProperty::OnNameChange( String const & p_value )
	{
		auto l_variable = GetVariable();
		auto l_buffer = GetBuffer();

		if ( l_buffer )
		{
			DoApplyChange( [&p_value, l_variable, &l_buffer, this]()
			{
				l_buffer->RemoveUniform( l_variable->GetName() );
				m_variable = l_buffer->CreateUniform( l_variable->GetFullType(), p_value, l_variable->GetOccCount() );
			} );
		}
	}

	void FrameVariableTreeItemProperty::OnValueChange( wxVariant const & p_value )
	{
		UniformSPtr l_variable = GetVariable();

		DoApplyChange( [&p_value, l_variable]()
		{
			DoSetValue( l_variable, p_value );
		} );
	}
}
