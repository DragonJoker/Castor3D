/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SHADOW_MAP_PASS_SPOT_H___
#define ___C3D_SHADOW_MAP_PASS_SPOT_H___

#include "ShadowMapModule.hpp"

#include "Castor3D/Render/ShadowMap/ShadowMapPass.hpp"

namespace castor3d
{
	class ShadowMapPassSpot
		: public ShadowMapPass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	matrixUbo	The scene matrices UBO.
		 *\param[in]	culler		The culler for this pass.
		 *\param[in]	shadowMap	The parent shadow map.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	matrixUbo	L'UBO de matrices de la scène.
		 *\param[in]	culler		Le culler pour cette passe.
		 *\param[in]	shadowMap	La shadow map parente.
		 */
		C3D_API ShadowMapPassSpot( Engine & engine
			, uint32_t index
			, MatrixUbo & matrixUbo
			, SceneCuller & culler
			, ShadowMap const & shadowMap );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~ShadowMapPassSpot();
		/**
		 *\copydoc		castor3d::ShadowMapPass::update
		 */
		bool update( Camera const & camera
			, RenderQueueArray & queues
			, Light & light
			, uint32_t index )override;
		/**
		 *\copydoc		castor3d::ShadowMapPass::updateDeviceDependent
		 */
		void updateDeviceDependent( uint32_t index )override;
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		inline ShadowType getShadowType()const
		{
			return m_shadowType;
		}
		/**@}*/

	private:
		/**
		 *\copydoc		castor3d::RenderPass::doInitialise
		 */
		bool doInitialise( castor::Size const & size )override;
		/**
		 *\copydoc		castor3d::ShadowMapPass::doCleanup
		 */
		void doCleanup()override;
		/**
		 *\copydoc		castor3d::RenderPass::doFillUboDescriptor
		 */
		void doFillUboDescriptor( ashes::DescriptorSetLayout const & layout
			, BillboardListRenderNode & node )override;
		/**
		 *\copydoc		castor3d::RenderPass::doFillUboDescriptor
		 */
		void doFillUboDescriptor( ashes::DescriptorSetLayout const & layout
			, SubmeshRenderNode & node )override;
		/**
		 *\copydoc		castor3d::RenderPass::doUpdate
		 */
		void doUpdate( RenderQueueArray & queues )override;
		/**
		 *\copydoc		castor3d::RenderPass::doCreateUboBindings
		 */
		ashes::VkDescriptorSetLayoutBindingArray doCreateUboBindings( PipelineFlags const & flags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doCreateDepthStencilState
		 */
		ashes::PipelineDepthStencilStateCreateInfo doCreateDepthStencilState( PipelineFlags const & flags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doCreateBlendState
		 */
		ashes::PipelineColorBlendStateCreateInfo doCreateBlendState( PipelineFlags const & flags )const override;

	public:
		C3D_API static uint32_t const TextureSize;

	private:
		ShadowType m_shadowType{ ShadowType::eRaw };
		castor::Matrix4x4f m_view;
	};
}

#endif
