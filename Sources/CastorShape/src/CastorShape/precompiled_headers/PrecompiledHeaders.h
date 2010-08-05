//******************************************************************************
#ifndef ___CS_PRECOMPILED_HEADER_H___
#define ___CS_PRECOMPILED_HEADER_H___
//******************************************************************************
#include <wx/app.h>
#include <wx/button.h>
#include <wx/bmpbuttn.h>
#include <wx/checkbox.h>
#include <wx/checklst.h>
#include <wx/colordlg.h>
#include <wx/combobox.h>
#include <wx/dcclient.h>
#include <wx/dialog.h>
#include <wx/file.h>
#include <wx/filedlg.h>
#include <wx/frame.h>
#include <wx/grid.h>
#include <wx/icon.h>
#include <wx/image.h>
#include <wx/imaglist.h>
#include <wx/listctrl.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/notebook.h>
#include <wx/panel.h>
#include <wx/radiobox.h>
#include <wx/scrolwin.h>
#include <wx/slider.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/timer.h>
#include <wx/treectrl.h>

#include <CastorUtils/STLMacros.h>
#include <CastorUtils/STLMapMacros.h>
#include <CastorUtils/STLSetMacros.h>
#include <CastorUtils/PreciseTimer.h>
#include <CastorUtils/File.h>
#include <CastorUtils/Vector3f.h>
#include <CastorUtils/Quaternion.h>
#include <CastorUtils/Named.h>
#include <CastorUtils/SlottedObjectPool.h>
#include <CastorUtils/Path.h>
#include <CastorUtils/MinMax.h>
#include <CastorUtils/Colour.h>
using namespace General::Math;
using namespace General::Theory;
using namespace General::Templates;

#include <Castor3D/Module_General.h>
#include <Castor3D/main/Module_Main.h>
#include <Castor3D/main/RenderWindow.h>
#include <Castor3D/main/Root.h>
#include <Castor3D/main/Plugin.h>
#include <Castor3D/render_system/Module_Render.h>
#include <Castor3D/render_system/RenderSystem.h>
#include <Castor3D/camera/Ray.h>
#include <Castor3D/main/RendererServer.h>
#include <Castor3D/main/FrameListener.h>
#include <Castor3D/material/Module_Material.h>
#include <Castor3D/material/TextureEnvironment.h>
#include <Castor3D/material/MaterialManager.h>
#include <Castor3D/material/Material.h>
#include <Castor3D/material/Pass.h>
#include <Castor3D/material/TextureUnit.h>
#include <Castor3D/geometry/Module_Geometry.h>
#include <Castor3D/geometry/mesh/MeshManager.h>
#include <Castor3D/geometry/mesh/Mesh.h>
#include <Castor3D/geometry/mesh/Submesh.h>
#include <Castor3D/geometry/primitives/Geometry.h>
#include <Castor3D/scene/Module_Scene.h>
#include <Castor3D/scene/SceneManager.h>
#include <Castor3D/scene/Scene.h>
#include <Castor3D/scene/SceneNode.h>
#include <Castor3D/light/Module_Light.h>
#include <Castor3D/light/Light.h>
#include <Castor3D/light/PointLight.h>
#include <Castor3D/light/DirectionalLight.h>
#include <Castor3D/light/SpotLight.h>
#include <Castor3D/Log.h>
#include <Castor3D/animation/Module_Animation.h>
#include <Castor3D/animation/AnimationManager.h>
#include <Castor3D/animation/Animation.h>
#include <Castor3D/animation/KeyFrame.h>
#include <Castor3D/animation/AnimatedObjectGroup.h>
#include <Castor3D/camera/Module_Camera.h>
#include <Castor3D/camera/Camera.h>
#include <Castor3D/render_engine/RenderEngine.h>
#include <Castor3D/shader/Module_Shader.h>
#include <Castor3D/shader/ShaderManager.h>
#include <Castor3D/shader/ShaderProgram.h>
#include <Castor3D/shader/UniformVariable.h>
#include <Castor3D/overlay/OverlayManager.h>
#include <Castor3D/overlay/Overlay.h>

#include <iostream>
#include <string>
#include <vector>
#include <math.h>

#ifndef CHECK_MEMORYLEAKS
#	ifdef _DEBUG
#		define CHECK_MEMORYLEAKS 1
#	else
#		define CHECK_MEMORYLEAKS 0
#	endif
#endif

#if CHECK_MEMORYLEAKS
//#	warning "CastorShape Checking Memory leaks"
#	include <CastorUtils/Memory.h>
#endif

using namespace General::Utils;

//******************************************************************************
#endif
//******************************************************************************
