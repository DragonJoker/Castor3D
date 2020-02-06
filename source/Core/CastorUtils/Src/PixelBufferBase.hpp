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
#ifndef ___CASTOR_PIXEL_BUFFER_BASE_H___
#define ___CASTOR_PIXEL_BUFFER_BASE_H___

#include "Pixel.hpp"
#include "Size.hpp"
#include "Position.hpp"

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		29/08/2011
	\~english
	\brief		Pixel buffer base definition
	\remark		It has 2 dimensions
	\~french
	\brief		D�finition de la classe de base d'un buffer de Pixel
	\remark		Il a 2 dimensions
	*/
	class PxBufferBase
	{
	public:
		//!\~english Typedef over a pointer to uint8_t	\~french Typedef d'un pointeur sur uint8_t
		typedef uint8_t * iterator;
		//!\~english Typedef over a constant pointer to uint8_t	\~french Typedef d'un pointeur constant sur uint8_t
		typedef uint8_t const * const_iterator;

	public:
		/**
		 *\~english
		 *\brief		Constructor from another buffer
		 *\param[in]	p_size			Buffer dimensions
		 *\param[in]	p_ePixelFormat	Pixels format
		 *\param[in]	p_pBuffer		Data buffer
		 *\param[in]	p_eBufferFormat	Data buffer's pixels format
		 *\~french
		 *\brief		Constructeur � partir d'un autre buffer
		 *\param[in]	p_size			Dimensions du buffer
		 *\param[in]	p_ePixelFormat	Format des pixels du buffer
		 *\param[in]	p_pBuffer		Buffer de donn�es
		 *\param[in]	p_eBufferFormat	Format des pixels du buffer de donn�es
		 */
		PxBufferBase( Size const & p_size, ePIXEL_FORMAT p_ePixelFormat, uint8_t const * p_pBuffer = NULL, ePIXEL_FORMAT p_eBufferFormat = ePIXEL_FORMAT_A8R8G8B8 );
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	p_pixelBuffer	The PxBufferBase object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_pixelBuffer	L'objet PxBufferBase � copier
		 */
		PxBufferBase( PxBufferBase const & p_pixelBuffer );
		/**
		 *\~english
		 *\brief		Move Constructor
		 *\param[in]	p_pixelBuffer	The PxBufferBase object to move
		 *\~french
		 *\brief		Constructeur par d�placement
		 *\param[in]	p_pixelBuffer	L'objet PxBufferBase � d�placer
		 */
		PxBufferBase( PxBufferBase && p_pixelBuffer );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~PxBufferBase();
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_pixelBuffer	The PxBufferBase object to copy
		 *\return		A reference to this PxBufferBase object
		 *\~french
		 *\brief		Op�rateur d'affectation par copie
		 *\param[in]	p_pixelBuffer	L'objet PxBufferBase � copier
		 *\return		Une r�f�rence sur cet objet PxBufferBase
		 */
		PxBufferBase & operator =( PxBufferBase const & p_pixelBuffer );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_pixelBuffer	The PxBufferBase object to move
		 *\return		A reference to this PxBufferBase object
		 *\~french
		 *\brief		Op�rateur d'affectation  par d�placement
		 *\param[in]	p_pixelBuffer	L'objet PxBufferBase � d�placer
		 *\return		Une r�f�rence sur cet objet PxBufferBase
		 */
		PxBufferBase & operator =( PxBufferBase && p_pixelBuffer );
		/**
		 *\~english
		 *\brief		Deletes the data buffer
		 *\~french
		 *\brief		D�truit le buffer de donn�es
		 */
		virtual void clear();
		/**
		 *\~english
		 *\brief		Initialises the data buffer to the given one
		 *\remark		Conversions are made if needed
		 *\param[in]	p_pBuffer		Data buffer
		 *\param[in]	p_eBufferFormat	Data buffer's pixels format
		 *\~french
		 *\brief		Initialise le buffer de donn�es � celui donn�
		 *\remark		Des conversions sont faites si besoin est
		 *\param[in]	p_pBuffer		Buffer de donn�es
		 *\param[in]	p_eBufferFormat	Format des pixels du buffer de donn�es
		 */
		virtual void init( uint8_t const * p_pBuffer, ePIXEL_FORMAT p_eBufferFormat );
		/**
		 *\~english
		 *\brief		Initialises the data buffer at the given size
		 *\remark		Conversions are made if needed
		 *\param[in]	p_size		Buffer dimensions
		 *\~french
		 *\brief		Initialise le buffer de donn�es � la taille donn�e
		 *\remark		Des conversions sont faites si besoin est
		 *\param[in]	p_size		Les dimensions du buffer
		 */
		virtual void init( Size const & p_size );
		/**
		 *\~english
		 *\brief		Swaps this buffer's data with the given one's
		 *\param[in]	p_pixelBuffer	The buffer to swap
		 *\~french
		 *\brief		Echange les donn�es de ce buffer avec celles du buffer donn�
		 *\param[in]	p_pixelBuffer	Le buffer � �changer
		 */
		virtual void swap( PxBufferBase & p_pixelBuffer );
		/**
		 *\~english
		 *\brief		Converts and assigns a data buffer to this buffer
		 *\param[in]	p_pBuffer		Data buffer
		 *\param[in]	p_eBufferFormat	Data buffer's pixels format
		 *\return
		 *\~french
		 *\brief		Convertit et assigne les donn�es du buffer donn� � ce buffer
		 *\param[in]	p_pBuffer		Buffer de donn�es
		 *\param[in]	p_eBufferFormat	Format des pixels du buffer de donn�es
		 *\return
		 */
		virtual void assign( std::vector< uint8_t > const & p_pBuffer, ePIXEL_FORMAT p_eBufferFormat ) = 0;
		/**
		 *\~english
		 *\brief		Retrieves the pointer on constant datas
		 *\return		The pointer
		 *\~french
		 *\brief		R�cup�re le pointeur sur les donn�es constantes
		 *\return		Les donn�es
		 */
		virtual uint8_t const * const_ptr()const = 0;
		/**
		 *\~english
		 *\brief		Retrieves the pointer on datas
		 *\return		The pointer
		 *\~french
		 *\brief		R�cup�re le pointeur sur les donn�es
		 *\return		Les donn�es
		 */
		virtual uint8_t * ptr() = 0;
		/**
		 *\~english
		 *\brief		Retrieves the total size of the buffer
		 *\return		count() * (size of a pixel)
		 *\~french
		 *\brief		R�cup�re la taille totale du buffer
		 *\return		count() * (size of a pixel)
		 */
		virtual uint32_t size()const = 0;
		/**
		 *\~english
		 *\brief		Creates a new buffer with same values as this one
		 *\return		The created buffer
		 *\~french
		 *\brief		Cr�e un nouveau buffer avec les m�mes valeurs
		 *\return		Le buffer cr��
		 */
		virtual std::shared_ptr<PxBufferBase> clone()const = 0;
		/**
		 *\~english
		 *\brief		Retrieves the pixel data at given position
		 *\param[in]	x, y	The pixel position
		 *\return		The pixel data
		 *\~french
		 *\brief		R�cup�re les donn�es du pixel � la position donn�e
		 *\param[in]	x, y	The pixel position
		 *\return		Les donn�es du pixel
		 */
		virtual uint8_t * get_at( uint32_t x, uint32_t y ) = 0;
		/**
		 *\~english
		 *\brief		Retrieves the pixel data at given position
		 *\param[in]	x, y	The pixel position
		 *\return		The constant pixel data
		 *\~french
		 *\brief		R�cup�re les donn�es du pixel � la position donn�e
		 *\param[in]	x, y	The pixel position
		 *\return		Les donn�es constantes du pixel
		 */
		virtual uint8_t const * get_at( uint32_t x, uint32_t y )const = 0;
		/**
		 *\~english
		 *\brief		Makes a vertical swap of pixels
		 *\~french
		 *\brief		Effectue un �change vertical des pixels
		 */
		virtual void flip() = 0;
		/**
		 *\~english
		 *\brief		Makes a horizontal swap of pixels
		 *\~french
		 *\brief		Effectue un �change horizontal des pixels
		 */
		virtual void mirror() = 0;
		/**
		 *\~english
		 *\brief		Retrieves the pixels format
		 *\return		The pixels format
		 *\~french
		 *\brief		R�cup�re le format ds pixels
		 *\return		Le format des pixels
		 */
		inline ePIXEL_FORMAT format()const
		{
			return m_ePixelFormat;
		}
		/**
		 *\~english
		 *\brief		Retrieves the buffer width
		 *\return		The buffer width
		 *\~french
		 *\brief		R�cup�re la largeur du buffer
		 *\return		La largeur du buffer
		 */
		inline uint32_t width()const
		{
			return m_size.width();
		}
		/**
		 *\~english
		 *\brief		Retrieves the buffer height
		 *\return		The buffer height
		 *\~french
		 *\brief		R�cup�re la hauteur du buffer
		 *\return		La hauteur du buffer
		 */
		inline uint32_t height()const
		{
			return m_size.height();
		}
		/**
		 *\~english
		 *\brief		Retrieves the buffer's dimensions
		 *\return		The buffer's dimensions
		 *\~french
		 *\brief		R�cup�re les dimensions du buffer
		 *\return		Les dimensions du buffer
		 */
		inline Size const & dimensions()const
		{
			return m_size;
		}
		/**
		 *\~english
		 *\brief		Retrieves the pixels count
		 *\return		width * height
		 *\~french
		 *\brief		R�cup�re le compte des pixels
		 *\return		largeur * hauteur
		 */
		inline uint32_t count()const
		{
			return width() * height();
		}
		/**
		 *\~english
		 *\brief		Retrieves the pixel data at given position
		 *\param[in]	p_position	The pixel position
		 *\return		The pixel data
		 *\~french
		 *\brief		R�cup�re les donn�es du pixel � la position donn�e
		 *\param[in]	p_position	The pixel position
		 *\return		Les donn�es du pixel
		 */
		inline uint8_t * get_at( Position const & p_position )
		{
			return get_at( p_position.x(), p_position.y() );
		}
		/**
		 *\~english
		 *\brief		Retrieves the pixel data at given position
		 *\param[in]	p_position	The pixel position
		 *\return		The pixel constant data
		 *\~french
		 *\brief		R�cup�re les donn�es du pixel � la position donn�e
		 *\param[in]	p_position	The pixel position
		 *\return		Les donn�es constantes du pixel
		 */
		inline uint8_t const * get_at( Position const & p_position )const
		{
			return get_at( p_position.x(), p_position.y() );
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the first element
		 *\return		The iterator
		 *\~french
		 *\brief		R�cup�re un it�rateur sur le premier �l�ment
		 *\return		L'it�rateur
		 */
		inline iterator begin()
		{
			return & m_pBuffer[0];
		}
		/**
		 *\~english
		 *\brief		Retrieves a constant iterator to the first element
		 *\return		The iterator
		 *\~french
		 *\brief		R�cup�re un it�rateur constant sur le premier �l�ment
		 *\return		L'it�rateur
		 */
		inline const_iterator begin()const
		{
			return & m_pBuffer[0];
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the last element
		 *\return		The iterator
		 *\~french
		 *\brief		R�cup�re un it�rateur sur le dernier �l�ment
		 *\return		L'it�rateur
		 */
		inline iterator end()
		{
			return m_pBuffer + size();
		}
		/**
		 *\~english
		 *\brief		Retrieves a constant iterator to the last element
		 *\return		The iterator
		 *\~french
		 *\brief		R�cup�re un it�rateur constant sur le dernier �l�ment
		 *\return		L'it�rateur
		 */
		inline const_iterator end()const
		{
			return m_pBuffer + size();
		}
		/**
		 *\~english
		 *\brief		Creates a buffer with the given data
		 *\param[in]	p_size			Buffer dimensions
		 *\param[in]	p_eWantedFormat	Pixels format
		 *\param[in]	p_pBuffer		Data buffer
		 *\param[in]	p_eBufferFormat	Data buffer's pixels format
		 *\return		The created buffer
		 *\~french
		 *\brief		Cr�e un buffer avec les donn�es voulues
		 *\param[in]	p_size			Dimensions du buffer
		 *\param[in]	p_eWantedFormat	Format des pixels du buffer
		 *\param[in]	p_pBuffer		Buffer de donn�es
		 *\param[in]	p_eBufferFormat	Format des pixels du buffer de donn�es
		 *\return		Le buffer cr��
		 */
		static PxBufferBaseSPtr create( Size const & p_size, ePIXEL_FORMAT p_eWantedFormat, uint8_t const * p_pBuffer = NULL, ePIXEL_FORMAT p_eBufferFormat = ePIXEL_FORMAT_A8R8G8B8 );

	private:
		ePIXEL_FORMAT m_ePixelFormat;

	protected:
		//!\~english Buffer dimensions	\~french Dimensions du buffer
		Size m_size;
		//!\~english Buffer data	\~french Donn�es du buffer
		uint8_t * m_pBuffer;
	};
}

#endif
