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
#ifndef ___C3D_SHADOW_MAP_PASS_POINT_H___
#define ___C3D_SHADOW_MAP_PASS_POINT_H___

#include "ShadowMapPass.hpp"

#define USE_GS_POINT_SHADOW_MAPS 1

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
	class ShadowMapPassPoint
		: public ShadowMapPass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_engine	The engine.
		 *\param[in]	p_scene		The scene.
		 *\param[in]	p_light		The light.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_engine	Le moteur.
		 *\param[in]	p_scene		La scène.
		 *\param[in]	p_light		La source lumineuse.
		 */
		C3D_API ShadowMapPassPoint( Engine & p_engine, Scene & p_scene, Light & p_light, TextureUnit & p_shadowMap, uint32_t p_index );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~ShadowMapPassPoint();
		/**
		 *\~english
		 *\brief		Creator function, used by the factory.
		 *\param[in]	p_engine	The engine.
		 *\param[in]	p_scene		The scene.
		 *\param[in]	p_light		The light.
		 *\~french
		 *\brief		La fonction de création utilisée par la fabrique.
		 *\param[in]	p_engine	Le moteur.
		 *\param[in]	p_scene		La scène.
		 *\param[in]	p_light		La source lumineuse.
		 */
		C3D_API static ShadowMapPassSPtr Create( Engine & p_engine, Scene & p_scene, Light & p_light, TextureUnit & p_shadowMap, uint32_t p_index );

	protected:
		void DoRenderNodes( SceneRenderNodes & p_nodes );

	private:
		/**
		 *\copydoc		Castor3D::ShadowMapPass::DoInitialise
		 */
		bool DoInitialisePass( Castor::Size const & p_size )override;
		/**
		 *\copydoc		Castor3D::ShadowMapPass::DoCleanup
		 */
		void DoCleanupPass()override;
		/**
		 *\copydoc		Castor3D::ShadowMapPass::DoUpdate
		 */
		void DoUpdate( RenderQueueArray & p_queues )override;
		/**
		 *\copydoc		Castor3D::ShadowMapPass::DoRender
		 */
		void DoRender()override;
		/**
		 *\copydoc		Castor3D::ShadowMapPass::DoUpdateProgram
		 */
		void DoUpdateProgram( ShaderProgram & p_program )override;
		/**
		 *\copydoc		Castor3D::RenderPass::DoPrepareBackPipeline
		 */
		void DoPrepareBackPipeline( ShaderProgram & p_program
			, PipelineFlags const & p_flags )override;
		/**
		 *\copydoc		Castor3D::RenderPass::DoGetVertexShaderSource
		 */
		Castor::String DoGetVertexShaderSource( TextureChannels const & p_textureFlags
			, ProgramFlags const & p_programFlags
			, uint8_t p_sceneFlags
			, bool p_invertNormals )const override;
		/**
		 *\copydoc		Castor3D::RenderPass::DoGetGeometryShaderSource
		 */
		Castor::String DoGetGeometryShaderSource( TextureChannels const & p_textureFlags
			, ProgramFlags const & p_programFlags
			, uint8_t p_sceneFlags )const override;
		/**
		 *\copydoc		Castor3D::RenderPass::DoGetPixelShaderSource
		 */
		Castor::String DoGetPixelShaderSource( TextureChannels const & p_textureFlags
			, ProgramFlags const & p_programFlags
			, uint8_t p_sceneFlags )const override;

	private:
		//!\~english	The connection to light's node changed signal.
		//!\~french		La connexion au signal de changement du noeud de la source lumineuse.
		SceneNode::OnChanged::Connection m_onNodeChanged;
		//!\~english	The projection matrix.
		//!\~french		La matrice de projection.
		Castor::Matrix4x4r m_projection;
		//!\~english	The shadow map matrices data UBO.
		//!\~french		L'UBO de données des matrices de shadow map.
		UniformBuffer m_shadowMatrices;
		//!\~english	The shadow map coniguration data UBO.
		//!\~french		L'UBO de données de configuration de shadow map.
		UniformBuffer m_shadowConfig;

#if USE_GS_POINT_SHADOW_MAPS

		//!\~english	The attach between depth buffer and main frame buffer.
		//!\~french		L'attache entre le tampon profondeur et le tampon principal.
		TextureAttachmentSPtr m_depthAttach;

#else

		//!\~english	The attaches between depth buffer and main frame buffer.
		//!\~french		Les attaches entre le tampon profondeur et le tampon principal.
		std::array< TextureAttachmentSPtr, size_t( CubeMapFace::eCount ) > m_depthAttachs;

#endif
	};
}

#endif
