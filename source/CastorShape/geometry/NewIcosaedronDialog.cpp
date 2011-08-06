#include "CastorShape/PrecompiledHeader.hpp"

#include "CastorShape/geometry/NewIcosaedronDialog.hpp"

using namespace CastorShape;
using namespace Castor3D;

NewIcosaedronDialog :: NewIcosaedronDialog( wxWindow * parent, wxWindowID p_id)
	:	NewGeometryDialog( parent, p_id, cuT( "New icosaedron"))
{
}

NewIcosaedronDialog :: ~NewIcosaedronDialog()
{
}

void NewIcosaedronDialog :: _initialise()
{
	_defaultInit();
	AddTextCtrl( cuT( "Radius"), cuT( "1.0"), 40);
	AddTextCtrl( cuT( "Subdiv Number"), cuT( "4"), 40);
	_endInit();
}

real NewIcosaedronDialog :: GetIcosaedronRadius()const
{
	wxString l_value = GetTextValue( cuT( "Radius"));

	if (l_value.BeforeFirst( '.').IsNumber() && l_value.AfterFirst( '.').IsNumber())
	{
		return ator( l_value.c_str());
	}

	return INT_MIN;
}

int NewIcosaedronDialog :: GetNbSubdiv()const
{
	wxString l_value = GetTextValue( cuT( "Subdiv Number"));

	if (l_value.IsNumber())
	{
		return atoi( l_value.c_str());
	}

	return INT_MIN;
}

String NewIcosaedronDialog :: GetNbSubdivStr()const
{
	return String( (const char *)GetTextValue( cuT( "Subdiv Number")).c_str());
}


BEGIN_EVENT_TABLE( NewIcosaedronDialog, NewGeometryDialog)
END_EVENT_TABLE()
