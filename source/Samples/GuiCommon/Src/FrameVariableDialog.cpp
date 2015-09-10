#include "FrameVariableDialog.hpp"

#include <FrameVariable.hpp>

using namespace Castor3D;
using namespace GuiCommon;

FrameVariableDialog::FrameVariableDialog( wxWindow * p_pParent, ShaderProgramBaseSPtr p_pProgram, Castor3D::eSHADER_TYPE p_eTargetStage, FrameVariableSPtr p_pFrameVariable )
	:	wxDialog( p_pParent, wxID_ANY, _( "Frame Variable" ), wxDefaultPosition, wxSize( 200, 200 )	)
	,	m_pFrameVariable( p_pFrameVariable	)
	,	m_pProgram( p_pProgram	)
	,	m_bOwn( p_pFrameVariable.use_count() == 0	)
	,	m_eTargetStage( p_eTargetStage	)
{
	wxBoxSizer * l_pSizer = new wxBoxSizer( wxVERTICAL );
	wxArrayString l_arrayChoices;
	wxStaticText * l_pStaticType;
	wxStaticText * l_pStaticName;
	wxStaticText * l_pStaticValue;
	l_arrayChoices.push_back( wxT( "int"	) );
	l_arrayChoices.push_back( wxT( "real"	) );
	l_arrayChoices.push_back( wxT( "ivec2"	) );
	l_arrayChoices.push_back( wxT( "ivec3"	) );
	l_arrayChoices.push_back( wxT( "ivec4"	) );
	l_arrayChoices.push_back( wxT( "vec2"	) );
	l_arrayChoices.push_back( wxT( "vec3"	) );
	l_arrayChoices.push_back( wxT( "vec4"	) );
	l_arrayChoices.push_back( wxT( "mat2"	) );
	l_arrayChoices.push_back( wxT( "mat3"	) );
	l_arrayChoices.push_back( wxT( "mat4"	) );
	l_pStaticType	= new wxStaticText(	this, wxID_ANY,			_( "Type"	),	wxPoint( 10, 10 ), wxSize( 90, 20 ) );
	m_pComboType	= new wxComboBox(	this, eID_COMBO_TYPE,	cuT( "int"	),	wxPoint( 100, 10 ), wxSize( 90, 20 ), l_arrayChoices, wxCB_READONLY );
	l_pStaticName	= new wxStaticText(	this, wxID_ANY,			_( "Name"	),	wxPoint( 10, 40 ), wxSize( 90, 20 ) );
	m_pEditName		= new wxTextCtrl(	this, wxID_ANY,			wxEmptyString,		wxPoint( 100, 40 ), wxSize( 90, 20 ), wxBORDER_SIMPLE );
	l_pStaticValue	= new wxStaticText(	this, wxID_ANY,			_( "Value"	),	wxPoint( 10, 70 ), wxSize( 90, 20 ) );
	m_pEditValue	= new wxTextCtrl(	this, wxID_ANY,			wxEmptyString,		wxPoint( 100, 70 ), wxSize( 90, 20 ), wxBORDER_SIMPLE );

	if ( m_bOwn )
	{
		m_pEditName->Hide();
		m_pEditValue->Hide();
	}
	else
	{
		m_pComboType->Disable();
		m_pEditName->SetValue( m_pFrameVariable.lock()->GetName() );
		m_pEditValue->SetValue( m_pFrameVariable.lock()->GetStrValue() );
	}

	wxFlexGridSizer * l_pSizerGrid = new wxFlexGridSizer( 3, 2, 5, 5 );
	l_pSizerGrid->SetFlexibleDirection( wxHORIZONTAL );
	l_pSizerGrid->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_ALL );
	l_pSizerGrid->Add(	l_pStaticType,	wxSizerFlags( 1 ).Expand()	);
	l_pSizerGrid->Add(	m_pComboType,	wxSizerFlags( 1 ).Expand()	);
	l_pSizerGrid->Add(	l_pStaticName,	wxSizerFlags( 1 ).Expand()	);
	l_pSizerGrid->Add(	m_pEditName,	wxSizerFlags( 1 ).Expand()	);
	l_pSizerGrid->Add(	l_pStaticValue,	wxSizerFlags( 1 ).Expand()	);
	l_pSizerGrid->Add(	m_pEditValue,	wxSizerFlags( 1 ).Expand()	);
	l_pSizer->Add(	l_pSizerGrid,	wxSizerFlags( 1 ).Expand()	);
	wxBoxSizer * l_pSizerBtn = new wxBoxSizer( wxHORIZONTAL );
	l_pSizerBtn->Add(	new wxButton( this, eID_BUTTON_OK,		_( "OK"	) ),	wxSizerFlags( 0 )	);
	l_pSizerBtn->Add(	100, 0,														0	);
	l_pSizerBtn->Add(	new wxButton( this, eID_BUTTON_CANCEL,	_( "Cancel"	) ),	wxSizerFlags( 0 )	);
	l_pSizer->Add(	l_pSizerBtn,												wxSizerFlags( 0 ).Expand()	);
	SetSizer( l_pSizer );
	l_pSizer->SetSizeHints( this );
}

FrameVariableDialog::~FrameVariableDialog()
{
}

BEGIN_EVENT_TABLE( FrameVariableDialog, wxDialog )
	EVT_CLOSE(	FrameVariableDialog::OnClose	)
	EVT_BUTTON(	eID_BUTTON_OK,		FrameVariableDialog::OnOk	)
	EVT_BUTTON(	eID_BUTTON_CANCEL,	FrameVariableDialog::OnCancel	)
	EVT_COMBOBOX(	eID_COMBO_TYPE,		FrameVariableDialog::OnSelectType	)
END_EVENT_TABLE()

void FrameVariableDialog::OnClose( wxCloseEvent & WXUNUSED( p_event ) )
{
	EndDialog( wxID_CANCEL );
}

void FrameVariableDialog::OnOk( wxCommandEvent & WXUNUSED( p_event ) )
{
	FrameVariableSPtr l_pFrameVariable = m_pFrameVariable.lock();

	if ( l_pFrameVariable )
	{
		l_pFrameVariable->SetName( ( wxChar const * )m_pEditName->GetValue().c_str() );
		l_pFrameVariable->SetValueStr( ( wxChar const * )m_pEditValue->GetValue().c_str() );
	}

	EndDialog( wxID_OK );
}

void FrameVariableDialog::OnCancel( wxCommandEvent & WXUNUSED( p_event ) )
{
	m_pFrameVariable.reset();
	EndDialog( wxID_CANCEL );
}

void FrameVariableDialog::OnSelectType( wxCommandEvent & p_event )
{
	m_pFrameVariable.reset();
	ShaderProgramBaseSPtr l_pProgram = m_pProgram.lock();

	switch ( p_event.GetInt() )
	{
// 	case 0:
// 		m_pFrameVariable = l_pProgram->CreateFrameVariable< OneIntFrameVariable >( 1, m_eTargetStage );
// 		break;
//
// 	case 1:
// 		m_pFrameVariable = l_pProgram->CreateFrameVariable< OneFloatFrameVariable >( 1, m_eTargetStage );
// 		break;
//
// 	case 2:
// 		m_pFrameVariable = l_pProgram->CreateFrameVariable< Point2iFrameVariable >( 1, m_eTargetStage );
// 		break;
//
// 	case 3:
// 		m_pFrameVariable = l_pProgram->CreateFrameVariable< Point3iFrameVariable >( 1, m_eTargetStage );
// 		break;
//
// 	case 4:
// 		m_pFrameVariable = l_pProgram->CreateFrameVariable< Point4iFrameVariable >( 1, m_eTargetStage );
// 		break;
//
// 	case 5:
// 		m_pFrameVariable = l_pProgram->CreateFrameVariable< Point2fFrameVariable >( 1, m_eTargetStage );
// 		break;
//
// 	case 6:
// 		m_pFrameVariable = l_pProgram->CreateFrameVariable< Point3fFrameVariable >( 1, m_eTargetStage );
// 		break;
//
// 	case 7:
// 		m_pFrameVariable = l_pProgram->CreateFrameVariable< Point4fFrameVariable >( 1, m_eTargetStage );
// 		break;
//
// 	case 8:
// 		m_pFrameVariable = l_pProgram->CreateFrameVariable< Matrix2x2fFrameVariable >( 1, m_eTargetStage );
// 		break;
//
// 	case 9:
// 		m_pFrameVariable = l_pProgram->CreateFrameVariable< Matrix3x3fFrameVariable >( 1, m_eTargetStage );
// 		break;
//
// 	case 10:
// 		m_pFrameVariable = l_pProgram->CreateFrameVariable< Matrix4x4fFrameVariable >( 1, m_eTargetStage );
// 		break;
//
	default:
		break;
	}

	if ( !m_pFrameVariable.expired() )
	{
		m_pEditName->Show();
		m_pEditValue->Show();
	}
}
