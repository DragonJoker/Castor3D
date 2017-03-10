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
#ifndef ___C3D_LightPass_H___
#define ___C3D_LightPass_H___

#include <Mesh/Buffer/BufferDeclaration.hpp>
#include <Technique/RenderTechnique.hpp>
#include <Render/Viewport.hpp>
#include <Shader/UniformBuffer.hpp>

namespace deferred
{
	enum class DsTexture
		: uint8_t
	{
		ePosition,
		eDiffuse,
		eNormals,
		eTangent,
		eSpecular,
		eEmissive,
		eInfos,
		CASTOR_SCOPED_ENUM_BOUNDS( ePosition ),
	};
	Castor::String GetTextureName( DsTexture p_texture );
	Castor::PixelFormat GetTextureFormat( DsTexture p_texture );
	Castor3D::AttachmentPoint GetTextureAttachmentPoint( DsTexture p_texture );
	uint32_t GetTextureAttachmentIndex( DsTexture p_texture );

	using GeometryPassResult = std::array< Castor3D::TextureUnitUPtr, size_t( DsTexture::eCount ) >;

	class LightPass
	{
	protected:
		LightPass( Castor3D::Engine & p_engine
			, Castor3D::UniformBuffer & p_matrixUbo
			, Castor3D::UniformBuffer & p_sceneUbo );
		void DoCreate( Castor3D::ShaderProgramSPtr p_program
			, Castor3D::Scene const & p_scene );
		void DoDestroy();
		void DoInitialise( Castor3D::VertexBuffer & p_vbo );
		void DoCleanup();
		void DoBeginRender( Castor::Size const & p_size
			, GeometryPassResult const & p_gp
			, Castor3D::LightCategory const & p_light
			, bool p_first );
		void DoEndRender( GeometryPassResult const & p_gp
			, Castor3D::LightCategory const & p_light );

	protected:
		//!\~english	The uniform buffer containing matrices data.
		//!\~french		Le tampon d'uniformes contenant les données de matrices.
		Castor3D::UniformBuffer & m_matrixUbo;
		//!\~english	The uniform buffer containing the scene data.
		//!\~french		Le tampon d'uniformes contenant les données de scène.
		Castor3D::UniformBuffer & m_sceneUbo;
		//!\~english	The viewport used when rendering is done.
		//!\~french		Le viewport utilisé pour rendre la cible sur sa cible (fenêtre ou texture).
		Castor3D::Viewport m_viewport;
		//!\~english	The uniform variable containing projection matrix.
		//!\~french		La variable uniforme contenant la matrice projection.
		Castor3D::Uniform4x4fSPtr m_projectionUniform;
		//!\~english	The shader program used to render lights.
		//!\~french		Le shader utilisé pour rendre les lumières.
		Castor3D::ShaderProgramSPtr m_program;
		//!\~english	Geometry buffers holder.
		//!\~french		Conteneur de buffers de géométries.
		Castor3D::GeometryBuffersSPtr m_geometryBuffers;
		//!\~english	The pipeline used by the light pass.
		//!\~french		Le pipeline utilisé par la passe lumières.
		Castor3D::RenderPipelineSPtr m_blendPipeline;
		//!\~english	The pipeline used by the light pass.
		//!\~french		Le pipeline utilisé par la passe lumières.
		Castor3D::RenderPipelineSPtr m_firstPipeline;
		//!\~english	The pipeline used by the light pass.
		//!\~french		Le pipeline utilisé par la passe lumières.
		Castor3D::RenderPipelineSPtr m_currentPipeline;
		//!\~english	The shader variable containing the camera position.
		//!\~french		La variable de shader contenant la position de la caméra.
		Castor3D::Uniform3fSPtr m_camera;
		//!\~english	The shader variable containing the render area size.
		//!\~french		La variable de shader contenant les dimensions de la zone de rendu.
		Castor3D::PushUniform2fSPtr m_renderSize;
		//!\~english	The variable containing the light colour.
		//!\~french		La variable contenant la couleur de la lumière.
		Castor3D::PushUniform3fSPtr m_lightColour;
		//!\~english	The variable containing the light intensities.
		//!\~french		La variable contenant les intensités de la lumière.
		Castor3D::PushUniform3fSPtr m_lightIntensity;
	};
}

#endif
