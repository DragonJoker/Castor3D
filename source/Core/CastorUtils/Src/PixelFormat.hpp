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

namespace Castor
{
	/*!
	\~english
	\brief Pixel component enumeration
	\~french
	\brief Enumération des composantes de Pixel
	*/
	typedef enum ePIXEL_COMPONENT
		: uint8_t
	{
		ePIXEL_COMPONENT_RED,		//!< Red
		ePIXEL_COMPONENT_GREEN,		//!< Green
		ePIXEL_COMPONENT_BLUE,		//!< Blue
		ePIXEL_COMPONENT_ALPHA,		//!< Alpha
		ePIXEL_COMPONENT_LUMINANCE,	//!< Luminance
		ePIXEL_COMPONENT_DEPTH,		//!< Depth
		ePIXEL_COMPONENT_STENCIL,	//!< Stencil
	}	ePIXEL_COMPONENT;
	/*!
	\author 	Sylvain DOREMUS
	\date 		27/05/2013
	\~english
	\brief		Holds colour/depth/stencil component helper functions
	\~french
	\brief		Contient des fonctions d'aides à la conversion de composantes couleur/profondeur/stencil
	*/
	template< TPL_PIXEL_FORMAT PF > struct component;
	/**
	 *\~english
	 *\brief		Helper struct that holds pixel size, to_string and converion functions
	 *\~french
	 *\brief		Structure d'aide contenant la taille d'un pixel ainsi que sa fonction to_string et les fonction de conversion vers d'autres formats
	 */
	template< TPL_PIXEL_FORMAT format > struct pixel_definitions;
	/**
	 *\~english
	 *\brief		Helper struct to tell if a pixel format represents a colour
	 *\~french
	 *\brief		Structure d'aide permettant de dire si un format de pixels représente une couleur
	 */
	template< TPL_PIXEL_FORMAT PF > struct is_colour_format : public std::true_type {};

	template<> struct is_colour_format< ePIXEL_FORMAT_DEPTH16 > : public std::false_type {};
	template<> struct is_colour_format< ePIXEL_FORMAT_DEPTH24 > : public std::false_type {};
	template<> struct is_colour_format< ePIXEL_FORMAT_DEPTH24S8 > : public std::false_type {};
	template<> struct is_colour_format< ePIXEL_FORMAT_DEPTH32 > : public std::false_type {};
	template<> struct is_colour_format< ePIXEL_FORMAT_STENCIL8 > : public std::false_type {};
	/**
	 *\~english
	 *\brief		Helper struct to tell if a pixel format represents a depth pixel
	 *\~french
	 *\brief		Structure d'aide permettant de dire si un format de pixels représente un pixel de profondeur
	 */
	template< TPL_PIXEL_FORMAT PF > struct is_depth_format : public std::false_type {};

	template<> struct is_depth_format< ePIXEL_FORMAT_DEPTH16 > : public std::true_type {};
	template<> struct is_depth_format< ePIXEL_FORMAT_DEPTH24 > : public std::true_type {};
	template<> struct is_depth_format< ePIXEL_FORMAT_DEPTH24S8 > : public std::true_type {};
	template<> struct is_depth_format< ePIXEL_FORMAT_DEPTH32 > : public std::true_type {};
	/**
	 *\~english
	 *\brief		Helper struct to tell if a pixel format represents a stencil pixel
	 *\~french
	 *\brief		Structure d'aide permettant de dire si un format de pixels représente un pixel de stencil
	 */
	template< TPL_PIXEL_FORMAT PF > struct is_stencil_format : public std::false_type {};

	template<> struct is_stencil_format< ePIXEL_FORMAT_DEPTH24S8 > : public std::true_type {};
	template<> struct is_stencil_format< ePIXEL_FORMAT_STENCIL8 > : public std::true_type {};
	/**
	 *\~english
	 *\brief		Helper struct to tell if a pixel format represents a depth or stencil pixel
	 *\~french
	 *\brief		Structure d'aide permettant de dire si un format de pixels représente un pixel de profondeur ou de stencil
	 */
	template< TPL_PIXEL_FORMAT PF > struct is_depth_stencil_format : public std::false_type {};

	template<> struct is_depth_stencil_format< ePIXEL_FORMAT_DEPTH16 > : public std::true_type {};
	template<> struct is_depth_stencil_format< ePIXEL_FORMAT_DEPTH24 > : public std::true_type {};
	template<> struct is_depth_stencil_format< ePIXEL_FORMAT_DEPTH24S8 > : public std::true_type {};
	template<> struct is_depth_stencil_format< ePIXEL_FORMAT_DEPTH32 > : public std::true_type {};
	template<> struct is_depth_stencil_format< ePIXEL_FORMAT_STENCIL8 > : public std::true_type {};

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
		CU_API uint8_t GetBytesPerPixel( ePIXEL_FORMAT p_pfFormat );
		/**
		 *\~english
		 *\brief		Function to retrieve pixel format from a name
		 *\param[in]	p_strFormat	The pixel format name
		 *\~french
		 *\brief		Fonction de récuperation d'un format de pixel par son nom
		 *\param[in]	p_strFormat	Le nom du format de pixels
		 */
		CU_API ePIXEL_FORMAT GetFormatByName( String const & p_strFormat );
		/**
		 *\~english
		 *\brief		Function to retrieve pixel format name
		 *\param[in]	p_eFormat	The pixel format
		 *\~french
		 *\brief		Fonction de récuperation du nom d'un format de pixel
		 *\param[in]	p_eFormat	Le format de pixels
		 */
		CU_API String GetFormatName( ePIXEL_FORMAT p_eFormat );
		/**
		 *\~english
		 *\brief		Function to retrieve pixel colour component in float
		 *\param[in]	p_pixel		The pixel
		 *\param[in]	p_component	The component
		 *\return		The component value
		 *\~french
		 *\brief		Fonction de récupération d'une composante couleur d'un pixel, en flottant
		 *\param[in]	p_pixel		Le pixel
		 *\param[in]	p_component	La composante
		 *\return		La valeur de la composante
		 */
		template< TPL_PIXEL_FORMAT PF >
		float GetFloatComponent( Pixel< PF > const & p_pixel, ePIXEL_COMPONENT p_component )
		{
			return PixelComponent< PF >::GetFloat( p_pixel, p_component );
		}
		/**
		 *\~english
		 *\brief		Function to define pixel colour component in float
		 *\param[in]	p_pixel		The pixel
		 *\param[in]	p_component	The component
		 *\param[in]	p_value		The component value
		 *\~french
		 *\brief		Fonction de définition d'une composante couleur d'un pixel, en flottant
		 *\param[in]	p_pixel		Le pixel
		 *\param[in]	p_component	La composante
		 *\param[in]	p_value		La valeur de la composante
		 */
		template< TPL_PIXEL_FORMAT PF >
		void SetFloatComponent( Pixel< PF > & p_pixel, ePIXEL_COMPONENT p_component, float p_value )
		{
			return PixelComponent< PF >::SetFloat( p_pixel, p_component, p_value );
		}
		/**
		 *\~english
		 *\brief		Function to retrieve pixel colour component in byte
		 *\param[in]	p_pixel		The pixel
		 *\param[in]	p_component	The component
		 *\return		The component value
		 *\~french
		 *\brief		Fonction de récupération d'une composante couleur d'un pixel, en octet
		 *\param[in]	p_pixel		Le pixel
		 *\param[in]	p_component	La composante
		 *\return		La valeur de la composante
		 */
		template< TPL_PIXEL_FORMAT PF >
		uint8_t GetByteComponent( Pixel< PF > const & p_pixel, ePIXEL_COMPONENT p_component )
		{
			return PixelComponent< PF >::GetByte( p_pixel, p_component );
		}
		/**
		 *\~english
		 *\brief		Function to define pixel colour component in byte
		 *\param[in]	p_pixel		The pixel
		 *\param[in]	p_component	The component
		 *\param[in]	p_value		The component value
		 *\~french
		 *\brief		Fonction de définition d'une composante couleur d'un pixel, en octet
		 *\param[in]	p_pixel		Le pixel
		 *\param[in]	p_component	La composante
		 *\param[in]	p_value		La valeur de la composante
		 */
		template< TPL_PIXEL_FORMAT PF >
		void SetByteComponent( Pixel< PF > & p_pixel, ePIXEL_COMPONENT p_component, uint8_t p_value )
		{
			return PixelComponent< PF >::SetByte( p_pixel, p_component, p_value );
		}
		/**
		 *\~english
		 *\brief		Function to retrieve pixel colour component in uint16_t
		 *\param[in]	p_pixel		The pixel
		 *\param[in]	p_component	The component
		 *\return		The component value
		 *\~french
		 *\brief		Fonction de récupération d'une composante couleur d'un pixel, en uint16_t
		 *\param[in]	p_pixel		Le pixel
		 *\param[in]	p_component	La composante
		 *\return		La valeur de la composante
		 */
		template< TPL_PIXEL_FORMAT PF >
		uint16_t GetUInt16Component( Pixel< PF > const & p_pixel, ePIXEL_COMPONENT p_component )
		{
			return PixelComponent< PF >::GetUInt16( p_pixel, p_component );
		}
		/**
		 *\~english
		 *\brief		Function to define pixel colour component in uint16_t
		 *\param[in]	p_pixel		The pixel
		 *\param[in]	p_component	The component
		 *\param[in]	p_value		The component value
		 *\~french
		 *\brief		Fonction de définition d'une composante couleur d'un pixel, en uint16_t
		 *\param[in]	p_pixel		Le pixel
		 *\param[in]	p_component	La composante
		 *\param[in]	p_value		La valeur de la composante
		 */
		template< TPL_PIXEL_FORMAT PF >
		void SetUInt16Component( Pixel< PF > & p_pixel, ePIXEL_COMPONENT p_component, uint16_t p_value )
		{
			return PixelComponent< PF >::SetUInt16( p_pixel, p_component, p_value );
		}
		/**
		 *\~english
		 *\brief		Function to retrieve pixel colour component in uint32_t, with 24 relevant bits
		 *\param[in]	p_pixel		The pixel
		 *\param[in]	p_component	The component
		 *\return		The component value
		 *\~french
		 *\brief		Fonction de récupération d'une composante couleur d'un pixel, en uint32_t, avec 24 bits utiles
		 *\param[in]	p_pixel		Le pixel
		 *\param[in]	p_component	La composante
		 *\return		La valeur de la composante
		 */
		template< TPL_PIXEL_FORMAT PF >
		uint32_t GetUInt24Component( Pixel< PF > const & p_pixel, ePIXEL_COMPONENT p_component )
		{
			return PixelComponent< PF >::GetUInt24( p_pixel, p_component );
		}
		/**
		 *\~english
		 *\brief		Function to define pixel colour component in uint32_t, with 24 relevant bits
		 *\param[in]	p_pixel		The pixel
		 *\param[in]	p_component	The component
		 *\param[in]	p_value		The component value
		 *\~french
		 *\brief		Fonction de définition d'une composante couleur d'un pixel, en uint32_t, avec 24 bits utiles
		 *\param[in]	p_pixel		Le pixel
		 *\param[in]	p_component	La composante
		 *\param[in]	p_value		La valeur de la composante
		 */
		template< TPL_PIXEL_FORMAT PF >
		void SetUInt24Component( Pixel< PF > & p_pixel, ePIXEL_COMPONENT p_component, uint32_t p_value )
		{
			return PixelComponent< PF >::SetUInt24( p_pixel, p_component, p_value );
		}
		/**
		 *\~english
		 *\brief		Function to retrieve pixel colour component in uint32_t
		 *\param[in]	p_pixel		The pixel
		 *\param[in]	p_component	The component
		 *\return		The component value
		 *\~french
		 *\brief		Fonction de récupération d'une composante couleur d'un pixel, en uint32_t
		 *\param[in]	p_pixel		Le pixel
		 *\param[in]	p_component	La composante
		 *\return		La valeur de la composante
		 */
		template< TPL_PIXEL_FORMAT PF >
		uint32_t GetUInt32Component( Pixel< PF > const & p_pixel, ePIXEL_COMPONENT p_component )
		{
			return PixelComponent< PF >::GetUInt32( p_pixel, p_component );
		}
		/**
		 *\~english
		 *\brief		Function to define pixel colour component in uint32_t
		 *\param[in]	p_pixel		The pixel
		 *\param[in]	p_component	The component
		 *\param[in]	p_value		The component value
		 *\~french
		 *\brief		Fonction de définition d'une composante couleur d'un pixel, en uint32_t
		 *\param[in]	p_pixel		Le pixel
		 *\param[in]	p_component	La composante
		 *\param[in]	p_value		La valeur de la composante
		 */
		template< TPL_PIXEL_FORMAT PF >
		void SetUInt32Component( Pixel< PF > & p_pixel, ePIXEL_COMPONENT p_component, uint32_t p_value )
		{
			return PixelComponent< PF >::SetUInt32( p_pixel, p_component, p_value );
		}
		/**
		 *\~english
		 *\brief		Function to retrieve pixel colour component in float
		 *\param[in]	p_pixel		The pixel
		 *\return		The component value
		 *\~french
		 *\brief		Fonction de récupération de la composante rouge d'un pixel, en flottant
		 *\param[in]	p_pixel		Le pixel
		 *\return		La valeur de la composante
		 */
		template< TPL_PIXEL_FORMAT PF >
		float GetFloatRed( Pixel< PF > const & p_pixel )
		{
			return GetFloatComponent( p_pixel, ePIXEL_COMPONENT_RED );
		}
		/**
		 *\~english
		 *\brief		Function to define pixel colour component in float
		 *\param[in]	p_pixel		The pixel
		 *\param[in]	p_value		The component value
		 *\~french
		 *\brief		Fonction de définition de la composante rouge d'un pixel, en flottant
		 *\param[in]	p_pixel		Le pixel
		 *\param[in]	p_value		La valeur de la composante
		 */
		template< TPL_PIXEL_FORMAT PF >
		void SetFloatRed( Pixel< PF > const & p_pixel, float p_value )
		{
			return SetFloatComponent( p_pixel, ePIXEL_COMPONENT_RED, p_value );
		}
		/**
		 *\~english
		 *\brief		Function to retrieve pixel red component in byte
		 *\param[in]	p_pixel		The pixel
		 *\return		The component value
		 *\~french
		 *\brief		Fonction de récupération de la composante rouge d'un pixel, en octet
		 *\param[in]	p_pixel		Le pixel
		 *\return		La valeur de la composante
		 */
		template< TPL_PIXEL_FORMAT PF >
		uint8_t GetByteRed( Pixel< PF > const & p_pixel )
		{
			return GetByteComponent( p_pixel, ePIXEL_COMPONENT_RED );
		}
		/**
		 *\~english
		 *\brief		Function to define pixel red component in byte
		 *\param[in]	p_pixel		The pixel
		 *\param[in]	p_value		The component value
		 *\~french
		 *\brief		Fonction de définition de la composante rouge d'un pixel, en octet
		 *\param[in]	p_pixel		Le pixel
		 *\param[in]	p_value		La valeur de la composante
		 */
		template< TPL_PIXEL_FORMAT PF >
		void SetByteRed( Pixel< PF > & p_pixel, uint8_t p_value )
		{
			return SetByteComponent( p_pixel, ePIXEL_COMPONENT_RED, p_value );
		}
		/**
		 *\~english
		 *\brief		Function to retrieve pixel green component in float
		 *\param[in]	p_pixel		The pixel
		 *\return		The component value
		 *\~french
		 *\brief		Fonction de récupération de la composante verte d'un pixel, en flottant
		 *\param[in]	p_pixel		Le pixel
		 *\return		La valeur de la composante
		 */
		template< TPL_PIXEL_FORMAT PF >
		float GetFloatGreen( Pixel< PF > const & p_pixel )
		{
			return GetFloatComponent( p_pixel, ePIXEL_COMPONENT_GREEN );
		}
		/**
		 *\~english
		 *\brief		Function to define pixel green component in float
		 *\param[in]	p_pixel		The pixel
		 *\param[in]	p_value		The component value
		 *\~french
		 *\brief		Fonction de définition de la composante verte d'un pixel, en flottant
		 *\param[in]	p_pixel		Le pixel
		 *\param[in]	p_value		La valeur de la composante
		 */
		template< TPL_PIXEL_FORMAT PF >
		void SetFloatGreen( Pixel< PF > & p_pixel, float p_value )
		{
			return SetFloatComponent( p_pixel, ePIXEL_COMPONENT_GREEN, p_value );
		}
		/**
		 *\~english
		 *\brief		Function to retrieve pixel green component in byte
		 *\param[in]	p_pixel		The pixel
		 *\return		The component value
		 *\~french
		 *\brief		Fonction de récupération de la composante verte d'un pixel, en octet
		 *\param[in]	p_pixel		Le pixel
		 *\return		La valeur de la composante
		 */
		template< TPL_PIXEL_FORMAT PF >
		uint8_t GetByteGreen( Pixel< PF > const & p_pixel )
		{
			return GetByteComponent( p_pixel, ePIXEL_COMPONENT_GREEN );
		}
		/**
		 *\~english
		 *\brief		Function to define pixel green component in byte
		 *\param[in]	p_pixel		The pixel
		 *\param[in]	p_value		The component value
		 *\~french
		 *\brief		Fonction de définition de la composante verte d'un pixel, en octet
		 *\param[in]	p_pixel		Le pixel
		 *\param[in]	p_value		La valeur de la composante
		 */
		template< TPL_PIXEL_FORMAT PF >
		void SetByteGreen( Pixel< PF > & p_pixel, uint8_t p_value )
		{
			return SetByteComponent( p_pixel, ePIXEL_COMPONENT_GREEN, p_value );
		}
		/**
		 *\~english
		 *\brief		Function to retrieve pixel blue component in float
		 *\param[in]	p_pixel		The pixel
		 *\return		The component value
		 *\~french
		 *\brief		Fonction de récupération de la composante bleue d'un pixel, en flottant
		 *\param[in]	p_pixel		Le pixel
		 *\return		La valeur de la composante
		 */
		template< TPL_PIXEL_FORMAT PF >
		float GetFloatBlue( Pixel< PF > const & p_pixel )
		{
			return GetFloatComponent( p_pixel, ePIXEL_COMPONENT_BLUE );
		}
		/**
		 *\~english
		 *\brief		Function to define pixel blue component in float
		 *\param[in]	p_pixel		The pixel
		 *\param[in]	p_value		The component value
		 *\~french
		 *\brief		Fonction de définition de la composante bleue d'un pixel, en flottant
		 *\param[in]	p_pixel		Le pixel
		 *\param[in]	p_value		La valeur de la composante
		 */
		template< TPL_PIXEL_FORMAT PF >
		void SetFloatBlue( Pixel< PF > & p_pixel, float p_value )
		{
			return SetFloatComponent( p_pixel, ePIXEL_COMPONENT_BLUE, p_value );
		}
		/**
		 *\~english
		 *\brief		Function to retrieve pixel blue component in byte
		 *\param[in]	p_pixel		The pixel
		 *\return		The component value
		 *\~french
		 *\brief		Fonction de récupération de la composante bleue d'un pixel, en octet
		 *\param[in]	p_pixel		Le pixel
		 *\return		La valeur de la composante
		 */
		template< TPL_PIXEL_FORMAT PF >
		uint8_t GetByteBlue( Pixel< PF > const & p_pixel )
		{
			return GetByteComponent( p_pixel, ePIXEL_COMPONENT_BLUE );
		}
		/**
		 *\~english
		 *\brief		Function to define pixel blue component in byte
		 *\param[in]	p_pixel		The pixel
		 *\param[in]	p_value		The component value
		 *\~french
		 *\brief		Fonction de définition de la composante bleue d'un pixel, en octet
		 *\param[in]	p_pixel		Le pixel
		 *\param[in]	p_value		La valeur de la composante
		 */
		template< TPL_PIXEL_FORMAT PF >
		void SetByteBlue( Pixel< PF > & p_pixel, uint8_t p_value )
		{
			return SetByteComponent( p_pixel, ePIXEL_COMPONENT_BLUE, p_value );
		}
		/**
		 *\~english
		 *\brief		Function to retrieve pixel alpha component in float
		 *\param[in]	p_pixel		The pixel
		 *\return		The component value
		 *\~french
		 *\brief		Fonction de récupération de la composante alpha d'un pixel, en flottant
		 *\param[in]	p_pixel		Le pixel
		 *\return		La valeur de la composante
		 */
		template< TPL_PIXEL_FORMAT PF >
		float GetFloatAlpha( Pixel< PF > const & p_pixel )
		{
			return GetFloatComponent( p_pixel, ePIXEL_COMPONENT_ALPHA );
		}
		/**
		 *\~english
		 *\brief		Function to define pixel alpha component in float
		 *\param[in]	p_pixel		The pixel
		 *\param[in]	p_value		The component value
		 *\~french
		 *\brief		Fonction de définition de la composante alpha d'un pixel, en flottant
		 *\param[in]	p_pixel		Le pixel
		 *\param[in]	p_value		La valeur de la composante
		 */
		template< TPL_PIXEL_FORMAT PF >
		void SetFloatAlpha( Pixel< PF > & p_pixel, float p_value )
		{
			return SetFloatComponent( p_pixel, ePIXEL_COMPONENT_ALPHA, p_value );
		}
		/**
		 *\~english
		 *\brief		Function to retrieve pixel alpha component in byte
		 *\param[in]	p_pixel		The pixel
		 *\return		The component value
		 *\~french
		 *\brief		Fonction de récupération de la composante alpha d'un pixel, en octet
		 *\param[in]	p_pixel		Le pixel
		 *\return		La valeur de la composante
		 */
		template< TPL_PIXEL_FORMAT PF >
		uint8_t GetByteAlpha( Pixel< PF > const & p_pixel )
		{
			return GetByteComponent( p_pixel, ePIXEL_COMPONENT_ALPHA );
		}
		/**
		 *\~english
		 *\brief		Function to define pixel alpha component in byte
		 *\param[in]	p_pixel		The pixel
		 *\param[in]	p_value		The component value
		 *\~french
		 *\brief		Fonction de définition de la composante alpha d'un pixel, en octet
		 *\param[in]	p_pixel		Le pixel
		 *\param[in]	p_value		La valeur de la composante
		 */
		template< TPL_PIXEL_FORMAT PF >
		void SetByteAlpha( Pixel< PF > & p_pixel, uint8_t p_value )
		{
			return SetByteComponent( p_pixel, ePIXEL_COMPONENT_ALPHA, p_value );
		}
		/**
		 *\~english
		 *\brief		Function to retrieve pixel luminance component in float
		 *\param[in]	p_pixel		The pixel
		 *\return		The component value
		 *\~french
		 *\brief		Fonction de récupération de la composante luminance d'un pixel, en flottant
		 *\param[in]	p_pixel		Le pixel
		 *\return		La valeur de la composante
		 */
		template< TPL_PIXEL_FORMAT PF >
		float GetFloatLuminance( Pixel< PF > const & p_pixel )
		{
			return GetFloatComponent( p_pixel, ePIXEL_COMPONENT_LUMINANCE );
		}
		/**
		 *\~english
		 *\brief		Function to define pixel luminance component in float
		 *\param[in]	p_pixel		The pixel
		 *\param[in]	p_value		The component value
		 *\~french
		 *\brief		Fonction de définition de la composante luminance d'un pixel, en flottant
		 *\param[in]	p_pixel		Le pixel
		 *\param[in]	p_value		La valeur de la composante
		 */
		template< TPL_PIXEL_FORMAT PF >
		void SetFloatLuminance( Pixel< PF > & p_pixel, float p_value )
		{
			return SetFloatComponent( p_pixel, ePIXEL_COMPONENT_LUMINANCE, p_value );
		}
		/**
		 *\~english
		 *\brief		Function to retrieve pixel luminance component in byte
		 *\param[in]	p_pixel		The pixel
		 *\return		The component value
		 *\~french
		 *\brief		Fonction de récupération de la composante luminance d'un pixel, en octet
		 *\param[in]	p_pixel		Le pixel
		 *\return		La valeur de la composante
		 */
		template< TPL_PIXEL_FORMAT PF >
		uint8_t GetByteLuminance( Pixel< PF > const & p_pixel )
		{
			return GetByteComponent( p_pixel, ePIXEL_COMPONENT_LUMINANCE );
		}
		/**
		 *\~english
		 *\brief		Function to define pixel luminance component in byte
		 *\param[in]	p_pixel		The pixel
		 *\param[in]	p_value		The component value
		 *\~french
		 *\brief		Fonction de définition de la composante luminance d'un pixel, en octet
		 *\param[in]	p_pixel		Le pixel
		 *\param[in]	p_value		La valeur de la composante
		 */
		template< TPL_PIXEL_FORMAT PF >
		void SetByteLuminance( Pixel< PF > & p_pixel, uint8_t p_value )
		{
			return SetByteComponent( p_pixel, ePIXEL_COMPONENT_LUMINANCE, p_value );
		}
		/**
		 *\~english
		 *\brief		Function to retrieve pixel depth component in float
		 *\param[in]	p_pixel		The pixel
		 *\return		The component value
		 *\~french
		 *\brief		Fonction de récupération de la composante profondeur d'un pixel, en flottant
		 *\param[in]	p_pixel		Le pixel
		 *\return		La valeur de la composante
		 */
		template< TPL_PIXEL_FORMAT PF >
		float GetFloatDepth( Pixel< PF > const & p_pixel )
		{
			return GetFloatComponent( p_pixel, ePIXEL_COMPONENT_DEPTH );
		}
		/**
		 *\~english
		 *\brief		Function to define pixel depth component in float
		 *\param[in]	p_pixel		The pixel
		 *\param[in]	p_value		The component value
		 *\~french
		 *\brief		Fonction de définition de la composante profondeur d'un pixel, en flottant
		 *\param[in]	p_pixel		Le pixel
		 *\param[in]	p_value		La valeur de la composante
		 */
		template< TPL_PIXEL_FORMAT PF >
		void SetFloatDepth( Pixel< PF > & p_pixel, float p_value )
		{
			return SetFloatComponent( p_pixel, ePIXEL_COMPONENT_DEPTH, p_value );
		}
		/**
		 *\~english
		 *\brief		Function to retrieve pixel depth component in byte
		 *\param[in]	p_pixel		The pixel
		 *\return		The component value
		 *\~french
		 *\brief		Fonction de récupération de la composante profondeur d'un pixel, en octet
		 *\param[in]	p_pixel		Le pixel
		 *\return		La valeur de la composante
		 */
		template< TPL_PIXEL_FORMAT PF >
		uint8_t GetByteDepth( Pixel< PF > const & p_pixel )
		{
			return GetByteComponent( p_pixel, ePIXEL_COMPONENT_DEPTH );
		}
		/**
		 *\~english
		 *\brief		Function to define pixel depth component in byte
		 *\param[in]	p_pixel		The pixel
		 *\param[in]	p_value		The component value
		 *\~french
		 *\brief		Fonction de définition de la composante profondeur d'un pixel, en octet
		 *\param[in]	p_pixel		Le pixel
		 *\param[in]	p_value		La valeur de la composante
		 */
		template< TPL_PIXEL_FORMAT PF >
		void SetByteDepth( Pixel< PF > & p_pixel, uint8_t p_value )
		{
			return SetByteComponent( p_pixel, ePIXEL_COMPONENT_DEPTH, p_value );
		}
		/**
		 *\~english
		 *\brief		Function to retrieve pixel depth component in uint16_t
		 *\param[in]	p_pixel		The pixel
		 *\return		The component value
		 *\~french
		 *\brief		Fonction de récupération de la composante profondeur d'un pixel, en uint16_t
		 *\param[in]	p_pixel		Le pixel
		 *\return		La valeur de la composante
		 */
		template< TPL_PIXEL_FORMAT PF >
		uint16_t GetUInt16Depth( Pixel< PF > const & p_pixel )
		{
			return GetUInt16Component( p_pixel, ePIXEL_COMPONENT_DEPTH );
		}
		/**
		 *\~english
		 *\brief		Function to define pixel depth component in uint16_t
		 *\param[in]	p_pixel		The pixel
		 *\param[in]	p_value		The component value
		 *\~french
		 *\brief		Fonction de définition de la composante profondeur d'un pixel, en uint16_t
		 *\param[in]	p_pixel		Le pixel
		 *\param[in]	p_value		La valeur de la composante
		 */
		template< TPL_PIXEL_FORMAT PF >
		void SetUInt16Depth( Pixel< PF > & p_pixel, uint16_t p_value )
		{
			return SetUInt16Component( p_pixel, ePIXEL_COMPONENT_DEPTH, p_value );
		}
		/**
		 *\~english
		 *\brief		Function to retrieve pixel depth component in uint32_t, with 24 relevant bits
		 *\param[in]	p_pixel		The pixel
		 *\return		The component value
		 *\~french
		 *\brief		Fonction de récupération de la composante profondeur d'un pixel, en uint32_t, avec 24 bits utiles
		 *\param[in]	p_pixel		Le pixel
		 *\return		La valeur de la composante
		 */
		template< TPL_PIXEL_FORMAT PF >
		uint32_t GetUInt24Depth( Pixel< PF > const & p_pixel )
		{
			return GetUInt24Component( p_pixel, ePIXEL_COMPONENT_DEPTH );
		}
		/**
		 *\~english
		 *\brief		Function to define pixel depth component in uint32_t, with 24 relevant bits
		 *\param[in]	p_pixel		The pixel
		 *\param[in]	p_value		The component value
		 *\~french
		 *\brief		Fonction de définition de la composante profondeur d'un pixel, en uint32_t, avec 24 bits utiles
		 *\param[in]	p_pixel		Le pixel
		 *\param[in]	p_value		La valeur de la composante
		 */
		template< TPL_PIXEL_FORMAT PF >
		void SetUInt24Depth( Pixel< PF > & p_pixel, uint32_t p_value )
		{
			return SetUInt24Component( p_pixel, ePIXEL_COMPONENT_DEPTH, p_value );
		}
		/**
		 *\~english
		 *\brief		Function to retrieve pixel depth component in uint32_t
		 *\param[in]	p_pixel		The pixel
		 *\return		The component value
		 *\~french
		 *\brief		Fonction de récupération de la composante profondeur d'un pixel, en uint32_t
		 *\param[in]	p_pixel		Le pixel
		 *\return		La valeur de la composante
		 */
		template< TPL_PIXEL_FORMAT PF >
		uint32_t GetUInt32Depth( Pixel< PF > const & p_pixel )
		{
			return GetUInt32Component( p_pixel, ePIXEL_COMPONENT_DEPTH );
		}
		/**
		 *\~english
		 *\brief		Function to define pixel depth component in uint32_t
		 *\param[in]	p_pixel		The pixel
		 *\param[in]	p_value		The component value
		 *\~french
		 *\brief		Fonction de définition de la composante profondeur d'un pixel, en uint32_t
		 *\param[in]	p_pixel		Le pixel
		 *\param[in]	p_value		La valeur de la composante
		 */
		template< TPL_PIXEL_FORMAT PF >
		void SetUInt32Depth( Pixel< PF > & p_pixel, uint32_t p_value )
		{
			return SetUInt32Component( p_pixel, ePIXEL_COMPONENT_DEPTH, p_value );
		}
		/**
		 *\~english
		 *\brief		Function to retrieve pixel depth stencil in float
		 *\param[in]	p_pixel		The pixel
		 *\return		The component value
		 *\~french
		 *\brief		Fonction de récupération de la composante stencil d'un pixel, en flottant
		 *\param[in]	p_pixel		Le pixel
		 *\return		La valeur de la composante
		 */
		template< TPL_PIXEL_FORMAT PF >
		float GetFloatStencil( Pixel< PF > const & p_pixel )
		{
			return GetFloatComponent( p_pixel, ePIXEL_COMPONENT_STENCIL );
		}
		/**
		 *\~english
		 *\brief		Function to define pixel depth stencil in float
		 *\param[in]	p_pixel		The pixel
		 *\param[in]	p_value		The component value
		 *\~french
		 *\brief		Fonction de définition de la composante stencil d'un pixel, en flottant
		 *\param[in]	p_pixel		Le pixel
		 *\param[in]	p_value		La valeur de la composante
		 */
		template< TPL_PIXEL_FORMAT PF >
		void SetFloatStencil( Pixel< PF > & p_pixel, float p_value )
		{
			return SetFloatComponent( p_pixel, ePIXEL_COMPONENT_STENCIL, p_value );
		}
		/**
		 *\~english
		 *\brief		Function to retrieve pixel depth stencil in byte
		 *\param[in]	p_pixel		The pixel
		 *\return		The component value
		 *\~french
		 *\brief		Fonction de récupération de la composante stencil d'un pixel, en octet
		 *\param[in]	p_pixel		Le pixel
		 *\return		La valeur de la composante
		 */
		template< TPL_PIXEL_FORMAT PF >
		uint8_t GetByteStencil( Pixel< PF > const & p_pixel )
		{
			return GetByteComponent( p_pixel, ePIXEL_COMPONENT_STENCIL );
		}
		/**
		 *\~english
		 *\brief		Function to define pixel depth stencil in byte
		 *\param[in]	p_pixel		The pixel
		 *\param[in]	p_value		The component value
		 *\~french
		 *\brief		Fonction de définition de la composante stencil d'un pixel, en octet
		 *\param[in]	p_pixel		Le pixel
		 *\param[in]	p_value		La valeur de la composante
		 */
		template< TPL_PIXEL_FORMAT PF >
		void SetByteStencil( Pixel< PF > & p_pixel, uint8_t p_value )
		{
			return SetByteComponent( p_pixel, ePIXEL_COMPONENT_STENCIL, p_value );
		}
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
		CU_API void ConvertPixel( ePIXEL_FORMAT p_eSrcFmt, uint8_t const *& p_pSrc, ePIXEL_FORMAT p_eDestFmt, uint8_t *& p_pDest );
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
		CU_API void ConvertBuffer( ePIXEL_FORMAT p_eSrcFormat, uint8_t const * p_pSrcBuffer, uint32_t p_uiSrcSize, ePIXEL_FORMAT p_eDstFormat, uint8_t * p_pDstBuffer, uint32_t p_uiDstSize );
		/**
		 *\~english
		 *\brief		Extracts alpha values from a source buffer holding alpha and puts it in a destination buffer
		 *\remark		Destination buffer will be a L8 pixel format buffer and alpha channel from source buffer will be removed
		 *\param[in]	p_pSrc	The source buffer
		 *\return		The destination buffer, \p nullptr if source didn't have alpha
		 *\~french
		 *\brief		Extrait les valeurs alpha d'un tampon source pour les mettre dans un tampon à part
		 *\remark		Le tampon contenant les valeurs alpha sera au format L8 et le canal alpha du tampon source sera supprimé
		 *\param[in]	p_pSrc	Le tampon source
		 *\return		Le tampon alpha, \p nullptr si la source n'avait pas d'alpha
		 */
		CU_API PxBufferBaseSPtr ExtractAlpha( PxBufferBaseSPtr & p_pSrc );
		/**
		 *\see			ePIXEL_FORMAT
		 *\~english
		 *\brief		Checks the alpha component support for given pixel format
		 *\return		\p false if format is depth, stencil or one without alpha
		 *\~french
		 *\brief		Vérifie si le format donné possède une composante alpha
		 *\return		\p false si le format est depth, stencil ou un format sans alpha
		 */
		CU_API bool HasAlpha( ePIXEL_FORMAT p_ePf );
		/**
		 *\see			ePIXEL_FORMAT
		 *\~english
		 *\brief		Checks if the given pixel format is a compressed one
		 *\return		The value
		 *\~french
		 *\brief		Vérifie si le format donné est un format compressé
		 *\return		La valeur
		 */
		CU_API bool IsCompressed( ePIXEL_FORMAT p_ePf );
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
		CU_API ePIXEL_FORMAT GetPFWithoutAlpha( ePIXEL_FORMAT p_ePixelFmt );
		/**
		 *\~english
		 *\brief			Reduces a buffer to it's alpha channel, stored in a L8 format buffer
		 *\param[in,out]	p_pSrc	The buffer to reduce
		 *\~english
		 *\brief			Réduit un tampon à son canal alpha stocké dans un tampon au format L8
		 *\param[in,out]	p_pSrc	Le tampon à réduire
		 */
		CU_API void ReduceToAlpha( PxBufferBaseSPtr & p_pSrc );
	}
}

#include "PixelFormat.inl"

#endif
