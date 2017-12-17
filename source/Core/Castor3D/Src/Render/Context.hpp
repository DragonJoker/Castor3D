/*
See LICENSE file in root folder
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
#include "RenderToTexture/RenderVarianceToTexture.hpp"
#include "RenderToTexture/RenderVarianceCubeToTexture.hpp"

#include "Shader/Ubos/MatrixUbo.hpp"

namespace castor3d
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
		: public castor::OwnedBy< RenderSystem >
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
		C3D_API bool initialise( RenderWindow * window );
		/**
		 *\~english
		 *\brief		Cleans this context up
		 *\~french
		 *\brief		Nettoie le contexte
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Defines this context to be the current rendering context
		 *\~french
		 *\brief		Définit le contexte pour être celui de rendu actuel
		 */
		C3D_API void setCurrent();
		/**
		 *\~english
		 *\brief		Defines this context not to be the current rendering context
		 *\~french
		 *\brief		Définit ce contexte pour ne pas être l'actuel
		 */
		C3D_API void endCurrent();
		/**
		 *\~english
		 *\brief		Swaps render buffers
		 *\~french
		 *\brief		Echange les buffers de rendu
		 */
		C3D_API void swapBuffers();
		/**
		 *\~english
		 *\brief		Raise a memory barrier.
		 *\param[in]	barriers	The barrier to wait.
		 *\~french
		 *\brief		Met en place un barrière mémoire.
		 *\param[in]	barriers	La barrière à attendre.
		 */
		C3D_API void memoryBarrier( MemoryBarriers const & barriers );
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
		C3D_API void renderTextureCube( castor::Position const & position
			, castor::Size const & size
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
		C3D_API void renderTextureCube( castor::Size const & size
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
		C3D_API void renderTexture( castor::Position const & position
			, castor::Size const & size
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
		C3D_API void renderTexture( castor::Position const & position
			, castor::Size const & size
			, TextureLayout const & texture );
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
		C3D_API void renderTextureNearest( castor::Position const & position
			, castor::Size const & size
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
		C3D_API void renderTexture( castor::Position const & position
			, castor::Size const & size
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
		C3D_API void renderDepthCube( castor::Position const & position
			, castor::Size const & size
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
		C3D_API void renderDepthCube( castor::Position const & position
			, castor::Size const & size
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
		C3D_API void renderDepth( castor::Position const & position
			, castor::Size const & size
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
		C3D_API void renderDepth( castor::Position const & position
			, castor::Size const & size
			, TextureLayout const & texture
			, uint32_t index );
		/**
		 *\~english
		 *\brief		Renders the given variance 2D texture to the currently draw-bound frame buffer.
		 *\param[in]	position	The render viewport position.
		 *\param[in]	size		The render viewport size.
		 *\param[in]	texture		The texture.
		 *\~french
		 *\brief		Rend la texture 2D de variance donnée dans le tampon d'image actuellement activé en dessin.
		 *\param[in]	position	La position du viewport de rendu.
		 *\param[in]	size		La taille du viewport de rendu.
		 *\param[in]	texture		La texture.
		 */
		C3D_API void renderVariance( castor::Position const & position
			, castor::Size const & size
			, TextureLayout const & texture );
		/**
		 *\~english
		 *\brief		Renders the given variance cube texture to the currently draw-bound frame buffer.
		 *\param[in]	position	The render viewport position.
		 *\param[in]	size		The render viewport size.
		 *\param[in]	texture		The texture.
		 *\~french
		 *\brief		Rend la texture cube de variance donnée dans le tampon d'image actuellement activé en dessin.
		 *\param[in]	position	La position du viewport de rendu.
		 *\param[in]	size		La taille du viewport de rendu.
		 *\param[in]	texture		La texture.
		 */
		C3D_API void renderVarianceCube( castor::Position const & position
			, castor::Size const & size
			, TextureLayout const & texture );
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
		inline void renderTexture( castor::Size const & size
			, TextureLayout const & texture
			, RenderPipeline & pipeline
			, MatrixUbo & matrixUbo )
		{
			static castor::Position const position;
			renderTexture( position
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
		inline void renderTexture( castor::Size const & size
			, TextureLayout const & texture )
		{
			static castor::Position const position;
			renderTexture( position
				, size
				, texture );
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
		inline void renderTextureNearest( castor::Size const & size
			, TextureLayout const & texture )
		{
			static castor::Position const position;
			renderTextureNearest( position
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
		inline void renderTexture( castor::Size const & size
			, TextureLayout const & texture
			, uint32_t index )
		{
			static castor::Position const position;
			renderTexture( position
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
		inline void renderTextureCube( castor::Size const & size
			, TextureLayout const & texture )
		{
			static castor::Position const position;
			renderTextureCube( position
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
		inline void renderDepth( castor::Size const & size
			, TextureLayout const & texture )
		{
			static castor::Position const position;
			renderDepth( position
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
		inline void renderDepth( castor::Size const & size
			, TextureLayout const & texture
			, uint32_t index )
		{
			static castor::Position const position;
			renderDepth( position
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
		inline void renderDepthCube( castor::Size const & size
			, TextureLayout const & texture )
		{
			static castor::Position const position;
			renderDepth( position
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
		inline void renderDepthCube( castor::Size const & size
			, TextureLayout const & texture
			, uint32_t index )
		{
			static castor::Position const position;
			renderDepth( position
				, size
				, texture
				, index );
		}
		/**
		 *\~english
		 *\brief		Renders an equirectangular texture into a cube texture.
		 *\param[in]	size		The render viewport size.
		 *\param[in]	texture		The equirectangular texture.
		 *\param[in]	cubeTexture	The cube texture.
		 *\param[in]	fbo			The framebuffer used during render.
		 *\param[in]	attachs		The framebuffer attaches for the cube texture.
		 *\param[in]	hdrConfig	The HDR configuration.
		 *\~french
		 *\brief		Rend une texture équirectangulaire dans une texture cube.
		 *\param[in]	size		La taille du viewport de rendu.
		 *\param[in]	texture		La texture équirectangulaire.
		 *\param[in]	cubeTexture	La texture cube.
		 *\param[in]	fbo			Le tampon d'image utilisé lors du rendu.
		 *\param[in]	attachs		Les attaches entre le tampon d'images et la texture cube.
		 *\param[in]	hdrConfig	La configuration HDR.
		 */
		inline void renderEquiToCube( castor::Size const & size
			, TextureLayout const & texture
			, TextureLayoutSPtr cubeTexture
			, FrameBufferSPtr fbo
			, std::array< FrameBufferAttachmentSPtr, 6 > const & attachs
			, HdrConfig const & hdrConfig )
		{
			m_cube.render( size
				, texture
				, cubeTexture
				, fbo
				, attachs
				, hdrConfig );
		}
		/**
		 *\~english
		 *\brief		Renders the wanted face of given cube texture to the currently draw-bound frame buffer.
		 *\param[in]	size		The render viewport size.
		 *\param[in]	texture		The texture.
		 *\param[in]	face		The face index.
		 *\param[in]	pipeline	The pipleline used to render the texture.
		 *\param[in]	matrixUbo	The uniform buffer receiving matrices.
		 *\~french
		 *\brief		Rend la face voulue de la texture cube donnée dans le tampon d'image actuellement activé en dessin.
		 *\param[in]	size		La taille du viewport de rendu.
		 *\param[in]	texture		La texture.
		 *\param[in]	face		L'index de la face.
		 *\param[in]	pipeline	Le pipeline utilisé pour dessiner la texture.
		 *\param[in]	matrixUbo	Le tampon d'uniformes recevant les matrices.
		 */
		inline void renderCubeFace( castor::Size const & size
			, TextureLayoutSPtr texture
			, CubeMapFace face
			, RenderPipeline & pipeline
			, MatrixUbo & matrixUbo )
		{
			m_colourCube.renderFace( size
				, *texture
				, face
				, pipeline
				, matrixUbo );
		}
		/**
		 *\~english
		 *\brief		Renders the wanted face of given cube texture to the currently draw-bound frame buffer.
		 *\param[in]	size	The render viewport size.
		 *\param[in]	texture	The texture.
		 *\param[in]	face	The face index.
		 *\~french
		 *\brief		Rend la face voulue de la texture cube donnée dans le tampon d'image actuellement activé en dessin.
		 *\param[in]	size	La taille du viewport de rendu.
		 *\param[in]	texture	La texture.
		 *\param[in]	face	L'index de la face.
		 */
		inline void renderCubeFace( castor::Size const & size
			, TextureLayoutSPtr texture
			, CubeMapFace face )
		{
			m_colourCube.renderFace( size
				, *texture
				, face );
		}
		/**
		 *\~english
		 *\brief		Tells the context is initialised
		 *\~french
		 *\brief		Dit si le contexte est initialisé
		 */
		inline bool isInitialised()const
		{
			return m_initialised;
		}
		/**
		 *\~english
		 *\return		The render window.
		 *\~french
		 *\return		La fenêtre de rendu.
		 */
		inline RenderWindow const & getWindow()const
		{
			return *m_window;
		}
		/**
		 *\~english
		 *\return		The render window.
		 *\~french
		 *\return		La fenêtre de rendu.
		 */
		inline RenderWindow & getWindow()
		{
			return *m_window;
		}
		/**
		 *\~english
		 *\return		\p true if initialised, false if not
		 *\~french
		 *\brief		Initialise le contexte
		 *\return		\p true si initialisé correctement
		 */
		C3D_API virtual bool doInitialise() = 0;
		/**
		 *\~english
		 *\brief		Cleans this context up
		 *\~french
		 *\brief		Nettoie le contexte
		 */
		C3D_API virtual void doCleanup() = 0;
		/**
		 *\~english
		 *\brief		Destroys the context on GPU.
		 *\~french
		 *\brief		Détruit le contexte sur le GPU.
		 */
		C3D_API virtual void doDestroy() = 0;
		/**
		 *\~english
		 *\brief		Defines this context to be the current rendering context
		 *\~french
		 *\brief		Définit le contexte pour être celui de rendu actuel
		 */
		C3D_API virtual void doSetCurrent() = 0;
		/**
		 *\~english
		 *\brief		Defines this context not to be the current rendering context
		 *\~french
		 *\brief		Définit ce contexte pour ne pas être l'actuel
		 */
		C3D_API virtual void doEndCurrent() = 0;
		/**
		 *\~english
		 *\brief		Swaps render buffers
		 *\~french
		 *\brief		Echange les buffers de rendu
		 */
		C3D_API virtual void doSwapBuffers() = 0;
		/**
		 *\~english
		 *\brief		Raise a memory barrier.
		 *\param[in]	barriers	The barrier to wait.
		 *\~french
		 *\brief		Met en place un barrière mémoire.
		 *\param[in]	barriers	La barrière à attendre.
		 */
		C3D_API virtual void doMemoryBarrier( MemoryBarriers const & barriers ) = 0;

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
		//!\~english	The pipeline used to render a variance texture in the current draw-bound framebuffer.
		//!\~french		Le pipeline utilisé pour le rendu d'une texture de variance dans le tampon d'image actuellement activé en dessin.
		RenderVarianceToTexture m_variance;
		//!\~english	The pipeline used to render a variance texture in the current draw-bound framebuffer.
		//!\~french		Le pipeline utilisé pour le rendu d'une texture de variance dans le tampon d'image actuellement activé en dessin.
		RenderVarianceCubeToTexture m_varianceCube;
		//!\~english	The pipeline used to render a cube texture in the current draw-bound framebuffer.
		//!\~french		Le pipeline utilisé pour le rendu d'une texture cube dans le tampon d'image actuellement activé en dessin.
		RenderColourToCube m_cube;
	};
}

#endif
