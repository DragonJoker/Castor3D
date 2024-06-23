/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SHADOW_MAP_PASS_DIRECTIONAL_H___
#define ___C3D_SHADOW_MAP_PASS_DIRECTIONAL_H___

#include "ShadowMapModule.hpp"

#include "Castor3D/Render/ShadowMap/ShadowMapPass.hpp"

namespace castor3d
{
	class ShadowMapPassDirectional
		: public ShadowMapPass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	pass			The parent frame pass.
		 *\param[in]	context			The rendering context.
		 *\param[in]	graph			The runnable graph.
		 *\param[in]	device			The GPU device.
		 *\param[in]	cameraUbo		The scene matrices UBO.
		 *\param[in]	culler			The culler for this pass.
		 *\param[in]	camera			The camera for this pass.
		 *\param[in]	shadowMap		The parent shadow map.
		 *\param[in]	needsVsm		Tells if the pass needs to generate variance shadow map.
		 *\param[in]	needsRsm		Tells if the pass needs to generate reflective shadow maps.
		 *\param[in]	isStatic		Tells if the pass is for static nodes.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	pass			La frame pass parente.
		 *\param[in]	context			Le contexte de rendu.
		 *\param[in]	graph			Le runnable graph.
		 *\param[in]	device			Le device GPU.
		 *\param[in]	cameraUbo		L'UBO de matrices de la scène.
		 *\param[in]	culler			Le culler pour cette passe.
		 *\param[in]	camera			La caméra pour cette passe.
		 *\param[in]	shadowMap		La shadow map parente.
		 *\param[in]	needsVsm		Dit si la passe doit générer la variance shadow map.
		 *\param[in]	needsRsm		Dit si la passe doit générer les reflective shadow maps.
		 *\param[in]	isStatic		Dit si la passe est pour les noeuds statiques.
		 */
		C3D_API ShadowMapPassDirectional( crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, RenderDevice const & device
			, CameraUbo const & cameraUbo
			, SceneCuller & culler
			, Camera & camera
			, ShadowMap const & shadowMap
			, bool needsVsm
			, bool needsRsm
			, bool isStatic );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~ShadowMapPassDirectional()noexcept override;

	private:
		void doUpdate( CpuUpdater & updater )override;
		void doUpdateUbos( CpuUpdater & updater )override;
		ashes::PipelineDepthStencilStateCreateInfo doCreateDepthStencilState( PipelineFlags const & flags )const override;
		ashes::PipelineColorBlendStateCreateInfo doCreateBlendState( PipelineFlags const & flags )const override;
		ProgramFlags doAdjustProgramFlags( ProgramFlags flags )const override;
		void doGetSubmeshShaderSource( PipelineFlags const & flags
			, ast::ShaderBuilder & builder )const override;
		void doGetPixelShaderSource( PipelineFlags const & flags
			, ast::ShaderBuilder & builder )const override;

		ShaderFlags doGetShaderFlags()const override
		{
			return ShaderFlag::eShadowMapDirectional
				| ( m_needsRsm ? ShaderFlag::eWorldSpace : ShaderFlag::eNone );
		}

	public:
		C3D_API static castor::String const Type;

	private:
		Camera & m_camera;
	};
}

#endif
