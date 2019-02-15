/*
See LICENSE file in root folder
*/
#ifndef ___CV_TRANSLATE_NODE_EVENT_H___
#define ___CV_TRANSLATE_NODE_EVENT_H___

#include "MouseNodeEvent.hpp"

namespace CastorViewer
{
	class TranslateNodeEvent
		: public MouseNodeEvent
	{
	public:
		TranslateNodeEvent( castor3d::SceneNodeSPtr p_node, castor::real p_dx, castor::real p_dy, castor::real p_dz );
		virtual ~TranslateNodeEvent();

		bool apply()override;
	};
}

#endif
