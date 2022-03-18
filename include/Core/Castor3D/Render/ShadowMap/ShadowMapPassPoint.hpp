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
		 *\param[in]	pass		The parent frame pass.
		 *\param[in]	context		The rendering context.
		 *\param[in]	graph		The runnable graph.
		 *\param[in]	device		The GPU device.
		 *\param[in]	index		The pass index.
		 *\param[in]	matrixUbo	The scene matrices UBO.
		 *\param[in]	culler		The culler for this pass.
		 *\param[in]	shadowMap	The parent shadow map.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	pass		La frame pass parente.
		 *\param[in]	context		Le contexte de rendu.
		 *\param[in]	graph		Le runnable graph.
		 *\param[in]	device		Le device GPU.
		 *\param[in]	index		L'indice de la passe.
		 *\param[in]	matrixUbo	L'UBO de matrices de la scène.
		 *\param[in]	culler		Le culler pour cette passe.
		 *\param[in]	shadowMap	La shadow map parente.
		 */
		C3D_API ShadowMapPassPoint( crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, RenderDevice const & device
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
		C3D_API ~ShadowMapPassPoint()override;
		/**
		 *\copydoc		castor3d::ShadowMapPass::update
		 */
		C3D_API void update( CpuUpdater & updater )override;
		/**
		 *\copydoc		castor3d::ShadowMapPass::update
		 */
		C3D_API void update( GpuUpdater & updater )override;

		C3D_API void updateFrustum( castor::Matrix4x4f const & viewMatrix );

	protected:
		void doUpdateNodes( QueueRenderNodes & nodes );

	private:
		void doUpdateUbos( CpuUpdater & updater )override;
		ashes::PipelineDepthStencilStateCreateInfo doCreateDepthStencilState( PipelineFlags const & flags )const override;
		ashes::PipelineColorBlendStateCreateInfo doCreateBlendState( PipelineFlags const & flags )const override;
		PassFlags doAdjustPassFlags( PassFlags flags )const override;
		ProgramFlags doAdjustProgramFlags( ProgramFlags flags )const override;
		ShaderPtr doGetVertexShaderSource( PipelineFlags const & flags )const override;
		ShaderPtr doGetPixelShaderSource( PipelineFlags const & flags )const override;

	public:
		C3D_API static uint32_t const TextureSize;
		C3D_API static castor::String const Type;

	private:
		OnSceneNodeChangedConnection m_onNodeChanged;
		castor::Matrix4x4f m_projection;
	};
}

#endif
