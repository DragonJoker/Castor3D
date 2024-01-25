/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_PIXEL_BUFFER_BASE_H___
#define ___CASTOR_PIXEL_BUFFER_BASE_H___

#include "CastorUtils/Design/NonCopyable.hpp"
#include "CastorUtils/Graphics/Pixel.hpp"
#include "CastorUtils/Graphics/Size.hpp"
#include "CastorUtils/Graphics/Position.hpp"
#include "CastorUtils/Math/Point.hpp"

#include "CastorUtils/Config/BeginExternHeaderGuard.hpp"
#include <atomic>
#include "CastorUtils/Config/EndExternHeaderGuard.hpp"

namespace castor
{
	struct PxCompressionSupport
	{
		bool supportBC1{ false };
		bool supportBC3{ false };
		bool supportBC5{ false };
		bool supportBC6{ false };
		bool supportBC7{ false };
	};

	struct PxBufferConvertOptions
		: public NonMovable
	{
		CU_API explicit PxBufferConvertOptions( PxCompressionSupport support );
		CU_API ~PxBufferConvertOptions()noexcept;

		CU_API PixelFormat getCompressed( PixelFormat format )const;
		CU_API uint32_t getAdditionalAlign( PixelFormat format )const;

		PxCompressionSupport support;
		void * additionalOptions{ nullptr };
	};

	class PxBufferBase
	{
	public:
		using PxArray = Vector< uint8_t >;
		using PixelData = PxArray::iterator;
		using ConstPixelData = PxArray::const_iterator;

	public:
		/**
		 *\~english
		 *\brief		Creates a buffer from a source buffer, uninitialised data if no source is given.
		 *\param[in]	options			The convertion options.
		 *\param[in]	interrupt		Tells if the convertion is to be interrupted.
		 *\param[in]	size			Buffer dimensions.
		 *\param[in]	pixelFormat		Pixels format.
		 *\param[in]	layers			The number of layers stored in the buffer.
		 *\param[in]	levels			The number of miplevels stored in the buffer.
		 *\param[in]	buffer			The source buffer.
		 *\param[in]	bufferFormat	The pixels format of the source buffer.
		 *\param[in]	bufferAlign		The alignment of the source buffer.
		 *\~french
		 *\brief		Crée un buffer depuis une source, données initialisées si aucune source n'est donnée.
		 *\param[in]	options			Les options de conversion.
		 *\param[in]	interrupt		Dit si la conversion est à interrompre.
		 *\param[in]	size			Dimensions du buffer.
		 *\param[in]	pixelFormat		Format des pixels du buffer.
		 *\param[in]	layers			Le nombre de layers du buffer.
		 *\param[in]	levels			Le nombre de miplevels du buffer.
		 *\param[in]	buffer			Le buffer source.
		 *\param[in]	bufferFormat	Le format des pixels du buffer source.
		 *\param[in]	bufferAlign		L'alignement mémoire du buffer source.
		 */
		CU_API PxBufferBase( PxBufferConvertOptions const * options
			, std::atomic_bool const * interrupt
			, Size const & size
			, PixelFormat pixelFormat
			, uint32_t layers = 1u
			, uint32_t levels = 1u
			, uint8_t const * buffer = nullptr
			, PixelFormat bufferFormat = PixelFormat::eR8G8B8A8_UNORM
			, uint32_t bufferAlign = 0u );
		/**
		 *\~english
		 *\brief		Creates a buffer from a source buffer, uninitialised data if no source is given.
		 *\param[in]	size			Buffer dimensions.
		 *\param[in]	pixelFormat		Pixels format.
		 *\param[in]	layers			The number of layers stored in the buffer.
		 *\param[in]	levels			The number of miplevels stored in the buffer.
		 *\param[in]	buffer			The source buffer.
		 *\param[in]	bufferFormat	The pixels format of the source buffer.
		 *\param[in]	bufferAlign		The alignment of the source buffer.
		 *\~french
		 *\brief		Crée un buffer depuis une source, données initialisées si aucune source n'est donnée. 
		 *\param[in]	size			Dimensions du buffer.
		 *\param[in]	pixelFormat		Format des pixels du buffer.
		 *\param[in]	layers			Le nombre de layers du buffer.
		 *\param[in]	levels			Le nombre de miplevels du buffer.
		 *\param[in]	buffer			Le buffer source.
		 *\param[in]	bufferFormat	Le format des pixels du buffer source.
		 *\param[in]	bufferAlign		L'alignement mémoire du buffer source.
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
		 *\brief		Move Constructor
		 *\param[in]	pixelBuffer	The PxBufferBase object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	pixelBuffer	L'objet PxBufferBase à déplacer
		 */
		CU_API PxBufferBase( PxBufferBase && pixelBuffer )noexcept;
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		CU_API ~PxBufferBase()noexcept = default;
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
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	pixelBuffer	The PxBufferBase object to move
		 *\return		A reference to this PxBufferBase object
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement
		 *\param[in]	pixelBuffer	L'objet PxBufferBase à déplacer
		 *\return		Une référence sur cet objet PxBufferBase
		 */
		CU_API PxBufferBase & operator=( PxBufferBase && pixelBuffer )noexcept;
		/**
		 *\brief		Deletes the data buffer
		 *\~french
		 *\brief		Détruit le tampon de données
		 */
		CU_API void clear();
		/**
		 *\~english
		 *\brief		Initialises the data buffer to the given one.
		 *\remarks		Conversions are made if needed.
		 *\param[in]	options			The convertion options.
		 *\param[in]	interrupt		Tells if the convertion is to be interrupted.
		 *\param[in]	buffer			Data buffer.
		 *\param[in]	bufferFormat	Data buffer's pixels format.
		 *\param[in]	bufferAlign		Buffer data's alignment.
		 *\~french
		 *\brief		Initialise le buffer de données à celui donné.
		 *\remarks		Des conversions sont faites si besoin est.
		 *\param[in]	options			Les options de conversion.
		 *\param[in]	interrupt		Dit si la conversion est à interrompre.
		 *\param[in]	buffer			Buffer de données.
		 *\param[in]	bufferFormat	Format des pixels du buffer de données.
		 *\param[in]	bufferAlign		Alignement des données du buffer.
		 */
		CU_API void initialise( PxBufferConvertOptions const * options
			, std::atomic_bool const * interrupt
			, uint8_t const * buffer
			, PixelFormat bufferFormat
			, uint32_t bufferAlign = 0u );
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
		void initialise( uint8_t const * buffer
			, PixelFormat bufferFormat
			, uint32_t bufferAlign = 0u )
		{
			initialise( nullptr, nullptr, buffer, bufferFormat, bufferAlign );
		}
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
		CU_API void swap( PxBufferBase & pixelBuffer )noexcept;
		/**
		 *\~english
		 *\brief		Updates the buffer container, with given layers and miplevels counts.
		 *\param[in]	layers	The number of layers stored in the buffer.
		 *\param[in]	levels	The number of miplevels stored in the buffer.
		 *\~french
		 *\brief		Met ç jour le conteneur du buffer, avec les nombres de layers et de miplevels donnés.
		 *\param[in]	layers	Le nombre de layers du buffer.
		 *\param[in]	levels	Le nombre de miplevels du buffer.
		 */
		CU_API void update( uint32_t layers, uint32_t levels );
		/**
		 *\~english
		 *\brief		Generate mipmaps.
		 *\~french
		 *\brief		Génère les mipmaps.
		 */
		CU_API void generateMips();
		/**
		 *\~english
		 *\brief		Convert to tiles map (no effect if m_layers <= 1).
		 *\~french
		 *\brief		Convvertit en tile map (aucun effet si m_layers <= 1).
		 */
		CU_API uint32_t convertToTiles( uint32_t maxSize );
		/**
		 *\~english
		 *\return		A clone of this buffer.
		 *\~french
		 *\return		Un clone de ce buffer.
		 */
		PxBufferBaseUPtr clone()const
		{
			return castor::makeUnique< PxBufferBase >( *this );
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
		uint64_t getSize()const noexcept
		{
			return m_buffer.size();
		}

		uint8_t const * begin()const noexcept
		{
			return m_buffer.data();
		}

		uint8_t const * end()const noexcept
		{
			return begin() + getSize();
		}

		uint8_t const * cbegin()const noexcept
		{
			return m_buffer.data();
		}

		uint8_t const * cend()const noexcept
		{
			return cbegin() + getSize();
		}

		uint8_t const * getConstPtr()const noexcept
		{
			return m_buffer.data();
		}

		uint8_t * getPtr()noexcept
		{
			return m_buffer.data();
		}

		bool isFlipped()const noexcept
		{
			return m_flipped;
		}
		
		PixelFormat getFormat()const noexcept
		{
			return m_format;
		}

		uint32_t getWidth()const noexcept
		{
			return m_size.getWidth();
		}

		uint32_t getHeight()const noexcept
		{
			return m_size.getHeight();
		}

		Point3ui getTiles()const noexcept
		{
			return m_tiles;
		}

		Size getTileSize()const noexcept
		{
			return { getTileWidth(), getTileHeight() };
		}

		uint32_t getTileWidth()const noexcept
		{
			return m_size.getWidth() / m_tiles->x;
		}

		uint32_t getTileHeight()const noexcept
		{
			return m_size.getHeight() / m_tiles->y;
		}

		uint32_t getLayers()const noexcept
		{
			return m_layers;
		}

		uint32_t getLevels()const noexcept
		{
			return m_levels;
		}

		uint32_t getAlign()const noexcept
		{
			return m_align;
		}

		Size const & getDimensions()const noexcept
		{
			return m_size;
		}

		uint32_t getCount()const noexcept
		{
			return getWidth() * getHeight();
		}

		PixelData getAt( Position const & position )
		{
			return getAt( uint32_t( position.x() ), uint32_t( position.y() ) );
		}

		ConstPixelData getAt( Position const & position )const
		{
			return getAt( uint32_t( position.x() ), uint32_t( position.y() ) );
		}
		/**@}*/
		/**
		 *\~english
		 *\brief		Creates a buffer with the given data.
		 *\remarks		Interruptible version.
		 *\param[in]	options			The convertion options.
		 *\param[in]	interrupt		Tells if the convertion is to be interrupted.
		 *\param[in]	size			Buffer dimensions.
		 *\param[in]	layers			Buffer layers (or slices).
		 *\param[in]	levels			Buffer mip levels.
		 *\param[in]	wantedFormat	Pixels format.
		 *\param[in]	buffer			Data buffer.
		 *\param[in]	bufferFormat	Data buffer's pixels format.
		 *\param[in]	bufferAlign		The alignment of the source buffer.
		 *\return		The created buffer.
		 *\~french
		 *\brief		Crée un buffer avec les données voulues.
		 *\remarks		Version interruptible.
		 *\param[in]	options			Les options de conversion.
		 *\param[in]	interrupt		Dit si la conversion est à interrompre.
		 *\param[in]	size			Dimensions du buffer.
		 *\param[in]	layers			Couches du buffer (layers ou slices).
		 *\param[in]	levels			Niveaux de mip du buffer.
		 *\param[in]	wantedFormat	Format des pixels du buffer.
		 *\param[in]	buffer			Buffer de données.
		 *\param[in]	bufferFormat	Format des pixels du buffer de données.
		 *\param[in]	bufferAlign		L'alignement mémoire du buffer source.
		 *\return		Le buffer créé.
		 */
		CU_API static PxBufferBaseUPtr create( PxBufferConvertOptions const * options
			, std::atomic_bool const * interrupt
			, Size const & size
			, uint32_t layers
			, uint32_t levels
			, PixelFormat wantedFormat
			, uint8_t const * buffer = nullptr
			, PixelFormat bufferFormat = PixelFormat::eR8G8B8A8_UNORM
			, uint32_t bufferAlign = 0u );
		/**
		 *\~english
		 *\brief		Creates a buffer with the given data.
		 *\param[in]	options			The convertion options.
		 *\param[in]	size			Buffer dimensions.
		 *\param[in]	layers			Buffer layers (or slices).
		 *\param[in]	levels			Buffer mip levels.
		 *\param[in]	wantedFormat	Pixels format.
		 *\param[in]	buffer			Data buffer.
		 *\param[in]	bufferFormat	Data buffer's pixels format.
		 *\param[in]	bufferAlign		The alignment of the source buffer.
		 *\return		The created buffer.
		 *\~french
		 *\brief		Crée un buffer avec les données voulues.
		 *\param[in]	options			Les options de conversion.
		 *\param[in]	size			Dimensions du buffer.
		 *\param[in]	layers			Couches du buffer (layers ou slices).
		 *\param[in]	levels			Niveaux de mip du buffer.
		 *\param[in]	wantedFormat	Format des pixels du buffer.
		 *\param[in]	buffer			Buffer de données.
		 *\param[in]	bufferFormat	Format des pixels du buffer de données.
		 *\param[in]	bufferAlign		L'alignement mémoire du buffer source.
		 *\return		Le buffer créé.
		 */
		CU_API static PxBufferBaseUPtr create( PxBufferConvertOptions const * options
			, Size const & size
			, uint32_t layers
			, uint32_t levels
			, PixelFormat wantedFormat
			, uint8_t const * buffer = nullptr
			, PixelFormat bufferFormat = PixelFormat::eR8G8B8A8_UNORM
			, uint32_t bufferAlign = 0u )
		{
			return create( options
				, nullptr
				, size
				, layers
				, levels
				, wantedFormat
				, buffer
				, bufferFormat
				, bufferAlign );
		}
		/**
		 *\~english
		 *\brief		Creates a buffer with the given data.
		 *\param[in]	size			Buffer dimensions.
		 *\param[in]	layers			Buffer layers (or slices).
		 *\param[in]	levels			Buffer mip levels.
		 *\param[in]	wantedFormat	Pixels format.
		 *\param[in]	buffer			Data buffer.
		 *\param[in]	bufferFormat	Data buffer's pixels format.
		 *\param[in]	bufferAlign		The alignment of the source buffer.
		 *\return		The created buffer.
		 *\~french
		 *\brief		Crée un buffer avec les données voulues.
		 *\param[in]	size			Dimensions du buffer.
		 *\param[in]	layers			Couches du buffer (layers ou slices).
		 *\param[in]	levels			Niveaux de mip du buffer.
		 *\param[in]	wantedFormat	Format des pixels du buffer.
		 *\param[in]	buffer			Buffer de données.
		 *\param[in]	bufferFormat	Format des pixels du buffer de données.
		 *\param[in]	bufferAlign		L'alignement mémoire du buffer source.
		 *\return		Le buffer créé.
		 */
		CU_API static PxBufferBaseUPtr create( Size const & size
			, uint32_t layers
			, uint32_t levels
			, PixelFormat wantedFormat
			, uint8_t const * buffer = nullptr
			, PixelFormat bufferFormat = PixelFormat::eR8G8B8A8_UNORM
			, uint32_t bufferAlign = 0u )
		{
			return create( nullptr
				, size
				, layers
				, levels
				, wantedFormat
				, buffer
				, bufferFormat
				, bufferAlign );
		}
		/**
		 *\~english
		 *\brief		Creates a buffer with the given data.
		 *\param[in]	options			The convertion options.
		 *\param[in]	interrupt		Tells if the convertion is to be interrupted.
		 *\param[in]	size			Buffer dimensions.
		 *\param[in]	wantedFormat	Pixels format.
		 *\param[in]	buffer			Data buffer.
		 *\param[in]	bufferFormat	Data buffer's pixels format.
		 *\param[in]	bufferAlign		The alignment of the source buffer.
		 *\return		The created buffer.
		 *\~french
		 *\brief		Crée un buffer avec les données voulues.
		 *\param[in]	options			Les options de conversion.
		 *\param[in]	interrupt		Dit si la conversion est à interrompre.
		 *\param[in]	size			Dimensions du buffer.
		 *\param[in]	wantedFormat	Format des pixels du buffer.
		 *\param[in]	buffer			Buffer de données.
		 *\param[in]	bufferFormat	Format des pixels du buffer de données.
		 *\param[in]	bufferAlign		L'alignement mémoire du buffer source.
		 *\return		Le buffer créé.
		 */
		CU_API static PxBufferBaseUPtr create( PxBufferConvertOptions const * options
			, std::atomic_bool const * interrupt
			, Size const & size
			, PixelFormat wantedFormat
			, uint8_t const * buffer = nullptr
			, PixelFormat bufferFormat = PixelFormat::eR8G8B8A8_UNORM
			, uint32_t bufferAlign = 0u )
		{
			return create( options
				, interrupt
				, size
				, 1u
				, 1u
				, wantedFormat
				, buffer
				, bufferFormat
				, bufferAlign );
		}
		/**
		 *\~english
		 *\brief		Creates a buffer with the given data.
		 *\param[in]	options			The convertion options.
		 *\param[in]	size			Buffer dimensions.
		 *\param[in]	wantedFormat	Pixels format.
		 *\param[in]	buffer			Data buffer.
		 *\param[in]	bufferFormat	Data buffer's pixels format.
		 *\param[in]	bufferAlign		The alignment of the source buffer.
		 *\return		The created buffer.
		 *\~french
		 *\brief		Crée un buffer avec les données voulues.
		 *\param[in]	options			Les options de conversion.
		 *\param[in]	size			Dimensions du buffer.
		 *\param[in]	wantedFormat	Format des pixels du buffer.
		 *\param[in]	buffer			Buffer de données.
		 *\param[in]	bufferFormat	Format des pixels du buffer de données.
		 *\param[in]	bufferAlign		L'alignement mémoire du buffer source.
		 *\return		Le buffer créé.
		 */
		CU_API static PxBufferBaseUPtr create( PxBufferConvertOptions const * options
			, Size const & size
			, PixelFormat wantedFormat
			, uint8_t const * buffer = nullptr
			, PixelFormat bufferFormat = PixelFormat::eR8G8B8A8_UNORM
			, uint32_t bufferAlign = 0u )
		{
			return create( options
				, size
				, 1u
				, 1u
				, wantedFormat
				, buffer
				, bufferFormat
				, bufferAlign );
		}
		/**
		 *\~english
		 *\brief		Creates a buffer with the given data.
		 *\param[in]	size			Buffer dimensions.
		 *\param[in]	wantedFormat	Pixels format.
		 *\param[in]	buffer			Data buffer.
		 *\param[in]	bufferFormat	Data buffer's pixels format.
		 *\param[in]	bufferAlign		The alignment of the source buffer.
		 *\return		The created buffer.
		 *\~french
		 *\brief		Crée un buffer avec les données voulues.
		 *\param[in]	size			Dimensions du buffer.
		 *\param[in]	wantedFormat	Format des pixels du buffer.
		 *\param[in]	buffer			Buffer de données.
		 *\param[in]	bufferFormat	Format des pixels du buffer de données.
		 *\param[in]	bufferAlign		L'alignement mémoire du buffer source.
		 *\return		Le buffer créé.
		 */
		CU_API static PxBufferBaseUPtr create( Size const & size
			, PixelFormat wantedFormat
			, uint8_t const * buffer = nullptr
			, PixelFormat bufferFormat = PixelFormat::eR8G8B8A8_UNORM
			, uint32_t bufferAlign = 0u )
		{
			return create( nullptr
				, size
				, 1u
				, 1u
				, wantedFormat
				, buffer
				, bufferFormat
				, bufferAlign );
		}

	protected:
		uint32_t doConvert( uint32_t x, uint32_t y )const
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
		uint32_t m_align;
		Point3ui m_tiles{ 1u, 1u, 1u };
		mutable PxArray m_buffer;
	};
}

#endif
