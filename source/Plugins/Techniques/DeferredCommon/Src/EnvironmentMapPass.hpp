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
#ifndef ___C3DDF_EnvironmentMapPass_H___
#define ___C3DDF_EnvironmentMapPass_H___

#include "LightPass.hpp"
#include <EnvironmentMap/EnvironmentMap.hpp>

namespace deferred_common
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date		06/04/2017
	\~english
	\brief		The post lighting environment pass.
	\~french
	\brief		La passe d'environnement post éclairage.
	*/
	class EnvironmentMapPass
		: Castor::OwnedBy< Castor3D::Engine >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_engine	Le moteur.
		 */
		EnvironmentMapPass( Castor3D::Engine & p_engine
			, Castor::Size const & p_size );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~EnvironmentMapPass();
		/**
		 *\~english
		 *\brief		Renders the environment mapping.
		 *\~french
		 *\brief		Dessine le mapping d'environnement.
		 */
		void Render( GeometryPassResult & p_gp
			, Castor3D::Scene & p_scene );
		/**
		 *\~english
		 *\return		The environment mapping result.
		 *\~english
		 *\return		Le résultat du maping d'environnement.
		 */
		inline Castor3D::TextureUnit & GetTexture()
		{
			return m_result;
		}
		/**
		 *\~english
		 *\return		The shadow map.
		 *\~english
		 *\return		La map d'ombres.
		 */
		inline Castor3D::TextureUnit const & GetTexture()const
		{
			return m_result;
		}

	private:
		//!\~english	The frame buffer.
		//!\~french		Le tampon d'image.
		Castor3D::FrameBufferSPtr m_frameBuffer;
		//!\~english	The shadow map texture.
		//!\~french		La texture de mappage d'ombres.
		Castor3D::TextureUnit m_result;
		//!\~english	The attach between colour buffer and the frame buffer.
		//!\~french		L'attache entre le tampon couleur et le tampon de frame.
		Castor3D::TextureAttachmentSPtr m_colourAttach;
		//!\~english	The shader program.
		//!\~french		Le shader program.
		Castor3D::ShaderProgramSPtr m_program;
		//!\~english	The matrices uniform buffer.
		//!\~french		Le tampon d'uniformes contenant les matrices.
		Castor3D::UniformBuffer m_matrixUbo;
		//!\~english	The render pipeline.
		//!\~french		Le pipeline de rendu.
		Castor3D::RenderPipelineUPtr m_pipeline;
		//!\~english	The uniform variable containing projection matrix.
		//!\~french		La variable uniforme contenant la matrice projection.
		Castor3D::Uniform4x4fSPtr m_projectionUniform{ nullptr };
		//!\~english	The uniform variable containing view matrix.
		//!\~french		La variable uniforme contenant la matrice vue.
		Castor3D::Uniform4x4fSPtr m_viewUniform{ nullptr };
	};
}

#endif
