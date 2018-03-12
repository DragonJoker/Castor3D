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
		 *\brief		adds a scene rendered through this technique.
		 *\param[in]	p_scene		The scene.
		 *\param[in]	p_camera	The camera through which the scene is viewed.
		 *\~french
		 *\brief		Ajoute une scène dessinée via cette technique.
		 *\param[in]	p_scene		La scène.
		 *\param[in]	p_camera	La caméra à travers laquelle la scène est vue.
		 */
		C3D_API void addScene( Scene & p_scene, Camera & p_camera );
		/**
		 *\~english
		 *\brief		Picks a geometry at given mouse position.
		 *\param[in]	p_position		The position in the pass.
		 *\param[in]	p_camera		The viewing camera.
		 *\return		PickingPass::NodeType::eNone if nothing was picked.
		 *\~french
		 *\brief		Sélectionne la géométrie à la position de souris donnée.
		 *\param[in]	p_position		La position dans la passe.
		 *\param[in]	p_camera		La caméra regardant la scène.
		 *\return		PickingPass::NodeType si rien n'a été pické.
		 */
		C3D_API NodeType pick( castor::Position const & p_position
			, Camera const & p_camera );
		/**
		 *\~english
		 *\return		The picked geometry.
		 *\~french
		 *\return		La géométrie sélectionnée.
		 */
		inline GeometrySPtr getPickedGeometry()const
		{
			return m_geometry.lock();
		}
		/**
		 *\~english
		 *\return		The picked billboard.
		 *\~french
		 *\return		Le billboard sélectionné.
		 */
		inline BillboardBaseSPtr getPickedBillboard()const
		{
			return m_billboard.lock();
		}
		/**
		 *\~english
		 *\return		The picked submesh.
		 *\~french
		 *\return		Le sous-maillage sélectionné.
		 */
		inline SubmeshSPtr getPickedSubmesh()const
		{
			return m_submesh.lock();
		}
		/**
		 *\~english
		 *\return		The picked face index.
		 *\~french
		 *\return		L'indice de la face sélectionnée.
		 */
		inline uint32_t getPickedFace()const
		{
			return m_face;
		}

	private:
		void doRenderNodes( SceneRenderNodes & p_nodes
			, Camera const & p_camera );

	private:
		/**
		 *\~english
		 *\brief		Executes the FBO picking.
		 *\param[in]	p_position	The position in the pass.
		 *\param[in]	p_camera	The viewing camera.
		 *\param[in]	p_nodes		The render nodes.
		 *\return		The picking data.
		 *\~french
		 *\brief		Exécute le picking FBO.
		 *\param[in]	p_position	La position dans la passe.
		 *\param[in]	p_camera	La caméra regardant la scène.
		 *\param[in]	p_nodes		Les noeuds de rendu.
		 *\return		Les données de picking.
		 */
		castor::Point3f doFboPick( castor::Position const & p_position
			, Camera const & p_camera
			, SceneRenderNodes & p_nodes );
		/**
		 *\~english
		 *\brief		Picks the node at given picking data.
		 *\param[in]	p_pixel	The picking data.
		 *\param[in]	p_nodes	The render nodes.
		 *\return		PickingPass::NodeType::eNone if nothing was picked.
		 *\~french
		 *\brief		Picke le noeud aux données de picking fournies.
		 *\param[in]	p_pixel	Les données de picking.
		 *\param[in]	p_nodes	Les noeuds de rendu.
		 *\return		PickingPass::NodeType si rien n'a été pické.
		 */
		PickingPass::NodeType doPick( castor::Point3f const & p_pixel
			, SceneRenderNodes & p_nodes );
		/**
		 *\copydoc		castor3d::RenderPass::doRender
		 */
		void doUpdate( Scene const & p_scene
			, SubmeshStaticRenderNodesByPipelineMap & p_nodes );
		/**
		 *\copydoc		castor3d::RenderPass::doRender
		 */
		void doUpdate( Scene const & p_scene
			, StaticRenderNodesByPipelineMap & p_nodes );
		/**
		 *\copydoc		castor3d::RenderPass::doRenderAnimatedSubmeshes
		 */
		void doUpdate( Scene const & p_scene
			, SkinningRenderNodesByPipelineMap & p_nodes );
		/**
		 *\copydoc		castor3d::RenderPass::doRender
		 */
		void doUpdate( Scene const & p_scene
			, SubmeshSkinningRenderNodesByPipelineMap & p_nodes );
		/**
		 *\copydoc		castor3d::RenderPass::doRenderAnimatedSubmeshes
		 */
		void doUpdate( Scene const & p_scene
			, MorphingRenderNodesByPipelineMap & p_nodes );
		/**
		 *\copydoc		castor3d::RenderPass::doRender
		 */
		void doUpdate( Scene const & p_scene
			, BillboardRenderNodesByPipelineMap & p_nodes );
		/**
		 *\copydoc		castor3d::RenderPass::doInitialise
		 */
		bool doInitialise( castor::Size const & p_size )override;
		/**
		 *\copydoc		castor3d::RenderPass::doCleanup
		 */
		void doCleanup()override;
		/**
		 *\copydoc		castor3d::RenderPass::doUpdate
		 */
		void doUpdate( RenderQueueArray & p_queues )override;
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
		void doUpdatePipeline( RenderPipeline & p_pipeline )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doPrepareFrontPipeline
		 */
		void doPrepareFrontPipeline( renderer::ShaderStageStateArray & p_program
			, PipelineFlags const & p_flags )override;
		/**
		 *\copydoc		castor3d::RenderPass::doPrepareBackPipeline
		 */
		void doPrepareBackPipeline( renderer::ShaderStageStateArray & p_program
			, PipelineFlags const & p_flags )override;
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
