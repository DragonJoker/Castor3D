//******************************************************************************
#include "PrecompiledHeaders.h"

#include "geometry/NewConeDialog.h"
//******************************************************************************

CSNewConeDialog :: CSNewConeDialog( wxWindow * parent, wxWindowID p_id)
	:	CSNewGeometryDialog( parent, p_id, C3D_T( "New cone"))
{
	AddTextCtrl( C3D_T( "Radius"), C3D_T( "0.5"), 40);
	AddTextCtrl( C3D_T( "Height"), C3D_T( "1.0"), 40);
	AddTextCtrl( C3D_T( "Faces Number"), C3D_T( "10"), 40);
/*
	new wxStaticText( this, wxID_ANY, C3D_T( "Radius :"), wxPoint( 10, 70), wxSize( 50, 20));
	m_radiusEdit = new wxTextCtrl( this, ngpRadius, C3D_T( "0.5"), wxPoint( 80, 70), wxSize( 40, 20), wxBORDER_SIMPLE);
	new wxStaticText( this, wxID_ANY, C3D_T( "Height :"), wxPoint( 10, 100), wxSize( 50, 20));
	m_heightEdit = new wxTextCtrl( this, ngpHeight, C3D_T( "1.0"), wxPoint( 80, 100), wxSize( 40, 20), wxBORDER_SIMPLE);
	new wxStaticText( this, wxID_ANY, C3D_T( "Faces Number"), wxPoint( 10, 130), wxSize( 70, 20));
	m_facesEdit = new wxTextCtrl( this, ngpNbFaces, C3D_T( "10"), wxPoint( 100, 130), wxSize( 40, 20), wxBORDER_SIMPLE);
	m_okButton->SetPosition( wxPoint( 70, 160));
*/
}

//******************************************************************************

CSNewConeDialog :: ~CSNewConeDialog()
{
}

//******************************************************************************

float CSNewConeDialog :: GetConeRadius()const
{
	wxString l_value = GetTextValue( C3D_T( "Radius"));
	
	if (l_value.BeforeFirst( '.').IsNumber() && l_value.AfterFirst( '.').IsNumber())
	{
		return (float)atof( l_value.char_str());
	}
	
	return 0.0f;
}

//******************************************************************************

float CSNewConeDialog :: GetConeHeight()const
{
	wxString l_value = GetTextValue( C3D_T( "Height"));
	
	if (l_value.BeforeFirst( '.').IsNumber() && l_value.AfterFirst( '.').IsNumber())
	{
		return (float)atof( l_value.char_str());
	}
	
	return 0.0f;
}

//******************************************************************************

int CSNewConeDialog :: GetFacesNumber()const
{
	wxString l_value = GetTextValue( C3D_T( "Faces Number"));
	
	if (l_value.IsNumber())
	{
		return atoi( l_value.char_str());
	}
	
	return INT_MIN;
}

//******************************************************************************

String CSNewConeDialog :: GetFacesNumberStr()const
{
	return String( GetTextValue( C3D_T( "Faces Number")).c_str());
}

//******************************************************************************
