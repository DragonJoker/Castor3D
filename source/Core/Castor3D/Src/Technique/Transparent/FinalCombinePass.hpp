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
		FinalCombineProgram( FinalCombineProgram const & p_rhs ) = delete;
		FinalCombineProgram & operator=( FinalCombineProgram const & p_rhs ) = delete;
		FinalCombineProgram( FinalCombineProgram && p_rhs ) = default;
		FinalCombineProgram & operator=( FinalCombineProgram && p_rhs ) = default;
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\param[in]	p_vbo		The vertex buffer containing the quad.
		 *\param[in]	p_matrixUbo	The matrix UBO.
		 *\param[in]	p_sceneUbo	The scene UBO.
		 *\param[in]	p_gpInfoUbo	The geometry pass UBO.
		 *\param[in]	p_fogType	The fog type.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 *\param[in]	p_vbo		Le tampon de sommets contenant le quad.
		 *\param[in]	p_matrixUbo	L'UBO des matrices.
		 *\param[in]	p_sceneUbo	L'UBO de la scène.
		 *\param[in]	p_gpInfoUbo	L'UBO de la geometry pass.
		 *\param[in]	p_fogType	Le type de brouillard.
		 */
		FinalCombineProgram( Engine & engine
			, VertexBuffer & p_vbo
			, MatrixUbo & p_matrixUbo
			, SceneUbo & p_sceneUbo
			, GpInfoUbo & p_gpInfoUbo
			, FogType p_fogType );
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
		void render( VertexBuffer const & vbo )const;

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
		 *\param[in]	engine	The engine.
		 *\param[in]	p_size		The render size.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 *\param[in]	p_size		La taille du rendu.
		 */
		FinalCombinePass( Engine & engine
			, castor::Size const & p_size );
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
		 *\param[in]	p_r				The weighted blend pass result.
		 *\param[in]	p_frameBuffer	The target framebuffer.
		 *\param[in]	p_camera		The viewing camera.
		 *\param[in]	p_invViewProj	The inverse view projection matrix.
		 *\param[in]	p_invView		The inverse view matrix.
		 *\param[in]	p_invProj		The inverse projection matrix.
		 *\param[in]	p_fog			The fog.
		 *\~french
		 *\brief		Dessine la combinaison sur le tampon d'image donné.
		 *\param[in]	p_r				Le résultat de la passe de weighted blend.
		 *\param[in]	p_frameBuffer	Le tampon d'image cible.
		 *\param[in]	p_camera		La caméra.
		 *\param[in]	p_invViewProj	La matrice vue projection inversée.
		 *\param[in]	p_invView		La matrice vue inversée.
		 *\param[in]	p_invProj		La matrice projection inversée.
		 *\param[in]	p_fog			Le brouillard.
		 */
		void render( WeightedBlendPassResult const & p_r
			, FrameBuffer const & p_frameBuffer
			, Camera const & p_camera
			, castor::Matrix4x4r const & p_invViewProj
			, castor::Matrix4x4r const & p_invView
			, castor::Matrix4x4r const & p_invProj
			, Fog const & p_fog );

	private:
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
		//!\~english	The scene uniform buffer.
		//!\~french		Le tampon d'uniformes contenant les informations de la scène.
		SceneUbo m_sceneUbo;
		//!\~english	The blend pass informations.
		//!\~french		Les informations de la passe de mélange.
		GpInfoUbo m_gpInfo;
		//!\~english	The shader program.
		//!\~french		Le shader program.
		FinalCombinePrograms m_programs;
	};
}

#endif
