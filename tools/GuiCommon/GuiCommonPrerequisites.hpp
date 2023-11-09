/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_PREREQUISITES_H___
#define ___GUICOMMON_PREREQUISITES_H___

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <wx/wx.h>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

#include <Castor3D/Castor3DPrerequisites.hpp>
#include <SceneExporter/SceneExporter.hpp>
#include <ashespp/Core/WindowHandle.hpp>

namespace GuiCommon
{
	typedef enum eBMP
	{
		eBMP_ANIMATED_OBJECTGROUP,
		eBMP_ANIMATED_OBJECTGROUP_SEL,
		eBMP_ANIMATED_OBJECT,
		eBMP_ANIMATED_OBJECT_SEL,
		eBMP_ANIMATION,
		eBMP_ANIMATION_SEL,
		eBMP_SCENE,
		eBMP_SCENE_SEL,
		eBMP_VIEWPORT,
		eBMP_VIEWPORT_SEL,
		eBMP_RENDER_TARGET,
		eBMP_RENDER_TARGET_SEL,
		eBMP_RENDER_WINDOW,
		eBMP_RENDER_WINDOW_SEL,
		eBMP_FRAME_VARIABLE,
		eBMP_FRAME_VARIABLE_SEL,
		eBMP_FRAME_VARIABLE_BUFFER,
		eBMP_FRAME_VARIABLE_BUFFER_SEL,
		eBMP_NODE,
		eBMP_NODE_SEL,
		eBMP_CAMERA,
		eBMP_CAMERA_SEL,
		eBMP_GEOMETRY,
		eBMP_GEOMETRY_SEL,
		eBMP_DIRECTIONAL_LIGHT,
		eBMP_DIRECTIONAL_LIGHT_SEL,
		eBMP_POINT_LIGHT,
		eBMP_POINT_LIGHT_SEL,
		eBMP_SPOT_LIGHT,
		eBMP_SPOT_LIGHT_SEL,
		eBMP_SUBMESH,
		eBMP_SUBMESH_SEL,
		eBMP_PANEL_OVERLAY,
		eBMP_PANEL_OVERLAY_SEL,
		eBMP_BORDER_PANEL_OVERLAY,
		eBMP_BORDER_PANEL_OVERLAY_SEL,
		eBMP_TEXT_OVERLAY,
		eBMP_TEXT_OVERLAY_SEL,
		eBMP_MATERIAL,
		eBMP_MATERIAL_SEL,
		eBMP_PASS,
		eBMP_PASS_SEL,
		eBMP_TEXTURE,
		eBMP_TEXTURE_SEL,
		eBMP_BILLBOARD,
		eBMP_BILLBOARD_SEL,
		eBMP_POST_EFFECT,
		eBMP_POST_EFFECT_SEL,
		eBMP_TONE_MAPPING,
		eBMP_TONE_MAPPING_SEL,
		eBMP_SKELETON,
		eBMP_SKELETON_SEL,
		eBMP_BONE,
		eBMP_BONE_SEL,
		eBMP_BACKGROUND,
		eBMP_BACKGROUND_SEL,
		eBMP_SSAO_CONFIG,
		eBMP_SSAO_CONFIG_SEL,
		eBMP_CLUSTERS_CONFIG,
		eBMP_CLUSTERS_CONFIG_SEL,
		eBMP_VCT_CONFIG,
		eBMP_VCT_CONFIG_SEL,
		eBMP_COLLAPSE_ALL,
		eBMP_EXPAND_ALL,
		eBMP_CONTROLS,
		eBMP_CONTROLS_SEL,
		eBMP_CONTROL,
		eBMP_CONTROL_SEL,
		eBMP_STYLES,
		eBMP_STYLES_SEL,
		eBMP_STYLE,
		eBMP_STYLE_SEL,
		eBMP_THEME,
		eBMP_THEME_SEL,
		eBMP_COUNT,
	}	eBMP;

	enum class ShaderLanguage
	{
		SPIRV,
#if GC_HasGLSL
		GLSL,
#endif
#if GC_HasHLSL
		HLSL,
#endif
	};

	static const int GC_IMG_SIZE = 16;

	class CastorApplication;
	class I3DController;

	class PropertiesContainer;
	class TreeItemProperty;
	class CameraTreeItemProperty;
	class GeometryTreeItemProperty;
	class LightTreeItemProperty;
	class MaterialTreeItemProperty;
	class NodeTreeItemProperty;
	class OverlayTreeItemProperty;
	class PassTreeItemProperty;
	class SubmeshTreeItemProperty;
	class TextureTreeItemProperty;
	class ButtonEventEditor;

	class SceneObjectsList;
	class FrameVariablesList;
	class ImagesLoader;
	class RendererSelector;
	class ShaderDialog;
	class ShaderEditor;
	class ShaderProgramPage;
	class SplashScreen;
	class StcTextEditor;
	class TreeHolder;
	class PropertiesHolder;

	class LanguageFileContext;
	class LanguageFileParser;
	class StyleInfo;
	class LanguageInfo;
	class StcContext;

	template< typename ListT >
	class TreeListContainerT;

	CU_DeclareSmartPtr( GuiCommon, LanguageInfo, );
	CU_DeclareSmartPtr( GuiCommon, I3DController, );
	CU_DeclareSmartPtr( GuiCommon, TreeItemProperty, );

	CU_DeclareMap( uint32_t, wxImage *, ImageId );
	CU_DeclareVector( LanguageInfoUPtr, LanguageInfo );

	static const wxColour PANEL_BACKGROUND_COLOUR = wxColour( 30, 30, 30 );
	static const wxColour PANEL_FOREGROUND_COLOUR = wxColour( 220, 220, 220 );
	static const wxColour BORDER_COLOUR = wxColour( 90, 90, 90 );
	static const wxColour INACTIVE_TAB_COLOUR = wxColour( 60, 60, 60 );
	static const wxColour INACTIVE_TEXT_COLOUR = wxColour( 200, 200, 200 );
	static const wxColour ACTIVE_TAB_COLOUR = wxColour( 51, 153, 255, 255 );
	static const wxColour ACTIVE_TEXT_COLOUR = wxColour( 255, 255, 255, 255 );

	/**
	 *\~english
	 *\brief		Copies the buffer into the bitmap.
	 *\remarks		The buffer must be in BGRA 32bits.
	 *\param[in]	buffer	The buffer.
	 *\param[in]	width	The buffer image's width.
	 *\param[in]	height	The buffer image's height.
	 *\param[in]	flip	Dit si l'image doit être flippée.
	 *\param[out]	bitmap	Receives the generated bitmap.
	 *\~french
	 *\brief		Copie le buffer donné dans un bitmap.
	 *\remarks		Le buffer doit être en format BGRA 32bits.
	 *\param[in]	buffer	Le buffer.
	 *\param[in]	width	La largeur de l'image.
	 *\param[in]	height	La hauteur de l'image.
	 *\param[in]	flip	Tells if the image mut be flipped.
	 *\param[out]	bitmap	Reçoit le bitmap généré.
	 */
	void createBitmapFromBuffer( uint8_t const * buffer
		, uint32_t width
		, uint32_t height
		, bool flip
		, wxBitmap & bitmap );
	/**
	 *\~english
	 *\brief		Copies the pixel buffer into the bitmap.
	 *\remarks		The buffer will be copied in a BGRA 32bits buffer, if needed.
	 *\param[in]	buffer	The pixel buffer.
	 *\param[in]	flip	Dit si l'image doit être flippée.
	 *\param[out]	bitmap	Receives the generated bitmap.
	 *\~french
	 *\brief		Copie le tampon de pixels dans un bitmap.
	 *\remarks		Le buffer va être copié dans un buffer BGRA 32bits, si nécessaire.
	 *\param[in]	buffer	Le tampon de pixels.
	 *\param[in]	flip	Tells if the image mut be flipped.
	 *\param[out]	bitmap	Reçoit le bitmap généré.
	 */
	void createBitmapFromBuffer( castor::PxBufferBase const & buffer
		, bool flip
		, wxBitmap & bitmap );
	/**
	 *\~english
	 *\brief		Copies the unit texture into the bitmap.
	 *\remarks		The image buffer will be copied in a BGRA 32bits buffer, if needed.
	 *\param[in]	unit	The unit.
	 *\param[in]	flip	Dit si l'image doit être flippée.
	 *\param[out]	bitmap	Receives the generated bitmap.
	 *\~french
	 *\brief		Copie la texture de l'unité dans un bitmap.
	 *\remarks		Le buffer de l'image va être copié dans un buffer BGRA 32bits, si nécessaire.
	 *\param[in]	unit	L'unité.
	 *\param[in]	flip	Tells if the image must be flipped.
	 *\param[out]	bitmap	Reçoit le bitmap généré.
	 */
	void createBitmapFromBuffer( castor3d::TextureUnit const & unit
		, bool flip
		, wxBitmap & bitmap );
	/**
	 *\~english
	 *\brief		Loads a scene.
	 *\param[in]	engine		The engine.
	 *\param[in]	fileName	The scene file name.
	 *\return		true if everything is ok.
	 *\~french
	 *\brief		Charge une scène.
	 *\param[in]	engine		Le moteur.
	 *\param[in]	fileName	Le nom du fichier de scène.
	 *\return		true si tout s'est bien passé.
	 */
	castor3d::RenderWindowDesc loadScene( castor3d::Engine & engine
		, castor::String const & appName
		, castor::Path const & fileName
		, castor3d::ProgressBar * progress );
	/**
	 *\~english
	 *\brief		Loads a scene, asynchronously, notifying an event handler on end.
	 *\remarks		Sends a wxThreadEvent with given ID to the event handler, with the event object being a wxVariant containing the render target address.
	 *\param[in]	engine		The engine.
	 *\param[in]	fileName	The scene file name.
	 *\return		true if everything is ok.
	 *\~french
	 *\brief		Charge une scène de manière asynchrone, en notifiant un event handler à la fin.
	 *\remarks		Envoie un wxThreadEvent avec l'ID donné à l'event handle, avec comme event objet, un wxVariant contenant l'addresse de la render target.
	 *\param[in]	engine		Le moteur.
	 *\param[in]	fileName	Le nom du fichier de scène.
	 *\return		true si tout s'est bien passé.
	 */
	void loadScene( castor3d::Engine & engine
		, castor::String const & appName
		, castor::Path const & fileName
		, castor3d::ProgressBar * progress
		, wxWindow * window
		, int eventID );
	/**
	 *\~english
	 *\brief		Loads the eingine plug-ins.
	 *\param[in]	engine	The engine.
	 *\~french
	 *\brief		Charge les plug-ins du moteur.
	 *\param[in]	engine	Le moteur.
	 */
	void loadPlugins( castor3d::Engine & engine );
	/**
	*\~english
	*\name
	*	Castor to wxWidgets conversions.
	*\~english
	*\name
	*	Conversions de Castor vers wxWidgets.
	*/
	//@{
	wxString make_wxString( castor::String const & value );
	wxString make_wxString( castor::U32String const & value );
	wxSize make_wxSize( castor::Size const & value );
	wxArrayString make_wxArrayString( castor::StringArray const & values );
	//@}
	/**
	*\~english
	*\name
	*	wxWidgets to Castor conversions.
	*\~english
	*\name
	*	Conversions de wxWidgets vers Castor.
	*/
	//@{
	castor::FontUPtr make_Font( wxFont const & font );
	castor::String make_String( wxString const & value );
	castor::U32String make_U32String( wxString const & value );
	castor::Path make_Path( wxString const & value );
	castor::Size makeSize( wxSize const & value );
	castor::StringArray make_StringArray( wxArrayString const & values );
	//@}
	/**
	 *\brief		Builds a wxArrayString from a an array of wxString.
	 *\param[in]	values	The array.
	 *\return		The wxArrayString.
	 */
	template< size_t Count >
	wxArrayString make_wxArrayString( std::array< wxString, Count > values )
	{
		return wxArrayString{ Count, values.data() };
	}
	/**
	 *\brief		Builds a make_wxArrayInt from a an array of int.
	 *\param[in]	values	The array.
	 *\return		The make_wxArrayInt.
	 */
	template< size_t Count >
	wxArrayInt make_wxArrayInt( std::array< int, Count > values )
	{
		wxArrayInt result{ Count };
		std::memcpy( &result[0], values.data(), Count * sizeof( int ) );
		return result;
	}

	ashes::WindowHandle makeWindowHandle( wxWindow * window );

#if wxVERSION_NUMBER >= 2900
#	define	wxIMAGE_QUALITY_HIGHEST wxIMAGE_QUALITY_BICUBIC
#else
#	define	wxIMAGE_QUALITY_HIGHEST wxIMAGE_QUALITY_HIGH
#endif

#define wxCOMBO_NEW	_( "New..." )

	static const wxString CSCN_WILDCARD = wxT( " (*.cscn)|*.cscn|" );
	static const wxString ZIP_WILDCARD = wxT( " (*.zip)|*.zip|" );
}

#if !wxCHECK_VERSION( 3, 1, 0 )
template< typename T >
static T const & FromDIP( T const & t )
{
	return t;
}
#endif

#endif
