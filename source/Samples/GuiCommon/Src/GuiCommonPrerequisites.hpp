/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___GUICOMMON_PREREQUISITES_H___
#define ___GUICOMMON_PREREQUISITES_H___

#include <Castor3DPrerequisites.hpp>
#include <wx/wx.h>

namespace GuiCommon
{
	/*!
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 19/10/2011
	\~english
	\brief Word and symbol type enumeration
	\~french
	\brief Enum�ration des diff�rents types de mots et symboles
	*/
	typedef enum eSTC_TYPE
	{
		eSTC_TYPE_DEFAULT,
		eSTC_TYPE_WORD1,
		eSTC_TYPE_WORD2,
		eSTC_TYPE_WORD3,
		eSTC_TYPE_WORD4,
		eSTC_TYPE_WORD5,
		eSTC_TYPE_WORD6,
		eSTC_TYPE_COMMENT,
		eSTC_TYPE_COMMENT_DOC,
		eSTC_TYPE_COMMENT_LINE,
		eSTC_TYPE_COMMENT_SPECIAL,
		eSTC_TYPE_CHARACTER,
		eSTC_TYPE_CHARACTER_EOL,
		eSTC_TYPE_STRING,
		eSTC_TYPE_STRING_EOL,
		eSTC_TYPE_DELIMITER,
		eSTC_TYPE_PUNCTUATION,
		eSTC_TYPE_OPERATOR,
		eSTC_TYPE_BRACE,
		eSTC_TYPE_COMMAND,
		eSTC_TYPE_IDENTIFIER,
		eSTC_TYPE_LABEL,
		eSTC_TYPE_NUMBER,
		eSTC_TYPE_PARAMETER,
		eSTC_TYPE_REGEX,
		eSTC_TYPE_UUID,
		eSTC_TYPE_VALUE,
		eSTC_TYPE_PREPROCESSOR,
		eSTC_TYPE_SCRIPT,
		eSTC_TYPE_ERROR,
		eSTC_TYPE_UNDEFINED,
		eSTC_TYPE_COUNT
	}	eSTC_TYPE;

	typedef enum eBMP
	{
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
		eBMP_COUNT,
	}	eBMP;

	static const int GC_IMG_SIZE = 16;

	class PropertiesHolder;
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

	class SceneObjectsList;
	class FrameVariablesList;
	class ImagesLoader;
	class MaterialsList;
	class RendererSelector;
	class ShaderDialog;
	class SplashScreen;
	class StcTextEditor;

	class LanguageFileContext;
	class LanguageFileParser;
	class StyleInfo;
	class LanguageInfo;
	class StcContext;

	typedef std::shared_ptr< std::thread > thread_sptr;
	typedef std::shared_ptr< LanguageInfo > LanguageInfoPtr;
	typedef std::shared_ptr< LanguageFileContext > LanguageFileContextPtr;
	typedef std::shared_ptr< StyleInfo > StyleInfoPtr;

	DECLARE_MAP( uint32_t, wxImage *, ImageId );
	DECLARE_VECTOR( thread_sptr, ThreadPtr );
	DECLARE_VECTOR( LanguageInfoPtr, LanguageInfoPtr );
	DECLARE_MAP( int, Castor3D::FrameVariableWPtr, FrameVariable );
	DECLARE_ARRAY( StyleInfoPtr, eSTC_TYPE_COUNT, StyleInfoPtr );

	static const wxColour PANEL_BACKGROUND_COLOUR = wxColour( 30, 30, 30 );
	static const wxColour PANEL_FOREGROUND_COLOUR = wxColour( 220, 220, 220 );
	static const wxColour BORDER_COLOUR = wxColour( 90, 90, 90 );
	static const wxColour INACTIVE_TAB_COLOUR = wxColour( 60, 60, 60 );
	static const wxColour ACTIVE_TAB_COLOUR = wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT );
	static const wxColour INACTIVE_TEXT_COLOUR = wxColour( 200, 200, 200 );
	static const wxColour ACTIVE_TEXT_COLOUR = wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHTTEXT );

	/**
	 *\~english
	 *\brief		Copies the buffer into the bitmap
	 *\remark		The buffer must be in BGRA 32bits
	 *\param[in]	p_pBuffer	The buffer
	 *\param[in]	p_uiWidth	The buffer image's width
	 *\param[in]	p_uiHeight	The buffer image's height
	 *\param[out]	p_bitmap	Receives the generated bitmap
	 *\~french
	 *\brief		Copie le buffer donné dans un bitmap
	 *\remark		Le buffer doit être en format BGRA 32bits
	 *\param[in]	p_pBuffer	Le buffer
	 *\param[in]	p_uiWidth	La largeur de l'image
	 *\param[in]	p_uiHeight	La hauteur de l'image
	 *\param[out]	p_bitmap	Re�oit le bitmap g�n�r�
	 */
	void CreateBitmapFromBuffer( uint8_t const * p_pBuffer, uint32_t p_uiWidth, uint32_t p_uiHeight, wxBitmap & p_bitmap );
	/**
	 *\~english
	 *\brief		Copies the unit texture into the bitmap
	 *\remark		The image buffer will be copied in a BGRA 32bits buffer
	 *\param[in]	p_pUnit		The unit
	 *\param[out]	p_bitmap	Receives the generated bitmap
	 *\~french
	 *\brief		Copie la texture de l'unité dans un bitmap
	 *\remark		Le buffer de l'image va être copié dans un buffer BGRA 32bits
	 *\param[in]	p_pUnit		L'unité
	 *\param[out]	p_bitmap	Reçoit le bitmap généré
	 */
	void CreateBitmapFromBuffer( Castor3D::TextureUnitSPtr p_pUnit, wxBitmap & p_bitmap );
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
	Castor3D::WindowHandle make_WindowHandle( wxWindow * p_window );
	/**
	 *\~english
	 *\brief		Loads a font glyphs using wxWidgets
	 *\remark		Uses a custom SFontImpl
	 *\param[in]	p_engine	The Castor3D engine, to check for font existence
	 *\param[in]	p_font		The wxWidgets font
	 *\return		The loaded font
	 *\~french
	 *\brief		Charge les glyphes de la police en utilisant wxWidgets
	 *\remark		Utilise une version personnalisée de SFontImpl
	 *\param[in]	p_engine	Le moteur, pour vérifier l'existance de la police
	 *\param[in]	p_font		La police wxWidgets
	 *\return		La police chargée
	 */
	Castor::FontSPtr make_Font( Castor3D::Engine * p_engine, wxFont const & p_font );
	/**
	 *\~english
	 *\brief		Creates a Castor::String from a wxString
	 *\param[in]	p_value	The wxString
	 *\return		The Castor::String
	 *\~french
	 *\brief		Cree un Castor::String a partir d'un wxString
	 *\param[in]	p_value	Le wxString
	 *\return		Le Castor::String
	 */
	inline Castor::String make_String( wxString const & p_value )
	{
		return Castor::String( p_value.c_str() );
	}
	/**
	 *\~english
	 *\brief		Creates a wxString from a Castor::String
	 *\param[in]	p_value	The Castor::String
	 *\return		The wxString
	 *\~french
	 *\brief		Cree wxString a partir d'un Castor::String
	 *\param[in]	p_value	Le Castor::String
	 *\return		Le wxString
	 */
	inline wxString make_wxString( Castor::String const & p_value )
	{
		return wxString( p_value.c_str() );
	}

#if wxVERSION_NUMBER >= 2900
#	define	wxIMAGE_QUALITY_HIGHEST wxIMAGE_QUALITY_BICUBIC
#else
#	define	wxIMAGE_QUALITY_HIGHEST wxIMAGE_QUALITY_HIGH
#endif

#define wxCOMBO_NEW	_( "New..." )
}

#endif
