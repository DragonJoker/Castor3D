#include "CastorShape/PrecompiledHeader.h"

#include "CastorShape/geometry/NewSphereDialog.h"

using namespace CastorShape;
using namespace Castor3D;

NewSphereDialog :: NewSphereDialog( MaterialManager * p_pManager, wxWindow * parent, wxWindowID p_id)
	:	NewGeometryDialog( p_pManager, parent, p_id, CU_T( "New sphere"))
{
}

NewSphereDialog :: ~NewSphereDialog()
{
}

void NewSphereDialog :: _initialise()
{
	_defaultInit();
	AddTextCtrl( CU_T( "Radius"), CU_T( "1.0"), 40);
	AddTextCtrl( CU_T( "Subdiv"), CU_T( "30"), 40);
	_endInit();
}

real NewSphereDialog :: GetSphereRadius()const
{
	wxString l_value = GetTextValue( CU_T( "Radius"));
	
	if (l_value.BeforeFirst( '.').IsNumber() && l_value.AfterFirst( '.').IsNumber())
	{
		return ator( l_value.c_str());
	}
	
	return INT_MIN;
}

int NewSphereDialog :: GetFacesNumber()const
{
	wxString l_value = GetTextValue( CU_T( "Subdiv"));
	
	if (l_value.IsNumber())
	{
		return atoi( l_value.c_str());
	}
	
	return INT_MIN;
}

String NewSphereDialog :: GetFacesNumberStr()const
{
	return String( GetTextValue( CU_T( "Subdiv")).c_str());
}

BEGIN_EVENT_TABLE( NewSphereDialog, NewGeometryDialog)
END_EVENT_TABLE()