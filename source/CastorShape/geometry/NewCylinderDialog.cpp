#include "CastorShape/PrecompiledHeader.hpp"

#include "CastorShape/geometry/NewCylinderDialog.hpp"

using namespace CastorShape;
using namespace Castor3D;

NewCylinderDialog :: NewCylinderDialog( wxWindow * parent, wxWindowID p_id)
	:	NewGeometryDialog( parent, p_id, cuT( "New cylinder"))
{
}

NewCylinderDialog :: ~NewCylinderDialog()
{
}

void NewCylinderDialog :: _initialise()
{
	_defaultInit();
	AddTextCtrl( cuT( "Radius"), cuT( "0.5"), 40);
	AddTextCtrl( cuT( "Height"), cuT( "1.0"), 40);
	AddTextCtrl( cuT( "Faces Number"), cuT( "10"), 40);
	_endInit();
}

real NewCylinderDialog :: GetCylinderRadius()const
{
	wxString l_value = GetTextValue( cuT( "Radius"));

	if (l_value.BeforeFirst( '.').IsNumber() && l_value.AfterFirst( '.').IsNumber())
	{
		return ator( l_value.c_str());
	}

	return 0.0f;
}

real NewCylinderDialog :: GetCylinderHeight()const
{
	wxString l_value = GetTextValue( cuT( "Height"));

	if (l_value.BeforeFirst( '.').IsNumber() && l_value.AfterFirst( '.').IsNumber())
	{
		return ator( l_value.c_str());
	}

	return 0.0f;
}

int NewCylinderDialog :: GetFacesNumber()const
{
	wxString l_value = GetTextValue( cuT( "Faces Number"));

	if (l_value.IsNumber())
	{
		return atoi( l_value.c_str());
	}

	return INT_MIN;
}

String NewCylinderDialog :: GetFacesNumberStr()const
{
	return String( (const char *)GetTextValue( cuT( "Faces Number")).c_str());
}


BEGIN_EVENT_TABLE( NewCylinderDialog, NewGeometryDialog)
END_EVENT_TABLE()
