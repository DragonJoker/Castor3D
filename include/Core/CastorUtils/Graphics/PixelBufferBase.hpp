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
	class PxBufferBase
	{
	public:
		typedef std::vector< uint8_t > PxArray;
		typedef PxArray::iterator PixelData;
		typedef PxArray::const_iterator ConstPixelData;

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
		CU_API PxBufferBase( Size const & size
			, PixelFormat pixelFormat
			, uint32_t layers = 1u
			, uint32_t levels = 1u
			, uint8_t const * buffer = nullptr
			, PixelFormat bufferFormat = PixelFormat::eR8G8B8A8_UNORM
			, uint32_t bufferAlign = 0u );
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
		CU_API ~PxBufferBase();
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
		 *\param[in]	buffer			Data buffer.
		 *\param[in]	bufferFormat	Data buffer's pixels format.
		 *\param[in]	bufferAlign		Buffer data's alignment.
		 *\~french
		 *\brief		Initialise le buffer de données à celui donné
		 *\remarks		Des conversions sont faites si besoin est
		 *\param[in]	buffer			Buffer de données.
		 *\param[in]	bufferFormat	Format des pixels du buffer de données.
		 *\param[in]	bufferAlign		Alignement des données du buffer.
		 */
		CU_API void initialise( uint8_t const * buffer
			, PixelFormat bufferFormat
			, uint32_t bufferAlign = 0u );
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
		CU_API void update( uint32_t layers, uint32_t levels );
		/**
		 *\~english
		 *\brief		Creates a new buffer with same values as this one
		 *\return		The created buffer
		 *\~french
		 *\brief		Crée un nouveau buffer avec les mêmes valeurs
		 *\return		Le buffer créé
		 */
		inline std::shared_ptr< PxBufferBase > clone()const
		{
			return std::make_shared< PxBufferBase >( *this );
		}
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		CU_API PixelData getAt( uint32_t x
			, uint32_t y
			, uint32_t index = 0u
			, uint32_t level = 0u );
		CU_API ConstPixelData getAt( uint32_t x
			, uint32_t y
			, uint32_t index = 0u
			, uint32_t level = 0u )const;
		/**
		 *\~english
		 *\return		count() * (size of a pixel)
		 *\~french
		 *\return		count() * (size of a pixel)
		 */
		inline uint64_t getSize()const
		{
			return m_buffer.size();
		}

		inline uint8_t const * getConstPtr()const
		{
			return m_buffer.data();
		}

		inline uint8_t * getPtr()
		{
			return m_buffer.data();
		}

		inline bool isFlipped()const
		{
			return m_flipped;
		}
		
		inline PixelFormat getFormat()const
		{
			return m_format;
		}

		inline uint32_t getWidth()const
		{
			return m_size.getWidth();
		}

		inline uint32_t getHeight()const
		{
			return m_size.getHeight();
		}

		inline uint32_t getLayers()const
		{
			return m_layers;
		}

		inline uint32_t getLevels()const
		{
			return m_levels;
		}

		inline Size const & getDimensions()const
		{
			return m_size;
		}

		inline uint32_t getCount()const
		{
			return getWidth() * getHeight();
		}

		inline PixelData getAt( Position const & position )
		{
			return getAt( position.x(), position.y() );
		}

		inline ConstPixelData getAt( Position const & position )const
		{
			return getAt( position.x(), position.y() );
		}
		/**@}*/
		/**
		 *\~english
		 *\brief		Creates a buffer with the given data
		 *\param[in]	size			Buffer dimensions
		 *\param[in]	layers			Buffer layers (or slices)
		 *\param[in]	levels			Buffer mip levels
		 *\param[in]	wantedFormat	Pixels format
		 *\param[in]	buffer			Data buffer
		 *\param[in]	bufferFormat	Data buffer's pixels format
		 *\return		The created buffer
		 *\~french
		 *\brief		Crée un buffer avec les données voulues
		 *\param[in]	size			Dimensions du buffer
		 *\param[in]	layers			Couches du buffer (layers ou slices)
		 *\param[in]	levels			Niveaux de mip du buffer
		 *\param[in]	wantedFormat	Format des pixels du buffer
		 *\param[in]	buffer			Buffer de données
		 *\param[in]	bufferFormat	Format des pixels du buffer de données
		 *\return		Le buffer créé
		 */
		CU_API static PxBufferBaseSPtr create( Size const & size
			, uint32_t layers
			, uint32_t levels
			, PixelFormat wantedFormat
			, uint8_t const * buffer = nullptr
			, PixelFormat bufferFormat = PixelFormat::eR8G8B8A8_UNORM
			, uint32_t bufferAlign = 0u );
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
		inline static PxBufferBaseSPtr create( Size const & size
			, PixelFormat wantedFormat
			, uint8_t const * buffer = nullptr
			, PixelFormat bufferFormat = PixelFormat::eR8G8B8A8_UNORM
			, uint32_t bufferAlign = 0u )
		{
			return create( size
				, 1u
				, 1u
				, wantedFormat
				, buffer
				, bufferFormat
				, bufferAlign );
		}

	protected:
		inline uint32_t doConvert( uint32_t x, uint32_t y )const
		{
			return y * getWidth() + x;
		}

	private:
		PixelFormat m_format;
		bool m_flipped{ false };

	protected:
		Size m_size;
		uint32_t m_layers;
		uint32_t m_levels;
		mutable PxArray m_buffer;
	};
}

#endif
