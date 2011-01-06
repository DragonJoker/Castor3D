#include "CastorShape/PrecompiledHeader.h"

#include "CastorShape/geometry/NewTorusDialog.h"

using namespace CastorShape;
using namespace Castor3D;

NewTorusDialog :: NewTorusDialog( MaterialManager * p_pManager, wxWindow * parent, wxWindowID p_id)
	:	NewGeometryDialog( p_pManager, parent, p_id, CU_T( "New torus"))
{
}

NewTorusDialog :: ~NewTorusDialog()
{
}

void NewTorusDialog :: _initialise()
{
	_defaultInit();
	AddTextCtrl( CU_T( "Width"), CU_T( "1.0"), 40);
	AddTextCtrl( CU_T( "Radius"), CU_T( "0.5"), 40);
	AddTextCtrl( CU_T( "Width Subdiv"), CU_T( "20"), 40);
	AddTextCtrl( CU_T( "Radius Subdiv"), CU_T( "20"), 40);
	_endInit();
}

real NewTorusDialog :: GetInternalRadius()const
{
	wxString l_value = GetTextValue( CU_T( "Radius"));
	
	if (l_value.BeforeFirst( '.').IsNumber() && l_value.AfterFirst( '.').IsNumber())
	{
		return ator( l_value.c_str());
	}
	
	return 0.0f;
}

real NewTorusDialog :: GetExternalRadius()const
{
	wxString l_value = GetTextValue( CU_T( "Width"));
	
	if (l_value.BeforeFirst( '.').IsNumber() && l_value.AfterFirst( '.').IsNumber())
	{
		return ator( l_value.c_str());
	}
	
	return 0.0f;
}

int NewTorusDialog :: GetInternalNbFaces()const
{
	wxString l_value = GetTextValue( CU_T( "Width Subdiv"));
	
	if (l_value.IsNumber())
	{
		return atoi( l_value.c_str());
	}
	
	return INT_MIN;
}

int NewTorusDialog :: GetExternalNbFaces()const
{
	wxString l_value = GetTextValue( CU_T( "Radius Subdiv"));
	
	if (l_value.IsNumber())
	{
		return atoi( l_value.c_str());
	}
	
	return INT_MIN;
}

String NewTorusDialog :: GetInternalNbFacesStr()const
{
	return String( GetTextValue( CU_T( "Width Subdiv")).c_str());
}

String NewTorusDialog :: GetExternalNbFacesStr()const
{
	return String( GetTextValue( CU_T( "Radius Subdiv")).c_str());
}


BEGIN_EVENT_TABLE( NewTorusDialog, NewGeometryDialog)
END_EVENT_TABLE()