#pragma once

#include "MouseNodeEvent.hpp"

namespace castortd
{
	class RotateNodeEvent
		: public MouseNodeEvent
	{
	public:
		RotateNodeEvent( Castor3D::SceneNodeSPtr p_node, Castor::real p_dx, Castor::real p_dy, Castor::real p_dz );
		virtual ~RotateNodeEvent();

		bool Apply()override;
	};
}
