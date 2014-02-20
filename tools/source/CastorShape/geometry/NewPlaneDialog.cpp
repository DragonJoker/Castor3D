#include "CastorShape/PrecompiledHeader.hpp"

#include "CastorShape/geometry/NewPlaneDialog.hpp"

using namespace CastorShape;
using namespace Castor3D;
using namespace Castor;

NewPlaneDialog :: NewPlaneDialog( Engine * p_pEngine, wxWindow * parent, wxWindowID p_id)
	:	NewGeometryDialog( p_pEngine, parent, p_id, _( "New plane"))
{
}

NewPlaneDialog :: ~NewPlaneDialog()
{
}

void NewPlaneDialog :: DoInitialise()
{
	wxSizer * l_pSizerAll = DoDefaultInit();
	wxSizer * l_pSizerWdh = AddTextCtrl( _( "Width"					), cuT( "1.0"	), 40 );
	wxSizer * l_pSizerHgt = AddTextCtrl( _( "Depth"					), cuT( "1.0"	), 40 );
	wxSizer * l_pSizerWsv = AddTextCtrl( _( "Width subdivisions"	), cuT( "0"		), 40 );
	wxSizer * l_pSizerDsv = AddTextCtrl( _( "Depth subdivisions"	), cuT( "0"		), 40 );

	l_pSizerAll->Add( l_pSizerWdh,	wxSizerFlags( 0 ).Expand().Border( wxALL, 5 ) );
	l_pSizerAll->Add( l_pSizerHgt,	wxSizerFlags( 0 ).Expand().Border( wxALL, 5 ) );
	l_pSizerAll->Add( l_pSizerWsv,	wxSizerFlags( 0 ).Expand().Border( wxALL, 5 ) );
	l_pSizerAll->Add( l_pSizerDsv,	wxSizerFlags( 0 ).Expand().Border( wxALL, 5 ) );

	DoEndInit( l_pSizerAll );
}

real NewPlaneDialog :: GetGeometryWidth()const
{
	wxString l_value = GetTextValue( _( "Width"));
	double l_dTmp = 0.0;

	if (l_value.BeforeFirst( '.').IsNumber() && l_value.AfterFirst( '.').IsNumber())
	{
		l_value.ToDouble( & l_dTmp);
	}
	
	return real( l_dTmp );
}

real NewPlaneDialog :: GetGeometryDepth()const
{
	wxString l_value = GetTextValue( _( "Depth"));
	double l_dTmp = 0.0;

	if (l_value.BeforeFirst( '.').IsNumber() && l_value.AfterFirst( '.').IsNumber())
	{
		l_value.ToDouble( & l_dTmp);
	}
	
	return real( l_dTmp );
}

int NewPlaneDialog :: GetNbWidthSubdiv()const
{
	wxString l_value = GetTextValue( _( "Width subdivisions"));

	if (l_value.IsNumber())
	{
		return atoi( l_value.char_str());
	}

	return INT_MIN;
}

int NewPlaneDialog :: GetNbDepthSubdiv()const
{
	wxString l_value = GetTextValue( _( "Depth subdivisions" ) );

	if (l_value.IsNumber())
	{
		return atoi( l_value.char_str());
	}

	return INT_MIN;
}

String NewPlaneDialog :: GetNbWidthSubdivStr()const
{
	return String( GetTextValue( _( "Width subdivisions" ) ).c_str() );
}

String NewPlaneDialog :: GetNbDepthSubdivStr()const
{
	return String( GetTextValue( _( "Depth subdivisions" ) ).c_str() );
}


BEGIN_EVENT_TABLE( NewPlaneDialog, NewGeometryDialog)
END_EVENT_TABLE()
