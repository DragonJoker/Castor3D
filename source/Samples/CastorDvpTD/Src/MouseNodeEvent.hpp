#pragma once

#include "RenderPanel.hpp"

#include <Event/Frame/FrameEvent.hpp>

namespace castortd
{
	class MouseNodeEvent
		: public Castor3D::FrameEvent
	{
	public:
		MouseNodeEvent( Castor3D::SceneNodeSPtr p_node, Castor::real p_dx, Castor::real p_dy, Castor::real p_dz );
		virtual ~MouseNodeEvent();

		void Modify( Castor::real p_dx, Castor::real p_dy, Castor::real p_dz );

	protected:
		Castor3D::SceneNodeWPtr m_node;
		Castor::real m_dx;
		Castor::real m_dy;
		Castor::real m_dz;
	};
}
