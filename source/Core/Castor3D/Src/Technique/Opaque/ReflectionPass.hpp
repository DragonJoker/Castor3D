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
		 *\param[in]	engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
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
		 *\~french
		 *\brief		Dessine le mapping de réflexion.
		 */
		void render( GeometryPassResult & gp
			, TextureUnit const & lightDiffuse
			, TextureUnit const & lightSpecular
			, Scene const & scene
			, FrameBuffer const & frameBuffer
			, RenderInfo & info );
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
				, VertexBuffer & p_vbo
				, MatrixUbo & p_matrixUbo
				, SceneUbo & p_sceneUbo
				, GpInfoUbo & p_gpInfo
				, HdrConfigUbo & p_configUbo
				, bool hasSsao
				, FogType fogType
				, MaterialType matType );
			~ProgramPipeline();
			void render( VertexBuffer const & vbo );
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
		using ReflectionPrograms = std::array< ProgramPipeline, size_t( FogType::eCount ) * size_t( MaterialType::eCount ) >;
		//!\~english	The render size.
		//!\~french		La taille du rendu.
		castor::Size m_size;
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
		GpInfoUbo & m_gpInfoUbo;
		//!\~english	The HDR configuration.
		//!\~french		La configuration HDR.
		HdrConfigUbo m_configUbo;
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
