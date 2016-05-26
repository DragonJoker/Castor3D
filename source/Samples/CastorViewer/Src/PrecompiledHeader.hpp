#ifndef ___CV_PrecompiledHeader___
#define ___CV_PrecompiledHeader___

#include <config.hpp>

#if CASTOR_USE_PCH
#	ifdef _WIN32
#		undef __WXGTK__
#		define __WXMSW__
#	endif

#	include <string>

#	include <wx/app.h>
#	include <wx/button.h>
#	include <wx/bmpbuttn.h>
#	include <wx/checkbox.h>
#	include <wx/checklst.h>
#	include <wx/cmdline.h>
#	include <wx/colordlg.h>
#	include <wx/combobox.h>
#	include <wx/dcclient.h>
#	include <wx/dialog.h>
#	include <wx/display.h>
#	include <wx/file.h>
#	include <wx/filedlg.h>
#	include <wx/frame.h>
#	include <wx/grid.h>
#	include <wx/icon.h>
#	include <wx/image.h>
#	include <wx/imaglist.h>
#	include <wx/listctrl.h>
#	include <wx/menu.h>
#	include <wx/msgdlg.h>
#	include <wx/notebook.h>
#	include <wx/panel.h>
#	include <wx/radiobox.h>
#	include <wx/scrolwin.h>
#	include <wx/sizer.h>
#	include <wx/slider.h>
#	include <wx/statbox.h>
#	include <wx/stattext.h>
#	include <wx/statusbr.h>
#	include <wx/stdpaths.h>
#	include <wx/textctrl.h>
#	include <wx/timer.h>
#	include <wx/toolbar.h>
#	include <wx/treectrl.h>
#	include <wx/aui/framemanager.h>

#	include <CastorUtils.hpp>
#	include <StringUtils.hpp>
#	include <Templates.hpp>
#	include <Logger.hpp>
#	include <PreciseTimer.hpp>
#	include <File.hpp>
#	include <Point.hpp>
#	include <Quaternion.hpp>
#	include <Named.hpp>
#	include <Path.hpp>
#	include <Math.hpp>
#	include <Colour.hpp>
#	include <Angle.hpp>

#	include <Castor3DPrerequisites.hpp>
#	include <Engine.hpp>
#	include <ShaderManager.hpp>

#	include <Animation/AnimatedObjectGroup.hpp>
#	include <Animation/Animation.hpp>
#	include <Animation/KeyFrame.hpp>
#	include <Event/Frame/FrameListener.hpp>
#	include <Material/Material.hpp>
#	include <Material/Pass.hpp>
#	include <Mesh/Buffer/Buffer.hpp>
#	include <Mesh/Face.hpp>
#	include <Mesh/Mesh.hpp>
#	include <Mesh/Submesh.hpp>
#	include <Mesh/Vertex.hpp>
#	include <Miscellaneous/Ray.hpp>
#	include <Overlay/Overlay.hpp>
#	include <Plugin/Plugin.hpp>
#	include <Render/RenderSystem.hpp>
#	include <Render/RenderWindow.hpp>
#	include <Scene/Camera.hpp>
#	include <Scene/Geometry.hpp>
#	include <Scene/Scene.hpp>
#	include <Scene/SceneFileParser.hpp>
#	include <Scene/SceneNode.hpp>
#	include <Scene/Light/DirectionalLight.hpp>
#	include <Scene/Light/Light.hpp>
#	include <Scene/Light/PointLight.hpp>
#	include <Scene/Light/SpotLight.hpp>
#	include <Shader/FrameVariable.hpp>
#	include <Shader/ShaderProgram.hpp>
#	include <Texture/Sampler.hpp>
#	include <Texture/TextureUnit.hpp>

#	include <iostream>
#	include <string>
#	include <vector>
#	include <cmath>

#	include <SceneObjectsList.hpp>
#	include <ImagesLoader.hpp>
#	include <PropertiesHolder.hpp>
#	include <MaterialsList.hpp>
#	include <RendererSelector.hpp>
#	include <ShaderDialog.hpp>
#	include <SplashScreen.hpp>
#else
#	include <GuiCommonPrerequisites.hpp>
#endif

#if !defined( VLD_AVAILABLE ) && defined( _MSC_VER ) && !defined( NDEBUG )
#	define _CRTDBG_MAP_ALLOC
#	include <cstdlib>
#	include <crtdbg.h>
#endif

#endif
