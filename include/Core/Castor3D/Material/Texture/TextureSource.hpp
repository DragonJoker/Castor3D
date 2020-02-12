/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextureSource_H___
#define ___C3D_TextureSource_H___

#include "TextureModule.hpp"

namespace castor3d
{
	class TextureSource
	{
	public:
		/**
		 *\~english
		 *\return		The texture buffer.
		 *\param[in]	engine	The engine.
		 *\~french
		 *\return		Le tampon de la texture.
		 *\param[in]	engine	Le moteur.
		 */
		C3D_API explicit TextureSource( Engine & engine )
			: m_engine{ engine }
		{
		}
		/**
		 *\~english
		 *\return		The texture buffer.
		 *\~french
		 *\return		Le tampon de la texture.
		 */
		C3D_API virtual ~TextureSource()
		{
		}
		/**
		 *\~english
		 *\return		The texture buffer.
		 *\~french
		 *\return		Le tampon de la texture.
		 */
		C3D_API virtual castor::PxBufferBaseSPtr getBuffer( uint32_t level = 0u )const = 0;
		/**
		 *\~english
		 *\return		The texture buffers.
		 *\~french
		 *\return		Les tampons de la texture.
		 */
		C3D_API virtual castor::PxBufferPtrArray const & getBuffers()const = 0;
		/**
		 *\~english
		 *\return		The texture buffers.
		 *\~french
		 *\return		Les tampons de la texture.
		 */
		C3D_API virtual castor::PxBufferPtrArray & getBuffers() = 0;
		/**
		 *\~english
		 *\brief		sets the texture buffer.
		 *\param[in]	buffer	The texture buffer.
		 *\~french
		 *\brief		Définit le tampon de la texture.
		 *\param[in]	buffer	Le tampon de la texture.
		 */
		C3D_API virtual void setBuffer( castor::PxBufferBaseSPtr buffer
			, uint32_t level = 0u ) = 0;
		/**
		 *\~english
		 *\return		The static source status.
		 *\~french
		 *\return		Le statut de source statique.
		 */
		C3D_API virtual bool isStatic()const = 0;

		/**
		 *\~english
		 *\return		The source as string.
		 *\~french
		 *\return		La source en chaîne de caractères.
		 */
		C3D_API virtual castor::String toString()const = 0;
		/**
		 *\~english
		 *\return		The source's dimensions.
		 *\~french
		 *\return		Les dimensions de la source.
		 */
		inline VkExtent3D getDimensions()const
		{
			return m_size;
		}
		/**
		 *\~english
		 *\return		The source's pixel format.
		 *\~french
		 *\return		Le format des pixels de la source.
		 */
		inline VkFormat getPixelFormat()const
		{
			return m_format;
		}
		/**
		 *\~english
		 *\return		The source's mipmap levels count.
		 *\~french
		 *\return		Le nombre de niveaux de mipmaps de la source.
		 */
		inline uint32_t getLevelCount()const
		{
			return uint32_t( getBuffers().size() );
		}

		C3D_API static TextureSourceUPtr create( Engine & engine
			, castor::Path const & folder
			, castor::Path const & relative );
		C3D_API static TextureSourceUPtr create( Engine & engine
			, castor::PxBufferBaseSPtr buffer
			, uint32_t depth );
		C3D_API static TextureSourceUPtr create( Engine & engine
			, VkExtent3D const & extent
			, VkFormat format );

	protected:
		/**
		 *\~english
		 *\brief			Readjusts dimensions if the selected rendering API doesn't support NPOT textures.
		 *\param[in,out]	size	The size.
		 *\return			\p true if the dimensions have changed.
		 *\~french
		 *\brief			Réajuste les dimensions données si l'API de rendu sélectionnée ne supporte pas les textures NPOT.
		 *\param[in,out]	size	La taille.
		 *\return			\p true si les dimensions ont changé.
		 */
		bool doAdjustDimensions( VkExtent3D & size );

	protected:
		//!\~english	The engine.
		//!\~french		Le moteur.
		Engine & m_engine;
		//!\~english	The source's pixel format.
		//!\~french		Le format des pixels de la source.
		VkFormat m_format;
		//!\~english	The source's dimensions.
		//!\~french		Les dimensions de la source.
		VkExtent3D m_size;
	};
}

#endif
