#pragma once

#include <wx/panel.h>

#include <Engine.hpp>

#include "CastorDvpTDPrerequisites.hpp"

namespace castortd
{
	class RenderPanel
		: public wxPanel
	{
	public:
		RenderPanel( wxWindow * p_parent, wxSize const & p_size );
		~RenderPanel();
		void SetRenderWindow( Castor3D::RenderWindowSPtr p_window );

	private:
		Castor::real m_x{ 0.0_r };
		Castor::real m_y{ 0.0_r };
		Castor::real m_oldX{ 0.0_r };
		Castor::real m_oldY{ 0.0_r };
		Castor3D::RenderWindowWPtr m_renderWindow;
		Castor3D::SceneNodeSPtr m_cameraNode;
		Castor::Point3r m_originalPosition;
		Castor::Quaternion m_originalOrientation;
	};
}
