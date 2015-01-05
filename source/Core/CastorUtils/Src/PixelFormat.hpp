/*
This source file is part of ProceduralGenerator (https://sourceforge.net/projects/proceduralgene/)

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
#ifndef ___CU_PixelFormat___
#define ___CU_PixelFormat___

#include "Exception.hpp"
#include "CastorUtilsPrerequisites.hpp"

#include <vector>
#include <algorithm>
#include <numeric>

#if defined( _MSC_VER )
#	define TPL_PIXEL_FORMAT	uint32_t
#else
#	define TPL_PIXEL_FORMAT	ePIXEL_FORMAT
#endif

namespace Castor
{
	/**
	 *\~english
	 *\brief		Helper struct that holds pixel size, to_string and converion functions
	 *\~french
	 *\brief		Structure d'aide contenant la taille d'un pixel ainsi que sa fonction to_string et les fonction de conversion vers d'autres formats
	 */
	template< TPL_PIXEL_FORMAT format > struct pixel_definitions;

	namespace PF
	{
		/**
		 *\~english
		 *\brief		Function to retrieve Pixel size without templates
		 *\param[in]	p_pfFormat	The pixel format
		 *\~french
		 *\brief		Fonction de récuperation de la taille d'un pixel sans templates
		 *\param[in]	p_pfFormat	Le format de pixels
		 */
		uint8_t GetBytesPerPixel( ePIXEL_FORMAT p_pfFormat );
		/**
		 *\~english
		 *\brief		Function to retrieve pixel format from a name
		 *\param[in]	p_strFormat	The pixel format name
		 *\~french
		 *\brief		Fonction de récuperation d'un format de pixel par son nom
		 *\param[in]	p_strFormat	Le nom du format de pixels
		 */
		ePIXEL_FORMAT GetFormatByName( String const & p_strFormat );
		/**
		 *\~english
		 *\brief		Function to retrieve pixel format name
		 *\param[in]	p_eFormat	The pixel format
		 *\~french
		 *\brief		Fonction de récuperation du nom d'un format de pixel
		 *\param[in]	p_eFormat	Le format de pixels
		 */
		String GetFormatName( ePIXEL_FORMAT p_eFormat );
		/**
		 *\~english
		 *\brief		Function to perform convertion without templates
		 *\param[in]		p_eSrcFmt	The source format
		 *\param[in/out]	p_pSrc		The source pixel
		 *\param[in]		p_eDestFmt	The destination format
		 *\param[in/out]	p_pDest		The destination pixel
		 *\~french
		 *\brief		Fonction de conversion sans templates
		 *\param[in]		p_eSrcFmt	Le format de la source
		 *\param[in/out]	p_pSrc		Le pixel source
		 *\param[in]		p_eDestFmt	Le format de la destination
		 *\param[in/out]	p_pDest		Le pixel destination
		 */
		void ConvertPixel( ePIXEL_FORMAT p_eSrcFmt, uint8_t const *& p_pSrc, ePIXEL_FORMAT p_eDestFmt, uint8_t *& p_pDest );
		/**
		 *\~english
		 *\brief		Function to perform convertion without templates
		 *\param[in]	p_eSrcFormat	The source format
		 *\param[in]	p_pSrcBuffer	The source buffer
		 *\param[in]	p_uiSrcSize		The source size
		 *\param[in]	p_eDstFormat	The destination format
		 *\param[in]	p_pDstBuffer	The destination buffer
		 *\param[in]	p_uiDstSize		The destination size
		 *\~french
		 *\brief		Fonction de conversion sans templates
		 *\param[in]	p_eSrcFormat	Le format de la source
		 *\param[in]	p_pSrcBuffer	Le buffer source
		 *\param[in]	p_uiSrcSize		La taille de la source
		 *\param[in]	p_eDstFormat	Le format de la destination
		 *\param[in]	p_pDstBuffer	Le buffer destination
		 *\param[in]	p_uiDstSize		La taille de la destination
		 */
		void ConvertBuffer( ePIXEL_FORMAT p_eSrcFormat, uint8_t const * p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t * p_pDstBuffer, uint32_t p_uiDstSize );
		/**
		 *\~english
		 *\brief		Extracts alpha values from a source buffer holding alpha and puts it in a destination buffer
		 *\remark		Destination buffer will be a L8 pixel format buffer and alpha channel from source buffer will be removed
		 *\param[in]	p_pSrc	The source buffer
		 *\return		The destination buffer, \p nullptr if source didn't have alpha
		 *\~french
		 *\brief		Extrait les valeurs alpha d'un buffer source pour les mettre dans un buffer à part
		 *\remark		Le buffer contenant les valeurs alpha sera au format L8 et le canal alpha du buffer source sera supprimé
		 *\param[in]	p_pSrc	Le buffer source
		 *\return		Le buffer alpha, \p nullptr si la source n'avait pas d'alpha
		 */
		PxBufferBaseSPtr ExtractAlpha( PxBufferBaseSPtr & p_pSrc );
		/**
		 *\see			ePIXEL_FORMAT
		 *\~english
		 *\brief		Checks the alpha component support for given pixel format
		 *\return		\p false if format is depth, stencil or one without alpha
		 *\~french
		 *\brief		Vérifie si le format donné possède une composante alpha
		 *\return		\p false si le format est depth, stencil ou un format sans alpha
		 */
		bool HasAlpha( ePIXEL_FORMAT p_ePf );
		/**
		 *\see			ePIXEL_FORMAT
		 *\~english
		 *\brief		Checks if the given pixel format is a compressed one
		 *\return		The value
		 *\~french
		 *\brief		Vérifie si le format donné est un format compressé
		 *\return		La valeur
		 */
		bool IsCompressed( ePIXEL_FORMAT p_ePf );
		/**
		 *\~english
		 *\brief		Retrieves the pixel format without alpha that is near from the one given
		 *\param[in]	p_ePixelFmt	The pixel format
		 *\return		The given pixel format if none found
		 *\~french
		 *\brief		Récupère le format de pixel sans alpha le plus proche de celui donné
		 *\param[in]	p_ePixelFmt	Le format de pixel
		 *\return		Le format de pixels donné si non trouvé
		 */
		ePIXEL_FORMAT GetPFWithoutAlpha( ePIXEL_FORMAT p_ePixelFmt );
		/**
		 *\~english
		 *\brief			Reduces an image to it's alpha channel, stored in a L8 format buffer
		 *\param[in,out]	p_pSrc	The buffer to reduce
		 *\~english
		 *\brief			Réduit une image à son canal alpha stocké dans un buffer au format L8
		 *\param[in,out]	p_pSrc	Le buffer à réduire
		 */
		void ReduceToAlpha( PxBufferBaseSPtr & p_pSrc );
	}
}

#include "PixelFormat.inl"

#endif
