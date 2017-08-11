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
#ifndef ___C3D_DeferredOpaquePass_H___
#define ___C3D_DeferredOpaquePass_H___

#include <Technique/RenderTechniquePass.hpp>

namespace castor3d
{
	class RenderTechnique;
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		12/11/2012
	\~english
	\brief		Deferred lighting Render technique pass.
	\~french
	\brief		Classe de passe de technique de rendu implémentant le Deferred lighting.
	*/
	class OpaquePass
		: public RenderTechniquePass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	scene	The scene for this technique.
		 *\param[in]	camera	The camera for this technique (may be null).
		 *\param[in]	config	The SSAO configuration.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	scene	La scène pour cette technique.
		 *\param[in]	camera	La caméra pour cette technique (peut être nulle).
		 *\param[in]	config	La configuration du SSAO.
		 */
		OpaquePass( Scene & scene
			, Camera * camera
			, SsaoConfig const & config );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~OpaquePass();
		/**
		 *\copydoc		castor3d::RenderTechniquePass::render
		 */
		void render( RenderInfo & info
			, ShadowMapLightTypeArray & shadowMaps )override;

	protected:
		/**
		 *\copydoc		castor3d::RenderPass::doUpdateFlags
		 */
		void doUpdateFlags( TextureChannels & textureFlags
			, ProgramFlags & programFlags
			, SceneFlags & sceneFlags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetVertexShaderSource
		 */
		glsl::Shader doGetVertexShaderSource( TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, bool invertNormals )const override;
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
		 *\copydoc		castor3d::RenderPass::doUpdatePipeline
		 */
		void doUpdatePipeline( RenderPipeline & pipeline )const override;

	private:
		static castor::String const Output1;
		static castor::String const Output2;
		static castor::String const Output3;
		static castor::String const Output4;
	};
}

#endif
