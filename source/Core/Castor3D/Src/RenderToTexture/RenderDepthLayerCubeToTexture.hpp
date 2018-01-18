/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderDepthLayerCubeToTexture_H___
#define ___C3D_RenderDepthLayerCubeToTexture_H___

#include "Render/Viewport.hpp"

#include <VertexLayout.hpp>

#include <Design/OwnedBy.hpp>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date		02/03/2017
	\version	0.9.0
	\~english
	\brief		Class used to render a depth cube texture array layer.
	\~french
	\brief		Classe utilisée pour rendre une couche d'un tableau de textures cube profonedur.
	*/
	class RenderDepthLayerCubeToTexture
		: public castor::OwnedBy< Context >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_context	The Context.
		 *\param[in]	p_matrixUbo	The UBO containing matrix data.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_context	Le Context.
		 *\param[in]	p_matrixUbo	L'UBO contenant les données de matrices.
		 */
		C3D_API explicit RenderDepthLayerCubeToTexture( Context & p_context
			, MatrixUbo & p_matrixUbo );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~RenderDepthLayerCubeToTexture();
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
		 *\brief		Renders the wanted layer of the given cube texture to the currently draw-bound frame buffer.
		 *\param[in]	p_position	The render viewport position.
		 *\param[in]	p_size		The render viewport size.
		 *\param[in]	p_texture	The texture.
		 *\param[in]	p_layer		The layer index.
		 *\~french
		 *\brief		Rend la couche voulue de la texture cube donnée dans le tampon d'image actuellement activé en dessin.
		 *\param[in]	p_position	La position du viewport de rendu.
		 *\param[in]	p_size		La taille du viewport de rendu.
		 *\param[in]	p_texture	La texture.
		 *\param[in]	p_layer		L'index de la couche.
		 */
		C3D_API void render( castor::Position const & p_position
			, castor::Size const & p_size
			, renderer::Texture const & p_texture
			, uint32_t p_layer );

	protected:
		/**
		 *\~english
		 *\brief		Renders the wanted face of given cube texture.
		 *\param[in]	p_position			The render viewport position.
		 *\param[in]	p_size				The render viewport size.
		 *\param[in]	p_texture			The texture.
		 *\param[in]	p_face				The cube face.
		 *\param[in]	p_uvMult			The UV multiplier.
		 *\param[in]	p_pipeline			The render pipeline.
		 *\param[in]	p_matrixUbo			The uniform buffer receiving matrices.
		 *\param[in]	p_geometryBuffers	The geometry buffers used to render the texture.
		 *\param[in]	p_layer				The layer index.
		 *\~french
		 *\brief		Dessine la face voulue de la texture cube donnée.
		 *\param[in]	p_position			La position du viewport de rendu.
		 *\param[in]	p_size				La taille du viewport de rendu.
		 *\param[in]	p_texture			La texture.
		 *\param[in]	p_face				La face du cube.
		 *\param[in]	p_uvMult			Le multiplicateur d'UV.
		 *\param[in]	p_pipeline			Le pipeline de rendu.
		 *\param[in]	p_matrixUbo			Le tampon d'uniformes recevant les matrices.
		 *\param[in]	p_geometryBuffers	Les tampons de géométrie utilisés pour dessiner la texture.
		 *\param[in]	p_layer				L'index de la couche.
		 */
		C3D_API void doRender( castor::Point2i const & p_position
			, castor::Size const & p_size
			, renderer::Texture const & p_texture
			, castor::Point3f const & p_face
			, castor::Point2f const & p_uvMult
			, renderer::Pipeline & p_pipeline
			, MatrixUbo & p_matrixUbo
			, renderer::GeometryBuffers const & p_geometryBuffers
			, uint32_t p_layer );
		/**
		 *\~english
		 *\brief		Creates the render a 2D texture shader program.
		 *\return		The program.
		 *\~french
		 *\brief		Crée le programme shader de dessin de texture 2D.
		 *\return		Le programme.
		 */
		renderer::ShaderProgramPtr doCreateProgram();

	private:
		//!\~english	The uniform buffer containing matrices data.
		//!\~french		Le tampon d'uniformes contenant les données de matrices.
		MatrixUbo & m_matrixUbo;
		//!\~english	The Viewport used when rendering a texture into to a frame buffer.
		//!\~french		Le Viewport utilisé lors du dessin d'une texture dans un tampon d'image.
		Viewport m_viewport;
		//!\~english	Buffer elements declaration.
		//!\~french		Déclaration des éléments d'un vertex.
		renderer::VertexLayoutPtr m_layout;
		//!\~english	The vertex buffer.
		//!\~french		Le tampon de sommets.
		renderer::VertexBufferPtr< TexturedQuad > m_vertexBuffer;
		//!\~english	The GeometryBuffers used when rendering a texture to the current frame buffer.
		//!\~french		Le GeometryBuffers utilisé lors du dessin d'une texture dans le tampon d'image courant.
		renderer::GeometryBuffersPtr m_geometryBuffers;
		//!\~english	The shader program.
		//!\~french		Le programme shader.
		renderer::ShaderProgramPtr m_program;
		//!\~english	The pipeline used to render a texture in the current draw-bound framebuffer.
		//!\~french		Le pipeline utilisé pour le rendu d'une texture dans le tampon d'image actuellement activé en dessin.
		renderer::PipelinePtr m_pipeline;
		//!\~english	The sampler for the texture.
		//!\~french		Le sampler pour la texture.
		SamplerSPtr m_sampler;
		//!\~english	The uniform variable holding the direction of the face to render.
		//!\~french		La variable uniforme contenant la direction de la face à dessiner.
		PushUniform3fSPtr m_faceUniform;
		//!\~english	The uniform variable holding the UV multiplier.
		//!\~french		La variable uniforme contenant le multiplicateur d'UV.
		PushUniform2fSPtr m_uvUniform;
		//!\~english	The uniform variable holding the index of the layer to render.
		//!\~french		La variable uniforme contenant l'indice de la couche à dessiner.
		PushUniform1iSPtr m_layerIndexUniform;
	};
}

#endif
