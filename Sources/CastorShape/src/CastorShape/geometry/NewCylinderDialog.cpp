//******************************************************************************
#include "PrecompiledHeaders.h"

#include "geometry/NewCylinderDialog.h"
//******************************************************************************

CSNewCylinderDialog :: CSNewCylinderDialog( wxWindow * parent, wxWindowID p_id)
	:	CSNewGeometryDialog( parent, p_id, CU_T( "New cylinder"))
{
	AddTextCtrl( CU_T( "Radius"), CU_T( "0.5"), 40);
	AddTextCtrl( CU_T( "Height"), CU_T( "1.0"), 40);
	AddTextCtrl( CU_T( "Faces Number"), CU_T( "10"), 40);
/*
	new wxStaticText( this, wxID_ANY, CU_T( "Radius :"), wxPoint( 10, 70), wxSize( 50, 20));
	m_radiusEdit = new wxTextCtrl( this, ngpRadius, CU_T( "0.5"), wxPoint( 80, 70), wxSize( 40, 20), wxBORDER_SIMPLE);
	new wxStaticText( this, wxID_ANY, CU_T( "Height :"), wxPoint( 10, 100), wxSize( 50, 20));
	m_heightEdit = new wxTextCtrl( this, ngpHeight, CU_T( "1.0"), wxPoint( 80, 100), wxSize( 40, 20), wxBORDER_SIMPLE);
	new wxStaticText( this, wxID_ANY, CU_T( "Faces Number"), wxPoint( 10, 130), wxSize( 70, 20));
	m_facesEdit = new wxTextCtrl( this, ngpNbFaces, CU_T( "10"), wxPoint( 100, 130), wxSize( 40, 20), wxBORDER_SIMPLE);
	m_okButton->SetPosition( wxPoint( 70, 160));
*/
}

//******************************************************************************

CSNewCylinderDialog :: ~CSNewCylinderDialog()
{
}

//******************************************************************************

real CSNewCylinderDialog :: GetCylinderRadius()const
{
	wxString l_value = GetTextValue( CU_T( "Radius"));
	
	if (l_value.BeforeFirst( '.').IsNumber() && l_value.AfterFirst( '.').IsNumber())
	{
		return ator( l_value.c_str());
	}
	
	return 0.0f;
}

//******************************************************************************

real CSNewCylinderDialog :: GetCylinderHeight()const
{
	wxString l_value = GetTextValue( CU_T( "Height"));
	
	if (l_value.BeforeFirst( '.').IsNumber() && l_value.AfterFirst( '.').IsNumber())
	{
		return ator( l_value.c_str());
	}
	
	return 0.0f;
}

//******************************************************************************

int CSNewCylinderDialog :: GetFacesNumber()const
{
	wxString l_value = GetTextValue( CU_T( "Faces Number"));
	
	if (l_value.IsNumber())
	{
		return atoi( l_value.c_str());
	}
	
	return INT_MIN;
}

//******************************************************************************

String CSNewCylinderDialog :: GetFacesNumberStr()const
{
	return String( GetTextValue( CU_T( "Faces Number")).c_str());
}

//******************************************************************************
