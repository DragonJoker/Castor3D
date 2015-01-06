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

	class wxFrameVariableDialog;
	class wxGeometryTreeItemData;
	class wxSubmeshTreeItemData;
	class wxGeometriesListFrame;
	class wxImagesLoader;
	class wxMaterialPanel;
	class wxMaterialsFrame;
	class wxMaterialsListView;
	class wxPassPanel;
	class wxRendererSelector;
	class wxShaderDialog;
	class wxSplashScreen;
	class wxStcTextEditor;

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


	/**
	 *\~english
	 *\brief		Creates a WindowHandle from a wxWindow
	 *\param[in]	p_window	The window
	 *\return		The created WindowHandle
	 *\~french
	 *\brief		Cr�e un WindowHandle � partir d'un wxWindow
	 *\param[in]	p_window	La fen�tre
	 *\return		Le WindowHandle cr��
	 */
	Castor3D::WindowHandle wxMakeWindowHandle( wxWindow * p_window );
	/**
	 *\~english
	 *\brief		Copies the buffer into the bitmap
	 *\remark		The buffer must be in BGRA 32bits
	 *\param[in]	p_pBuffer	The buffer
	 *\param[in]	p_uiWidth	The buffer image's width
	 *\param[in]	p_uiHeight	The buffer image's height
	 *\param[out]	p_bitmap	Receives the generated bitmap
	 *\~french
	 *\brief		Copie le buffer donn� dans un bitmap
	 *\remark		Le buffer doit �tre en format BGRA 32bits
	 *\param[in]	p_pBuffer	Le buffer
	 *\param[in]	p_uiWidth	La largeur de l'image
	 *\param[in]	p_uiHeight	La hauteur de l'image
	 *\param[out]	p_bitmap	Re�oit le bitmap g�n�r�
	 */
	void wxCreateBitmapFromBuffer( uint8_t const * p_pBuffer, uint32_t p_uiWidth, uint32_t p_uiHeight, wxBitmap & p_bitmap );
	/**
	 *\~english
	 *\brief		Copies the unit texture into the bitmap
	 *\remark		The image buffer will be copied in a BGRA 32bits buffer
	 *\param[in]	p_pUnit		The unit
	 *\param[out]	p_bitmap	Receives the generated bitmap
	 *\~french
	 *\brief		Copie la texture de l'unit� dans un bitmap
	 *\remark		Le buffer de l'image va �tre copi� dans un buffer BGRA 32bits
	 *\param[in]	p_pUnit		L'unit�
	 *\param[out]	p_bitmap	Re�oit le bitmap g�n�r�
	 */
	void wxCreateBitmapFromBuffer( Castor3D::TextureUnitSPtr p_pUnit, wxBitmap & p_bitmap );

#if wxVERSION_NUMBER >= 2900
#	define	wxIMAGE_QUALITY_HIGHEST wxIMAGE_QUALITY_BICUBIC
#else
#	define	wxIMAGE_QUALITY_HIGHEST wxIMAGE_QUALITY_HIGH
#endif

#define wxCOMBO_NEW	_( "New..." )
}

#endif
