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

#include <OwnedBy.hpp>

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
	class OverlayRenderer
		: public Castor::OwnedBy< RenderSystem >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		C3D_API OverlayRenderer( RenderSystem & p_renderSystem );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~OverlayRenderer();
		/**
		 *\~english
		 *\brief		Initialises the buffers
		 *\~french
		 *\brief		Initialise les tampons
		 */
		C3D_API void Initialise();
		/**
		 *\~english
		 *\brief		Flushes the renderer
		 *\~french
		 *\brief		Nettoie le renderer
		 */
		C3D_API void Cleanup();
		/**
		 *\~english
		 *\brief		Function to draw a PanelOverlay
		 *\~french
		 *\brief		Fonction de dessin d'un PanelOverlay
		 */
		C3D_API void DrawPanel( PanelOverlay & p_pPanelOverlay );
		/**
		 *\~english
		 *\brief		Function to draw a BorderPanelOverlay
		 *\~french
		 *\brief		Fonction de dessin d'un BorderPanelOverlay
		 */
		C3D_API void DrawBorderPanel( BorderPanelOverlay & p_pBorderPanelOverlay );
		/**
		 *\~english
		 *\brief		Function to draw a TextOverlay
		 *\~french
		 *\brief		Fonction de dessin d'un TextOverlay
		 */
		C3D_API void DrawText( TextOverlay & p_pTextPanelOverlay );
		/**
		 *\~english
		 *\brief		Begins the overlays rendering
		 *\param[in]	p_size	The render window size
		 *\~french
		 *\brief		Commence le rendu des incrustations
		 *\param[in]	p_size	La taille de la fenêtre de rendu
		 */
		C3D_API void BeginRender( Castor::Size const & p_size );
		/**
		 *\~english
		 *\brief		Ends the overlays rendering
		 *\~french
		 *\brief		Termine le rendu des incrustations
		 */
		C3D_API void EndRender();
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
		/**
		 *\~english
		 *\return		true if the render size has changed.
		 *\~french
		 *\return		true si la taille de rendu a changé.
		 */
		bool IsSizeChanged()const
		{
			return m_sizeChanged;
		}

	protected:
		/**
		 *\~english
		 *\brief		Retrieves a program compiled using given flags
		 *\param[in]	p_flags	Bitwise ORed eTEXTURE_CHANNEL
		 *\return		The program
		 *\~french
		 *\brief		Récupère un programme compilé en utilisant les indicateurs donnés
		 *\param[in]	p_flags	Combinaison de eTEXTURE_CHANNEL
		 *\return		Le programme
		 */
		C3D_API virtual ShaderProgramBaseSPtr DoCreateProgram( uint32_t p_flags ) = 0;
		/**
		 *\~english
		 *\brief		Initialises the buffers
		 *\~french
		 *\brief		Initialise les tampons
		 */
		C3D_API virtual void DoInitialise() = 0;
		/**
		 *\~english
		 *\brief		Flushes the renderer
		 *\~french
		 *\brief		Nettoie le renderer
		 */
		C3D_API virtual void DoCleanup() = 0;
		/**
		 *\~english
		 *\brief		Retrieves a panel program compiled using given flags
		 *\param[in]	p_flags	Bitwise ORed eTEXTURE_CHANNEL
		 *\return		The program
		 *\~french
		 *\brief		Récupère un programme de panneau compilé en utilisant les indicateurs donnés
		 *\param[in]	p_flags	Combinaison de eTEXTURE_CHANNEL
		 *\return		Le programme
		 */
		C3D_API ShaderProgramBaseSPtr DoGetPanelProgram( uint32_t p_flags );
		/**
		 *\~english
		 *\brief		Retrieves a text program compiled using given flags
		 *\param[in]	p_flags	Bitwise ORed eTEXTURE_CHANNEL
		 *\return		The program
		 *\~french
		 *\brief		Récupère un programme de texte compilé en utilisant les indicateurs donnés
		 *\param[in]	p_flags	Combinaison de eTEXTURE_CHANNEL
		 *\return		Le programme
		 */
		C3D_API ShaderProgramBaseSPtr DoGetTextProgram( uint32_t p_flags );
		/**
		 *\~english
		 *\brief		Retrieves a program compiled using given flags.
		 *\param[in]	p_flags	Bitwise ORed eTEXTURE_CHANNEL.
		 *\param[in,out]p_programs	Looks for a matching shader in this map, creates it if not found and add it to the map.
		 *\return		The program
		 *\~french
		 *\brief		Récupère un programme compilé en utilisant les indicateurs donnés.
		 *\param[in]	p_flags	Combinaison de eTEXTURE_CHANNEL.
		 *\param[in,out]p_programs	Recherche un shader correspondant dans cette map. S'il n'y en a pas, crée le programme et l'ajoute à la map.
		 *\return		Le programme
		 */
		C3D_API virtual ShaderProgramBaseSPtr DoGetProgram( uint32_t p_flags, std::map< uint32_t, ShaderProgramBaseSPtr > & p_programs );
		/**
		 *\~english
		 *\brief		Creates a GeometryBuffers that can contain 1000 characters.
		 *\remarks		Adds this GeometryBuffers to the text GeometryBuffers array.
		 *\return		The created GeometryBuffers.
		 *\~french
		 *\brief		Crée un GeometryBuffers pouvant contenir 1000 caractères.
		 *\remarks		Ajoute de GeometryBuffers au tableau de GeometryBuffers de texte.
		 *\return		Le GeometryBuffers créé.
		 */
		C3D_API GeometryBuffersSPtr DoCreateTextGeometryBuffers();
		/**
		 *\~english
		 *\brief		Function to draw an overlay
		 *\param[in]	p_material			The overlay material
		 *\param[in]	p_geometryBuffers	The overlay geometry buffers
		 *\param[in]	p_texture			An optional font texture
		 *\param[in]	p_count				The vertex count
		 *\~french
		 *\brief		Fonction de dessin d'une incrustation
		 *\param[in]	p_material			The overlay material
		 *\param[in]	p_geometryBuffers	Les buffer de la géométrie de l'incrustation
		 *\param[in]	p_texture			Une texture de polices optionnelle
		 *\param[in]	p_count				Le nombre de sommets
		 */
		C3D_API void DoDrawItem( Material & p_material, GeometryBuffersSPtr p_geometryBuffers, TextureBaseSPtr p_texture, uint32_t p_count );
		/**
		 *\~english
		 *\brief		Fills a GeometryBuffers from a part of a text vertex array
		 *\param[in]	p_count	The number of vertex to copy in the GeometryBuffers
		 *\param[in]	p_it	The beginning of the vertex array
		 *\param[in]	p_index	The current index in the text GeometryBuffers array
		 *\return		The used GeometryBuffers.
		 *\~french
		 *\brief		Remplit un GeometryBuffers d'une partie d'un tableau de sommets pour texte
		 *\param[in]	p_count	Le nombre de sommets à copier dans le GeometryBuffers
		 *\param[in]	p_it	Le début du tableau de sommets
		 *\param[in]	p_index	L'indice courant dans le tableau de GeometryBuffers de texte
		 *\return		Le GeometryBuffers utilisé.
		 */
		C3D_API GeometryBuffersSPtr DoFillTextPart( int32_t p_count, OverlayCategory::VertexArray::const_iterator & p_it, uint32_t & p_index );

	protected:
		//!\~english Geometry buffers for panels	\~french Tampons de géometrie pour les panneaux
		GeometryBuffersSPtr m_pPanelGeometryBuffer;
		//!\~english Geometry buffers for borders	\~french Tampons de géometrie pour les bordures
		GeometryBuffersSPtr m_pBorderGeometryBuffer;
		//!\~english The GeometryBuffers used to render texts	\~french Les GeometryBuffers utilisé pour rendre les textes
		std::vector< GeometryBuffersSPtr > m_pTextsGeometryBuffers;
		//!\~english The buffer elements declaration	\~french La déclaration des éléments du tampon
		BufferDeclarationSPtr m_pDeclaration;
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
		//!\~english Panel overlays vertex array (quad definition)	\~french Tableau de vertex (définition du quad) pour les incrustations panneau
		std::array< Castor3D::BufferElementGroupSPtr, 6 > m_panelVertex;
		//!\~english Border panel overlays vertex array (quad definition)	\~french Tableau de vertex (définition du quad) pour les incrustations bordure
		std::array< Castor3D::BufferElementGroupSPtr, 8 * 6 > m_borderVertex;
		//!\~english Tells if the render size has changed.	\~french Dit si les dimension du rendu ont changé.
		bool m_sizeChanged;
		//!\~english The	overlays blend state	\~french L'état de mélange pour les overlays
		BlendStateWPtr m_wp2DBlendState;
		//!\~english The	overlays depth stencil state	\~french L'état de profoundeur et stenxil pour les overlays
		DepthStencilStateWPtr m_wp2DDepthStencilState;
	};
}

#endif
