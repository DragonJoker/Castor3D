#include "CastorShape/PrecompiledHeader.h"

#include "CastorShape/geometry/NewIcosaedronDialog.h"

using namespace CastorShape;
using namespace Castor3D;

NewIcosaedronDialog :: NewIcosaedronDialog( MaterialManager * p_pManager, wxWindow * parent, wxWindowID p_id)
	:	NewGeometryDialog( p_pManager, parent, p_id, CU_T( "New icosaedron"))
{
}

NewIcosaedronDialog :: ~NewIcosaedronDialog()
{
}

void NewIcosaedronDialog :: _initialise()
{
	_defaultInit();
	AddTextCtrl( CU_T( "Radius"), CU_T( "1.0"), 40);
	AddTextCtrl( CU_T( "Subdiv Number"), CU_T( "4"), 40);
	_endInit();
}

real NewIcosaedronDialog :: GetIcosaedronRadius()const
{
	wxString l_value = GetTextValue( CU_T( "Radius"));
	
	if (l_value.BeforeFirst( '.').IsNumber() && l_value.AfterFirst( '.').IsNumber())
	{
		return ator( l_value.c_str());
	}
	
	return INT_MIN;
}

int NewIcosaedronDialog :: GetNbSubdiv()const
{
	wxString l_value = GetTextValue( CU_T( "Subdiv Number"));
	
	if (l_value.IsNumber())
	{
		return atoi( l_value.c_str());
	}
	
	return INT_MIN;
}

String NewIcosaedronDialog :: GetNbSubdivStr()const
{
	return String( GetTextValue( CU_T( "Subdiv Number")).c_str());
}


BEGIN_EVENT_TABLE( NewIcosaedronDialog, NewGeometryDialog)
END_EVENT_TABLE()