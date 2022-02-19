/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PickingPass_H___
#define ___C3D_PickingPass_H___

#include "Castor3D/Render/RenderNodesPass.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include <ashespp/Image/ImageView.hpp>

#pragma warning( push )
#pragma warning( disable:4365 )
#include <atomic>
#pragma warning( pop )

namespace castor3d
{
	class PickingPass
		: public RenderNodesPass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	matrixUbo	The scene matrices UBO.
		 *\param[in]	culler		The culler for this pass.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	matrixUbo	L'UBO de matrices de la scène.
		 *\param[in]	culler		Le culler pour cette passe.
		 */
		C3D_API explicit PickingPass( crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, RenderDevice const & device
			, castor::Size const & size
			, MatrixUbo & matrixUbo
			, SceneCuller & culler );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~PickingPass()override;
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
		 *\brief		Picks a geometry at given mouse position.
		 *\param[in]	device		The GPU device.
		 *\param[in]	position	The position in the pass.
		 *\param[in]	camera		The viewing camera.
		 *\return		PickingPass::PickNodeType::eNone if nothing was picked.
		 *\~french
		 *\brief		Sélectionne la géométrie à la position de souris donnée.
		 *\param[in]	device		Le device GPU.
		 *\param[in]	position	La position dans la passe.
		 *\param[in]	camera		La caméra regardant la scène.
		 *\return		PickingPass::PickNodeType si rien n'a été pické.
		 */
		C3D_API bool updateNodes( VkRect2D const & scissor );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		C3D_API QueueCulledRenderNodes const & getCulledRenderNodes()const;
		C3D_API TextureFlags getTexturesMask()const override;

		C3D_API ShaderFlags getShaderFlags()const override
		{
			return ShaderFlag::eNone;
		}
		/**@}*/

	private:
		void doUpdateNodes( QueueCulledRenderNodes & nodes );
		void doUpdate( SubmeshRenderNodePtrByPipelineMap & nodes );
		void doUpdate( BillboardRenderNodePtrByPipelineMap & nodes );
		void doUpdate( SubmeshRenderNodesPtrByPipelineMap & nodes );
		bool doIsValidPass( Pass const & pass )const override;
		void doFillAdditionalBindings( PipelineFlags const & flags
			, ashes::VkDescriptorSetLayoutBindingArray & bindings )const override;
		void doFillAdditionalDescriptor( RenderPipeline const & pipeline
			, ashes::WriteDescriptorSetArray & descriptorWrites
			, BillboardRenderNode & node
			, ShadowMapLightTypeArray const & shadowMaps )override;
		void doFillAdditionalDescriptor( RenderPipeline const & pipeline
			, ashes::WriteDescriptorSetArray & descriptorWrites
			, SubmeshRenderNode & node
			, ShadowMapLightTypeArray const & shadowMaps )override;
		void doUpdate( RenderQueueArray & queues )override;
		virtual ShaderPtr doGetVertexShaderSource( PipelineFlags const & flags )const override;
		ShaderPtr doGetHullShaderSource( PipelineFlags const & flags )const override;
		ShaderPtr doGetDomainShaderSource( PipelineFlags const & flags )const override;
		ShaderPtr doGetGeometryShaderSource( PipelineFlags const & flags )const override;
		ShaderPtr doGetPixelShaderSource( PipelineFlags const & flags )const override;
		void doUpdatePipeline( RenderPipeline & pipeline )override;
		ashes::PipelineDepthStencilStateCreateInfo doCreateDepthStencilState( PipelineFlags const & flags )const override;
		ashes::PipelineColorBlendStateCreateInfo doCreateBlendState( PipelineFlags const & flags )const override;
		void doUpdateFlags( PipelineFlags & flags )const override;

	public:
		C3D_API static castor::String const Type;
		static uint32_t constexpr PickingWidth = 32u;

	private:
		using CameraQueueMap = std::map< Camera const *, RenderQueue >;
		C3D_API static uint32_t const UboBindingPoint;

	private:
		std::map< castor::String, GeometryWPtr > m_pickable;
		std::unordered_map< SubmeshRenderNode const *, UniformBufferOffsetT< PickingUboConfiguration > > m_submeshBuffers;
		std::unordered_map< BillboardRenderNode const *, UniformBufferOffsetT< PickingUboConfiguration > > m_billboardBuffers;
	};
}

#endif
