#include "CastorShape/PrecompiledHeader.hpp"

#include "CastorShape/geometry/NewCubeDialog.hpp"

using namespace CastorShape;
using namespace Castor3D;

NewCubeDialog :: NewCubeDialog( Engine * p_pEngine, wxWindow * parent, wxWindowID p_id)
	:	NewGeometryDialog( p_pEngine, parent, p_id, _( "New cube"))
{
}

NewCubeDialog :: ~NewCubeDialog()
{
}

void NewCubeDialog :: DoInitialise()
{
	wxSizer * l_pSizerAll = DoDefaultInit();
	wxSizer * l_pSizerWdh = AddTextCtrl( _( "Width"	), cuT( "1.0" ), 40 );
	wxSizer * l_pSizerHgt = AddTextCtrl( _( "Height"	), cuT( "1.0" ), 40 );
	wxSizer * l_pSizerDph = AddTextCtrl( _( "Depth"	), cuT( "1.0" ), 40 );

	l_pSizerAll->Add( l_pSizerWdh,	wxSizerFlags( 0 ).Expand().Border( wxALL, 5 ) );
	l_pSizerAll->Add( l_pSizerHgt,	wxSizerFlags( 0 ).Expand().Border( wxALL, 5 ) );
	l_pSizerAll->Add( l_pSizerDph,	wxSizerFlags( 0 ).Expand().Border( wxALL, 5 ) );

	DoEndInit( l_pSizerAll );
}

real NewCubeDialog :: GetCubeWidth()const
{
	wxString l_value = GetTextValue( _( "Width"));
	double l_dTmp = 0;

	if (l_value.BeforeFirst( '.').IsNumber() && l_value.AfterFirst( '.').IsNumber())
	{
		l_value.ToDouble( & l_dTmp);
	}

	return real( l_dTmp);
}

real NewCubeDialog :: GetCubeHeight()const
{
	wxString l_value = GetTextValue( _( "Height"));
	double l_dTmp = 0;

	if (l_value.BeforeFirst( '.').IsNumber() && l_value.AfterFirst( '.').IsNumber())
	{
		l_value.ToDouble( & l_dTmp);
	}

	return real( l_dTmp);
}

real NewCubeDialog :: GetCubeDepth()const
{
	wxString l_value = GetTextValue( _( "Depth"));
	double l_dTmp = 0;

	if (l_value.BeforeFirst( '.').IsNumber() && l_value.AfterFirst( '.').IsNumber())
	{
		l_value.ToDouble( & l_dTmp);
	}

	return real( l_dTmp);
}


BEGIN_EVENT_TABLE( NewCubeDialog, NewGeometryDialog)
END_EVENT_TABLE()
