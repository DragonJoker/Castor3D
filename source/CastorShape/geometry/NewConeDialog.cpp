#include "CastorShape/PrecompiledHeader.h"

#include "CastorShape/geometry/NewConeDialog.h"

using namespace CastorShape;
using namespace Castor3D;

NewConeDialog :: NewConeDialog( MaterialManager * p_pManager, wxWindow * parent, wxWindowID p_id)
	:	NewGeometryDialog( p_pManager, parent, p_id, CU_T( "New cone"))
{
}

NewConeDialog :: ~NewConeDialog()
{
}

void NewConeDialog :: _initialise()
{
	_defaultInit();
	AddTextCtrl( CU_T( "Radius"), CU_T( "0.5"), 40);
	AddTextCtrl( CU_T( "Height"), CU_T( "1.0"), 40);
	AddTextCtrl( CU_T( "Faces Number"), CU_T( "10"), 40);
	_endInit();
}

real NewConeDialog :: GetConeRadius()const
{
	wxString l_value = GetTextValue( CU_T( "Radius"));
	
	if (l_value.BeforeFirst( '.').IsNumber() && l_value.AfterFirst( '.').IsNumber())
	{
		return ator( l_value.c_str());
	}
	
	return 0.0f;
}

real NewConeDialog :: GetConeHeight()const
{
	wxString l_value = GetTextValue( CU_T( "Height"));
	
	if (l_value.BeforeFirst( '.').IsNumber() && l_value.AfterFirst( '.').IsNumber())
	{
		return ator( l_value.c_str());
	}
	
	return 0.0f;
}

int NewConeDialog :: GetFacesNumber()const
{
	wxString l_value = GetTextValue( CU_T( "Faces Number"));
	
	if (l_value.IsNumber())
	{
		return atoi( l_value.c_str());
	}
	
	return INT_MIN;
}

String NewConeDialog :: GetFacesNumberStr()const
{
	return String( GetTextValue( CU_T( "Faces Number")).c_str());
}


BEGIN_EVENT_TABLE( NewConeDialog, NewGeometryDialog)
END_EVENT_TABLE()