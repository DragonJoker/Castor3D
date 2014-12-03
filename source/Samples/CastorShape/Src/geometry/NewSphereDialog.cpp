#include "../geometry/NewSphereDialog.hpp"

using namespace CastorShape;
using namespace Castor3D;
using namespace Castor;

NewSphereDialog::NewSphereDialog( Engine * p_pEngine, wxWindow * parent, wxWindowID p_id )
	:	NewGeometryDialog( p_pEngine, parent, p_id, _( "New sphere" ) )
{
}

NewSphereDialog::~NewSphereDialog()
{
}

void NewSphereDialog::DoInitialise()
{
	wxSizer * l_pSizerAll = DoDefaultInit();
	wxSizer * l_pSizerRds = AddTextCtrl( _( "Radius"	), cuT( "1.0"	), 40 );
	wxSizer * l_pSizerSdv = AddTextCtrl( _( "Subdivisions"	), cuT( "30"	), 40 );
	l_pSizerAll->Add( l_pSizerRds,	wxSizerFlags( 0 ).Expand().Border( wxALL, 5 ) );
	l_pSizerAll->Add( l_pSizerSdv,	wxSizerFlags( 0 ).Expand().Border( wxALL, 5 ) );
	DoEndInit( l_pSizerAll );
}

real NewSphereDialog::GetSphereRadius()const
{
	wxString l_value = GetTextValue( _( "Radius" ) );
	double l_dTmp = 0.0;

	if ( l_value.BeforeFirst( '.' ).IsNumber() && l_value.AfterFirst( '.' ).IsNumber() )
	{
		l_value.ToDouble( & l_dTmp );
	}

	return real( l_dTmp );
}

int NewSphereDialog::GetFacesNumber()const
{
	wxString l_value = GetTextValue( _( "Subdivisions" ) );

	if ( l_value.IsNumber() )
	{
		return atoi( l_value.char_str() );
	}

	return INT_MIN;
}

String NewSphereDialog::GetFacesNumberStr()const
{
	return String( GetTextValue( _( "Subdivisions" ) ).c_str() );
}

BEGIN_EVENT_TABLE( NewSphereDialog, NewGeometryDialog )
END_EVENT_TABLE()
