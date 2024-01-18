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
		 *\param[in]	pass		The parent frame pass.
		 *\param[in]	context		The rendering context.
		 *\param[in]	graph		The runnable graph.
		 *\param[in]	device		The GPU device.
		 *\param[in]	index		The pass index.
		 *\param[in]	cameraUbo	The scene matrices UBO.
		 *\param[in]	culler		The culler for this pass.
		 *\param[in]	shadowMap	The parent shadow map.
		 *\param[in]	needsVsm	Tells if the pass needs to generate variance shadow map.
		 *\param[in]	needsRsm	Tells if the pass needs to generate reflective shadow maps.
		 *\param[in]	isStatic	Tells if the pass is for static nodes.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	pass		La frame pass parente.
		 *\param[in]	context		Le contexte de rendu.
		 *\param[in]	graph		Le runnable graph.
		 *\param[in]	device		Le device GPU.
		 *\param[in]	index		L'indice de la passe.
		 *\param[in]	cameraUbo	L'UBO de matrices de la scène.
		 *\param[in]	culler		Le culler pour cette passe.
		 *\param[in]	shadowMap	La shadow map parente.
		 *\param[in]	needsVsm	Dit si la passe doit générer la variance shadow map.
		 *\param[in]	needsRsm	Dit si la passe doit générer les reflective shadow maps.
		 *\param[in]	isStatic	Dit si la passe est pour les noeuds statiques.
		 */
		C3D_API ShadowMapPassSpot( crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, RenderDevice const & device
			, uint32_t index
			, CameraUbo const & cameraUbo
			, SceneCuller & culler
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
		C3D_API ~ShadowMapPassSpot()noexcept override;
		/**
		 *\copydoc		castor3d::ShadowMapPass::update
		 */
		C3D_API void update( CpuUpdater & updater )override;
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		ShadowType getShadowType()const noexcept
		{
			return m_shadowType;
		}
		/**@}*/

	private:
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
			return ShaderFlag::eShadowMapSpot
				| ShaderFlag::eWorldSpace
				| ShaderFlag::eViewSpace;
		}

	public:
		C3D_API static castor::String const Type;

	private:
		ShadowType m_shadowType{ ShadowType::eRaw };
		castor::Matrix4x4f m_view;
		float m_angle{};
	};
}

#endif
