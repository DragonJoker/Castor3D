/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_PREREQUISITES_H___
#define ___GUICOMMON_PREREQUISITES_H___

#include <Castor3D/Castor3DPrerequisites.hpp>
#include <ashespp/Core/WindowHandle.hpp>
#include <wx/wx.h>

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
		eBMP_COLLAPSE_ALL,
		eBMP_EXPAND_ALL,
		eBMP_COUNT,
	}	eBMP;

	enum class ShaderLanguage
	{
		SPIRV,
#if C3D_HasGLSL
		GLSL,
#endif
#if GC_HasHLSL
		HLSL,
#endif
	};

	static const int GC_IMG_SIZE = 16;

	class CastorApplication;

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
	class MaterialsList;
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

	typedef std::shared_ptr< std::thread > thread_sptr;
	typedef std::shared_ptr< LanguageInfo > LanguageInfoPtr;
	typedef std::shared_ptr< LanguageFileContext > LanguageFileContextPtr;
	typedef std::shared_ptr< StyleInfo > StyleInfoPtr;

	CU_DeclareMap( uint32_t, wxImage *, ImageId );
	CU_DeclareVector( thread_sptr, ThreadPtr );
	CU_DeclareVector( LanguageInfoPtr, LanguageInfoPtr );

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
	 *\param[in]	p_buffer	The buffer.
	 *\param[in]	p_width		The buffer image's width.
	 *\param[in]	p_height	The buffer image's height.
	 *\param[in]	p_flip		Dit si l'image doit être flippée.
	 *\param[out]	p_bitmap	Receives the generated bitmap.
	 *\~french
	 *\brief		Copie le buffer donné dans un bitmap.
	 *\remarks		Le buffer doit être en format BGRA 32bits.
	 *\param[in]	p_buffer	Le buffer.
	 *\param[in]	p_width		La largeur de l'image.
	 *\param[in]	p_height	La hauteur de l'image.
	 *\param[in]	p_flip		Tells if the image mut be flipped.
	 *\param[out]	p_bitmap	Reçoit le bitmap généré.
	 */
	void CreateBitmapFromBuffer( uint8_t const * p_buffer, uint32_t p_width, uint32_t p_height, bool p_flip, wxBitmap & p_bitmap );
	/**
	 *\~english
	 *\brief		Copies the pixel buffer into the bitmap.
	 *\remarks		The buffer will be copied in a BGRA 32bits buffer, if needed.
	 *\param[in]	p_buffer	The pixel buffer.
	 *\param[in]	p_flip		Dit si l'image doit être flippée.
	 *\param[out]	p_bitmap	Receives the generated bitmap.
	 *\~french
	 *\brief		Copie le tampon de pixels dans un bitmap.
	 *\remarks		Le buffer va être copié dans un buffer BGRA 32bits, si nécessaire.
	 *\param[in]	p_pUnit		Le tampon de pixels.
	 *\param[in]	p_flip		Tells if the image mut be flipped.
	 *\param[out]	p_bitmap	Reçoit le bitmap généré.
	 */
	void CreateBitmapFromBuffer( castor::PxBufferBaseSPtr p_buffer, bool p_flip, wxBitmap & p_bitmap );
	/**
	 *\~english
	 *\brief		Copies the unit texture into the bitmap.
	 *\remarks		The image buffer will be copied in a BGRA 32bits buffer, if needed.
	 *\param[in]	p_unit		The unit.
	 *\param[in]	p_flip		Dit si l'image doit être flippée.
	 *\param[out]	p_bitmap	Receives the generated bitmap.
	 *\~french
	 *\brief		Copie la texture de l'unité dans un bitmap.
	 *\remarks		Le buffer de l'image va être copié dans un buffer BGRA 32bits, si nécessaire.
	 *\param[in]	p_unit		L'unité.
	 *\param[in]	p_flip		Tells if the image must be flipped.
	 *\param[out]	p_bitmap	Reçoit le bitmap généré.
	 */
	void CreateBitmapFromBuffer( castor3d::TextureUnitSPtr p_unit, bool p_flip, wxBitmap & p_bitmap );
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
	castor3d::RenderWindowSPtr loadScene( castor3d::Engine & engine, castor::Path const & fileName );
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
	 *\brief		Creates a WindowHandle from a wxWindow
	 *\param[in]	p_window	The window
	 *\return		The created WindowHandle
	 *\~french
	 *\brief		Crée un WindowHandle à partir d'un wxWindow
	 *\param[in]	p_window	La fenêtre
	 *\return		Le WindowHandle créé
	 */
	ashes::WindowHandle makeWindowHandle( wxWindow * p_window );
	/**
	 *\~english
	 *\brief		Loads a font glyphs using wxWidgets
	 *\remarks		Uses a custom SFontImpl
	 *\param[in]	engine	The Castor3D engine, to check for font existence
	 *\param[in]	p_font		The wxWidgets font
	 *\return		The loaded font
	 *\~french
	 *\brief		Charge les glyphes de la police en utilisant wxWidgets
	 *\remarks		Utilise une version personnalisée de SFontImpl
	 *\param[in]	engine	Le moteur, pour vérifier l'existance de la police
	 *\param[in]	p_font		La police wxWidgets
	 *\return		La police chargée
	 */
	castor::FontSPtr make_Font( castor3d::Engine * engine, wxFont const & p_font );
	/**
	 *\~english
	 *\brief		Creates a castor::String from a wxString
	 *\param[in]	p_value	The wxString
	 *\return		The castor::String
	 *\~french
	 *\brief		Cree un castor::String a partir d'un wxString
	 *\param[in]	p_value	Le wxString
	 *\return		Le castor::String
	 */
	castor::String make_String( wxString const & p_value );
	/**
	 *\~english
	 *\brief		Creates a castor::Path from a wxString
	 *\param[in]	p_value	The wxString
	 *\return		The castor::Path
	 *\~french
	 *\brief		Cree un castor::Path a partir d'un wxString
	 *\param[in]	p_value	Le wxString
	 *\return		Le castor::Path
	 */
	castor::Path make_Path( wxString const & p_value );
	/**
	 *\~english
	 *\brief		Creates a wxString from a castor::String
	 *\param[in]	p_value	The castor::String
	 *\return		The wxString
	 *\~french
	 *\brief		Cree wxString a partir d'un castor::String
	 *\param[in]	p_value	Le castor::String
	 *\return		Le wxString
	 */
	wxString make_wxString( castor::String const & p_value );
	/**
	 *\~english
	 *\brief		Creates a castor::Size from a wxSize
	 *\param[in]	p_value	The wxSize
	 *\return		The castor::Size
	 *\~french
	 *\brief		Cree un castor::Size a partir d'un wxSize
	 *\param[in]	p_value	Le wxSize
	 *\return		Le castor::Size
	 */
	castor::Size makeSize( wxSize const & p_value );
	/**
	 *\~english
	 *\brief		Creates a wxSize from a castor::Size
	 *\param[in]	p_value	The castor::Size
	 *\return		The wxSize
	 *\~french
	 *\brief		Cree wxString a partir d'un castor::Size
	 *\param[in]	p_value	Le castor::Size
	 *\return		Le wxSize
	 */
	wxSize make_wxSize( castor::Size const & p_value );

	ast::ShaderStage convert( VkShaderStageFlagBits stage );

#if wxVERSION_NUMBER >= 2900
#	define	wxIMAGE_QUALITY_HIGHEST wxIMAGE_QUALITY_BICUBIC
#else
#	define	wxIMAGE_QUALITY_HIGHEST wxIMAGE_QUALITY_HIGH
#endif

#define wxCOMBO_NEW	_( "New..." )

	static const wxString CSCN_WILDCARD = wxT( " (*.cscn)|*.cscn|" );
	static const wxString ZIP_WILDCARD = wxT( " (*.zip)|*.zip|" );
}

#endif
