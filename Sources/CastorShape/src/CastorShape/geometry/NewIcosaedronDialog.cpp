//******************************************************************************
#include "PrecompiledHeaders.h"

#include "geometry/NewIcosaedronDialog.h"
//******************************************************************************

CSNewIcosaedronDialog :: CSNewIcosaedronDialog( wxWindow * parent, wxWindowID p_id)
	:	CSNewGeometryDialog( parent, p_id, C3D_T( "New icosaedron"))
{
	AddTextCtrl( C3D_T( "Radius"), C3D_T( "1.0"), 40);
	AddTextCtrl( C3D_T( "Subdiv Number"), C3D_T( "4"), 40);
/*
	new wxStaticText( this, wxID_ANY, C3D_T( "Radius :"), wxPoint( 10, 70), wxSize( 50, 20));
	m_radiusEdit = new wxTextCtrl( this, ngpRadius, C3D_T( "1.0"), wxPoint( 80, 70), wxSize( 40, 20), wxBORDER_SIMPLE);
	new wxStaticText( this, wxID_ANY, C3D_T( "Subdiv Number"), wxPoint( 10, 100), wxSize( 70, 20));
	m_subdivEdit = new wxTextCtrl( this, ngpNbSubdiv, C3D_T( "4"), wxPoint( 100, 100), wxSize( 40, 20), wxBORDER_SIMPLE);
	m_okButton->SetPosition( wxPoint( 70, 130));
*/
}

//******************************************************************************

CSNewIcosaedronDialog :: ~CSNewIcosaedronDialog()
{
}

//******************************************************************************

float CSNewIcosaedronDialog :: GetIcosaedronRadius()const
{
	wxString l_value = GetTextValue( C3D_T( "Radius"));
	
	if (l_value.BeforeFirst( '.').IsNumber() && l_value.AfterFirst( '.').IsNumber())
	{
		return (float)atof( l_value.char_str());
	}
	
	return INT_MIN;
}

//******************************************************************************

int CSNewIcosaedronDialog :: GetNbSubdiv()const
{
	wxString l_value = GetTextValue( C3D_T( "Subdiv Number"));
	
	if (l_value.IsNumber())
	{
		return atoi( l_value.char_str());
	}
	
	return INT_MIN;
}

//******************************************************************************

String CSNewIcosaedronDialog :: GetNbSubdivStr()const
{
	return String( GetTextValue( C3D_T( "Subdiv Number")).c_str());
}

//******************************************************************************
