/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SHADOW_MAP_PASS_POINT_H___
#define ___C3D_SHADOW_MAP_PASS_POINT_H___

#include "ShadowMapModule.hpp"

#include "Castor3D/Render/ShadowMap/ShadowMapPass.hpp"
#include "Castor3D/Render/Viewport.hpp"

#include <ashespp/Buffer/UniformBuffer.hpp>

namespace castor3d
{
	class ShadowMapPassPoint
		: public ShadowMapPass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	index		The pass index.
		 *\param[in]	matrixUbo	The scene matrices UBO.
		 *\param[in]	culler		The culler for this pass.
		 *\param[in]	shadowMap	The parent shadow map.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	index		L'indice de la passe.
		 *\param[in]	matrixUbo	L'UBO de matrices de la scène.
		 *\param[in]	culler		Le culler pour cette passe.
		 *\param[in]	shadowMap	La shadow map parente.
		 */
		C3D_API ShadowMapPassPoint( Engine & engine
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
		C3D_API ~ShadowMapPassPoint();
		/**
		 *\copydoc		castor3d::ShadowMapPass::update
		 */
		bool update( CpuUpdater & updater )override;
		/**
		 *\copydoc		castor3d::ShadowMapPass::update
		 */
		void update( GpuUpdater & updater )override;

	protected:
		void doUpdateNodes( SceneCulledRenderNodes & nodes );

	private:
		bool doInitialise( RenderDevice const & device
			, castor::Size const & size )override;
		void doCleanup( RenderDevice const & device )override;
		void doUpdateUbos( CpuUpdater & updater )override;
		void doFillUboDescriptor( ashes::DescriptorSetLayout const & layout
			, BillboardListRenderNode & node )override;
		void doFillUboDescriptor( ashes::DescriptorSetLayout const & layout
			, SubmeshRenderNode & node )override;
		ashes::VkDescriptorSetLayoutBindingArray doCreateUboBindings( PipelineFlags const & flags )const override;
		ashes::PipelineDepthStencilStateCreateInfo doCreateDepthStencilState( PipelineFlags const & flags )const override;
		ashes::PipelineColorBlendStateCreateInfo doCreateBlendState( PipelineFlags const & flags )const override;
		void doUpdateFlags( PipelineFlags & flags )const override;
		ShaderPtr doGetVertexShaderSource( PipelineFlags const & flags )const override;
		ShaderPtr doGetPhongPixelShaderSource( PipelineFlags const & flags )const override;
		ShaderPtr doGetPbrMRPixelShaderSource( PipelineFlags const & flags )const override;
		ShaderPtr doGetPbrSGPixelShaderSource( PipelineFlags const & flags )const override;

	public:
		C3D_API static uint32_t const TextureSize;

	private:
		OnSceneNodeChangedConnection m_onNodeChanged;
		castor::Matrix4x4f m_projection;
		Viewport m_viewport;
		std::array< castor::Matrix4x4f, size_t( CubeMapFace::eCount ) > m_matrices;
	};
}

#endif
