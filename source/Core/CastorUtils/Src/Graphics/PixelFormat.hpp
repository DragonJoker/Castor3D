/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___CU_PixelFormat___
#define ___CU_PixelFormat___

#include "CastorUtilsPrerequisites.hpp"

#include "Exception/Exception.hpp"

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
	enum class PixelComponent
		: uint8_t
	{
		//!\~english Red	\~french Rouge
		eRed,
		//!\~english Green	\~french Vert
		eGreen,
		//!\~english Blue	\~french Bleu
		eBlue,
		//!\~english Alpha	\~french Alpha
		eAlpha,
		//!\~english Luminance	\~french Luminance
		eLuminance,
		//!\~english Depth	\~french Profondeur
		eDepth,
		//!\~english Stencil	\~french Stencil
		eStencil,
		CASTOR_SCOPED_ENUM_BOUNDS( eRed )
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		27/05/2013
	\~english
	\brief		Holds colour/depth/stencil component helper functions
	\~french
	\brief		Contient des fonctions d'aides à la conversion de composantes couleur/profondeur/stencil
	*/
	template< PixelFormat PF > struct component;
	/**
	 *\~english
	 *\brief		Helper struct that holds pixel size, to_string and converion functions
	 *\~french
	 *\brief		Structure d'aide contenant la taille d'un pixel ainsi que sa fonction to_string et les fonction de conversion vers d'autres formats
	 */
	template< PixelFormat format > struct pixel_definitions;
	/**
	 *\~english
	 *\brief		Helper struct to tell if a pixel format represents a colour
	 *\~french
	 *\brief		Structure d'aide permettant de dire si un format de pixels représente une couleur
	 */
	template< PixelFormat PF > struct is_colour_format : public std::true_type {};

	template<> struct is_colour_format< PixelFormat::eD16 > : public std::false_type {};
	template<> struct is_colour_format< PixelFormat::eD24 > : public std::false_type {};
	template<> struct is_colour_format< PixelFormat::eD24S8 > : public std::false_type {};
	template<> struct is_colour_format< PixelFormat::eD32 > : public std::false_type {};
	template<> struct is_colour_format< PixelFormat::eD32F > : public std::false_type {};
	template<> struct is_colour_format< PixelFormat::eS1 > : public std::false_type {};
	template<> struct is_colour_format< PixelFormat::eS8 > : public std::false_type {};
	/**
	 *\~english
	 *\brief		Helper struct to tell if a pixel format represents a depth pixel
	 *\~french
	 *\brief		Structure d'aide permettant de dire si un format de pixels représente un pixel de profondeur
	 */
	template< PixelFormat PF > struct is_depth_format : public std::false_type {};

	template<> struct is_depth_format< PixelFormat::eD16 > : public std::true_type {};
	template<> struct is_depth_format< PixelFormat::eD24 > : public std::true_type {};
	template<> struct is_depth_format< PixelFormat::eD24S8 > : public std::true_type {};
	template<> struct is_depth_format< PixelFormat::eD32 > : public std::true_type {};
	template<> struct is_depth_format< PixelFormat::eD32F > : public std::true_type {};
	/**
	 *\~english
	 *\brief		Helper struct to tell if a pixel format represents a stencil pixel
	 *\~french
	 *\brief		Structure d'aide permettant de dire si un format de pixels représente un pixel de stencil
	 */
	template< PixelFormat PF > struct is_stencil_format : public std::false_type {};

	template<> struct is_stencil_format< PixelFormat::eD24S8 > : public std::true_type {};
	template<> struct is_stencil_format< PixelFormat::eS1 > : public std::true_type {};
	template<> struct is_stencil_format< PixelFormat::eS8 > : public std::true_type {};
	/**
	 *\~english
	 *\brief		Helper struct to tell if a pixel format represents a depth or stencil pixel
	 *\~french
	 *\brief		Structure d'aide permettant de dire si un format de pixels représente un pixel de profondeur ou de stencil
	 */
	template< PixelFormat PF > struct is_depth_stencil_format : public std::false_type {};

	template<> struct is_depth_stencil_format< PixelFormat::eD16 > : public std::true_type {};
	template<> struct is_depth_stencil_format< PixelFormat::eD24 > : public std::true_type {};
	template<> struct is_depth_stencil_format< PixelFormat::eD24S8 > : public std::true_type {};
	template<> struct is_depth_stencil_format< PixelFormat::eD32 > : public std::true_type {};
	template<> struct is_depth_stencil_format< PixelFormat::eD32F > : public std::true_type {};
	template<> struct is_depth_stencil_format< PixelFormat::eS1 > : public std::true_type {};
	template<> struct is_depth_stencil_format< PixelFormat::eS8 > : public std::true_type {};

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
		CU_API uint8_t GetBytesPerPixel( PixelFormat p_pfFormat );
		/**
		 *\~english
		 *\brief		Function to retrieve pixel format from a name
		 *\param[in]	p_strFormat	The pixel format name
		 *\~french
		 *\brief		Fonction de récuperation d'un format de pixel par son nom
		 *\param[in]	p_strFormat	Le nom du format de pixels
		 */
		CU_API PixelFormat GetFormatByName( String const & p_strFormat );
		/**
		 *\~english
		 *\brief		Function to retrieve pixel format name
		 *\param[in]	p_format	The pixel format
		 *\~french
		 *\brief		Fonction de récuperation du nom d'un format de pixel
		 *\param[in]	p_format	Le format de pixels
		 */
		CU_API String GetFormatName( PixelFormat p_format );
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
		template< PixelFormat PF >
		float GetFloatComponent( Pixel< PF > const & p_pixel, PixelComponent p_component );
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
		template< PixelFormat PF >
		void SetFloatComponent( Pixel< PF > & p_pixel, PixelComponent p_component, float p_value );
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
		template< PixelFormat PF >
		uint8_t GetByteComponent( Pixel< PF > const & p_pixel, PixelComponent p_component );
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
		template< PixelFormat PF >
		void SetByteComponent( Pixel< PF > & p_pixel, PixelComponent p_component, uint8_t p_value );
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
		template< PixelFormat PF >
		uint16_t GetUInt16Component( Pixel< PF > const & p_pixel, PixelComponent p_component );
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
		template< PixelFormat PF >
		void SetUInt16Component( Pixel< PF > & p_pixel, PixelComponent p_component, uint16_t p_value );
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
		template< PixelFormat PF >
		uint32_t GetUInt24Component( Pixel< PF > const & p_pixel, PixelComponent p_component );
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
		template< PixelFormat PF >
		void SetUInt24Component( Pixel< PF > & p_pixel, PixelComponent p_component, uint32_t p_value );
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
		template< PixelFormat PF >
		uint32_t GetUInt32Component( Pixel< PF > const & p_pixel, PixelComponent p_component );
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
		template< PixelFormat PF >
		void SetUInt32Component( Pixel< PF > & p_pixel, PixelComponent p_component, uint32_t p_value );
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
		template< PixelFormat PF >
		float GetFloatRed( Pixel< PF > const & p_pixel )
		{
			return GetFloatComponent( p_pixel, PixelComponent::eRed );
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
		template< PixelFormat PF >
		void SetFloatRed( Pixel< PF > const & p_pixel, float p_value )
		{
			return SetFloatComponent( p_pixel, PixelComponent::eRed, p_value );
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
		template< PixelFormat PF >
		uint8_t GetByteRed( Pixel< PF > const & p_pixel )
		{
			return GetByteComponent( p_pixel, PixelComponent::eRed );
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
		template< PixelFormat PF >
		void SetByteRed( Pixel< PF > & p_pixel, uint8_t p_value )
		{
			return SetByteComponent( p_pixel, PixelComponent::eRed, p_value );
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
		template< PixelFormat PF >
		float GetFloatGreen( Pixel< PF > const & p_pixel )
		{
			return GetFloatComponent( p_pixel, PixelComponent::eGreen );
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
		template< PixelFormat PF >
		void SetFloatGreen( Pixel< PF > & p_pixel, float p_value )
		{
			return SetFloatComponent( p_pixel, PixelComponent::eGreen, p_value );
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
		template< PixelFormat PF >
		uint8_t GetByteGreen( Pixel< PF > const & p_pixel )
		{
			return GetByteComponent( p_pixel, PixelComponent::eGreen );
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
		template< PixelFormat PF >
		void SetByteGreen( Pixel< PF > & p_pixel, uint8_t p_value )
		{
			return SetByteComponent( p_pixel, PixelComponent::eGreen, p_value );
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
		template< PixelFormat PF >
		float GetFloatBlue( Pixel< PF > const & p_pixel )
		{
			return GetFloatComponent( p_pixel, PixelComponent::eBlue );
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
		template< PixelFormat PF >
		void SetFloatBlue( Pixel< PF > & p_pixel, float p_value )
		{
			return SetFloatComponent( p_pixel, PixelComponent::eBlue, p_value );
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
		template< PixelFormat PF >
		uint8_t GetByteBlue( Pixel< PF > const & p_pixel )
		{
			return GetByteComponent( p_pixel, PixelComponent::eBlue );
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
		template< PixelFormat PF >
		void SetByteBlue( Pixel< PF > & p_pixel, uint8_t p_value )
		{
			return SetByteComponent( p_pixel, PixelComponent::eBlue, p_value );
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
		template< PixelFormat PF >
		float GetFloatAlpha( Pixel< PF > const & p_pixel )
		{
			return GetFloatComponent( p_pixel, PixelComponent::eAlpha );
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
		template< PixelFormat PF >
		void SetFloatAlpha( Pixel< PF > & p_pixel, float p_value )
		{
			return SetFloatComponent( p_pixel, PixelComponent::eAlpha, p_value );
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
		template< PixelFormat PF >
		uint8_t GetByteAlpha( Pixel< PF > const & p_pixel )
		{
			return GetByteComponent( p_pixel, PixelComponent::eAlpha );
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
		template< PixelFormat PF >
		void SetByteAlpha( Pixel< PF > & p_pixel, uint8_t p_value )
		{
			return SetByteComponent( p_pixel, PixelComponent::eAlpha, p_value );
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
		template< PixelFormat PF >
		float GetFloatLuminance( Pixel< PF > const & p_pixel )
		{
			return GetFloatComponent( p_pixel, PixelComponent::eLuminance );
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
		template< PixelFormat PF >
		void SetFloatLuminance( Pixel< PF > & p_pixel, float p_value )
		{
			return SetFloatComponent( p_pixel, PixelComponent::eLuminance, p_value );
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
		template< PixelFormat PF >
		uint8_t GetByteLuminance( Pixel< PF > const & p_pixel )
		{
			return GetByteComponent( p_pixel, PixelComponent::eLuminance );
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
		template< PixelFormat PF >
		void SetByteLuminance( Pixel< PF > & p_pixel, uint8_t p_value )
		{
			return SetByteComponent( p_pixel, PixelComponent::eLuminance, p_value );
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
		template< PixelFormat PF >
		float GetFloatDepth( Pixel< PF > const & p_pixel )
		{
			return GetFloatComponent( p_pixel, PixelComponent::eDepth );
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
		template< PixelFormat PF >
		void SetFloatDepth( Pixel< PF > & p_pixel, float p_value )
		{
			return SetFloatComponent( p_pixel, PixelComponent::eDepth, p_value );
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
		template< PixelFormat PF >
		uint8_t GetByteDepth( Pixel< PF > const & p_pixel )
		{
			return GetByteComponent( p_pixel, PixelComponent::eDepth );
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
		template< PixelFormat PF >
		void SetByteDepth( Pixel< PF > & p_pixel, uint8_t p_value )
		{
			return SetByteComponent( p_pixel, PixelComponent::eDepth, p_value );
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
		template< PixelFormat PF >
		uint16_t GetUInt16Depth( Pixel< PF > const & p_pixel )
		{
			return GetUInt16Component( p_pixel, PixelComponent::eDepth );
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
		template< PixelFormat PF >
		void SetUInt16Depth( Pixel< PF > & p_pixel, uint16_t p_value )
		{
			return SetUInt16Component( p_pixel, PixelComponent::eDepth, p_value );
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
		template< PixelFormat PF >
		uint32_t GetUInt24Depth( Pixel< PF > const & p_pixel )
		{
			return GetUInt24Component( p_pixel, PixelComponent::eDepth );
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
		template< PixelFormat PF >
		void SetUInt24Depth( Pixel< PF > & p_pixel, uint32_t p_value )
		{
			return SetUInt24Component( p_pixel, PixelComponent::eDepth, p_value );
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
		template< PixelFormat PF >
		uint32_t GetUInt32Depth( Pixel< PF > const & p_pixel )
		{
			return GetUInt32Component( p_pixel, PixelComponent::eDepth );
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
		template< PixelFormat PF >
		void SetUInt32Depth( Pixel< PF > & p_pixel, uint32_t p_value )
		{
			return SetUInt32Component( p_pixel, PixelComponent::eDepth, p_value );
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
		template< PixelFormat PF >
		float GetFloatStencil( Pixel< PF > const & p_pixel )
		{
			return GetFloatComponent( p_pixel, PixelComponent::eStencil );
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
		template< PixelFormat PF >
		void SetFloatStencil( Pixel< PF > & p_pixel, float p_value )
		{
			return SetFloatComponent( p_pixel, PixelComponent::eStencil, p_value );
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
		template< PixelFormat PF >
		uint8_t GetByteStencil( Pixel< PF > const & p_pixel )
		{
			return GetByteComponent( p_pixel, PixelComponent::eStencil );
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
		template< PixelFormat PF >
		void SetByteStencil( Pixel< PF > & p_pixel, uint8_t p_value )
		{
			return SetByteComponent( p_pixel, PixelComponent::eStencil, p_value );
		}
		/**
		 *\~english
		 *\brief		Function to perform convertion without templates
		 *\param[in]		p_eSrcFmt	The source format
		 *\param[in,out]	p_pSrc		The source pixel
		 *\param[in]		p_eDestFmt	The destination format
		 *\param[in,out]	p_pDest		The destination pixel
		 *\~french
		 *\brief		Fonction de conversion sans templates
		 *\param[in]		p_eSrcFmt	Le format de la source
		 *\param[in,out]	p_pSrc		Le pixel source
		 *\param[in]		p_eDestFmt	Le format de la destination
		 *\param[in,out]	p_pDest		Le pixel destination
		 */
		CU_API void ConvertPixel( PixelFormat p_eSrcFmt, uint8_t const *& p_pSrc, PixelFormat p_eDestFmt, uint8_t *& p_pDest );
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
		CU_API void ConvertBuffer( PixelFormat p_eSrcFormat, uint8_t const * p_pSrcBuffer, uint32_t p_uiSrcSize, PixelFormat p_eDstFormat, uint8_t * p_pDstBuffer, uint32_t p_uiDstSize );
		/**
		 *\~english
		 *\brief		Extracts alpha values from a source buffer holding alpha and puts it in a destination buffer
		 *\remarks		Destination buffer will be a L8 pixel format buffer and alpha channel from source buffer will be removed
		 *\param[in]	p_pSrc	The source buffer
		 *\return		The destination buffer, \p nullptr if source didn't have alpha
		 *\~french
		 *\brief		Extrait les valeurs alpha d'un tampon source pour les mettre dans un tampon à part
		 *\remarks		Le tampon contenant les valeurs alpha sera au format L8 et le canal alpha du tampon source sera supprimé
		 *\param[in]	p_pSrc	Le tampon source
		 *\return		Le tampon alpha, \p nullptr si la source n'avait pas d'alpha
		 */
		CU_API PxBufferBaseSPtr ExtractAlpha( PxBufferBaseSPtr & p_pSrc );
		/**
		 *\see			PixelFormat
		 *\~english
		 *\brief		Checks the alpha component support for given pixel format
		 *\return		\p false if format is depth, stencil or one without alpha
		 *\~french
		 *\brief		Vérifie si le format donné possède une composante alpha
		 *\return		\p false si le format est depth, stencil ou un format sans alpha
		 */
		CU_API bool HasAlpha( PixelFormat p_ePf );
		/**
		 *\see			PixelFormat
		 *\~english
		 *\brief		Checks if the given pixel format is a compressed one
		 *\return		The value
		 *\~french
		 *\brief		Vérifie si le format donné est un format compressé
		 *\return		La valeur
		 */
		CU_API bool IsCompressed( PixelFormat p_ePf );
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
		CU_API PixelFormat GetPFWithoutAlpha( PixelFormat p_ePixelFmt );
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
