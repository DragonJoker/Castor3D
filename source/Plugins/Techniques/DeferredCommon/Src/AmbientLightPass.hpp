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
#ifndef ___C3D_AmbientLightPass_H___
#define ___C3D_AmbientLightPass_H___

#include "LightPass.hpp"

namespace deferred_common
{
	class AmbientLightPass
		: public LightPass
	{
	protected:
		struct Program
			: public LightPass::Program
		{
		public:
			Program( Castor3D::Engine & p_engine
				, Castor::String const & p_vtx
				, Castor::String const & p_pxl
				, bool p_ssao );
			virtual ~Program();

		private:
			virtual Castor3D::RenderPipelineUPtr DoCreatePipeline( bool p_blend )override;
			void DoBind( Castor3D::Light const & p_light )override;
		};

	public:
		AmbientLightPass( Castor3D::Engine & p_engine
			, Castor3D::FrameBuffer & p_frameBuffer
			, Castor3D::FrameBufferAttachment & p_depthAttach
			, bool p_ssao );
		~AmbientLightPass();
		void Initialise( Castor3D::Scene const & p_scene
			, Castor3D::SceneUbo & p_sceneUbo )override;
		void Cleanup()override;
		virtual void Render( Castor::Size const & p_size
			, GeometryPassResult const & p_gp
			, Castor3D::Camera const & p_camera
			, Castor::Matrix4x4r const & p_invViewProj
			, Castor::Matrix4x4r const & p_invView
			, Castor::Matrix4x4r const & p_invProj
			, Castor3D::TextureUnit const * p_ssao
			, bool p_first );
		uint32_t GetCount()const override;

	protected:
		void DoUpdate( Castor::Size const & p_size
			, Castor3D::Light const & p_light
			, Castor3D::Camera const & p_camera )override;

	private:
		Castor::String DoGetPixelShaderSource( Castor3D::SceneFlags const & p_sceneFlags
			, Castor3D::LightType p_type )const override;
		Castor::String DoGetVertexShaderSource( Castor3D::SceneFlags const & p_sceneFlags )const override;
		LightPass::ProgramPtr DoCreateProgram( Castor::String const & p_vtx
			, Castor::String const & p_pxl )const override;

	private:
		//!\~english	The vertex buffer.
		//!\~french		Le tampon de sommets.
		Castor3D::VertexBufferSPtr m_vertexBuffer;
		//!\~english	The viewport used when rendering is done.
		//!\~french		Le viewport utilisé pour rendre la cible sur sa cible (fenêtre ou texture).
		Castor3D::Viewport m_viewport;
	};
}

#endif
