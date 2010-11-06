#include "PrecompiledHeader.h"

#include "UniformVariableDialog.h"

#ifdef __WXMSW__
#	include <wx/msw/msvcrt.h>      // redefines the new() operator 
#endif

using namespace Castor3D;
using namespace CastorViewer;

UniformVariableDialog :: UniformVariableDialog( wxWindow * p_pParent, UniformVariablePtr p_pUniformVariable)
	:	wxDialog( p_pParent, wxID_ANY, "Uniform Variable", wxDefaultPosition, wxSize( 200, 200)),
		m_pUniformVariable( p_pUniformVariable),
		m_bOwn( p_pUniformVariable.null())
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
		m_pEditName->SetValue( m_pUniformVariable->GetName());
		m_pEditValue->SetValue( m_pUniformVariable->GetStrValue());
	}

	wxSize l_size = GetClientSize();
	new wxButton( this, eOK, "Valider", wxPoint( 20, l_size.y - 30), wxSize( 70, 20));
	new wxButton( this, eCancel, "Annuler", wxPoint( l_size.x - 90, l_size.y - 30), wxSize( 70, 20));
}

UniformVariableDialog :: ~UniformVariableDialog()
{
}

BEGIN_EVENT_TABLE( UniformVariableDialog, wxDialog)
	EVT_CLOSE(					UniformVariableDialog::_onClose)
	EVT_BUTTON(		eOK,		UniformVariableDialog::_onOk)
	EVT_BUTTON(		eCancel,	UniformVariableDialog::_onCancel)
	EVT_COMBOBOX(	eType,		UniformVariableDialog::_onSelectType)
END_EVENT_TABLE()

void UniformVariableDialog :: _onClose( wxCloseEvent & event)
{
	EndDialog( wxID_CANCEL);
}

void UniformVariableDialog :: _onOk( wxCommandEvent & event)
{
	if ( ! m_pUniformVariable.null())
	{
		m_pUniformVariable->SetName( m_pEditName->GetValue().c_str());
		m_pUniformVariable->SetValue( m_pEditValue->GetValue().c_str());
	}

	EndDialog( wxID_OK);
}

void UniformVariableDialog :: _onCancel( wxCommandEvent & event)
{
	if (m_bOwn && ! m_pUniformVariable.null())
	{
		m_pUniformVariable.reset();
//		delete m_pUniformVariable;
//		m_pUniformVariable = NULL;
	}

	EndDialog( wxID_CANCEL);
}

void UniformVariableDialog :: _onSelectType( wxCommandEvent & event)
{
	m_pUniformVariable.reset();
//	delete m_pUniformVariable;
//	m_pUniformVariable = NULL;

	switch (event.GetInt())
	{
	case 0:
		m_pUniformVariable = new OneUniformVariable<int>();
		break;

	case 1:
		m_pUniformVariable = new OneUniformVariable<real>();
		break;

	case 2:
		m_pUniformVariable = new PointUniformVariable<int, 2>();
		break;

	case 3:
		m_pUniformVariable = new PointUniformVariable<int, 3>();
		break;

	case 4:
		m_pUniformVariable = new PointUniformVariable<int, 4>();
		break;

	case 5:
		m_pUniformVariable = new PointUniformVariable<real, 2>();
		break;

	case 6:
		m_pUniformVariable = new PointUniformVariable<real, 3>();
		break;

	case 7:
		m_pUniformVariable = new PointUniformVariable<real, 4>();
		break;

	case 8:
		m_pUniformVariable = new MatrixUniformVariable<real, 2>();
		break;

	case 9:
		m_pUniformVariable = new MatrixUniformVariable<real, 3>();
		break;

	case 10:
		m_pUniformVariable = new MatrixUniformVariable<real, 4>();
		break;

	default:
		break;
	}

	if ( ! m_pUniformVariable.null())
	{
		m_pEditName->Show();
		m_pEditValue->Show();
	}
}