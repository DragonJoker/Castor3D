#ifndef ___GC_PrecompiledHeader___
#define ___GC_PrecompiledHeader___

#ifndef CHECK_MEMORYLEAKS
#	ifdef _DEBUG
#		define CHECK_MEMORYLEAKS 1
#	else
#		define CHECK_MEMORYLEAKS 0
#	endif
#endif

#include <string>

#if CHECK_MEMORYLEAKS
#	pragma message( "********************************************************************")
#	pragma message( "	GuiCommon Checking Memory leaks")
#	pragma message( "********************************************************************")
#endif
#include <CastorUtils/Memory.h>

#ifdef _WIN32
#   ifdef CASTOR_GTK
#       ifdef __WXGTK__
#           undef __WXMSW__
#       else
#           define __WXMSW__
#       endif
#   else
#       undef __WXGTK__
#       define __WXMSW__
#   endif
#endif

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
#include <wx/gauge.h>
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
#include <wx/rawbmp.h>
#include <wx/scrolwin.h>
#include <wx/slider.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/statusbr.h>
#include <wx/textctrl.h>
#include <wx/timer.h>
#include <wx/treectrl.h>

#include <CastorUtils/Value.h>
using namespace Castor::Templates;

#include <CastorUtils/Vector.h>
#include <CastorUtils/List.h>
#include <CastorUtils/Map.h>
#include <CastorUtils/Multimap.h>
#include <CastorUtils/Set.h>
#include <CastorUtils/PreciseTimer.h>
#include <CastorUtils/File.h>
#include <CastorUtils/Point.h>
#include <CastorUtils/Quaternion.h>
#include <CastorUtils/Named.h>
#include <CastorUtils/Path.h>
#include <CastorUtils/FastMath.h>
#include <CastorUtils/Colour.h>
#include <CastorUtils/Angle.h>

using namespace Castor::Math;
using namespace Castor::Theory;
using namespace Castor::Templates;

#include <Castor3D/Prerequisites.h>
#include <Castor3D/main/RenderWindow.h>
#include <Castor3D/main/Root.h>
#include <Castor3D/main/Plugin.h>
#include <Castor3D/render_system/RenderSystem.h>
#include <Castor3D/camera/Ray.h>
#include <Castor3D/main/RendererServer.h>
#include <Castor3D/main/FrameListener.h>
#include <Castor3D/material/TextureEnvironment.h>
#include <Castor3D/material/MaterialManager.h>
#include <Castor3D/material/Material.h>
#include <Castor3D/material/Pass.h>
#include <Castor3D/material/TextureUnit.h>
#include <Castor3D/geometry/mesh/MeshManager.h>
#include <Castor3D/geometry/mesh/Mesh.h>
#include <Castor3D/geometry/mesh/Submesh.h>
#include <Castor3D/geometry/primitives/Geometry.h>
#include <Castor3D/scene/SceneManager.h>
#include <Castor3D/scene/Scene.h>
#include <Castor3D/scene/SceneNode.h>
#include <Castor3D/light/Light.h>
#include <Castor3D/light/PointLight.h>
#include <Castor3D/light/DirectionalLight.h>
#include <Castor3D/light/SpotLight.h>
#include <Castor3D/animation/AnimationManager.h>
#include <Castor3D/animation/Animation.h>
#include <Castor3D/animation/KeyFrame.h>
#include <Castor3D/animation/AnimatedObjectGroup.h>
#include <Castor3D/camera/Camera.h>
#include <Castor3D/render_engine/RenderEngine.h>
#include <Castor3D/shader/ShaderManager.h>
#include <Castor3D/shader/ShaderProgram.h>
#include <Castor3D/shader/ShaderObject.h>
#include <Castor3D/shader/FrameVariable.h>
#include <Castor3D/shader/Cg/CgFrameVariable.h>
#include <Castor3D/overlay/OverlayManager.h>
#include <Castor3D/overlay/Overlay.h>
#include <Castor3D/scene/SceneFileParser.h>
#include <Castor3D/Log.h>

#include <iostream>
#include <string>
#include <vector>
#include <math.h>

using namespace Castor::Utils;

#ifndef _UNICODE
#	if CASTOR_UNICODE
#		error "GuiCommon compiling in ASCII mode while Castor3D is in UNICODE mode"
#	endif
#else
#	if ! CASTOR_UNICODE
#		error "GuiCommon compiling in UNICODE mode while Castor3D is in ASCII mode"
#	endif
#endif

#endif
