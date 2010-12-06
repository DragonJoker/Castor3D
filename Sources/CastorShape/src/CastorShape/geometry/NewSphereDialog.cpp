//******************************************************************************
#include "PrecompiledHeaders.h"

#include "geometry/NewSphereDialog.h"
//******************************************************************************

CSNewSphereDialog :: CSNewSphereDialog( wxWindow * parent, wxWindowID p_id)
	:	CSNewGeometryDialog( parent, p_id, CU_T( "New sphere"))
{
	AddTextCtrl( CU_T( "Radius"), CU_T( "1.0"), 40);
	AddTextCtrl( CU_T( "Subdiv"), CU_T( "30"), 40);
/*
	new wxStaticText( this, wxID_ANY, CU_T( "Radius :"), wxPoint( 10, 70), wxSize( 50, 20));
	m_radiusEdit = new wxTextCtrl( this, ngpRadius, CU_T( "1.0"), wxPoint( 80, 70), wxSize( 40, 20), wxBORDER_SIMPLE);
	new wxStaticText( this, wxID_ANY, CU_T( "Subdiv :"), wxPoint( 10, 100), wxSize( 70, 20));
	m_facesEdit = new wxTextCtrl( this, ngpNbFaces, CU_T( "30"), wxPoint( 100, 100), wxSize( 40, 20), wxBORDER_SIMPLE);
	m_okButton->SetPosition( wxPoint( 70, 130));
*/
}

//******************************************************************************

CSNewSphereDialog :: ~CSNewSphereDialog()
{
}

//******************************************************************************

real CSNewSphereDialog :: GetSphereRadius()const
{
	wxString l_value = GetTextValue( CU_T( "Radius"));
	
	if (l_value.BeforeFirst( '.').IsNumber() && l_value.AfterFirst( '.').IsNumber())
	{
		return ator( l_value.c_str());
	}
	
	return INT_MIN;
}

//******************************************************************************

int CSNewSphereDialog :: GetFacesNumber()const
{
	wxString l_value = GetTextValue( CU_T( "Subdiv"));
	
	if (l_value.IsNumber())
	{
		return atoi( l_value.c_str());
	}
	
	return INT_MIN;
}

//******************************************************************************

String CSNewSphereDialog :: GetFacesNumberStr()const
{
	return String( GetTextValue( CU_T( "Subdiv")).c_str());
}

//******************************************************************************
