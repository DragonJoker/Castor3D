#include "CameraManager.hpp"

#include "Render/RenderSystem.hpp"

#include "Shader/FrameVariableBuffer.hpp"
#include "Shader/ShaderProgram.hpp"

using namespace Castor;

namespace Castor3D
{
	const String ManagedObjectNamer< Camera >::Name = cuT( "Camera" );

	CameraManager::CameraManager( Scene & p_owner, SceneNodeSPtr p_rootNode, SceneNodeSPtr p_rootCameraNode, SceneNodeSPtr p_rootObjectNode )
		: ObjectManager< Castor::String, Camera >( p_owner, p_rootNode, p_rootCameraNode, p_rootObjectNode )
	{
	}

	CameraManager::~CameraManager()
	{
	}

	void CameraManager::BindCamera( FrameVariableBuffer & p_sceneBuffer )
	{
		RenderSystem * l_renderSystem = GetEngine()->GetRenderSystem();
		Camera * l_pCamera = l_renderSystem->GetCurrentCamera();

		if ( l_pCamera )
		{
			Point3r l_position = l_pCamera->GetParent()->GetDerivedPosition();
			Point3rFrameVariableSPtr l_cameraPos;
			p_sceneBuffer.GetVariable( ShaderProgram::CameraPos, l_cameraPos );

			if ( l_cameraPos )
			{
				l_cameraPos->SetValue( l_position );
			}
		}
	}
}
