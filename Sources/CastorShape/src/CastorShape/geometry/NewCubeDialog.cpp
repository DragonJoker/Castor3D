//******************************************************************************
#include "PrecompiledHeaders.h"

#include "geometry/NewCubeDialog.h"
//******************************************************************************

CSNewCubeDialog :: CSNewCubeDialog( wxWindow * parent, wxWindowID p_id)
	:	CSNewGeometryDialog( parent, p_id, CU_T( "New cube"))
{
	AddTextCtrl( CU_T( "Width"), CU_T( "1.0"), 40);
	AddTextCtrl( CU_T( "Height"), CU_T( "1.0"), 40);
	AddTextCtrl( CU_T( "Depth"), CU_T( "1.0"), 40);
/*
	new wxStaticText( this, wxID_ANY, CU_T( "Width :"), wxPoint( 10, 70), wxSize( 50, 20));
	m_widthEdit = new wxTextCtrl( this, ngpWidth, CU_T( "1.0"), wxPoint( 80, 70), wxSize( 40, 20), wxBORDER_SIMPLE);
	new wxStaticText( this, wxID_ANY, CU_T( "Height :"), wxPoint( 10, 100), wxSize( 50, 20));
	m_heightEdit = new wxTextCtrl( this, ngpHeight, CU_T( "1.0"), wxPoint( 80, 100), wxSize( 40, 20), wxBORDER_SIMPLE);
	new wxStaticText( this, wxID_ANY, CU_T( "Depth"), wxPoint( 10, 130), wxSize( 70, 20));
	m_depthEdit = new wxTextCtrl( this, ngpDepth, CU_T( "1.0"), wxPoint( 80, 130), wxSize( 40, 20), wxBORDER_SIMPLE);
	m_okButton->SetPosition( wxPoint( 70, 160));
*/
}

//******************************************************************************

CSNewCubeDialog :: ~CSNewCubeDialog()
{
}

//******************************************************************************

real CSNewCubeDialog :: GetCubeWidth()const
{
	wxString l_value = GetTextValue( CU_T( "Width"));
	
	if (l_value.BeforeFirst( '.').IsNumber() && l_value.AfterFirst( '.').IsNumber())
	{
		return ator( l_value.char_str());
	}
	
	return 0.0f;
}

//******************************************************************************

real CSNewCubeDialog :: GetCubeHeight()const
{
	wxString l_value = GetTextValue( CU_T( "Height"));
	
	if (l_value.BeforeFirst( '.').IsNumber() && l_value.AfterFirst( '.').IsNumber())
	{
		return ator( l_value.char_str());
	}
	
	return 0.0f;
}

//******************************************************************************

real CSNewCubeDialog :: GetCubeDepth()const
{
	wxString l_value = GetTextValue( CU_T( "Depth"));
	
	if (l_value.BeforeFirst( '.').IsNumber() && l_value.AfterFirst( '.').IsNumber())
	{
		return ator( l_value.char_str());
	}
	
	return 0.0f;
}

//******************************************************************************
