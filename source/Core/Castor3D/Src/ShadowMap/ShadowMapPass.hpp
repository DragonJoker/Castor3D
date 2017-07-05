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
		 *\param[in]	p_light		The light source.
		 *\param[in]	p_shadowMap	The parent shadow map.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_engine	Le moteur.
		 *\param[in]	p_light		La source lumineuse.
		 *\param[in]	p_shadowMap	La shadow map parente.
		 */
		C3D_API ShadowMapPass( Engine & p_engine
			, Light & p_light
			, ShadowMap const & p_shadowMap );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~ShadowMapPass();
		/**
		 *\~english
		 *\brief		Render function
		 *\~french
		 *\brief		Fonction de rendu.
		 */
		C3D_API void Render( uint32_t p_face = 0 );
		/**
		 *\~english
		 *\brief		Updates the render pass.
		 *\remarks		Gather the render queues, for further update.
		 *\param[out]	p_queues	Receives the render queues needed for the rendering of the frame.
		 *\param[out]	p_index		The pass index.
		 *\~french
		 *\brief		Met à jour la passe de rendu.
		 *\remarks		Récupère les files de rendu, pour mise à jour ultérieure.
		 *\param[out]	p_queues	Reçoit les files de rendu nécessaires pour le dessin de la frame.
		 *\param[out]	p_index		L'indice de la passe.
		 */
		C3D_API void Update( RenderQueueArray & p_queues
			, int32_t p_index );

	protected:
		/**
		 *\~english
		 *\brief		Renders the given nodes.
		 *\param		p_nodes		The nodes to render.
		 *\param		p_camera	The viewing camera.
		 *\~french
		 *\brief		Dessine les noeuds donnés.
		 *\param		p_nodes		Les noeuds à dessiner.
		 *\param		p_camera	La caméra regardant la scène.
		 */
		void DoRenderNodes( SceneRenderNodes & p_nodes
			, Camera const & p_camera );
		/**
		 *\~english
		 *\brief			Modifies the given flags to make them match the render pass requirements.
		 *\param[in,out]	p_textureFlags	A combination of TextureChannel.
		 *\param[in,out]	p_programFlags	A combination of ProgramFlag.
		 *\param[in,out]	p_sceneFlags	A combination of SceneFlag.
		 *\~french
		 *\brief			Modifie les indicateurs donnés pour le faire correspondre au pré-requis de la passe de rendus.
		 *\param[in,out]	p_textureFlags	Une combinaison de TextureChannel.
		 *\param[in,out]	p_programFlags	Une combinaison de ProgramFlag.
		 *\param[in,out]	p_sceneFlags	Une combinaison de SceneFlag.
		 */
		void DoUpdateFlags( TextureChannels & p_textureFlags
			, ProgramFlags & p_programFlags
			, SceneFlags & p_sceneFlags )const override;

	private:
		/**
		 *\~english
		 *\brief		Render function.
		 *\param[in]	p_scene		The scene to render.
		 *\param[in]	p_camera	The camera through which the scene is viewed.
		 *\~french
		 *\brief		Fonction de rendu.
		 *\param[in]	p_scene		La scène à dessiner.
		 *\param[in]	p_camera	La caméra à travers laquelle la scène est vue.
		 */
		C3D_API virtual void DoRender( uint32_t p_face ) = 0;
		/**
		 *\~english
		 *\brief		Cleans up the pass.
		 *\~french
		 *\brief		Nettoie la passe.
		 */
		virtual void DoCleanup()= 0;
		/**
		 *\copydoc		Castor3D::RenderPass::DoUpdatePipeline
		 */
		void DoUpdatePipeline( RenderPipeline & p_pipeline )const override;
		/**
		 *\copydoc		Castor3D::RenderPass::DoPrepareFrontPipeline
		 */
		void DoPrepareFrontPipeline( ShaderProgram & p_program
			, PipelineFlags const & p_flags )override;
		/**
		 *\copydoc		Castor3D::RenderPass::DoPrepareBackPipeline
		 */
		void DoPrepareBackPipeline( ShaderProgram & p_program
			, PipelineFlags const & p_flags )override;
		/**
		 *\copydoc		Castor3D::RenderPass::DoGetVertexShaderSource
		 */
		GLSL::Shader DoGetVertexShaderSource( TextureChannels const & p_textureFlags
			, ProgramFlags const & p_programFlags
			, SceneFlags const & p_sceneFlags
			, bool p_invertNormals )const override;
		/**
		 *\copydoc		Castor3D::ShadowMap::DoGetGeometryShaderSource
		 */
		GLSL::Shader DoGetGeometryShaderSource( TextureChannels const & p_textureFlags
			, ProgramFlags const & p_programFlags
			, SceneFlags const & p_sceneFlags )const override;
		/**
		 *\copydoc		Castor3D::RenderPass::DoGetLegacyPixelShaderSource
		 */
		GLSL::Shader DoGetLegacyPixelShaderSource( TextureChannels const & p_textureFlags
			, ProgramFlags const & p_programFlags
			, SceneFlags const & p_sceneFlags
			, ComparisonFunc p_alphaFunc )const override;
		/**
		 *\copydoc		Castor3D::RenderPass::DoGetPbrPixelShaderSource
		 */
		GLSL::Shader DoGetPbrPixelShaderSource( TextureChannels const & p_textureFlags
			, ProgramFlags const & p_programFlags
			, SceneFlags const & p_sceneFlags
			, ComparisonFunc p_alphaFunc )const override;

	protected:
		//!\~english	The parent shadow map.
		//!\~french		La shadow map parente.
		ShadowMap const & m_shadowMap;
		//!\~english	The light source.
		//!\~french		La source lumineuse.
		Light & m_light;
		//!\~english	Tells if the pass is initialised.
		//!\~french		Dit si la passe est initialisée.
		bool m_initialised{ false };
		//!\~english	The pass index.
		//!\~french		L'indice de la passe.
		int32_t m_index{ 0u };
	};
}

#endif
