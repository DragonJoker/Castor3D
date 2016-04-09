#ifndef ___CV_TRANSLATE_NODE_EVENT_H___
#define ___CV_TRANSLATE_NODE_EVENT_H___

#include "MouseNodeEvent.hpp"

namespace CastorViewer
{
	class TranslateNodeEvent
		: public MouseNodeEvent
	{
	public:
		TranslateNodeEvent( Castor3D::SceneNodeSPtr p_node, Castor::real p_dx, Castor::real p_dy, Castor::real p_dz );
		virtual ~TranslateNodeEvent();

		virtual bool Apply();
	};
}

#endif
