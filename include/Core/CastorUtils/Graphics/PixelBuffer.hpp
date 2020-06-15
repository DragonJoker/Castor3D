/*
See LICENSE file in root folder
*/
#ifndef ___CU_PxBuffer_H___
#define ___CU_PxBuffer_H___

#include "CastorUtils/Design/ArrayView.hpp"
#include "CastorUtils/Graphics/PixelBufferBase.hpp"
#include "CastorUtils/Graphics/PixelConstIterator.hpp"
#include "CastorUtils/Graphics/PixelIterator.hpp"

#include <cstddef>
#include <iterator>

namespace castor
{
	template< PixelFormat PF >
	class PxBuffer
		: public PxBufferBase
	{
	public:
		typedef Pixel< PF > Pixel;

		using iterator = PixelIterator< PF >;
		using const_iterator = ConstPixelIterator< PF >;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	size			Buffer wanted dimensions
		 *\param[in]	layers			Buffer layers (or slices)
		 *\param[in]	levels			Buffer mip levels
		 *\param[in]	buffer			Data buffer
		 *\param[in]	bufferFormat	Data buffer's pixels format
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	size			Les dimensions voulues pour le buffer
		 *\param[in]	layers			Couches du buffer (layers ou slices)
		 *\param[in]	levels			Niveaux de mip du buffer
		 *\param[in]	buffer			Buffer de données
		 *\param[in]	bufferFormat	Format des pixels du buffer de données
		 */
		PxBuffer( Size const & size
			, uint32_t layers
			, uint32_t levels
			, uint8_t const * buffer = nullptr
			, PixelFormat bufferFormat = PixelFormat::eR8G8B8A8_UNORM );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	size			Buffer wanted dimensions
		 *\param[in]	buffer			Data buffer
		 *\param[in]	bufferFormat	Data buffer's pixels format
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	size			Les dimensions voulues pour le buffer
		 *\param[in]	buffer			Buffer de données
		 *\param[in]	bufferFormat	Format des pixels du buffer de données
		 */
		explicit inline PxBuffer( Size const & size
			, uint8_t const * buffer = nullptr
			, PixelFormat bufferFormat = PixelFormat::eR8G8B8A8_UNORM )
			: PxBuffer{ size, 1u, 1u, buffer, bufferFormat }
		{
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
		inline Pixel at( uint32_t x, uint32_t y )const;

		inline iterator begin()const
		{
			return iterator{ m_buffer, m_buffer.begin() };
		}

		inline iterator end()const
		{
			return iterator{ m_buffer, m_buffer.end() };
		}
		/**@}*/
	};
}

#include "PixelBuffer.inl"

#endif
