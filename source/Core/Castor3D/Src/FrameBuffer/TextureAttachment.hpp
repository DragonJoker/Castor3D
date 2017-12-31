/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TEXTURE_ATTACHMENT_H___
#define ___C3D_TEXTURE_ATTACHMENT_H___

#include "FrameBufferAttachment.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		12/11/2012
	\~english
	\brief		Description of an attachment between a frame buffer and a texture
	\~french
	\brief		Description d'une liaison entre un tampon d'image et une texture
	*/
	class TextureAttachment
		: public FrameBufferAttachment
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	texture		The texture.
		 *\param[in]	mipLevel	The mipmap level.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	texture		La texture.
		 *\param[in]	mipLevel	Le niveau de mipmap.
		 */
		C3D_API explicit TextureAttachment( TextureLayoutSPtr texture
			, uint32_t mipLevel );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~TextureAttachment();
		/**
		 *\copydoc		castor3d::FrameBufferAttachment::getBuffer
		 */
		C3D_API virtual castor::PxBufferBaseSPtr getBuffer()const;
		/**
		 *\~english
		 *\brief		Retrieves the texture
		 *\return		The texture
		 *\~french
		 *\brief		Récupère la texture
		 *\return		La texture
		 */
		inline TextureLayoutSPtr getTexture()const
		{
			return m_texture.lock();
		}
		/**
		 *\~english
		 *\return		The target dimension.
		 *\~french
		 *\return		La dimension cible.
		 */
		inline TextureType getTarget()const
		{
			return m_target;
		}
		/**
		 *\~english
		 *\return		The attached layer
		 *\~french
		 *\return		La couche associée.
		 */
		inline int getLayer()const
		{
			return m_layer;
		}
		/**
		 *\~english
		 *\return		The mipmap level.
		 *\~french
		 *\return		Le niveau de mipmap.
		 */
		inline int getMipLevel()const
		{
			return m_mipLevel;
		}
		/**
		 *\~english
		 *\brief		Sets the attached target dimension.
		 *\param[in]	target	The target dimension.
		 *\~french
		 *\brief		Définit la dimension cible attachée.
		 *\param[in]	target	La dimension cible.
		 */
		inline void setTarget( TextureType target )
		{
			m_target = target;
		}
		/**
		 *\~english
		 *\brief		Sets the attached layer.
		 *\param[in]	layer	The layer.
		 *\~french
		 *\brief		Définit la couche associée.
		 *\param[in]	layer	La couche.
		 */
		inline void setLayer( int layer )
		{
			m_layer = layer;
		}

	private:
		//!\~english	The attached texture.
		//!\~french		La texture attachée.
		TextureLayoutWPtr m_texture;
		//!\~english	The texture target type.
		//!\~french		Le type de cible de la texture.
		TextureType m_target;
		//!\~english	The attached layer.
		//!\~french		La couche attachée.
		int m_layer;
		//!\~english	The attached layer.
		//!\~french		La couche attachée.
		uint32_t m_mipLevel;
	};
}

#endif
