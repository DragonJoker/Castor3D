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
#ifndef ___C3D_DeferredLightingPass_H___
#define ___C3D_DeferredLightingPass_H___

#include "CombinePass.hpp"
#include "ReflectionPass.hpp"

namespace Castor3D
{
	class OpaquePass;

	class LightingPass
	{
		using LightPasses = std::array< std::unique_ptr< LightPass >, size_t( LightType::eCount ) >;

	public:
		LightingPass( Engine & p_engine
			, Castor::Size const & p_size
			, Scene const & p_scene
			, OpaquePass & p_opaque
			, FrameBufferAttachment & p_depthAttach
			, SceneUbo & p_sceneUbo );
		~LightingPass();

		bool Render( Scene const & p_scene
			, Camera const & p_camera
			, GeometryPassResult const & p_gp
			, Castor::Matrix4x4r const & p_invViewProj
			, Castor::Matrix4x4r const & p_invView
			, Castor::Matrix4x4r const & p_invProj );

		inline TextureUnit const & GetResult()const
		{
			return m_result;
		}

	private:
		void DoRenderLights( Scene const & p_scene
			, Camera const & p_camera
			, LightType p_type
			, GeometryPassResult const & p_gp
			, Castor::Matrix4x4r const & p_invViewProj
			, Castor::Matrix4x4r const & p_invView
			, Castor::Matrix4x4r const & p_invProj
			, bool & p_first );

	private:
		Castor::Size const m_size;
		//!\~english	The light pass output.
		//!\~french		La sortie de la passe de lumières.
		TextureUnit m_result;
		//!\~english	The target FBO.
		//!\~french		Le FBO cible.
		FrameBufferSPtr m_frameBuffer;
		//!\~english	The attachments between textures and deferred shading frame buffer.
		//!\~french		Les attaches entre les textures et le tampon deferred shading.
		TextureAttachmentSPtr m_resultAttach;
		//!\~english	The light passes.
		//!\~french		Les passes de lumière.
		LightPasses m_lightPass;
		//!\~english	The light passes, with shadows.
		//!\~french		Les passes de lumière, avec ombres.
		LightPasses m_lightPassShadow;
	};
}

#endif
