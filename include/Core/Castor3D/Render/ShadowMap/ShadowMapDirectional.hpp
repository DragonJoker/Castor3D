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
		 *\param[in]	handler		The render graph resources handler.
		 *\param[in]	device		The GPU device.
		 *\param[in]	scene		The scene.
		 *\param[in]	progress	The optional progress bar.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	handler		Le gestionnaire de ressources du render graph.
		 *\param[in]	device		Le device GPU.
		 *\param[in]	scene		La scène.
		 *\param[in]	progress	La barre de progression optionnelle.
		 */
		C3D_API explicit ShadowMapDirectional( crg::ResourceHandler & handler
			, RenderDevice const & device
			, Scene & scene
			, ProgressBar * progress );
		/**
		 *\copydoc		castor3d::ShadowMap::update
		 */
		C3D_API void update( GpuUpdater & updater )override;

	private:
		std::vector< ShadowMap::PassDataPtr > doCreatePass( uint32_t index )override;
		void doUpdate( CpuUpdater & updater )override;
		bool doIsUpToDate( uint32_t index )const override;
		void doSetUpToDate( uint32_t index )override;
		uint32_t doGetMaxCount()const override
		{
			return 1u;
		}

	private:
		CameraSPtr m_camera;
		crg::ImageId m_blurIntermediate;
		crg::ImageViewId m_blurIntermediateView;
		std::vector< std::unique_ptr< GaussianBlur > > m_blurs;
		ShadowType m_shadowType{ ShadowType::eRaw };
		uint32_t m_cascades;
		std::vector< MeshResPtr > m_frustumMeshes;
	};
}

#endif
