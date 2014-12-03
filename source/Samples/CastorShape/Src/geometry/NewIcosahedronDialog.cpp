#include "../geometry/NewIcosahedronDialog.hpp"

using namespace CastorShape;
using namespace Castor3D;
using namespace Castor;

NewIcosahedronDialog::NewIcosahedronDialog( Engine * p_pEngine, wxWindow * parent, wxWindowID p_id )
	:	NewGeometryDialog( p_pEngine, parent, p_id, _( "New icosaedron" ) )
{
}

NewIcosahedronDialog::~NewIcosahedronDialog()
{
}

void NewIcosahedronDialog::DoInitialise()
{
	wxSizer * l_pSizerAll = DoDefaultInit();
	wxSizer * l_pSizerRds = AddTextCtrl( _( "Radius"	), cuT( "1.0"	), 40 );
	wxSizer * l_pSizerCnt = AddTextCtrl( _( "Subdivision count"	), cuT( "4"	), 40 );
	l_pSizerAll->Add( l_pSizerRds,	wxSizerFlags( 0 ).Expand().Border( wxALL, 5 ) );
	l_pSizerAll->Add( l_pSizerCnt,	wxSizerFlags( 0 ).Expand().Border( wxALL, 5 ) );
	DoEndInit( l_pSizerAll );
}

real NewIcosahedronDialog::GetIcosahedronRadius()const
{
	wxString l_value = GetTextValue( _( "Radius" ) );
	double l_dTmp = 0.0;

	if ( l_value.BeforeFirst( '.' ).IsNumber() && l_value.AfterFirst( '.' ).IsNumber() )
	{
		l_value.ToDouble( & l_dTmp );
	}

	return real( l_dTmp );
}

int NewIcosahedronDialog::GetNbSubdiv()const
{
	wxString l_value = GetTextValue( _( "Subdivision count" ) );

	if ( l_value.IsNumber() )
	{
		return atoi( l_value.char_str() );
	}

	return INT_MIN;
}

String NewIcosahedronDialog::GetNbSubdivStr()const
{
	return String( GetTextValue( _( "Subdivision count" ) ).c_str() );
}


BEGIN_EVENT_TABLE( NewIcosahedronDialog, NewGeometryDialog )
END_EVENT_TABLE()
