/*
See LICENSE file in root folder
*/
#ifndef ___CV_PrecompiledHeader___
#define ___CV_PrecompiledHeader___

#include <CastorUtils/config.hpp>

#if CU_UsePCH
#	ifdef CU_PlatformWindows
#		undef __WXGTK__
#		define __WXMSW__
#	endif

#	pragma warning( push )
#	pragma warning( disable: 4365 )
#	include <iostream>
#	include <memory>
#	pragma warning( pop )

#	include <string>

#	pragma warning( push )
#	pragma warning( disable: 4365 )
#	pragma warning( disable: 4371 )
#	pragma warning( disable: 5054 )
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
#	pragma warning( pop )

#	include <CastorUtils/CastorUtils.hpp>
#	include <CastorUtils/Miscellaneous/StringUtils.hpp>
#	include <CastorUtils/Design/Templates.hpp>
#	include <CastorUtils/Log/Logger.hpp>
#	include <CastorUtils/Miscellaneous/PreciseTimer.hpp>
#	include <CastorUtils/Data/File.hpp>
#	include <CastorUtils/Math/Point.hpp>
#	include <CastorUtils/Math/Quaternion.hpp>
#	include <CastorUtils/Design/Named.hpp>
#	include <CastorUtils/Data/Path.hpp>
#	include <CastorUtils/Math/Math.hpp>
#	include <CastorUtils/Graphics/Colour.hpp>
#	include <CastorUtils/Math/Angle.hpp>

#	include <Castor3D/Engine.hpp>
#	include <Castor3D/Animation/Animation.hpp>
#	include <Castor3D/Animation/AnimationKeyFrame.hpp>
#	include <Castor3D/Cache/ShaderCache.hpp>
#	include <Castor3D/Event/Frame/FrameListener.hpp>
#	include <Castor3D/Material/Material.hpp>
#	include <Castor3D/Material/Pass/Pass.hpp>
#	include <Castor3D/Model/Mesh/Submesh/Component/Face.hpp>
#	include <Castor3D/Model/Mesh/Mesh.hpp>
#	include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#	include <Castor3D/Model/Vertex.hpp>
#	include <Castor3D/Render/Ray.hpp>
#	include <Castor3D/Overlay/Overlay.hpp>
#	include <Castor3D/Plugin/Plugin.hpp>
#	include <Castor3D/Render/RenderSystem.hpp>
#	include <Castor3D/Render/RenderWindow.hpp>
#	include <Castor3D/Scene/Camera.hpp>
#	include <Castor3D/Scene/Geometry.hpp>
#	include <Castor3D/Scene/Scene.hpp>
#	include <Castor3D/Scene/SceneFileParser.hpp>
#	include <Castor3D/Scene/SceneNode.hpp>
#	include <Castor3D/Scene/Animation/AnimatedObjectGroup.hpp>
#	include <Castor3D/Scene/Light/DirectionalLight.hpp>
#	include <Castor3D/Scene/Light/Light.hpp>
#	include <Castor3D/Scene/Light/PointLight.hpp>
#	include <Castor3D/Scene/Light/SpotLight.hpp>
#	include <Castor3D/Material/Texture/Sampler.hpp>
#	include <Castor3D/Material/Texture/TextureUnit.hpp>

#	include <vector>
#	include <cmath>

#	include <GuiCommon/Properties/PropertiesContainer.hpp>
#	include <GuiCommon/Properties/PropertiesDialog.hpp>
#	include <GuiCommon/Properties/PropertiesHolder.hpp>
#	include <GuiCommon/Shader/ShaderDialog.hpp>
#	include <GuiCommon/System/SceneObjectsList.hpp>
#	include <GuiCommon/System/ImagesLoader.hpp>
#	include <GuiCommon/System/MaterialsList.hpp>
#	include <GuiCommon/System/RendererSelector.hpp>
#	include <GuiCommon/System/SplashScreen.hpp>
#else
#	include <GuiCommon/GuiCommonPrerequisites.hpp>
#endif

#if !defined( VLD_AVAILABLE ) && defined( CU_PlatformWindows ) && !defined( NDEBUG )
#	define _CRTDBG_MAP_ALLOC
#	include <cstdlib>
#	include <crtdbg.h>
#endif

#endif
