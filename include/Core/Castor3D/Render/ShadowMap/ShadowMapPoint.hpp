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
		 *\param[in]	resources	The render graph resources handler.
		 *\param[in]	device		The GPU device.
		 *\param[in]	scene		The scene.
		 *\param[in]	progress	The optional progress bar.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	resources	Le gestionnaire de ressources du render graph.
		 *\param[in]	device		Le device GPU.
		 *\param[in]	scene		La scène.
		 *\param[in]	progress	La barre de progression optionnelle.
		 */
		C3D_API explicit ShadowMapPoint( crg::ResourcesCache & resources
			, RenderDevice const & device
			, Scene & scene
			, ProgressBar * progress );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		Texture const & getTexture()const noexcept
		{
			return m_result[SmTexture::eVariance];
		}
		/**@}*/

	private:
		crg::FramePassArray doCreatePass( crg::FramePassGroup & graph
			, crg::FramePassArray const & previousPasses
			, uint32_t index
			, bool vsm
			, bool rsm
			, bool isStatic
			, Passes & passes )override;
		bool doIsUpToDate( uint32_t index
			, Passes const & passes )const override;
		void doSetUpToDate( uint32_t index
			, Passes & passes )override;
		void doUpdate( CpuUpdater & updater
			, Passes & passes )override;
		void doUpdate( GpuUpdater & updater
			, Passes & passes )override;
		uint32_t doGetMaxCount()const override;

	private:
		crg::ImageId m_blurIntermediate;
		crg::ImageViewId m_blurIntermediateView;
	};
}

#endif
