/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___C3D_CONTEXT_H___
#define ___C3D_CONTEXT_H___

#include "Castor3DPrerequisites.hpp"
#include "Viewport.hpp"

#include "Mesh/Buffer/BufferDeclaration.hpp"

#include <Graphics/Colour.hpp>
#include <Design/OwnedBy.hpp>

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date		03/01/2011
	\version	0.6.1.1
	\~english
	\brief		Class which holds the rendering context
	\~french
	\brief		Classe contenant le contexte de rendu
	*/
	class Context
		: public Castor::OwnedBy< RenderSystem >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_renderSystem	The RenderSystem.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_renderSystem	Le RenderSystem.
		 */
		C3D_API explicit Context( RenderSystem & p_renderSystem );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~Context();
		/**
		 *\~english
		 *\brief		Initialises this context
		 *\param[in]	p_window	The RenderWindow
		 *\return		\p true if initialised, false if not
		 *\~french
		 *\brief		Initialise le contexte
		 *\param[in]	p_window	La RenderWindow
		 *\return		\p true si initialisé correctement
		 */
		C3D_API bool Initialise( RenderWindow * p_window );
		/**
		 *\~english
		 *\brief		Cleans this context up
		 *\~french
		 *\brief		Nettoie le contexte
		 */
		C3D_API void Cleanup();
		/**
		 *\~english
		 *\brief		Defines this context to be the current rendering context
		 *\~french
		 *\brief		Définit le contexte pour être celui de rendu actuel
		 */
		C3D_API void SetCurrent();
		/**
		 *\~english
		 *\brief		Defines this context not to be the current rendering context
		 *\~french
		 *\brief		Définit ce contexte pour ne pas être l'actuel
		 */
		C3D_API void EndCurrent();
		/**
		 *\~english
		 *\brief		Swaps render buffers
		 *\~french
		 *\brief		Echange les buffers de rendu
		 */
		C3D_API void SwapBuffers();
		/**
		 *\~english
		 *\brief		Renders the given texture to the currently draw-bound frame buffer.
		 *\param[in]	p_size			The render viewport size.
		 *\param[in]	p_texture		The texture.
		 *\~french
		 *\brief		Rend la texture donnée dans le tampon d'image actuellement activé en dessin.
		 *\param[in]	p_size			La taille du viewport de rendu.
		 *\param[in]	p_texture		La texture.
		 */
		C3D_API void RenderTexture( Castor::Size const & p_size, TextureLayout const & p_texture );
		/**
		 *\~english
		 *\brief		Renders the given texture.
		 *\param[in]	p_size				The render viewport size.
		 *\param[in]	p_texture			The texture.
		 *\param[in]	p_program			The program used to render the texture.
		 *\~french
		 *\brief		Dessine la texture donnée.
		 *\param[in]	p_size				La taille du viewport de rendu.
		 *\param[in]	p_texture			La texture.
		 *\param[in]	p_program			Le programme utilisé pour dessiner la texture.
		 */
		C3D_API void RenderTexture( Castor::Size const & p_size, TextureLayout const & p_texture, ShaderProgramSPtr p_program );
		/**
		 *\~english
		 *\brief		Renders the given depth texture to the currently draw-bound frame buffer.
		 *\param[in]	p_size			The render viewport size.
		 *\param[in]	p_texture		The texture.
		 *\~french
		 *\brief		Rend la texture de profondeur donnée dans le tampon d'image actuellement activé en dessin.
		 *\param[in]	p_size			La taille du viewport de rendu.
		 *\param[in]	p_texture		La texture.
		 */
		C3D_API void RenderDepth( Castor::Size const & p_size, TextureLayout const & p_texture );
		/**
		 *\~english
		 *\brief		Tells the context is initialised
		 *\~french
		 *\brief		Dit si le contexte est initialisé
		 */
		inline bool IsInitialised()const
		{
			return m_initialised;
		}
		/**
		 *\~english
		 *\brief		Tells the context is using multisampling
		 *\~french
		 *\brief		Dit si le contexte utilise le multisampling
		 */
		inline bool IsMultiSampling()const
		{
			return m_bMultiSampling;
		}
		/**
		 *\~english
		 *\brief		Defines the multisampling status
		 *\param[in]	p_value	The status
		 *\~french
		 *\brief		Définit le statut du multisampling
		 *\param[in]	p_value	Le statut
		 */
		inline void SetMultiSampling( bool p_value )
		{
			m_bMultiSampling = p_value;
		}
		/**
		 *\~english
		 *\return		The render window.
		 *\~french
		 *\return		La fenêtre de rendu.
		 */
		inline RenderWindow const & GetWindow()const
		{
			return *m_window;
		}
		/**
		 *\~english
		 *\return		The render window.
		 *\~french
		 *\return		La fenêtre de rendu.
		 */
		inline RenderWindow & GetWindow()
		{
			return *m_window;
		}

	protected:
		/**
		 *\~english
		 *\brief		Renders the given texture.
		 *\param[in]	p_size				The render viewport size.
		 *\param[in]	p_texture			The texture.
		 *\param[in]	p_pipeline			The render pipeline.
		 *\param[in]	p_geometryBuffers	The geometry buffers used to render the texture.
		 *\param[in]	p_program			The program used to render the texture.
		 *\~french
		 *\brief		Dessine la texture donnée.
		 *\param[in]	p_size				La taille du viewport de rendu.
		 *\param[in]	p_texture			La texture.
		 *\param[in]	p_pipeline			Le pipeline de rendu.
		 *\param[in]	p_geometryBuffers	Les tampons de géométrie utilisés pour dessiner la texture.
		 *\param[in]	p_program			Le programme utilisé pour dessiner la texture.
		 */
		C3D_API void DoRenderTexture( Castor::Size const & p_size, TextureLayout const & p_texture, Pipeline & p_pipeline, GeometryBuffers const & p_geometryBuffers, ShaderProgram const & p_program );
		/**
		 *\~english
		 *\brief		Creates the render to texture shader program.
		 *\return		The program.
		 *\~french
		 *\brief		Crée le programme shader de dessin de texture.
		 *\return		Le programme.
		 */
		ShaderProgramSPtr DoCreateProgram( bool p_depth );
		/**
		 *\~english
		 *\brief		Initialises this context
		 *\return		\p true if initialised, false if not
		 *\~french
		 *\brief		Initialise le contexte
		 *\return		\p true si initialisé correctement
		 */
		C3D_API virtual bool DoInitialise() = 0;
		/**
		 *\~english
		 *\brief		Cleans this context up
		 *\~french
		 *\brief		Nettoie le contexte
		 */
		C3D_API virtual void DoCleanup() = 0;
		/**
		 *\~english
		 *\brief		Destroys the context on GPU.
		 *\~french
		 *\brief		Détruit le contexte sur le GPU.
		 */
		C3D_API virtual void DoDestroy() = 0;
		/**
		 *\~english
		 *\brief		Defines this context to be the current rendering context
		 *\~french
		 *\brief		Définit le contexte pour être celui de rendu actuel
		 */
		C3D_API virtual void DoSetCurrent() = 0;
		/**
		 *\~english
		 *\brief		Defines this context not to be the current rendering context
		 *\~french
		 *\brief		Définit ce contexte pour ne pas être l'actuel
		 */
		C3D_API virtual void DoEndCurrent() = 0;
		/**
		 *\~english
		 *\brief		Swaps render buffers
		 *\~french
		 *\brief		Echange les buffers de rendu
		 */
		C3D_API virtual void DoSwapBuffers() = 0;

	protected:
		//!\~english	RenderWindow associated to this context.
		//!\~french		RenderWindow associée à ce contexte.
		RenderWindow * m_window;
		//!\~english	Tells if the context is initialised.
		//!\~french		Dit si le contexte est initialisé.
		bool m_initialised;
		//!\~english	Tells the context is currently set to use multisampling.
		//!\~french		Dit si le contexte est actuellement configuré pour utiliser le multisampling.
		bool m_bMultiSampling;
		//!\~english	The ShaderProgram used when rendering from a buffer to another one.
		//!\~french		Le ShaderProgram utilisé lors du rendu d'un tampon vers un autre.
		ShaderProgramWPtr m_renderTextureProgram;
		//!\~english	The ShaderProgram used when rendering depth from a buffer to another one.
		//!\~french		Le ShaderProgram utilisé lors du rendu de profondeur d'un tampon vers un autre.
		ShaderProgramWPtr m_renderDepthProgram;
		//!\~english	The diffuse map frame variable, in the buffer-to-buffer shader program.
		//!\~french		La frame variable de l'image diffuse, dans le shader buffer-to-buffer.
		OneIntFrameVariableSPtr m_mapDiffuse;
		//!\~english	The GeometryBuffers used when rendering a texture to the current frame buffer.
		//!\~french		Le GeometryBuffers utilisé lors du dessin d'une texture dans le tampon d'image courant.
		GeometryBuffersSPtr m_geometryBuffers;
		//!\~english	The GeometryBuffers used when rendering a depth texture to the current frame buffer.
		//!\~french		Le GeometryBuffers utilisé lors du dessin d'une texture de profondeur dans le tampon d'image courant.
		GeometryBuffersSPtr m_geometryBuffersDepth;
		//!\~english	The Viewport used when rendering a texture into to a frame buffer.
		//!\~french		Le Viewport utilisé lors du dessin d'une texture dans un tampon d'image.
		Viewport m_viewport;
		//!\~english	Buffer elements declaration.
		//!\~french		Déclaration des éléments d'un vertex.
		Castor3D::BufferDeclaration m_declaration;
		//!\~english	Vertex array (quad definition).
		//!\~french		Tableau de vertex (définition du quad).
		std::array< Castor3D::BufferElementGroupSPtr, 6 > m_arrayVertex;
		//!	6 * [2(vertex position) 2(texture coordinates)]
		Castor::real m_bufferVertex[24];
		//!\~english	The vertex buffer.
		//!\~french		Le tampon de sommets.
		VertexBufferSPtr m_vertexBuffer;
		//!\~english	The vertex buffer.
		//!\~french		Le tampon de sommets.
		VertexBufferSPtr m_vertexBufferDepth;
		//!\~english	The GPU time elapsed queries.
		//!\~french		Les requêtes GPU de temps écoulé.
		std::array< GpuQuerySPtr, 2 > m_timerQuery;
		//!\~english	The active query index.
		//!\~french		L'index de la requête active.
		uint32_t m_queryIndex = 0;
		//!\~english	The pipeline used for render to texture.
		//!\~french		Le pipeline utilisé pour le rendu en texture.
		PipelineUPtr m_texturePipeline;
		//!\~english	The pipeline used for render to depth.
		//!\~french		Le pipeline utilisé pour le rendu en profondeur.
		PipelineUPtr m_depthPipeline;
	};
}

#endif
