#include "RenderPanel.hpp"

#include <Render/RenderWindow.hpp>
#include <Scene/Camera.hpp>
#include <Scene/SceneNode.hpp>

using namespace Castor;
using namespace Castor3D;

namespace castortd
{
	RenderPanel::RenderPanel( wxWindow * p_parent, wxSize const & p_size )
		: wxPanel{ p_parent, wxID_ANY, wxDefaultPosition, p_size }
	{
	}

	RenderPanel::~RenderPanel()
	{
	}

	void RenderPanel::SetRenderWindow( Castor3D::RenderWindowSPtr p_window )
	{
		m_renderWindow.reset();

		if ( p_window )
		{
			Castor::Size l_sizeWnd = GuiCommon::make_Size( GetClientSize() );

			if ( p_window->Initialise( l_sizeWnd, GuiCommon::make_WindowHandle( this ) ) )
			{
				Castor::Size l_sizeScreen;
				Castor::System::GetScreenSize( 0, l_sizeScreen );
				GetParent()->SetClientSize( l_sizeWnd.width(), l_sizeWnd.height() );
				l_sizeWnd = GuiCommon::make_Size( GetParent()->GetClientSize() );
				GetParent()->SetPosition( wxPoint( std::abs( int( l_sizeScreen.width() ) - int( l_sizeWnd.width() ) ) / 2, std::abs( int( l_sizeScreen.height() ) - int( l_sizeWnd.height() ) ) / 2 ) );
				SceneSPtr l_scene = p_window->GetScene();

				if ( l_scene )
				{
					m_cameraNode = p_window->GetCamera()->GetParent();

					if ( m_cameraNode )
					{
						m_originalPosition = m_cameraNode->GetPosition();
						m_originalOrientation = m_cameraNode->GetOrientation();
					}

					m_renderWindow = p_window;
				}
			}
		}
	}
}
