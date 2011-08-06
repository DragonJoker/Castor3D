#include "CastorShape/PrecompiledHeader.hpp"

#include "CastorShape/geometry/NewTorusDialog.hpp"

using namespace CastorShape;
using namespace Castor3D;

NewTorusDialog :: NewTorusDialog( wxWindow * parent, wxWindowID p_id)
	:	NewGeometryDialog( parent, p_id, cuT( "New torus"))
{
}

NewTorusDialog :: ~NewTorusDialog()
{
}

void NewTorusDialog :: _initialise()
{
	_defaultInit();
	AddTextCtrl( cuT( "Width"), cuT( "1.0"), 40);
	AddTextCtrl( cuT( "Radius"), cuT( "0.5"), 40);
	AddTextCtrl( cuT( "Width Subdiv"), cuT( "20"), 40);
	AddTextCtrl( cuT( "Radius Subdiv"), cuT( "20"), 40);
	_endInit();
}

real NewTorusDialog :: GetInternalRadius()const
{
	wxString l_value = GetTextValue( cuT( "Radius"));

	if (l_value.BeforeFirst( '.').IsNumber() && l_value.AfterFirst( '.').IsNumber())
	{
		return ator( l_value.c_str());
	}

	return 0.0f;
}

real NewTorusDialog :: GetExternalRadius()const
{
	wxString l_value = GetTextValue( cuT( "Width"));

	if (l_value.BeforeFirst( '.').IsNumber() && l_value.AfterFirst( '.').IsNumber())
	{
		return ator( l_value.c_str());
	}

	return 0.0f;
}

int NewTorusDialog :: GetInternalNbFaces()const
{
	wxString l_value = GetTextValue( cuT( "Width Subdiv"));

	if (l_value.IsNumber())
	{
		return atoi( l_value.c_str());
	}

	return INT_MIN;
}

int NewTorusDialog :: GetExternalNbFaces()const
{
	wxString l_value = GetTextValue( cuT( "Radius Subdiv"));

	if (l_value.IsNumber())
	{
		return atoi( l_value.c_str());
	}

	return INT_MIN;
}

String NewTorusDialog :: GetInternalNbFacesStr()const
{
	return String( (const char *)GetTextValue( cuT( "Width Subdiv")).c_str());
}

String NewTorusDialog :: GetExternalNbFacesStr()const
{
	return String( (const char *)GetTextValue( cuT( "Radius Subdiv")).c_str());
}


BEGIN_EVENT_TABLE( NewTorusDialog, NewGeometryDialog)
END_EVENT_TABLE()
