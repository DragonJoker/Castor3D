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

#include "Shader/UniformBuffer.hpp"

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
		 *\brief		Prepares the skybox faces from an equirectangular HDR image.
		 *\param[in]	p_texture	The texture holding the HDR image.
		 *\param[in]	p_size		The skybox faces dimensions.
		 *\param[out]	p_skybox	The skybox to prepare.
		 *\~french
		 *\brief		Prépare les face d'une skybox depuis une image HDR equirectangulaire.
		 *\param[in]	p_texture	La texture contenant l'image HDR.
		 *\param[in]	p_size		Les dimensions des faces de la skybox.
		 *\param[out]	p_skybox	La skybox à préparer.
		 */
		C3D_API void PrepareSkybox( TextureLayout const & p_texture
			, Castor::Size const & p_size
			, Skybox & p_skybox );
		/**
		 *\~english
		 *\brief		Renders the given cube texture to the currently draw-bound frame buffer.
		 *\param[in]	p_size		The render viewport size.
		 *\param[in]	p_texture	The texture.
		 *\~french
		 *\brief		Rend la texture cube donnée dans le tampon d'image actuellement activé en dessin.
		 *\param[in]	p_size		La taille du viewport de rendu.
		 *\param[in]	p_texture	La texture.
		 */
		C3D_API void RenderTextureCube( Castor::Size const & p_size
			, TextureLayout const & p_texture );
		/**
		 *\~english
		 *\brief		Renders the wanted layer of given cube texture array to the currently draw-bound frame buffer.
		 *\param[in]	p_size		The render viewport size.
		 *\param[in]	p_texture	The texture.
		 *\param[in]	p_index		The layer index.
		 *\~french
		 *\brief		Rend la couche voulue du tableau de textures cube donné dans le tampon d'image actuellement activé en dessin.
		 *\param[in]	p_size		La taille du viewport de rendu.
		 *\param[in]	p_texture	La texture.
		 *\param[in]	p_index		L'index de la couche.
		 */
		C3D_API void RenderTextureCube( Castor::Size const & p_size
			, TextureLayout const & p_texture
			, uint32_t p_index );
		/**
		 *\~english
		 *\brief		Renders the given 2D texture.
		 *\param[in]	p_position	The render viewport position.
		 *\param[in]	p_size		The render viewport size.
		 *\param[in]	p_texture	The texture.
		 *\param[in]	p_pipeline	The pipleline used to render the texture.
		 *\param[in]	p_matrixUbo	The uniform buffer receiving matrices.
		 *\~french
		 *\brief		Dessine la texture 2D donnée.
		 *\param[in]	p_position	La position du viewport de rendu.
		 *\param[in]	p_size		La taille du viewport de rendu.
		 *\param[in]	p_texture	La texture.
		 *\param[in]	p_pipeline	Le pipeline utilisé pour dessiner la texture.
		 *\param[in]	p_matrixUbo	Le tampon d'uniformes recevant les matrices.
		 */
		C3D_API void RenderTexture( Castor::Position const & p_position
			, Castor::Size const & p_size
			, TextureLayout const & p_texture
			, RenderPipeline & p_pipeline
			, UniformBuffer & p_matrixUbo );
		/**
		 *\~english
		 *\brief		Renders the given 2D texture to the currently draw-bound frame buffer.
		 *\param[in]	p_position	The render viewport position.
		 *\param[in]	p_size		The render viewport size.
		 *\param[in]	p_texture	The texture.
		 *\~french
		 *\brief		Rend la texture 2D donnée dans le tampon d'image actuellement activé en dessin.
		 *\param[in]	p_position	La position du viewport de rendu.
		 *\param[in]	p_size		La taille du viewport de rendu.
		 *\param[in]	p_texture	La texture.
		 */
		C3D_API void RenderTexture( Castor::Position const & p_position
			, Castor::Size const & p_size
			, TextureLayout const & p_texture );
		/**
		 *\~english
		 *\brief		Renders the wanted layer of given 2D texture array to the currently draw-bound frame buffer.
		 *\param[in]	p_position	The render viewport position.
		 *\param[in]	p_size		The render viewport size.
		 *\param[in]	p_texture	The texture.
		 *\param[in]	p_index		The layer index.
		 *\~french
		 *\brief		Rend la couche voulue du tableau de textures 2D donné dans le tampon d'image actuellement activé en dessin.
		 *\param[in]	p_position	La position du viewport de rendu.
		 *\param[in]	p_size		La taille du viewport de rendu.
		 *\param[in]	p_texture	La texture.
		 *\param[in]	p_index		L'index de la couche.
		 */
		C3D_API void RenderTexture( Castor::Position const & p_position
			, Castor::Size const & p_size
			, TextureLayout const & p_texture
			, uint32_t p_index );
		/**
		 *\~english
		 *\brief		Renders the given depth cube texture to the currently draw-bound frame buffer.
		 *\param[in]	p_size		The render viewport size.
		 *\param[in]	p_texture	The texture.
		 *\~french
		 *\brief		Rend la texture cube de profondeur donnée dans le tampon d'image actuellement activé en dessin.
		 *\param[in]	p_size		La taille du viewport de rendu.
		 *\param[in]	p_texture	La texture.
		 */
		C3D_API void RenderDepthCube( Castor::Position const & p_position
			, Castor::Size const & p_size
			, TextureLayout const & p_texture );
		/**
		 *\~english
		 *\brief		Renders the wanted layer of given depth cube texture array to the currently draw-bound frame buffer.
		 *\param[in]	p_size		The render viewport size.
		 *\param[in]	p_texture	The texture.
		 *\param[in]	p_index		The layer index.
		 *\~french
		 *\brief		Rend la couche voulue du tableau de textures cube de profondeur donné dans le tampon d'image actuellement activé en dessin.
		 *\param[in]	p_size		La taille du viewport de rendu.
		 *\param[in]	p_texture	La texture.
		 *\param[in]	p_index		L'index de la couche.
		 */
		C3D_API void RenderDepthCube( Castor::Position const & p_position
			, Castor::Size const & p_size
			, TextureLayout const & p_texture
			, uint32_t p_index );
		/**
		 *\~english
		 *\brief		Renders the given depth 2D texture to the currently draw-bound frame buffer.
		 *\param[in]	p_position	The render viewport position.
		 *\param[in]	p_size		The render viewport size.
		 *\param[in]	p_texture	The texture.
		 *\~french
		 *\brief		Rend la texture 2D de profondeur donnée dans le tampon d'image actuellement activé en dessin.
		 *\param[in]	p_position	La position du viewport de rendu.
		 *\param[in]	p_size		La taille du viewport de rendu.
		 *\param[in]	p_texture	La texture.
		 */
		C3D_API void RenderDepth( Castor::Position const & p_position
			, Castor::Size const & p_size
			, TextureLayout const & p_texture );
		/**
		 *\~english
		 *\brief		Renders the wanted layer of given depth 2D texture array to the currently draw-bound frame buffer.
		 *\param[in]	p_position	The render viewport position.
		 *\param[in]	p_size		The render viewport size.
		 *\param[in]	p_texture	The texture.
		 *\param[in]	p_index		The layer index.
		 *\~french
		 *\brief		Rend la couche voulue du tableau de textures 2D de profondeur donné dans le tampon d'image actuellement activé en dessin.
		 *\param[in]	p_position	La position du viewport de rendu.
		 *\param[in]	p_size		La taille du viewport de rendu.
		 *\param[in]	p_texture	La texture.
		 *\param[in]	p_index		L'index de la couche.
		 */
		C3D_API void RenderDepth( Castor::Position const & p_position
			, Castor::Size const & p_size
			, TextureLayout const & p_texture
			, uint32_t p_index );
		/**
		 *\~english
		 *\brief		Renders the given 2D texture.
		 *\param[in]	p_size		The render viewport size.
		 *\param[in]	p_texture	The texture.
		 *\param[in]	p_pipeline	The pipleline used to render the texture.
		 *\param[in]	p_matrixUbo	The uniform buffer receiving matrices.
		 *\~french
		 *\brief		Dessine la texture 2D donnée.
		 *\param[in]	p_size		La taille du viewport de rendu.
		 *\param[in]	p_texture	La texture.
		 *\param[in]	p_pipeline	Le pipeline utilisé pour dessiner la texture.
		 *\param[in]	p_matrixUbo	Le tampon d'uniformes recevant les matrices.
		 */
		inline void RenderTexture( Castor::Size const & p_size
			, TextureLayout const & p_texture
			, RenderPipeline & p_pipeline
			, UniformBuffer & p_matrixUbo )
		{
			static Castor::Position const l_position;
			RenderTexture( l_position
				, p_size
				, p_texture
				, p_pipeline
				, p_matrixUbo );
		}
		/**
		 *\~english
		 *\brief		Renders the given 2D texture to the currently draw-bound frame buffer.
		 *\param[in]	p_size		The render viewport size.
		 *\param[in]	p_texture	The texture.
		 *\~french
		 *\brief		Rend la texture 2D donnée dans le tampon d'image actuellement activé en dessin.
		 *\param[in]	p_size		La taille du viewport de rendu.
		 *\param[in]	p_texture	La texture.
		 */
		inline void RenderTexture( Castor::Size const & p_size
			, TextureLayout const & p_texture )
		{
			static Castor::Position const l_position;
			RenderTexture( l_position
				, p_size
				, p_texture );
		}
		/**
		 *\~english
		 *\brief		Renders the wanted layer of given 2D texture array to the currently draw-bound frame buffer.
		 *\param[in]	p_size		The render viewport size.
		 *\param[in]	p_texture	The texture.
		 *\param[in]	p_index		The layer index.
		 *\~french
		 *\brief		Rend la couche voulue du tableau de textures 2D donné dans le tampon d'image actuellement activé en dessin.
		 *\param[in]	p_size		La taille du viewport de rendu.
		 *\param[in]	p_texture	La texture.
		 *\param[in]	p_index		L'index de la couche.
		 */
		inline void RenderTexture( Castor::Size const & p_size
			, TextureLayout const & p_texture
			, uint32_t p_index )
		{
			static Castor::Position const l_position;
			RenderTexture( l_position
				, p_size
				, p_texture
				, p_index );
		}
		/**
		 *\~english
		 *\brief		Renders the given depth 2D texture to the currently draw-bound frame buffer.
		 *\param[in]	p_size		The render viewport size.
		 *\param[in]	p_texture	The texture.
		 *\~french
		 *\brief		Rend la texture 2D de profondeur donnée dans le tampon d'image actuellement activé en dessin.
		 *\param[in]	p_size		La taille du viewport de rendu.
		 *\param[in]	p_texture	La texture.
		 */
		inline void RenderDepth( Castor::Size const & p_size
			, TextureLayout const & p_texture )
		{
			static Castor::Position const l_position;
			RenderDepth( l_position
				, p_size
				, p_texture );
		}
		/**
		 *\~english
		 *\brief		Renders the wanted layer of given depth 2D texture array to the currently draw-bound frame buffer.
		 *\param[in]	p_size		The render viewport size.
		 *\param[in]	p_texture	The texture.
		 *\param[in]	p_index		The layer index.
		 *\~french
		 *\brief		Rend la couche voulue du tableau de textures 2D de profondeur donné dans le tampon d'image actuellement activé en dessin.
		 *\param[in]	p_size		La taille du viewport de rendu.
		 *\param[in]	p_texture	La texture.
		 *\param[in]	p_index		L'index de la couche.
		 */
		inline void RenderDepth( Castor::Size const & p_size
			, TextureLayout const & p_texture
			, uint32_t p_index )
		{
			static Castor::Position const l_position;
			RenderDepth( l_position
				, p_size
				, p_texture
				, p_index );
		}
		/**
		 *\~english
		 *\brief		Renders the given depth cube texture to the currently draw-bound frame buffer.
		 *\param[in]	p_size		The render viewport size.
		 *\param[in]	p_texture	The texture.
		 *\~french
		 *\brief		Rend la texture cube de profondeur donnée dans le tampon d'image actuellement activé en dessin.
		 *\param[in]	p_size		La taille du viewport de rendu.
		 *\param[in]	p_texture	La texture.
		 */
		inline void RenderDepthCube( Castor::Size const & p_size
			, TextureLayout const & p_texture )
		{
			static Castor::Position const l_position;
			RenderDepth( l_position
				, p_size
				, p_texture );
		}
		/**
		 *\~english
		 *\brief		Renders the wanted layer of given depth cube texture array to the currently draw-bound frame buffer.
		 *\param[in]	p_size		The render viewport size.
		 *\param[in]	p_texture	The texture.
		 *\param[in]	p_index		The layer index.
		 *\~french
		 *\brief		Rend la couche voulue du tableau de textures cube de profondeur donné dans le tampon d'image actuellement activé en dessin.
		 *\param[in]	p_size		La taille du viewport de rendu.
		 *\param[in]	p_texture	La texture.
		 *\param[in]	p_index		L'index de la couche.
		 */
		inline void RenderDepthCube( Castor::Size const & p_size
			, TextureLayout const & p_texture
			, uint32_t p_index )
		{
			static Castor::Position const l_position;
			RenderDepth( l_position
				, p_size
				, p_texture
				, p_index );
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
		 *\brief		Renders the wanted face of given cube texture.
		 *\param[in]	p_position			The render viewport position.
		 *\param[in]	p_size				The render viewport size.
		 *\param[in]	p_texture			The texture.
		 *\param[in]	p_face				The cube face.
		 *\param[in]	p_pipeline			The render pipeline.
		 *\param[in]	p_matrixUbo			The uniform buffer receiving matrices.
		 *\param[in]	p_geometryBuffers	The geometry buffers used to render the texture.
		 *\~french
		 *\brief		Dessine la face voulue de la texture cube donnée.
		 *\param[in]	p_position			La position du viewport de rendu.
		 *\param[in]	p_size				La taille du viewport de rendu.
		 *\param[in]	p_texture			La texture.
		 *\param[in]	p_face				La face du cube.
		 *\param[in]	p_pipeline			Le pipeline de rendu.
		 *\param[in]	p_matrixUbo			Le tampon d'uniformes recevant les matrices.
		 *\param[in]	p_geometryBuffers	Les tampons de géométrie utilisés pour dessiner la texture.
		 */
		C3D_API void DoRenderTextureFace( Castor::Position const & p_position
			, Castor::Size const & p_size
			, TextureLayout const & p_texture
			, CubeMapFace p_face
			, RenderPipeline & p_pipeline
			, UniformBuffer & p_matrixUbo
			, GeometryBuffers const & p_geometryBuffers );
		/**
		 *\~english
		 *\brief		Renders the wanted face of wanted layer of given cube texture array.
		 *\param[in]	p_position			The render viewport position.
		 *\param[in]	p_size				The render viewport size.
		 *\param[in]	p_texture			The texture.
		 *\param[in]	p_face				The cube face.
		 *\param[in]	p_pipeline			The render pipeline.
		 *\param[in]	p_matrixUbo			The uniform buffer receiving matrices.
		 *\param[in]	p_geometryBuffers	The geometry buffers used to render the texture.
		 *\param[in]	p_index				The layer index.
		 *\~french
		 *\brief		Dessine la face voulue de la couche voulue du tableau de textures cube donné.
		 *\param[in]	p_position			La position du viewport de rendu.
		 *\param[in]	p_size				La taille du viewport de rendu.
		 *\param[in]	p_texture			La texture.
		 *\param[in]	p_face				La face du cube.
		 *\param[in]	p_pipeline			Le pipeline de rendu.
		 *\param[in]	p_matrixUbo			Le tampon d'uniformes recevant les matrices.
		 *\param[in]	p_geometryBuffers	Les tampons de géométrie utilisés pour dessiner la texture.
		 *\param[in]	p_index				L'index de la couche.
		 */
		C3D_API void DoRenderTextureFace( Castor::Position const & p_position
			, Castor::Size const & p_size
			, TextureLayout const & p_texture
			, CubeMapFace p_face
			, RenderPipeline & p_pipeline
			, UniformBuffer & p_matrixUbo
			, GeometryBuffers const & p_geometryBuffers
			, uint32_t p_index );
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
		//!\~english	The GPU time elapsed queries.
		//!\~french		Les requêtes GPU de temps écoulé.
		std::array< GpuQueryUPtr, 2 > m_timerQuery;
		//!\~english	The active query index.
		//!\~french		L'index de la requête active.
		uint32_t m_queryIndex = 0;
		//!\~english	The uniform buffer containing matrices data.
		//!\~french		Le tampon d'uniformes contenant les données de matrices.
		UniformBuffer m_matrixUbo;
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
