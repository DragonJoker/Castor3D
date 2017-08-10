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
#ifndef ___C3D_OVERLAY_RENDERER_H___
#define ___C3D_OVERLAY_RENDERER_H___

#include "Mesh/Buffer/BufferDeclaration.hpp"
#include "Render/RenderNode/PassRenderNode.hpp"
#include "Shader/MatrixUbo.hpp"
#include "Shader/OverlayUbo.hpp"
#include "Shader/UniformBuffer.hpp"
#include "TextOverlay.hpp"

#include <Design/OwnedBy.hpp>

#ifdef drawText
#	undef drawText
#endif

namespace castor3d
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
		: public castor::OwnedBy< RenderSystem >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		C3D_API explicit OverlayRenderer( RenderSystem & renderSystem );
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
		C3D_API void initialise();
		/**
		 *\~english
		 *\brief		Flushes the renderer
		 *\~french
		 *\brief		Nettoie le renderer
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Function to draw a PanelOverlay
		 *\~french
		 *\brief		Fonction de dessin d'un PanelOverlay
		 */
		C3D_API void drawPanel( PanelOverlay & panelOverlay );
		/**
		 *\~english
		 *\brief		Function to draw a BorderPanelOverlay
		 *\~french
		 *\brief		Fonction de dessin d'un BorderPanelOverlay
		 */
		C3D_API void drawBorderPanel( BorderPanelOverlay & borderPanelOverlay );
		/**
		 *\~english
		 *\brief		Function to draw a TextOverlay
		 *\~french
		 *\brief		Fonction de dessin d'un TextOverlay
		 */
		C3D_API void drawText( TextOverlay & textOverlay );
		/**
		 *\~english
		 *\brief		Begins the overlays rendering.
		 *\param[in]	viewport	The render window viewport.
		 *\~french
		 *\brief		Commence le rendu des incrustations.
		 *\param[in]	viewport	Le viewport de la fenêtre de rendu.
		 */
		C3D_API void beginRender( Viewport const & viewport );
		/**
		 *\~english
		 *\brief		Ends the overlays rendering
		 *\~french
		 *\brief		Termine le rendu des incrustations
		 */
		C3D_API void endRender();
		/**
		 *\~english
		 *\return		The current render target size
		 *\~french
		 *\return		Les dimensions de la cible du rendu courant
		 */
		castor::Size const & getSize()const
		{
			return m_size;
		}
		/**
		 *\~english
		 *\return		\p true if the render size has changed.
		 *\~french
		 *\return		\p true si la taille de rendu a changé.
		 */
		bool isSizeChanged()const
		{
			return m_sizeChanged;
		}

	protected:
		/*!
		\author 	Sylvain DOREMUS
		\date 		14/12/2016
		\version	0.9.0
		\~english
		\brief		Holds the needed data to render an overlay.
		\~french
		\brief		Contient les données nécessaires au dessin d'une incrustation.
		*/
		struct OverlayRenderNode
		{
			RenderPipeline & m_pipeline;
			PassRenderNode m_passNode;
			OverlayUbo & m_overlayUbo;
		};
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
		 *\param[in]	pass	The pass.
		 *\return		The program.
		 *\~french
		 *\brief		Récupère un programme de panneau compilé en utilisant la passe donnée.
		 *\param[in]	pass	La passe.
		 *\return		Le programme.
		 */
		C3D_API OverlayRenderNode & doGetPanelNode( Pass & pass );
		/**
		 *\~english
		 *\brief		Retrieves a text program compiled using given pass.
		 *\param[in]	pass	Bitwise ORed TextureChannel.
		 *\return		The program.
		 *\~french
		 *\brief		Récupère un programme de texte compilé en utilisant la passe donnée.
		 *\param[in]	pass	Combinaison de TextureChannel.
		 *\return		Le programme.
		 */
		C3D_API OverlayRenderNode & doGetTextNode( Pass & pass );
		/**
		 *\~english
		 *\brief		Retrieves a panel program compiled using given texture flags.
		 *\param[in]	textureFlags	Bitwise ORed TextureChannel.
		 *\return		The program.
		 *\~french
		 *\brief		Récupère un programme de panneau compilé en utilisant les indicateurs de texture donnés.
		 *\param[in]	textureFlags	Combinaison de TextureChannel.
		 *\return		Le programme.
		 */
		C3D_API RenderPipeline & doGetPanelPipeline( TextureChannels textureFlags );
		/**
		 *\~english
		 *\brief		Retrieves a text program compiled using given texture flags.
		 *\param[in]	textureFlags	Bitwise ORed TextureChannel.
		 *\return		The program.
		 *\~french
		 *\brief		Récupère un programme de texte compilé en utilisant les indicateurs de texture donnés.
		 *\param[in]	textureFlags	Combinaison de TextureChannel.
		 *\return		Le programme.
		 */
		C3D_API RenderPipeline & doGetTextPipeline( TextureChannels textureFlags );
		/**
		 *\~english
		 *\brief		Retrieves a program compiled using given flags.
		 *\param[in]	textureFlags	Bitwise ORed TextureChannel.
		 *\return		The program
		 *\~french
		 *\brief		Récupère un programme compilé en utilisant les indicateurs donnés.
		 *\param[in]	textureFlags	Combinaison de TextureChannel.
		 *\return		Le programme
		 */
		C3D_API RenderPipeline & doGetPipeline( TextureChannels const & textureFlags );
		/**
		 *\~english
		 *\brief		Creates a GeometryBuffers that can contain 1000 characters.
		 *\remarks		adds this GeometryBuffers to the text GeometryBuffers array.
		 *\return		The created GeometryBuffers.
		 *\~french
		 *\brief		Crée un GeometryBuffers pouvant contenir 1000 caractères.
		 *\remarks		Ajoute de GeometryBuffers au tableau de GeometryBuffers de texte.
		 *\return		Le GeometryBuffers créé.
		 */
		C3D_API OverlayGeometryBuffers doCreateTextGeometryBuffers();
		/**
		 *\~english
		 *\brief		Function to draw an overlay.
		 *\param[in]	material		The material.
		 *\param[in]	geometryBuffers	The overlay geometry buffers.
		 *\param[in]	count			The vertex count.
		 *\~french
		 *\brief		Fonction de dessin d'une incrustation.
		 *\param[in]	material		Le matériau.
		 *\param[in]	geometryBuffers	Les buffer de la géométrie de l'incrustation.
		 *\param[in]	count			Le nombre de sommets.
		 */
		C3D_API void doDrawItem( Material & material
			, OverlayGeometryBuffers const & geometryBuffers
			, uint32_t count );
		/**
		 *\~english
		 *\brief		Function to draw an overlay.
		 *\param[in]	pass			The pass.
		 *\param[in]	geometryBuffers	The overlay geometry buffers.
		 *\param[in]	count			The vertex count.
		 *\~french
		 *\brief		Fonction de dessin d'une incrustation.
		 *\param[in]	pass			La passe.
		 *\param[in]	geometryBuffers	Les buffer de la géométrie de l'incrustation.
		 *\param[in]	count			Le nombre de sommets.
		 */
		C3D_API void doDrawItem( Pass & pass
			, GeometryBuffers const & geometryBuffers
			, uint32_t count );
		/**
		 *\~english
		 *\brief		Function to draw a text overlay.
		 *\param[in]	pass			The pass.
		 *\param[in]	geometryBuffers	The overlay geometry buffers.
		 *\param[in]	texture			The font texture.
		 *\param[in]	sampler			The texture sampler.
		 *\param[in]	count			The vertex count.
		 *\~french
		 *\brief		Fonction de dessin d'une incrustation texte.
		 *\param[in]	pass			La passe.
		 *\param[in]	geometryBuffers	Les buffer de la géométrie de l'incrustation.
		 *\param[in]	texture			La texture de polices.
		 *\param[in]	sampler			L'échantillonneur de la texture.
		 *\param[in]	count			Le nombre de sommets.
		 */
		C3D_API void doDrawItem( Pass & pass
			, GeometryBuffers const & geometryBuffers
			, TextureLayout const & texture
			, Sampler const & sampler
			, uint32_t count );
		/**
		 *\~english
		 *\brief		Fills a GeometryBuffers from a part of a text vertex array
		 *\param[in]	count	The number of vertex to copy in the GeometryBuffers
		 *\param[in]	it		The beginning of the vertex array
		 *\param[in]	index	The current index in the text GeometryBuffers array
		 *\return		The used GeometryBuffers.
		 *\~french
		 *\brief		Remplit un GeometryBuffers d'une partie d'un tableau de sommets pour texte
		 *\param[in]	count	Le nombre de sommets à copier dans le GeometryBuffers
		 *\param[in]	it		Le début du tableau de sommets
		 *\param[in]	index	L'indice courant dans le tableau de GeometryBuffers de texte
		 *\return		Le GeometryBuffers utilisé.
		 */
		C3D_API OverlayGeometryBuffers doFillTextPart( int32_t count
			, TextOverlay::VertexArray::const_iterator & it
			, uint32_t & index );
		/**
		 *\~english
		 *\brief		Creates a shader program for overlays rendering use.
		 *\param[in]	textureFlags	Bitwise ORed TextureChannel.
		 *\return		The created program.
		 *\~french
		 *\brief		Crée un programme shader pour les rendu d'incrustations.
		 *\param[in]	textureFlags	Combinaison de TextureChannel.
		 *\return		Le programme créé.
		 */
		C3D_API ShaderProgramSPtr doCreateOverlayProgram( TextureChannels const & textureFlags );

	protected:
		//!\~english	Vertex buffers for panels.
		//!\~french		Tampons de sommets pour les panneaux.
		VertexBufferSPtr m_panelVertexBuffer;
		//!\~english	Vertex buffers for borders.
		//!\~french		Tampons de sommets pour les bordures.
		VertexBufferSPtr m_borderVertexBuffer;
		//!\~english	The Vertex buffers used to render texts.
		//!\~french		Les tampons de sommets utilisés pour rendre les textes.
		std::vector< VertexBufferSPtr > m_textsVertexBuffers;
		//!\~english	Geometry buffers for panels.
		//!\~french		Tampons de géometrie pour les panneaux.
		OverlayGeometryBuffers m_panelGeometryBuffers;
		//!\~english	Geometry buffers for borders.
		//!\~french		Tampons de géometrie pour les bordures.
		OverlayGeometryBuffers m_borderGeometryBuffers;
		//!\~english	The GeometryBuffers used to render texts.
		//!\~french		Les GeometryBuffers utilisé pour rendre les textes.
		std::vector< OverlayGeometryBuffers > m_textsGeometryBuffers;
		//!\~english	The buffer elements declaration.
		//!\~french		La déclaration des éléments du tampon.
		BufferDeclaration m_declaration;
		//!\~english	The text overlay buffer elements declaration.
		//!\~french		La déclaration des éléments du tampon, pour les textes.
		BufferDeclaration m_textDeclaration;
		//!\~english	The current render target size.
		//!\~french		Les dimensions de la cible du rendu courant.
		castor::Size m_size;
		//!\~english	The shader programs used to render a panel (used for borders too).
		//!\~french		Les programmes de shader utilisés pour rendre un panneau (utilisé pour les bords aussi).
		std::map< Pass *, OverlayRenderNode > m_mapPanelNodes;
		//!\~english	The shader programs used to render texts.
		//!\~french		Les programmes de shader utilisés pour rendre les textes.
		std::map< Pass *, OverlayRenderNode > m_mapTextNodes;
		//!\~english	The shader programs.
		//!\~french		Les programmes de shader.
		std::map< uint32_t, RenderPipelineUPtr > m_pipelines;
		//!\~english	Text texture sampler.
		//!\~french		Echantillonneur de la texture de texte.
		Uniform1iSPtr m_mapText;
		//!\~english	The previously rendered BorderPanelOverlay z-index.
		//!\~french		Le z-index du précedent BorderPanelOverlay rendu.
		int m_previousBorderZIndex{ 0 };
		//!\~english	The previously rendered PanelOverlay z-index.
		//!\~french		Le z-index du précedent PanelOverlay rendu.
		int m_previousPanelZIndex{ 0 };
		//!\~english	The previously rendered TextOverlay z-index.
		//!\~french		Le z-index du précedent TextOverlay rendu.
		int m_previousTextZIndex{ 0 };
		//!\~english	The previously rendered text.
		//!\~french		Le texte rendu précédemment.
		castor::String m_previousCaption;
		//!\~english	Panel overlays vertex array (quad definition).
		//!\~french		Tableau de vertex (définition du quad) pour les incrustations panneau.
		std::array< castor3d::BufferElementGroupSPtr, 6 > m_panelVertex;
		//!\~english	Border panel overlays vertex array (quad definition).
		//!\~french		Tableau de vertex (définition du quad) pour les incrustations bordure.
		std::array< castor3d::BufferElementGroupSPtr, 8 * 6 > m_borderVertex;
		//!\~english	Tells if the render size has changed.
		//!\~french		Dit si les dimension du rendu ont changé.
		bool m_sizeChanged{ true };
		//!\~english	The uniform buffer containing matrices data.
		//!\~french		Le tampon d'uniformes contenant les données de matrices.
		MatrixUbo m_matrixUbo;
		//!\~english	The uniform buffer containing overlay data.
		//!\~french		Le tampon d'uniformes contenant les données d'incrustation.
		OverlayUbo m_overlayUbo;
	};
}

#endif
