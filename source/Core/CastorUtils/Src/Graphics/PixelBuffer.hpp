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
#ifndef ___CASTOR_PIXEL_BUFFER_H___
#define ___CASTOR_PIXEL_BUFFER_H___

#include "PixelBufferBase.hpp"

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		Pixel buffer class, with pixel format as a template param
	\~french
	\brief		Buffer de pixels avec le format de pixel en argument template
	*/
	template< TPL_PIXEL_FORMAT PF >
	class PxBuffer
		: public PxBufferBase
	{
	public:
		//!\~english Typedef on a Pixel	\~french Typedef sur un Pixel
		typedef Pixel< PF > pixel;
		//!\~english Typedef on a vector of pixel	\~french Typedef sur un vector de pixel
		typedef std::vector< pixel > column;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_size			Buffer wanted dimensions
		 *\param[in]	p_buffer		Data buffer
		 *\param[in]	p_bufferFormat	Data buffer's pixels format
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_size			Les dimensions voulues pour le buffer
		 *\param[in]	p_buffer		Buffer de données
		 *\param[in]	p_bufferFormat	Format des pixels du buffer de données
		 */
		PxBuffer( Size const & p_size, uint8_t const * p_buffer = nullptr, PixelFormat p_bufferFormat = PixelFormat::A8R8G8B8 );
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	p_pixelBuffer	The PxBuffer object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_pixelBuffer	L'objet PxBuffer à copier
		 */
		PxBuffer( PxBuffer const & p_pixelBuffer );
		/**
		 *\~english
		 *\brief		Move Constructor
		 *\param[in]	p_pixelBuffer	The PxBuffer object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_pixelBuffer	L'objet PxBuffer à déplacer
		 */
		PxBuffer( PxBuffer && p_pixelBuffer );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~PxBuffer();
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_pixelBuffer	The PxBuffer object to copy
		 *\return		A reference to this PxBuffer object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_pixelBuffer	L'objet PxBuffer à copier
		 *\return		Une référence sur cet objet PxBuffer
		 */
		PxBuffer & operator=( PxBuffer const & p_pixelBuffer );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_pixelBuffer	The PxBuffer object to move
		 *\return		A reference to this PxBuffer object
		 *\~french
		 *\brief		Opérateur d'affectation  par déplacement
		 *\param[in]	p_pixelBuffer	L'objet PxBuffer à déplacer
		 *\return		Une référence sur cet objet PxBuffer
		 */
		PxBuffer & operator=( PxBuffer && p_pixelBuffer );
		/**
		 *\~english
		 *\brief		Retrieves the column at given index
		 *\remarks		No check is made, if you make an index error, expect a crash
		 *\param[in]	p_index	The wanted index
		 *\return		A constant reference on column at wanted index
		 *\~french
		 *\brief		Récupère la colonne à l'index donné
		 *\remarks		Aucun check n'est fait, s'il y a une erreur d'index, attendez-vous à un crash
		 *\param[in]	p_index	L'index
		 *\return		Une référence constante sur la colonne à l'index voulu
		 */
		inline column const & operator[]( uint32_t p_index )const;
		/**
		 *\~english
		 *\brief		Retrieves the column at given index
		 *\remarks		No check is made, if you make an index error, expect a crash
		 *\param[in]	p_index	The wanted index
		 *\return		A reference on column at wanted index
		 *\~french
		 *\brief		Récupère la colonne à l'index donné
		 *\remarks		Aucun check n'est fait, s'il y a une erreur d'index, attendez-vous à un crash
		 *\param[in]	p_index	L'index
		 *\return		Une référence sur la colonne à l'index voulu
		 */
		inline column & operator[]( uint32_t p_index );
		/**
		 *\~english
		 *\brief		Swaps this buffer's data with the given one's
		 *\param[in]	p_pixelBuffer	The buffer to swap
		 *\~french
		 *\brief		Echange les données de ce buffer avec celles du buffer donné
		 *\param[in]	p_pixelBuffer	Le buffer à échanger
		 */
		virtual void swap( PxBuffer & p_pixelBuffer );
		/**
		 *\~english
		 *\brief		Converts and assigns a data buffer to this buffer
		 *\param[in]	p_buffer		Data buffer
		 *\param[in]	p_bufferFormat	Data buffer's pixels format
		 *\return
		 *\~french
		 *\brief		Convertit et assigne les données du buffer donné à ce buffer
		 *\param[in]	p_buffer		Buffer de données
		 *\param[in]	p_bufferFormat	Format des pixels du buffer de données
		 *\return
		 */
		virtual void assign( std::vector< uint8_t > const & p_buffer, PixelFormat p_bufferFormat );
		/**
		 *\~english
		 *\brief		Retrieves the pointer on constant datas
		 *\return		The pointer
		 *\~french
		 *\brief		Récupère le pointeur sur les données constantes
		 *\return		Les données
		 */
		virtual uint8_t const * const_ptr()const;
		/**
		 *\~english
		 *\brief		Retrieves the pointer on datas
		 *\return		The pointer
		 *\~french
		 *\brief		Récupère le pointeur sur les données
		 *\return		Les données
		 */
		virtual uint8_t * ptr();
		/**
		 *\~english
		 *\brief		Retrieves the total size of the buffer
		 *\return		count() * pixel_definitions< PF >::Size
		 *\~french
		 *\brief		Récupère la taille totale du buffer
		 *\return		count() * pixel_definitions< PF >::Size
		 */
		virtual uint32_t size()const;
		/**
		 *\~english
		 *\brief		Creates a new buffer with same values as this one
		 *\return		The created buffer
		 *\~french
		 *\brief		Crée un nouveau buffer avec les mêmes valeurs
		 *\return		Le buffer créé
		 */
		virtual std::shared_ptr< PxBufferBase >	clone()const;
		/**
		 *\~english
		 *\brief		Retrieves the pixel data at given position
		 *\param[in]	x, y	The pixel position
		 *\return		The pixel data
		 *\~french
		 *\brief		Récupère les données du pixel à la position donnée
		 *\param[in]	x, y	The pixel position
		 *\return		Les données du pixel
		 */
		virtual iterator get_at( uint32_t x, uint32_t y );
		/**
		 *\~english
		 *\brief		Retrieves the pixel data at given position
		 *\param[in]	x, y	The pixel position
		 *\return		The pixel constant data
		 *\~french
		 *\brief		Récupère les données du pixel à la position donnée
		 *\param[in]	x, y	The pixel position
		 *\return		Les données constantes du pixel
		 */
		virtual const_iterator get_at( uint32_t x, uint32_t y )const;
		/**
		 *\~english
		 *\brief		Makes a horizontal swap of pixels
		 *\~french
		 *\brief		Effectue un échange horizontal des pixels
		 */
		virtual void mirror();

	private:
		virtual void do_init_column( uint32_t p_column )const;
		virtual void do_init_column( uint32_t p_column );

	private:
		pixel m_pixel;
		mutable column m_column;
	};
}

#include "PixelBuffer.inl"

#endif
