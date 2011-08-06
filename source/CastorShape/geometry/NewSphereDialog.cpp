#include "CastorShape/PrecompiledHeader.hpp"

#include "CastorShape/geometry/NewSphereDialog.hpp"

using namespace CastorShape;
using namespace Castor3D;

NewSphereDialog :: NewSphereDialog( wxWindow * parent, wxWindowID p_id)
	:	NewGeometryDialog( parent, p_id, cuT( "New sphere"))
{
}

NewSphereDialog :: ~NewSphereDialog()
{
}

void NewSphereDialog :: _initialise()
{
	_defaultInit();
	AddTextCtrl( cuT( "Radius"), cuT( "1.0"), 40);
	AddTextCtrl( cuT( "Subdiv"), cuT( "30"), 40);
	_endInit();
}

real NewSphereDialog :: GetSphereRadius()const
{
	wxString l_value = GetTextValue( cuT( "Radius"));

	if (l_value.BeforeFirst( '.').IsNumber() && l_value.AfterFirst( '.').IsNumber())
	{
		return ator( l_value.c_str());
	}

	return INT_MIN;
}

int NewSphereDialog :: GetFacesNumber()const
{
	wxString l_value = GetTextValue( cuT( "Subdiv"));

	if (l_value.IsNumber())
	{
		return atoi( l_value.c_str());
	}

	return INT_MIN;
}

String NewSphereDialog :: GetFacesNumberStr()const
{
	return String( (const char *)GetTextValue( cuT( "Subdiv")).c_str());
}

BEGIN_EVENT_TABLE( NewSphereDialog, NewGeometryDialog)
END_EVENT_TABLE()
