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
		 *\param[in]	scene	The scene.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	scene	La scène.
		 */
		C3D_API explicit ShadowMapDirectional( crg::FrameGraph & graph
			, crg::FramePass const & previousPass
			, RenderDevice const & device
			, Scene & scene );
		/**
		 *\copydoc		castor3d::ShadowMap::update
		 */
		C3D_API void update( CpuUpdater & updater )override;
		/**
		 *\copydoc		castor3d::ShadowMap::update
		 */
		C3D_API void update( GpuUpdater & updater )override;

	private:
		bool isUpToDate( uint32_t index )const override;

	public:
		static VkFormat constexpr VarianceFormat = VK_FORMAT_R32G32_SFLOAT;
		static VkFormat constexpr LinearDepthFormat = VK_FORMAT_R32_SFLOAT;

	private:
		CameraSPtr m_camera;
		crg::ImageId m_blurIntermediate;
		crg::ImageViewId m_blurIntermediateView;
		std::vector< std::unique_ptr< GaussianBlur > > m_blurs;
		ShadowType m_shadowType{ ShadowType::eRaw };
		uint32_t m_cascades;
	};
}

#endif
