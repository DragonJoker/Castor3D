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
#ifndef ___C3D_DeferredCombinePass_H___
#define ___C3D_DeferredCombinePass_H___

#include "LightPass.hpp"
#include "SsaoPass.hpp"

#include "Shader/MatrixUbo.hpp"
#include "Shader/SceneUbo.hpp"

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date		08/06/2017
	\~english
	\brief		Program used to combine the geometry pass and the light pass result, in deferred rendering.
	\~french
	\brief		Programme utilisé pour combiner la geometry pass et la light pass dans le rendu différé.
	*/
	struct CombineProgram
	{
		CombineProgram( CombineProgram const & p_rhs ) = delete;
		CombineProgram & operator=( CombineProgram const & p_rhs ) = delete;
		CombineProgram( CombineProgram && p_rhs ) = default;
		CombineProgram & operator=( CombineProgram && p_rhs ) = default;
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_engine	The engine.
		 *\param[in]	p_vbo		The vertex buffer containing the quad.
		 *\param[in]	p_matrixUbo	The matrix UBO.
		 *\param[in]	p_sceneUbo	The scene UBO.
		 *\param[in]	p_gpInfo	The geometry pass UBO.
		 *\param[in]	p_ssao		The SSAO activation status.
		 *\param[in]	p_fogType	The fog type.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_engine	Le moteur.
		 *\param[in]	p_vbo		Le tampon de sommets contenant le quad.
		 *\param[in]	p_matrixUbo	L'UBO des matrices.
		 *\param[in]	p_sceneUbo	L'UBO de la scène.
		 *\param[in]	p_gpInfo	L'UBO de la geometry pass.
		 *\param[in]	p_ssao		Le statut d'activation du SSAO.
		 *\param[in]	p_fogType	Le type de brouillard.
		 */
		CombineProgram( Engine & p_engine
			, VertexBuffer & p_vbo
			, MatrixUbo & p_matrixUbo
			, SceneUbo & p_sceneUbo
			, GpInfoUbo & p_gpInfo
			, bool p_ssao
			, bool p_pbr
			, GLSL::FogType p_fogType );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~CombineProgram();
		/**
		 *\~english
		 *\brief		Applies the program.
		 *\~french
		 *\brief		Applique le programme.
		 */
		void Render()const;

	private:
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
	//!\~english	An array of CombineProgram, one per fog type.
	//!\~french		Un tableau de CombineProgram, un par type de brouillard.
	using CombinePrograms = std::array< CombineProgram, size_t( GLSL::FogType::eCount ) * 2u >;
	/*!
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date		08/06/2017
	\~english
	\brief		Pass used to combine the geometry pass and the light pass result, in deferred rendering.
	\~french
	\brief		Passe utilisée pour combiner la geometry pass et la light pass dans le rendu différé.
	*/
	class CombinePass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_engine	The engine.
		 *\param[in]	p_size		The render size.
		 *\param[in]	p_config	The SSAO configuration.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_engine	Le moteur.
		 *\param[in]	p_size		La taille du rendu.
		 *\param[in]	p_config	La configuration du SSAO.
		 */
		CombinePass( Engine & p_engine
			, Castor::Size const & p_size
			, SceneUbo & p_sceneUbo
			, SsaoConfig const & p_config );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~CombinePass();
		/**
		 *\~english
		 *\brief		Renders the combination on given framebuffer.
		 *\param[in]	p_gp			The geometry pass result.
		 *\param[in]	p_lp			The light pas result.
		 *\param[in]	p_camera		The viewing camera.
		 *\param[in]	p_invViewProj	The inverse view projection matrix.
		 *\param[in]	p_invView		The inverse view matrix.
		 *\param[in]	p_invProj		The inverse projection matrix.
		 *\param[in]	p_fog			The fog.
		 *\param[in]	p_frameBuffer	The target framebuffer.
		 *\~french
		 *\brief		Dessine la combinaison sur le tampon d'image donné.
		 *\param[in]	p_gp			Le résultat de la geometry pass.
		 *\param[in]	p_lp			Le résultat de la light pass.
		 *\param[in]	p_camera		La caméra.
		 *\param[in]	p_invViewProj	La matrice vue projection inversée.
		 *\param[in]	p_invView		La matrice vue inversée.
		 *\param[in]	p_invProj		La matrice projection inversée.
		 *\param[in]	p_fog			Le brouillard.
		 *\param[in]	p_frameBuffer	Le tampon d'image cible.
		 */
		void Render( GeometryPassResult const & p_gp
			, TextureUnit const & p_light
			, TextureUnit const & p_reflection
			, TextureUnit const & p_refraction
			, Camera const & p_camera
			, Castor::Matrix4x4r const & p_invViewProj
			, Castor::Matrix4x4r const & p_invView
			, Castor::Matrix4x4r const & p_invProj
			, Fog const & p_fog
			, FrameBuffer const & p_frameBuffer );
		/**
		 *\~english
		 *\brief		Renders the combination on given framebuffer.
		 *\param[in]	p_gp			The geometry pass result.
		 *\param[in]	p_lp			The light pas result.
		 *\param[in]	p_camera		The viewing camera.
		 *\param[in]	p_invViewProj	The inverse view projection matrix.
		 *\param[in]	p_invView		The inverse view matrix.
		 *\param[in]	p_invProj		The inverse projection matrix.
		 *\param[in]	p_fog			The fog.
		 *\param[in]	p_frameBuffer	The target framebuffer.
		 *\~french
		 *\brief		Dessine la combinaison sur le tampon d'image donné.
		 *\param[in]	p_gp			Le résultat de la geometry pass.
		 *\param[in]	p_lp			Le résultat de la light pass.
		 *\param[in]	p_camera		La caméra.
		 *\param[in]	p_invViewProj	La matrice vue projection inversée.
		 *\param[in]	p_invView		La matrice vue inversée.
		 *\param[in]	p_invProj		La matrice projection inversée.
		 *\param[in]	p_fog			Le brouillard.
		 *\param[in]	p_frameBuffer	Le tampon d'image cible.
		 */
		void Render( GeometryPassResult const & p_gp
			, TextureUnit const & p_light
			, TextureUnit const & p_reflection
			, TextureUnit const & p_refraction
			, IblTextures const & p_ibl
			, Camera const & p_camera
			, Castor::Matrix4x4r const & p_invViewProj
			, Castor::Matrix4x4r const & p_invView
			, Castor::Matrix4x4r const & p_invProj
			, Fog const & p_fog
			, FrameBuffer const & p_frameBuffer );
		/**
		 *\~english
		 *\return		The SSAO texture.
		 *\~french
		 *\return		La texture SSAO.
		 */
		inline TextureLayout const & GetSsao()const
		{
			return *m_ssao.GetResult().GetTexture();
		}

	private:
		//!\~english	The render size.
		//!\~french		La taille du rendu.
		Castor::Size m_size;
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
		//!\~english	The shader program.
		//!\~french		Le shader program.
		CombinePrograms m_programs;
		//!\~english	Tells if SSAO is to be used in lighting pass.
		//!\~french		Dit si le SSAO doit être utilisé dans la light pass.
		bool m_ssaoEnabled{ false };
		//!\~english	The SSAO pass.
		//!\~french		La passe SSAO.
		SsaoPass m_ssao;
	};
}

#endif
