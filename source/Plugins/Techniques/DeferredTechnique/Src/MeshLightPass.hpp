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
#ifndef ___C3D_MeshLightPass_H___
#define ___C3D_MeshLightPass_H___

#include "StencilPass.hpp"

namespace deferred
{
	class MeshLightPass
		: public LightPass
	{
	protected:
		struct Program
			: public LightPass::Program
		{
		public:
			Program( Castor3D::Scene const & p_scene
				, Castor::String const & p_vtx
				, Castor::String const & p_pxl );
			virtual ~Program();
		};

	public:
		MeshLightPass( Castor3D::Engine & p_engine
			, Castor3D::FrameBuffer & p_frameBuffer
			, Castor3D::RenderBufferAttachment & p_depthAttach
			, Castor3D::LightType p_type
			, bool p_shadows );
		~MeshLightPass();
		void Initialise( Castor3D::Scene const & p_scene
			, Castor3D::UniformBuffer & p_sceneUbo )override;
		void Cleanup()override;
		uint32_t GetCount()const override;

	protected:
		void DoUpdate( Castor::Size const & p_size
			, Castor3D::Light const & p_light
			, Castor3D::Camera const & p_camera )override;

	private:
		Castor::String DoGetVertexShaderSource( Castor3D::SceneFlags const & p_sceneFlags )const override;
		virtual Castor::Point3fArray DoGenerateVertices()const = 0;
		virtual Castor3D::UIntArray DoGenerateFaces()const = 0;
		virtual Castor::Matrix4x4r DoComputeModelMatrix( Castor3D::Light const & p_light
			, Castor3D::Camera const & p_camera )const = 0;

	private:
		//!\~english	The uniform buffer containing the model data.
		//!\~french		Le tampon d'uniformes contenant les données de modèle.
		Castor3D::UniformBuffer m_modelMatrixUbo;
		//!\~english	The uniform variable containing projection matrix.
		//!\~french		La variable uniforme contenant la matrice projection.
		Castor3D::Uniform4x4fSPtr m_projectionUniform;
		//!\~english	The uniform variable containing view matrix.
		//!\~french		La variable uniforme contenant la matrice vue.
		Castor3D::Uniform4x4fSPtr m_viewUniform;
		//!\~english	The uniform variable containing model matrix.
		//!\~french		La variable uniforme contenant la matrice modèle.
		Castor3D::Uniform4x4fSPtr m_modelUniform;
		//!\~english	The vertex buffer.
		//!\~french		Le tampon de sommets.
		Castor3D::VertexBufferSPtr m_vertexBuffer;
		//!\~english	The index buffer.
		//!\~french		Le tampon d'indices.
		Castor3D::IndexBufferSPtr m_indexBuffer;
		//!\~english	The light's stencil pass.
		//!\~french		La passe stencil de la lumière.
		StencilPass m_stencilPass;
		//!\~english	The light source type.
		//!\~french		Le type de source lumineuse.
		Castor3D::LightType m_type;
	};
}

#endif
