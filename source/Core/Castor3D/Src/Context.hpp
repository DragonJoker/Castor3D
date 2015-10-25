/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_CONTEXT_H___
#define ___C3D_CONTEXT_H___

#include "Castor3DPrerequisites.hpp"
#include "Viewport.hpp"

#include <Colour.hpp>
#include <OwnedBy.hpp>

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
		 *\param[in]	p_invertFinal	Tells if the final render is to be inverted.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_renderSystem	Le RenderSystem.
		 *\param[in]	p_invertFinal	Dit si on inverse l'image du rendu final.
		 */
		C3D_API Context( RenderSystem & p_renderSystem, bool p_invertFinal );
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
		 *\brief		Defines the culling option for current render
		 *\param[in]	p_eCullFace	The culling option
		 *\~french
		 *\brief		Définit l'option de culling pour le rendu courant
		 *\param[in]	p_eCullFace	L'option de culling
		 */
		C3D_API void CullFace( eFACE p_eCullFace );
		/**
		 *\~english
		 *\brief		Swaps render buffers
		 *\~french
		 *\brief		Echange les buffers de rendu
		 */
		C3D_API void SwapBuffers();
		/**
		 *\~english
		 *\brief		Defines the alpha text function
		 *\param[in]	p_eFunc		The function
		 *\param[in]	p_byValue	The comparison value
		 *\~french
		 *\brief		Définit la fonction de test alpha
		 *\param[in]	p_eFunc		La fonction
		 *\param[in]	p_byValue	La valeur de comparaison
		 */
		C3D_API void SetAlphaFunc( eALPHA_FUNC p_eFunc, uint8_t p_byValue );
		/**
		 *\~english
		 *\brief		Renders the given texture to the currently draw-bound frame buffer.
		 *\param[in]	p_size			The render viewport size.
		 *\param[in]	p_pTexture		The texture.
		 *\~french
		 *\brief		Rend la texture donnée dans le tampon d'image actuellement activé en dessin.
		 *\param[in]	p_size			La taille du viewport de rendu.
		 *\param[in]	p_pTexture		La texture.
		 */
		C3D_API void RenderTextureToCurrentBuffer( Castor::Size const & p_size, TextureBaseSPtr p_pTexture );
		/**
		 *\~english
		 *\brief		Renders the given texture to the back buffers.
		 *\param[in]	p_size			The render viewport size.
		 *\param[in]	p_pTexture		The texture.
		 *\~french
		 *\brief		Rend la texture donnée dans le tampon e la fenêtre.
		 *\param[in]	p_size			La taille du viewport de rendu.
		 *\param[in]	p_pTexture		La texture.
		 */
		C3D_API void RenderTextureToBackBuffer( Castor::Size const & p_size, TextureBaseSPtr p_pTexture );
		/**
		 *\~english
		 *\brief		Changes fullscreen status
		 *\param[in]	val	The new fullscreen status
		 *\~french
		 *\brief		Change le statut de plein écran
		 *\param[in]	val	Le nouveau statut de plein écran
		 */
		C3D_API virtual void UpdateFullScreen( bool val ) = 0;
		/**
		 *\~english
		 *\brief		Retrieves the maximal supported size, given a wanted size
		 *\param[in]	p_size	The wanted size
		 *\return		The maximal supported size less than or equal to p_size
		 *\~french
		 *\brief		Récupère la taille maximale supportée, en fonction d'une taille données
		 *\param[in]	p_size	La taille voulue
		 *\return		La taille maximale supportée inférieure ou égale à p_size
		 */
		C3D_API virtual Castor::Size GetMaxSize( Castor::Size const & p_size ) = 0;
		/**
		 *\~english
		 *\brief		Tells the context is initialised
		 *\~french
		 *\brief		Dit si le contexte est initialisé
		 */
		inline bool IsInitialised()const
		{
			return m_bInitialised;
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
		 *\param[in]	p_bVal	The status
		 *\~french
		 *\brief		Définit le statut du multisampling
		 *\param[in]	p_bVal	Le statut
		 */
		inline void SetMultiSampling( bool p_bVal )
		{
			m_bMultiSampling = p_bVal;
		}
		/**
		 *\~english
		 *\brief		Retrieve the background DepthStencilState
		 *\~french
		 *\brief		Récupère le DepthStencilState de fond d'écran
		 */
		inline DepthStencilStateSPtr GetBackgroundDSState()const
		{
			return m_pDsStateBackground;
		}

	protected:
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
		 *\brief		Defines this context to be the current rendering context
		 *\param[in]	p_window	The RenderWindow
		 *\~french
		 *\brief		Définit le contexte pour être celui de rendu actuel
		 *\param[in]	p_window	La RenderWindow
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
		 *\brief		Defines the alpha text function
		 *\param[in]	p_eFunc		The function
		 *\param[in]	p_byValue	The comparison value
		 *\~french
		 *\brief		Définit la fonction de test alpha
		 *\param[in]	p_eFunc		La fonction
		 *\param[in]	p_byValue	La valeur de comparaison
		 */
		C3D_API virtual void DoSetAlphaFunc( eALPHA_FUNC p_eFunc, uint8_t p_byValue ) = 0;
		/**
		 *\~english
		 *\brief		Defines the culling option for current render
		 *\param[in]	p_eCullFace	The culling option
		 *\~french
		 *\brief		Définit l'option de culling pour le rendu courant
		 *\param[in]	p_eCullFace	L'option de culling
		 */
		C3D_API virtual void DoCullFace( eFACE p_eCullFace ) = 0;

	private:
		/**
		 *\~english
		 *\brief		Renders the given texture.
		 *\param[in]	p_size				The render viewport size.
		 *\param[in]	p_texture			The texture.
		 *\param[in]	p_geometryBuffers	The geometry buffers used to render the texture.
		 *\~french
		 *\brief		Dessine la texture donnée.
		 *\param[in]	p_size				La taille du viewport de rendu.
		 *\param[in]	p_texture			La texture.
		 *\param[in]	p_geometryBuffers	Les tampons de géométrie utilisés pour dessiner la texture.
		 */
		void DoRenderTexture( Castor::Size const & p_size, TextureBaseSPtr p_texture, GeometryBuffersSPtr p_geometryBuffers );

	protected:
		//!\~english RenderWindow associated to this context	\~french RenderWindow associée à ce contexte
		RenderWindow * m_pWindow;
		//!\~english Tells if the context is initialised	\~french Dit si le contexte est initialisé
		bool m_bInitialised;
		//!\~english Tells the context is currently set to use multisampling	\~french Dit si le contexte est actuellement configuré pour utiliser le multisampling
		bool m_bMultiSampling;
		//!\~english The ShaderProgram used when rendering from a buffer to another one	\~french Le ShaderProgram utilisé lors du rendu d'un tampon vers un autre
		ShaderProgramBaseWPtr m_pBtoBShaderProgram;
		//!\~english The diffuse map frame variable, in the buffer-to-buffer shader program	\french La frame variable de l'image diffuse, dans le shader buffer-to-buffer
		OneTextureFrameVariableSPtr m_mapDiffuse;
		//!\~english The GeometryBuffers used when rendering a texture to the current frame buffer.	\~french Le GeometryBuffers utilisé lors du dessin d'une texture dans le tampon d'image courant.
		GeometryBuffersSPtr m_pGeometryBuffers;
		//!\~english The GeometryBuffers used when rendering a texture to the back buffer.	\~french Le GeometryBuffers utilisé lors du dessin d'une texture dans le tampon de fenêtre.
		GeometryBuffersSPtr m_finalGeometryBuffers;
		//!\~english The Viewport used when rendering a texture into to a frame buffer.	\~french Le Viewport utilisé lors du dessin d'une texture dans un tampon d'image.
		Viewport m_viewport;
		//!\~english Buffer elements declaration	\~french Déclaration des éléments d'un vertex
		Castor3D::BufferDeclarationSPtr m_pDeclaration;
		//!\~english Vertex array (quad definition)	\~french Tableau de vertex (définition du quad)
		std::array< Castor3D::BufferElementGroupSPtr, 6 > m_arrayVertex;
		//!\~english Vertex array (quad definition)	\~french Tableau de vertex (définition du quad)
		std::array< Castor3D::BufferElementGroupSPtr, 6 > m_finalVertex;
		//!	6 * [2(vertex position) 2(texture coordinates)]
		Castor::real m_pBuffer[24];
		//!	6 * [2(vertex position) 2(texture coordinates)]
		Castor::real m_finalBuffer[24];
		//!\~english DepthStencilState used while rendering background image	\~french DepthStencilState utilisé pour le rendu de l'image de fond
		DepthStencilStateSPtr m_pDsStateBackground;
	};
}

#endif
