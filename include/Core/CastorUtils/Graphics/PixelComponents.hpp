/*
See LICENSE file in root folder
*/
#ifndef ___CU_PixelComponents___
#define ___CU_PixelComponents___

#include "CastorUtils/Graphics/GraphicsModule.hpp"

namespace castor::PF
{
	/**
	 *\~english
	 *\brief		Function to retrieve pixel colour PixelComponents in float
	 *\param[in]	pixel		The pixel
	 *\param[in]	component	The PixelComponents
	 *\return		The PixelComponents value
	 *\~french
	 *\brief		Fonction de récupération d'une composante couleur d'un pixel, en flottant
	 *\param[in]	pixel		Le pixel
	 *\param[in]	component	La composante
	 *\return		La valeur de la composante
	 */
	template< PixelFormat PF >
	float getFloatComponent( Pixel< PF > const & pixel, PixelComponent component );
	/**
	 *\~english
	 *\brief		Function to define pixel colour PixelComponents in float
	 *\param[in]	pixel		The pixel
	 *\param[in]	component	The PixelComponents
	 *\param[in]	value		The PixelComponents value
	 *\~french
	 *\brief		Fonction de définition d'une composante couleur d'un pixel, en flottant
	 *\param[in]	pixel		Le pixel
	 *\param[in]	component	La composante
	 *\param[in]	value		La valeur de la composante
	 */
	template< PixelFormat PF >
	void setFloatComponent( Pixel< PF > & pixel, PixelComponent component, float value );
	/**
	 *\~english
	 *\brief		Function to retrieve pixel colour PixelComponents in byte
	 *\param[in]	pixel		The pixel
	 *\param[in]	component	The PixelComponents
	 *\return		The PixelComponents value
	 *\~french
	 *\brief		Fonction de récupération d'une composante couleur d'un pixel, en octet
	 *\param[in]	pixel		Le pixel
	 *\param[in]	component	La composante
	 *\return		La valeur de la composante
	 */
	template< PixelFormat PF >
	uint8_t getByteComponent( Pixel< PF > const & pixel, PixelComponent component );
	/**
	 *\~english
	 *\brief		Function to define pixel colour PixelComponents in byte
	 *\param[in]	pixel		The pixel
	 *\param[in]	component	The PixelComponents
	 *\param[in]	value		The PixelComponents value
	 *\~french
	 *\brief		Fonction de définition d'une composante couleur d'un pixel, en octet
	 *\param[in]	pixel		Le pixel
	 *\param[in]	component	La composante
	 *\param[in]	value		La valeur de la composante
	 */
	template< PixelFormat PF >
	void setByteComponent( Pixel< PF > & pixel, PixelComponent component, uint8_t value );
	/**
	 *\~english
	 *\brief		Function to retrieve pixel colour PixelComponents in uint16_t
	 *\param[in]	pixel		The pixel
	 *\param[in]	component	The PixelComponents
	 *\return		The PixelComponents value
	 *\~french
	 *\brief		Fonction de récupération d'une composante couleur d'un pixel, en uint16_t
	 *\param[in]	pixel		Le pixel
	 *\param[in]	component	La composante
	 *\return		La valeur de la composante
	 */
	template< PixelFormat PF >
	uint16_t getUInt16Component( Pixel< PF > const & pixel, PixelComponent component );
	/**
	 *\~english
	 *\brief		Function to define pixel colour PixelComponents in uint16_t
	 *\param[in]	pixel		The pixel
	 *\param[in]	component	The PixelComponents
	 *\param[in]	value		The PixelComponents value
	 *\~french
	 *\brief		Fonction de définition d'une composante couleur d'un pixel, en uint16_t
	 *\param[in]	pixel		Le pixel
	 *\param[in]	component	La composante
	 *\param[in]	value		La valeur de la composante
	 */
	template< PixelFormat PF >
	void setUInt16Component( Pixel< PF > & pixel, PixelComponent component, uint16_t value );
	/**
	 *\~english
	 *\brief		Function to retrieve pixel colour PixelComponents in uint32_t, with 24 relevant bits
	 *\param[in]	pixel		The pixel
	 *\param[in]	component	The PixelComponents
	 *\return		The PixelComponents value
	 *\~french
	 *\brief		Fonction de récupération d'une composante couleur d'un pixel, en uint32_t, avec 24 bits utiles
	 *\param[in]	pixel		Le pixel
	 *\param[in]	component	La composante
	 *\return		La valeur de la composante
	 */
	template< PixelFormat PF >
	uint32_t getUInt24Component( Pixel< PF > const & pixel, PixelComponent component );
	/**
	 *\~english
	 *\brief		Function to define pixel colour PixelComponents in uint32_t, with 24 relevant bits
	 *\param[in]	pixel		The pixel
	 *\param[in]	component	The PixelComponents
	 *\param[in]	value		The PixelComponents value
	 *\~french
	 *\brief		Fonction de définition d'une composante couleur d'un pixel, en uint32_t, avec 24 bits utiles
	 *\param[in]	pixel		Le pixel
	 *\param[in]	component	La composante
	 *\param[in]	value		La valeur de la composante
	 */
	template< PixelFormat PF >
	void setUInt24Component( Pixel< PF > & pixel, PixelComponent component, uint32_t value );
	/**
	 *\~english
	 *\brief		Function to retrieve pixel colour PixelComponents in uint32_t
	 *\param[in]	pixel		The pixel
	 *\param[in]	component	The PixelComponents
	 *\return		The PixelComponents value
	 *\~french
	 *\brief		Fonction de récupération d'une composante couleur d'un pixel, en uint32_t
	 *\param[in]	pixel		Le pixel
	 *\param[in]	component	La composante
	 *\return		La valeur de la composante
	 */
	template< PixelFormat PF >
	uint32_t getUInt32Component( Pixel< PF > const & pixel, PixelComponent component );
	/**
	 *\~english
	 *\brief		Function to define pixel colour PixelComponents in uint32_t
	 *\param[in]	pixel		The pixel
	 *\param[in]	component	The PixelComponents
	 *\param[in]	value		The PixelComponents value
	 *\~french
	 *\brief		Fonction de définition d'une composante couleur d'un pixel, en uint32_t
	 *\param[in]	pixel		Le pixel
	 *\param[in]	component	La composante
	 *\param[in]	value		La valeur de la composante
	 */
	template< PixelFormat PF >
	void setUInt32Component( Pixel< PF > & pixel, PixelComponent component, uint32_t value );
	/**
	 *\~english
	 *\brief		Function to retrieve pixel colour PixelComponents in float
	 *\param[in]	pixel		The pixel
	 *\return		The PixelComponents value
	 *\~french
	 *\brief		Fonction de récupération de la composante rouge d'un pixel, en flottant
	 *\param[in]	pixel		Le pixel
	 *\return		La valeur de la composante
	 */
	template< PixelFormat PF >
	float getFloatRed( Pixel< PF > const & pixel )
	{
		return getFloatComponent( pixel, PixelComponent::eRed );
	}
	/**
	 *\~english
	 *\brief		Function to define pixel colour PixelComponents in float
	 *\param[in]	pixel		The pixel
	 *\param[in]	value		The PixelComponents value
	 *\~french
	 *\brief		Fonction de définition de la composante rouge d'un pixel, en flottant
	 *\param[in]	pixel		Le pixel
	 *\param[in]	value		La valeur de la composante
	 */
	template< PixelFormat PF >
	void setFloatRed( Pixel< PF > const & pixel, float value )
	{
		return setFloatComponent( pixel, PixelComponent::eRed, value );
	}
	/**
	 *\~english
	 *\brief		Function to retrieve pixel red PixelComponents in byte
	 *\param[in]	pixel		The pixel
	 *\return		The PixelComponents value
	 *\~french
	 *\brief		Fonction de récupération de la composante rouge d'un pixel, en octet
	 *\param[in]	pixel		Le pixel
	 *\return		La valeur de la composante
	 */
	template< PixelFormat PF >
	uint8_t getByteRed( Pixel< PF > const & pixel )
	{
		return getByteComponent( pixel, PixelComponent::eRed );
	}
	/**
	 *\~english
	 *\brief		Function to define pixel red PixelComponents in byte
	 *\param[in]	pixel		The pixel
	 *\param[in]	value		The PixelComponents value
	 *\~french
	 *\brief		Fonction de définition de la composante rouge d'un pixel, en octet
	 *\param[in]	pixel		Le pixel
	 *\param[in]	value		La valeur de la composante
	 */
	template< PixelFormat PF >
	void setByteRed( Pixel< PF > & pixel, uint8_t value )
	{
		return setByteComponent( pixel, PixelComponent::eRed, value );
	}
	/**
	 *\~english
	 *\brief		Function to retrieve pixel green PixelComponents in float
	 *\param[in]	pixel		The pixel
	 *\return		The PixelComponents value
	 *\~french
	 *\brief		Fonction de récupération de la composante verte d'un pixel, en flottant
	 *\param[in]	pixel		Le pixel
	 *\return		La valeur de la composante
	 */
	template< PixelFormat PF >
	float getFloatGreen( Pixel< PF > const & pixel )
	{
		return getFloatComponent( pixel, PixelComponent::eGreen );
	}
	/**
	 *\~english
	 *\brief		Function to define pixel green PixelComponents in float
	 *\param[in]	pixel		The pixel
	 *\param[in]	value		The PixelComponents value
	 *\~french
	 *\brief		Fonction de définition de la composante verte d'un pixel, en flottant
	 *\param[in]	pixel		Le pixel
	 *\param[in]	value		La valeur de la composante
	 */
	template< PixelFormat PF >
	void setFloatGreen( Pixel< PF > & pixel, float value )
	{
		return setFloatComponent( pixel, PixelComponent::eGreen, value );
	}
	/**
	 *\~english
	 *\brief		Function to retrieve pixel green PixelComponents in byte
	 *\param[in]	pixel		The pixel
	 *\return		The PixelComponents value
	 *\~french
	 *\brief		Fonction de récupération de la composante verte d'un pixel, en octet
	 *\param[in]	pixel		Le pixel
	 *\return		La valeur de la composante
	 */
	template< PixelFormat PF >
	uint8_t getByteGreen( Pixel< PF > const & pixel )
	{
		return getByteComponent( pixel, PixelComponent::eGreen );
	}
	/**
	 *\~english
	 *\brief		Function to define pixel green PixelComponents in byte
	 *\param[in]	pixel		The pixel
	 *\param[in]	value		The PixelComponents value
	 *\~french
	 *\brief		Fonction de définition de la composante verte d'un pixel, en octet
	 *\param[in]	pixel		Le pixel
	 *\param[in]	value		La valeur de la composante
	 */
	template< PixelFormat PF >
	void setByteGreen( Pixel< PF > & pixel, uint8_t value )
	{
		return setByteComponent( pixel, PixelComponent::eGreen, value );
	}
	/**
	 *\~english
	 *\brief		Function to retrieve pixel blue PixelComponents in float
	 *\param[in]	pixel		The pixel
	 *\return		The PixelComponents value
	 *\~french
	 *\brief		Fonction de récupération de la composante bleue d'un pixel, en flottant
	 *\param[in]	pixel		Le pixel
	 *\return		La valeur de la composante
	 */
	template< PixelFormat PF >
	float getFloatBlue( Pixel< PF > const & pixel )
	{
		return getFloatComponent( pixel, PixelComponent::eBlue );
	}
	/**
	 *\~english
	 *\brief		Function to define pixel blue PixelComponents in float
	 *\param[in]	pixel		The pixel
	 *\param[in]	value		The PixelComponents value
	 *\~french
	 *\brief		Fonction de définition de la composante bleue d'un pixel, en flottant
	 *\param[in]	pixel		Le pixel
	 *\param[in]	value		La valeur de la composante
	 */
	template< PixelFormat PF >
	void setFloatBlue( Pixel< PF > & pixel, float value )
	{
		return setFloatComponent( pixel, PixelComponent::eBlue, value );
	}
	/**
	 *\~english
	 *\brief		Function to retrieve pixel blue PixelComponents in byte
	 *\param[in]	pixel		The pixel
	 *\return		The PixelComponents value
	 *\~french
	 *\brief		Fonction de récupération de la composante bleue d'un pixel, en octet
	 *\param[in]	pixel		Le pixel
	 *\return		La valeur de la composante
	 */
	template< PixelFormat PF >
	uint8_t getByteBlue( Pixel< PF > const & pixel )
	{
		return getByteComponent( pixel, PixelComponent::eBlue );
	}
	/**
	 *\~english
	 *\brief		Function to define pixel blue PixelComponents in byte
	 *\param[in]	pixel		The pixel
	 *\param[in]	value		The PixelComponents value
	 *\~french
	 *\brief		Fonction de définition de la composante bleue d'un pixel, en octet
	 *\param[in]	pixel		Le pixel
	 *\param[in]	value		La valeur de la composante
	 */
	template< PixelFormat PF >
	void setByteBlue( Pixel< PF > & pixel, uint8_t value )
	{
		return setByteComponent( pixel, PixelComponent::eBlue, value );
	}
	/**
	 *\~english
	 *\brief		Function to retrieve pixel alpha PixelComponents in float
	 *\param[in]	pixel		The pixel
	 *\return		The PixelComponents value
	 *\~french
	 *\brief		Fonction de récupération de la composante alpha d'un pixel, en flottant
	 *\param[in]	pixel		Le pixel
	 *\return		La valeur de la composante
	 */
	template< PixelFormat PF >
	float getFloatAlpha( Pixel< PF > const & pixel )
	{
		return getFloatComponent( pixel, PixelComponent::eAlpha );
	}
	/**
	 *\~english
	 *\brief		Function to define pixel alpha PixelComponents in float
	 *\param[in]	pixel		The pixel
	 *\param[in]	value		The PixelComponents value
	 *\~french
	 *\brief		Fonction de définition de la composante alpha d'un pixel, en flottant
	 *\param[in]	pixel		Le pixel
	 *\param[in]	value		La valeur de la composante
	 */
	template< PixelFormat PF >
	void setFloatAlpha( Pixel< PF > & pixel, float value )
	{
		return setFloatComponent( pixel, PixelComponent::eAlpha, value );
	}
	/**
	 *\~english
	 *\brief		Function to retrieve pixel alpha PixelComponents in byte
	 *\param[in]	pixel		The pixel
	 *\return		The PixelComponents value
	 *\~french
	 *\brief		Fonction de récupération de la composante alpha d'un pixel, en octet
	 *\param[in]	pixel		Le pixel
	 *\return		La valeur de la composante
	 */
	template< PixelFormat PF >
	uint8_t getByteAlpha( Pixel< PF > const & pixel )
	{
		return getByteComponent( pixel, PixelComponent::eAlpha );
	}
	/**
	 *\~english
	 *\brief		Function to define pixel alpha PixelComponents in byte
	 *\param[in]	pixel		The pixel
	 *\param[in]	value		The PixelComponents value
	 *\~french
	 *\brief		Fonction de définition de la composante alpha d'un pixel, en octet
	 *\param[in]	pixel		Le pixel
	 *\param[in]	value		La valeur de la composante
	 */
	template< PixelFormat PF >
	void setByteAlpha( Pixel< PF > & pixel, uint8_t value )
	{
		return setByteComponent( pixel, PixelComponent::eAlpha, value );
	}
	/**
	 *\~english
	 *\brief		Function to retrieve pixel luminance PixelComponents in float
	 *\param[in]	pixel		The pixel
	 *\return		The PixelComponents value
	 *\~french
	 *\brief		Fonction de récupération de la composante luminance d'un pixel, en flottant
	 *\param[in]	pixel		Le pixel
	 *\return		La valeur de la composante
	 */
	template< PixelFormat PF >
	float getFloatLuminance( Pixel< PF > const & pixel )
	{
		return getFloatComponent( pixel, PixelComponent::eLuminance );
	}
	/**
	 *\~english
	 *\brief		Function to define pixel luminance PixelComponents in float
	 *\param[in]	pixel		The pixel
	 *\param[in]	value		The PixelComponents value
	 *\~french
	 *\brief		Fonction de définition de la composante luminance d'un pixel, en flottant
	 *\param[in]	pixel		Le pixel
	 *\param[in]	value		La valeur de la composante
	 */
	template< PixelFormat PF >
	void setFloatLuminance( Pixel< PF > & pixel, float value )
	{
		return setFloatComponent( pixel, PixelComponent::eLuminance, value );
	}
	/**
	 *\~english
	 *\brief		Function to retrieve pixel luminance PixelComponents in byte
	 *\param[in]	pixel		The pixel
	 *\return		The PixelComponents value
	 *\~french
	 *\brief		Fonction de récupération de la composante luminance d'un pixel, en octet
	 *\param[in]	pixel		Le pixel
	 *\return		La valeur de la composante
	 */
	template< PixelFormat PF >
	uint8_t getByteLuminance( Pixel< PF > const & pixel )
	{
		return getByteComponent( pixel, PixelComponent::eLuminance );
	}
	/**
	 *\~english
	 *\brief		Function to define pixel luminance PixelComponents in byte
	 *\param[in]	pixel		The pixel
	 *\param[in]	value		The PixelComponents value
	 *\~french
	 *\brief		Fonction de définition de la composante luminance d'un pixel, en octet
	 *\param[in]	pixel		Le pixel
	 *\param[in]	value		La valeur de la composante
	 */
	template< PixelFormat PF >
	void setByteLuminance( Pixel< PF > & pixel, uint8_t value )
	{
		return setByteComponent( pixel, PixelComponent::eLuminance, value );
	}
	/**
	 *\~english
	 *\brief		Function to retrieve pixel depth PixelComponents in float
	 *\param[in]	pixel		The pixel
	 *\return		The PixelComponents value
	 *\~french
	 *\brief		Fonction de récupération de la composante profondeur d'un pixel, en flottant
	 *\param[in]	pixel		Le pixel
	 *\return		La valeur de la composante
	 */
	template< PixelFormat PF >
	float getFloatDepth( Pixel< PF > const & pixel )
	{
		return getFloatComponent( pixel, PixelComponent::eDepth );
	}
	/**
	*\~english
	*\brief		Function to define pixel depth PixelComponents in float
	*\param[in]	pixel		The pixel
	*\param[in]	value		The PixelComponents value
	*\~french
	*\brief		Fonction de définition de la composante profondeur d'un pixel, en flottant
	*\param[in]	pixel		Le pixel
	*\param[in]	value		La valeur de la composante
	*/
	template< PixelFormat PF >
	void setFloatDepth( Pixel< PF > & pixel, float value )
	{
		return setFloatComponent( pixel, PixelComponent::eDepth, value );
	}
	/**
	 *\~english
	 *\brief		Function to retrieve pixel depth PixelComponents in byte
	 *\param[in]	pixel		The pixel
	 *\return		The PixelComponents value
	 *\~french
	 *\brief		Fonction de récupération de la composante profondeur d'un pixel, en octet
	 *\param[in]	pixel		Le pixel
	 *\return		La valeur de la composante
	 */
	template< PixelFormat PF >
	uint8_t getByteDepth( Pixel< PF > const & pixel )
	{
		return getByteComponent( pixel, PixelComponent::eDepth );
	}
	/**
	 *\~english
	 *\brief		Function to define pixel depth PixelComponents in byte
	 *\param[in]	pixel		The pixel
	 *\param[in]	value		The PixelComponents value
	 *\~french
	 *\brief		Fonction de définition de la composante profondeur d'un pixel, en octet
	 *\param[in]	pixel		Le pixel
	 *\param[in]	value		La valeur de la composante
	 */
	template< PixelFormat PF >
	void setByteDepth( Pixel< PF > & pixel, uint8_t value )
	{
		return setByteComponent( pixel, PixelComponent::eDepth, value );
	}
	/**
	 *\~english
	 *\brief		Function to retrieve pixel depth PixelComponents in uint16_t
	 *\param[in]	pixel		The pixel
	 *\return		The PixelComponents value
	 *\~french
	 *\brief		Fonction de récupération de la composante profondeur d'un pixel, en uint16_t
	 *\param[in]	pixel		Le pixel
	 *\return		La valeur de la composante
	 */
	template< PixelFormat PF >
	uint16_t getUInt16Depth( Pixel< PF > const & pixel )
	{
		return getUInt16Component( pixel, PixelComponent::eDepth );
	}
	/**
	 *\~english
	 *\brief		Function to define pixel depth PixelComponents in uint16_t
	 *\param[in]	pixel		The pixel
	 *\param[in]	value		The PixelComponents value
	 *\~french
	 *\brief		Fonction de définition de la composante profondeur d'un pixel, en uint16_t
	 *\param[in]	pixel		Le pixel
	 *\param[in]	value		La valeur de la composante
	 */
	template< PixelFormat PF >
	void setUInt16Depth( Pixel< PF > & pixel, uint16_t value )
	{
		return setUInt16Component( pixel, PixelComponent::eDepth, value );
	}
	/**
	 *\~english
	 *\brief		Function to retrieve pixel depth PixelComponents in uint32_t, with 24 relevant bits
	 *\param[in]	pixel		The pixel
	 *\return		The PixelComponents value
	 *\~french
	 *\brief		Fonction de récupération de la composante profondeur d'un pixel, en uint32_t, avec 24 bits utiles
	 *\param[in]	pixel		Le pixel
	 *\return		La valeur de la composante
	 */
	template< PixelFormat PF >
	uint32_t getUInt24Depth( Pixel< PF > const & pixel )
	{
		return getUInt24Component( pixel, PixelComponent::eDepth );
	}
	/**
	 *\~english
	 *\brief		Function to define pixel depth PixelComponents in uint32_t, with 24 relevant bits
	 *\param[in]	pixel		The pixel
	 *\param[in]	value		The PixelComponents value
	 *\~french
	 *\brief		Fonction de définition de la composante profondeur d'un pixel, en uint32_t, avec 24 bits utiles
	 *\param[in]	pixel		Le pixel
	 *\param[in]	value		La valeur de la composante
	 */
	template< PixelFormat PF >
	void setUInt24Depth( Pixel< PF > & pixel, uint32_t value )
	{
		return setUInt24Component( pixel, PixelComponent::eDepth, value );
	}
	/**
	 *\~english
	 *\brief		Function to retrieve pixel depth PixelComponents in uint32_t
	 *\param[in]	pixel		The pixel
	 *\return		The PixelComponents value
	 *\~french
	 *\brief		Fonction de récupération de la composante profondeur d'un pixel, en uint32_t
	 *\param[in]	pixel		Le pixel
	 *\return		La valeur de la composante
	 */
	template< PixelFormat PF >
	uint32_t getUInt32Depth( Pixel< PF > const & pixel )
	{
		return getUInt32Component( pixel, PixelComponent::eDepth );
	}
	/**
	 *\~english
	 *\brief		Function to define pixel depth PixelComponents in uint32_t
	 *\param[in]	pixel		The pixel
	 *\param[in]	value		The PixelComponents value
	 *\~french
	 *\brief		Fonction de définition de la composante profondeur d'un pixel, en uint32_t
	 *\param[in]	pixel		Le pixel
	 *\param[in]	value		La valeur de la composante
	 */
	template< PixelFormat PF >
	void setUInt32Depth( Pixel< PF > & pixel, uint32_t value )
	{
		return setUInt32Component( pixel, PixelComponent::eDepth, value );
	}
	/**
	 *\~english
	 *\brief		Function to retrieve pixel depth stencil in float
	 *\param[in]	pixel		The pixel
	 *\return		The PixelComponents value
	 *\~french
	 *\brief		Fonction de récupération de la composante stencil d'un pixel, en flottant
	 *\param[in]	pixel		Le pixel
	 *\return		La valeur de la composante
	 */
	template< PixelFormat PF >
	float getFloatStencil( Pixel< PF > const & pixel )
	{
		return getFloatComponent( pixel, PixelComponent::eStencil );
	}
	/**
	 *\~english
	 *\brief		Function to define pixel depth stencil in float
	 *\param[in]	pixel		The pixel
	 *\param[in]	value		The PixelComponents value
	 *\~french
	 *\brief		Fonction de définition de la composante stencil d'un pixel, en flottant
	 *\param[in]	pixel		Le pixel
	 *\param[in]	value		La valeur de la composante
	 */
	template< PixelFormat PF >
	void setFloatStencil( Pixel< PF > & pixel, float value )
	{
		return setFloatComponent( pixel, PixelComponent::eStencil, value );
	}
	/**
	 *\~english
	 *\brief		Function to retrieve pixel depth stencil in byte
	 *\param[in]	pixel		The pixel
	 *\return		The PixelComponents value
	 *\~french
	 *\brief		Fonction de récupération de la composante stencil d'un pixel, en octet
	 *\param[in]	pixel		Le pixel
	 *\return		La valeur de la composante
	 */
	template< PixelFormat PF >
	uint8_t getByteStencil( Pixel< PF > const & pixel )
	{
		return getByteComponent( pixel, PixelComponent::eStencil );
	}
	/**
	 *\~english
	 *\brief		Function to define pixel depth stencil in byte
	 *\param[in]	pixel		The pixel
	 *\param[in]	value		The PixelComponents value
	 *\~french
	 *\brief		Fonction de définition de la composante stencil d'un pixel, en octet
	 *\param[in]	pixel		Le pixel
	 *\param[in]	value		La valeur de la composante
	 */
	template< PixelFormat PF >
	void setByteStencil( Pixel< PF > & pixel, uint8_t value )
	{
		return setByteComponent( pixel, PixelComponent::eStencil, value );
	}
}

#include "PixelComponents.inl"

#endif
