/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderVarianceToTexture_H___
#define ___C3D_RenderVarianceToTexture_H___

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
	\brief		Class used to render variance textures.
	\~french
	\brief		Classe utilisée pour rendre les textures de variance.
	*/
	class RenderVarianceToTexture
		: public castor::OwnedBy< Context >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	context		The Context.
		 *\param[in]	matrixUbo	The UBO containing matrix data.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	context		Le Context.
		 *\param[in]	matrixUbo	L'UBO contenant les données de matrices.
		 */
		C3D_API explicit RenderVarianceToTexture( Context & context
			, MatrixUbo & matrixUbo );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~RenderVarianceToTexture();
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
		 *\param[in]	position	The render viewport position.
		 *\param[in]	size		The render viewport size.
		 *\param[in]	texture		The texture.
		 *\~french
		 *\brief		Rend la texture donnée dans le tampon d'image actuellement activé en dessin.
		 *\param[in]	position	La position du viewport de rendu.
		 *\param[in]	size		La taille du viewport de rendu.
		 *\param[in]	texture		La texture.
		 */
		C3D_API void render( castor::Position const & position
			, castor::Size const & size
			, renderer::Texture const & texture );

	private:
		/**
		 *\~english
		 *\brief		Renders the given texture.
		 *\param[in]	position		The render viewport position.
		 *\param[in]	size			The render viewport size.
		 *\param[in]	texture			The texture.
		 *\param[in]	pipeline		The render pipeline.
		 *\param[in]	matrixUbo		The uniform buffer receiving matrices.
		 *\param[in]	geometryBuffers	The geometry buffers used to render the texture.
		 *\~french
		 *\brief		Dessine la texture donnée.
		 *\param[in]	position		La position du viewport de rendu.
		 *\param[in]	size			La taille du viewport de rendu.
		 *\param[in]	texture			La texture.
		 *\param[in]	pipeline		Le pipeline de rendu.
		 *\param[in]	matrixUbo		Le tampon d'uniformes recevant les matrices.
		 *\param[in]	geometryBuffers	Les tampons de géométrie utilisés pour dessiner la texture.
		 */
		C3D_API void doRender( castor::Position const & position
			, castor::Size const & size
			, renderer::Texture const & texture
			, renderer::Pipeline & pipeline
			, MatrixUbo & matrixUbo
			, renderer::GeometryBuffers const & geometryBuffers );
		/**
		 *\~english
		 *\brief		Creates the render a 2D texture shader program.
		 *\return		The program.
		 *\~french
		 *\brief		Crée le programme shader de dessin de texture 2D.
		 *\return		Le programme.
		 */
		renderer::ShaderProgramPtr doCreateProgramDepth();
		/**
		 *\~english
		 *\brief		Creates the render a 2D texture shader program.
		 *\return		The program.
		 *\~french
		 *\brief		Crée le programme shader de dessin de texture 2D.
		 *\return		Le programme.
		 */
		renderer::ShaderProgramPtr doCreateProgramVariance();
		void doInitialiseDepth();
		void doInitialiseVariance();

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
		renderer::GeometryBuffersPtr m_geometryBuffersDepth;
		//!\~english	The shader program.
		//!\~french		Le programme shader.
		renderer::ShaderProgramPtr m_programDepth;
		//!\~english	The pipeline used to render a texture in the current draw-bound framebuffer.
		//!\~french		Le pipeline utilisé pour le rendu d'une texture dans le tampon d'image actuellement activé en dessin.
		renderer::PipelinePtr m_pipelineDepth;
		//!\~english	The GeometryBuffers used when rendering a texture to the current frame buffer.
		//!\~french		Le GeometryBuffers utilisé lors du dessin d'une texture dans le tampon d'image courant.
		renderer::GeometryBuffersPtr m_geometryBuffersVariance;
		//!\~english	The shader program.
		//!\~french		Le programme shader.
		renderer::ShaderProgramPtr m_programVariance;
		//!\~english	The pipeline used to render a texture in the current draw-bound framebuffer.
		//!\~french		Le pipeline utilisé pour le rendu d'une texture dans le tampon d'image actuellement activé en dessin.
		renderer::PipelinePtr m_pipelineVariance;
		//!\~english	The sampler for the texture.
		//!\~french		Le sampler pour la texture.
		SamplerSPtr m_sampler;
	};
}

#endif
