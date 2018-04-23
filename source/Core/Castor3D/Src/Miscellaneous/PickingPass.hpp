/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PICKING_PASS_H___
#define ___C3D_PICKING_PASS_H___

#include "Render/RenderPass.hpp"
#include "Render/Viewport.hpp"
#include "Scene/Geometry.hpp"

#include <Image/Texture.hpp>
#include <Pipeline/VertexLayout.hpp>

namespace castor3d
{
	struct PickingUboData
	{
		uint32_t drawIndex;
		uint32_t nodeIndex;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		30/08/2016
	\~english
	\brief		Picking pass, using FBO.
	\~french
	\brief		Passe de picking, utilisant les FBO.
	*/
	class PickingPass
		: public RenderPass
	{
	public:
		enum class NodeType
			: uint8_t
		{
			eNone,
			eStatic,
			eInstantiatedStatic,
			eSkinning,
			eInstantiatedSkinning,
			eMorphing,
			eBillboard
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 */
		C3D_API explicit PickingPass( Engine & engine );
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
		C3D_API void addScene( Scene & scene, Camera & camera );
		/**
		 *\~english
		 *\brief		Picks a geometry at given mouse position.
		 *\param[in]	position	The position in the pass.
		 *\param[in]	camera		The viewing camera.
		 *\return		PickingPass::NodeType::eNone if nothing was picked.
		 *\~french
		 *\brief		Sélectionne la géométrie à la position de souris donnée.
		 *\param[in]	position	La position dans la passe.
		 *\param[in]	camera		La caméra regardant la scène.
		 *\return		PickingPass::NodeType si rien n'a été pické.
		 */
		C3D_API NodeType pick( castor::Position const & position
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
		/**@}*/

	private:
		void doUpdateNodes( SceneCulledRenderNodes & nodes );
		castor::Point3f doFboPick( castor::Position const & position
			, Camera const & camera
			, renderer::CommandBuffer const & commandBuffer );
		PickingPass::NodeType doPick( castor::Point3f const & pixel
			, SceneCulledRenderNodes & nodes );
		/**
		 *\copydoc		castor3d::RenderPass::doRender
		 */
		void doUpdate( Scene const & scene
			, Camera const & camera
			, SubmeshStaticRenderNodesPtrByPipelineMap & nodes );
		/**
		 *\copydoc		castor3d::RenderPass::doRender
		 */
		void doUpdate( Scene const & scene
			, Camera const & camera
			, StaticRenderNodesPtrByPipelineMap & nodes );
		/**
		 *\copydoc		castor3d::RenderPass::doRenderAnimatedSubmeshes
		 */
		void doUpdate( Scene const & scene
			, Camera const & camera
			, SkinningRenderNodesPtrByPipelineMap & nodes );
		/**
		 *\copydoc		castor3d::RenderPass::doRender
		 */
		void doUpdate( Scene const & scene
			, Camera const & camera
			, SubmeshSkinningRenderNodesPtrByPipelineMap & nodes );
		/**
		 *\copydoc		castor3d::RenderPass::doRenderAnimatedSubmeshes
		 */
		void doUpdate( Scene const & scene
			, Camera const & camera
			, MorphingRenderNodesPtrByPipelineMap & nodes );
		/**
		 *\copydoc		castor3d::RenderPass::doRender
		 */
		void doUpdate( Scene const & scene
			, Camera const & camera
			, BillboardRenderNodesPtrByPipelineMap & nodes );
		/**
		 *\copydoc		castor3d::RenderPass::doInitialise
		 */
		bool doInitialise( castor::Size const & size )override;
		/**
		 *\copydoc		castor3d::RenderPass::doCleanup
		 */
		void doCleanup()override;
		/**
		 *\copydoc		castor3d::RenderPass::doFillUboDescriptor
		 */
		void doFillUboDescriptor( renderer::DescriptorSetLayout const & layout
			, uint32_t & index
			, BillboardListRenderNode & node )override;
		/**
		 *\copydoc		castor3d::RenderPass::doFillUboDescriptor
		 */
		void doFillUboDescriptor( renderer::DescriptorSetLayout const & layout
			, uint32_t & index
			, SubmeshRenderNode & node )override;
		/**
		 *\copydoc		castor3d::RenderPass::doFillTextureDescriptor
		 */
		void doFillTextureDescriptor( renderer::DescriptorSetLayout const & layout
			, uint32_t & index
			, BillboardListRenderNode & nodes )override;
		/**
		 *\copydoc		castor3d::RenderPass::doFillTextureDescriptor
		 */
		void doFillTextureDescriptor( renderer::DescriptorSetLayout const & layout
			, uint32_t & index
			, SubmeshRenderNode & nodes )override;
		/**
		 *\copydoc		castor3d::RenderPass::doUpdate
		 */
		void doUpdate( RenderQueueArray & queues )override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetGeometryShaderSource
		 */
		glsl::Shader doGetGeometryShaderSource( PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetLegacyPixelShaderSource
		 */
		glsl::Shader doGetLegacyPixelShaderSource( PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, renderer::CompareOp alphaFunc )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetPbrMRPixelShaderSource
		 */
		glsl::Shader doGetPbrMRPixelShaderSource( PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, renderer::CompareOp alphaFunc )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetPbrSGPixelShaderSource
		 */
		glsl::Shader doGetPbrSGPixelShaderSource( PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, renderer::CompareOp alphaFunc )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetPixelShaderSource
		 */
		glsl::Shader doGetPixelShaderSource( PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, renderer::CompareOp alphaFunc )const;
		/**
		 *\copydoc		castor3d::RenderPass::doUpdatePipeline
		 */
		void doUpdatePipeline( RenderPipeline & pipeline )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doCreateTextureBindings
		 */
		renderer::DescriptorSetLayoutBindingArray doCreateTextureBindings( PipelineFlags const & flags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doPrepareFrontPipeline
		 */
		void doPrepareFrontPipeline( ShaderProgramSPtr program
			, renderer::VertexLayoutCRefArray const & layouts
			, PipelineFlags const & flags )override;
		/**
		 *\copydoc		castor3d::RenderPass::doPrepareBackPipeline
		 */
		void doPrepareBackPipeline( ShaderProgramSPtr program
			, renderer::VertexLayoutCRefArray const & layouts
			, PipelineFlags const & flags )override;
		/**
		 *\copydoc		castor3d::RenderPass::doUpdateFlags
		 */
		void doUpdateFlags( PassFlags & passFlags
			, TextureChannels & textureFlags
			, ProgramFlags & programFlags
			, SceneFlags & sceneFlags )const override;

	private:
		using CameraQueueMap = std::map< Camera const *, RenderQueue >;

	private:
		std::map< castor::String, GeometryWPtr > m_pickable;
		renderer::TexturePtr m_colourTexture;
		renderer::TexturePtr m_depthTexture;
		renderer::TextureViewPtr m_colourView;
		renderer::TextureViewPtr m_depthView;
		renderer::RenderPassPtr m_renderPass;
		renderer::FrameBufferPtr m_frameBuffer;
		renderer::BufferImageCopy m_copyRegion;
		renderer::BufferPtr< castor::Point4f > m_stagingBuffer;
		std::map< Scene const *, CameraQueueMap > m_scenes;
		GeometryWPtr m_geometry;
		BillboardBaseWPtr m_billboard;
		SubmeshWPtr m_submesh;
		uint32_t m_face{ 0u };
		renderer::UniformBufferPtr< PickingUboData > m_pickingUbo;
		castor::PxBufferBaseSPtr m_buffer;
	};
}

#endif
