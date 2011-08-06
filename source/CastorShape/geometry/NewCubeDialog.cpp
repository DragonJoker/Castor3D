#include "CastorShape/PrecompiledHeader.hpp"

#include "CastorShape/geometry/NewCubeDialog.hpp"

using namespace CastorShape;
using namespace Castor3D;

NewCubeDialog :: NewCubeDialog( wxWindow * parent, wxWindowID p_id)
	:	NewGeometryDialog( parent, p_id, cuT( "New cube"))
{
}

NewCubeDialog :: ~NewCubeDialog()
{
}

void NewCubeDialog :: _initialise()
{
	_defaultInit();
	AddTextCtrl( cuT( "Width"), cuT( "1.0"), 40);
	AddTextCtrl( cuT( "Height"), cuT( "1.0"), 40);
	AddTextCtrl( cuT( "Depth"), cuT( "1.0"), 40);
	_endInit();
}

real NewCubeDialog :: GetCubeWidth()const
{
	wxString l_value = GetTextValue( cuT( "Width"));
	
	if (l_value.BeforeFirst( '.').IsNumber() && l_value.AfterFirst( '.').IsNumber())
	{
		return ator( l_value.c_str());
	}
	
	return 0.0f;
}

real NewCubeDialog :: GetCubeHeight()const
{
	wxString l_value = GetTextValue( cuT( "Height"));
	
	if (l_value.BeforeFirst( '.').IsNumber() && l_value.AfterFirst( '.').IsNumber())
	{
		return ator( l_value.c_str());
	}
	
	return 0.0f;
}

real NewCubeDialog :: GetCubeDepth()const
{
	wxString l_value = GetTextValue( cuT( "Depth"));
	
	if (l_value.BeforeFirst( '.').IsNumber() && l_value.AfterFirst( '.').IsNumber())
	{
		return ator( l_value.c_str());
	}
	
	return 0.0f;
}


BEGIN_EVENT_TABLE( NewCubeDialog, NewGeometryDialog)
END_EVENT_TABLE()