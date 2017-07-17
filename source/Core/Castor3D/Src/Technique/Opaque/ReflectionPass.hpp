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
#ifndef ___C3D_DeferredReflectionPass_H___
#define ___C3D_DeferredReflectionPass_H___

#include "LightPass.hpp"
#include "EnvironmentMap/EnvironmentMap.hpp"
#include "Shader/SceneUbo.hpp"
#include "Shader/GpInfoUbo.hpp"
#include "Shader/HdrConfigUbo.hpp"

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date		06/04/2017
	\~english
	\brief		The post lighting reflection pass.
	\~french
	\brief		La passe de réflexion post éclairage.
	*/
	class ReflectionPass
		: Castor::OwnedBy< Engine >
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
		ReflectionPass( Engine & p_engine
			, Castor::Size const & p_size
			, SceneUbo & p_sceneUbo );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~ReflectionPass();
		/**
		 *\~english
		 *\brief		Renders the reflection mapping.
		 *\~french
		 *\brief		Dessine le mapping de réflexion.
		 */
		void Render( GeometryPassResult & p_gp
			, TextureUnit const & p_lp
			, Scene const & p_scene
			, Camera const & p_camera
			, Castor::Matrix4x4r const & p_invViewProj
			, Castor::Matrix4x4r const & p_invView
			, Castor::Matrix4x4r const & p_invProj );

		inline TextureUnit const & GetResult()const
		{
			return m_reflection;
		}

		inline TextureUnit const & GetReflection()const
		{
			return m_reflection;
		}

		inline TextureUnit const & GetRefraction()const
		{
			return m_refraction;
		}

	private:
		struct ProgramPipeline
		{
			ProgramPipeline( ProgramPipeline const & ) = delete;
			ProgramPipeline( ProgramPipeline && ) = default;
			ProgramPipeline & operator=( ProgramPipeline const & ) = delete;
			ProgramPipeline & operator=( ProgramPipeline && ) = default;
			ProgramPipeline( Engine & p_engine
				, VertexBuffer & p_vbo
				, MatrixUbo & p_matrixUbo
				, SceneUbo & p_sceneUbo
				, GpInfoUbo & p_gpInfo
				, HdrConfigUbo & p_configUbo
				, bool p_pbr
				, bool p_isMR );
			~ProgramPipeline();
			void Render();
			//!\~english	The shader program.
			//!\~french		Le shader program.
			ShaderProgramSPtr m_program;
			//!\~english	The geometry buffers.
			//!\~french		Les tampons de géométrie.
			GeometryBuffersSPtr m_geometryBuffers;
			//!\~english	The render pipeline.
			//!\~french		Le pipeline de rendu.
			RenderPipelineUPtr m_pipeline;
		};
		//!\~english	The render size.
		//!\~french		La taille du rendu.
		Castor::Size m_size;
		//!\~english	The light pass output.
		//!\~french		La sortie de la passe de lumières.
		TextureUnit m_reflection;
		//!\~english	The light pass output.
		//!\~french		La sortie de la passe de lumières.
		TextureUnit m_refraction;
		//!\~english	The target FBO.
		//!\~french		Le FBO cible.
		FrameBufferSPtr m_frameBuffer;
		//!\~english	The attachments between reflection result and FBO.
		//!\~french		Les attaches entre le résultat de la réflexion et le FBO.
		TextureAttachmentSPtr m_reflectAttach;
		//!\~english	The attachments between refraction result and FBO.
		//!\~french		Les attaches entre le résultat de la réfraction et le FBO.
		TextureAttachmentSPtr m_refractAttach;
		//!\~english	The render viewport.
		//!\~french		La viewport du rendu.
		Viewport m_viewport;
		//!\~english	The vertex buffer.
		//!\~french		Le tampon de sommets.
		VertexBufferSPtr m_vertexBuffer;
		//!\~english	The matrices uniform buffer.
		//!\~french		Le tampon d'uniformes contenant les matrices.
		MatrixUbo m_matrixUbo;
		//!\~english	The geometry pass informations.
		//!\~french		Les informations de la passe de géométrie.
		GpInfoUbo m_gpInfo;
		//!\~english	The HDR configuration.
		//!\~french		La configuration HDR.
		HdrConfigUbo m_configUbo;
		//!\~english	The shader program.
		//!\~french		Le shader program.
		std::array< ProgramPipeline, 3u > m_programs;
	};
}

#endif
