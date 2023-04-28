/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ShadowMapDirectional_H___
#define ___C3D_ShadowMapDirectional_H___

#include "ShadowMapModule.hpp"

#include "Castor3D/Render/Passes/GaussianBlur.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMap.hpp"

#include <ashespp/Image/ImageView.hpp>

namespace castor3d
{
	class ShadowMapDirectional
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
		C3D_API explicit ShadowMapDirectional( crg::ResourcesCache & resources
			, RenderDevice const & device
			, Scene & scene
			, ProgressBar * progress );

	private:
		crg::FramePassArray doCreatePass( crg::FrameGraph & graph
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

		uint32_t doGetMaxCount()const override
		{
			return 1u;
		}

	private:
		crg::ImageId m_blurIntermediate;
		crg::ImageViewId m_blurIntermediateView;
		ShadowType m_shadowType{ ShadowType::eRaw };
		uint32_t m_cascades;
		std::vector< MeshResPtr > m_frustumMeshes;
	};
}

#endif
