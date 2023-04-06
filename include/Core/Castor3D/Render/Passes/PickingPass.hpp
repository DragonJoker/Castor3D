/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PickingPass_H___
#define ___C3D_PickingPass_H___

#include "Castor3D/Render/RenderNodesPass.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include <ashespp/Image/ImageView.hpp>

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <atomic>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

namespace castor3d
{
	class PickingPass
		: public RenderNodesPass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	pass		The parent frame pass.
		 *\param[in]	context		The rendering context.
		 *\param[in]	graph		The runnable graph.
		 *\param[in]	device		The GPU device.
		 *\param[in]	size		The render area dimensions.
		 *\param[in]	cameraUbo	The scene matrices UBO.
		 *\param[in]	sceneUbo	The scene UBO.
		 *\param[in]	culler		The culler for this pass.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	pass		La frame pass parente.
		 *\param[in]	context		Le contexte de rendu.
		 *\param[in]	graph		Le runnable graph.
		 *\param[in]	device		Le device GPU.
		 *\param[in]	size		Les dimensions de la zone de rendu.
		 *\param[in]	cameraUbo	L'UBO de matrices de la scène.
		 *\param[in]	sceneUbo	L'UBO de scène.
		 *\param[in]	culler		Le culler pour cette passe.
		 */
		C3D_API explicit PickingPass( crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, RenderDevice const & device
			, castor::Size const & size
			, CameraUbo const & cameraUbo
			, SceneUbo const & sceneUbo
			, SceneCuller & culler );
		/**
		 *\~english
		 *\brief		Adds a scene rendered through this technique.
		 *\param[in]	scene	The scene.
		 *\param[in]	camera	The camera through which the scene is viewed.
		 *\~french
		 *\brief		Ajoute une scène dessinée via cette technique.
		 *\param[in]	scene	La scène.
		 *\param[in]	camera	La caméra à travers laquelle la scène est vue.
		 */
		C3D_API void addScene( Scene & scene
			, Camera & camera );
		/**
		 *\~english
		 *\brief		Updates the final render area.
		 *\param[in]	scissor		The scissor reducing the render area.
		 *\~french
		 *\brief		Met ç jour la zone de rendu finale.
		 *\param[in]	scissor		Le scissor réduisant la zone de rendu.
		 */
		C3D_API void updateArea( VkRect2D const & scissor );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		/**
		 *\copydoc		castor3d::RenderNodesPass::getComponentsMask
		 */
		C3D_API ComponentModeFlags getComponentsMask()const override;

		C3D_API ShaderFlags getShaderFlags()const override
		{
			return ShaderFlag::ePicking
				| ShaderFlag::eOpacity;
		}
		/**@}*/

	private:
		bool doIsValidPass( Pass const & pass )const override;
		void doFillAdditionalBindings( PipelineFlags const & flags
			, ashes::VkDescriptorSetLayoutBindingArray & bindings )const override;
		void doFillAdditionalDescriptor( PipelineFlags const & flags
			, ashes::WriteDescriptorSetArray & descriptorWrites
			, ShadowMapLightTypeArray const & shadowMaps )override;
		SubmeshFlags doAdjustSubmeshFlags( SubmeshFlags flags )const override;
		ProgramFlags doAdjustProgramFlags( ProgramFlags flags )const override;
		ashes::PipelineDepthStencilStateCreateInfo doCreateDepthStencilState( PipelineFlags const & flags )const override;
		ashes::PipelineColorBlendStateCreateInfo doCreateBlendState( PipelineFlags const & flags )const override;
		ShaderPtr doGetPixelShaderSource( PipelineFlags const & flags )const override;

	public:
		C3D_API static castor::String const Type;

	private:
		using CameraQueueMap = std::map< Camera const *, RenderQueue >;
		C3D_API static uint32_t const UboBindingPoint;

	private:
		std::map< castor::String, GeometryRPtr > m_pickable{};
	};
}

#endif
