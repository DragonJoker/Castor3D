﻿/*
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
#include "BufferDeclaration.hpp"

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
		C3D_API void RenderTexture( Castor::Size const & p_size, Texture const & p_texture );
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
		C3D_API void RenderTexture( Castor::Size const & p_size, Texture const & p_texture, ShaderProgramSPtr p_program );
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
		 *\brief		Retrieve the DepthStencilState with no depth write and test.
		 *\~french
		 *\brief		Récupère le DepthStencilState sans test de profondeur ni écriture dans le tampon de profondeur.
		 */
		inline DepthStencilStateSPtr GetNoDepthState()const
		{
			return m_pDsStateNoDepth;
		}
		/**
		 *\~english
		 *\brief		Retrieve the DepthStencilState without depth write.
		 *\~french
		 *\brief		Récupère le DepthStencilState sans écriture dans le tampon de profondeur.
		 */
		inline DepthStencilStateSPtr GetNoDepthWriteState()const
		{
			return m_pDsStateNoDepthWrite;
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
		/**
		 *\~english
		 *\brief		Renders the given texture.
		 *\param[in]	p_size				The render viewport size.
		 *\param[in]	p_texture			The texture.
		 *\param[in]	p_geometryBuffers	The geometry buffers used to render the texture.
		 *\param[in]	p_program			The program used to render the texture.
		 *\~french
		 *\brief		Dessine la texture donnée.
		 *\param[in]	p_size				La taille du viewport de rendu.
		 *\param[in]	p_texture			La texture.
		 *\param[in]	p_geometryBuffers	Les tampons de géométrie utilisés pour dessiner la texture.
		 *\param[in]	p_program			Le programme utilisé pour dessiner la texture.
		 */
		C3D_API void DoRenderTexture( Castor::Size const & p_size, Texture const & p_texture, GeometryBuffersSPtr p_geometryBuffers, ShaderProgramSPtr p_program );

	protected:
		//!\~english RenderWindow associated to this context	\~french RenderWindow associée à ce contexte
		RenderWindow * m_window;
		//!\~english Tells if the context is initialised	\~french Dit si le contexte est initialisé
		bool m_initialised;
		//!\~english Tells the context is currently set to use multisampling	\~french Dit si le contexte est actuellement configuré pour utiliser le multisampling
		bool m_bMultiSampling;
		//!\~english The ShaderProgram used when rendering from a buffer to another one	\~french Le ShaderProgram utilisé lors du rendu d'un tampon vers un autre
		ShaderProgramWPtr m_renderTextureProgram;
		//!\~english The diffuse map frame variable, in the buffer-to-buffer shader program	\~french La frame variable de l'image diffuse, dans le shader buffer-to-buffer
		OneIntFrameVariableSPtr m_mapDiffuse;
		//!\~english The GeometryBuffers used when rendering a texture to the current frame buffer.	\~french Le GeometryBuffers utilisé lors du dessin d'une texture dans le tampon d'image courant.
		GeometryBuffersSPtr m_geometryBuffers;
		//!\~english The Viewport used when rendering a texture into to a frame buffer.	\~french Le Viewport utilisé lors du dessin d'une texture dans un tampon d'image.
		Viewport m_viewport;
		//!\~english Buffer elements declaration	\~french Déclaration des éléments d'un vertex
		Castor3D::BufferDeclaration m_declaration;
		//!\~english Vertex array (quad definition)	\~french Tableau de vertex (définition du quad)
		std::array< Castor3D::BufferElementGroupSPtr, 6 > m_arrayVertex;
		//!	6 * [2(vertex position) 2(texture coordinates)]
		Castor::real m_pBuffer[24];
		//!\~english The DepthStencilState without depth write and test.	\~french Le DepthStencilState sans test ni écriture de profondeur.
		DepthStencilStateSPtr m_pDsStateNoDepth;
		//!\~english The DepthStencilState without depth write.	\~french Le DepthStencilState sans écriture de profondeur.
		DepthStencilStateSPtr m_pDsStateNoDepthWrite;
		//!\~english The vertex buffer.	\~french Le tampon de sommets.
		VertexBufferUPtr m_vertexBuffer;
		//!\~english The GPU time elapsed queries.	\~french Les requêtes GPU de temps écoulé.
		std::array< GpuQuerySPtr, 2 > m_timerQuery;
		//!\~english The active query index.	\~french L'index de la requête active.
		uint32_t m_queryIndex = 0;
	};
}

#endif
