#include "CastorShape/PrecompiledHeader.h"

#include "CastorShape/geometry/NewPlaneDialog.h"

using namespace CastorShape;
using namespace Castor3D;

NewPlaneDialog :: NewPlaneDialog( MaterialManager * p_pManager, wxWindow * parent, wxWindowID p_id)
	:	NewGeometryDialog( p_pManager, parent, p_id, CU_T( "New plane"))
{
}

NewPlaneDialog :: ~NewPlaneDialog()
{
}

void NewPlaneDialog :: _initialise()
{
	_defaultInit();
	AddTextCtrl( CU_T( "Width"), CU_T( "1.0"), 40);
	AddTextCtrl( CU_T( "Depth"), CU_T( "1.0"), 40);
	AddTextCtrl( CU_T( "Width Subdiv"), CU_T( "0"), 40);
	AddTextCtrl( CU_T( "Depth Subdiv"), CU_T( "0"), 40);
	_endInit();
}

real NewPlaneDialog :: GetGeometryWidth()const
{
	wxString l_value = GetTextValue( CU_T( "Width"));
	
	if (l_value.BeforeFirst( '.').IsNumber() && l_value.AfterFirst( '.').IsNumber())
	{
		return ator( l_value.c_str());
	}
	
	return 0.0f;
}

real NewPlaneDialog :: GetGeometryDepth()const
{
	wxString l_value = GetTextValue( CU_T( "Depth"));
	
	if (l_value.BeforeFirst( '.').IsNumber() && l_value.AfterFirst( '.').IsNumber())
	{
		return ator( l_value.c_str());
	}
	
	return 0.0f;
}

int NewPlaneDialog :: GetNbWidthSubdiv()const
{
	wxString l_value = GetTextValue( CU_T( "Width Subdiv"));
	
	if (l_value.IsNumber())
	{
		return atoi( l_value.c_str());
	}
	
	return INT_MIN;
}

int NewPlaneDialog :: GetNbDepthSubdiv()const
{
	wxString l_value = GetTextValue( CU_T( "Depth Subdiv"));
	
	if (l_value.IsNumber())
	{
		return atoi( l_value.c_str());
	}
	
	return INT_MIN;
}

String NewPlaneDialog :: GetNbWidthSubdivStr()const
{
	return String( GetTextValue( CU_T( "Width Subdiv")).c_str());
}

String NewPlaneDialog :: GetNbDepthSubdivStr()const
{
	return String( GetTextValue( CU_T( "Depth Subdiv")).c_str());
}


BEGIN_EVENT_TABLE( NewPlaneDialog, NewGeometryDialog)
END_EVENT_TABLE()