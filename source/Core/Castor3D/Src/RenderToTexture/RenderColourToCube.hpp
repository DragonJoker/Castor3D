/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderColourToCube_H___
#define ___C3D_RenderColourToCube_H___

#include "Render/Viewport.hpp"

#include "Mesh/Buffer/BufferDeclaration.hpp"
#include "Shader/Ubos/HdrConfigUbo.hpp"

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
	class RenderColourToCube
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
		C3D_API explicit RenderColourToCube( Context & context
			, MatrixUbo & matrixUbo );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~RenderColourToCube();
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
		 *\brief		Renders the wanted equirectangular 2D texture to given cube texture.
		 *\param[in]	size		La taille du viewport de rendu.
		 *\param[in]	texture		The 2D texture.
		 *\param[in]	cubeTexture	The cube texture.
		 *\param[in]	fbo			The active FBO
		 *\param[in]	attachs		The cube texture attaches to the active FBO.
		 *\param[in]	hdrConfig	The HDR configuration.
		 *\~french
		 *\brief		Dessine a texture equirectangulaire 2D donnée dans la texture cube donnée.
		 *\param[in]	size		La taille du viewport de rendu.
		 *\param[in]	texture		La texture 2D.
		 *\param[in]	cubeTexture	La texture cube.
		 *\param[in]	fbo			Le FBO actif.
		 *\param[in]	attachs		Les attaches de la texture cube au FBO actif.
		 *\param[in]	hdrConfig	La configuration HDR.
		 */
		C3D_API void render( castor::Size const & size
			, TextureLayout const & texture
			, TextureLayoutSPtr cubeTexture
			, FrameBufferSPtr fbo
			, std::array< FrameBufferAttachmentSPtr, 6 > const & attachs
			, HdrConfig const & hdrConfig );

	private:
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
		//!\~english	The HDR configuration UBO.
		//!\~french		L'UBO de configuration HDR.
		HdrConfigUbo m_configUbo;
		//!\~english	The dummy HDR configuration.
		//!\~french		La configuration HDR dummy.
		HdrConfig m_dummy;
	};
}

#endif
