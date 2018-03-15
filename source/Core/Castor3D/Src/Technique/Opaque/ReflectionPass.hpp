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
			ProgramPipeline( Engine & engine
				, renderer::VertexBufferBase & vbo
				, MatrixUbo & matrixUbo
				, SceneUbo & sceneUbo
				, GpInfoUbo & gpInfoUbo
				, bool hasSsao
				, FogType fogType
				, MaterialType matType );
			~ProgramPipeline();

			void render( renderer::VertexBufferBase const & vbo );
			renderer::ShaderStageStateArray m_program;
			renderer::PipelinePtr m_pipeline;
		};
		using ReflectionPrograms = std::array< ProgramPipeline, size_t( FogType::eCount ) >;
		castor::Size m_size;
		Viewport m_viewport;
		renderer::VertexBufferBasePtr m_vertexBuffer;
		MatrixUbo m_matrixUbo;
		GpInfoUbo & m_gpInfoUbo;
		renderer::DescriptorSetLayoutPtr m_descriptorLayout;
		renderer::DescriptorSetPoolPtr m_descriptorPool;
		renderer::DescriptorSetPtr m_uboDescriptorSet;
		renderer::DescriptorSetPtr m_texDescriptorSet;
		renderer::PipelineLayoutPtr m_pipelineLayout;
		ReflectionPrograms m_programs;
		RenderPassTimerSPtr m_timer;
		bool m_ssaoEnabled{ false };
		SsaoPass m_ssao;
	};
}

#endif
