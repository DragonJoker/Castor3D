#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/RenderTarget.hpp"
#include "Castor3D/Scene.hpp"
#include "Castor3D/Root.hpp"
#include "Castor3D/ShaderManager.hpp"
#include "Castor3D/Camera.hpp"

using namespace Castor3D;

RenderTarget :: RenderTarget( String const & p_strName, Root * p_pRoot, ScenePtr p_mainScene, Point2i const & p_ptSize, eVIEWPORT_TYPE	p_eType, ePROJECTION_DIRECTION p_eLookAt, eTARGET_TYPE p_eTargetType, eTARGET_DATA p_eTargetData)
	:	m_strName		( p_strName)
	,	m_pRoot			( p_pRoot)
	,	m_pMainScene	( p_mainScene)
	,	m_eType			( p_eType)
	,	m_eDrawType		( ePRIMITIVE_TYPE_TRIANGLES)
	,	m_eLookAt		( p_eLookAt)
	,	m_eTargetType	( p_eTargetType)
	,	m_eTargetData	( p_eTargetData)
{
	m_ptTargetSize.copy( p_ptSize);
	String l_camName = cuT( "RenderCamera_") + m_strName;
	m_pCamera = m_pMainScene->CreateCamera( l_camName, p_ptSize[0], p_ptSize[1], p_mainScene->CreateSceneNode( l_camName + cuT( "Node")), m_eType);
	_setViewPoint();
}

RenderTarget :: ~RenderTarget()
{
	m_pCamera.reset();
	m_pListener.reset();
}

void RenderTarget :: RenderOneFrame( const real & p_tslf, const bool & p_bForce)
{
	CASTOR_TRACK;
	ShaderManager l_shaderManager;
	l_shaderManager.Update();
	Root::GetSingleton()->GetMaterialManager()->Update();
	Root::GetSingleton()->GetBufferManager()->Update();
	RenderSystem::GetSingletonPtr()->CleanupRenderersToCleanup();

	m_pCamera->Render( m_eDrawType);
	m_pMainScene->Render( m_eDrawType, p_tslf);

	if (m_pListener)
	{
		m_pListener->FireEvents( eEVENT_TYPE_QUEUE_RENDER);
	}

	Root::GetSingleton()->FireEvents( eEVENT_TYPE_QUEUE_RENDER);
	m_pCamera->EndRender();
}

void RenderTarget :: _setViewPoint()
{
	if (m_eType == eVIEWPORT_TYPE_3D)
	{
		m_eDrawType = ePRIMITIVE_TYPE_TRIANGLES;
	}
	else
	{
		m_eDrawType = ePRIMITIVE_TYPE_LINES;

		switch (m_eLookAt)
		{
		case ePROJECTION_DIRECTION_BACK:
			m_pCamera->GetParent()->Yaw( Angle::FromDegrees( 180.0));
			break;

		case ePROJECTION_DIRECTION_LEFT:
			m_pCamera->GetParent()->Yaw( Angle::FromDegrees( -90.0));
			break;

		case ePROJECTION_DIRECTION_RIGHT:
			m_pCamera->GetParent()->Yaw( Angle::FromDegrees( 90.0));
			break;

		case ePROJECTION_DIRECTION_TOP:
			m_pCamera->GetParent()->Pitch( Angle::FromDegrees( -90.0));
			break;

		case ePROJECTION_DIRECTION_BOTTOM:
			m_pCamera->GetParent()->Pitch( Angle::FromDegrees( 90.0));
			break;
		}
	}
}
