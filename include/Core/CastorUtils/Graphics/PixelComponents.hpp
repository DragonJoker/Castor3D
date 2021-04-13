/*
See LICENSE file in root folder
*/
#ifndef ___CU_PixelComponents___
#define ___CU_PixelComponents___

#include "CastorUtils/Graphics/GraphicsModule.hpp"

namespace castor
{
	namespace PF
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
		float getX32F( Pixel< PF > const & pixel, PixelComponent component );
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
		void setX32F( Pixel< PF > & pixel, PixelComponent component, float value );
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
		uint8_t getX8U( Pixel< PF > const & pixel, PixelComponent component );
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
		void setX8U( Pixel< PF > & pixel, PixelComponent component, uint8_t value );
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
		uint16_t getX16U( Pixel< PF > const & pixel, PixelComponent component );
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
		void setX16U( Pixel< PF > & pixel, PixelComponent component, uint16_t value );
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
		uint32_t getX24U( Pixel< PF > const & pixel, PixelComponent component );
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
		void setX24U( Pixel< PF > & pixel, PixelComponent component, uint32_t value );
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
		float getR32F( Pixel< PF > const & pixel )
		{
			return getX32F( pixel, PixelComponent::eRed );
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
		void setR32F( Pixel< PF > const & pixel, float value )
		{
			return setX32F( pixel, PixelComponent::eRed, value );
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
		uint8_t getR8U( Pixel< PF > const & pixel )
		{
			return getX8U( pixel, PixelComponent::eRed );
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
		void setR8U( Pixel< PF > & pixel, uint8_t value )
		{
			return setX8U( pixel, PixelComponent::eRed, value );
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
		float getG32F( Pixel< PF > const & pixel )
		{
			return getX32F( pixel, PixelComponent::eGreen );
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
		void setG32F( Pixel< PF > & pixel, float value )
		{
			return setX32F( pixel, PixelComponent::eGreen, value );
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
		uint8_t getG8U( Pixel< PF > const & pixel )
		{
			return getX8U( pixel, PixelComponent::eGreen );
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
		void setG8U( Pixel< PF > & pixel, uint8_t value )
		{
			return setX8U( pixel, PixelComponent::eGreen, value );
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
		float getB32F( Pixel< PF > const & pixel )
		{
			return getX32F( pixel, PixelComponent::eBlue );
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
		void setB32F( Pixel< PF > & pixel, float value )
		{
			return setX32F( pixel, PixelComponent::eBlue, value );
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
		uint8_t getB8U( Pixel< PF > const & pixel )
		{
			return getX8U( pixel, PixelComponent::eBlue );
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
		void setB8U( Pixel< PF > & pixel, uint8_t value )
		{
			return setX8U( pixel, PixelComponent::eBlue, value );
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
		float getA32F( Pixel< PF > const & pixel )
		{
			return getX32F( pixel, PixelComponent::eAlpha );
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
		void setA32F( Pixel< PF > & pixel, float value )
		{
			return setX32F( pixel, PixelComponent::eAlpha, value );
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
		uint8_t getA8U( Pixel< PF > const & pixel )
		{
			return getX8U( pixel, PixelComponent::eAlpha );
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
		void setA8U( Pixel< PF > & pixel, uint8_t value )
		{
			return setX8U( pixel, PixelComponent::eAlpha, value );
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
		float getD32F( Pixel< PF > const & pixel )
		{
			return getX32F( pixel, PixelComponent::eDepth );
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
		void setD32F( Pixel< PF > & pixel, float value )
		{
			return setX32F( pixel, PixelComponent::eDepth, value );
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
		uint16_t getD16U( Pixel< PF > const & pixel )
		{
			return getX16U( pixel, PixelComponent::eDepth );
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
		void setD16U( Pixel< PF > & pixel, uint16_t value )
		{
			return setX16U( pixel, PixelComponent::eDepth, value );
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
		uint32_t getD24U( Pixel< PF > const & pixel )
		{
			return getX24U( pixel, PixelComponent::eDepth );
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
		void setD24U( Pixel< PF > & pixel, uint32_t value )
		{
			return setX24U( pixel, PixelComponent::eDepth, value );
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
		uint8_t getS8U( Pixel< PF > const & pixel )
		{
			return getX8U( pixel, PixelComponent::eStencil );
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
		void setS8U( Pixel< PF > & pixel, uint8_t value )
		{
			return setX8U( pixel, PixelComponent::eStencil, value );
		}
	}

	template< PixelFormat PFT >
	uint8_t getR8U( uint8_t const * buffer );
	template< PixelFormat PFT >
	int8_t getR8S( uint8_t const * buffer );
	template< PixelFormat PFT >
	uint16_t getR16U( uint8_t * buffer );
	template< PixelFormat PFT >
	int16_t getR16S( uint8_t const * buffer );
	template< PixelFormat PFT >
	int16_t getR16F( uint8_t const * buffer );
	template< PixelFormat PFT >
	uint32_t getR32S( uint8_t const * buffer );
	template< PixelFormat PFT >
	int32_t getR32U( uint8_t const * buffer );
	template< PixelFormat PFT >
	float getR32F( uint8_t const * buffer );
	template< PixelFormat PFT >
	uint64_t getR64U( uint8_t const * buffer );
	template< PixelFormat PFT >
	int64_t getR64S( uint8_t const * buffer );
	template< PixelFormat PFT >
	double getR64F( uint8_t const * buffer );

	template< PixelFormat PFT >
	uint8_t getG8U( uint8_t const * buffer );
	template< PixelFormat PFT >
	int8_t getG8S( uint8_t const * buffer );
	template< PixelFormat PFT >
	uint16_t getG16U( uint8_t const * buffer );
	template< PixelFormat PFT >
	int16_t getG16S( uint8_t const * buffer );
	template< PixelFormat PFT >
	int16_t getG16F( uint8_t const * buffer );
	template< PixelFormat PFT >
	uint32_t getG32U( uint8_t const * buffer );
	template< PixelFormat PFT >
	int32_t getG32S( uint8_t const * buffer );
	template< PixelFormat PFT >
	float getG32F( uint8_t const * buffer );
	template< PixelFormat PFT >
	uint64_t getG64U( uint8_t const * buffer );
	template< PixelFormat PFT >
	int64_t getG64S( uint8_t const * buffer );
	template< PixelFormat PFT >
	double getG64F( uint8_t const * buffer );

	template< PixelFormat PFT >
	uint8_t getB8U( uint8_t const * buffer );
	template< PixelFormat PFT >
	int8_t getB8S( uint8_t const * buffer );
	template< PixelFormat PFT >
	uint16_t getB16U( uint8_t const * buffer );
	template< PixelFormat PFT >
	int16_t getB16S( uint8_t const * buffer );
	template< PixelFormat PFT >
	int16_t getB16F( uint8_t const * buffer );
	template< PixelFormat PFT >
	uint32_t getB32U( uint8_t const * buffer );
	template< PixelFormat PFT >
	int32_t getB32S( uint8_t const * buffer );
	template< PixelFormat PFT >
	float getB32F( uint8_t const * buffer );
	template< PixelFormat PFT >
	uint64_t getB64U( uint8_t const * buffer );
	template< PixelFormat PFT >
	int64_t getB64S( uint8_t const * buffer );
	template< PixelFormat PFT >
	double getB64F( uint8_t const * buffer );

	template< PixelFormat PFT >
	uint8_t getA8U( uint8_t const * buffer );
	template< PixelFormat PFT >
	int8_t getA8S( uint8_t const * buffer );
	template< PixelFormat PFT >
	uint16_t getA16U( uint8_t const * buffer );
	template< PixelFormat PFT >
	int16_t getA16S( uint8_t const * buffer );
	template< PixelFormat PFT >
	int16_t getA16F( uint8_t const * buffer );
	template< PixelFormat PFT >
	uint32_t getA32U( uint8_t const * buffer );
	template< PixelFormat PFT >
	int32_t getA32S( uint8_t const * buffer );
	template< PixelFormat PFT >
	float getA32F( uint8_t const * buffer );
	template< PixelFormat PFT >
	uint64_t getA64U( uint8_t const * buffer );
	template< PixelFormat PFT >
	int64_t getA64S( uint8_t const * buffer );
	template< PixelFormat PFT >
	double getA64F( uint8_t const * buffer );

	template< PixelFormat PFT >
	uint16_t getD16U( uint8_t const * buffer );
	template< PixelFormat PFT >
	uint32_t getD24U( uint8_t const * buffer );
	template< PixelFormat PFT >
	float getD32F( uint8_t const * buffer );

	template< PixelFormat PFT >
	uint8_t getS8U( uint8_t const * buffer );

	template< PixelFormat PFT >
	void setR8U( uint8_t  * buffer, uint8_t value );
	template< PixelFormat PFT >
	void setR8S( uint8_t  * buffer, int8_t value );
	template< PixelFormat PFT >
	void setR16U( uint8_t * buffer, uint16_t value );
	template< PixelFormat PFT >
	void setR16S( uint8_t  * buffer, int16_t value );
	template< PixelFormat PFT >
	void setR16F( uint8_t  * buffer, int16_t value );
	template< PixelFormat PFT >
	void setR32S( uint8_t  * buffer, uint32_t value );
	template< PixelFormat PFT >
	void setR32U( uint8_t  * buffer, int32_t value );
	template< PixelFormat PFT >
	void setR32F( uint8_t  * buffer, float value );
	template< PixelFormat PFT >
	void setR64U( uint8_t  * buffer, uint64_t value );
	template< PixelFormat PFT >
	void setR64S( uint8_t  * buffer, int64_t value );
	template< PixelFormat PFT >
	void setR64F( uint8_t  * buffer, double value );

	template< PixelFormat PFT >
	void setG8U( uint8_t  * buffer, uint8_t value );
	template< PixelFormat PFT >
	void setG8S( uint8_t  * buffer, int8_t value );
	template< PixelFormat PFT >
	void setG16U( uint8_t  * buffer, uint16_t value );
	template< PixelFormat PFT >
	void setG16S( uint8_t  * buffer, int16_t value );
	template< PixelFormat PFT >
	void setG16F( uint8_t  * buffer, int16_t value );
	template< PixelFormat PFT >
	void setG32U( uint8_t  * buffer, uint32_t value );
	template< PixelFormat PFT >
	void setG32S( uint8_t  * buffer, int32_t value );
	template< PixelFormat PFT >
	void setG32F( uint8_t  * buffer, float value );
	template< PixelFormat PFT >
	void setG64U( uint8_t  * buffer, uint64_t value );
	template< PixelFormat PFT >
	void setG64S( uint8_t  * buffer, int64_t value );
	template< PixelFormat PFT >
	void setG64F( uint8_t  * buffer, double value );

	template< PixelFormat PFT >
	void setB8U( uint8_t  * buffer, uint8_t value );
	template< PixelFormat PFT >
	void setB8S( uint8_t  * buffer, int8_t value );
	template< PixelFormat PFT >
	void setB16U( uint8_t  * buffer, uint16_t value );
	template< PixelFormat PFT >
	void setB16S( uint8_t  * buffer, int16_t value );
	template< PixelFormat PFT >
	void setB16F( uint8_t  * buffer, int16_t value );
	template< PixelFormat PFT >
	void setB32U( uint8_t  * buffer, uint32_t value );
	template< PixelFormat PFT >
	void setB32S( uint8_t  * buffer, int32_t value );
	template< PixelFormat PFT >
	void setB32F( uint8_t  * buffer, float value );
	template< PixelFormat PFT >
	void setB64U( uint8_t  * buffer, uint64_t value );
	template< PixelFormat PFT >
	void setB64S( uint8_t  * buffer, int64_t value );
	template< PixelFormat PFT >
	void setB64F( uint8_t  * buffer, double value );

	template< PixelFormat PFT >
	void setA8U( uint8_t  * buffer, uint8_t value );
	template< PixelFormat PFT >
	void setA8S( uint8_t  * buffer, int8_t value );
	template< PixelFormat PFT >
	void setA16U( uint8_t  * buffer, uint16_t value );
	template< PixelFormat PFT >
	void setA16S( uint8_t  * buffer, int16_t value );
	template< PixelFormat PFT >
	void setA16F( uint8_t  * buffer, int16_t value );
	template< PixelFormat PFT >
	void setA32U( uint8_t  * buffer, uint32_t value );
	template< PixelFormat PFT >
	void setA32S( uint8_t  * buffer, int32_t value );
	template< PixelFormat PFT >
	void setA32F( uint8_t  * buffer, float value );
	template< PixelFormat PFT >
	void setA64U( uint8_t  * buffer, uint64_t value );
	template< PixelFormat PFT >
	void setA64S( uint8_t  * buffer, int64_t value );
	template< PixelFormat PFT >
	void setA64F( uint8_t  * buffer, double value );

	template< PixelFormat PFT >
	void setD16U( uint8_t  * buffer, uint16_t value );
	template< PixelFormat PFT >
	void setD24U( uint8_t  * buffer, uint32_t value );
	template< PixelFormat PFT >
	void setD32F( uint8_t  * buffer, float value );

	template< PixelFormat PFT >
	void setS8U( uint8_t  * buffer, uint8_t value );
}

#include "PixelComponents.inl"

#endif
