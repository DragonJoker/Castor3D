/*
See LICENSE file in root folder
*/
#ifndef ___CU_PxBuffer_H___
#define ___CU_PxBuffer_H___

#include "CastorUtils/Graphics/PixelBufferBase.hpp"
#include "CastorUtils/Graphics/PixelConstIterator.hpp"
#include "CastorUtils/Graphics/PixelIterator.hpp"

#include <cstddef>
#include <iterator>

namespace castor
{
	/**
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		Pixel buffer class, with pixel format as a template param
	\~french
	\brief		Buffer de pixels avec le format de pixel en argument template
	*/
	template< PixelFormat PF >
	class PxBuffer
		: public PxBufferBase
	{
	public:
		//!\~english	Typedef on a Pixel.
		//!\~french		Typedef sur un Pixel.
		typedef Pixel< PF > pixel;
		//!\~english	Typedef on a vector of pixel.
		//!\~french		Typedef sur un vector de pixel.
		typedef std::vector< pixel > column;

		using iterator = PixelIterator< PF >;
		using const_iterator = ConstPixelIterator< PF >;

	public:
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
		explicit PxBuffer( Size const & size
			, uint8_t const * buffer = nullptr
			, PixelFormat bufferFormat = PixelFormat::eR8G8B8A8_UNORM );
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	pixelBuffer	The PxBuffer object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	pixelBuffer	L'objet PxBuffer à copier
		 */
		PxBuffer( PxBuffer const & pixelBuffer );
		/**
		 *\~english
		 *\brief		Move Constructor
		 *\param[in]	pixelBuffer	The PxBuffer object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	pixelBuffer	L'objet PxBuffer à déplacer
		 */
		PxBuffer( PxBuffer && pixelBuffer );
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
		 *\param[in]	pixelBuffer	The PxBuffer object to copy
		 *\return		A reference to this PxBuffer object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	pixelBuffer	L'objet PxBuffer à copier
		 *\return		Une référence sur cet objet PxBuffer
		 */
		PxBuffer & operator=( PxBuffer const & pixelBuffer );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	pixelBuffer	The PxBuffer object to move
		 *\return		A reference to this PxBuffer object
		 *\~french
		 *\brief		Opérateur d'affectation  par déplacement
		 *\param[in]	pixelBuffer	L'objet PxBuffer à déplacer
		 *\return		Une référence sur cet objet PxBuffer
		 */
		PxBuffer & operator=( PxBuffer && pixelBuffer );
		/**
		 *\~english
		 *\brief		Swaps this buffer's data with the given one's
		 *\param[in]	pixelBuffer	The buffer to swap
		 *\~french
		 *\brief		Echange les données de ce buffer avec celles du buffer donné
		 *\param[in]	pixelBuffer	Le buffer à échanger
		 */
		void swap( PxBuffer & pixelBuffer );
		/**
		 *\~english
		 *\brief		Converts and assigns a data buffer to this buffer
		 *\param[in]	buffer			Data buffer
		 *\param[in]	bufferFormat	Data buffer's pixels format
		 *\return
		 *\~french
		 *\brief		Convertit et assigne les données du buffer donné à ce buffer
		 *\param[in]	buffer			Buffer de données
		 *\param[in]	bufferFormat	Format des pixels du buffer de données
		 *\return
		 */
		void assign( std::vector< uint8_t > const & buffer, PixelFormat bufferFormat )override;
		/**
		 *\~english
		 *\brief		Creates a new buffer with same values as this one
		 *\return		The created buffer
		 *\~french
		 *\brief		Crée un nouveau buffer avec les mêmes valeurs
		 *\return		Le buffer créé
		 */
		std::shared_ptr< PxBufferBase > clone()const override;
		/**
		 *\~english
		 *\brief		Makes a horizontal swap of pixels
		 *\~french
		 *\brief		Effectue un échange horizontal des pixels
		 */
		void mirror()override;
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		uint32_t getSize()const override;
		uint8_t const * getConstPtr()const override;
		uint8_t * getPtr()override;
		PxBufferBase::pixel_data getAt( uint32_t x, uint32_t y )override;
		PxBufferBase::const_pixel_data getAt( uint32_t x, uint32_t y )const override;

		inline pixel at( uint32_t x, uint32_t y );
		inline pixel at( uint32_t x, uint32_t y )const;
		inline column const & operator[]( uint32_t index )const;
		inline column & operator[]( uint32_t index );

		inline iterator begin()
		{
			return iterator{ m_buffer, m_buffer.begin() };
		}

		inline const_iterator begin()const
		{
			return const_iterator{ m_buffer, m_buffer.begin() };
		}

		inline iterator end()
		{
			return iterator{ m_buffer, m_buffer.end() };
		}

		inline const_iterator end()const
		{
			return const_iterator{ m_buffer, m_buffer.end() };
		}
		/**@}*/

	private:
		uint32_t doConvert( uint32_t x, uint32_t y )const;
		virtual void doInitColumn( uint32_t column )const;
		virtual void doInitColumn( uint32_t column );

	private:
		mutable column m_column;
	};
}

#include "PixelBuffer.inl"

#endif
