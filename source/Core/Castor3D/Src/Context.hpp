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

#include <Colour.hpp>

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

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
	class C3D_API Context
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		Context();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~Context();
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
		bool Initialise( RenderWindow * p_window );
		/**
		 *\~english
		 *\brief		Cleans this context up
		 *\~french
		 *\brief		Nettoie le contexte
		 */
		void Cleanup();
		/**
		 *\~english
		 *\brief		Defines this context to be the current rendering context
		 *\~french
		 *\brief		Définit le contexte pour être celui de rendu actuel
		 */
		void SetCurrent();
		/**
		 *\~english
		 *\brief		Defines this context not to be the current rendering context
		 *\~french
		 *\brief		Définit ce contexte pour ne pas être l'actuel
		 */
		void EndCurrent();
		/**
		 *\~english
		 *\brief		Defines the culling option for current render
		 *\param[in]	p_eCullFace	The culling option
		 *\~french
		 *\brief		Définit l'option de culling pour le rendu courant
		 *\param[in]	p_eCullFace	L'option de culling
		 */
		void CullFace( eFACE p_eCullFace );
		/**
		 *\~english
		 *\brief		Swaps render buffers
		 *\~french
		 *\brief		Echange les buffers de rendu
		 */
		void SwapBuffers();
		/**
		 *\~english
		 *\brief		Defines the colour used when Context::Clear is called on the color buffer
		 *\param[in]	p_clrClear	The colour
		 *\~french
		 *\brief		Définit la couleur utilisée quand Context::Clear est appelée sur le tampon couleur
		 *\param[in]	p_clrClear	La couleur
		 */
		void SetClearColour( Castor::Colour const & p_clrClear );
		/**
		 *\~english
		 *\brief		Clears the wanted buffers
		 *\param[in]	p_uiTargets	The buffers
		 *\~french
		 *\brief		Vide le(s) tampon(s) voulu(s)
		 *\param[in]	p_uiTargets	Le(s) tampon(s)
		 */
		void Clear( uint32_t p_uiTargets );
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
		 *\brief		Tells the context is using deferred shading
		 *\~french
		 *\brief		Dit si le contexte utilise le deferred shading
		 */
		inline bool IsDeferredShadingSet()const
		{
			return m_bDeferredShadingSet;
		}
		/**
		 *\~english
		 *\brief		Defines the deferred shading status
		 *\param[in]	p_bVal	The status
		 *\~french
		 *\brief		Définit le statut du deferred shading
		 *\param[in]	p_bVal	Le statut
		 */
		inline void SetDeferredShading( bool p_bVal )
		{
			m_bDeferredShadingSet = p_bVal;
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
		/**
		 *\~english
		 *\brief		Retrieves the background colour
		 *\~french
		 *\brief		Récupère la couleur de fond
		 */
		inline Castor::Colour GetClearColour()const
		{
			return m_clearColour;
		}
		/**
		 *\~english
		 *\brief		Binds given system frame buffer to given mode
		 *\param[in]	p_eBuffer	The buffer
		 *\param[in]	p_eTarget	The target
		 *\~french
		 *\brief		Associe le tampon d'image système donné dans le mode donné
		 *\param[in]	p_eBuffer	Le tampon
		 *\param[in]	p_eTarget	La cible
		 */
		void Bind( eBUFFER p_eBuffer, eFRAMEBUFFER_TARGET p_eTarget );
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
		void SetAlphaFunc( eALPHA_FUNC p_eFunc, uint8_t p_byValue );
		/**
		 *\~english
		 *\brief		Changes fullscreen status
		 *\param[in]	val	The new fullscreen status
		 *\~french
		 *\brief		Change le statut de plein écran
		 *\param[in]	val	Le nouveau statut de plein écran
		 */
		virtual void UpdateFullScreen( bool val ) = 0;
		/**
		 *\~english
		 *\brief		Renders the given texture to the currently draw-bound frame buffer
		 *\param[in]	p_size			The render viewport size
		 *\param[in]	p_pTexture		The texture
		 *\param[in]	p_uiComponents	The render target components (bitwise combination of eBUFFER_COMPONENT)
		 *\~french
		 *\brief		Rend la texture donnée dans le tampon d'image actuellement activé en dessin
		 *\param[in]	p_size			La taille du viewport de rendu
		 *\param[in]	p_pTexture		La texture
		 *\param[in]	p_uiComponents	Les composantes cibles du rendu (combinaison binaire de eBUFFER_COMPONENT)
		 */
		void BToBRender( Castor::Size const & p_size, TextureBaseSPtr p_pTexture, uint32_t p_uiComponents );
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
		virtual Castor::Size GetMaxSize( Castor::Size const & p_size ) = 0;

	protected:
		/**
		 *\~english
		 *\brief		Initialises this context
		 *\return		\p true if initialised, false if not
		 *\~french
		 *\brief		Initialise le contexte
		 *\return		\p true si initialisé correctement
		 */
		virtual bool DoInitialise() = 0;
		/**
		 *\~english
		 *\brief		Cleans this context up
		 *\~french
		 *\brief		Nettoie le contexte
		 */
		virtual void DoCleanup() = 0;
		/**
		 *\~english
		 *\brief		Defines this context to be the current rendering context
		 *\param[in]	p_window	The RenderWindow
		 *\~french
		 *\brief		Définit le contexte pour être celui de rendu actuel
		 *\param[in]	p_window	La RenderWindow
		 */
		virtual void DoSetCurrent() = 0;
		/**
		 *\~english
		 *\brief		Defines this context not to be the current rendering context
		 *\~french
		 *\brief		Définit ce contexte pour ne pas être l'actuel
		 */
		virtual void DoEndCurrent() = 0;
		/**
		 *\~english
		 *\brief		Swaps render buffers
		 *\~french
		 *\brief		Echange les buffers de rendu
		 */
		virtual void DoSwapBuffers() = 0;
		/**
		 *\~english
		 *\brief		Defines the colour used when Context::Clear is called on the color buffer
		 *\param[in]	p_clrClear	The colour
		 *\~french
		 *\brief		Définit la couleur utilisée quand Context::Clear est appelée sur le tampon couleur
		 *\param[in]	p_clrClear	La couleur
		 */
		virtual void DoSetClearColour( Castor::Colour const & p_clrClear ) = 0;
		/**
		 *\~english
		 *\brief		Clears the wanted buffers
		 *\param[in]	p_uiTargets	The buffers
		 *\~french
		 *\brief		Vide le(s) buffer(s) voulu(s)
		 *\param[in]	p_uiTargets	Le(s) tampon(s)
		 */
		virtual void DoClear( uint32_t p_uiTargets ) = 0;
		/**
		 *\~english
		 *\brief		Binds given system frame buffer to given mode
		 *\param[in]	p_eBuffer	The buffer
		 *\param[in]	p_eTarget	The target
		 *\~french
		 *\brief		Associe le tampon d'image système donné dans le mode donné
		 *\param[in]	p_eBuffer	Le tampon
		 *\param[in]	p_eTarget	La cible
		 */
		virtual void DoBind( eBUFFER p_eBuffer, eFRAMEBUFFER_TARGET p_eTarget ) = 0;
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
		virtual void DoSetAlphaFunc( eALPHA_FUNC p_eFunc, uint8_t p_byValue ) = 0;
		/**
		 *\~english
		 *\brief		Defines the culling option for current render
		 *\param[in]	p_eCullFace	The culling option
		 *\~french
		 *\brief		Définit l'option de culling pour le rendu courant
		 *\param[in]	p_eCullFace	L'option de culling
		 */
		virtual void DoCullFace( eFACE p_eCullFace ) = 0;

	protected:
		//!\~english RenderWindow associated to this context	\~french RenderWindow associée à ce contexte
		RenderWindow * m_pWindow;
		//!\~english The render system	\~french Le render system
		RenderSystem * m_pRenderSystem;
		//!\~english Tells if the context is initialised	\~french Dit si le contexte est initialisé
		bool m_bInitialised;
		//!\~english Tells the context is currently set to use deferred shading	\~french Dit si le contexte est actuellement configuré pour utiliser le deferred shading
		bool m_bDeferredShadingSet;
		//!\~english Tells the context is currently set to use multisampling	\~french Dit si le contexte est actuellement configuré pour utiliser le multisampling
		bool m_bMultiSampling;
		//!\~english The ShaderProgram used when rendering from a buffer to another one	\~french Le ShaderProgram utilisé lors du rendu d'un tampon vers un autre
		ShaderProgramBaseWPtr m_pBtoBShaderProgram;
		//!\~english The diffuse map frame variable, in the buffer-to-buffer shader program	\french La frame variable de l'image diffuse, dans le shader buffer-to-buffer
		OneTextureFrameVariableSPtr m_mapDiffuse;
		//!\~english The GeometryBuffers used when rendering from a buffer to another one	\~french Le GeometryBuffers utilisé lors du rendu d'un tampon vers un autre
		GeometryBuffersSPtr m_pGeometryBuffers;
		//!\~english The Viewport used when rendering from a buffer to another one	\~french Le Viewport utilisé lors du rendu d'un tampon vers un autre
		ViewportSPtr m_pViewport;
		//!\~english Buffer elements declaration	\~french Déclaration des éléments d'un vertex
		Castor3D::BufferDeclarationSPtr m_pDeclaration;
		//!\~english Vertex array (quad definition)	\~french Tableau de vertex (définition du quad)
		std::array< Castor3D::BufferElementGroupSPtr, 6 > m_arrayVertex;
		//!	6 * [2(vertex position) 2(texture coordinates)]
		Castor::real m_pBuffer[24];
		//!\~english DepthStencilState used while rendering background image	\~french DepthStencilState utilisé pour le rendu de l'image de fond
		DepthStencilStateSPtr m_pDsStateBackground;
		//!\~english The background colour	\french La couleur de fond
		Castor::Colour m_clearColour;
	};
}

#pragma warning( pop )

#endif
