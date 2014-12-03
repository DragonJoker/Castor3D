#include "CastorShape/PrecompiledHeader.hpp"

#include "CastorShape/geometry/NewTorusDialog.hpp"

using namespace CastorShape;
using namespace Castor3D;
using namespace Castor;

NewTorusDialog :: NewTorusDialog( Engine * p_pEngine, wxWindow * parent, wxWindowID p_id)
	:	NewGeometryDialog( p_pEngine, parent, p_id, cuT( "New torus"))
{
}

NewTorusDialog :: ~NewTorusDialog()
{
}

void NewTorusDialog :: DoInitialise()
{
	wxSizer * l_pSizerAll = DoDefaultInit();
	wxSizer * l_pSizerWdh = AddTextCtrl( cuT( "Width"				), cuT( "1.0"	), 40 );
	wxSizer * l_pSizerHgt = AddTextCtrl( cuT( "Radius"				), cuT( "0.5"	), 40 );
	wxSizer * l_pSizerWsv = AddTextCtrl( cuT( "Width subdivisions"	), cuT( "20"	), 40 );
	wxSizer * l_pSizerDsv = AddTextCtrl( cuT( "Depth subdivisions"	), cuT( "20"	), 40 );

	l_pSizerAll->Add( l_pSizerWdh,	wxSizerFlags( 0 ).Expand().Border( wxALL, 5 ) );
	l_pSizerAll->Add( l_pSizerHgt,	wxSizerFlags( 0 ).Expand().Border( wxALL, 5 ) );
	l_pSizerAll->Add( l_pSizerWsv,	wxSizerFlags( 0 ).Expand().Border( wxALL, 5 ) );
	l_pSizerAll->Add( l_pSizerDsv,	wxSizerFlags( 0 ).Expand().Border( wxALL, 5 ) );

	DoEndInit( l_pSizerAll );
}

real NewTorusDialog :: GetInternalRadius()const
{
	wxString l_value = GetTextValue( cuT( "Radius"));
	double l_dTmp = 0.0;

	if (l_value.BeforeFirst( '.').IsNumber() && l_value.AfterFirst( '.').IsNumber())
	{
		l_value.ToDouble( & l_dTmp);
	}
	
	return real( l_dTmp );
}

real NewTorusDialog :: GetExternalRadius()const
{
	wxString l_value = GetTextValue( cuT( "Width"));
	double l_dTmp = 0.0;

	if (l_value.BeforeFirst( '.').IsNumber() && l_value.AfterFirst( '.').IsNumber())
	{
		l_value.ToDouble( & l_dTmp);
	}
	
	return real( l_dTmp );
}

int NewTorusDialog :: GetInternalNbFaces()const
{
	wxString l_value = GetTextValue( cuT( "Width subdivisions" ) );

	if (l_value.IsNumber())
	{
		return atoi( l_value.char_str());
	}

	return INT_MIN;
}

int NewTorusDialog :: GetExternalNbFaces()const
{
	wxString l_value = GetTextValue( cuT( "Radius subdivisions" ) );

	if (l_value.IsNumber())
	{
		return atoi( l_value.char_str());
	}

	return INT_MIN;
}

String NewTorusDialog :: GetInternalNbFacesStr()const
{
	return String( GetTextValue( cuT( "Width subdivisions" ) ).c_str() );
}

String NewTorusDialog :: GetExternalNbFacesStr()const
{
	return String( GetTextValue( cuT( "Radius subdivisions" ) ).c_str() );
}


BEGIN_EVENT_TABLE( NewTorusDialog, NewGeometryDialog)
END_EVENT_TABLE()
