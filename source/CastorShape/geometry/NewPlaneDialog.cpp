#include "CastorShape/PrecompiledHeader.hpp"

#include "CastorShape/geometry/NewPlaneDialog.hpp"

using namespace CastorShape;
using namespace Castor3D;

NewPlaneDialog :: NewPlaneDialog( wxWindow * parent, wxWindowID p_id)
	:	NewGeometryDialog( parent, p_id, cuT( "New plane"))
{
}

NewPlaneDialog :: ~NewPlaneDialog()
{
}

void NewPlaneDialog :: _initialise()
{
	_defaultInit();
	AddTextCtrl( cuT( "Width"), cuT( "1.0"), 40);
	AddTextCtrl( cuT( "Depth"), cuT( "1.0"), 40);
	AddTextCtrl( cuT( "Width Subdiv"), cuT( "0"), 40);
	AddTextCtrl( cuT( "Depth Subdiv"), cuT( "0"), 40);
	_endInit();
}

real NewPlaneDialog :: GetGeometryWidth()const
{
	wxString l_value = GetTextValue( cuT( "Width"));

	if (l_value.BeforeFirst( '.').IsNumber() && l_value.AfterFirst( '.').IsNumber())
	{
		return ator( l_value.c_str());
	}

	return 0.0f;
}

real NewPlaneDialog :: GetGeometryDepth()const
{
	wxString l_value = GetTextValue( cuT( "Depth"));

	if (l_value.BeforeFirst( '.').IsNumber() && l_value.AfterFirst( '.').IsNumber())
	{
		return ator( l_value.c_str());
	}

	return 0.0f;
}

int NewPlaneDialog :: GetNbWidthSubdiv()const
{
	wxString l_value = GetTextValue( cuT( "Width Subdiv"));

	if (l_value.IsNumber())
	{
		return atoi( l_value.c_str());
	}

	return INT_MIN;
}

int NewPlaneDialog :: GetNbDepthSubdiv()const
{
	wxString l_value = GetTextValue( cuT( "Depth Subdiv"));

	if (l_value.IsNumber())
	{
		return atoi( l_value.c_str());
	}

	return INT_MIN;
}

String NewPlaneDialog :: GetNbWidthSubdivStr()const
{
	return String( (const char *)GetTextValue( cuT( "Width Subdiv")).c_str());
}

String NewPlaneDialog :: GetNbDepthSubdivStr()const
{
	return String( (const char *)GetTextValue( cuT( "Depth Subdiv")).c_str());
}


BEGIN_EVENT_TABLE( NewPlaneDialog, NewGeometryDialog)
END_EVENT_TABLE()
