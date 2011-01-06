#include "CastorShape/PrecompiledHeader.h"

#include "CastorShape/geometry/NewCubeDialog.h"

using namespace CastorShape;
using namespace Castor3D;

NewCubeDialog :: NewCubeDialog( MaterialManager * p_pManager, wxWindow * parent, wxWindowID p_id)
	:	NewGeometryDialog( p_pManager, parent, p_id, CU_T( "New cube"))
{
}

NewCubeDialog :: ~NewCubeDialog()
{
}

void NewCubeDialog :: _initialise()
{
	_defaultInit();
	AddTextCtrl( CU_T( "Width"), CU_T( "1.0"), 40);
	AddTextCtrl( CU_T( "Height"), CU_T( "1.0"), 40);
	AddTextCtrl( CU_T( "Depth"), CU_T( "1.0"), 40);
	_endInit();
}

real NewCubeDialog :: GetCubeWidth()const
{
	wxString l_value = GetTextValue( CU_T( "Width"));
	
	if (l_value.BeforeFirst( '.').IsNumber() && l_value.AfterFirst( '.').IsNumber())
	{
		return ator( l_value.c_str());
	}
	
	return 0.0f;
}

real NewCubeDialog :: GetCubeHeight()const
{
	wxString l_value = GetTextValue( CU_T( "Height"));
	
	if (l_value.BeforeFirst( '.').IsNumber() && l_value.AfterFirst( '.').IsNumber())
	{
		return ator( l_value.c_str());
	}
	
	return 0.0f;
}

real NewCubeDialog :: GetCubeDepth()const
{
	wxString l_value = GetTextValue( CU_T( "Depth"));
	
	if (l_value.BeforeFirst( '.').IsNumber() && l_value.AfterFirst( '.').IsNumber())
	{
		return ator( l_value.c_str());
	}
	
	return 0.0f;
}


BEGIN_EVENT_TABLE( NewCubeDialog, NewGeometryDialog)
END_EVENT_TABLE()