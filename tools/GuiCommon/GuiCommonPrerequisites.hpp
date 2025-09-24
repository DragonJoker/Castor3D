/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_PREREQUISITES_H___
#define ___GUICOMMON_PREREQUISITES_H___

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <wx/wx.h>
#include <wx/windowptr.h>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

#include <Castor3D/Castor3DPrerequisites.hpp>
#include <SceneExporter/SceneExporter.hpp>
#include <ashespp/Core/WindowHandle.hpp>

namespace GuiCommon
{
	enum class eBMP
	{
		eAnimatedObjectGroup,
		eAnimatedObjectGroupSelected,
		eAnimatedObject,
		eAnimatedObjectSelected,
		eAnimation,
		eAnimationSelected,
		eScene,
		eSceneSelected,
		eViewport,
		eViewportSelected,
		eRenderTarget,
		eRenderTargetSelected,
		eRenderWindow,
		eRenderWindowSelected,
		eFrameVariable,
		eFrameVariableSelected,
		eFrameVariableBuffer,
		eFrameVariableBufferSelected,
		eNode,
		eNodeSelected,
		eCamera,
		eCameraSelected,
		eGeometry,
		eGeometrySelected,
		eDirectionalLight,
		eDirectionalLightSelected,
		ePointLight,
		ePointLightSelected,
		eSpotLight,
		eSpotLightSelected,
		eSubmesh,
		eSubmeshSelected,
		ePanelOverlay,
		ePanelOverlaySelected,
		eBorderPanelOverlay,
		eBorderPanelOverlaySelected,
		eTextOverlay,
		eTextOverlaySelected,
		eMaterial,
		eMaterialSelected,
		ePass,
		ePassSelected,
		eTexture,
		eTextureSelected,
		eBillboard,
		eBillboardSelected,
		ePostEffect,
		ePostEffectSelected,
		eToneMapping,
		eToneMappingSelected,
		eSkeleton,
		eSkeletonSelected,
		eBone,
		eBoneSelected,
		eBackground,
		eBackgroundSelected,
		eSSAOConfig,
		eSSAOConfigSelected,
		eColourGradingConfig,
		eColourGradingConfigSelected,
		eClustersConfig,
		eClustersConfigSelected,
		eVCTConfig,
		eVCTConfigSelected,
		eCollapseAll,
		eExpandAll,
		eControls,
		eControlsSelected,
		eControl,
		eControlSelected,
		eStyles,
		eStylesSelected,
		eStyle,
		eStyleSelected,
		eTheme,
		eThemeSelected,
		eParticle,
		eParticleSelected,
		eCount,
	};

	enum class ShaderLanguage
	{
		eSPIRV,
#if GC_HasGLSL
		eGLSL,
#endif
#if GC_HasHLSL
		eHLSL,
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

	class SceneObjectsTree;
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
	void createBitmapFromBuffer( c3d::PxBufferBase const & buffer
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
	void createBitmapFromBuffer( c3d::TextureUnit const & unit
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
	c3d::RenderWindowDesc loadScene( c3d::Engine & engine
		, c3d::String const & appName
		, c3d::Path const & fileName
		, c3d::ProgressBar * progress );
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
	void loadScene( c3d::Engine & engine
		, c3d::String const & appName
		, c3d::Path const & fileName
		, c3d::ProgressBar * progress
		, wxWindow const * window
		, int eventID );
	/**
	 *\~english
	 *\brief		Loads the eingine plug-ins.
	 *\param[in]	engine	The engine.
	 *\~french
	 *\brief		Charge les plug-ins du moteur.
	 *\param[in]	engine	Le moteur.
	 */
	void loadPlugins( c3d::Engine & engine );
	/**
	*\~english
	*\name
	*	Castor to wxWidgets conversions.
	*\~english
	*\name
	*	Conversions de Castor vers wxWidgets.
	*/
	//@{
	wxString make_wxString( c3d::MbString const & value );
	wxString make_wxString( c3d::WString const & value );
	wxString make_wxString( c3d::U32String const & value );
	wxSize make_wxSize( c3d::Size const & value );
	wxArrayString make_wxArrayString( c3d::StringArray const & values );
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
	c3d::FontUPtr make_Font( wxFont const & font );
	c3d::String make_String( wxString const & value );
	c3d::U32String make_U32String( wxString const & value );
	c3d::Path make_Path( wxString const & value );
	c3d::Size makeSize( wxSize const & value );
	c3d::StringArray make_StringArray( wxArrayString const & values );
	//@}
	/**
	 *\brief		Builds a wxArrayString from a an array of wxString.
	 *\param[in]	values	The array.
	 *\return		The wxArrayString.
	 */
	template< size_t Count >
	wxArrayString make_wxArrayString( c3d::Array< wxString, Count > values )
	{
		return wxArrayString{ Count, values.data() };
	}
	/**
	 *\brief		Builds a make_wxArrayInt from a an array of int.
	 *\param[in]	values	The array.
	 *\return		The make_wxArrayInt.
	 */
	template< size_t Count >
	wxArrayInt make_wxArrayInt( c3d::Array< int, Count > values )
	{
		wxArrayInt result{ Count };
		std::memcpy( &result[0], values.data(), Count * sizeof( int ) );
		return result;
	}

	ashes::WindowHandle makeWindowHandle( wxWindow const * window );

#if wxVERSION_NUMBER >= 2900
#	define	wxIMAGE_QUALITY_HIGHEST wxIMAGE_QUALITY_BICUBIC
#else
#	define	wxIMAGE_QUALITY_HIGHEST wxIMAGE_QUALITY_HIGH
#endif

#define wxCOMBO_NEW	_( "New..." )

	static const wxString CSCN_WILDCARD = wxT( " (*.cscn)|*.cscn|" );
	static const wxString ZIP_WILDCARD = wxT( " (*.zip)|*.zip|" );

	template< typename WindowT, typename ... ParamsT >
	wxWindowPtr< WindowT > wxMakeWindowPtr( ParamsT && ... params )
	{
		return wxWindowPtr< WindowT >{ new WindowT{ c3d::forward< ParamsT >( params )... } };
	}
}

#if !wxCHECK_VERSION( 3, 1, 0 )
template< typename T >
static T const & FromDIP( T const & t )
{
	return t;
}
#endif

#endif
