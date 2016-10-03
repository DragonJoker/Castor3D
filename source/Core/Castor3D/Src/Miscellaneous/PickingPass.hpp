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

namespace Castor3D
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
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_engine	Le moteur.
		 */
		C3D_API PickingPass( Engine & p_engine );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~PickingPass();
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
		 *\brief		Picks a geometry at given mouse position.
		 *\param[in]	p_position		The position in the pass.
		 *\param[in]	p_camera		The viewing camera.
		 *\return		\p true if something was picked.
		 *\~french
		 *\brief		Sélectionne la géométrie à la position de souris donnée.
		 *\param[in]	p_position		La position dans la passe.
		 *\param[in]	p_camera		La caméra regardant la scène.
		 *\return		\p true si quelque chose a été sélectionné.
		 */
		C3D_API bool Pick( Castor::Position const & p_position, Camera const & p_camera );
		/**
		*\~english
		*\return		The picked geometry.
		*\~french
		*\return		La géométrie sélectionnée.
		*/
		inline GeometrySPtr GetPickedGeometry()const
		{
			return m_geometry.lock();
		}
		/**
		*\~english
		*\return		The picked submesh.
		*\~french
		*\return		Le sous-maillage sélectionné.
		*/
		inline SubmeshSPtr GetPickedSubmesh()const
		{
			return m_submesh.lock();
		}
		/**
		*\~english
		*\return		The picked face index.
		*\~french
		*\return		L'indice de la face sélectionnée.
		*/
		inline uint32_t GetPickedFace()const
		{
			return m_face;
		}
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

	private:
		void DoRenderOpaqueNodes( SceneRenderNodes & p_nodes, Camera const & p_camera );
		void DoRenderTransparentNodes( SceneRenderNodes & p_nodes, Camera const & p_camera );

	private:
		/**
		 *\~copydoc		Castor3D::RenderPass::DoRenderStaticSubmeshesNonInstanced
		 */
		void DoRenderOpaqueInstancedSubmeshesInstanced( Scene & p_scene, Camera const & p_camera, uint8_t p_index, SubmeshStaticRenderNodesByPipelineMap & p_nodes );
		/**
		 *\~copydoc		Castor3D::RenderPass::DoRenderStaticSubmeshesNonInstanced
		 */
		void DoRenderOpaqueStaticSubmeshesNonInstanced( Scene & p_scene, Camera const & p_camera, uint8_t p_index, StaticGeometryRenderNodesByPipelineMap & p_nodes );
		/**
		 *\~copydoc		Castor3D::RenderPass::DoRenderAnimatedSubmeshesNonInstanced
		 */
		void DoRenderOpaqueAnimatedSubmeshesNonInstanced( Scene & p_scene, Camera const & p_camera, uint8_t p_index, AnimatedGeometryRenderNodesByPipelineMap & p_nodes );
		/**
		 *\~copydoc		Castor3D::RenderPass::DoRenderBillboards
		 */
		void DoRenderOpaqueBillboards( Scene & p_scene, Camera const & p_camera, uint8_t p_index, BillboardRenderNodesByPipelineMap & p_nodes );
		/**
		 *\~copydoc		Castor3D::RenderPass::DoRenderStaticSubmeshesNonInstanced
		 */
		void DoRenderTransparentInstancedSubmeshesInstanced( Scene & p_scene, Camera const & p_camera, uint8_t p_index, SubmeshStaticRenderNodesByPipelineMap & p_nodes );
		/**
		 *\~copydoc		Castor3D::RenderPass::DoRenderStaticSubmeshesNonInstanced
		 */
		void DoRenderTransparentStaticSubmeshesNonInstanced( Scene & p_scene, Camera const & p_camera, uint8_t p_index, StaticGeometryRenderNodesByPipelineMap & p_nodes );
		/**
		 *\~copydoc		Castor3D::RenderPass::DoRenderAnimatedSubmeshesNonInstanced
		 */
		void DoRenderTransparentAnimatedSubmeshesNonInstanced( Scene & p_scene, Camera const & p_camera, uint8_t p_index, AnimatedGeometryRenderNodesByPipelineMap & p_nodes );
		/**
		 *\~copydoc		Castor3D::RenderPass::DoRenderBillboards
		 */
		void DoRenderTransparentBillboards( Scene & p_scene, Camera const & p_camera, uint8_t p_index, BillboardRenderNodesByPipelineMap & p_nodes );
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
		void DoUpdateOpaquePipeline( Camera const & p_camera, Pipeline & p_pipeline )const override;
		/**
		 *\copydoc		Castor3D::RenderPass::DoUpdateTransparentPipeline
		 */
		void DoUpdateTransparentPipeline( Camera const & p_camera, Pipeline & p_pipeline )const override;
		/**
		 *\~copydoc		Castor3D::RenderPass::DoPrepareOpaquePipeline
		 */
		Pipeline & DoPrepareOpaquePipeline( ShaderProgram & p_program, PipelineFlags const & p_flags )override;
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
		//!\~english	The scenes, and cameras used to render them.
		//!\~french		Les scènes, et les caméras utilisées pour les dessiner.
		std::map< Castor::String, GeometryWPtr > m_pickable;
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
		//!\~english	The geometry buffer.
		//!\~french		Les tampons de géométrie.
		std::set< GeometryBuffersSPtr > m_geometryBuffers;
		//!\~english	The picked geometry.
		//!\~french		La géométrie sélectionnée.
		GeometryWPtr m_geometry;
		//!\~english	The picked submesh.
		//!\~french		Le sous-maillage sélectionné.
		SubmeshWPtr m_submesh;
		//!\~english	The picked face index.
		//!\~french		L'indice de la face sélectionnée.
		uint32_t m_face;
	};
}

#endif
