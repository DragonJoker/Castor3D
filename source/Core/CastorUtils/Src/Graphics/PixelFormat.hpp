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

namespace castor
{
	/*!
	\~english
	\brief Pixel PixelComponents enumeration
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
	\brief		Holds colour/depth/stencil PixelComponents helper functions
	\~french
	\brief		Contient des fonctions d'aides à la conversion de composantes couleur/profondeur/stencil
	*/
	template< PixelFormat PF > struct PixelComponents;
	/**
	 *\~english
	 *\brief		Helper struct that holds pixel size, toString and converion functions
	 *\~french
	 *\brief		Structure d'aide contenant la taille d'un pixel ainsi que sa fonction toString et les fonction de conversion vers d'autres formats
	 */
	template< PixelFormat format > struct PixelDefinitions;
	/**
	 *\~english
	 *\brief		Helper struct to tell if a pixel format represents a colour
	 *\~french
	 *\brief		Structure d'aide permettant de dire si un format de pixels représente une couleur
	 */
	template< PixelFormat PF > struct IsColourFormat : public std::true_type {};

	template<> struct IsColourFormat< PixelFormat::eD16 > : public std::false_type {};
	template<> struct IsColourFormat< PixelFormat::eD24 > : public std::false_type {};
	template<> struct IsColourFormat< PixelFormat::eD24S8 > : public std::false_type {};
	template<> struct IsColourFormat< PixelFormat::eD32 > : public std::false_type {};
	template<> struct IsColourFormat< PixelFormat::eD32F > : public std::false_type {};
	template<> struct IsColourFormat< PixelFormat::eD32FS8 > : public std::false_type {};
	template<> struct IsColourFormat< PixelFormat::eS1 > : public std::false_type {};
	template<> struct IsColourFormat< PixelFormat::eS8 > : public std::false_type {};
	/**
	 *\~english
	 *\brief		Helper struct to tell if a pixel format represents a depth pixel
	 *\~french
	 *\brief		Structure d'aide permettant de dire si un format de pixels représente un pixel de profondeur
	 */
	template< PixelFormat PF > struct IsDepthFormat : public std::false_type {};

	template<> struct IsDepthFormat< PixelFormat::eD16 > : public std::true_type {};
	template<> struct IsDepthFormat< PixelFormat::eD24 > : public std::true_type {};
	template<> struct IsDepthFormat< PixelFormat::eD24S8 > : public std::true_type {};
	template<> struct IsDepthFormat< PixelFormat::eD32 > : public std::true_type {};
	template<> struct IsDepthFormat< PixelFormat::eD32F > : public std::true_type {};
	template<> struct IsDepthFormat< PixelFormat::eD32FS8 > : public std::true_type {};
	/**
	 *\~english
	 *\brief		Helper struct to tell if a pixel format represents a stencil pixel
	 *\~french
	 *\brief		Structure d'aide permettant de dire si un format de pixels représente un pixel de stencil
	 */
	template< PixelFormat PF > struct IsStencilFormat : public std::false_type {};

	template<> struct IsStencilFormat< PixelFormat::eD24S8 > : public std::true_type {};
	template<> struct IsStencilFormat< PixelFormat::eD32FS8 > : public std::true_type {};
	template<> struct IsStencilFormat< PixelFormat::eS1 > : public std::true_type {};
	template<> struct IsStencilFormat< PixelFormat::eS8 > : public std::true_type {};
	/**
	 *\~english
	 *\brief		Helper struct to tell if a pixel format represents a depth or stencil pixel
	 *\~french
	 *\brief		Structure d'aide permettant de dire si un format de pixels représente un pixel de profondeur ou de stencil
	 */
	template< PixelFormat PF > struct IsDepthStencilFormat : public std::false_type {};

	template<> struct IsDepthStencilFormat< PixelFormat::eD16 > : public std::true_type {};
	template<> struct IsDepthStencilFormat< PixelFormat::eD24 > : public std::true_type {};
	template<> struct IsDepthStencilFormat< PixelFormat::eD24S8 > : public std::true_type {};
	template<> struct IsDepthStencilFormat< PixelFormat::eD32 > : public std::true_type {};
	template<> struct IsDepthStencilFormat< PixelFormat::eD32F > : public std::true_type {};
	template<> struct IsDepthStencilFormat< PixelFormat::eD32FS8 > : public std::true_type {};
	template<> struct IsDepthStencilFormat< PixelFormat::eS1 > : public std::true_type {};
	template<> struct IsDepthStencilFormat< PixelFormat::eS8 > : public std::true_type {};

	namespace PF
	{
		/**
		 *\~english
		 *\brief		Function to retrieve Pixel size without templates
		 *\param[in]	p_format	The pixel format
		 *\~french
		 *\brief		Fonction de récuperation de la taille d'un pixel sans templates
		 *\param[in]	p_format	Le format de pixels
		 */
		CU_API uint8_t getBytesPerPixel( PixelFormat p_format );
		/**
		 *\~english
		 *\brief		Function to retrieve the number of components of a pixel format.
		 *\param[in]	p_format	The pixel format
		 *\~french
		 *\brief		Fonction de récuperation du nombre de composantes d'un format de pixel.
		 *\param[in]	p_format	Le format de pixels
		 */
		CU_API uint8_t getComponentsCount( PixelFormat p_format );
		/**
		 *\~english
		 *\brief		Function to retrieve pixel format from a name
		 *\param[in]	p_strFormat	The pixel format name
		 *\~french
		 *\brief		Fonction de récuperation d'un format de pixel par son nom
		 *\param[in]	p_strFormat	Le nom du format de pixels
		 */
		CU_API PixelFormat getFormatByName( String const & p_strFormat );
		/**
		 *\~english
		 *\brief		Function to retrieve pixel format name
		 *\param[in]	p_format	The pixel format
		 *\~french
		 *\brief		Fonction de récuperation du nom d'un format de pixel
		 *\param[in]	p_format	Le format de pixels
		 */
		CU_API String getFormatName( PixelFormat p_format );
		/**
		 *\~english
		 *\brief		Function to retrieve pixel colour PixelComponents in float
		 *\param[in]	p_pixel		The pixel
		 *\param[in]	p_component	The PixelComponents
		 *\return		The PixelComponents value
		 *\~french
		 *\brief		Fonction de récupération d'une composante couleur d'un pixel, en flottant
		 *\param[in]	p_pixel		Le pixel
		 *\param[in]	p_component	La composante
		 *\return		La valeur de la composante
		 */
		template< PixelFormat PF >
		float getFloatComponent( Pixel< PF > const & p_pixel, PixelComponent p_component );
		/**
		 *\~english
		 *\brief		Function to define pixel colour PixelComponents in float
		 *\param[in]	p_pixel		The pixel
		 *\param[in]	p_component	The PixelComponents
		 *\param[in]	p_value		The PixelComponents value
		 *\~french
		 *\brief		Fonction de définition d'une composante couleur d'un pixel, en flottant
		 *\param[in]	p_pixel		Le pixel
		 *\param[in]	p_component	La composante
		 *\param[in]	p_value		La valeur de la composante
		 */
		template< PixelFormat PF >
		void setFloatComponent( Pixel< PF > & p_pixel, PixelComponent p_component, float p_value );
		/**
		 *\~english
		 *\brief		Function to retrieve pixel colour PixelComponents in byte
		 *\param[in]	p_pixel		The pixel
		 *\param[in]	p_component	The PixelComponents
		 *\return		The PixelComponents value
		 *\~french
		 *\brief		Fonction de récupération d'une composante couleur d'un pixel, en octet
		 *\param[in]	p_pixel		Le pixel
		 *\param[in]	p_component	La composante
		 *\return		La valeur de la composante
		 */
		template< PixelFormat PF >
		uint8_t getByteComponent( Pixel< PF > const & p_pixel, PixelComponent p_component );
		/**
		 *\~english
		 *\brief		Function to define pixel colour PixelComponents in byte
		 *\param[in]	p_pixel		The pixel
		 *\param[in]	p_component	The PixelComponents
		 *\param[in]	p_value		The PixelComponents value
		 *\~french
		 *\brief		Fonction de définition d'une composante couleur d'un pixel, en octet
		 *\param[in]	p_pixel		Le pixel
		 *\param[in]	p_component	La composante
		 *\param[in]	p_value		La valeur de la composante
		 */
		template< PixelFormat PF >
		void setByteComponent( Pixel< PF > & p_pixel, PixelComponent p_component, uint8_t p_value );
		/**
		 *\~english
		 *\brief		Function to retrieve pixel colour PixelComponents in uint16_t
		 *\param[in]	p_pixel		The pixel
		 *\param[in]	p_component	The PixelComponents
		 *\return		The PixelComponents value
		 *\~french
		 *\brief		Fonction de récupération d'une composante couleur d'un pixel, en uint16_t
		 *\param[in]	p_pixel		Le pixel
		 *\param[in]	p_component	La composante
		 *\return		La valeur de la composante
		 */
		template< PixelFormat PF >
		uint16_t getUInt16Component( Pixel< PF > const & p_pixel, PixelComponent p_component );
		/**
		 *\~english
		 *\brief		Function to define pixel colour PixelComponents in uint16_t
		 *\param[in]	p_pixel		The pixel
		 *\param[in]	p_component	The PixelComponents
		 *\param[in]	p_value		The PixelComponents value
		 *\~french
		 *\brief		Fonction de définition d'une composante couleur d'un pixel, en uint16_t
		 *\param[in]	p_pixel		Le pixel
		 *\param[in]	p_component	La composante
		 *\param[in]	p_value		La valeur de la composante
		 */
		template< PixelFormat PF >
		void setUInt16Component( Pixel< PF > & p_pixel, PixelComponent p_component, uint16_t p_value );
		/**
		 *\~english
		 *\brief		Function to retrieve pixel colour PixelComponents in uint32_t, with 24 relevant bits
		 *\param[in]	p_pixel		The pixel
		 *\param[in]	p_component	The PixelComponents
		 *\return		The PixelComponents value
		 *\~french
		 *\brief		Fonction de récupération d'une composante couleur d'un pixel, en uint32_t, avec 24 bits utiles
		 *\param[in]	p_pixel		Le pixel
		 *\param[in]	p_component	La composante
		 *\return		La valeur de la composante
		 */
		template< PixelFormat PF >
		uint32_t getUInt24Component( Pixel< PF > const & p_pixel, PixelComponent p_component );
		/**
		 *\~english
		 *\brief		Function to define pixel colour PixelComponents in uint32_t, with 24 relevant bits
		 *\param[in]	p_pixel		The pixel
		 *\param[in]	p_component	The PixelComponents
		 *\param[in]	p_value		The PixelComponents value
		 *\~french
		 *\brief		Fonction de définition d'une composante couleur d'un pixel, en uint32_t, avec 24 bits utiles
		 *\param[in]	p_pixel		Le pixel
		 *\param[in]	p_component	La composante
		 *\param[in]	p_value		La valeur de la composante
		 */
		template< PixelFormat PF >
		void setUInt24Component( Pixel< PF > & p_pixel, PixelComponent p_component, uint32_t p_value );
		/**
		 *\~english
		 *\brief		Function to retrieve pixel colour PixelComponents in uint32_t
		 *\param[in]	p_pixel		The pixel
		 *\param[in]	p_component	The PixelComponents
		 *\return		The PixelComponents value
		 *\~french
		 *\brief		Fonction de récupération d'une composante couleur d'un pixel, en uint32_t
		 *\param[in]	p_pixel		Le pixel
		 *\param[in]	p_component	La composante
		 *\return		La valeur de la composante
		 */
		template< PixelFormat PF >
		uint32_t getUInt32Component( Pixel< PF > const & p_pixel, PixelComponent p_component );
		/**
		 *\~english
		 *\brief		Function to define pixel colour PixelComponents in uint32_t
		 *\param[in]	p_pixel		The pixel
		 *\param[in]	p_component	The PixelComponents
		 *\param[in]	p_value		The PixelComponents value
		 *\~french
		 *\brief		Fonction de définition d'une composante couleur d'un pixel, en uint32_t
		 *\param[in]	p_pixel		Le pixel
		 *\param[in]	p_component	La composante
		 *\param[in]	p_value		La valeur de la composante
		 */
		template< PixelFormat PF >
		void setUInt32Component( Pixel< PF > & p_pixel, PixelComponent p_component, uint32_t p_value );
		/**
		 *\~english
		 *\brief		Function to retrieve pixel colour PixelComponents in float
		 *\param[in]	p_pixel		The pixel
		 *\return		The PixelComponents value
		 *\~french
		 *\brief		Fonction de récupération de la composante rouge d'un pixel, en flottant
		 *\param[in]	p_pixel		Le pixel
		 *\return		La valeur de la composante
		 */
		template< PixelFormat PF >
		float getFloatRed( Pixel< PF > const & p_pixel )
		{
			return getFloatComponent( p_pixel, PixelComponent::eRed );
		}
		/**
		 *\~english
		 *\brief		Function to define pixel colour PixelComponents in float
		 *\param[in]	p_pixel		The pixel
		 *\param[in]	p_value		The PixelComponents value
		 *\~french
		 *\brief		Fonction de définition de la composante rouge d'un pixel, en flottant
		 *\param[in]	p_pixel		Le pixel
		 *\param[in]	p_value		La valeur de la composante
		 */
		template< PixelFormat PF >
		void setFloatRed( Pixel< PF > const & p_pixel, float p_value )
		{
			return setFloatComponent( p_pixel, PixelComponent::eRed, p_value );
		}
		/**
		 *\~english
		 *\brief		Function to retrieve pixel red PixelComponents in byte
		 *\param[in]	p_pixel		The pixel
		 *\return		The PixelComponents value
		 *\~french
		 *\brief		Fonction de récupération de la composante rouge d'un pixel, en octet
		 *\param[in]	p_pixel		Le pixel
		 *\return		La valeur de la composante
		 */
		template< PixelFormat PF >
		uint8_t getByteRed( Pixel< PF > const & p_pixel )
		{
			return getByteComponent( p_pixel, PixelComponent::eRed );
		}
		/**
		 *\~english
		 *\brief		Function to define pixel red PixelComponents in byte
		 *\param[in]	p_pixel		The pixel
		 *\param[in]	p_value		The PixelComponents value
		 *\~french
		 *\brief		Fonction de définition de la composante rouge d'un pixel, en octet
		 *\param[in]	p_pixel		Le pixel
		 *\param[in]	p_value		La valeur de la composante
		 */
		template< PixelFormat PF >
		void setByteRed( Pixel< PF > & p_pixel, uint8_t p_value )
		{
			return setByteComponent( p_pixel, PixelComponent::eRed, p_value );
		}
		/**
		 *\~english
		 *\brief		Function to retrieve pixel green PixelComponents in float
		 *\param[in]	p_pixel		The pixel
		 *\return		The PixelComponents value
		 *\~french
		 *\brief		Fonction de récupération de la composante verte d'un pixel, en flottant
		 *\param[in]	p_pixel		Le pixel
		 *\return		La valeur de la composante
		 */
		template< PixelFormat PF >
		float getFloatGreen( Pixel< PF > const & p_pixel )
		{
			return getFloatComponent( p_pixel, PixelComponent::eGreen );
		}
		/**
		 *\~english
		 *\brief		Function to define pixel green PixelComponents in float
		 *\param[in]	p_pixel		The pixel
		 *\param[in]	p_value		The PixelComponents value
		 *\~french
		 *\brief		Fonction de définition de la composante verte d'un pixel, en flottant
		 *\param[in]	p_pixel		Le pixel
		 *\param[in]	p_value		La valeur de la composante
		 */
		template< PixelFormat PF >
		void setFloatGreen( Pixel< PF > & p_pixel, float p_value )
		{
			return setFloatComponent( p_pixel, PixelComponent::eGreen, p_value );
		}
		/**
		 *\~english
		 *\brief		Function to retrieve pixel green PixelComponents in byte
		 *\param[in]	p_pixel		The pixel
		 *\return		The PixelComponents value
		 *\~french
		 *\brief		Fonction de récupération de la composante verte d'un pixel, en octet
		 *\param[in]	p_pixel		Le pixel
		 *\return		La valeur de la composante
		 */
		template< PixelFormat PF >
		uint8_t getByteGreen( Pixel< PF > const & p_pixel )
		{
			return getByteComponent( p_pixel, PixelComponent::eGreen );
		}
		/**
		 *\~english
		 *\brief		Function to define pixel green PixelComponents in byte
		 *\param[in]	p_pixel		The pixel
		 *\param[in]	p_value		The PixelComponents value
		 *\~french
		 *\brief		Fonction de définition de la composante verte d'un pixel, en octet
		 *\param[in]	p_pixel		Le pixel
		 *\param[in]	p_value		La valeur de la composante
		 */
		template< PixelFormat PF >
		void setByteGreen( Pixel< PF > & p_pixel, uint8_t p_value )
		{
			return setByteComponent( p_pixel, PixelComponent::eGreen, p_value );
		}
		/**
		 *\~english
		 *\brief		Function to retrieve pixel blue PixelComponents in float
		 *\param[in]	p_pixel		The pixel
		 *\return		The PixelComponents value
		 *\~french
		 *\brief		Fonction de récupération de la composante bleue d'un pixel, en flottant
		 *\param[in]	p_pixel		Le pixel
		 *\return		La valeur de la composante
		 */
		template< PixelFormat PF >
		float getFloatBlue( Pixel< PF > const & p_pixel )
		{
			return getFloatComponent( p_pixel, PixelComponent::eBlue );
		}
		/**
		 *\~english
		 *\brief		Function to define pixel blue PixelComponents in float
		 *\param[in]	p_pixel		The pixel
		 *\param[in]	p_value		The PixelComponents value
		 *\~french
		 *\brief		Fonction de définition de la composante bleue d'un pixel, en flottant
		 *\param[in]	p_pixel		Le pixel
		 *\param[in]	p_value		La valeur de la composante
		 */
		template< PixelFormat PF >
		void setFloatBlue( Pixel< PF > & p_pixel, float p_value )
		{
			return setFloatComponent( p_pixel, PixelComponent::eBlue, p_value );
		}
		/**
		 *\~english
		 *\brief		Function to retrieve pixel blue PixelComponents in byte
		 *\param[in]	p_pixel		The pixel
		 *\return		The PixelComponents value
		 *\~french
		 *\brief		Fonction de récupération de la composante bleue d'un pixel, en octet
		 *\param[in]	p_pixel		Le pixel
		 *\return		La valeur de la composante
		 */
		template< PixelFormat PF >
		uint8_t getByteBlue( Pixel< PF > const & p_pixel )
		{
			return getByteComponent( p_pixel, PixelComponent::eBlue );
		}
		/**
		 *\~english
		 *\brief		Function to define pixel blue PixelComponents in byte
		 *\param[in]	p_pixel		The pixel
		 *\param[in]	p_value		The PixelComponents value
		 *\~french
		 *\brief		Fonction de définition de la composante bleue d'un pixel, en octet
		 *\param[in]	p_pixel		Le pixel
		 *\param[in]	p_value		La valeur de la composante
		 */
		template< PixelFormat PF >
		void setByteBlue( Pixel< PF > & p_pixel, uint8_t p_value )
		{
			return setByteComponent( p_pixel, PixelComponent::eBlue, p_value );
		}
		/**
		 *\~english
		 *\brief		Function to retrieve pixel alpha PixelComponents in float
		 *\param[in]	p_pixel		The pixel
		 *\return		The PixelComponents value
		 *\~french
		 *\brief		Fonction de récupération de la composante alpha d'un pixel, en flottant
		 *\param[in]	p_pixel		Le pixel
		 *\return		La valeur de la composante
		 */
		template< PixelFormat PF >
		float getFloatAlpha( Pixel< PF > const & p_pixel )
		{
			return getFloatComponent( p_pixel, PixelComponent::eAlpha );
		}
		/**
		 *\~english
		 *\brief		Function to define pixel alpha PixelComponents in float
		 *\param[in]	p_pixel		The pixel
		 *\param[in]	p_value		The PixelComponents value
		 *\~french
		 *\brief		Fonction de définition de la composante alpha d'un pixel, en flottant
		 *\param[in]	p_pixel		Le pixel
		 *\param[in]	p_value		La valeur de la composante
		 */
		template< PixelFormat PF >
		void setFloatAlpha( Pixel< PF > & p_pixel, float p_value )
		{
			return setFloatComponent( p_pixel, PixelComponent::eAlpha, p_value );
		}
		/**
		 *\~english
		 *\brief		Function to retrieve pixel alpha PixelComponents in byte
		 *\param[in]	p_pixel		The pixel
		 *\return		The PixelComponents value
		 *\~french
		 *\brief		Fonction de récupération de la composante alpha d'un pixel, en octet
		 *\param[in]	p_pixel		Le pixel
		 *\return		La valeur de la composante
		 */
		template< PixelFormat PF >
		uint8_t getByteAlpha( Pixel< PF > const & p_pixel )
		{
			return getByteComponent( p_pixel, PixelComponent::eAlpha );
		}
		/**
		 *\~english
		 *\brief		Function to define pixel alpha PixelComponents in byte
		 *\param[in]	p_pixel		The pixel
		 *\param[in]	p_value		The PixelComponents value
		 *\~french
		 *\brief		Fonction de définition de la composante alpha d'un pixel, en octet
		 *\param[in]	p_pixel		Le pixel
		 *\param[in]	p_value		La valeur de la composante
		 */
		template< PixelFormat PF >
		void setByteAlpha( Pixel< PF > & p_pixel, uint8_t p_value )
		{
			return setByteComponent( p_pixel, PixelComponent::eAlpha, p_value );
		}
		/**
		 *\~english
		 *\brief		Function to retrieve pixel luminance PixelComponents in float
		 *\param[in]	p_pixel		The pixel
		 *\return		The PixelComponents value
		 *\~french
		 *\brief		Fonction de récupération de la composante luminance d'un pixel, en flottant
		 *\param[in]	p_pixel		Le pixel
		 *\return		La valeur de la composante
		 */
		template< PixelFormat PF >
		float getFloatLuminance( Pixel< PF > const & p_pixel )
		{
			return getFloatComponent( p_pixel, PixelComponent::eLuminance );
		}
		/**
		 *\~english
		 *\brief		Function to define pixel luminance PixelComponents in float
		 *\param[in]	p_pixel		The pixel
		 *\param[in]	p_value		The PixelComponents value
		 *\~french
		 *\brief		Fonction de définition de la composante luminance d'un pixel, en flottant
		 *\param[in]	p_pixel		Le pixel
		 *\param[in]	p_value		La valeur de la composante
		 */
		template< PixelFormat PF >
		void setFloatLuminance( Pixel< PF > & p_pixel, float p_value )
		{
			return setFloatComponent( p_pixel, PixelComponent::eLuminance, p_value );
		}
		/**
		 *\~english
		 *\brief		Function to retrieve pixel luminance PixelComponents in byte
		 *\param[in]	p_pixel		The pixel
		 *\return		The PixelComponents value
		 *\~french
		 *\brief		Fonction de récupération de la composante luminance d'un pixel, en octet
		 *\param[in]	p_pixel		Le pixel
		 *\return		La valeur de la composante
		 */
		template< PixelFormat PF >
		uint8_t getByteLuminance( Pixel< PF > const & p_pixel )
		{
			return getByteComponent( p_pixel, PixelComponent::eLuminance );
		}
		/**
		 *\~english
		 *\brief		Function to define pixel luminance PixelComponents in byte
		 *\param[in]	p_pixel		The pixel
		 *\param[in]	p_value		The PixelComponents value
		 *\~french
		 *\brief		Fonction de définition de la composante luminance d'un pixel, en octet
		 *\param[in]	p_pixel		Le pixel
		 *\param[in]	p_value		La valeur de la composante
		 */
		template< PixelFormat PF >
		void setByteLuminance( Pixel< PF > & p_pixel, uint8_t p_value )
		{
			return setByteComponent( p_pixel, PixelComponent::eLuminance, p_value );
		}
		/**
		 *\~english
		 *\brief		Function to retrieve pixel depth PixelComponents in float
		 *\param[in]	p_pixel		The pixel
		 *\return		The PixelComponents value
		 *\~french
		 *\brief		Fonction de récupération de la composante profondeur d'un pixel, en flottant
		 *\param[in]	p_pixel		Le pixel
		 *\return		La valeur de la composante
		 */
		template< PixelFormat PF >
		float getFloatDepth( Pixel< PF > const & p_pixel )
		{
			return getFloatComponent( p_pixel, PixelComponent::eDepth );
		}
		/**
		 *\~english
		 *\brief		Function to define pixel depth PixelComponents in float
		 *\param[in]	p_pixel		The pixel
		 *\param[in]	p_value		The PixelComponents value
		 *\~french
		 *\brief		Fonction de définition de la composante profondeur d'un pixel, en flottant
		 *\param[in]	p_pixel		Le pixel
		 *\param[in]	p_value		La valeur de la composante
		 */
		template< PixelFormat PF >
		void setFloatDepth( Pixel< PF > & p_pixel, float p_value )
		{
			return setFloatComponent( p_pixel, PixelComponent::eDepth, p_value );
		}
		/**
		 *\~english
		 *\brief		Function to retrieve pixel depth PixelComponents in byte
		 *\param[in]	p_pixel		The pixel
		 *\return		The PixelComponents value
		 *\~french
		 *\brief		Fonction de récupération de la composante profondeur d'un pixel, en octet
		 *\param[in]	p_pixel		Le pixel
		 *\return		La valeur de la composante
		 */
		template< PixelFormat PF >
		uint8_t getByteDepth( Pixel< PF > const & p_pixel )
		{
			return getByteComponent( p_pixel, PixelComponent::eDepth );
		}
		/**
		 *\~english
		 *\brief		Function to define pixel depth PixelComponents in byte
		 *\param[in]	p_pixel		The pixel
		 *\param[in]	p_value		The PixelComponents value
		 *\~french
		 *\brief		Fonction de définition de la composante profondeur d'un pixel, en octet
		 *\param[in]	p_pixel		Le pixel
		 *\param[in]	p_value		La valeur de la composante
		 */
		template< PixelFormat PF >
		void setByteDepth( Pixel< PF > & p_pixel, uint8_t p_value )
		{
			return setByteComponent( p_pixel, PixelComponent::eDepth, p_value );
		}
		/**
		 *\~english
		 *\brief		Function to retrieve pixel depth PixelComponents in uint16_t
		 *\param[in]	p_pixel		The pixel
		 *\return		The PixelComponents value
		 *\~french
		 *\brief		Fonction de récupération de la composante profondeur d'un pixel, en uint16_t
		 *\param[in]	p_pixel		Le pixel
		 *\return		La valeur de la composante
		 */
		template< PixelFormat PF >
		uint16_t getUInt16Depth( Pixel< PF > const & p_pixel )
		{
			return getUInt16Component( p_pixel, PixelComponent::eDepth );
		}
		/**
		 *\~english
		 *\brief		Function to define pixel depth PixelComponents in uint16_t
		 *\param[in]	p_pixel		The pixel
		 *\param[in]	p_value		The PixelComponents value
		 *\~french
		 *\brief		Fonction de définition de la composante profondeur d'un pixel, en uint16_t
		 *\param[in]	p_pixel		Le pixel
		 *\param[in]	p_value		La valeur de la composante
		 */
		template< PixelFormat PF >
		void setUInt16Depth( Pixel< PF > & p_pixel, uint16_t p_value )
		{
			return setUInt16Component( p_pixel, PixelComponent::eDepth, p_value );
		}
		/**
		 *\~english
		 *\brief		Function to retrieve pixel depth PixelComponents in uint32_t, with 24 relevant bits
		 *\param[in]	p_pixel		The pixel
		 *\return		The PixelComponents value
		 *\~french
		 *\brief		Fonction de récupération de la composante profondeur d'un pixel, en uint32_t, avec 24 bits utiles
		 *\param[in]	p_pixel		Le pixel
		 *\return		La valeur de la composante
		 */
		template< PixelFormat PF >
		uint32_t getUInt24Depth( Pixel< PF > const & p_pixel )
		{
			return getUInt24Component( p_pixel, PixelComponent::eDepth );
		}
		/**
		 *\~english
		 *\brief		Function to define pixel depth PixelComponents in uint32_t, with 24 relevant bits
		 *\param[in]	p_pixel		The pixel
		 *\param[in]	p_value		The PixelComponents value
		 *\~french
		 *\brief		Fonction de définition de la composante profondeur d'un pixel, en uint32_t, avec 24 bits utiles
		 *\param[in]	p_pixel		Le pixel
		 *\param[in]	p_value		La valeur de la composante
		 */
		template< PixelFormat PF >
		void setUInt24Depth( Pixel< PF > & p_pixel, uint32_t p_value )
		{
			return setUInt24Component( p_pixel, PixelComponent::eDepth, p_value );
		}
		/**
		 *\~english
		 *\brief		Function to retrieve pixel depth PixelComponents in uint32_t
		 *\param[in]	p_pixel		The pixel
		 *\return		The PixelComponents value
		 *\~french
		 *\brief		Fonction de récupération de la composante profondeur d'un pixel, en uint32_t
		 *\param[in]	p_pixel		Le pixel
		 *\return		La valeur de la composante
		 */
		template< PixelFormat PF >
		uint32_t getUInt32Depth( Pixel< PF > const & p_pixel )
		{
			return getUInt32Component( p_pixel, PixelComponent::eDepth );
		}
		/**
		 *\~english
		 *\brief		Function to define pixel depth PixelComponents in uint32_t
		 *\param[in]	p_pixel		The pixel
		 *\param[in]	p_value		The PixelComponents value
		 *\~french
		 *\brief		Fonction de définition de la composante profondeur d'un pixel, en uint32_t
		 *\param[in]	p_pixel		Le pixel
		 *\param[in]	p_value		La valeur de la composante
		 */
		template< PixelFormat PF >
		void setUInt32Depth( Pixel< PF > & p_pixel, uint32_t p_value )
		{
			return setUInt32Component( p_pixel, PixelComponent::eDepth, p_value );
		}
		/**
		 *\~english
		 *\brief		Function to retrieve pixel depth stencil in float
		 *\param[in]	p_pixel		The pixel
		 *\return		The PixelComponents value
		 *\~french
		 *\brief		Fonction de récupération de la composante stencil d'un pixel, en flottant
		 *\param[in]	p_pixel		Le pixel
		 *\return		La valeur de la composante
		 */
		template< PixelFormat PF >
		float getFloatStencil( Pixel< PF > const & p_pixel )
		{
			return getFloatComponent( p_pixel, PixelComponent::eStencil );
		}
		/**
		 *\~english
		 *\brief		Function to define pixel depth stencil in float
		 *\param[in]	p_pixel		The pixel
		 *\param[in]	p_value		The PixelComponents value
		 *\~french
		 *\brief		Fonction de définition de la composante stencil d'un pixel, en flottant
		 *\param[in]	p_pixel		Le pixel
		 *\param[in]	p_value		La valeur de la composante
		 */
		template< PixelFormat PF >
		void setFloatStencil( Pixel< PF > & p_pixel, float p_value )
		{
			return setFloatComponent( p_pixel, PixelComponent::eStencil, p_value );
		}
		/**
		 *\~english
		 *\brief		Function to retrieve pixel depth stencil in byte
		 *\param[in]	p_pixel		The pixel
		 *\return		The PixelComponents value
		 *\~french
		 *\brief		Fonction de récupération de la composante stencil d'un pixel, en octet
		 *\param[in]	p_pixel		Le pixel
		 *\return		La valeur de la composante
		 */
		template< PixelFormat PF >
		uint8_t getByteStencil( Pixel< PF > const & p_pixel )
		{
			return getByteComponent( p_pixel, PixelComponent::eStencil );
		}
		/**
		 *\~english
		 *\brief		Function to define pixel depth stencil in byte
		 *\param[in]	p_pixel		The pixel
		 *\param[in]	p_value		The PixelComponents value
		 *\~french
		 *\brief		Fonction de définition de la composante stencil d'un pixel, en octet
		 *\param[in]	p_pixel		Le pixel
		 *\param[in]	p_value		La valeur de la composante
		 */
		template< PixelFormat PF >
		void setByteStencil( Pixel< PF > & p_pixel, uint8_t p_value )
		{
			return setByteComponent( p_pixel, PixelComponent::eStencil, p_value );
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
		CU_API void convertPixel( PixelFormat p_eSrcFmt, uint8_t const *& p_pSrc, PixelFormat p_eDestFmt, uint8_t *& p_pDest );
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
		CU_API void convertBuffer( PixelFormat p_eSrcFormat, uint8_t const * p_pSrcBuffer, uint32_t p_uiSrcSize, PixelFormat p_eDstFormat, uint8_t * p_pDstBuffer, uint32_t p_uiDstSize );
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
		CU_API PxBufferBaseSPtr extractAlpha( PxBufferBaseSPtr & p_pSrc );
		/**
		 *\see			PixelFormat
		 *\~english
		 *\brief		Checks the alpha PixelComponents support for given pixel format
		 *\return		\p false if format is depth, stencil or one without alpha
		 *\~french
		 *\brief		Vérifie si le format donné possède une composante alpha
		 *\return		\p false si le format est depth, stencil ou un format sans alpha
		 */
		CU_API bool hasAlpha( PixelFormat p_ePf );
		/**
		 *\see			PixelFormat
		 *\~english
		 *\brief		Checks if the given pixel format is a compressed one
		 *\return		The value
		 *\~french
		 *\brief		Vérifie si le format donné est un format compressé
		 *\return		La valeur
		 */
		CU_API bool isCompressed( PixelFormat p_ePf );
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
		CU_API PixelFormat getPFWithoutAlpha( PixelFormat p_ePixelFmt );
		/**
		 *\~english
		 *\brief			Reduces a buffer to it's alpha channel, stored in a L8 format buffer
		 *\param[in,out]	p_pSrc	The buffer to reduce
		 *\~english
		 *\brief			Réduit un tampon à son canal alpha stocké dans un tampon au format L8
		 *\param[in,out]	p_pSrc	Le tampon à réduire
		 */
		CU_API void reduceToAlpha( PxBufferBaseSPtr & p_pSrc );
	}
}

#include "PixelFormat.inl"

#endif
