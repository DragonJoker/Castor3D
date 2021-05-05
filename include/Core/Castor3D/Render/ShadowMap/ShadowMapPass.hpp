/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SHADOW_MAP_PASS_H___
#define ___C3D_SHADOW_MAP_PASS_H___

#include "ShadowMapModule.hpp"

#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Render/RenderPass.hpp"
#include "Castor3D/Render/Viewport.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMap.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Shader/Ubos/ShadowMapUbo.hpp"

#include <ashespp/Pipeline/PipelineVertexInputStateCreateInfo.hpp>

namespace castor3d
{
	class ShadowMapPass
		: public SceneRenderPass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	name		The pass name.
		 *\param[in]	engine		The engine.
		 *\param[in]	matrixUbo	The scene matrices UBO.
		 *\param[in]	culler		The culler for this pass.
		 *\param[in]	shadowMap	The parent shadow map.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	name		Le nom de la passe.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	matrixUbo	L'UBO de matrices de la scène.
		 *\param[in]	culler		Le culler pour cette passe.
		 *\param[in]	shadowMap	La shadow map parente.
		 */
		C3D_API ShadowMapPass( RenderDevice const & device
			, castor::String name
			, MatrixUbo & matrixUbo
			, SceneCuller & culler
			, ShadowMap const & shadowMap
			, ashes::RenderPassPtr renderPass );
		/**
		 *\~english
		 *\brief			Updates the render pass, CPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau CPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API virtual bool update( CpuUpdater & updater ) = 0;
		/**
		 *\~english
		 *\brief			Updates the render pass, GPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau GPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API virtual void update( GpuUpdater & updater ) = 0;

		RenderPassTimer & getTimer()
		{
			return *m_timer;
		}

		bool isUpToDate()const
		{
			return !m_outOfDate;
		}

		void setUpToDate()
		{
			m_outOfDate = false;
		}

		C3D_API TextureFlags getTexturesMask()const override
		{
			return ShadowMap::textureFlags;
		}

		C3D_API ShaderFlags getShaderFlags()const override
		{
			return ShaderFlag::eWorldSpace
				| ShaderFlag::eTangentSpace;
		}

	protected:
		/**
		 *\~english
		 *\brief		Updates the given nodes.
		 *\param		nodes	The nodes.
		 *\~french
		 *\brief		Met à jour les noeuds donnés.
		 *\param		nodes	Les noeuds.
		 */
		void doUpdateNodes( SceneCulledRenderNodes & nodes );
		/**
		 *\copydoc		castor3d::SceneRenderPass::doCreateTextureBindings
		 */
		ashes::VkDescriptorSetLayoutBindingArray doCreateTextureBindings( PipelineFlags const & flags )const override;

	private:
		void doFillTextureDescriptor( RenderPipeline const & pipeline
			, ashes::DescriptorSetLayout const & layout
			, uint32_t & index
			, BillboardListRenderNode & nodes
			, ShadowMapLightTypeArray const & shadowMaps )override;
		void doFillTextureDescriptor( RenderPipeline const & pipeline
			, ashes::DescriptorSetLayout const & layout
			, uint32_t & index
			, SubmeshRenderNode & nodes
			, ShadowMapLightTypeArray const & shadowMaps )override;
		ShaderPtr doGetGeometryShaderSource( PipelineFlags const & flags )const override;
		void doUpdatePipeline( RenderPipeline & pipeline )override;

	protected:
		ShadowMap const & m_shadowMap;
		mutable bool m_initialised{ false };
		bool m_outOfDate{ true };
		ShadowMapUbo m_shadowMapUbo;
	};
}

#endif
