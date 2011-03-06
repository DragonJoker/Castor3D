#include "GuiCommon/PrecompiledHeader.h"

#include "GuiCommon/FrameVariableDialog.h"

using namespace Castor3D;
using namespace GuiCommon;

FrameVariableDialog :: FrameVariableDialog( wxWindow * p_pParent, FrameVariablePtr p_pFrameVariable)
	:	wxDialog( p_pParent, wxID_ANY, CU_T( "Uniform Variable"), wxDefaultPosition, wxSize( 200, 200)),
		m_pFrameVariable( p_pFrameVariable),
		m_bOwn( p_pFrameVariable == NULL)
{
	wxArrayString l_arrayChoices;
	l_arrayChoices.push_back( CU_T( "int"));
	l_arrayChoices.push_back( CU_T( "real"));
	l_arrayChoices.push_back( CU_T( "ivec2"));
	l_arrayChoices.push_back( CU_T( "ivec3"));
	l_arrayChoices.push_back( CU_T( "ivec4"));
	l_arrayChoices.push_back( CU_T( "vec2"));
	l_arrayChoices.push_back( CU_T( "vec3"));
	l_arrayChoices.push_back( CU_T( "vec4"));
	l_arrayChoices.push_back( CU_T( "mat2"));
	l_arrayChoices.push_back( CU_T( "mat3"));
	l_arrayChoices.push_back( CU_T( "mat4"));

	new wxStaticText( this, wxID_ANY, CU_T( "Type"), wxPoint( 10, 10), wxSize( 90, 20));
	m_pComboType = new wxComboBox( this, eType, CU_T( "int"), wxPoint( 100, 10), wxSize( 90, 20), l_arrayChoices, wxCB_READONLY);

	new wxStaticText( this, wxID_ANY, CU_T( "Nom"), wxPoint( 10, 40), wxSize( 90, 20));
	m_pEditName = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxPoint( 100, 40), wxSize( 90, 20), wxBORDER_SIMPLE);

	new wxStaticText( this, wxID_ANY, CU_T( "Valeur"), wxPoint( 10, 70), wxSize( 90, 20));
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
	new wxButton( this, eOK, CU_T( "Valider"), wxPoint( 20, l_size.y - 30), wxSize( 70, 20));
	new wxButton( this, eCancel, CU_T( "Annuler"), wxPoint( l_size.x - 90, l_size.y - 30), wxSize( 70, 20));
}

FrameVariableDialog :: ~FrameVariableDialog()
{
}

BEGIN_EVENT_TABLE( FrameVariableDialog, wxDialog)
	EVT_CLOSE(					FrameVariableDialog::_onClose)
	EVT_BUTTON(		eOK,		FrameVariableDialog::_onOk)
	EVT_BUTTON(		eCancel,	FrameVariableDialog::_onCancel)
	EVT_COMBOBOX(	eType,		FrameVariableDialog::_onSelectType)
END_EVENT_TABLE()

void FrameVariableDialog :: _onClose( wxCloseEvent & event)
{
	EndDialog( wxID_CANCEL);
}

void FrameVariableDialog :: _onOk( wxCommandEvent & event)
{
	if (m_pFrameVariable != NULL)
	{
		m_pFrameVariable->SetName( m_pEditName->GetValue().c_str());
		m_pFrameVariable->SetValue( m_pEditValue->GetValue().c_str());
	}

	EndDialog( wxID_OK);
}

void FrameVariableDialog :: _onCancel( wxCommandEvent & event)
{
	if (m_bOwn && m_pFrameVariable != NULL)
	{
		m_pFrameVariable.reset();
//		delete m_pFrameVariable;
//		m_pFrameVariable = NULL;
	}

	EndDialog( wxID_CANCEL);
}

void FrameVariableDialog :: _onSelectType( wxCommandEvent & event)
{
	m_pFrameVariable.reset();
//	delete m_pFrameVariable;
//	m_pFrameVariable = NULL;

	switch (event.GetInt())
	{
	case 0:
		m_pFrameVariable = FrameVariablePtr( new OneFrameVariable<int>());
		break;

	case 1:
		m_pFrameVariable = FrameVariablePtr( new OneFrameVariable<float>());
		break;

	case 2:
		m_pFrameVariable = FrameVariablePtr( new PointFrameVariable<int, 2>());
		break;

	case 3:
		m_pFrameVariable = FrameVariablePtr( new PointFrameVariable<int, 3>());
		break;

	case 4:
		m_pFrameVariable = FrameVariablePtr( new PointFrameVariable<int, 4>());
		break;

	case 5:
		m_pFrameVariable = FrameVariablePtr( new PointFrameVariable<float, 2>());
		break;

	case 6:
		m_pFrameVariable = FrameVariablePtr( new PointFrameVariable<float, 3>());
		break;

	case 7:
		m_pFrameVariable = FrameVariablePtr( new PointFrameVariable<float, 4>());
		break;

	case 8:
		m_pFrameVariable = FrameVariablePtr( new MatrixFrameVariable<float, 2, 2>());
		break;

	case 9:
		m_pFrameVariable = FrameVariablePtr( new MatrixFrameVariable<float, 3, 3>());
		break;

	case 10:
		m_pFrameVariable = FrameVariablePtr( new MatrixFrameVariable<float, 4, 4>());
		break;

	default:
		break;
	}

	if (m_pFrameVariable != NULL)
	{
		m_pEditName->Show();
		m_pEditValue->Show();
	}
}
