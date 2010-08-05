//******************************************************************************
#ifndef ___CS_PRECOMPILED_HEADER_H___
#define ___CS_PRECOMPILED_HEADER_H___
//******************************************************************************
/*
#ifndef CHECK_MEMORYLEAKS
	#define CHECK_MEMORYLEAKS 1
#endif
*/
#include <wx/app.h>
#include <wx/button.h>
#include <wx/colordlg.h>
#include <wx/combobox.h>
#include <wx/dcclient.h>
#include <wx/dialog.h>
#include <wx/file.h>
#include <wx/filedlg.h>
#include <wx/frame.h>
#include <wx/icon.h>
#include <wx/imaglist.h>
#include <wx/image.h>
#include <wx/menu.h>
#include <wx/panel.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>

#include <General/STLMacros.h>
#include <General/STLMapMacros.h>
#include <General/STLSetMacros.h>
#include <General/PreciseTimer.h>

/*
#if CHECK_MEMORYLEAKS
#include <General/Memory.h>
#endif
*/

using namespace General::Utils;

#include <Castor3D/main/RenderWindow.h>
#include <Castor3D/main/Root.h>
#include <Castor3D/render_system/RenderSystem.h>
#include <Castor3D/main/Root.h>
#include <Castor3D/main/Plugin.h>
#include <Castor3D/main/RendererServer.h>
#include <Castor3D/material/TextureEnvironment.h>
#include <Castor3D/material/MaterialManager.h>
#include <Castor3D/material/Material.h>
#include <Castor3D/material/TextureUnit.h>
#include <Castor3D/geometry/mesh/MeshManager.h>
#include <Castor3D/geometry/mesh/Mesh.h>
#include <Castor3D/geometry/primitives/Geometry.h>
#include <Castor3D/scene/SceneManager.h>
#include <Castor3D/scene/Scene.h>
#include <Castor3D/scene/SceneNode.h>
#include <Castor3D/light/Light.h>
#include <Castor3D/light/PointLight.h>
#include <Castor3D/light/DirectionalLight.h>
#include <Castor3D/light/SpotLight.h>
#include <Castor3D/log/Log.h>
#include <Castor3D/animation/AnimationManager.h>
#include <Castor3D/animation/Animation.h>
#include <Castor3D/animation/KeyFrame.h>
#include <Castor3D/animation/AnimatedObjectGroup.h>

#include <iostream>
#include <string>
#include <vector>
//******************************************************************************
#endif
//******************************************************************************