/*
See LICENSE file in root folder
*/
#ifndef ___C3D_UniformBufferPools_HPP___
#define ___C3D_UniformBufferPools_HPP___

#include "Castor3D/Buffer/UniformBufferPool.hpp"

namespace castor3d
{
	class UniformBufferPools
		: public castor::OwnedBy< RenderSystem >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	renderSystem	The RenderSystem.
		 *\param[in]	debugName		The buffer debug name.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	renderSystem	Le RenderSystem.
		 *\param[in]	debugName		Le nom debug du tampon.
		 */
		C3D_API UniformBufferPools( RenderSystem & renderSystem );
		/**
		 *\~english
		 *\brief		Cleans up all GPU buffers.
		 *\~french
		 *\brief		Nettoie tous les tampons GPU.
		 */
		C3D_API ~UniformBufferPools();
		/**
		 *\~english
		 *\brief		Uploads all GPU buffers to VRAM.
		 *\~french
		 *\brief		Met à jour tous les tampons GPU en VRAM.
		 */
		C3D_API void upload( ashes::CommandBuffer const & cb )const;
		/**
		 *\~english
		 *\brief		Retrieves a uniform buffer.
		 *\param[in]	flags	The buffer memory flags.
		 *\return		The uniform buffer.
		 *\~french
		 *\brief		Récupère un tampon d'uniformes.
		 *\param[in]	flags	Les indicateurs de mémoire du tampon.
		 *\return		Le tampon d'uniformes.
		 */
		template< typename DataT >
		UniformBufferOffsetT< DataT > getBuffer( VkMemoryPropertyFlags flags );
		/**
		 *\~english
		 *\brief		Releases a GPU buffer.
		 *\param[in]	bufferOffset	The buffer offset to release.
		 *\~french
		 *\brief		Libère un tampon GPU.
		 *\param[in]	bufferOffset	Le tampon à libérer.
		 */
		template< typename DataT >
		void putBuffer( UniformBufferOffsetT< DataT > const & bufferOffset );

	private:
		UniformBufferPoolT< MatrixUboConfiguration > m_matrixUboPool;
		UniformBufferPoolT< HdrConfig > m_hdrConfigUboPool;
		UniformBufferPoolT< RsmUboConfiguration > m_rsmConfigUboPool;
		UniformBufferPoolT< ModelMatrixUboConfiguration > m_modelMatrixUboPool;
		UniformBufferPoolT< ModelUboConfiguration > m_modelUboPool;
		UniformBufferPoolT< BillboardUboConfiguration > m_billboardUboPool;
		UniformBufferPoolT< PickingUboConfiguration > m_pickingUboPool;
		UniformBufferPoolT< TexturesUboConfiguration > m_texturesUboPool;
		UniformBufferPoolT< ShadowMapUboConfiguration > m_shadowMapUboPool;
		UniformBufferPoolT< SkinningUboConfiguration > m_skinningUboPool;
		UniformBufferPoolT< MorphingUboConfiguration > m_morphingUboPool;
	};
}

#include "Castor3D/Buffer/UniformBufferPools.inl"

#endif
