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
		TranslateNodeEvent( castor3d::SceneNodeRPtr node
			, float dx
			, float dy
			, float dz );

	private:
		void doApply()override;
	};
}

#endif
