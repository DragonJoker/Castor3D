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
#ifndef ___C3D_DepthPass_H___
#define ___C3D_DepthPass_H___

#include "Render/RenderPass.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		12/11/2012
	\~english
	\brief		Deferred lighting Render technique pass.
	\~french
	\brief		Classe de passe de technique de rendu implémentant le Deferred lighting.
	*/
	class DepthPass
		: public RenderPass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor for opaque nodes.
		 *\param[in]	p_name		This pass name.
		 *\param[in]	p_scene		The scene for this technique.
		 *\param[in]	p_camera	The camera for this technique (may be null).
		 *\~french
		 *\brief		Constructeur pour les noeuds opaques.
		 *\param[in]	p_name		Le nom de cette passe.
		 *\param[in]	p_scene		La scène pour cette technique.
		 *\param[in]	p_camera	La caméra pour cette technique (peut être nulle).
		 */
		DepthPass( castor::String const & p_name
			, Scene & p_scene
			, Camera * p_camera );
		/**
		 *\~english
		 *\brief		Constructor for transparent nodes.
		 *\param[in]	p_name		This pass name.
		 *\param[in]	p_scene		The scene for this technique.
		 *\param[in]	p_camera	The camera for this technique (may be null).
		 *\~french
		 *\brief		Constructeur pour les noeuds transparents.
		 *\param[in]	p_name		Le nom de cette passe.
		 *\param[in]	p_scene		La scène pour cette technique.
		 *\param[in]	p_camera	La caméra pour cette technique (peut être nulle).
		 */
		DepthPass( castor::String const & p_name
			, Scene & p_scene
			, Camera * p_camera
			, bool p_oit );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~DepthPass();
		/**
		 *\~english
		 *\brief		Render function.
		 *\~french
		 *\brief		Fonction de rendu.
		 */
		void render();

	private:
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
		 *\copydoc		castor3d::RenderPass::doUpdateFlags
		 */
		void doUpdateFlags( TextureChannels & textureFlags
			, ProgramFlags & programFlags
			, SceneFlags & sceneFlags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doUpdatePipeline
		 */
		C3D_API void doUpdatePipeline( RenderPipeline & p_pipeline)const override;
		/**
		 *\copydoc		castor3d::RenderPass::doPrepareFrontPipeline
		 */
		C3D_API void doPrepareFrontPipeline( ShaderProgram & p_program
			, PipelineFlags const & p_flags )override;
		/**
		 *\copydoc		castor3d::RenderPass::doPrepareBackPipeline
		 */
		C3D_API void doPrepareBackPipeline( ShaderProgram & p_program
			, PipelineFlags const & p_flags )override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetVertexShaderSource
		 */
		C3D_API glsl::Shader doGetVertexShaderSource( TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, bool invertNormals )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetGeometryShaderSource
		 */
		C3D_API glsl::Shader doGetGeometryShaderSource( TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetLegacyPixelShaderSource
		 */
		C3D_API glsl::Shader doGetLegacyPixelShaderSource( TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, ComparisonFunc alphaFunc )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetPbrMRPixelShaderSource
		 */
		C3D_API glsl::Shader doGetPbrMRPixelShaderSource( TextureChannels const & textureFlags
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
		C3D_API glsl::Shader doGetPixelShaderSource( TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, ComparisonFunc alphaFunc )const;

	private:
		/**
		 *\~english
		 *\brief			Renders render nodes.
		 *\param[in]		p_nodes		The scene render nodes.
		 *\param			p_camera	The viewing camera.
		 *\~french
		 *\brief			Dessine les noeuds de rendu.
		 *\param[in]		p_nodes		Les noeuds de rendu de la scène.
		 *\param			p_camera	La caméra regardant la scène.
		 */
		C3D_API void doRenderNodes( SceneRenderNodes & p_nodes
			, Camera const & p_camera )const;

	private:
		//!\~english	The rendered scne.
		//!\~french		La scène rendue.
		Scene & m_scene;
		//!\~english	The viewer camera, if any.
		//!\~french		La caméra, s'il y en a une.
		Camera * m_camera{ nullptr };
	};
}

#endif
