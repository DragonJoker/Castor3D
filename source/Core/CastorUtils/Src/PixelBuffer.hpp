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
	template< ePIXEL_FORMAT PF >
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
		 *\param[in]	p_ptSize		Buffer wanted dimensions
		 *\param[in]	p_pBuffer		Data buffer
		 *\param[in]	p_eBufferFormat	Data buffer's pixels format
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_ptSize		Les dimensions voulues pour le buffer
		 *\param[in]	p_pBuffer		Buffer de donn�es
		 *\param[in]	p_eBufferFormat	Format des pixels du buffer de donn�es
		 */
		PxBuffer( Size const & p_ptSize, uint8_t const * p_pBuffer = NULL, ePIXEL_FORMAT p_eBufferFormat = ePIXEL_FORMAT_A8R8G8B8 );
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	p_pixelBuffer	The PxBuffer object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_pixelBuffer	L'objet PxBuffer � copier
		 */
		PxBuffer( PxBuffer const & p_pixelBuffer );
		/**
		 *\~english
		 *\brief		Move Constructor
		 *\param[in]	p_pixelBuffer	The PxBuffer object to move
		 *\~french
		 *\brief		Constructeur par d�placement
		 *\param[in]	p_pixelBuffer	L'objet PxBuffer � d�placer
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
		 *\brief		Op�rateur d'affectation par copie
		 *\param[in]	p_pixelBuffer	L'objet PxBuffer � copier
		 *\return		Une r�f�rence sur cet objet PxBuffer
		 */
		PxBuffer & operator =( PxBuffer const & p_pixelBuffer );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_pixelBuffer	The PxBuffer object to move
		 *\return		A reference to this PxBuffer object
		 *\~french
		 *\brief		Op�rateur d'affectation  par d�placement
		 *\param[in]	p_pixelBuffer	L'objet PxBuffer � d�placer
		 *\return		Une r�f�rence sur cet objet PxBuffer
		 */
		PxBuffer & operator =( PxBuffer && p_pixelBuffer );
		/**
		 *\~english
		 *\brief		Retrieves the column at given index
		 *\remark		No check is made, if you make an index error, expect a crash
		 *\param[in]	p_uiIndex	The wanted index
		 *\return		A constant reference on column at wanted index
		 *\~french
		 *\brief		R�cup�re la colonne � l'index donn�
		 *\remark		Aucun check n'est fait, s'il y a une erreur d'index, attendez-vous � un crash
		 *\param[in]	p_uiIndex	L'index
		 *\return		Une r�f�rence constante sur la colonne � l'index voulu
		 */
		inline	column const & operator []( uint32_t p_uiIndex )const
		{
			do_init_column( p_uiIndex );
			return m_column;
		}
		/**
		 *\~english
		 *\brief		Retrieves the column at given index
		 *\remark		No check is made, if you make an index error, expect a crash
		 *\param[in]	p_uiIndex	The wanted index
		 *\return		A reference on column at wanted index
		 *\~french
		 *\brief		R�cup�re la colonne � l'index donn�
		 *\remark		Aucun check n'est fait, s'il y a une erreur d'index, attendez-vous � un crash
		 *\param[in]	p_uiIndex	L'index
		 *\return		Une r�f�rence sur la colonne � l'index voulu
		 */
		inline	column & operator []( uint32_t p_uiIndex )
		{
			do_init_column( p_uiIndex );
			return m_column;
		}
		/**
		 *\~english
		 *\brief		Swaps this buffer's data with the given one's
		 *\param[in]	p_pixelBuffer	The buffer to swap
		 *\~french
		 *\brief		Echange les donn�es de ce buffer avec celles du buffer donn�
		 *\param[in]	p_pixelBuffer	Le buffer � �changer
		 */
		virtual void swap( PxBuffer & p_pixelBuffer );
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
		virtual void assign( std::vector< uint8_t > const & p_pBuffer, ePIXEL_FORMAT p_eBufferFormat );
		/**
		 *\~english
		 *\brief		Retrieves the pointer on constant datas
		 *\return		The pointer
		 *\~french
		 *\brief		R�cup�re le pointeur sur les donn�es constantes
		 *\return		Les donn�es
		 */
		virtual uint8_t const * const_ptr()const
		{
			return m_pBuffer;
		}
		/**
		 *\~english
		 *\brief		Retrieves the pointer on datas
		 *\return		The pointer
		 *\~french
		 *\brief		R�cup�re le pointeur sur les donn�es
		 *\return		Les donn�es
		 */
		virtual uint8_t * ptr()
		{
			return m_pBuffer;
		}
		/**
		 *\~english
		 *\brief		Retrieves the total size of the buffer
		 *\return		count() * pixel_definitions< PF >::Size
		 *\~french
		 *\brief		R�cup�re la taille totale du buffer
		 *\return		count() * pixel_definitions< PF >::Size
		 */
		virtual uint32_t size()const
		{
			return count() * pixel_definitions< PF >::Size;
		}
		/**
		 *\~english
		 *\brief		Creates a new buffer with same values as this one
		 *\return		The created buffer
		 *\~french
		 *\brief		Cr�e un nouveau buffer avec les m�mes valeurs
		 *\return		Le buffer cr��
		 */
		virtual std::shared_ptr< PxBufferBase >	clone()const;
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
		virtual uint8_t * get_at( uint32_t x, uint32_t y );
		/**
		 *\~english
		 *\brief		Retrieves the pixel data at given position
		 *\param[in]	x, y	The pixel position
		 *\return		The pixel constant data
		 *\~french
		 *\brief		R�cup�re les donn�es du pixel � la position donn�e
		 *\param[in]	x, y	The pixel position
		 *\return		Les donn�es constantes du pixel
		 */
		virtual uint8_t const * get_at( uint32_t x, uint32_t y )const;
		/**
		 *\~english
		 *\brief		Makes a vertical swap of pixels
		 *\~french
		 *\brief		Effectue un �change vertical des pixels
		 */
		virtual void flip();
		/**
		 *\~english
		 *\brief		Makes a horizontal swap of pixels
		 *\~french
		 *\brief		Effectue un �change horizontal des pixels
		 */
		virtual void mirror();

	private:
		virtual void do_init_column( uint32_t p_uiColumn );

	private:
		pixel m_pixel;
		column m_column;
	};
}

#include "PixelBuffer.inl"

#endif
