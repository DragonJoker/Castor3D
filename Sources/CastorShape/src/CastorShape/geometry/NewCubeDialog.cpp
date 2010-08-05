//******************************************************************************
#include "PrecompiledHeaders.h"

#include "geometry/NewCubeDialog.h"
//******************************************************************************

CSNewCubeDialog :: CSNewCubeDialog( wxWindow * parent, wxWindowID p_id)
	:	CSNewGeometryDialog( parent, p_id, C3D_T( "New cube"))
{
	AddTextCtrl( C3D_T( "Width"), C3D_T( "1.0"), 40);
	AddTextCtrl( C3D_T( "Height"), C3D_T( "1.0"), 40);
	AddTextCtrl( C3D_T( "Depth"), C3D_T( "1.0"), 40);
/*
	new wxStaticText( this, wxID_ANY, C3D_T( "Width :"), wxPoint( 10, 70), wxSize( 50, 20));
	m_widthEdit = new wxTextCtrl( this, ngpWidth, C3D_T( "1.0"), wxPoint( 80, 70), wxSize( 40, 20), wxBORDER_SIMPLE);
	new wxStaticText( this, wxID_ANY, C3D_T( "Height :"), wxPoint( 10, 100), wxSize( 50, 20));
	m_heightEdit = new wxTextCtrl( this, ngpHeight, C3D_T( "1.0"), wxPoint( 80, 100), wxSize( 40, 20), wxBORDER_SIMPLE);
	new wxStaticText( this, wxID_ANY, C3D_T( "Depth"), wxPoint( 10, 130), wxSize( 70, 20));
	m_depthEdit = new wxTextCtrl( this, ngpDepth, C3D_T( "1.0"), wxPoint( 80, 130), wxSize( 40, 20), wxBORDER_SIMPLE);
	m_okButton->SetPosition( wxPoint( 70, 160));
*/
}

//******************************************************************************

CSNewCubeDialog :: ~CSNewCubeDialog()
{
}

//******************************************************************************

float CSNewCubeDialog :: GetCubeWidth()const
{
	wxString l_value = GetTextValue( C3D_T( "Width"));
	
	if (l_value.BeforeFirst( '.').IsNumber() && l_value.AfterFirst( '.').IsNumber())
	{
		return (float)atof( l_value.char_str());
	}
	
	return 0.0f;
}

//******************************************************************************

float CSNewCubeDialog :: GetCubeHeight()const
{
	wxString l_value = GetTextValue( C3D_T( "Height"));
	
	if (l_value.BeforeFirst( '.').IsNumber() && l_value.AfterFirst( '.').IsNumber())
	{
		return (float)atof( l_value.char_str());
	}
	
	return 0.0f;
}

//******************************************************************************

float CSNewCubeDialog :: GetCubeDepth()const
{
	wxString l_value = GetTextValue( C3D_T( "Depth"));
	
	if (l_value.BeforeFirst( '.').IsNumber() && l_value.AfterFirst( '.').IsNumber())
	{
		return (float)atof( l_value.char_str());
	}
	
	return 0.0f;
}

//******************************************************************************
