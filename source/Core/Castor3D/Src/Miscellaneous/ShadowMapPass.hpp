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
#ifndef ___C3D_SHADOW_MAP_PASS_H___
#define ___C3D_SHADOW_MAP_PASS_H___

#include "Mesh/Buffer/BufferDeclaration.hpp"
#include "Render/RenderPass.hpp"
#include "Render/Viewport.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Geometry.hpp"
#include "Texture/TextureUnit.hpp"

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		30/08/2016
	\~english
	\brief		Shadow mapping implementation.
	\~french
	\brief		Implémentation du mappage d'ombres.
	*/
	class ShadowMapPass
		: public RenderPass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_engine	Le moteur.
		 */
		C3D_API ShadowMapPass( Engine & p_engine, Scene & p_scene, Light const & p_light );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~ShadowMapPass();
		/**
		 *\~english
		 *\brief		Initialises the pipeline, FBO and program.
		 *\param		p_size	The FBO dimensions.
		 *\return		\p true if ok.
		 *\~french
		 *\brief		Initialise le pipeline, le FBO et le programme.
		 *\param		p_size	Les dimensions du FBO.
		 *\return		\p true if ok.
		 */
		C3D_API bool Initialise( Castor::Size const & p_size );
		/**
		 *\~english
		 *\brief		Cleanup function.
		 *\~french
		 *\brief		Fonction de nettoyage.
		 */
		C3D_API void Cleanup();
		/**
		 *\~english
		 *\brief		Update function.
		 *\remarks		Updates the scenes render nodes, if needed.
		 *\~french
		 *\brief		Fonction de mise à jour.
		 *\remarks		Met les noeuds de scènes à jour, si nécessaire.
		 */
		C3D_API void Update();
		/**
		 *\~english
		 *\brief		Render function
		 *\param[in]	p_scene		The scene to render.
		 *\param[in]	p_camera	The camera through which the scene is viewed.
		 *\~french
		 *\brief		Fonction de rendu.
		 *\param[in]	p_scene		La scène à dessiner.
		 *\param[in]	p_camera	La caméra à travers laquelle la scène est vue.
		 */
		C3D_API void Render();
		/**
		 *\~copydoc		Castor3D::RenderPass::CreateAnimatedNode
		 */
		C3D_API AnimatedGeometryRenderNode CreateAnimatedNode( Pass & p_pass
															   , Pipeline & p_pipeline
															   , Submesh & p_submesh
															   , Geometry & p_primitive
															   , AnimatedSkeletonSPtr p_skeleton
															   , AnimatedMeshSPtr p_mesh )override;
		/**
		 *\~copydoc		Castor3D::RenderPass::CreateStaticNode
		 */
		C3D_API StaticGeometryRenderNode CreateStaticNode( Pass & p_pass
														   , Pipeline & p_pipeline
														   , Submesh & p_submesh
														   , Geometry & p_primitive )override;
		/**
		 *\~copydoc		Castor3D::RenderPass::CreateBillboardNode
		 */
		C3D_API BillboardRenderNode CreateBillboardNode( Pass & p_pass
														 , Pipeline & p_pipeline
														 , BillboardList & p_billboard )override;
		/**
		 *\~english
		 *\return		The depth texture holding the render's result.
		 *\~french
		 *\return		La texture de profondeur contenant le résultat du rendu.
		 */
		inline TextureUnit const & GetResult()const
		{
			return m_depthTexture;
		}
		/**
		 *\~english
		 *\return		The camera's viewport.
		 *\~french
		 *\return		Le viewport de la caméra.
		 */
		inline Viewport const & GetViewport()const
		{
			return m_camera->GetViewport();
		}

	private:
		void DoRenderOpaqueNodes( SceneRenderNodes & p_nodes, Camera const & p_camera );
		void DoRenderTransparentNodes( SceneRenderNodes & p_nodes, Camera const & p_camera );

	private:
		/**
		 *\~copydoc		Castor3D::RenderPass::DoRenderStaticSubmeshesNonInstanced
		 */
		void DoRenderInstancedSubmeshesInstanced( Scene & p_scene, Camera const & p_camera, SubmeshStaticRenderNodesByPipelineMap & p_nodes );
		/**
		 *\~copydoc		Castor3D::RenderPass::DoRenderStaticSubmeshesNonInstanced
		 */
		void DoRenderStaticSubmeshesNonInstanced( Scene & p_scene, Camera const & p_camera, StaticGeometryRenderNodesByPipelineMap & p_nodes );
		/**
		 *\~copydoc		Castor3D::RenderPass::DoRenderAnimatedSubmeshesNonInstanced
		 */
		void DoRenderAnimatedSubmeshesNonInstanced( Scene & p_scene, Camera const & p_camera, AnimatedGeometryRenderNodesByPipelineMap & p_nodes );
		/**
		 *\~copydoc		Castor3D::RenderPass::DoRenderBillboards
		 */
		void DoRenderBillboards( Scene & p_scene, Camera const & p_camera, BillboardRenderNodesByPipelineMap & p_nodes );
		/**
		 *\~copydoc		Castor3D::RenderPass::DoGetOpaquePixelShaderSource
		 */
		Castor::String DoGetOpaquePixelShaderSource( uint16_t p_textureFlags, uint8_t p_programFlags, uint8_t p_sceneFlags )const override;
		/**
		 *\~copydoc		Castor3D::RenderPass::DoGetTransparentPixelShaderSource
		 */
		Castor::String DoGetTransparentPixelShaderSource( uint16_t p_textureFlags, uint8_t p_programFlags, uint8_t p_sceneFlags )const override;
		/**
		 *\copydoc		Castor3D::RenderPass::DoUpdateOpaquePipeline
		 */
		void DoUpdateOpaquePipeline( Camera const & p_camera, Pipeline & p_pipeline, DepthMapArray const & p_depthMaps )const override;
		/**
		 *\copydoc		Castor3D::RenderPass::DoUpdateTransparentPipeline
		 */
		void DoUpdateTransparentPipeline( Camera const & p_camera, Pipeline & p_pipeline, DepthMapArray const & p_depthMaps )const override;
		/**
		 *\~copydoc		Castor3D::RenderPass::DoPrepareOpaqueFrontPipeline
		 */
		Pipeline & DoPrepareOpaqueFrontPipeline( ShaderProgram & p_program, PipelineFlags const & p_flags )override;
		/**
		 *\~copydoc		Castor3D::RenderPass::DoPrepareOpaqueBackPipeline
		 */
		Pipeline & DoPrepareOpaqueBackPipeline( ShaderProgram & p_program, PipelineFlags const & p_flags )override;
		/**
		 *\~copydoc		Castor3D::RenderPass::DoPrepareTransparentFrontPipeline
		 */
		Pipeline & DoPrepareTransparentFrontPipeline( ShaderProgram & p_program, PipelineFlags const & p_flags )override;
		/**
		 *\~copydoc		Castor3D::RenderPass::DoPrepareTransparentBackPipeline
		 */
		Pipeline & DoPrepareTransparentBackPipeline( ShaderProgram & p_program, PipelineFlags const & p_flags )override;
		/**
		 *\~copydoc		Castor3D::RenderPass::DoCompleteProgramFlags
		 */
		void DoCompleteProgramFlags( uint8_t & p_programFlags )const override;

	private:
		//!\~english	The scene.
		//!\~french		La scène.
		Scene & m_scene;
		//!\~english	The light used to generate the shadows.
		//!\~french		La lumière utilisée pour générer les ombres.
		Light const & m_light;
		//!\~english	The camera created from the light.
		//!\~french		La caméra créée à partir de la lumière.
		CameraSPtr m_camera;
		//!\~english	The camera node.
		//!\~french		Le noeud de la caméra.
		SceneNodeSPtr m_cameraNode;
		//!\~english	The view matrix.
		//!\~french		La matrice vue.
		Castor::Matrix4x4r m_view;
		//!\~english	The texture receiving the render.
		//!\~french		La texture recevant le rendu.
		TextureUnit m_depthTexture;
		//!\~english	The frame buffer.
		//!\~french		Le tampon d'image.
		FrameBufferSPtr m_frameBuffer;
		//!\~english	The attach between depth buffer and main frame buffer.
		//!\~french		L'attache entre le tampon profondeur et le tampon principal.
		TextureAttachmentSPtr m_depthAttach;
		//!\~english	The geometry buffer.
		//!\~french		Les tampons de géométrie.
		std::set< GeometryBuffersSPtr > m_geometryBuffers;
	};
}

#endif
