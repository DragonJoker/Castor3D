/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_IMAGE_H___
#define ___CASTOR_IMAGE_H___

#include "CastorUtils/Data/BinaryLoader.hpp"
#include "CastorUtils/Data/BinaryWriter.hpp"
#include "CastorUtils/Design/Resource.hpp"
#include "CastorUtils/Graphics/Colour.hpp"
#include "CastorUtils/Graphics/PixelBuffer.hpp"

namespace castor
{
	class Image
		: public Resource< Image >
	{
	private:
		friend class Font;

		CU_DeclareInvariantBlock();

	public:
		/**
		 *\~english
		 *\brief		Creates the image with given params
		 *\param[in]	name			The resource name
		 *\param[in]	size			The wanted image dimensions
		 *\param[in]	format			The wanted image pixel format
		 *\param[in]	buffer			A buffer containing pixels
		 *\param[in]	bufferFormat	The buffer pixel format
		 *\~french
		 *\brief		Crée l'image avec les paramètres donnés
		 *\param[in]	name			Le nom de ressource
		 *\param[in]	size			Les dimensions voulues pour l'image
		 *\param[in]	format			Le format de pixel voulu pour l'image
		 *\param[in]	buffer			Un buffer de pixels
		 *\param[in]	bufferFormat	Le format des pixels du buffer
		 */
		CU_API Image( String const & name
			, Size const & size
			, PixelFormat format
			, ByteArray const & buffer
			, PixelFormat bufferFormat );
		/**
		 *\~english
		 *\brief		Creates the image with given params
		 *\param[in]	name			The resource name
		 *\param[in]	size			The wanted image dimensions
		 *\param[in]	format			The wanted image pixel format
		 *\param[in]	buffer			A buffer containing pixels
		 *\param[in]	bufferFormat	The buffer pixel format
		 *\~french
		 *\brief		Crée l'image avec les paramètres donnés
		 *\param[in]	name			Le nom de ressource
		 *\param[in]	size			Les dimensions voulues pour l'image
		 *\param[in]	format			Le format de pixel voulu pour l'image
		 *\param[in]	buffer			Un buffer de pixels
		 *\param[in]	bufferFormat	Le format des pixels du buffer
		 */
		CU_API Image( String const & name
			, Size const & size
			, PixelFormat format = PixelFormat::eR8G8B8A8_UNORM
			, uint8_t const * buffer = nullptr
			, PixelFormat bufferFormat = PixelFormat::eR8G8B8A8_UNORM );
		/**
		 *\~english
		 *\brief		Creates the image with given params
		 *\param[in]	name		The resource name
		 *\param[in]	buffer		A buffer containing pixels
		 *\~french
		 *\brief		Crée l'image avec les paramètres donnés
		 *\param[in]	name		Le nom de ressource
		 *\param[in]	buffer		Un buffer de pixels
		 */
		CU_API Image( String const & name
			, PxBufferBase const & buffer );
		/**
		 *\~english
		 *\brief		Creates the image with given params
		 *\param[in]	name	The resource name
		 *\param[in]	buffers	A buffer array containing the pixels for each mipmap level.
		 *\~french
		 *\brief		Crée l'image avec les paramètres donnés
		 *\param[in]	name	Le nom de ressource
		 *\param[in]	buffers	Un tableau de buffers contenant les pixels de chaque niveau de mipmap.
		 */
		CU_API Image( String const & name
			, PxBufferPtrArray const & buffers );
		/**
		 *\~english
		 *\brief		Creates the image with given params
		 *\remarks		The wanted pixel format and buffer pixel format are the template params
		 *\param[in]	name		The resource name
		 *\param[in]	size		The wanted image dimensions
		 *\param[in]	buffer		A buffer containing pixels
		 *\~french
		 *\brief		Crée l'image avec les paramètres donnés
		 *\remarks		Le format voulu ainsi que le format du buffer sont les arguments template
		 *\param[in]	name		Le nom de ressource
		 *\param[in]	size		Les dimensions voulues pour l'image
		 *\param[in]	buffer		Un buffer de pixels
		 */
		template< PixelFormat PFSrc, PixelFormat PFDst >
		Image( String const & name
			, Size const & size
			, uint8_t const * buffer = nullptr )
			: Resource< Image > ( name )
			, m_buffers( std::make_shared< PxBuffer< PFDst > >( size, buffer, PFSrc ) )
		{
			CU_CheckInvariants();
		}
		/**
		 *\~english
		 *\brief		Creates the image from a file.
		 *\param[in]	name		The resource name.
		 *\param[in]	pathFile	The file path.
		 *\param[in]	components	The components to keep for the image.
		 *\param[in]	loader		The image loader.
		 *\~french
		 *\brief		Crée l'image à partir d'un fichier.
		 *\param[in]	name		Le nom de ressource.
		 *\param[in]	pathFile	Le chemin du fichier.
		 *\param[in]	components	Les composantes à garder pour l'image.
		 *\param[in]	loader		Le loader d'image.
		 */
		CU_API Image( String const & name
			, Path const & pathFile
			, ImageLoader const & loader );
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	image	The Image object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	image	L'objet Image à copier
		 */
		CU_API Image( Image const & image );
		/**
		 *\~english
		 *\brief		Move Constructor
		 *\param[in]	image	The Image object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	image	L'objet Image à déplacer
		 */
		CU_API Image( Image && image ) = default;
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	image	The Image object to copy
		 *\return		A reference to this Image object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	image	L'objet Image à copier
		 *\return		Une référence sur cet objet Image
		 */
		CU_API Image & operator=( Image const & image );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	image	The Image object to move
		 *\return		A reference to this Image object
		 *\~french
		 *\brief		Opérateur d'affectation  par déplacement
		 *\param[in]	image	L'objet Image à déplacer
		 *\return		Une référence sur cet objet Image
		 */
		CU_API Image & operator=( Image && image ) = default;
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		CU_API ~Image();
		/**
		 *\~english
		 *\brief		Resizes the image to the given resolution
		 *\param[in]	size	The new resolution
		 *\return		A reference to the image
		 *\~french
		 *\brief		Redimensionne l'image à la résolution donnée
		 *\param[in]	size	La nouvelle résolution
		 *\return		La référence de l'image
		 */
		CU_API Image & resample( Size const & size );
		/**
		 *\~english
		 *\brief		Fills all image pixels with the given colour
		 *\param[in]	colour	The fill colour
		 *\return		A reference to the image
		 *\~french
		 *\brief		Remplit tous les pixels de l'image avec la couleur donnée
		 *\param[in]	colour	La couleur de remplissage
		 *\return		La référence de l'image
		 */
		CU_API Image & fill( RgbColour const & colour );
		/**
		 *\~english
		 *\brief		Fills all image pixels with the given colour
		 *\param[in]	colour	The fill colour
		 *\return		A reference to the image
		 *\~french
		 *\brief		Remplit tous les pixels de l'image avec la couleur donnée
		 *\param[in]	colour	La couleur de remplissage
		 *\return		La référence de l'image
		 */
		CU_API Image & fill( RgbaColour const & colour );
		/**
		 *\~english
		 *\brief		Fills the wanted pixel with the given buffer
		 *\param[in]	x,y		The pixel coordinates
		 *\param[in]	pixel	The buffer
		 *\param[in]	format	The buffer's pixel format
		 *\return		A reference to the image
		 *\~french
		 *\brief		Remplit le pixel demandé avec le buffer donné
		 *\param[in]	x,y		Les coordonnées du pixel
		 *\param[in]	pixel	Le buffer
		 *\param[in]	format	Le format de pixel du tampon
		 *\return		La référence de l'image
		 */
		CU_API Image & setPixel( uint32_t x
			, uint32_t y
			, uint8_t const * pixel
			, PixelFormat format );
		/**
		 *\~english
		 *\brief		sets the colour of the wanted pixel to the given one
		 *\param[in]	x,y		The pixel coordinates
		 *\param[in]	colour	The colour
		 *\return		A reference to the image
		 *\~french
		 *\brief		Définit la couleur du pixel demandé
		 *\param[in]	x,y		Les coordonnées du pixel
		 *\param[in]	colour	La couleur
		 *\return		La référence de l'image
		 */
		CU_API Image & setPixel( uint32_t x
			, uint32_t y
			, RgbColour const & colour );
		/**
		 *\~english
		 *\brief		sets the colour of the wanted pixel to the given one
		 *\param[in]	x,y			The pixel coordinates
		 *\param[in]	colour	The colour
		 *\return		A reference to the image
		 *\~french
		 *\brief		Définit la couleur du pixel demandé
		 *\param[in]	x,y			Les coordonnées du pixel
		 *\param[in]	colour	La couleur
		 *\return		La référence de l'image
		 */
		CU_API Image & setPixel( uint32_t x
			, uint32_t y
			, RgbaColour const & colour );
		/**
		 *\~english
		 *\brief		set the wanted pixel to the given pixel
		 *\param[in]	x,y		The pixel coordinates
		 *\param[in]	pixel	The pixel
		 *\return		A reference to the image
		 *\~french
		 *\brief		Définit le pixel demandé avec le pixel donné
		 *\param[in]	x,y		Les coordonnées du pixel
		 *\param[in]	pixel	Le pixel
		 *\return		La référence de l'image
		 */
		template< PixelFormat PF >
		Image & setPixel( uint32_t x
			, uint32_t y
			, Pixel< PF > const & pixel )
		{
			CU_CheckInvariants();
			CU_Require( x < getWidth() && y < getHeight() );
			convertPixel( PF, pixel.constPtr(), getPixelFormat(), m_buffers.front()->getAt( x, y ) );
			CU_CheckInvariants();
			return * this;
		}
		/**
		 *\~english
		 *\brief		Retrieves the pixel at given coordinates
		 *\param[in]	x,y		The pixel coordinates
		 *\param[out]	pixel	Receives the pixel
		 *\~french
		 *\brief		Récupère le pixel aux coordonnées données
		 *\param[in]	x,y		Les coordonnées du pixel
		 *\param[out]	pixel	Reçoit le pixel
		 */
		template< PixelFormat PF >
		void getPixel( uint32_t x
			, uint32_t y
			, Pixel< PF > & pixel )const
		{
			CU_CheckInvariants();
			CU_Require( x < getWidth() && y < getHeight() );
			convertPixel( getPixelFormat(), m_buffers.front()->getAt( x, y ), PF, pixel.ptr() );
			CU_CheckInvariants();
		}
		/**
		 *\~english
		 *\brief		Retrieves the wanted pixel and puts it in the buffer to the given format
		 *\param[in]	x,y		The pixel coordinates
		 *\param[out]	pixel	Receives the pixel in given format
		 *\param[in]	format	The wanted format for the pixel buffer
		 *\~french
		 *\brief		Récupère le pixel demandé et remplit le buffer au format donné
		 *\param[in]	x,y		Les coordonnées du pixel
		 *\param[out]	pixel	Reçoit le pixel dans le format donné
		 *\param[in]	format	Le format voulu pour le buffer
		 */
		CU_API void getPixel( uint32_t x
			, uint32_t y
			, uint8_t * pixel
			, PixelFormat format )const;
		/**
		 *\~english
		 *\brief		Retrieves the pixel colour
		 *\param[in]	x,y	The pixel coordinates
		 *\return		The pixel colour
		 *\~french
		 *\brief		Récupère la couleur du pixel demandé
		 *\param[in]	x,y	Les coordonnées du pixel
		 *\return		La couleur du pixel
		 */
		CU_API RgbaColour getPixel( uint32_t x, uint32_t y )const;
		/**
		 *\~english
		 *\brief		Copies the given image into this one
		 *\param[in]	src	The image to copy
		 *\return		A reference to the image
		 *\~french
		 *\brief		Copie l'image donnée dans celle-ci
		 *\param[in]	src	L'image à copier
		 *\return		La référence de l'image
		 */
		CU_API Image & copyImage( Image const & src );
		/**
		 *\~english
		 *\brief		Gives the sub-image contained in the given rectangle
		 *\param[in]	rect	The sub-image rectangle
		 *\return		The sub-image
		 *\~french
		 *\brief		Retourne la portion d'image contenue dans le rectangle donné
		 *\param[in]	rect	Le rectangle de la portion d'image
		 *\return		La portion d'image
		 */
		CU_API Image subImage( Rectangle const & rect )const;
		/**
		 *\~english
		 *\brief		Swaps the image lines
		 *\return		A reference to the image
		 *\~french
		 *\brief		Retourne verticalement l'image
		 *\return		La référence de l'image
		 */
		CU_API Image & flip();
		/**
		 *\~english
		 *\brief		Swaps the image columns
		 *\return		A reference to the image
		 *\~french
		 *\brief		Retourne horizontalement l'image
		 *\return		La référence de l'image
		 */
		CU_API Image & mirror();
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		inline uint32_t getLevels()const
		{
			return uint32_t( m_buffers.size() );
		}
		
		inline Size const & getDimensions()const
		{
			return m_buffers.front()->getDimensions();
		}

		inline std::size_t getSize( uint32_t level = 0u )const
		{
			return m_buffers[level]->getSize();
		}

		inline uint32_t getWidth()const
		{
			return getDimensions().getWidth();
		}

		inline uint32_t getHeight()const
		{
			return getDimensions().getHeight();
		}

		inline Path getPath()const
		{
			return m_pathFile;
		}

		inline uint8_t * getBuffer( uint32_t level = 0u )
		{
			return m_buffers[level]->getPtr();
		}

		inline uint8_t const * getBuffer( uint32_t level = 0u )const
		{
			return m_buffers[level]->getConstPtr();
		}

		inline PxBufferBaseSPtr getPixels( uint32_t level = 0u )const
		{
			return m_buffers[level];
		}

		inline PxBufferPtrArray const & getBuffers()const
		{
			return m_buffers;
		}

		inline PxBufferPtrArray & getBuffers()
		{
			return m_buffers;
		}

		inline PixelFormat getPixelFormat()const
		{
			return m_buffers.front()->getFormat();
		}
		/**@}*/

	private:
		Path m_pathFile;
		PxBufferPtrArray m_buffers;
	};
}

#endif
