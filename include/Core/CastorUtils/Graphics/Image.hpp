/*
See LICENSE file in root folder
*/
#ifndef ___CU_Image_HPP___
#define ___CU_Image_HPP___

#include "CastorUtils/Data/BinaryLoader.hpp"
#include "CastorUtils/Data/BinaryWriter.hpp"
#include "CastorUtils/Data/Path.hpp"
#include "CastorUtils/Design/Resource.hpp"
#include "CastorUtils/Graphics/Colour.hpp"
#include "CastorUtils/Graphics/PixelBuffer.hpp"
#include "CastorUtils/Graphics/ImageLayout.hpp"

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
		 *\name		Construction/Destruction.
		 */
		//@{
		CU_API Image( String const & name
			, Path const & path
			, Size  const & size
			, PixelFormat format
			, ByteArray const & buffer
			, PixelFormat bufferFormat );
		CU_API Image( String const & name
			, Path const & path
			, Size const & size
			, PixelFormat format = PixelFormat::eR8G8B8A8_UNORM
			, uint8_t const * buffer = nullptr
			, PixelFormat bufferFormat = PixelFormat::eR8G8B8A8_UNORM );
		CU_API Image( String const & name
			, Path const & path
			, PxBufferBase const & buffer );
		CU_API Image( String const & name
			, Path const & path
			, ImageLayout layout
			, PxBufferBaseSPtr buffer = nullptr );
		template< PixelFormat PFSrc, PixelFormat PFDst >
		Image( String const & name
			, Path const & path
			, Size const & size
			, uint8_t const * buffer = nullptr )
			: Resource< Image >( std::move( name ) )
			, m_buffer( std::make_shared< PxBuffer< PFDst > >( size, buffer, PFSrc ) )
			, m_pathFile{ std::move( path ) }
		{
			CU_CheckInvariants();
		}

		Image( String const & name
			, Size const & size
			, PixelFormat format
			, uint8_t const * buffer
			, PixelFormat bufferFormat )
			: Image{ name, {}, size, format, buffer, bufferFormat }
		{
		}

		Image( String const & name
			, Size const & size
			, PixelFormat format
			, ByteArray const & buffer
			, PixelFormat bufferFormat )
			: Image{ name, {}, size, format, buffer, bufferFormat }
		{
		}

		Image( String const & name
			, PxBufferBase const & buffer )
			: Image{ name, {}, buffer }
		{
		}

		Image( String const & name
			, ImageLayout layout
			, PxBufferBaseSPtr buffer )
			: Image{ name, {}, layout, std::move( buffer ) }
		{
		}

		CU_API ~Image() = default;
		//@}
		/**
		 *\name		Copy/Move.
		 */
		//@{
		CU_API Image( Image const & image );
		CU_API Image( Image && image ) = default;
		CU_API Image & operator=( Image const & image );
		CU_API Image & operator=( Image && image ) = default;
		//@}
		/**
		 *\name		Modification.
		 */
		//@{
		CU_API PxBufferBaseSPtr updateLayerLayout( Size const & extent
			, PixelFormat format );
		//@}
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
		//@}
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
		CU_API Image getResampled( Size const & size )const
		{
			auto result = *this;
			result.resample( size );
			return result;
		}
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
			convertPixel( PF, pixel.constPtr(), getPixelFormat(), m_buffer->getAt( x, y ) );
			CU_CheckInvariants();
			return * this;
		}
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
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		inline uint32_t getLevels()const
		{
			return m_layout.levels;
		}
		
		inline Size getDimensions()const
		{
			return m_layout.dimensions();
		}

		inline std::size_t getSize( uint32_t level = 0u )const
		{
			return std::size_t( m_layout.layerMipSize( level ) );
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

		inline castor::ImageLayout::Buffer getBuffer()
		{
			CU_Require( hasBuffer() );
			return castor::makeArrayView( m_buffer->getPtr()
				, m_buffer->getPtr() + m_buffer->getSize() );
		}

		inline castor::ImageLayout::ConstBuffer getBuffer()const
		{
			CU_Require( hasBuffer() );
			return castor::makeArrayView( m_buffer->getConstPtr()
				, m_buffer->getConstPtr() + m_buffer->getSize() );
		}

		inline castor::ImageLayout::Buffer getBuffer( uint32_t index )
		{
			CU_Require( hasBuffer() );
			return getLayerBuffer( m_layout, *m_buffer, index );
		}

		inline castor::ImageLayout::ConstBuffer getBuffer( uint32_t index )const
		{
			CU_Require( hasBuffer() );
			auto const & buffer = *m_buffer;
			return getLayerBuffer( m_layout, buffer, index );
		}

		inline castor::ImageLayout::Buffer getBuffer( uint32_t index
			, uint32_t level )
		{
			CU_Require( hasBuffer() );
			return getLayerMipBuffer( m_layout, *m_buffer, index, level );
		}

		inline castor::ImageLayout::ConstBuffer getBuffer( uint32_t index
			, uint32_t level )const
		{
			CU_Require( hasBuffer() );
			auto const & buffer = *m_buffer;
			return getLayerMipBuffer( m_layout, buffer, index, level );
		}

		inline bool hasBuffer()const
		{
			return m_buffer != nullptr;
		}

		inline PxBufferBase const & getPxBuffer()const
		{
			CU_Require( hasBuffer() );
			return *m_buffer;
		}

		inline PxBufferBase & getPxBuffer()
		{
			CU_Require( hasBuffer() );
			return *m_buffer;
		}

		inline PxBufferBaseSPtr getPixels()const
		{
			return m_buffer;
		}

		inline ImageLayout const & getLayout()const
		{
			return m_layout;
		}

		inline ImageLayout & getLayout()
		{
			return m_layout;
		}

		inline PixelFormat getPixelFormat()const
		{
			return m_layout.format;
		}
		/**@}*/

	private:
		Path m_pathFile;
		PxBufferBaseSPtr m_buffer;
		ImageLayout m_layout;
	};
}

#endif
