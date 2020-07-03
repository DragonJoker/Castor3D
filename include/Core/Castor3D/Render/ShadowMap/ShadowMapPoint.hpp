/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ShadowMapPoint_H___
#define ___C3D_ShadowMapPoint_H___

#include "ShadowMapModule.hpp"

#include "Castor3D/Render/Viewport.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"
#include "Castor3D/Render/Passes/GaussianBlur.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMap.hpp"

#include <ashespp/Image/ImageView.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/Sync/Semaphore.hpp>

namespace castor3d
{
	class ShadowMapPoint
		: public ShadowMap
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\param[in]	scene	The scene.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 *\param[in]	scene	La scène.
		 */
		C3D_API ShadowMapPoint( Engine & engine
			, Scene & scene );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~ShadowMapPoint();
		/**
		 *\copydoc		castor3d::ShadowMap::update
		 */
		C3D_API void update( Camera const & camera
			, RenderQueueArray & queues
			, Light & light
			, uint32_t index )override;
		/**
		 *\~english
		 *\brief		Updates VRAM data.
		 *\param[out]	index	The map index.
		 *\~french
		 *\brief		Met à jour les données VRAM.
		 *\param[out]	index	L'indice de la texture.
		 */
		C3D_API void updateDeviceDependent( uint32_t index )override;
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		C3D_API ashes::ImageView const & getView( SmTexture texture
			, uint32_t index )const override;

		inline TextureUnit const & getTexture()const
		{
			return m_result[SmTexture::eVariance];
		}
		/**@}*/

	private:
		void doInitialiseFramebuffers();
		/**
		 *\copydoc		castor3d::ShadowMap::doInitialise
		 */
		void doInitialise()override;
		/**
		 *\copydoc		castor3d::ShadowMap::doCleanup
		 */
		void doCleanup()override;
		/**
		 *\copydoc		castor3d::ShadowMap::doRender
		 */
		ashes::Semaphore const & doRender( ashes::Semaphore const & toWait
			, uint32_t index )override;
		/**
		 *\copydoc		castor3d::ShadowMap::isUpToDate
		 */
		bool isUpToDate( uint32_t index )const override;

	private:
		struct FrameBuffer
		{
			std::array< ashes::ImageView, size_t( SmTexture::eCount ) > views;
			ashes::ImageView varianceView;
			ashes::FrameBufferPtr frameBuffer;
			std::unique_ptr< GaussianBlur > blur;
			CommandsSemaphore blurCommands{ nullptr, nullptr };
		};
		struct PassData
		{
			ashes::CommandBufferPtr commandBuffer;
			std::array< FrameBuffer, 6u > frameBuffers;
			ashes::SemaphorePtr finished;
			ShadowType shadowType;
			std::array< ashes::ImageView, size_t( SmTexture::eCount ) > views;
		};
		std::vector< PassData > m_passesData;
	};
}

#endif
