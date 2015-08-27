#include "FirstPersonCameraTranslateEvent.hpp"

#include <SceneNode.hpp>

using namespace Castor3D;
using namespace Castor;

namespace CastorViewer
{
	FirstPersonCameraTranslateEvent::FirstPersonCameraTranslateEvent( SceneNodeSPtr p_pNode, real p_rDeltaX, real p_rDeltaY, real p_rDeltaZ )
		:	FirstPersonCameraEvent( p_pNode.get(), p_rDeltaX, p_rDeltaY, p_rDeltaZ	)
	{
	}

	FirstPersonCameraTranslateEvent::FirstPersonCameraTranslateEvent( SceneNode * p_pNode, real p_rDeltaX, real p_rDeltaY, real p_rDeltaZ )
		:	FirstPersonCameraEvent( p_pNode, p_rDeltaX, p_rDeltaY, p_rDeltaZ	)
	{
	}

	FirstPersonCameraTranslateEvent::~FirstPersonCameraTranslateEvent()
	{
	}

	bool FirstPersonCameraTranslateEvent::Apply()
	{
		if ( m_bOK )
		{
			// Translates the camera according to the translate vector which is
			// controlled by the keyboard arrows.
			Point3r l_ptTranslate( m_rDeltaX, m_rDeltaY, m_rDeltaZ );
			m_pNodeRoll->GetDerivedOrientation().Transform( l_ptTranslate, l_ptTranslate );
			m_pNode->Translate( l_ptTranslate );
		}

		m_rDeltaX = 0;
		m_rDeltaY = 0;
		m_rDeltaZ = 0;
		return true;
	}
}
