/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DeferredStencilPass_H___
#define ___C3D_DeferredStencilPass_H___

#include "LightPass.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date		08/06/2017
	\~english
	\brief		Stencil pre-pass for light passes needing a mesh.
	\~french
	\brief		Pré-passe de stencil pour les passes d'éclairage nécessitant un maillage.
	*/
	class StencilPass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	frameBuffer		The target framebuffer.
		 *\param[in]	depthAttach		The depth buffer attach.
		 *\param[in]	matrixUbo		The matrix UBO.
		 *\param[in]	modelMatrixUbo	The model matrix UBO.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	frameBuffer		Le tampon d'image cible.
		 *\param[in]	depthAttach		L'attache du tampon de profondeur.
		 *\param[in]	matrixUbo		L'UBO des matrices.
		 *\param[in]	modelMatrixUbo	L'UBO des matrices modèle.
		 */
		StencilPass( FrameBuffer & frameBuffer
			, FrameBufferAttachment & depthAttach
			, MatrixUbo & matrixUbo
			, ModelMatrixUbo & modelMatrixUbo );
		/**
		 *\~english
		 *\brief		Initialises the program and its pipeline.
		 *\param[in]	vbo	The vertex buffer containing the object to render.
		 *\~french
		 *\brief		Initialise le programme et son pipeline.
		 *\param[in]	vbo	Le tampon de sommets contenant l'objet à dessiner.
		 */
		void initialise( VertexBuffer & vbo );
		/**
		*\~english
		*\brief		Cleans up the program and its pipeline.
		*\~french
		*\brief		Nettoie le programme et son pipeline.
		*/
		void cleanup();
		/**
		 *\~english
		 *\brief		Renders the stencil pass.
		 *\param[in]	count	The number of primitives to draw.
		 *\~french
		 *\brief		Dessine la passe de stencil.
		 *\param[in]	count	Le nombre de primitives à dessiner.
		 */
		void render( uint32_t count );

	private:
		//!\~english	The target FBO.
		//!\~french		Le FBO cible.
		FrameBuffer & m_frameBuffer;
		//!\~english	The target RBO attach.
		//!\~french		L'attache de RBO cible.
		FrameBufferAttachment & m_depthAttach;
		//!\~english	The uniform buffer containing matrices data.
		//!\~french		Le tampon d'uniformes contenant les données de matrices.
		MatrixUbo & m_matrixUbo;
		//!\~english	The uniform buffer containing the model data.
		//!\~french		Le tampon d'uniformes contenant les données de modèle.
		ModelMatrixUbo & m_modelMatrixUbo;
		//!\~english	The shader program used to render lights.
		//!\~french		Le shader utilisé pour rendre les lumières.
		ShaderProgramSPtr m_program;
		//!\~english	Geometry buffers holder.
		//!\~french		Conteneur de buffers de géométries.
		GeometryBuffersSPtr m_geometryBuffers;
		//!\~english	The pipeline used by the light pass.
		//!\~french		Le pipeline utilisé par la passe lumières.
		RenderPipelineUPtr m_pipeline;
		//!\~english	The vertex buffer used to render the pass.
		//!\~french		Le tampon de sommets utilisé par le rendu de la passe.
		VertexBuffer const * m_vbo{ nullptr };
	};
}

#endif
