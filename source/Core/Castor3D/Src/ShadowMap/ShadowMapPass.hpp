﻿/*
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

namespace castor3d
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
		 *\param[in]	engine		The engine.
		 *\param[in]	scene		The scene.
		 *\param[in]	shadowMap	The parent shadow map.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	scene		La scène.
		 *\param[in]	shadowMap	La shadow map parente.
		 */
		C3D_API ShadowMapPass( Engine & engine
			, Scene & scene
			, ShadowMap const & shadowMap );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~ShadowMapPass();
		/**
		 *\~english
		 *\brief		Updates the render pass.
		 *\remarks		Gather the render queues, for further update.
		 *\param[in]	camera	The viewer camera.
		 *\param[out]	queues	Receives the render queues needed for the rendering of the frame.
		 *\param[out]	light	The light source.
		 *\param[out]	index	The pass index.
		 *\~french
		 *\brief		Met à jour la passe de rendu.
		 *\remarks		Récupère les files de rendu, pour mise à jour ultérieure.
		 *\param[in]	camera	La caméra de l'observateur.
		 *\param[out]	queues	Reçoit les files de rendu nécessaires pour le dessin de la frame.
		 *\param[out]	light	La source lumineuse.
		 *\param[out]	index	L'indice de la passe.
		 */
		C3D_API virtual void update( Camera const & camera
			, RenderQueueArray & queues
			, Light & light
			, uint32_t index ) = 0;
		/**
		 *\~english
		 *\brief		Render function.
		 *\param[in]	index	The render index.
		 *\~french
		 *\brief		Fonction de rendu.
		 *\param[in]	index	L'indice du rendu.
		 */
		C3D_API virtual void render( uint32_t index = 0 ) = 0;

	protected:
		/**
		 *\~english
		 *\brief		Renders the given nodes.
		 *\param		nodes	The nodes to render.
		 *\param		camera	The viewing camera.
		 *\~french
		 *\brief		Dessine les noeuds donnés.
		 *\param		nodes	Les noeuds à dessiner.
		 *\param		camera	La caméra regardant la scène.
		 */
		void doRenderNodes( SceneRenderNodes & nodes
			, Camera const & camera );
		/**
		 *\~english
		 *\brief			Modifies the given flags to make them match the render pass requirements.
		 *\param[in,out]	textureFlags	A combination of TextureChannel.
		 *\param[in,out]	programFlags	A combination of ProgramFlag.
		 *\param[in,out]	sceneFlags	A combination of SceneFlag.
		 *\~french
		 *\brief			Modifie les indicateurs donnés pour le faire correspondre au pré-requis de la passe de rendus.
		 *\param[in,out]	textureFlags	Une combinaison de TextureChannel.
		 *\param[in,out]	programFlags	Une combinaison de ProgramFlag.
		 *\param[in,out]	sceneFlags	Une combinaison de SceneFlag.
		 */
		void doUpdateFlags( TextureChannels & textureFlags
			, ProgramFlags & programFlags
			, SceneFlags & sceneFlags )const override;

	private:
		/**
		 *\~english
		 *\brief		Cleans up the pass.
		 *\~french
		 *\brief		Nettoie la passe.
		 */
		virtual void doCleanup()= 0;
		/**
		 *\~english
		 *\brief		Prepares the pipeline, culling back faces.
		 *\~french
		 *\brief		Prépare le pipeline de rendu, en supprimant les faces arrière.
		 */
		virtual void doPreparePipeline( ShaderProgram & program
			, PipelineFlags const & flags );
		/**
		 *\copydoc		castor3d::RenderPass::doUpdatePipeline
		 */
		void doUpdatePipeline( RenderPipeline & pipeline )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doPrepareFrontPipeline
		 */
		void doPrepareFrontPipeline( ShaderProgram & program
			, PipelineFlags const & flags )override;
		/**
		 *\copydoc		castor3d::RenderPass::doPrepareBackPipeline
		 */
		void doPrepareBackPipeline( ShaderProgram & program
			, PipelineFlags const & flags )override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetVertexShaderSource
		 */
		glsl::Shader doGetVertexShaderSource( TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, bool invertNormals )const override;
		/**
		 *\copydoc		castor3d::ShadowMap::doGetGeometryShaderSource
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

	protected:
		//!\~english	The scene.
		//!\~french		La scène.
		Scene & m_scene;
		//!\~english	The parent shadow map.
		//!\~french		La shadow map parente.
		ShadowMap const & m_shadowMap;
		//!\~english	Tells if the pass is initialised.
		//!\~french		Dit si la passe est initialisée.
		bool m_initialised{ false };
	};
}

#endif
