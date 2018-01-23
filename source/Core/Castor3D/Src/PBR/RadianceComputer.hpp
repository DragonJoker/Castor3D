/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RadianceComputer_H___
#define ___C3D_RadianceComputer_H___

#include "Render/Viewport.hpp"

#include "Shader/Ubos/MatrixUbo.hpp"

#include <Pipeline/VertexLayout.hpp>

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
	class RadianceComputer
		: public castor::OwnedBy< Engine >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\param[in]	size	The render size.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 *\param[in]	size	La taille du rendu.
		 */
		C3D_API explicit RadianceComputer( Engine & engine
			, castor::Size const & size );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~RadianceComputer();
		/**
		 *\~english
		 *\brief		Renders the wanted equirectangular 2D texture to given cube texture.
		 *\param[in]	srcTexture	The cube texture source.
		 *\param[in]	dstTexture	The cube texture destination.
		 *\~french
		 *\brief		Dessine a texture equirectangulaire 2D donnée dans la texture cube donnée.
		 *\param[in]	srcTexture	La texture cube source.
		 *\param[in]	dstTexture	La texture cube destination.
		 */
		C3D_API void render( TextureLayout const & srcTexture
			, TextureLayout const & dstTexture );

	private:
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
		MatrixUbo m_matrixUbo;
		//!\~english	The resulting dimensions.
		//!\~french		Les dimensions du résultat.
		castor::Size m_size;
		//!\~english	The Viewport used when rendering a texture into to a frame buffer.
		//!\~french		Le Viewport utilisé lors du dessin d'une texture dans un tampon d'image.
		Viewport m_viewport;
		//!\~english	Buffer elements declaration.
		//!\~french		Déclaration des éléments d'un vertex.
		renderer::VertexLayoutPtr m_declaration;
		//!\~english	The vertex buffer.
		//!\~french		Le tampon de sommets.
		renderer::VertexBufferPtr< TexturedCube > m_vertexBuffer;
		//!\~english	The GeometryBuffers used when rendering a texture to the frame buffer.
		//!\~french		Le GeometryBuffers utilisé lors du dessin d'une texture dans le tampon d'image.
		renderer::GeometryBuffersPtr m_geometryBuffers;
		//!\~english	The pipeline used to render a texture in the framebuffer.
		//!\~french		Le pipeline utilisé pour le rendu d'une texture dans le tampon d'image.
		RenderPipelineUPtr m_pipeline;
		//!\~english	The frame buffer.
		//!\~french		Le tampon d'image.
		renderer::FrameBufferPtr m_frameBuffer;
		//!\~english	The depth buffer.
		//!\~french		Le tampon de profondeur.
		renderer::RenderBufferPtr m_depthBuffer;
		//!\~english	The depth buffer attach.
		//!\~french		L'attache du tampon de profondeur.
		// RenderBufferAttachmentSPtr m_depthAttach;
		//!\~english	The sampler for the texture.
		//!\~french		Le sampler pour la texture.
		SamplerSPtr m_sampler;
	};
}

#endif
