﻿/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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
#ifndef ___CASTOR_IMAGE_H___
#define ___CASTOR_IMAGE_H___

#include "Resource.hpp"
#include "Loader.hpp"
#include "Colour.hpp"
#include "PixelBuffer.hpp"

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\date		14/02/2010
	\~english
	\brief		Image resource
	\~french
	\brief		Ressource Image
	*/
	class Image
		: public Resource< Image >
	{
	public:
		/*!
		\author		Sylvain DOREMUS
		\date		14/02/2010
		\~english
		\brief		Image resource loader
		\~french
		\brief		Image resource loader
		*/
		class BinaryLoader
			: public Loader< Image, eFILE_TYPE_BINARY, BinaryFile >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			BinaryLoader();
			/**
			 *\~english
			 *\brief			Loads an image from a binary file
			 *\param[in,out]	p_image	The image name
			 *\param[in]		p_path	The path of the file from which to load the image
			 *\~french
			 *\brief			Charge une image à partir d'un fichier binaire
			 *\param[in,out]	p_image	Le nom de l'image
			 *\param[in]		p_path	Le chemin du fichier contenant l'image
			 */
			virtual bool operator()( Image & p_image, Path const & p_path );
			/**
			 *\~english
			 *\brief			Writes an image into a binary file
			 *\param[in,out]	p_image	The image name
			 *\param[in]		p_path	The path of the file into which we write the image
			 *\~french
			 *\brief			Ecrit une image dans un fichier binaire
			 *\param[in,out]	p_image	Le nom de l'image
			 *\param[in]		p_path	Le chemin du fichier recevant l'image
			 */
			virtual bool operator()( Image const & p_image, Path const & p_path );
		};

	private:
		friend class Font;
		friend class Image::BinaryLoader;

		DECLARE_INVARIANT_BLOCK()

	public:
		/**
		 *\~english
		 *\brief		Creates the image with given params
		 *\param[in]	p_strName		The resource name
		 *\param[in]	p_ptSize		The wanted image dimensions
		 *\param[in]	p_ePixelFormat	The wanted image pixel format
		 *\param[in]	p_pBuffer		A buffer containing pixels
		 *\param[in]	p_eBufferFormat	The buffer pixel format
		 *\~french
		 *\brief		Crée l'image avec les paramètres donnés
		 *\param[in]	p_strName		Le nom de ressource
		 *\param[in]	p_ptSize		Les dimensions voulues pour l'image
		 *\param[in]	p_ePixelFormat	Le format de pixel voulu pour l'image
		 *\param[in]	p_pBuffer		Un buffer de pixels
		 *\param[in]	p_eBufferFormat	Le format des pixels du buffer
		 */
		Image( String const & p_strName, Size const & p_ptSize, ePIXEL_FORMAT p_ePixelFormat, ByteArray const & p_pBuffer, ePIXEL_FORMAT p_eBufferFormat );
		/**
		 *\~english
		 *\brief		Creates the image with given params
		 *\param[in]	p_strName		The resource name
		 *\param[in]	p_ptSize		The wanted image dimensions
		 *\param[in]	p_ePixelFormat	The wanted image pixel format
		 *\param[in]	p_pBuffer		A buffer containing pixels
		 *\param[in]	p_eBufferFormat	The buffer pixel format
		 *\~french
		 *\brief		Crée l'image avec les paramètres donnés
		 *\param[in]	p_strName		Le nom de ressource
		 *\param[in]	p_ptSize		Les dimensions voulues pour l'image
		 *\param[in]	p_ePixelFormat	Le format de pixel voulu pour l'image
		 *\param[in]	p_pBuffer		Un buffer de pixels
		 *\param[in]	p_eBufferFormat	Le format des pixels du buffer
		 */
		Image( String const & p_strName, Size const & p_ptSize, ePIXEL_FORMAT p_ePixelFormat = ePIXEL_FORMAT_A8R8G8B8, uint8_t const * p_pBuffer = NULL, ePIXEL_FORMAT p_eBufferFormat = ePIXEL_FORMAT_A8R8G8B8 );
		/**
		 *\~english
		 *\brief		Creates the image with given params
		 *\param[in]	p_strName		The resource name
		 *\param[in]	p_pBuffer		A buffer containing pixels
		 *\~french
		 *\brief		Crée l'image avec les paramètres donnés
		 *\param[in]	p_strName		Le nom de ressource
		 *\param[in]	p_pBuffer		Un buffer de pixels
		 */
		Image( String const & p_strName, PxBufferBase const & p_pBuffer );
		/**
		 *\~english
		 *\brief		Creates the image with given params
		 *\remark		The wanted pixel format and buffer pixel format are the template params
		 *\param[in]	p_strName		The resource name
		 *\param[in]	p_ptSize		The wanted image dimensions
		 *\param[in]	p_pBuffer		A buffer containing pixels
		 *\~french
		 *\brief		Crée l'image avec les paramètres donnés
		 *\remark		Le format voulu ainsi que le format du buffer sont les arguments template
		 *\param[in]	p_strName		Le nom de ressource
		 *\param[in]	p_ptSize		Les dimensions voulues pour l'image
		 *\param[in]	p_pBuffer		Un buffer de pixels
		 */
		template< ePIXEL_FORMAT PFSrc, ePIXEL_FORMAT PFDst >
		Image( String const & p_strName, Size const & p_ptSize, uint8_t const * p_pBuffer = NULL )
			:	Resource< Image >	( p_strName	)
			,	m_pBuffer( new PxBuffer< PFDst >( p_ptSize, p_pBuffer, PFSrc )	)
		{
			CHECK_INVARIANTS();
		}
		/**
		 *\~english
		 *\brief		Creates the image from a file
		 *\param[in]	p_strName		The resource name
		 *\param[in]	p_pathFile		The file path
		 *\~french
		 *\brief		Crée l'image à partir d'un fichier
		 *\param[in]	p_strName		Le nom de ressource
		 *\param[in]	p_pathFile		Le chemin du fichier
		 */
		Image( String const & p_strName, Path const & p_pathFile );
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	p_image	The Image object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_image	L'objet Image à copier
		 */
		Image( Image const & p_image );
		/**
		 *\~english
		 *\brief		Move Constructor
		 *\param[in]	p_image	The Image object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_image	L'objet Image à déplacer
		 */
		Image( Image && p_image );
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_image	The Image object to copy
		 *\return		A reference to this Image object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_image	L'objet Image à copier
		 *\return		Une référence sur cet objet Image
		 */
		Image & operator =( Image const & p_image );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_image	The Image object to move
		 *\return		A reference to this Image object
		 *\~french
		 *\brief		Opérateur d'affectation  par déplacement
		 *\param[in]	p_image	L'objet Image à déplacer
		 *\return		Une référence sur cet objet Image
		 */
		Image & operator =( Image && p_image );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~Image();
		/**
		 *\~english
		 *\brief		Resizes the image to the given resolution
		 *\param[in]	p_size	The new resolution
		 *\return		A reference to the image
		 *\~french
		 *\brief		Redimensionne l'image à la résolution donnée
		 *\param[in]	p_size	La nouvelle résolution
		 *\return		La référence de l'image
		 */
		Image & Resample( Size const & p_size );
		/**
		 *\~english
		 *\brief		Fills all image pixels with the given colour
		 *\param[in]	p_clrColour	The fill colour
		 *\return		A reference to the image
		 *\~french
		 *\brief		Remplit tous les pixels de l'image avec la couleur donnée
		 *\param[in]	p_clrColour	La couleur de remplissage
		 *\return		La référence de l'image
		 */
		Image & Fill( Colour const & p_clrColour );
		/**
		 *\~english
		 *\brief		Fills the wanted pixel with the given buffer
		 *\param[in]	x,y			The pixel coordinates
		 *\param[in]	p_pPixel	The buffer
		 *\return		A reference to the image
		 *\~french
		 *\brief		Remplit le pixel demandé avec le buffer donné
		 *\param[in]	x,y			Les coordonnées du pixel
		 *\param[in]	p_pPixel	Le buffer
		 *\return		La référence de l'image
		 */
		Image & SetPixel( uint32_t x, uint32_t y, uint8_t const * p_pPixel );
		/**
		 *\~english
		 *\brief		Sets the colour of the wanted pixel to the given one
		 *\param[in]	x,y			The pixel coordinates
		 *\param[in]	p_clrColour	The colour
		 *\return		A reference to the image
		 *\~french
		 *\brief		Définit la couleur du pixel demandé
		 *\param[in]	x,y			Les coordonnées du pixel
		 *\param[in]	p_clrColour	La couleur
		 *\return		La référence de l'image
		 */
		Image & SetPixel( uint32_t x, uint32_t y, Colour const & p_clrColour );
		/**
		 *\~english
		 *\brief		Set the wanted pixel to the given pixel
		 *\param[in]	x,y		The pixel coordinates
		 *\param[in]	p_pixel	The pixel
		 *\return		A reference to the image
		 *\~french
		 *\brief		Définit le pixel demandé avec le pixel donné
		 *\param[in]	x,y		Les coordonnées du pixel
		 *\param[in]	p_pixel	Le pixel
		 *\return		La référence de l'image
		 */
		template< ePIXEL_FORMAT PF > Image & SetPixel( uint32_t x, uint32_t y, Pixel< PF > const & p_pixel )
		{
			CHECK_INVARIANTS();
			REQUIRE( x < m_pBuffer->width() && y < m_pBuffer->height() );
			ConvertPixel( PF, p_pixel.const_ptr(), GetPixelFormat(), m_pBuffer->get_at( x, y ) );
			CHECK_INVARIANTS();
			return * this;
		}
		/**
		 *\~english
		 *\brief		Retrieves the pixel at given coordinates
		 *\param[in]	x,y		The pixel coordinates
		 *\param[out]	p_pixel	Receives the pixel
		 *\~french
		 *\brief		Récupère le pixel aux coordonnées données
		 *\param[in]	x,y		Les coordonnées du pixel
		 *\param[out]	p_pixel	Reçoit le pixel
		 */
		template< ePIXEL_FORMAT PF > void GetPixel( uint32_t x, uint32_t y, Pixel< PF > & p_pixel )const
		{
			CHECK_INVARIANTS();
			REQUIRE( x < m_pBuffer->width() && y < m_pBuffer->height() );
			ConvertPixel( GetPixelFormat(), m_pBuffer->get_at( x, y ), PF, p_pixel.ptr() );
			CHECK_INVARIANTS();
		}
		/**
		 *\~english
		 *\brief		Retrieves the wanted pixel and puts it in the buffer to the given format
		 *\param[in]	x,y			The pixel coordinates
		 *\param[out]	p_pPixel	Receives the pixel in given format
		 *\param[in]	p_eFormat	The wanted format for the pixel buffer
		 *\~french
		 *\brief		Récupère le pixel demandé et remplit le buffer au format donné
		 *\param[in]	x,y			Les coordonnées du pixel
		 *\param[out]	p_pPixel	Reçoit le pixel dans le format donné
		 *\param[in]	p_eFormat	Le format voulu pour le buffer
		 */
		void GetPixel( uint32_t x, uint32_t y, uint8_t * p_pPixel, ePIXEL_FORMAT p_eFormat )const;
		/**
		 *\~english
		 *\brief		Retrieves the pixel colour
		 *\param[in]	x,y			The pixel coordinates
		 *\return		The pixel colour
		 *\~french
		 *\brief		Récupère la couleur du pixel demandé
		 *\param[in]	x,y			Les coordonnées du pixel
		 *\return		La couleur du pixel
		 */
		Colour GetPixel( uint32_t x, uint32_t y )const;
		/**
		 *\~english
		 *\brief		Copies the given image into this one
		 *\param[in]	p_src	The image to copy
		 *\return		A reference to the image
		 *\~french
		 *\brief		Copie l'image donnée dans celle-ci
		 *\param[in]	p_src	L'image à copier
		 *\return		La référence de l'image
		 */
		Image & CopyImage( Image const & p_src );
		/**
		 *\~english
		 *\brief		Gives the sub-image contained in the given rectangle
		 *\param[in]	p_rcRect	The sub-image rectangle
		 *\return		The sub-image
		 *\~french
		 *\brief		Retourne la portion d'image contenue dans le rectangle donné
		 *\param[in]	p_rcRect	Le rectangle de la portion d'image
		 *\return		La portion d'image
		 */
		Image SubImage( Rectangle const & p_rcRect )const;
		/**
		 *\~english
		 *\brief		Swaps the image lines
		 *\return		A reference to the image
		 *\~french
		 *\brief		Retourne verticalement l'image
		 *\return		La référence de l'image
		 */
		Image & Flip();
		/**
		 *\~english
		 *\brief		Swaps the image columns
		 *\return		A reference to the image
		 *\~french
		 *\brief		Retourne horizontalement l'image
		 *\return		La référence de l'image
		 */
		Image & Mirror();
		/**
		 *\~english
		 *\brief		Retrieves the image dimensions
		 *\return		The image dimensions
		 *\~french
		 *\brief		Récupère les dimensions de l'image
		 *\return		Les dimensions de l'image
		 */
		inline Size const & GetDimensions()const
		{
			return m_pBuffer->dimensions();
		}
		/**
		 *\~english
		 *\brief		Retrieves the image data length
		 *\return		The image data length
		 *\~french
		 *\brief		Récupère la taille des données de l'image
		 *\return		La taille des données de l'image
		 */
		inline std::size_t GetSize()const
		{
			return m_pBuffer->size();
		}
		/**
		 *\~english
		 *\brief		Retrieves the image width
		 *\return		The image width
		 *\~french
		 *\brief		Récupère la largeur de l'image
		 *\return		La largeur de l'image
		 */
		inline uint32_t GetWidth()const
		{
			return m_pBuffer->width();
		}
		/**
		 *\~english
		 *\brief		Retrieves the image height
		 *\return		The image height
		 *\~french
		 *\brief		Récupère la hauteur de l'image
		 *\return		La hauteur de l'image
		 */
		inline uint32_t GetHeight()const
		{
			return m_pBuffer->height();
		}
		/**
		 *\~english
		 *\brief		Retrieves the image path
		 *\return		The image path
		 *\~french
		 *\brief		Récupère le chemin du fichier de l'image
		 *\return		Le chemin du fichier de l'image
		 */
		inline Path GetPath()const
		{
			return m_pathFile;
		}
		/**
		 *\~english
		 *\brief		Retrieves the image data
		 *\return		The image data
		 *\~french
		 *\brief		Récupère les données de l'image
		 *\return		Les données de l'image
		 */
		inline uint8_t * GetBuffer()
		{
			return m_pBuffer->ptr();
		}
		/**
		 *\~english
		 *\brief		Retrieves the image data
		 *\return		The image data
		 *\~french
		 *\brief		Récupère les données de l'image
		 *\return		Les données de l'image
		 */
		inline uint8_t const * GetBuffer()const
		{
			return m_pBuffer->const_ptr();
		}
		/**
		 *\~english
		 *\brief		Retrieves the image data
		 *\return		The image data
		 *\~french
		 *\brief		Récupère les données de l'image
		 *\return		Les données de l'image
		 */
		inline PxBufferBaseSPtr GetPixels()
		{
			return m_pBuffer;
		}
		/**
		 *\~english
		 *\brief		Retrieves the image data
		 *\return		The image data
		 *\~french
		 *\brief		Récupère les données de l'image
		 *\return		Les données de l'image
		 */
		inline PxBufferBaseSPtr GetPixels()const
		{
			return m_pBuffer;
		}
		/**
		 *\~english
		 *\brief		Retrieves the image pixel format
		 *\return		The image pixel format
		 *\~french
		 *\brief		Récupère le format des pixels de l'image
		 *\return		Le format des pixels de l'image
		 */
		inline ePIXEL_FORMAT GetPixelFormat()const
		{
			return m_pBuffer->format();
		}

	private:
		Path m_pathFile;
		PxBufferBaseSPtr m_pBuffer;
	};
}

#endif
