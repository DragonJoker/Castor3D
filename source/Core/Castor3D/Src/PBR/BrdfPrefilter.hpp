/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BrdfPrefilter_H___
#define ___C3D_BrdfPrefilter_H___

#include "Render/Viewport.hpp"

#include "Mesh/Buffer/BufferDeclaration.hpp"
#include "Shader/Ubos/MatrixUbo.hpp"

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
	class BrdfPrefilter
		: public castor::OwnedBy< Engine >
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
		C3D_API explicit BrdfPrefilter( Engine & engine
			, castor::Size const & p_size );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~BrdfPrefilter();
		/**
		 *\~english
		 *\brief		Computes the convoluted BRDF.
		 *\param[in]	p_dstTexture	The cube texture destination.
		 *\~french
		 *\brief		Calcule le BRDF circonvolu.
		 *\param[in]	p_dstTexture	La texture cube destination.
		 */
		C3D_API void render( TextureLayoutSPtr p_dstTexture );

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
		MatrixUbo m_matrixUbo;
		//!\~english	The resulting dimensions.
		//!\~french		Les dimensions du résultat.
		castor::Size m_size;
		//!\~english	The Viewport used when rendering a texture into to a frame buffer.
		//!\~french		Le Viewport utilisé lors du dessin d'une texture dans un tampon d'image.
		Viewport m_viewport;
		//!	6 * 2(vertex position)
		std::array< castor::real, 6 * 2 > m_bufferVertex;
		//!\~english	Buffer elements declaration.
		//!\~french		Déclaration des éléments d'un vertex.
		castor3d::BufferDeclaration m_declaration;
		//!\~english	Vertex array (quad definition).
		//!\~french		Tableau de vertex (définition du quad).
		std::array< castor3d::BufferElementGroupSPtr, 6 > m_arrayVertex;
		//!\~english	The vertex buffer.
		//!\~french		Le tampon de sommets.
		VertexBufferSPtr m_vertexBuffer;
		//!\~english	The GeometryBuffers used when rendering a texture to the frame buffer.
		//!\~french		Le GeometryBuffers utilisé lors du dessin d'une texture dans le tampon d'image.
		GeometryBuffersSPtr m_geometryBuffers;
		//!\~english	The pipeline used to render a texture in the framebuffer.
		//!\~french		Le pipeline utilisé pour le rendu d'une texture dans le tampon d'image.
		RenderPipelineUPtr m_pipeline;
		//!\~english	The frame buffer.
		//!\~french		Le tampon d'image.
		FrameBufferSPtr m_frameBuffer;
		//!\~english	The depth buffer.
		//!\~french		Le tampon de profondeur.
		DepthStencilRenderBufferSPtr m_depthBuffer;
		//!\~english	The depth buffer attach.
		//!\~french		L'attache du tampon de profondeur.
		RenderBufferAttachmentSPtr m_depthAttach;
		//!\~english	The roughness variable.
		//!\~french		La variable pour la roughness.
		PushUniform1fSPtr m_roughnessUniform;
	};
}

#endif
