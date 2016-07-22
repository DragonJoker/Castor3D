#include "CastorShape.hpp"
#include "MainFrame.hpp"

#include <wx/cmdline.h>

#include <Data/File.hpp>
#include <Exception/Exception.hpp>

#include <ImagesLoader.hpp>
#include <RendererSelector.hpp>

#include <xpms/ajouter.xpm>
#include <xpms/castor_transparent.xpm>
#include <xpms/fichier.xpm>
#include <xpms/mat_blanc.xpm>
#include <xpms/parametres.xpm>
#include <xpms/scene_blanc.xpm>

using namespace Castor;
using namespace Castor3D;
using namespace GuiCommon;

#if CHECK_MEMORYLEAKS && defined( VLD_AVAILABLE ) && USE_VLD
#	include <vld.h>
#endif

wxIMPLEMENT_APP( CastorShape::CastorShapeApp );

namespace CastorShape
{
	CastorShapeApp::CastorShapeApp()
		: CastorApplication( cuT( "CastorShape" ), cuT( "Castor Shape" ), 8 )
		, m_mainFrame( NULL )
	{
	}

	void CastorShapeApp::DoLoadAppImages()
	{
		ImagesLoader::AddBitmap( CV_IMG_CASTOR, castor_transparent_xpm );
		ImagesLoader::AddBitmap( eBMP_FICHIER, fichier_xpm );
		ImagesLoader::AddBitmap( eBMP_AJOUTER, ajouter_xpm );
		ImagesLoader::AddBitmap( eBMP_PARAMETRES, parametres_xpm );
		ImagesLoader::AddBitmap( eBMP_SCENES, scene_blanc_xpm );
		ImagesLoader::AddBitmap( eBMP_MATERIALS, mat_blanc_xpm );
		ImagesLoader::AddBitmap( eBMP_PROPERTIES, parametres_xpm );
	}

	wxWindow * CastorShapeApp::DoInitialiseMainFrame( GuiCommon::SplashScreen * p_splashScreen )
	{
		wxAppConsole::SetAppName( wxT( "castor_shape" ) );
		wxAppConsole::SetVendorName( wxT( "dragonjoker" ) );

#if wxCHECK_VERSION( 2, 9, 0 )

		wxAppConsole::SetAppDisplayName( m_displayName );
		wxAppConsole::SetVendorDisplayName( wxT( "DragonJoker" ) );

#endif
		m_mainFrame = new MainFrame( p_splashScreen, make_wxString( m_internalName ) );
		bool l_return = m_mainFrame->Initialise();

		if ( l_return )
		{
			SetTopWindow( m_mainFrame );

			if ( !GetFileName().empty() )
			{
				m_mainFrame->LoadScene( GetFileName() );
			}
		}
		else
		{
			delete m_mainFrame;
			m_mainFrame = NULL;
		}

		return m_mainFrame;
	}
}
