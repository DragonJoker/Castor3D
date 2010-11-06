//******************************************************************************
#include "PrecompiledHeaders.h"

#include "geometry/NewPlaneDialog.h"
//******************************************************************************

CSNewPlaneDialog :: CSNewPlaneDialog( wxWindow * parent, wxWindowID p_id)
	:	CSNewGeometryDialog( parent, p_id, CU_T( "New plane"))
{
	AddTextCtrl( CU_T( "Width"), CU_T( "1.0"), 40);
	AddTextCtrl( CU_T( "Depth"), CU_T( "1.0"), 40);
	AddTextCtrl( CU_T( "Width Subdiv"), CU_T( "0"), 40);
	AddTextCtrl( CU_T( "Depth Subdiv"), CU_T( "0"), 40);
/*
	new wxStaticText( this, wxID_ANY, CU_T( "Width :"), wxPoint( 10, 70), wxSize( 50, 20));
	m_widthEdit = new wxTextCtrl( this, ngpWidth, CU_T( "1.0"), wxPoint( 80, 40), wxSize( 70, 20), wxBORDER_SIMPLE);
	new wxStaticText( this, wxID_ANY, CU_T( "Depth :"), wxPoint( 10, 100), wxSize( 50, 20));
	m_depthEdit = new wxTextCtrl( this, ngpDepth, CU_T( "1.0"), wxPoint( 80, 70), wxSize( 100, 20), wxBORDER_SIMPLE);
	new wxStaticText( this, wxID_ANY, CU_T( "Width Subdiv"), wxPoint( 10, 130), wxSize( 70, 20));
	m_widthSubdivEdit = new wxTextCtrl( this, ngpNbSubdivWidth, CU_T( "0"), wxPoint( 100, 130), wxSize( 40, 20), wxBORDER_SIMPLE);
	new wxStaticText( this, wxID_ANY, CU_T( "Depth Subdiv"), wxPoint( 10, 160), wxSize( 70, 20));
	m_depthSubdivEdit = new wxTextCtrl( this, ngpNbSubdivDepth, CU_T( "0"), wxPoint( 100, 160), wxSize( 40, 20), wxBORDER_SIMPLE);
	m_okButton->SetPosition( wxPoint( 70, 190));
*/
}

//******************************************************************************

CSNewPlaneDialog :: ~CSNewPlaneDialog()
{
}

//******************************************************************************

real CSNewPlaneDialog :: GetGeometryWidth()const
{
	wxString l_value = GetTextValue( CU_T( "Width"));
	
	if (l_value.BeforeFirst( '.').IsNumber() && l_value.AfterFirst( '.').IsNumber())
	{
		return ator( l_value.char_str());
	}
	
	return 0.0f;
}

//******************************************************************************

real CSNewPlaneDialog :: GetGeometryDepth()const
{
	wxString l_value = GetTextValue( CU_T( "Depth"));
	
	if (l_value.BeforeFirst( '.').IsNumber() && l_value.AfterFirst( '.').IsNumber())
	{
		return ator( l_value.char_str());
	}
	
	return 0.0f;
}

//******************************************************************************

int CSNewPlaneDialog :: GetNbWidthSubdiv()const
{
	wxString l_value = GetTextValue( CU_T( "Width Subdiv"));
	
	if (l_value.IsNumber())
	{
		return atoi( l_value.char_str());
	}
	
	return INT_MIN;
}

//******************************************************************************

int CSNewPlaneDialog :: GetNbDepthSubdiv()const
{
	wxString l_value = GetTextValue( CU_T( "Depth Subdiv"));
	
	if (l_value.IsNumber())
	{
		return atoi( l_value.char_str());
	}
	
	return INT_MIN;
}

//******************************************************************************

String CSNewPlaneDialog :: GetNbWidthSubdivStr()const
{
	return String( GetTextValue( CU_T( "Width Subdiv")).c_str());
}

//******************************************************************************

String CSNewPlaneDialog :: GetNbDepthSubdivStr()const
{
	return String( GetTextValue( CU_T( "Depth Subdiv")).c_str());
}

//******************************************************************************
