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
	\brief		Définition de la classe de base d'un buffer de Pixel
	\remark		Il a 2 dimensions
	*/
	class PxBufferBase
	{
	public:
		typedef std::vector< uint8_t > px_array;
		typedef px_array::iterator iterator;
		typedef px_array::const_iterator const_iterator;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_size			Buffer dimensions.
		 *\param[in]	p_pixelFormat	Pixels format.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_size			Dimensions du buffer.
		 *\param[in]	p_pixelFormat	Format des pixels du buffer.
		 */
		CU_API PxBufferBase( Size const & p_size, ePIXEL_FORMAT p_pixelFormat );
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	p_pixelBuffer	The PxBufferBase object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_pixelBuffer	L'objet PxBufferBase à copier
		 */
		CU_API PxBufferBase( PxBufferBase const & p_pixelBuffer );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		CU_API virtual ~PxBufferBase();
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_pixelBuffer	The PxBufferBase object to copy
		 *\return		A reference to this PxBufferBase object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_pixelBuffer	L'objet PxBufferBase à copier
		 *\return		Une référence sur cet objet PxBufferBase
		 */
		CU_API PxBufferBase & operator=( PxBufferBase const & p_pixelBuffer );
		/**
		 *\brief		Deletes the data buffer
		 *\~french
		 *\brief		Détruit le tampon de données
		 */
		CU_API virtual void clear();
		/**
		 *\~english
		 *\brief		Initialises the data buffer to the given one
		 *\remark		Conversions are made if needed
		 *\param[in]	p_buffer		Data buffer
		 *\param[in]	p_pixelFormat	Data buffer's pixels format
		 *\~french
		 *\brief		Initialise le buffer de données à celui donné
		 *\remark		Des conversions sont faites si besoin est
		 *\param[in]	p_buffer		Buffer de données
		 *\param[in]	p_pixelFormat	Format des pixels du buffer de données
		 */
		CU_API virtual void init( uint8_t const * p_buffer, ePIXEL_FORMAT p_pixelFormat );
		/**
		 *\~english
		 *\brief		Initialises the data buffer at the given size
		 *\remark		Conversions are made if needed
		 *\param[in]	p_size		Buffer dimensions
		 *\~french
		 *\brief		Initialise le buffer de données à la taille donnée
		 *\remark		Des conversions sont faites si besoin est
		 *\param[in]	p_size		Les dimensions du buffer
		 */
		CU_API virtual void init( Size const & p_size );
		/**
		 *\~english
		 *\brief		Swaps this buffer's data with the given one's
		 *\param[in]	p_pixelBuffer	The buffer to swap
		 *\~french
		 *\brief		Echange les données de ce buffer avec celles du buffer donné
		 *\param[in]	p_pixelBuffer	Le buffer à échanger
		 */
		CU_API virtual void swap( PxBufferBase & p_pixelBuffer );
		/**
		 *\~english
		 *\brief		Makes a vertical swap of pixels
		 *\~french
		 *\brief		Effectue un échange vertical des pixels
		 */
		CU_API void flip();
		/**
		 *\~english
		 *\brief		Converts and assigns a data buffer to this buffer
		 *\param[in]	p_buffer		Data buffer
		 *\param[in]	p_pixelFormat	Data buffer's pixels format
		 *\return
		 *\~french
		 *\brief		Convertit et assigne les données du buffer donné à ce buffer
		 *\param[in]	p_buffer		Buffer de données
		 *\param[in]	p_pixelFormat	Format des pixels du buffer de données
		 *\return
		 */
		CU_API virtual void assign( std::vector< uint8_t > const & p_buffer, ePIXEL_FORMAT p_pixelFormat ) = 0;
		/**
		 *\~english
		 *\brief		Retrieves the pointer on constant datas
		 *\return		The pointer
		 *\~french
		 *\brief		Récupère le pointeur sur les données constantes
		 *\return		Les données
		 */
		CU_API virtual uint8_t const * const_ptr()const = 0;
		/**
		 *\~english
		 *\brief		Retrieves the pointer on datas
		 *\return		The pointer
		 *\~french
		 *\brief		Récupère le pointeur sur les données
		 *\return		Les données
		 */
		CU_API virtual uint8_t * ptr() = 0;
		/**
		 *\~english
		 *\brief		Retrieves the total size of the buffer
		 *\return		count() * (size of a pixel)
		 *\~french
		 *\brief		Récupère la taille totale du buffer
		 *\return		count() * (size of a pixel)
		 */
		CU_API virtual uint32_t size()const = 0;
		/**
		 *\~english
		 *\brief		Creates a new buffer with same values as this one
		 *\return		The created buffer
		 *\~french
		 *\brief		Crée un nouveau buffer avec les mêmes valeurs
		 *\return		Le buffer créé
		 */
		CU_API virtual std::shared_ptr<PxBufferBase> clone()const = 0;
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
		CU_API virtual iterator get_at( uint32_t x, uint32_t y ) = 0;
		/**
		 *\~english
		 *\brief		Retrieves the pixel data at given position
		 *\param[in]	x, y	The pixel position
		 *\return		The constant pixel data
		 *\~french
		 *\brief		Récupère les données du pixel à la position donnée
		 *\param[in]	x, y	The pixel position
		 *\return		Les données constantes du pixel
		 */
		CU_API virtual const_iterator get_at( uint32_t x, uint32_t y )const = 0;
		/**
		 *\~english
		 *\brief		Makes a horizontal swap of pixels
		 *\~french
		 *\brief		Effectue un échange horizontal des pixels
		 */
		CU_API virtual void mirror() = 0;
		/**
		 *\~english
		 *\brief		Retrieves the pixels format
		 *\return		The pixels format
		 *\~french
		 *\brief		Récupère le format ds pixels
		 *\return		Le format des pixels
		 */
		inline ePIXEL_FORMAT format()const
		{
			return m_pixelFormat;
		}
		/**
		 *\~english
		 *\brief		Retrieves the buffer width
		 *\return		The buffer width
		 *\~french
		 *\brief		Récupère la largeur du buffer
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
		 *\brief		Récupère la hauteur du buffer
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
		 *\brief		Récupère les dimensions du buffer
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
		 *\brief		Récupère le compte des pixels
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
		 *\brief		Récupère les données du pixel à la position donnée
		 *\param[in]	p_position	The pixel position
		 *\return		Les données du pixel
		 */
		inline iterator get_at( Position const & p_position )
		{
			return get_at( p_position.x(), p_position.y() );
		}
		/**
		 *\~english
		 *\brief		Retrieves the pixel data at given position
		 *\param[in]	p_position	The pixel position
		 *\return		The pixel constant data
		 *\~french
		 *\brief		Récupère les données du pixel à la position donnée
		 *\param[in]	p_position	The pixel position
		 *\return		Les données constantes du pixel
		 */
		inline const_iterator get_at( Position const & p_position )const
		{
			return get_at( p_position.x(), p_position.y() );
		}
		/**
		 *\~english
		 *\brief		Creates a buffer with the given data
		 *\param[in]	p_size			Buffer dimensions
		 *\param[in]	p_wantedFormat	Pixels format
		 *\param[in]	p_buffer		Data buffer
		 *\param[in]	p_bufferFormat	Data buffer's pixels format
		 *\return		The created buffer
		 *\~french
		 *\brief		Crée un buffer avec les données voulues
		 *\param[in]	p_size			Dimensions du buffer
		 *\param[in]	p_wantedFormat	Format des pixels du buffer
		 *\param[in]	p_buffer		Buffer de données
		 *\param[in]	p_bufferFormat	Format des pixels du buffer de données
		 *\return		Le buffer créé
		 */
		CU_API static PxBufferBaseSPtr create( Size const & p_size, ePIXEL_FORMAT p_wantedFormat, uint8_t const * p_buffer = NULL, ePIXEL_FORMAT p_bufferFormat = ePIXEL_FORMAT_A8R8G8B8 );

	private:
		ePIXEL_FORMAT m_pixelFormat;

	protected:
		//!\~english Buffer dimensions	\~french Dimensions du buffer
		Size m_size;
		//!\~english Buffer data	\~french Données du buffer
		px_array m_buffer;
	};
}

#endif
