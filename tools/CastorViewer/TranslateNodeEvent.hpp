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
		TranslateNodeEvent( castor3d::SceneNodeSPtr p_node, float p_dx, float p_dy, float p_dz );

	private:
		void doApply()override;
	};
}

#endif
