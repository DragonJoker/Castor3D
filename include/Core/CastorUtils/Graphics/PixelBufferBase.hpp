/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_PIXEL_BUFFER_BASE_H___
#define ___CASTOR_PIXEL_BUFFER_BASE_H___

#include "CastorUtils/Graphics/Pixel.hpp"
#include "CastorUtils/Graphics/Size.hpp"
#include "CastorUtils/Graphics/Position.hpp"

namespace castor
{
	/**
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
		typedef px_array::iterator pixel_data;
		typedef px_array::const_iterator const_pixel_data;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	size		Buffer dimensions.
		 *\param[in]	pixelFormat	Pixels format.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	size		Dimensions du buffer.
		 *\param[in]	pixelFormat	Format des pixels du buffer.
		 */
		CU_API PxBufferBase( Size const & size, PixelFormat pixelFormat );
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	pixelBuffer	The PxBufferBase object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	pixelBuffer	L'objet PxBufferBase à copier
		 */
		CU_API PxBufferBase( PxBufferBase const & pixelBuffer );
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
		 *\param[in]	pixelBuffer	The PxBufferBase object to copy
		 *\return		A reference to this PxBufferBase object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	pixelBuffer	L'objet PxBufferBase à copier
		 *\return		Une référence sur cet objet PxBufferBase
		 */
		CU_API PxBufferBase & operator=( PxBufferBase const & pixelBuffer );
		/**
		 *\brief		Deletes the data buffer
		 *\~french
		 *\brief		Détruit le tampon de données
		 */
		CU_API void clear();
		/**
		 *\~english
		 *\brief		Initialises the data buffer to the given one
		 *\remarks		Conversions are made if needed
		 *\param[in]	buffer		Data buffer
		 *\param[in]	pixelFormat	Data buffer's pixels format
		 *\~french
		 *\brief		Initialise le buffer de données à celui donné
		 *\remarks		Des conversions sont faites si besoin est
		 *\param[in]	buffer		Buffer de données
		 *\param[in]	pixelFormat	Format des pixels du buffer de données
		 */
		CU_API void initialise( uint8_t const * buffer, PixelFormat pixelFormat );
		/**
		 *\~english
		 *\brief		Initialises the data buffer at the given size
		 *\remarks		Conversions are made if needed
		 *\param[in]	size	Buffer dimensions
		 *\~french
		 *\brief		Initialise le buffer de données à la taille donnée
		 *\remarks		Des conversions sont faites si besoin est
		 *\param[in]	size	Les dimensions du buffer
		 */
		CU_API void initialise( Size const & size );
		/**
		 *\~english
		 *\brief		Makes a vertical swap of pixels
		 *\~french
		 *\brief		Effectue un échange vertical des pixels
		 */
		CU_API void flip();
		/**
		 *\~english
		 *\brief		Swaps this buffer's data with the given one's
		 *\param[in]	pixelBuffer	The buffer to swap
		 *\~french
		 *\brief		Echange les données de ce buffer avec celles du buffer donné
		 *\param[in]	pixelBuffer	Le buffer à échanger
		 */
		CU_API void swap( PxBufferBase & pixelBuffer );
		/**
		 *\~english
		 *\brief		Converts and assigns a data buffer to this buffer
		 *\param[in]	buffer		Data buffer
		 *\param[in]	pixelFormat	Data buffer's pixels format
		 *\return
		 *\~french
		 *\brief		Convertit et assigne les données du buffer donné à ce buffer
		 *\param[in]	buffer		Buffer de données
		 *\param[in]	pixelFormat	Format des pixels du buffer de données
		 *\return
		 */
		CU_API virtual void assign( std::vector< uint8_t > const & buffer, PixelFormat pixelFormat ) = 0;
		/**
		 *\~english
		 *\brief		Creates a new buffer with same values as this one
		 *\return		The created buffer
		 *\~french
		 *\brief		Crée un nouveau buffer avec les mêmes valeurs
		 *\return		Le buffer créé
		 */
		CU_API virtual std::shared_ptr< PxBufferBase > clone()const = 0;
		/**
		 *\~english
		 *\brief		Makes a horizontal swap of pixels
		 *\~french
		 *\brief		Effectue un échange horizontal des pixels
		 */
		CU_API virtual void mirror() = 0;
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		/**
		 *\~english
		 *\return		count() * (size of a pixel)
		 *\~french
		 *\return		count() * (size of a pixel)
		 */
		CU_API virtual uint32_t getSize()const = 0;
		CU_API virtual uint8_t const * getConstPtr()const = 0;
		CU_API virtual uint8_t * getPtr() = 0;
		CU_API virtual pixel_data getAt( uint32_t x, uint32_t y ) = 0;
		CU_API virtual const_pixel_data getAt( uint32_t x, uint32_t y )const = 0;

		inline bool isFlipped()const
		{
			return m_flipped;
		}
		
		inline PixelFormat getFormat()const
		{
			return m_pixelFormat;
		}

		inline uint32_t getWidth()const
		{
			return m_size.getWidth();
		}

		inline uint32_t getHeight()const
		{
			return m_size.getHeight();
		}

		inline Size const & getDimensions()const
		{
			return m_size;
		}

		inline uint32_t getCount()const
		{
			return getWidth() * getHeight();
		}

		inline pixel_data getAt( Position const & position )
		{
			return getAt( position.x(), position.y() );
		}

		inline const_pixel_data getAt( Position const & position )const
		{
			return getAt( position.x(), position.y() );
		}
		/**@}*/
		/**
		 *\~english
		 *\brief		Creates a buffer with the given data
		 *\param[in]	size			Buffer dimensions
		 *\param[in]	wantedFormat	Pixels format
		 *\param[in]	buffer			Data buffer
		 *\param[in]	bufferFormat	Data buffer's pixels format
		 *\return		The created buffer
		 *\~french
		 *\brief		Crée un buffer avec les données voulues
		 *\param[in]	size			Dimensions du buffer
		 *\param[in]	wantedFormat	Format des pixels du buffer
		 *\param[in]	buffer			Buffer de données
		 *\param[in]	bufferFormat	Format des pixels du buffer de données
		 *\return		Le buffer créé
		 */
		CU_API static PxBufferBaseSPtr create( Size const & size
			, PixelFormat wantedFormat
			, uint8_t const * buffer = nullptr
			, PixelFormat bufferFormat = PixelFormat::eR8G8B8A8_UNORM );

	private:
		PixelFormat m_pixelFormat;
		bool m_flipped{ false };

	protected:
		//!\~english	Buffer dimensions.
		//!\~french		Dimensions du buffer
		Size m_size;
		//!\~english	Buffer data.
		//!\~french		Données du buffer.
		px_array m_buffer;
	};
}

#endif
