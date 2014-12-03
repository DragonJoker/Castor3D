#include "../geometry/NewConeDialog.hpp"

using namespace CastorShape;
using namespace Castor3D;
using namespace Castor;

NewConeDialog::NewConeDialog( Engine * p_pEngine, wxWindow * parent, wxWindowID p_id )
	:	NewGeometryDialog( p_pEngine, parent, p_id, _( "New cone" ) )
{
}

NewConeDialog::~NewConeDialog()
{
}

void NewConeDialog::DoInitialise()
{
	wxSizer * l_pSizerAll = DoDefaultInit();
	wxSizer * l_pSizerRds = AddTextCtrl( _( "Radius"	), cuT( "0.5"	), 40 );
	wxSizer * l_pSizerHgt = AddTextCtrl( _( "Height"	), cuT( "1.0"	), 40 );
	wxSizer * l_pSizerCnt = AddTextCtrl( _( "Face count"	), cuT( "10"	), 40 );
	l_pSizerAll->Add( l_pSizerRds,	wxSizerFlags( 0 ).Expand().Border( wxALL, 5 ) );
	l_pSizerAll->Add( l_pSizerHgt,	wxSizerFlags( 0 ).Expand().Border( wxALL, 5 ) );
	l_pSizerAll->Add( l_pSizerCnt,	wxSizerFlags( 0 ).Expand().Border( wxALL, 5 ) );
	DoEndInit( l_pSizerAll );
}

real NewConeDialog::GetConeRadius()const
{
	wxString l_value = GetTextValue( _( "Radius" ) );
	double l_dTmp = 0.0;

	if ( l_value.BeforeFirst( '.' ).IsNumber() && l_value.AfterFirst( '.' ).IsNumber() )
	{
		l_value.ToDouble( & l_dTmp );
	}

	return real( l_dTmp );
}

real NewConeDialog::GetConeHeight()const
{
	wxString l_value = GetTextValue( _( "Height" ) );
	double l_dTmp = 0.0;

	if ( l_value.BeforeFirst( '.' ).IsNumber() && l_value.AfterFirst( '.' ).IsNumber() )
	{
		l_value.ToDouble( & l_dTmp );
	}

	return real( l_dTmp );
}

int NewConeDialog::GetFacesNumber()const
{
	wxString l_value = GetTextValue( _( "Face count" ) );

	if ( l_value.IsNumber() )
	{
		return atoi( l_value.char_str() );
	}

	return INT_MIN;
}

String NewConeDialog::GetFacesNumberStr()const
{
	return String( GetTextValue( _( "Face count" ) ).c_str() );
}


BEGIN_EVENT_TABLE( NewConeDialog, NewGeometryDialog )
END_EVENT_TABLE()
