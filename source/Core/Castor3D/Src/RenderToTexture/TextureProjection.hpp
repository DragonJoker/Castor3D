/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextureProjection_H___
#define ___C3D_TextureProjection_H___

#include "Mesh/Buffer/BufferDeclaration.hpp"
#include "Shader/Ubos/MatrixUbo.hpp"
#include "Shader/Ubos/ModelMatrixUbo.hpp"

#include <Design/OwnedBy.hpp>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date		02/03/2017
	\version	0.9.0
	\~english
	\brief		Class used to render colour equirectangular textures to cube maps.
	\~french
	\brief		Classe utilisée pour rendre les textures couleur équirectangulaires dans des cube maps.
	*/
	class TextureProjection
		: public castor::OwnedBy< Context >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_context	The Context.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_context	Le Context.
		 */
		C3D_API explicit TextureProjection( Context & p_context );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~TextureProjection();
		/**
		 *\~english
		 *\brief		Initialises this render to texture.
		 *\~french
		 *\brief		Initialise le rendu en texture.
		 */
		C3D_API void initialise();
		/**
		 *\~english
		 *\brief		Cleans up the render to texture.
		 *\~french
		 *\brief		Nettoie le rendu en texture.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Projects the given 2D texture on the current framebuffer.
		 *\param[in]	p_texture	The 2D texture.
		 *\param[in]	p_camera	The camera.
		 *\~french
		 *\brief		Projette la texture 2D donnée sur le framebuffer courant.
		 *\param[in]	p_texture	La texture 2D.
		 *\param[in]	p_camera	La caméra.
		 */
		C3D_API void render( TextureLayout const & p_texture
			, Camera const & p_camera );

	private:
		ShaderProgram & doInitialiseShader();
		bool doInitialiseVertexBuffer();
		bool doInitialisePipeline( ShaderProgram & p_program );

	private:
		//!\~english	The shader matrices constants buffer.
		//!\~french		Le tampon de constantes de shader contenant les matrices.
		MatrixUbo m_matrixUbo;
		//!\~english	The uniform buffer containing matrices data.
		//!\~french		Le tampon d'uniformes contenant les données de matrices.
		ModelMatrixUbo m_modelMatrixUbo;
		//!\~english	The uniform variable containing render target size.
		//!\~french		La variable uniforme contenant la taille de la cible du rendu.
		PushUniform2fSPtr m_sizeUniform{ nullptr };
		//!	6 (faces) * 6 (vertex) * 3 (vertex position)
		std::array< castor::real, 6 * 6 * 3 > m_bufferVertex;
		//!\~english	Buffer elements declaration.
		//!\~french		Déclaration des éléments d'un vertex.
		castor3d::BufferDeclaration m_declaration;
		//!\~english	Vertex array (quad definition).
		//!\~french		Tableau de vertex (définition du quad).
		std::array< castor3d::BufferElementGroupSPtr, 36 > m_arrayVertex;
		//!\~english	The vertex buffer.
		//!\~french		Le tampon de sommets.
		VertexBufferSPtr m_vertexBuffer;
		//!\~english	The GeometryBuffers used when rendering a texture to the current frame buffer.
		//!\~french		Le GeometryBuffers utilisé lors du dessin d'une texture dans le tampon d'image courant.
		GeometryBuffersSPtr m_geometryBuffers;
		//!\~english	The pipeline used to render a texture in the current draw-bound framebuffer.
		//!\~french		Le pipeline utilisé pour le rendu d'une texture dans le tampon d'image actuellement activé en dessin.
		RenderPipelineUPtr m_pipeline;
		//!\~english	The sampler for the texture.
		//!\~french		Le sampler pour la texture.
		SamplerSPtr m_sampler;
		//!\~english	The model matrix.
		//!\~french		La matrice modèle.
		castor::Matrix4x4r m_mtxModel;
	};
}

#endif
