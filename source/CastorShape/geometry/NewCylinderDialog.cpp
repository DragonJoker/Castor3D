#include "CastorShape/PrecompiledHeader.h"

#include "CastorShape/geometry/NewCylinderDialog.h"

using namespace CastorShape;
using namespace Castor3D;

NewCylinderDialog :: NewCylinderDialog( MaterialManager * p_pManager, wxWindow * parent, wxWindowID p_id)
	:	NewGeometryDialog( p_pManager, parent, p_id, CU_T( "New cylinder"))
{
}

NewCylinderDialog :: ~NewCylinderDialog()
{
}

void NewCylinderDialog :: _initialise()
{
	_defaultInit();
	AddTextCtrl( CU_T( "Radius"), CU_T( "0.5"), 40);
	AddTextCtrl( CU_T( "Height"), CU_T( "1.0"), 40);
	AddTextCtrl( CU_T( "Faces Number"), CU_T( "10"), 40);
	_endInit();
}

real NewCylinderDialog :: GetCylinderRadius()const
{
	wxString l_value = GetTextValue( CU_T( "Radius"));
	
	if (l_value.BeforeFirst( '.').IsNumber() && l_value.AfterFirst( '.').IsNumber())
	{
		return ator( l_value.c_str());
	}
	
	return 0.0f;
}

real NewCylinderDialog :: GetCylinderHeight()const
{
	wxString l_value = GetTextValue( CU_T( "Height"));
	
	if (l_value.BeforeFirst( '.').IsNumber() && l_value.AfterFirst( '.').IsNumber())
	{
		return ator( l_value.c_str());
	}
	
	return 0.0f;
}

int NewCylinderDialog :: GetFacesNumber()const
{
	wxString l_value = GetTextValue( CU_T( "Faces Number"));
	
	if (l_value.IsNumber())
	{
		return atoi( l_value.c_str());
	}
	
	return INT_MIN;
}

String NewCylinderDialog :: GetFacesNumberStr()const
{
	return String( GetTextValue( CU_T( "Faces Number")).c_str());
}


BEGIN_EVENT_TABLE( NewCylinderDialog, NewGeometryDialog)
END_EVENT_TABLE()