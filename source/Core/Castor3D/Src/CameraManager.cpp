#include "CameraManager.hpp"

#include "Render/RenderSystem.hpp"

#include "Shader/FrameVariableBuffer.hpp"
#include "Shader/ShaderProgram.hpp"

using namespace Castor;

namespace Castor3D
{
	const String CachedObjectNamer< Camera >::Name = cuT( "Camera" );

	void BindCamera( ObjectCache< Camera, Castor::String, CameraProducer > const & p_cache, FrameVariableBuffer & p_sceneBuffer )
	{
		RenderSystem * l_renderSystem = p_cache.GetEngine()->GetRenderSystem();
		Camera * l_camera = l_renderSystem->GetCurrentCamera();

		if ( l_camera )
		{
			Point3r l_position = l_camera->GetParent()->GetDerivedPosition();
			Point3rFrameVariableSPtr l_cameraPos;
			p_sceneBuffer.GetVariable( ShaderProgram::CameraPos, l_cameraPos );

			if ( l_cameraPos )
			{
				l_cameraPos->SetValue( l_position );
			}
		}
	}

	void testCameraCache( Scene & p_scene )
	{
		auto cache = MakeObjectCache< Camera, Castor::String, CameraProducer >( p_scene.GetRootNode(), p_scene.GetCameraRootNode(), p_scene.GetObjectRootNode(), SceneGetter{ p_scene }, CameraProducer{} );
	}
}
