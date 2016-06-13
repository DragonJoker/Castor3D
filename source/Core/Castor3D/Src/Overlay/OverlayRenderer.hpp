/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
#include "Mesh/Buffer/BufferDeclaration.hpp"
#include "TextOverlay.hpp"

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
		C3D_API explicit OverlayRenderer( RenderSystem & p_renderSystem );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~OverlayRenderer();
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
		/*!
		\author 	Sylvain DOREMUS
		\date 		26/03/2016
		\version	0.8.0
		\~english
		\brief		Holds the two geometry buffers used to render overlays.
		\~french
		\brief		Contient les deux geometry buffers utilisés pour dessiner les overlays.
		*/
		struct OverlayGeometryBuffers
		{
			GeometryBuffersSPtr m_noTexture;
			GeometryBuffersSPtr m_textured;
		};
		/**
		 *\~english
		 *\brief		Retrieves a panel program compiled using given pass.
		 *\param[in]	p_pass	The pass.
		 *\return		The program.
		 *\~french
		 *\brief		Récupère un programme de panneau compilé en utilisant la passe donnée.
		 *\param[in]	p_pass	La passe.
		 *\return		Le programme.
		 */
		C3D_API RenderNode & DoGetPanelProgram( Pass & p_pass );
		/**
		 *\~english
		 *\brief		Retrieves a text program compiled using given pass.
		 *\param[in]	p_pass	Bitwise ORed TextureChannel.
		 *\return		The program.
		 *\~french
		 *\brief		Récupère un programme de texte compilé en utilisant la passe donnée.
		 *\param[in]	p_pass	Combinaison de TextureChannel.
		 *\return		Le programme.
		 */
		C3D_API RenderNode & DoGetTextProgram( Pass & p_pass );
		/**
		 *\~english
		 *\brief		Retrieves a panel program compiled using given texture flags.
		 *\param[in]	p_flags	Bitwise ORed TextureChannel.
		 *\return		The program.
		 *\~french
		 *\brief		Récupère un programme de panneau compilé en utilisant les indicateurs de texture donnés.
		 *\param[in]	p_flags	Combinaison de TextureChannel.
		 *\return		Le programme.
		 */
		C3D_API ShaderProgramSPtr DoGetPanelProgram( uint32_t p_flags );
		/**
		 *\~english
		 *\brief		Retrieves a text program compiled using given texture flags
		 *\param[in]	p_flags	Bitwise ORed TextureChannel.
		 *\return		The program.
		 *\~french
		 *\brief		Récupère un programme de texte compilé en utilisant les indicateurs de texture donnés.
		 *\param[in]	p_flags	Combinaison de TextureChannel.
		 *\return		Le programme.
		 */
		C3D_API ShaderProgramSPtr DoGetTextProgram( uint32_t p_flags );
		/**
		 *\~english
		 *\brief			Retrieves a program compiled using given flags.
		 *\param[in]		p_flags		Bitwise ORed TextureChannel.
		 *\return			The program
		 *\~french
		 *\brief			Récupère un programme compilé en utilisant les indicateurs donnés.
		 *\param[in]		p_flags		Combinaison de TextureChannel.
		 *\return			Le programme
		 */
		C3D_API ShaderProgramSPtr DoGetProgram( uint32_t p_flags );
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
		C3D_API OverlayGeometryBuffers DoCreateTextGeometryBuffers();
		/**
		 *\~english
		 *\brief		Function to draw an overlay.
		 *\param[in]	p_material			The material.
		 *\param[in]	p_geometryBuffers	The overlay geometry buffers.
		 *\param[in]	p_count				The vertex count.
		 *\~french
		 *\brief		Fonction de dessin d'une incrustation.
		 *\param[in]	p_material			Le matériau.
		 *\param[in]	p_geometryBuffers	Les buffer de la géométrie de l'incrustation.
		 *\param[in]	p_count				Le nombre de sommets.
		 */
		C3D_API void DoDrawItem( Material & p_material, OverlayGeometryBuffers const & p_geometryBuffers, uint32_t p_count );
		/**
		 *\~english
		 *\brief		Function to draw an overlay.
		 *\param[in]	p_pass				The pass.
		 *\param[in]	p_geometryBuffers	The overlay geometry buffers.
		 *\param[in]	p_count				The vertex count.
		 *\~french
		 *\brief		Fonction de dessin d'une incrustation.
		 *\param[in]	p_pass				La passe.
		 *\param[in]	p_geometryBuffers	Les buffer de la géométrie de l'incrustation.
		 *\param[in]	p_count				Le nombre de sommets.
		 */
		C3D_API void DoDrawItem( Pass & p_pass, GeometryBuffers const & p_geometryBuffers, uint32_t p_count );
		/**
		 *\~english
		 *\brief		Function to draw a text overlay.
		 *\param[in]	p_pass				The pass.
		 *\param[in]	p_geometryBuffers	The overlay geometry buffers.
		 *\param[in]	p_texture			The font texture.
		 *\param[in]	p_sampler			The texture sampler.
		 *\param[in]	p_count				The vertex count.
		 *\~french
		 *\brief		Fonction de dessin d'une incrustation texte.
		 *\param[in]	p_pass				La passe.
		 *\param[in]	p_geometryBuffers	Les buffer de la géométrie de l'incrustation.
		 *\param[in]	p_texture			La texture de polices.
		 *\param[in]	p_sampler			L'échantillonneur de la texture.
		 *\param[in]	p_count				Le nombre de sommets.
		 */
		C3D_API void DoDrawItem( Pass & p_pass, GeometryBuffers const & p_geometryBuffers, TextureLayout const & p_texture, Sampler const & p_sampler, uint32_t p_count );
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
		C3D_API OverlayGeometryBuffers DoFillTextPart( int32_t p_count, TextOverlay::VertexArray::const_iterator & p_it, uint32_t & p_index );
		/**
		 *\~english
		 *\brief		Creates a shader program for overlays rendering use.
		 *\return		The created program.
		 *\~french
		 *\brief		Crée un programme shader pour les rendu d'incrustations
		 *\return		Le programme créé.
		 */
		C3D_API ShaderProgramSPtr DoCreateOverlayProgram( uint32_t p_flags );

	protected:
		//!\~english Vertex buffers for panels.	\~french Tampons de sommets pour les panneaux.
		VertexBufferSPtr m_panelVertexBuffer;
		//!\~english Vertex buffers for borders.	\~french Tampons de sommets pour les bordures.
		VertexBufferSPtr m_borderVertexBuffer;
		//!\~english The Vertex buffers used to render texts.	\~french Les tampons de sommets utilisés pour rendre les textes.
		std::vector< VertexBufferSPtr > m_textsVertexBuffers;
		//!\~english Geometry buffers for panels	\~french Tampons de géometrie pour les panneaux
		OverlayGeometryBuffers m_panelGeometryBuffers;
		//!\~english Geometry buffers for borders	\~french Tampons de géometrie pour les bordures
		OverlayGeometryBuffers m_borderGeometryBuffers;
		//!\~english The GeometryBuffers used to render texts	\~french Les GeometryBuffers utilisé pour rendre les textes
		std::vector< OverlayGeometryBuffers > m_textsGeometryBuffers;
		//!\~english The buffer elements declaration	\~french La déclaration des éléments du tampon
		BufferDeclaration m_declaration;
		//!\~english The text overlay buffer elements declaration.	\~french La déclaration des éléments du tampon, pour les textes.
		BufferDeclaration m_textDeclaration;
		//!\~english The current render target size	\~french Les dimensions de la cible du rendu courant
		Castor::Size m_size;
		//!\~english The shader programs used to render a panel (used for borders too)	\~french Les programmes de shader utilisés pour rendre un panneau (utilisé pour les bords aussi)
		std::map< Pass *, RenderNode > m_mapPanelPrograms;
		//!\~english The shader programs used to render texts	\~french Les programmes de shader utilisés pour rendre les textes
		std::map< Pass *, RenderNode > m_mapTextPrograms;
		//!\~english The shader programs.	\~french Les programmes de shader.
		std::map< uint32_t, ShaderProgramSPtr > m_programs;
		//!\~english Text texture sampler	\~french Echantillonneur de la texture de texte
		OneIntFrameVariableSPtr m_mapText;
		//!\~english The previously rendered BorderPanelOverlay z-index	\~french Le z-index du précedent BorderPanelOverlay rendu
		int m_previousBorderZIndex{ 0 };
		//!\~english The previously rendered PanelOverlay z-index	\~french Le z-index du précedent PanelOverlay rendu
		int m_previousPanelZIndex{ 0 };
		//!\~english The previously rendered TextOverlay z-index	\~french Le z-index du précedent TextOverlay rendu
		int m_previousTextZIndex{ 0 };
		//!\~english The previously rendered text	\~french Le texte rendu précédemment
		Castor::String m_previousCaption;
		//!\~english Panel overlays vertex array (quad definition)	\~french Tableau de vertex (définition du quad) pour les incrustations panneau
		std::array< Castor3D::BufferElementGroupSPtr, 6 > m_panelVertex;
		//!\~english Border panel overlays vertex array (quad definition)	\~french Tableau de vertex (définition du quad) pour les incrustations bordure
		std::array< Castor3D::BufferElementGroupSPtr, 8 * 6 > m_borderVertex;
		//!\~english Tells if the render size has changed.	\~french Dit si les dimension du rendu ont changé.
		bool m_sizeChanged{ true };
		//!\~english The overlays blend state.	\~french L'état de mélange pour les incrustations.
		BlendStateWPtr m_wpBlendState;
		//!\~english The overlays depth stencil state.	\~french L'état de profoundeur et stencil pour les incrustations.
		DepthStencilStateWPtr m_wpDepthStencilState;
		//!\~english The overlays rasteriser state.	\~french L'état de rastérisation pour les incrustations.
		RasteriserStateWPtr m_wpRasteriserState;
	};
}

#endif
