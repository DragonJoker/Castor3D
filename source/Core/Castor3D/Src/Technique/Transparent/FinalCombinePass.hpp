/*
See LICENSE file in root folder
*/
#ifndef ___C3D_FinalCombinePass_H___
#define ___C3D_FinalCombinePass_H___

#include "Shader/Ubos/MatrixUbo.hpp"
#include "Shader/Ubos/GpInfoUbo.hpp"
#include "Shader/Ubos/SceneUbo.hpp"
#include "TransparentPass.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date		08/06/2017
	\~english
	\brief		Program used to combine the transparent and opaque passes.
	\~french
	\brief		Programme utilisé pour combiner les passes opaque et transparente.
	*/
	struct FinalCombineProgram
	{
		FinalCombineProgram( FinalCombineProgram const & rhs ) = delete;
		FinalCombineProgram & operator=( FinalCombineProgram const & rhs ) = delete;
		FinalCombineProgram( FinalCombineProgram && rhs ) = default;
		FinalCombineProgram & operator=( FinalCombineProgram && rhs ) = default;
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	vbo			The vertex buffer containing the quad.
		 *\param[in]	matrixUbo	The matrix UBO.
		 *\param[in]	sceneUbo	The scene UBO.
		 *\param[in]	gpInfoUbo	The geometry pass UBO.
		 *\param[in]	fogType		The fog type.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	vbo			Le tampon de sommets contenant le quad.
		 *\param[in]	matrixUbo	L'UBO des matrices.
		 *\param[in]	sceneUbo	L'UBO de la scène.
		 *\param[in]	gpInfoUbo	L'UBO de la geometry pass.
		 *\param[in]	fogType		Le type de brouillard.
		 */
		FinalCombineProgram( Engine & engine
			, renderer::VertexBufferBase & vbo
			, MatrixUbo & matrixUbo
			, SceneUbo & sceneUbo
			, GpInfoUbo & gpInfoUbo
			, FogType fogType );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~FinalCombineProgram();
		/**
		 *\~english
		 *\brief		Applies the program.
		 *\~french
		 *\brief		Applique le programme.
		 */
		void render( renderer::VertexBufferBase const & vbo )const;

	private:
		//!\~english	The shader program.
		//!\~french		Le shader program.
		renderer::ShaderProgramPtr m_program;
		//!\~english	The geometry buffers.
		//!\~french		Les tampons de géométrie.
		renderer::GeometryBuffersPtr m_geometryBuffers;
		//!\~english	The render pipeline.
		//!\~french		Le pipeline de rendu.
		RenderPipelineUPtr m_pipeline;
	};
	//!\~english	An array of FinalCombineProgram, one per fog type.
	//!\~french		Un tableau de FinalCombineProgram, un par type de brouillard.
	using FinalCombinePrograms = std::array< FinalCombineProgram, size_t( FogType::eCount ) >;
	/*!
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date		08/06/2017
	\~english
	\brief		Pass used to combine the transparent and opaque passes.
	\~french
	\brief		Passe utilisée pour combiner les passes opaque et transparente.
	*/
	class FinalCombinePass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	size		The render size.
		 *\param[in]	sceneUbo	The scene UBO.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	size		La taille du rendu.
		 *\param[in]	sceneUbo	L'UBO de la scène.
		 */
		FinalCombinePass( Engine & engine
			, castor::Size const & size
			, SceneUbo & sceneUbo );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~FinalCombinePass();
		/**
		 *\~english
		 *\brief		Renders the combination on given framebuffer.
		 *\param[in]	wbResult	The weighted blend pass result.
		 *\param[in]	frameBuffer	The target framebuffer.
		 *\param[in]	camera		The viewing camera.
		 *\param[in]	invViewProj	The inverse view projection matrix.
		 *\param[in]	invView		The inverse view matrix.
		 *\param[in]	invProj		The inverse projection matrix.
		 *\param[in]	fogType		The fog type.
		 *\~french
		 *\brief		Dessine la combinaison sur le tampon d'image donné.
		 *\param[in]	wbResult	Le résultat de la passe de weighted blend.
		 *\param[in]	frameBuffer	Le tampon d'image cible.
		 *\param[in]	camera		La caméra.
		 *\param[in]	invViewProj	La matrice vue projection inversée.
		 *\param[in]	invView		La matrice vue inversée.
		 *\param[in]	invProj		La matrice projection inversée.
		 *\param[in]	fogType		Le type de brouillard.
		 */
		void render( WeightedBlendPassResult const & wbResult
			, renderer::FrameBuffer const & frameBuffer
			, Camera const & camera
			, castor::Matrix4x4r const & invViewProj
			, castor::Matrix4x4r const & invView
			, castor::Matrix4x4r const & invProj
			, FogType fogType );

	private:
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
		//!\~english	The scene uniform buffer.
		//!\~french		Le tampon d'uniformes contenant les informations de la scène.
		SceneUbo & m_sceneUbo;
		//!\~english	The blend pass informations.
		//!\~french		Les informations de la passe de mélange.
		GpInfoUbo m_gpInfo;
		//!\~english	The shader program.
		//!\~french		Le shader program.
		FinalCombinePrograms m_programs;
	};
}

#endif
