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
#ifndef ___C3D_OVERLAY_RENDERER_H___
#define ___C3D_OVERLAY_RENDERER_H___

#include "Castor3DPrerequisites.hpp"
#include "OverlayCategory.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

#ifdef DrawText
#	undef DrawText
#endif

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		09/02/2010
	\~english
	\brief		The overlay renderer
	\~french
	\brief		Le renderer d'incrustation
	*/
	class C3D_API OverlayRenderer
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		OverlayRenderer( RenderSystem * p_pRenderSystem );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~OverlayRenderer();
		/**
		 *\~english
		 *\brief		Initialises the buffers
		 *\~french
		 *\brief		Initialise les tampons
		 */
		void Initialise();
		/**
		 *\~english
		 *\brief		Flushes the renderer
		 *\~french
		 *\brief		Nettoie le renderer
		 */
		void Cleanup();
		/**
		 *\~english
		 *\brief		Function to draw a PanelOverlay
		 *\~french
		 *\brief		Fonction de dessin d'un PanelOverlay
		 */
		void DrawPanel( PanelOverlay & p_pPanelOverlay );
		/**
		 *\~english
		 *\brief		Function to draw a BorderPanelOverlay
		 *\~french
		 *\brief		Fonction de dessin d'un BorderPanelOverlay
		 */
		void DrawBorderPanel( BorderPanelOverlay & p_pBorderPanelOverlay );
		/**
		 *\~english
		 *\brief		Function to draw a TextOverlay
		 *\~french
		 *\brief		Fonction de dessin d'un TextOverlay
		 */
		void DrawText( TextOverlay & p_pTextPanelOverlay );
		/**
		 *\~english
		 *\brief		Begins the overlays rendering
		 *\param[in]	p_size	The render window size
		 *\~french
		 *\brief		Commence le rendu des incrustations
		 *\param[in]	p_size	La taille de la fenêtre de rendu
		 */
		void BeginRender( Castor::Size const & p_size );
		/**
		 *\~english
		 *\brief		Ends the overlays rendering
		 *\~french
		 *\brief		Termine le rendu des incrustations
		 */
		void EndRender();
		/**
		 *\~english
		 *\brief		Retrieves the current render target size
		 *\~french
		 *\brief		Récupère les dimensions de la cible du rendu courant
		 */
		Castor::Size const & GetSize()const
		{
			return m_size;
		}

	protected:
		/**
		 *\~english
		 *\brief		Retrieves a program compiled using given flags
		 *\param[in]	p_uiFlags	Bitwise ORed eTEXTURE_CHANNEL
		 *\return		The program
		 *\~french
		 *\brief		Récupère un programme compilé en utilisant les indicateurs donnés
		 *\param[in]	p_uiFlags	Combinaison de eTEXTURE_CHANNEL
		 *\return		Le programme
		 */
		virtual ShaderProgramBaseSPtr DoGetProgram( uint32_t p_uiFlags ) = 0;
		/**
		 *\~english
		 *\brief		Initialises the buffers
		 *\~french
		 *\brief		Initialise les tampons
		 */
		virtual void DoInitialise() = 0;
		/**
		 *\~english
		 *\brief		Flushes the renderer
		 *\~french
		 *\brief		Nettoie le renderer
		 */
		virtual void DoCleanup() = 0;
		/**
		 *\~english
		 *\brief		Retrieves a panel program compiled using given flags
		 *\param[in]	p_uiFlags	Bitwise ORed eTEXTURE_CHANNEL
		 *\return		The program
		 *\~french
		 *\brief		Récupère un programme de panneau compilé en utilisant les indicateurs donnés
		 *\param[in]	p_uiFlags	Combinaison de eTEXTURE_CHANNEL
		 *\return		Le programme
		 */
		ShaderProgramBaseSPtr GetPanelProgram( uint32_t p_uiFlags );
		/**
		 *\~english
		 *\brief		Retrieves a text program compiled using given flags
		 *\param[in]	p_uiFlags	Bitwise ORed eTEXTURE_CHANNEL
		 *\return		The program
		 *\~french
		 *\brief		Récupère un programme de texte compilé en utilisant les indicateurs donnés
		 *\param[in]	p_uiFlags	Combinaison de eTEXTURE_CHANNEL
		 *\return		Le programme
		 */
		ShaderProgramBaseSPtr GetTextProgram( uint32_t p_uiFlags );

	protected:
		//!\~english Pointer over geometry vertex buffer	\~french Pointeur sur le tampon de sommets
		VertexBufferWPtr m_pVertexBuffer;
		//!\~english Geometry indices buffer	\~french Le tampon de sommets
		IndexBufferWPtr m_pIndexBuffer;
		//!\~english Geometry buffers for panels	\~french Tampons de géometrie pour les panneaux
		GeometryBuffersSPtr m_pPanelGeometryBuffer;
		//!\~english Geometry buffers for borders	\~french Tampons de géometrie pour les bordures
		GeometryBuffersSPtr m_pBorderGeometryBuffer;
		//!\~english The GeometryBuffers used to render one character	\~french Le GeometryBuffers utilisé pour rendre un caractère
		GeometryBuffersSPtr m_pTextGeometryBuffers;
		//!\~english The buffer elements declaration	\~french La déclaration des éléments du tampon
		BufferDeclarationSPtr m_pDeclaration;
		//!\~english The render system	\~french Le render system
		RenderSystem * m_pRenderSystem;
		//!\~english The current render target size	\~french Les dimensions de la cible du rendu courant
		Castor::Size m_size;
		//!\~english The shader programs used to render a panel (used for borders too)	\~french Les programmes de shader utilisés pour rendre un panneau (utilisé pour les bords aussi)
		std::map< uint32_t, ShaderProgramBaseSPtr > m_mapPanelPrograms;
		//!\~english The shader programs used to render texts	\~french Les programmes de shader utilisés pour rendre les textes
		std::map< uint32_t, ShaderProgramBaseSPtr > m_mapTextPrograms;
		//!\~english Text texture sampler	\~french Echantillonneur de la texture de texte
		OneTextureFrameVariableSPtr m_mapText;
		//!\~english The previously rendered BorderPanelOverlay z-index	\~french Le z-index du précedent BorderPanelOverlay rendu
		int m_previousBorderZIndex;
		//!\~english The previously rendered PanelOverlay z-index	\~french Le z-index du précedent PanelOverlay rendu
		int m_previousPanelZIndex;
		//!\~english The previously rendered TextOverlay z-index	\~french Le z-index du précedent TextOverlay rendu
		int m_previousTextZIndex;
		//!\~english The previously rendered text	\~french Le texte rendu précédemment
		Castor::String m_previousCaption;
		//!\~english Panel overlays vertex array (quad definition)	\~french Tableau de vertex (définition du quad) pour les increustations panneau
		std::array< Castor3D::BufferElementGroupSPtr, 6 > m_panelVertex;
		//!	6 * [3 uint (vertex position) + 2 float (texture coordinates)]
		uint8_t m_panelBuffer[120];
		//!	6 * [3 uint (vertex position) + 2 float (texture coordinates)]
		uint8_t m_borderBuffer[60];
	};
}

#pragma warning( pop )

#endif
