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
#ifndef ___C3D_Context_H___
#define ___C3D_Context_H___

#include "RenderToTexture/RenderColourCubeToTexture.hpp"
#include "RenderToTexture/RenderColourLayerCubeToTexture.hpp"
#include "RenderToTexture/RenderColourLayerToTexture.hpp"
#include "RenderToTexture/RenderColourToCube.hpp"
#include "RenderToTexture/RenderColourToTexture.hpp"
#include "RenderToTexture/RenderDepthCubeToTexture.hpp"
#include "RenderToTexture/RenderDepthLayerCubeToTexture.hpp"
#include "RenderToTexture/RenderDepthLayerToTexture.hpp"
#include "RenderToTexture/RenderDepthToTexture.hpp"

#include "Shader/MatrixUbo.hpp"

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
		 *\param[in]	renderSystem	The RenderSystem.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	renderSystem	Le RenderSystem.
		 */
		C3D_API explicit Context( RenderSystem & renderSystem );
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
		 *\param[in]	window	The RenderWindow
		 *\return		\p true if initialised, false if not
		 *\~french
		 *\brief		Initialise le contexte
		 *\param[in]	window	La RenderWindow
		 *\return		\p true si initialisé correctement
		 */
		C3D_API bool Initialise( RenderWindow * window );
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
		 *\brief		Raise a memory barrier.
		 *\param[in]	barriers	The barrier to wait.
		 *\~french
		 *\brief		Met en place un barrière mémoire.
		 *\param[in]	barriers	La barrière à attendre.
		 */
		C3D_API void Barrier( MemoryBarriers const & barriers );
		/**
		 *\~english
		 *\brief		Renders the given cube texture to the currently draw-bound frame buffer.
		 *\param[in]	position	The render viewport position.
		 *\param[in]	size		The render viewport size.
		 *\param[in]	texture	The texture.
		 *\~french
		 *\brief		Rend la texture cube donnée dans le tampon d'image actuellement activé en dessin.
		 *\param[in]	position	La position du viewport de rendu.
		 *\param[in]	size		La taille du viewport de rendu.
		 *\param[in]	texture	La texture.
		 */
		C3D_API void RenderTextureCube( Castor::Position const & position
			, Castor::Size const & size
			, TextureLayout const & texture );
		/**
		 *\~english
		 *\brief		Renders the wanted layer of given cube texture array to the currently draw-bound frame buffer.
		 *\param[in]	size	The render viewport size.
		 *\param[in]	texture	The texture.
		 *\param[in]	index	The layer index.
		 *\~french
		 *\brief		Rend la couche voulue du tableau de textures cube donné dans le tampon d'image actuellement activé en dessin.
		 *\param[in]	size	La taille du viewport de rendu.
		 *\param[in]	texture	La texture.
		 *\param[in]	index	L'index de la couche.
		 */
		C3D_API void RenderTextureCube( Castor::Size const & size
			, TextureLayout const & texture
			, uint32_t index );
		/**
		 *\~english
		 *\brief		Renders the given 2D texture.
		 *\param[in]	position	The render viewport position.
		 *\param[in]	size		The render viewport size.
		 *\param[in]	texture		The texture.
		 *\param[in]	pipeline	The pipleline used to render the texture.
		 *\param[in]	matrixUbo	The uniform buffer receiving matrices.
		 *\~french
		 *\brief		Dessine la texture 2D donnée.
		 *\param[in]	position	La position du viewport de rendu.
		 *\param[in]	size		La taille du viewport de rendu.
		 *\param[in]	texture		La texture.
		 *\param[in]	pipeline	Le pipeline utilisé pour dessiner la texture.
		 *\param[in]	matrixUbo	Le tampon d'uniformes recevant les matrices.
		 */
		C3D_API void RenderTexture( Castor::Position const & position
			, Castor::Size const & size
			, TextureLayout const & texture
			, RenderPipeline & pipeline
			, MatrixUbo & matrixUbo );
		/**
		 *\~english
		 *\brief		Renders the given 2D texture to the currently draw-bound frame buffer.
		 *\param[in]	position	The render viewport position.
		 *\param[in]	size		The render viewport size.
		 *\param[in]	texture		The texture.
		 *\~french
		 *\brief		Rend la texture 2D donnée dans le tampon d'image actuellement activé en dessin.
		 *\param[in]	position	La position du viewport de rendu.
		 *\param[in]	size		La taille du viewport de rendu.
		 *\param[in]	texture		La texture.
		 */
		C3D_API void RenderTexture( Castor::Position const & position
			, Castor::Size const & size
			, TextureLayout const & texture );
		/**
		 *\~english
		 *\brief		Renders the wanted layer of given 2D texture array to the currently draw-bound frame buffer.
		 *\param[in]	position	The render viewport position.
		 *\param[in]	size		The render viewport size.
		 *\param[in]	texture		The texture.
		 *\param[in]	index		The layer index.
		 *\~french
		 *\brief		Rend la couche voulue du tableau de textures 2D donné dans le tampon d'image actuellement activé en dessin.
		 *\param[in]	position	La position du viewport de rendu.
		 *\param[in]	size		La taille du viewport de rendu.
		 *\param[in]	texture		La texture.
		 *\param[in]	index		L'index de la couche.
		 */
		C3D_API void RenderTexture( Castor::Position const & position
			, Castor::Size const & size
			, TextureLayout const & texture
			, uint32_t index );
		/**
		 *\~english
		 *\brief		Renders the given depth cube texture to the currently draw-bound frame buffer.
		 *\param[in]	position	The render viewport position.
		 *\param[in]	size		The render viewport size.
		 *\param[in]	texture		The texture.
		 *\~french
		 *\brief		Rend la texture cube de profondeur donnée dans le tampon d'image actuellement activé en dessin.
		 *\param[in]	position	La position du viewport de rendu.
		 *\param[in]	size		La taille du viewport de rendu.
		 *\param[in]	texture		La texture.
		 */
		C3D_API void RenderDepthCube( Castor::Position const & position
			, Castor::Size const & size
			, TextureLayout const & texture );
		/**
		 *\~english
		 *\brief		Renders the wanted layer of given depth cube texture array to the currently draw-bound frame buffer.
		 *\param[in]	position	The render viewport position.
		 *\param[in]	size		The render viewport size.
		 *\param[in]	texture		The texture.
		 *\param[in]	index		The layer index.
		 *\~french
		 *\brief		Rend la couche voulue du tableau de textures cube de profondeur donné dans le tampon d'image actuellement activé en dessin.
		 *\param[in]	position	La position du viewport de rendu.
		 *\param[in]	size		La taille du viewport de rendu.
		 *\param[in]	texture		La texture.
		 *\param[in]	index		L'index de la couche.
		 */
		C3D_API void RenderDepthCube( Castor::Position const & position
			, Castor::Size const & size
			, TextureLayout const & texture
			, uint32_t index );
		/**
		 *\~english
		 *\brief		Renders the given depth 2D texture to the currently draw-bound frame buffer.
		 *\param[in]	position	The render viewport position.
		 *\param[in]	size		The render viewport size.
		 *\param[in]	texture		The texture.
		 *\~french
		 *\brief		Rend la texture 2D de profondeur donnée dans le tampon d'image actuellement activé en dessin.
		 *\param[in]	position	La position du viewport de rendu.
		 *\param[in]	size		La taille du viewport de rendu.
		 *\param[in]	texture		La texture.
		 */
		C3D_API void RenderDepth( Castor::Position const & position
			, Castor::Size const & size
			, TextureLayout const & texture );
		/**
		 *\~english
		 *\brief		Renders the wanted layer of given depth 2D texture array to the currently draw-bound frame buffer.
		 *\param[in]	position	The render viewport position.
		 *\param[in]	size		The render viewport size.
		 *\param[in]	texture		The texture.
		 *\param[in]	index		The layer index.
		 *\~french
		 *\brief		Rend la couche voulue du tableau de textures 2D de profondeur donné dans le tampon d'image actuellement activé en dessin.
		 *\param[in]	position	La position du viewport de rendu.
		 *\param[in]	size		La taille du viewport de rendu.
		 *\param[in]	texture		La texture.
		 *\param[in]	index		L'index de la couche.
		 */
		C3D_API void RenderDepth( Castor::Position const & position
			, Castor::Size const & size
			, TextureLayout const & texture
			, uint32_t index );
		/**
		 *\~english
		 *\brief		Renders the given 2D texture.
		 *\param[in]	size		The render viewport size.
		 *\param[in]	texture		The texture.
		 *\param[in]	pipeline	The pipleline used to render the texture.
		 *\param[in]	matrixUbo	The uniform buffer receiving matrices.
		 *\~french
		 *\brief		Dessine la texture 2D donnée.
		 *\param[in]	size		La taille du viewport de rendu.
		 *\param[in]	texture		La texture.
		 *\param[in]	pipeline	Le pipeline utilisé pour dessiner la texture.
		 *\param[in]	matrixUbo	Le tampon d'uniformes recevant les matrices.
		 */
		inline void RenderTexture( Castor::Size const & size
			, TextureLayout const & texture
			, RenderPipeline & pipeline
			, MatrixUbo & matrixUbo )
		{
			static Castor::Position const position;
			RenderTexture( position
				, size
				, texture
				, pipeline
				, matrixUbo );
		}
		/**
		 *\~english
		 *\brief		Renders the given 2D texture to the currently draw-bound frame buffer.
		 *\param[in]	size		The render viewport size.
		 *\param[in]	texture		The texture.
		 *\~french
		 *\brief		Rend la texture 2D donnée dans le tampon d'image actuellement activé en dessin.
		 *\param[in]	size		La taille du viewport de rendu.
		 *\param[in]	texture		La texture.
		 */
		inline void RenderTexture( Castor::Size const & size
			, TextureLayout const & texture )
		{
			static Castor::Position const position;
			RenderTexture( position
				, size
				, texture );
		}
		/**
		 *\~english
		 *\brief		Renders the wanted layer of given 2D texture array to the currently draw-bound frame buffer.
		 *\param[in]	size	The render viewport size.
		 *\param[in]	texture	The texture.
		 *\param[in]	index	The layer index.
		 *\~french
		 *\brief		Rend la couche voulue du tableau de textures 2D donné dans le tampon d'image actuellement activé en dessin.
		 *\param[in]	size	La taille du viewport de rendu.
		 *\param[in]	texture	La texture.
		 *\param[in]	index	L'index de la couche.
		 */
		inline void RenderTexture( Castor::Size const & size
			, TextureLayout const & texture
			, uint32_t index )
		{
			static Castor::Position const position;
			RenderTexture( position
				, size
				, texture
				, index );
		}
		/**
		 *\~english
		 *\brief		Renders the given cube texture to the currently draw-bound frame buffer.
		 *\param[in]	size	The render viewport size.
		 *\param[in]	texture	The texture.
		 *\~french
		 *\brief		Rend la texture cube donnée dans le tampon d'image actuellement activé en dessin.
		 *\param[in]	size	La taille du viewport de rendu.
		 *\param[in]	texture	La texture.
		 */
		inline void RenderTextureCube( Castor::Size const & size
			, TextureLayout const & texture )
		{
			static Castor::Position const position;
			RenderTextureCube( position
				, size
				, texture );
		}
		/**
		 *\~english
		 *\brief		Renders the given depth 2D texture to the currently draw-bound frame buffer.
		 *\param[in]	size	The render viewport size.
		 *\param[in]	texture	The texture.
		 *\~french
		 *\brief		Rend la texture 2D de profondeur donnée dans le tampon d'image actuellement activé en dessin.
		 *\param[in]	size	La taille du viewport de rendu.
		 *\param[in]	texture	La texture.
		 */
		inline void RenderDepth( Castor::Size const & size
			, TextureLayout const & texture )
		{
			static Castor::Position const position;
			RenderDepth( position
				, size
				, texture );
		}
		/**
		 *\~english
		 *\brief		Renders the wanted layer of given depth 2D texture array to the currently draw-bound frame buffer.
		 *\param[in]	size	The render viewport size.
		 *\param[in]	texture	The texture.
		 *\param[in]	index	The layer index.
		 *\~french
		 *\brief		Rend la couche voulue du tableau de textures 2D de profondeur donné dans le tampon d'image actuellement activé en dessin.
		 *\param[in]	size	La taille du viewport de rendu.
		 *\param[in]	texture	La texture.
		 *\param[in]	index	L'index de la couche.
		 */
		inline void RenderDepth( Castor::Size const & size
			, TextureLayout const & texture
			, uint32_t index )
		{
			static Castor::Position const position;
			RenderDepth( position
				, size
				, texture
				, index );
		}
		/**
		 *\~english
		 *\brief		Renders the given depth cube texture to the currently draw-bound frame buffer.
		 *\param[in]	size	The render viewport size.
		 *\param[in]	texture	The texture.
		 *\~french
		 *\brief		Rend la texture cube de profondeur donnée dans le tampon d'image actuellement activé en dessin.
		 *\param[in]	size	La taille du viewport de rendu.
		 *\param[in]	texture	La texture.
		 */
		inline void RenderDepthCube( Castor::Size const & size
			, TextureLayout const & texture )
		{
			static Castor::Position const position;
			RenderDepth( position
				, size
				, texture );
		}
		/**
		 *\~english
		 *\brief		Renders the wanted layer of given depth cube texture array to the currently draw-bound frame buffer.
		 *\param[in]	size	The render viewport size.
		 *\param[in]	texture	The texture.
		 *\param[in]	index	The layer index.
		 *\~french
		 *\brief		Rend la couche voulue du tableau de textures cube de profondeur donné dans le tampon d'image actuellement activé en dessin.
		 *\param[in]	size	La taille du viewport de rendu.
		 *\param[in]	texture	La texture.
		 *\param[in]	index	L'index de la couche.
		 */
		inline void RenderDepthCube( Castor::Size const & size
			, TextureLayout const & texture
			, uint32_t index )
		{
			static Castor::Position const position;
			RenderDepth( position
				, size
				, texture
				, index );
		}
		/**
		 *\~english
		 *\brief		Renders the wanted layer of given depth cube texture array to the currently draw-bound frame buffer.
		 *\param[in]	size	The render viewport size.
		 *\param[in]	texture	The texture.
		 *\param[in]	index	The layer index.
		 *\~french
		 *\brief		Rend la couche voulue du tableau de textures cube de profondeur donné dans le tampon d'image actuellement activé en dessin.
		 *\param[in]	size	La taille du viewport de rendu.
		 *\param[in]	texture	La texture.
		 *\param[in]	index	L'index de la couche.
		 */
		inline void RenderEquiToCube( Castor::Size const & size
			, TextureLayout const & p_2dTexture
			, TextureLayoutSPtr p_cubeTexture
			, FrameBufferSPtr p_fbo
			, std::array< FrameBufferAttachmentSPtr, 6 > const & p_attachs )
		{
			m_cube.Render( size
				, p_2dTexture
				, p_cubeTexture
				, p_fbo
				, p_attachs );
		}
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
		 *\brief		Renders the wanted face of given cube texture.
		 *\param[in]	position		The render viewport position.
		 *\param[in]	size			The render viewport size.
		 *\param[in]	texture			The texture.
		 *\param[in]	face			The cube face.
		 *\param[in]	pipeline		The render pipeline.
		 *\param[in]	matrixUbo		The uniform buffer receiving matrices.
		 *\param[in]	geometryBuffers	The geometry buffers used to render the texture.
		 *\~french
		 *\brief		Dessine la face voulue de la texture cube donnée.
		 *\param[in]	position		La position du viewport de rendu.
		 *\param[in]	size			La taille du viewport de rendu.
		 *\param[in]	texture			La texture.
		 *\param[in]	face			La face du cube.
		 *\param[in]	pipeline		Le pipeline de rendu.
		 *\param[in]	matrixUbo		Le tampon d'uniformes recevant les matrices.
		 *\param[in]	geometryBuffers	Les tampons de géométrie utilisés pour dessiner la texture.
		 */
		C3D_API void DoRenderTextureFace( Castor::Position const & position
			, Castor::Size const & size
			, TextureLayout const & texture
			, CubeMapFace face
			, RenderPipeline & pipeline
			, MatrixUbo & matrixUbo
			, GeometryBuffers const & geometryBuffers );
		/**
		 *\~english
		 *\brief		Renders the wanted face of wanted layer of given cube texture array.
		 *\param[in]	position		The render viewport position.
		 *\param[in]	size			The render viewport size.
		 *\param[in]	texture			The texture.
		 *\param[in]	face			The cube face.
		 *\param[in]	pipeline		The render pipeline.
		 *\param[in]	matrixUbo		The uniform buffer receiving matrices.
		 *\param[in]	geometryBuffers	The geometry buffers used to render the texture.
		 *\param[in]	index			The layer index.
		 *\~french
		 *\brief		Dessine la face voulue de la couche voulue du tableau de textures cube donné.
		 *\param[in]	position		La position du viewport de rendu.
		 *\param[in]	size			La taille du viewport de rendu.
		 *\param[in]	texture			La texture.
		 *\param[in]	face			La face du cube.
		 *\param[in]	pipeline		Le pipeline de rendu.
		 *\param[in]	matrixUbo		Le tampon d'uniformes recevant les matrices.
		 *\param[in]	geometryBuffers	Les tampons de géométrie utilisés pour dessiner la texture.
		 *\param[in]	index			L'index de la couche.
		 */
		C3D_API void DoRenderTextureFace( Castor::Position const & position
			, Castor::Size const & size
			, TextureLayout const & texture
			, CubeMapFace face
			, RenderPipeline & pipeline
			, MatrixUbo & matrixUbo
			, GeometryBuffers const & geometryBuffers
			, uint32_t index );
		/**
		 *\~english
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
		/**
		 *\~english
		 *\brief		Raise a memory barrier.
		 *\param[in]	barriers	The barrier to wait.
		 *\~french
		 *\brief		Met en place un barrière mémoire.
		 *\param[in]	barriers	La barrière à attendre.
		 */
		C3D_API virtual void DoBarrier( MemoryBarriers const & barriers ) = 0;

	protected:
		//!\~english	RenderWindow associated to this context.
		//!\~french		RenderWindow associée à ce contexte.
		RenderWindow * m_window;
		//!\~english	Tells if the context is initialised.
		//!\~french		Dit si le contexte est initialisé.
		bool m_initialised;
		//!\~english	The uniform buffer containing matrices data.
		//!\~french		Le tampon d'uniformes contenant les données de matrices.
		MatrixUbo m_matrixUbo;
		//!\~english	The pipeline used to render a texture in the current draw-bound framebuffer.
		//!\~french		Le pipeline utilisé pour le rendu d'une texture dans le tampon d'image actuellement activé en dessin.
		RenderColourToTexture m_colour;
		//!\~english	The pipeline used to render a texture in the current draw-bound framebuffer.
		//!\~french		Le pipeline utilisé pour le rendu d'une texture dans le tampon d'image actuellement activé en dessin.
		RenderColourCubeToTexture m_colourCube;
		//!\~english	The pipeline used to render a layer from a texture array in the current draw-bound framebuffer.
		//!\~french		Le pipeline utilisé pour le rendu d'une couche d'un tableau de textures dans le tampon d'image actuellement activé en dessin.
		RenderColourLayerToTexture m_colourLayer;
		//!\~english	The pipeline used to render a layer from a cube texture in the current draw-bound framebuffer.
		//!\~french		Le pipeline utilisé pour le rendu d'une couche d'une texture cube dans le tampon d'image actuellement activé en dessin.
		RenderColourLayerCubeToTexture m_colourLayerCube;
		//!\~english	The pipeline used to render a depth texture in the current draw-bound framebuffer.
		//!\~french		Le pipeline utilisé pour le rendu d'une texture de profondeur dans le tampon d'image actuellement activé en dessin.
		RenderDepthToTexture m_depth;
		//!\~english	The pipeline used to render a cube depth texture in the current draw-bound framebuffer.
		//!\~french		Le pipeline utilisé pour le rendu d'une texture cube de profondeur dans le tampon d'image actuellement activé en dessin.
		RenderDepthCubeToTexture m_depthCube;
		//!\~english	The pipeline used to render a layer from a depth texture array in the current draw-bound framebuffer.
		//!\~french		Le pipeline utilisé pour le rendu d'une couche d'un tableau de textures de profondeur dans le tampon d'image actuellement activé en dessin.
		RenderDepthLayerToTexture m_depthLayer;
		//!\~english	The pipeline used to render a layer from a cube depth texture in the current draw-bound framebuffer.
		//!\~french		Le pipeline utilisé pour le rendu d'une couche d'une texture cube de profondeur dans le tampon d'image actuellement activé en dessin.
		RenderDepthLayerCubeToTexture m_depthLayerCube;
		//!\~english	The pipeline used to render a cube texture in the current draw-bound framebuffer.
		//!\~french		Le pipeline utilisé pour le rendu d'une texture cube dans le tampon d'image actuellement activé en dessin.
		RenderColourToCube m_cube;
	};
}

#endif
