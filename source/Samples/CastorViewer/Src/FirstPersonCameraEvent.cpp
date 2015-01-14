#include "FirstPersonCameraEvent.hpp"

#include <SceneNode.hpp>

using namespace Castor3D;
using namespace Castor;

namespace CastorViewer
{
	FirstPersonCameraEvent::FirstPersonCameraEvent( SceneNodeSPtr p_pNode, real p_rDeltaX, real p_rDeltaY, real p_rDeltaZ )
		:	MouseCameraEvent( p_pNode, p_rDeltaX, p_rDeltaY, p_rDeltaZ	)
		,	m_bOK( false	)
	{
		DoInitialise();
	}

	FirstPersonCameraEvent::~FirstPersonCameraEvent()
	{
	}

	void FirstPersonCameraEvent::DoInitialise()
	{
		m_pNodeRoll = m_pNode;

		if ( m_pNodeRoll )
		{
			m_pNodePitch = m_pNodeRoll->GetParent();
		}

		if ( m_pNodePitch )
		{
			m_pNodeYaw = m_pNodePitch->GetParent();
		}

		if ( m_pNodeYaw && m_pNodeYaw->GetParent() )
		{
			m_pNode = m_pNodeYaw->GetParent();
			m_bOK = true;
		}

		if ( !m_bOK )
		{
			// Create the needed nodes ?
		}
	}
}
