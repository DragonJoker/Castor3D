/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___C3D_PICKING_PASS_H___
#define ___C3D_PICKING_PASS_H___

#include "Mesh/Buffer/BufferDeclaration.hpp"
#include "Render/RenderPass.hpp"
#include "Render/Viewport.hpp"
#include "Scene/Geometry.hpp"

namespace castor3d
{
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
			eInstantiated,
			eStatic,
			eSkinning,
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
		C3D_API PickingPass( Engine & engine );
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
		void doRender( Scene & p_scene
			, SubmeshStaticRenderNodesByPipelineMap & p_nodes );
		/**
		 *\copydoc		castor3d::RenderPass::doRender
		 */
		void doRender( Scene & p_scene
			, StaticRenderNodesByPipelineMap & p_nodes );
		/**
		 *\copydoc		castor3d::RenderPass::doRenderAnimatedSubmeshes
		 */
		void doRender( Scene & p_scene
			, SkinningRenderNodesByPipelineMap & p_nodes );
		/**
		 *\copydoc		castor3d::RenderPass::doRender
		 */
		void doRender( Scene & p_scene
			, SubmeshSkinningRenderNodesByPipelineMap & p_nodes );
		/**
		 *\copydoc		castor3d::RenderPass::doRenderAnimatedSubmeshes
		 */
		void doRender( Scene & p_scene
			, MorphingRenderNodesByPipelineMap & p_nodes );
		/**
		 *\copydoc		castor3d::RenderPass::doRender
		 */
		void doRender( Scene & p_scene
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
		glsl::Shader doGetGeometryShaderSource( TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetLegacyPixelShaderSource
		 */
		glsl::Shader doGetLegacyPixelShaderSource( TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, ComparisonFunc alphaFunc )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetPbrMRPixelShaderSource
		 */
		glsl::Shader doGetPbrMRPixelShaderSource( TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, ComparisonFunc alphaFunc )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetPbrSGPixelShaderSource
		 */
		glsl::Shader doGetPbrSGPixelShaderSource( TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, ComparisonFunc alphaFunc )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetPixelShaderSource
		 */
		glsl::Shader doGetPixelShaderSource( TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, ComparisonFunc alphaFunc )const;
		/**
		 *\copydoc		castor3d::RenderPass::doUpdatePipeline
		 */
		void doUpdatePipeline( RenderPipeline & p_pipeline )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doPrepareFrontPipeline
		 */
		void doPrepareFrontPipeline( ShaderProgram & p_program
			, PipelineFlags const & p_flags )override;
		/**
		 *\copydoc		castor3d::RenderPass::doPrepareBackPipeline
		 */
		void doPrepareBackPipeline( ShaderProgram & p_program
			, PipelineFlags const & p_flags )override;
		/**
		 *\copydoc		castor3d::RenderPass::doUpdateFlags
		 */
		void doUpdateFlags( TextureChannels & textureFlags
			, ProgramFlags & programFlags
			, SceneFlags & sceneFlags )const override;

	private:
		using CameraQueueMap = std::map< Camera const *, RenderQueue >;

	private:
		//!\~english	The scenes, and cameras used to render them.
		//!\~french		Les scènes, et les caméras utilisées pour les dessiner.
		std::map< castor::String, GeometryWPtr > m_pickable;
		//!\~english	The texture receiving the color render.
		//!\~french		La texture recevant le rendu couleur.
		TextureLayoutSPtr m_colourTexture;
		//!\~english	The buffer receiving the depth render.
		//!\~french		Le tampon recevant le rendu profondeur.
		DepthStencilRenderBufferSPtr m_depthBuffer;
		//!\~english	The frame buffer.
		//!\~french		Le tampon d'image.
		FrameBufferSPtr m_frameBuffer;
		//!\~english	The attach between texture and main frame buffer.
		//!\~french		L'attache entre la texture et le tampon principal.
		TextureAttachmentSPtr m_colourAttach;
		//!\~english	The attach between depth buffer and main frame buffer.
		//!\~french		L'attache entre le tampon profondeur et le tampon principal.
		RenderBufferAttachmentSPtr m_depthAttach;
		//!\~english	The scenes, and cameras used to render them.
		//!\~french		Les scènes, et les caméras utilisées pour les dessiner.
		std::map< Scene const *, CameraQueueMap > m_scenes;
		//!\~english	The picked geometry.
		//!\~french		La géométrie sélectionnée.
		GeometryWPtr m_geometry;
		//!\~english	The picked geometry.
		//!\~french		La géométrie sélectionnée.
		BillboardBaseWPtr m_billboard;
		//!\~english	The picked submesh.
		//!\~french		Le sous-maillage sélectionné.
		SubmeshWPtr m_submesh;
		//!\~english	The picked face index.
		//!\~french		L'indice de la face sélectionnée.
		uint32_t m_face;
		//!\~english	The picking data UBO.
		//!\~french		L'UBO de données de picking.
		UniformBuffer m_pickingUbo;
		//!\~english	Receives the picking area pixels.
		//!\~french		Reçoit les pixels de la zone pickée.
		castor::PxBufferBaseSPtr m_buffer;
	};
}

#endif
