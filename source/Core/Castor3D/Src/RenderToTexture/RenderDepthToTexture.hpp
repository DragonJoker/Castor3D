/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderDepthToTexture_H___
#define ___C3D_RenderDepthToTexture_H___

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
	\brief		Class used to render depth textures.
	\~french
	\brief		Classe utilisée pour rendre les textures profondeur.
	*/
	class RenderDepthToTexture
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
		C3D_API explicit RenderDepthToTexture( Context & p_context
			, MatrixUbo & p_matrixUbo );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~RenderDepthToTexture();
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
		 *\brief		Renders the given texture to the currently draw-bound frame buffer.
		 *\param[in]	p_position	The render viewport position.
		 *\param[in]	p_size		The render viewport size.
		 *\param[in]	p_texture	The texture.
		 *\param[in]	p_pipeline	The pipleline used to render the texture.
		 *\param[in]	p_matrixUbo	The uniform buffer receiving matrices.
		 *\~french
		 *\brief		Dessine la texture donnée dans le tampon d'image actuellement activé en dessin.
		 *\param[in]	p_position	La position du viewport de rendu.
		 *\param[in]	p_size		La taille du viewport de rendu.
		 *\param[in]	p_texture	La texture.
		 *\param[in]	p_pipeline	Le pipeline utilisé pour dessiner la texture.
		 *\param[in]	p_matrixUbo	Le tampon d'uniformes recevant les matrices.
		 */
		C3D_API void render( castor::Position const & p_position
			, castor::Size const & p_size
			, renderer::Texture const & p_texture
			, MatrixUbo & p_matrixUbo
			, renderer::Pipeline & p_pipeline );
		/**
		 *\~english
		 *\brief		Renders the given texture to the currently draw-bound frame buffer.
		 *\param[in]	p_position	The render viewport position.
		 *\param[in]	p_size		The render viewport size.
		 *\param[in]	p_texture	The texture.
		 *\~french
		 *\brief		Rend la texture donnée dans le tampon d'image actuellement activé en dessin.
		 *\param[in]	p_position	La position du viewport de rendu.
		 *\param[in]	p_size		La taille du viewport de rendu.
		 *\param[in]	p_texture	La texture.
		 */
		C3D_API void render( castor::Position const & p_position
			, castor::Size const & p_size
			, renderer::Texture const & p_texture );

	private:
		/**
		 *\~english
		 *\brief		Renders the given texture.
		 *\param[in]	p_position			The render viewport position.
		 *\param[in]	p_size				The render viewport size.
		 *\param[in]	p_texture			The texture.
		 *\param[in]	p_pipeline			The render pipeline.
		 *\param[in]	p_matrixUbo			The uniform buffer receiving matrices.
		 *\param[in]	p_geometryBuffers	The geometry buffers used to render the texture.
		 *\~french
		 *\brief		Dessine la texture donnée.
		 *\param[in]	p_position			La position du viewport de rendu.
		 *\param[in]	p_size				La taille du viewport de rendu.
		 *\param[in]	p_texture			La texture.
		 *\param[in]	p_pipeline			Le pipeline de rendu.
		 *\param[in]	p_matrixUbo			Le tampon d'uniformes recevant les matrices.
		 *\param[in]	p_geometryBuffers	Les tampons de géométrie utilisés pour dessiner la texture.
		 */
		C3D_API void doRender( castor::Position const & p_position
			, castor::Size const & p_size
			, renderer::Texture const & p_texture
			, renderer::Pipeline & p_pipeline
			, MatrixUbo & p_matrixUbo
			, renderer::GeometryBuffers const & p_geometryBuffers );
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
	};
}

#endif
