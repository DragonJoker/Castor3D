/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DeferredReflectionPass_H___
#define ___C3D_DeferredReflectionPass_H___

#include "LightPass.hpp"
#include "SsaoPass.hpp"
#include "EnvironmentMap/EnvironmentMap.hpp"
#include "Render/RenderInfo.hpp"
#include "Shader/Ubos/SceneUbo.hpp"
#include "Shader/Ubos/GpInfoUbo.hpp"
#include "Shader/Ubos/HdrConfigUbo.hpp"

namespace castor3d
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
		: castor::OwnedBy< Engine >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	size		The render size.
		 *\param[in]	sceneUbo	The scene UBO.
		 *\param[in]	gpInfoUbo	The geometry pass UBO.
		 *\param[in]	config		The SSAO configuration.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	size		Les dimensions du rendu.
		 *\param[in]	sceneUbo	L'UBO de la scène.
		 *\param[in]	gpInfoUbo	L'UBO de la passe géométrique.
		 *\param[in]	config		La configuration du SSAO.
		 */
		ReflectionPass( Engine & engine
			, castor::Size const & size
			, SceneUbo & sceneUbo
			, GpInfoUbo & gpInfoUbo
			, SsaoConfig const & config );
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
		 *\param[in]	gp				The geometry pass result.
		 *\param[in]	lightDiffuse	The diffuse result of the lighting pass.
		 *\param[in]	lightSpecular	The specular result of the lighting pass.
		 *\param[in]	scene			The rendered scene.
		 *\param[in]	camera			The viewing camera.
		 *\param[in]	frameBuffer		The target frame buffer.
		 *\~french
		 *\brief		Dessine le mapping de réflexion.
		 *\param[in]	gp				Le résultat de la passe géométrique.
		 *\param[in]	lightDiffuse	Le résultat diffus de la passe d'éclairage.
		 *\param[in]	lightSpecular	Le résultat spéculaire de la passe d'éclairage.
		 *\param[in]	scene			La scène rendue.
		 *\param[in]	camera			La caméra de rendu.
		 *\param[in]	frameBuffer		Le tampon d'image cible.
		 */
		void render( GeometryPassResult & gp
			, TextureUnit const & lightDiffuse
			, TextureUnit const & lightSpecular
			, Scene const & scene
			, Camera const & camera
			, renderer::FrameBuffer const & frameBuffer );
		/**
		 *\~english
		 *\return		The SSAO texture.
		 *\~french
		 *\return		La texture SSAO.
		 */
		inline TextureLayout const & getSsao()const
		{
			return *m_ssao.getResult().getTexture();
		}

	private:
		struct ProgramPipeline
		{
			ProgramPipeline( ProgramPipeline const & ) = delete;
			ProgramPipeline( ProgramPipeline && ) = default;
			ProgramPipeline & operator=( ProgramPipeline const & ) = delete;
			ProgramPipeline & operator=( ProgramPipeline && ) = default;
			/**
			 *\~english
			 *\brief		Constructor.
			 *\param[in]	engine		The engine.
			 *\param[in]	vbo			The vertex buffer.
			 *\param[in]	matrixUbo	The matrix UBO.
			 *\param[in]	sceneUbo	The scene UBO.
			 *\param[in]	gpInfoUbo	The geometry pass UBO.
			 *\param[in]	hasSsao		The SSAO enabled status.
			 *\param[in]	fogType		The fog type.
			 *\param[in]	matType		The materials type.
			 *\~french
			 *\brief		Constructeur.
			 *\param[in]	engine		Le moteur.
			 *\param[in]	vbo			Le tampon de sommets.
			 *\param[in]	matrixUbo	L'UBO des matrices.
			 *\param[in]	sceneUbo	L'UBO de la scène.
			 *\param[in]	gpInfoUbo	L'UBO de la passe géométrique.
			 *\param[in]	hasSsao		Le statut d'activation du SSAO.
			 *\param[in]	fogType		Le type de brouillard.
			 *\param[in]	matType		Le type de matériaux.
			 */
			ProgramPipeline( Engine & engine
				, renderer::VertexBufferBase & vbo
				, MatrixUbo & matrixUbo
				, SceneUbo & sceneUbo
				, GpInfoUbo & gpInfoUbo
				, bool hasSsao
				, FogType fogType
				, MaterialType matType );
			/**
			 *\~english
			 *\brief		Destructor.
			 *\~french
			 *\brief		Destructeur.
			 */
			~ProgramPipeline();
			/**
			 *\~english
			 *\brief		Applique le programme.
			 *\param[in]	vbo	The vertex buffer.
			 *\~french
			 *\brief		Applies the program.
			 *\param[in]	vbo	Le tampon de sommets.
			 */
			void render( renderer::VertexBufferBase const & vbo );
			//!\~english	The shader program.
			//!\~french		Le shader program.
			renderer::ShaderProgramPtr m_program;
			//!\~english	The render pipeline.
			//!\~french		Le pipeline de rendu.
			RenderPipelineUPtr m_pipeline;
		};
		//!\~english	An array of CombineProgram, one per fog type.
		//!\~french		Un tableau de CombineProgram, un par type de brouillard.
		using ReflectionPrograms = std::array< ProgramPipeline, size_t( FogType::eCount ) >;
		//!\~english	The render size.
		//!\~french		La taille du rendu.
		castor::Size m_size;
		//!\~english	The render viewport.
		//!\~french		La viewport du rendu.
		Viewport m_viewport;
		//!\~english	The vertex buffer.
		//!\~french		Le tampon de sommets.
		renderer::VertexBufferBasePtr m_vertexBuffer;
		//!\~english	The matrices uniform buffer.
		//!\~french		Le tampon d'uniformes contenant les matrices.
		MatrixUbo m_matrixUbo;
		//!\~english	The geometry pass informations.
		//!\~french		Les informations de la passe de géométrie.
		GpInfoUbo & m_gpInfoUbo;
		//!\~english	The shader program.
		//!\~french		Le shader program.
		ReflectionPrograms m_programs;
		//!\~english	The render pass timer.
		//!\~french		Le timer de la passe de rendu.
		RenderPassTimerSPtr m_timer;
		//!\~english	Tells if SSAO is to be used in lighting pass.
		//!\~french		Dit si le SSAO doit être utilisé dans la light pass.
		bool m_ssaoEnabled{ false };
		//!\~english	The SSAO pass.
		//!\~french		La passe SSAO.
		SsaoPass m_ssao;
	};
}

#endif
