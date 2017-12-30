﻿/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderColourLayerToTexture_H___
#define ___C3D_RenderColourLayerToTexture_H___

#include "Render/Viewport.hpp"

#include "Mesh/Buffer/BufferDeclaration.hpp"

#include <Design/OwnedBy.hpp>
#include <Graphics/Position.hpp>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date		02/03/2017
	\version	0.9.0
	\~english
	\brief		Class used to render a colour texture array's layer.
	\~french
	\brief		Classe utilisée pour rendre une couche d'un tableau de textures couleur.
	*/
	class RenderColourLayerToTexture
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
		C3D_API explicit RenderColourLayerToTexture( Context & p_context
			, MatrixUbo & p_matrixUbo );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~RenderColourLayerToTexture();
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
		 *\brief		Renders the wanted layer of given texture array to the currently draw-bound frame buffer.
		 *\param[in]	p_position	The render viewport position.
		 *\param[in]	p_size		The render viewport size.
		 *\param[in]	p_texture	The texture.
		 *\param[in]	p_pipeline	The pipleline used to render the texture.
		 *\param[in]	p_matrixUbo	The uniform buffer receiving matrices.
		 *\param[in]	p_layer		The layer index.
		 *\~french
		 *\brief		Dessine la couche voulue du tableau de textures donnée dans le tampon d'image actuellement activé en dessin.
		 *\param[in]	p_position	La position du viewport de rendu.
		 *\param[in]	p_size		La taille du viewport de rendu.
		 *\param[in]	p_texture	La texture.
		 *\param[in]	p_pipeline	Le pipeline utilisé pour dessiner la texture.
		 *\param[in]	p_matrixUbo	Le tampon d'uniformes recevant les matrices.
		 *\param[in]	p_layer		L'index de la couche.
		 */
		C3D_API void render( castor::Position const & p_position
			, castor::Size const & p_size
			, TextureLayout const & p_texture
			, MatrixUbo & p_matrixUbo
			, RenderPipeline & p_pipeline
			, uint32_t p_layer );
		/**
		 *\~english
		 *\brief		Renders the given texture to the currently draw-bound frame buffer.
		 *\param[in]	p_position	The render viewport position.
		 *\param[in]	p_size		The render viewport size.
		 *\param[in]	p_texture	The texture.
		 *\param[in]	p_layer		The layer index.
		 *\~french
		 *\brief		Rend la texture donnée dans le tampon d'image actuellement activé en dessin.
		 *\param[in]	p_position	La position du viewport de rendu.
		 *\param[in]	p_size		La taille du viewport de rendu.
		 *\param[in]	p_texture	La texture.
		 *\param[in]	p_layer		L'index de la couche.
		 */
		C3D_API void render( castor::Position const & p_position
			, castor::Size const & p_size
			, TextureLayout const & p_texture
			, uint32_t p_layer );

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
		 *\param[in]	p_layer				The layer index.
		 *\~french
		 *\brief		Dessine la texture donnée.
		 *\param[in]	p_position			La position du viewport de rendu.
		 *\param[in]	p_size				La taille du viewport de rendu.
		 *\param[in]	p_texture			La texture.
		 *\param[in]	p_pipeline			Le pipeline de rendu.
		 *\param[in]	p_matrixUbo			Le tampon d'uniformes recevant les matrices.
		 *\param[in]	p_geometryBuffers	Les tampons de géométrie utilisés pour dessiner la texture.
		 *\param[in]	p_layer				L'index de la couche.
		 */
		C3D_API void doRender( castor::Position const & p_position
			, castor::Size const & p_size
			, TextureLayout const & p_texture
			, RenderPipeline & p_pipeline
			, MatrixUbo & p_matrixUbo
			, GeometryBuffers const & p_geometryBuffers
			, uint32_t p_layer );
		/**
		 *\~english
		 *\brief		Creates the render a 2D texture shader program.
		 *\return		The program.
		 *\~french
		 *\brief		Crée le programme shader de dessin de texture 2D.
		 *\return		Le programme.
		 */
		ShaderProgramSPtr doCreateProgram();

	private:
		//!\~english	The uniform buffer containing matrices data.
		//!\~french		Le tampon d'uniformes contenant les données de matrices.
		MatrixUbo & m_matrixUbo;
		//!\~english	The Viewport used when rendering a texture into to a frame buffer.
		//!\~french		Le Viewport utilisé lors du dessin d'une texture dans un tampon d'image.
		Viewport m_viewport;
		//!	6 * [2(vertex position) + 2(texture coordinates)]
		std::array< castor::real, 6 * ( 2 + 2 ) > m_bufferVertex;
		//!\~english	Buffer elements declaration.
		//!\~french		Déclaration des éléments d'un vertex.
		BufferDeclaration m_declaration;
		//!\~english	Vertex array (quad definition).
		//!\~french		Tableau de vertex (définition du quad).
		std::array< BufferElementGroupSPtr, 6 > m_arrayVertex;
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
		//!\~english	The uniform variable holding the index of the layer to render.
		//!\~french		La variable uniforme contenant l'indice de la couche à dessiner.
		PushUniform1iSPtr m_layerIndexUniform;
	};
}

#endif
