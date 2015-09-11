#ifndef ___CS_PRECOMPILED_HEADER_H___
#define ___CS_PRECOMPILED_HEADER_H___

#include <config.hpp>

#if CASTOR_USE_PCH
#	ifdef _WIN32
#		undef __WXGTK__
#	   define __WXMSW__
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
#	include <RenderWindow.hpp>
#	include <Engine.hpp>
#	include <Plugin.hpp>
#	include <RenderSystem.hpp>
#	include <Ray.hpp>
#	include <FrameListener.hpp>
#	include <Material.hpp>
#	include <Pass.hpp>
#	include <TextureUnit.hpp>
#	include <Vertex.hpp>
#	include <Mesh.hpp>
#	include <Submesh.hpp>
#	include <Geometry.hpp>
#	include <Scene.hpp>
#	include <SceneNode.hpp>
#	include <SceneFileParser.hpp>
#	include <Light.hpp>
#	include <PointLight.hpp>
#	include <DirectionalLight.hpp>
#	include <SpotLight.hpp>
#	include <Animation.hpp>
#	include <KeyFrame.hpp>
#	include <Subdivider.hpp>
#	include <AnimatedObjectGroup.hpp>
#	include <Camera.hpp>
#	include <ShaderManager.hpp>
#	include <ShaderProgram.hpp>
#	include <FrameVariable.hpp>
#	include <Overlay.hpp>
#	include <RenderTarget.hpp>

#	include <iostream>
#	include <string>
#	include <vector>
#	include <cmath>

#	include <ImagesLoader.hpp>
#	include <RendererSelector.hpp>
#	include <ShaderDialog.hpp>
#	include <SplashScreen.hpp>

#	if defined( _MSC_VER )
#		pragma warning( pop )
#	endif
#endif

#endif
