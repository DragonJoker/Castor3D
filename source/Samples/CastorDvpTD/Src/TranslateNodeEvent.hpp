#pragma once

#include "MouseNodeEvent.hpp"

namespace castortd
{
	class TranslateNodeEvent
		: public MouseNodeEvent
	{
	public:
		TranslateNodeEvent( Castor3D::SceneNodeSPtr p_node, Castor::real p_dx, Castor::real p_dy, Castor::real p_dz );
		virtual ~TranslateNodeEvent();

		bool Apply()override;
	};
}
