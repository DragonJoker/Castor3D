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
#ifndef ___C3D_LightingPass_H___
#define ___C3D_LightingPass_H___

#include "FogPass.hpp"
#include "ReflectionPass.hpp"
#include "SsaoPass.hpp"

namespace deferred_common
{
	class OpaquePass;

	class LightingPass
	{
		using LightPasses = std::array< std::unique_ptr< LightPass >, size_t( Castor3D::LightType::eCount ) >;

	public:
		LightingPass( Castor3D::Engine & p_engine
			, Castor::Size const & p_size
			, Castor3D::Scene const & p_scene
			, OpaquePass & p_opaque
			, bool p_ssao
			, Castor3D::FrameBufferAttachment & p_depthAttach
			, Castor3D::SceneUbo & p_sceneUbo );
		~LightingPass();

		bool Render( Castor3D::Scene const & p_scene
			, Castor3D::Camera const & p_camera
			, GeometryPassResult const & p_gp
			, Castor::Matrix4x4r const & p_invViewProj
			, Castor::Matrix4x4r const & p_invView
			, Castor::Matrix4x4r const & p_invProj );

		inline Castor3D::TextureUnit const & GetResult()const
		{
			return m_result;
		}

		inline Castor3D::TextureLayout const & GetSsao()const
		{
			return m_ssao.GetRaw();
		}

	private:
		void DoRenderLights( Castor3D::Scene const & p_scene
			, Castor3D::Camera const & p_camera
			, Castor3D::LightType p_type
			, GeometryPassResult const & p_gp
			, Castor::Matrix4x4r const & p_invViewProj
			, Castor::Matrix4x4r const & p_invView
			, Castor::Matrix4x4r const & p_invProj
			, Castor3D::TextureUnit const * p_ssao
			, bool & p_first );

	private:
		Castor::Size const m_size;
		//!\~english	The light pass output.
		//!\~french		La sortie de la passe de lumières.
		Castor3D::TextureUnit m_result;
		//!\~english	The target FBO.
		//!\~french		Le FBO cible.
		Castor3D::FrameBufferSPtr m_frameBuffer;
		//!\~english	The attachments between textures and deferred shading frame buffer.
		//!\~french		Les attaches entre les textures et le tampon deferred shading.
		Castor3D::TextureAttachmentSPtr m_resultAttach;
		//!\~english	The light passes.
		//!\~french		Les passes de lumière.
		LightPasses m_lightPass;
		//!\~english	The light passes, with shadows.
		//!\~french		Les passes de lumière, avec ombres.
		LightPasses m_lightPassShadow;
		//!\~english	Tells if SSAO is to be used in lighting pass.
		//!\~french		Dit si le SSAO doit être utilisé dans la light pass.
		bool m_ssaoEnabled{ false };
		//!\~english	The SSAO pass.
		//!\~french		La passe SSAO.
		SsaoPass m_ssao;
	};
}

#endif
