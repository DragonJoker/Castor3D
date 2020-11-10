/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PickingPass_H___
#define ___C3D_PickingPass_H___

#include "Castor3D/Render/RenderPass.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include <ashespp/Image/ImageView.hpp>

#include <atomic>

namespace castor3d
{
	class PickingPass
		: public RenderPass
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
		C3D_API explicit PickingPass( Engine & engine
			, MatrixUbo & matrixUbo
			, SceneCuller & culler );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~PickingPass();
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
		 *\param[in]	position	The position in the pass.
		 *\param[in]	camera		The viewing camera.
		 *\return		PickingPass::PickNodeType::eNone if nothing was picked.
		 *\~french
		 *\brief		Sélectionne la géométrie à la position de souris donnée.
		 *\param[in]	position	La position dans la passe.
		 *\param[in]	camera		La caméra regardant la scène.
		 *\return		PickingPass::PickNodeType si rien n'a été pické.
		 */
		C3D_API PickNodeType pick( RenderDevice const & device
			, castor::Position position
			, Camera const & camera );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		C3D_API TextureFlags getTexturesMask()const override;

		inline GeometrySPtr getPickedGeometry()const
		{
			return m_geometry.lock();
		}

		inline BillboardBaseSPtr getPickedBillboard()const
		{
			return m_billboard.lock();
		}

		inline SubmeshSPtr getPickedSubmesh()const
		{
			return m_submesh.lock();
		}

		inline uint32_t getPickedFace()const
		{
			return m_face;
		}

		inline ashes::ImageView const & getResult()const
		{
			return m_colourView;
		}

		inline PickNodeType getPickedNodeType()const
		{
			return m_pickNodeType;
		}

		inline bool isPicking()const
		{
			return m_picking;
		}
		/**@}*/

	private:
		void doUpdateNodes( SceneCulledRenderNodes & nodes );
		castor::Point4f doFboPick( RenderDevice const & device
			, castor::Position const & position
			, Camera const & camera
			, ashes::CommandBuffer const & commandBuffer );
		PickNodeType doPick( castor::Point4f const & pixel
			, SceneCulledRenderNodes & nodes );
		/**
		 *\copydoc		castor3d::RenderPass::doRender
		 */
		void doUpdate( SubmeshStaticRenderNodesPtrByPipelineMap & nodes );
		/**
		 *\copydoc		castor3d::RenderPass::doRender
		 */
		void doUpdate( StaticRenderNodesPtrByPipelineMap & nodes );
		/**
		 *\copydoc		castor3d::RenderPass::doRenderAnimatedSubmeshes
		 */
		void doUpdate( SkinningRenderNodesPtrByPipelineMap & nodes );
		/**
		 *\copydoc		castor3d::RenderPass::doRender
		 */
		void doUpdate( SubmeshSkinningRenderNodesPtrByPipelineMap & nodes );
		/**
		 *\copydoc		castor3d::RenderPass::doRenderAnimatedSubmeshes
		 */
		void doUpdate( MorphingRenderNodesPtrByPipelineMap & nodes );
		/**
		 *\copydoc		castor3d::RenderPass::doRender
		 */
		void doUpdate( BillboardRenderNodesPtrByPipelineMap & nodes );
		/**
		 *\copydoc		castor3d::RenderPass::doInitialise
		 */
		bool doInitialise( RenderDevice const & device
			, castor::Size const & size )override;
		/**
		 *\copydoc		castor3d::RenderPass::doCleanup
		 */
		void doCleanup( RenderDevice const & device )override;
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
		 *\copydoc		castor3d::RenderPass::doFillTextureDescriptor
		 */
		void doFillTextureDescriptor( ashes::DescriptorSetLayout const & layout
			, uint32_t & index
			, BillboardListRenderNode & nodes
			, ShadowMapLightTypeArray const & shadowMaps )override;
		/**
		 *\copydoc		castor3d::RenderPass::doFillTextureDescriptor
		 */
		void doFillTextureDescriptor( ashes::DescriptorSetLayout const & layout
			, uint32_t & index
			, SubmeshRenderNode & nodes
			, ShadowMapLightTypeArray const & shadowMaps )override;
		/**
		 *\copydoc		castor3d::RenderPass::doUpdate
		 */
		void doUpdate( RenderQueueArray & queues )override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetVertexShaderSource
		 */
		virtual ShaderPtr doGetVertexShaderSource( PipelineFlags const & flags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetGeometryShaderSource
		 */
		ShaderPtr doGetGeometryShaderSource( PipelineFlags const & flags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetPhongPixelShaderSource
		 */
		ShaderPtr doGetPhongPixelShaderSource( PipelineFlags const & flags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetPbrMRPixelShaderSource
		 */
		ShaderPtr doGetPbrMRPixelShaderSource( PipelineFlags const & flags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetPbrSGPixelShaderSource
		 */
		ShaderPtr doGetPbrSGPixelShaderSource( PipelineFlags const & flags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetPixelShaderSource
		 */
		ShaderPtr doGetPixelShaderSource( PipelineFlags const & flags )const;
		/**
		 *\copydoc		castor3d::RenderPass::doUpdatePipeline
		 */
		void doUpdatePipeline( RenderPipeline & pipeline )override;
		/**
		 *\copydoc		castor3d::RenderPass::doCreateUboBindings
		 */
		ashes::VkDescriptorSetLayoutBindingArray doCreateUboBindings( PipelineFlags const & flags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doCreateTextureBindings
		 */
		ashes::VkDescriptorSetLayoutBindingArray doCreateTextureBindings( PipelineFlags const & flags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doCreateDepthStencilState
		 */
		ashes::PipelineDepthStencilStateCreateInfo doCreateDepthStencilState( PipelineFlags const & flags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doCreateBlendState
		 */
		ashes::PipelineColorBlendStateCreateInfo doCreateBlendState( PipelineFlags const & flags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doPrepareFrontPipeline
		 */
		void doPrepareFrontPipeline( ShaderProgramSPtr program
			, ashes::PipelineVertexInputStateCreateInfoCRefArray const & layouts
			, PipelineFlags const & flags )override;
		/**
		 *\copydoc		castor3d::RenderPass::doUpdateFlags
		 */
		void doUpdateFlags( PipelineFlags & flags )const override;

	public:
		static uint32_t constexpr PickingWidth = 32u;

	private:
		using CameraQueueMap = std::map< Camera const *, RenderQueue >;
		C3D_API static uint32_t const UboBindingPoint;

	private:
		std::map< castor::String, GeometryWPtr > m_pickable;
		ashes::ImagePtr m_colourTexture;
		ashes::ImagePtr m_depthTexture;
		ashes::ImageView m_colourView;
		ashes::ImageView m_depthView;
		ashes::FrameBufferPtr m_frameBuffer;
		VkBufferImageCopy m_copyRegion{};
		VkBufferImageCopy m_transferDisplayRegion{};
		std::vector< VkBufferImageCopy > m_pickDisplayRegions;
		ashes::CommandBufferPtr m_commandBuffer;
		ashes::BufferPtr< castor::Point4f > m_stagingBuffer;
		std::map< Scene const *, CameraQueueMap > m_scenes;
		GeometryWPtr m_geometry;
		BillboardBaseWPtr m_billboard;
		SubmeshWPtr m_submesh;
		uint32_t m_face{ 0u };
		std::vector< castor::Point4f > m_buffer;
		ashes::FencePtr m_transferFence;
		PickNodeType m_pickNodeType{ PickNodeType::eNone };
		std::atomic_bool m_picking{ false };
	};
}

#endif
