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
#ifndef ___C3D_LightPassShadow_H___
#define ___C3D_LightPassShadow_H___

#include "LightPass.hpp"

namespace deferred
{
	class LightPassShadow
	{
	protected:
		LightPassShadow( Castor3D::Engine & p_engine );
		void DoBeginRender( Castor::Size const & p_size
			, GeometryPassResult const & p_gp );
		void DoEndRender( GeometryPassResult const & p_gp );
		Castor::String DoGetPixelShaderSource( Castor3D::SceneFlags const & p_sceneFlags
			, Castor3D::LightType p_type )const;

	protected:
		struct Program
		{
		protected:
			void DoCreate( Castor3D::Scene const & p_scene
				, Castor::String const & p_vtx
				, Castor::String const & p_pxl
				, uint16_t p_fogType );
			void DoDestroy();
			void DoInitialise( Castor3D::UniformBuffer & p_matrixUbo
			, Castor3D::UniformBuffer & p_sceneUbo );
			void DoCleanup();
			void DoBind( Castor::Size const & p_size
				, Castor3D::LightCategory const & p_light
				, bool p_first );

		public:
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

	protected:
		//!\~english	The engine.
		//!\~french		Le moteur.
		Castor3D::Engine & m_engine;
		//!\~english	The uniform buffer containing matrices data.
		//!\~french		Le tampon d'uniformes contenant les données de matrices.
		Castor3D::UniformBuffer m_matrixUbo;
	};
}

#endif
