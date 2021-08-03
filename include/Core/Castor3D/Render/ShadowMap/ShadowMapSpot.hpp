/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ShadowMapSpot_H___
#define ___C3D_ShadowMapSpot_H___

#include "ShadowMapModule.hpp"

#include "Castor3D/Render/Passes/GaussianBlur.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMap.hpp"

#include <ashespp/Image/ImageView.hpp>

namespace castor3d
{
	class ShadowMapSpot
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
		C3D_API explicit ShadowMapSpot( crg::ResourceHandler & handler
			, RenderDevice const & device
			, Scene & scene
			, ProgressBar * progress );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~ShadowMapSpot();
		/**
		 *\copydoc		castor3d::ShadowMap::update
		 */
		C3D_API void update( GpuUpdater & updater )override;

	private:
		std::vector< ShadowMap::PassDataPtr > doCreatePass( uint32_t index )override;
		bool doIsUpToDate( uint32_t index )const override;
		void doUpdate( CpuUpdater & updater )override;
		uint32_t doGetMaxCount()const override;

	public:
		static VkFormat constexpr VarianceFormat = VK_FORMAT_R32G32_SFLOAT;
		static VkFormat constexpr LinearDepthFormat = VK_FORMAT_R32_SFLOAT;

	private:
		crg::ImageId m_blurIntermediate;
		crg::ImageViewId m_blurIntermediateView;
		std::vector< std::unique_ptr< GaussianBlur > > m_blurs;
	};
}

#endif
