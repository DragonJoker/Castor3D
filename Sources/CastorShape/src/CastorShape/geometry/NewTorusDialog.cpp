//******************************************************************************
#include "PrecompiledHeaders.h"

#include "geometry/NewTorusDialog.h"
//******************************************************************************

CSNewTorusDialog :: CSNewTorusDialog( wxWindow * parent, wxWindowID p_id)
	:	CSNewGeometryDialog( parent, p_id, CU_T( "New torus"))
{
	AddTextCtrl( CU_T( "Width"), CU_T( "1.0"), 40);
	AddTextCtrl( CU_T( "Radius"), CU_T( "0.5"), 40);
	AddTextCtrl( CU_T( "Width Subdiv"), CU_T( "20"), 40);
	AddTextCtrl( CU_T( "Radius Subdiv"), CU_T( "20"), 40);
/*
	new wxStaticText( this, wxID_ANY, CU_T( "Width :"), wxPoint( 10, 70), wxSize( 50, 20));
	m_internalRadiusEdit = new wxTextCtrl( this, ngpInternalRadius, CU_T( "0.2"), wxPoint( 80, 70), wxSize( 40, 20), wxBORDER_SIMPLE);
	new wxStaticText( this, wxID_ANY, CU_T( "Radius :"), wxPoint( 10, 100), wxSize( 50, 20));
	m_externalRadiusEdit = new wxTextCtrl( this, ngpExternalRadius, CU_T( "0.5"), wxPoint( 80, 100), wxSize( 40, 20), wxBORDER_SIMPLE);
	new wxStaticText( this, wxID_ANY, CU_T( "Width Subdiv"), wxPoint( 10, 130), wxSize( 70, 20));
	m_internalNbFacesEdit = new wxTextCtrl( this, ngpNbInternalFaces, CU_T( "10"), wxPoint( 100, 130), wxSize( 40, 20), wxBORDER_SIMPLE);
	new wxStaticText( this, wxID_ANY, CU_T( "Radius Subdiv"), wxPoint( 10, 160), wxSize( 70, 20));
	m_externalNbFacesEdit = new wxTextCtrl( this, ngpExternalNbFaces, CU_T( "10"), wxPoint( 100, 160), wxSize( 40, 20), wxBORDER_SIMPLE);
	m_okButton->SetPosition( wxPoint( 70, 190));
*/
}

//******************************************************************************

CSNewTorusDialog :: ~CSNewTorusDialog()
{
}

//******************************************************************************

real CSNewTorusDialog :: GetInternalRadius()const
{
	wxString l_value = GetTextValue( CU_T( "Radius"));
	
	if (l_value.BeforeFirst( '.').IsNumber() && l_value.AfterFirst( '.').IsNumber())
	{
		return ator( l_value.char_str());
	}
	
	return 0.0f;
}

//******************************************************************************

real CSNewTorusDialog :: GetExternalRadius()const
{
	wxString l_value = GetTextValue( CU_T( "Width"));
	
	if (l_value.BeforeFirst( '.').IsNumber() && l_value.AfterFirst( '.').IsNumber())
	{
		return ator( l_value.char_str());
	}
	
	return 0.0f;
}

//******************************************************************************

int CSNewTorusDialog :: GetInternalNbFaces()const
{
	wxString l_value = GetTextValue( CU_T( "Width Subdiv"));
	
	if (l_value.IsNumber())
	{
		return atoi( l_value.char_str());
	}
	
	return INT_MIN;
}

//******************************************************************************

int CSNewTorusDialog :: GetExternalNbFaces()const
{
	wxString l_value = GetTextValue( CU_T( "Radius Subdiv"));
	
	if (l_value.IsNumber())
	{
		return atoi( l_value.char_str());
	}
	
	return INT_MIN;
}

//******************************************************************************

String CSNewTorusDialog :: GetInternalNbFacesStr()const
{
	return String( GetTextValue( CU_T( "Width Subdiv")).c_str());
}

//******************************************************************************

String CSNewTorusDialog :: GetExternalNbFacesStr()const
{
	return String( GetTextValue( CU_T( "Radius Subdiv")).c_str());
}

//******************************************************************************
