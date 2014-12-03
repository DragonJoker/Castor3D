#ifndef ___CV_PrecompiledHeader___
#define ___CV_PrecompiledHeader___

#include <config.hpp>

#if CASTOR_USE_PCH
#	ifdef _WIN32
#		undef __WXGTK__
#		define __WXMSW__
#	endif

#	if defined( _MSC_VER )
#		pragma warning( push )
#		pragma warning( disable:4311 )
#		pragma warning( disable:4312 )
#		pragma warning( disable:4996 )
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
#	include <AnimatedObjectGroup.hpp>
#	include <Animation.hpp>
#	include <Buffer.hpp>
#	include <Camera.hpp>
#	include <DirectionalLight.hpp>
#	include <Engine.hpp>
#	include <Face.hpp>
#	include <FrameListener.hpp>
#	include <FrameVariable.hpp>
#	include <Geometry.hpp>
#	include <KeyFrame.hpp>
#	include <Light.hpp>
#	include <Material.hpp>
#	include <Mesh.hpp>
#	include <Overlay.hpp>
#	include <Pass.hpp>
#	include <Plugin.hpp>
#	include <PointLight.hpp>
#	include <Ray.hpp>
#	include <RenderSystem.hpp>
#	include <RenderWindow.hpp>
#	include <Sampler.hpp>
#	include <Scene.hpp>
#	include <SceneFileParser.hpp>
#	include <SceneNode.hpp>
#	include <ShaderManager.hpp>
#	include <ShaderProgram.hpp>
#	include <SpotLight.hpp>
#	include <Submesh.hpp>
#	include <TextureUnit.hpp>
#	include <Vertex.hpp>

#	include <iostream>
#	include <string>
#	include <vector>
#	include <cmath>

#	ifndef _UNICODE
#		if CASTOR_UNICODE
#			error "CastorViewer compiling in ASCII mode while Castor3D is in UNICODE mode"
#		endif
#	else
#		if ! CASTOR_UNICODE
#			error "CastorViewer compiling in UNICODE mode while Castor3D is in ASCII mode"
#		endif
#	endif

#	include <FrameVariableDialog.hpp>
#	include <GeometriesListFrame.hpp>
#	include <ImagesLoader.hpp>
#	include <MaterialPanel.hpp>
#	include <MaterialsFrame.hpp>
#	include <MaterialsListView.hpp>
#	include <PassPanel.hpp>
#	include <RendererSelector.hpp>
#	include <ShaderDialog.hpp>
#	include <SplashScreen.hpp>

#	if defined( _MSC_VER )
#		pragma warning( pop )
#	endif
#else
#	include <GuiCommonPrerequisites.hpp>
#endif

using namespace GuiCommon;

#endif
