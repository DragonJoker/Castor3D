#include "CastorShape/PrecompiledHeader.hpp"

#include "CastorShape/geometry/NewConeDialog.hpp"

using namespace CastorShape;
using namespace Castor3D;

NewConeDialog :: NewConeDialog( wxWindow * parent, wxWindowID p_id)
	:	NewGeometryDialog( parent, p_id, cuT( "New cone"))
{
}

NewConeDialog :: ~NewConeDialog()
{
}

void NewConeDialog :: _initialise()
{
	_defaultInit();
	AddTextCtrl( cuT( "Radius"), cuT( "0.5"), 40);
	AddTextCtrl( cuT( "Height"), cuT( "1.0"), 40);
	AddTextCtrl( cuT( "Faces Number"), cuT( "10"), 40);
	_endInit();
}

real NewConeDialog :: GetConeRadius()const
{
	wxString l_value = GetTextValue( cuT( "Radius"));

	if (l_value.BeforeFirst( '.').IsNumber() && l_value.AfterFirst( '.').IsNumber())
	{
		return ator( l_value.c_str());
	}

	return 0.0f;
}

real NewConeDialog :: GetConeHeight()const
{
	wxString l_value = GetTextValue( cuT( "Height"));

	if (l_value.BeforeFirst( '.').IsNumber() && l_value.AfterFirst( '.').IsNumber())
	{
		return ator( l_value.c_str());
	}

	return 0.0f;
}

int NewConeDialog :: GetFacesNumber()const
{
	wxString l_value = GetTextValue( cuT( "Faces Number"));

	if (l_value.IsNumber())
	{
		return atoi( l_value.c_str());
	}

	return INT_MIN;
}

String NewConeDialog :: GetFacesNumberStr()const
{
	return String( (const char *)GetTextValue( cuT( "Faces Number")).c_str());
}


BEGIN_EVENT_TABLE( NewConeDialog, NewGeometryDialog)
END_EVENT_TABLE()
