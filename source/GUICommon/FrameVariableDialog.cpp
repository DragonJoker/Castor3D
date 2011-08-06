#include "GuiCommon/PrecompiledHeader.hpp"

#include "GuiCommon/FrameVariableDialog.hpp"

using namespace Castor3D;
using namespace GuiCommon;

wxFrameVariableDialog :: wxFrameVariableDialog( wxWindow * p_pParent, ShaderProgramPtr p_pProgram, ShaderObjectPtr p_pObject, FrameVariablePtr p_pFrameVariable)
	:	wxDialog( p_pParent, wxID_ANY, wxT( "Frame Variable"), wxDefaultPosition, wxSize( 200, 200))
	,	m_pFrameVariable( p_pFrameVariable)
	,	m_pProgram( p_pProgram)
	,	m_pObject( p_pObject)
	,	m_bOwn( p_pFrameVariable.use_count() == 0)
{
	wxArrayString l_arrayChoices;
	l_arrayChoices.push_back( wxT( "int"));
	l_arrayChoices.push_back( wxT( "real"));
	l_arrayChoices.push_back( wxT( "ivec2"));
	l_arrayChoices.push_back( wxT( "ivec3"));
	l_arrayChoices.push_back( wxT( "ivec4"));
	l_arrayChoices.push_back( wxT( "vec2"));
	l_arrayChoices.push_back( wxT( "vec3"));
	l_arrayChoices.push_back( wxT( "vec4"));
	l_arrayChoices.push_back( wxT( "mat2"));
	l_arrayChoices.push_back( wxT( "mat3"));
	l_arrayChoices.push_back( wxT( "mat4"));

	new wxStaticText( this, wxID_ANY, wxT( "Type"), wxPoint( 10, 10), wxSize( 90, 20));
	m_pComboType = new wxComboBox( this, eID_COMBO_TYPE, cuT( "int"), wxPoint( 100, 10), wxSize( 90, 20), l_arrayChoices, wxCB_READONLY);

	new wxStaticText( this, wxID_ANY, wxT( "Name"), wxPoint( 10, 40), wxSize( 90, 20));
	m_pEditName = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxPoint( 100, 40), wxSize( 90, 20), wxBORDER_SIMPLE);

	new wxStaticText( this, wxID_ANY, wxT( "Value"), wxPoint( 10, 70), wxSize( 90, 20));
	m_pEditValue = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxPoint( 100, 70), wxSize( 90, 20), wxBORDER_SIMPLE);

	if (m_bOwn)
	{
		m_pEditName->Hide();
		m_pEditValue->Hide();
	}
	else
	{
		m_pComboType->Disable();
		m_pEditName->SetValue( m_pFrameVariable->GetName());
		m_pEditValue->SetValue( m_pFrameVariable->GetStrValue());
	}

	wxSize l_size = GetClientSize();
	new wxButton( this, eID_BUTTON_OK, wxT( "OK"), wxPoint( 20, l_size.y - 30), wxSize( 70, 20));
	new wxButton( this, eID_BUTTON_CANCEL, wxT( "Cancel"), wxPoint( l_size.x - 90, l_size.y - 30), wxSize( 70, 20));
}

wxFrameVariableDialog :: ~wxFrameVariableDialog()
{
}

BEGIN_EVENT_TABLE( wxFrameVariableDialog, wxDialog)
	EVT_CLOSE(							wxFrameVariableDialog::_onClose)
	EVT_BUTTON(		eID_BUTTON_OK,		wxFrameVariableDialog::_onOk)
	EVT_BUTTON(		eID_BUTTON_CANCEL,	wxFrameVariableDialog::_onCancel)
	EVT_COMBOBOX(	eID_COMBO_TYPE,		wxFrameVariableDialog::_onSelectType)
END_EVENT_TABLE()

void wxFrameVariableDialog :: _onClose( wxCloseEvent & event)
{
	EndDialog( wxID_CANCEL);
}

void wxFrameVariableDialog :: _onOk( wxCommandEvent & event)
{
	if (m_pFrameVariable)
	{
		m_pFrameVariable->SetName( (const wxChar *)m_pEditName->GetValue().c_str());
		m_pFrameVariable->SetValue( (const wxChar *)m_pEditValue->GetValue().c_str());
	}

	EndDialog( wxID_OK);
}

void wxFrameVariableDialog :: _onCancel( wxCommandEvent & event)
{
	if (m_bOwn && m_pFrameVariable)
	{
		m_pFrameVariable.reset();
	}

	EndDialog( wxID_CANCEL);
}

void wxFrameVariableDialog :: _onSelectType( wxCommandEvent & event)
{
	m_pFrameVariable.reset();

	switch (event.GetInt())
	{
	case 0:
		m_pFrameVariable = m_pProgram->CreateFrameVariable<OneIntFrameVariable>( 1, m_pObject);
		break;

	case 1:
		m_pFrameVariable = m_pProgram->CreateFrameVariable<OneFloatFrameVariable>( 1, m_pObject);
		break;

	case 2:
		m_pFrameVariable = m_pProgram->CreateFrameVariable<Point2iFrameVariable>( 1, m_pObject);
		break;

	case 3:
		m_pFrameVariable = m_pProgram->CreateFrameVariable<Point3iFrameVariable>( 1, m_pObject);
		break;

	case 4:
		m_pFrameVariable = m_pProgram->CreateFrameVariable<Point4iFrameVariable>( 1, m_pObject);
		break;

	case 5:
		m_pFrameVariable = m_pProgram->CreateFrameVariable<Point2fFrameVariable>( 1, m_pObject);
		break;

	case 6:
		m_pFrameVariable = m_pProgram->CreateFrameVariable<Point3fFrameVariable>( 1, m_pObject);
		break;

	case 7:
		m_pFrameVariable = m_pProgram->CreateFrameVariable<Point4fFrameVariable>( 1, m_pObject);
		break;

	case 8:
		m_pFrameVariable = m_pProgram->CreateFrameVariable<Matrix2x2fFrameVariable>( 1, m_pObject);
		break;

	case 9:
		m_pFrameVariable = m_pProgram->CreateFrameVariable<Matrix3x3fFrameVariable>( 1, m_pObject);
		break;

	case 10:
		m_pFrameVariable = m_pProgram->CreateFrameVariable<Matrix4x4fFrameVariable>( 1, m_pObject);
		break;

	default:
		break;
	}

	if (m_pFrameVariable)
	{
		m_pEditName->Show();
		m_pEditValue->Show();
	}
}
