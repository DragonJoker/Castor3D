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
#ifndef ___C3D_DEFERRED_SHADING_RENDER_TECHNIQUE_H___
#define ___C3D_DEFERRED_SHADING_RENDER_TECHNIQUE_H___

#include <RenderTechnique.hpp>

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

#ifndef _WIN32
#	define C3D_DeferredTechnique_API
#else
#	ifdef DeferredTechnique_EXPORTS
#		define C3D_DeferredTechnique_API __declspec(dllexport)
#	else
#		define C3D_DeferredTechnique_API __declspec(dllimport)
#	endif
#endif

namespace Deferred
{
	using Castor3D::Point3rFrameVariable;
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		12/11/2012
	\~english
	\brief		Deferred lighting Render technique class
	\remarks	Creates up to 6 render buffers (position, diffuse, normals, tangents, bitangents, specular)
				<br />it renders to these buffers (Geometry Pass)
				<br />then renders these buffers into the final one, adding the lights (Light Pass)
	\~french
	\brief		Classe de technique de rendu implémentant le Deferred lighting
	\remarks	On crée jusqu'à 6 tampons de rendu (position, diffuse, normales, tangentes, bitangentes, spéculaire)
				<br />on fait le rendu dans ces tampons (Geometry Pass)
				<br />puis on fait un rendu de ces tampons en ajoutant les lumières (Light Pass)
	*/
	class RenderTechnique
		:	public Castor3D::RenderTechniqueBase
	{
	protected:
		typedef enum eDS_TEXTURE CASTOR_TYPE( uint8_t )
		{
			eDS_TEXTURE_POSITION,
			eDS_TEXTURE_DIFFUSE,
			eDS_TEXTURE_NORMALS,
			eDS_TEXTURE_TANGENT,
			eDS_TEXTURE_BITANGENT,
			eDS_TEXTURE_SPECULAR,
			eDS_TEXTURE_COUNT,
		}	eDS_TEXTURE;

		DECLARE_SMART_PTR( Point3rFrameVariable );

	protected:
		/**
		 *\~english
		 *\brief		Constructor
		 *\remarks		Used only by TechniqueFactory
		 *\~french
		 *\brief		Constructeur
		 *\remarks		Utilisé uniquement par TechniqueFactory
		 */
		RenderTechnique();
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_renderTarget	The render target for this technique
		 *\param[in]	p_pRenderSystem	The render system
		 *\param[in]	p_params		The technique parameters
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_renderTarget	La render target pour cette technique
		 *\param[in]	p_pRenderSystem	Le render system
		 *\param[in]	p_params		Les paramètres de la technique
		 */
		RenderTechnique( Castor3D::RenderTarget & p_renderTarget, Castor3D::RenderSystem * p_pRenderSystem, Castor3D::Parameters const & p_params );

	public:
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~RenderTechnique();
		/**
		 *\~english
		 *\brief		Instantiation function, used by the factory to create objects of a wanted type
		 *\param[in]	p_renderTarget	The technique render target
		 *\param[in]	p_pRenderSystem	The render system
		 *\param[in]	p_params		The technique parameters
		 *\return		A clone of this object
		 *\~french
		 *\brief		Fonction d'instanciation, utilisée par la fabrique pour créer des objets d'un type donné
		 *\param[in]	p_renderTarget	La cible de rendu de la technique
		 *\param[in]	p_pRenderSystem	Le render system
		 *\param[in]	p_params		Les paramètres de la technique
		 *\return		Un clône de cet objet
		 */
		static Castor3D::RenderTechniqueBaseSPtr CreateInstance( Castor3D::RenderTarget & p_renderTarget, Castor3D::RenderSystem * p_pRenderSystem, Castor3D::Parameters const & p_params );

	protected:
		/**
		 *\~english
		 *\brief		Creation function
		 *\return		\p true if OK
		 *\~french
		 *\brief		Fonction de création
		 *\return		\p true si tout s'est bien passé
		 */
		virtual bool DoCreate();
		/**
		 *\~english
		 *\brief		Destruction function
		 *\~french
		 *\brief		Fonction de destruction
		 */
		virtual void DoDestroy();
		/**
		 *\~english
		 *\brief		Initialisation function
		 *\param[in]	p_index	The base texture index
		 *\return		\p true if ok
		 *\~french
		 *\brief		Fonction d'initialisation
		 *\param[in]	p_index	L'index de texture de base
		 *\return		\p true if ok
		 */
		virtual bool DoInitialise( uint32_t & p_index );
		/**
		 *\~english
		 *\brief		Cleanup function
		 *\~french
		 *\brief		Fonction de nettoyage
		 */
		virtual void DoCleanup();
		/**
		 *\~english
		 *\brief		Render begin function
		 *\return		\p true if ok
		 *\~french
		 *\brief		Fonction de début de rendu
		 *\return		\p true si tout s'est bien passé
		 */
		virtual bool DoBeginRender();
		/**
		 *\~english
		 *\brief		Render function
		 *\param[in]	p_scene			The scene to render
		 *\param[in]	p_camera		The camera through which the scene is viewed
		 *\param[in]	p_ePrimitives	The display mode
		 *\param[in]	p_dFrameTime	The time elapsed since last frame was rendered
		 *\return		\p true if ok
		 *\~french
		 *\brief		Fonction de rendu
		 *\param[in]	p_scene			La scène à rendre
		 *\param[in]	p_camera		La caméra à travers laquelle la scène est vue
		 *\param[in]	p_ePrimitives	Le mode d'affichage
		 *\param[in]	p_dFrameTime	Le temps écoulé depuis le rendu de la dernière frame
		 *\return		\p true si tout s'est bien passé
		 */
		virtual bool DoRender( Castor3D::Scene & p_scene, Castor3D::Camera & p_camera, Castor3D::eTOPOLOGY p_ePrimitives, double p_dFrameTime );
		/**
		 *\~english
		 *\brief		Render end function
		 *\~french
		 *\brief		Fonction de fin de rendu
		 */
		virtual void DoEndRender();
		/**
		 *\~english
		 *\brief		Retrieves the vertex shader source matching the given flags
		 *\param[in]	p_uiProgramFlags	Bitwise ORed ePROGRAM_FLAG
		 *\~french
		 *\brief		Récupère le source du vertex shader correspondant aux flags donnés
		 *\param[in]	p_uiProgramFlags	Une combinaison de ePROGRAM_FLAG
		 */
		virtual Castor::String DoGetVertexShaderSource( uint32_t p_uiProgramFlags )const;
		/**
		 *\~english
		 *\brief		Retrieves the pixel shader source matching the given flags
		 *\param[in]	p_uiFlags	A combination of eTEXTURE_CHANNEL
		 *\~french
		 *\brief		Récupère le source du pixel shader correspondant aux flags donnés
		 *\param[in]	p_uiFlags	Une combinaison de eTEXTURE_CHANNEL
		 */
		virtual Castor::String DoGetPixelShaderSource( uint32_t p_uiFlags )const;
		/**
		 *\~english
		 *\brief		Retrieves the vertex shader source matching the given flags
		 *\param[in]	p_uiProgramFlags	Bitwise ORed ePROGRAM_FLAG
		 *\~french
		 *\brief		Récupère le source du vertex shader correspondant aux flags donnés
		 *\param[in]	p_uiProgramFlags	Une combinaison de ePROGRAM_FLAG
		 */
		Castor::String DoGetLightPassVertexShaderSource( uint32_t p_uiProgramFlags )const;
		/**
		 *\~english
		 *\brief		Retrieves the pixel shader source matching the given flags
		 *\param[in]	p_uiFlags	A combination of eTEXTURE_CHANNEL
		 *\~french
		 *\brief		Récupère le source du pixel shader correspondant aux flags donnés
		 *\param[in]	p_uiFlags	Une combinaison de eTEXTURE_CHANNEL
		 */
		Castor::String DoGetLightPassPixelShaderSource( uint32_t p_uiFlags )const;

#if C3D_HAS_GL_RENDERER
		/**
		 *\~english
		 *\brief		Retrieves the vertex shader source matching the given flags
		 *\param[in]	p_uiProgramFlags	Bitwise ORed ePROGRAM_FLAG
		 *\~french
		 *\brief		Récupère le source du vertex shader correspondant aux flags donnés
		 *\param[in]	p_uiProgramFlags	Une combinaison de ePROGRAM_FLAG
		 */
		Castor::String DoGetGlVertexShaderSource( uint32_t p_uiProgramFlags )const;
		/**
		 *\~english
		 *\brief		Retrieves the pixel shader source matching the given flags
		 *\param[in]	p_uiFlags	A combination of eTEXTURE_CHANNEL
		 *\~french
		 *\brief		Récupère le source du pixel shader correspondant aux flags donnés
		 *\param[in]	p_uiFlags	Une combinaison de eTEXTURE_CHANNEL
		 */
		Castor::String DoGetGlPixelShaderSource( uint32_t p_uiFlags )const;
		/**
		 *\~english
		 *\brief		Retrieves the vertex shader source matching the given flags
		 *\param[in]	p_uiProgramFlags	Bitwise ORed ePROGRAM_FLAG
		 *\~french
		 *\brief		Récupère le source du vertex shader correspondant aux flags donnés
		 *\param[in]	p_uiProgramFlags	Une combinaison de ePROGRAM_FLAG
		 */
		Castor::String DoGetGlLightPassVertexShaderSource( uint32_t p_uiProgramFlags )const;
		/**
		 *\~english
		 *\brief		Retrieves the pixel shader source matching the given flags
		 *\param[in]	p_uiFlags	A combination of eTEXTURE_CHANNEL
		 *\~french
		 *\brief		Récupère le source du pixel shader correspondant aux flags donnés
		 *\param[in]	p_uiFlags	Une combinaison de eTEXTURE_CHANNEL
		 */
		Castor::String DoGetGlLightPassPixelShaderSource( uint32_t p_uiFlags )const;
#endif

#if C3D_HAS_D3D11_RENDERER
		/**
		 *\~english
		 *\brief		Retrieves the vertex shader source matching the given flags
		 *\param[in]	p_uiProgramFlags	Bitwise ORed ePROGRAM_FLAG
		 *\~french
		 *\brief		Récupère le source du vertex shader correspondant aux flags donnés
		 *\param[in]	p_uiProgramFlags	Une combinaison de ePROGRAM_FLAG
		 */
		Castor::String DoGetD3D11VertexShaderSource( uint32_t p_uiProgramFlags )const;
		/**
		 *\~english
		 *\brief		Retrieves the pixel shader source matching the given flags
		 *\param[in]	p_uiFlags	A combination of eTEXTURE_CHANNEL
		 *\~french
		 *\brief		Récupère le source du pixel shader correspondant aux flags donnés
		 *\param[in]	p_uiFlags	Une combinaison de eTEXTURE_CHANNEL
		 */
		Castor::String DoGetD3D11PixelShaderSource( uint32_t p_uiFlags )const;
		/**
		 *\~english
		 *\brief		Retrieves the vertex shader source matching the given flags
		 *\param[in]	p_uiProgramFlags	Bitwise ORed ePROGRAM_FLAG
		 *\~french
		 *\brief		Récupère le source du vertex shader correspondant aux flags donnés
		 *\param[in]	p_uiProgramFlags	Une combinaison de ePROGRAM_FLAG
		 */
		Castor::String DoGetD3D11LightPassVertexShaderSource( uint32_t p_uiProgramFlags )const;
		/**
		 *\~english
		 *\brief		Retrieves the pixel shader source matching the given flags
		 *\param[in]	p_uiFlags	A combination of eTEXTURE_CHANNEL
		 *\~french
		 *\brief		Récupère le source du pixel shader correspondant aux flags donnés
		 *\param[in]	p_uiFlags	Une combinaison de eTEXTURE_CHANNEL
		 */
		Castor::String DoGetD3D11LightPassPixelShaderSource( uint32_t p_uiFlags )const;
#endif

	protected:
		//!\~english The various textures	\~french Les diverses textures
		Castor3D::DynamicTextureSPtr m_lightPassTextures[eDS_TEXTURE_COUNT];
		//!\~english The buffer receiving the depth colour	\~french Le tampon recevant la couleur de la profondeur
		Castor3D::DepthStencilRenderBufferSPtr m_lightPassBufDepth;
		//!\~english The deferred shading frame buffer	\~french Le tampon d'image pour le deferred shading
		Castor3D::FrameBufferSPtr m_lightPassFrameBuffer;
		//!\~english The attachments between textures and deferred shading frame buffer	\~french Les attaches entre les texture et le tampon deferred shading
		Castor3D::TextureAttachmentSPtr m_lightPassTexAttachs[eDS_TEXTURE_COUNT];
		//!\~english The attach between depth texture and deferred shading frame buffer	\~french L'attache entre la texture profondeur et le tampon deferred shading
		Castor3D::RenderBufferAttachmentSPtr m_lightPassDepthAttach;
		//!\~english The shader program used to render lights	\~french Le shader utilisé pour rendre les lumières
		Castor3D::ShaderProgramBaseSPtr m_lightPassShaderProgram;
		//!\~english The framve variable buffer used to apply matrices	\~french Le tampon de variables shader utilisé pour appliquer les matrices
		Castor3D::FrameVariableBufferWPtr m_lightPassMatrices;
		//!\~english The framve variable buffer used to transmit scene values	\~french Le tampon de variables shader utilisé pour transmettre les variables de scène
		Castor3D::FrameVariableBufferWPtr m_lightPassScene;
		//!\~english Buffer elements declaration	\~french Déclaration des éléments d'un vertex
		Castor3D::BufferDeclarationSPtr m_pDeclaration;
		//!\~english Vertex array (quad definition)	\~french Tableau de vertex (définition du quad)
		std::array< Castor3D::BufferElementGroupSPtr, 6 > m_arrayVertex;
		//!\~english Geometry buffers holder	\~french Conteneur de buffers de géométries
		Castor3D::GeometryBuffersSPtr m_pGeometryBuffers;
		//!\~english The viewport used when rendering is done	\~french Le viewport utilisé pour rendre la cible sur sa cible (fenêtre ou texture)
		Castor3D::ViewportSPtr m_pViewport;
		//!\~english The shader variable containing the camera position	\~french La variable de shader contenant la position de la caméra
		Castor3D::Point3rFrameVariableSPtr m_pShaderCamera;
		//!\~english The depth stencil state used by the geometric pass	\~french Le DepthStencilState utilisé par la passe géométrique
		Castor3D::DepthStencilStateSPtr m_geometryPassDsState;
		//!\~english The depth stencil state used byt he lights pass	\~french Le DepthStencilState utilisé par la passe lumières
		Castor3D::DepthStencilStateSPtr m_lightPassDsState;
	};
}

#pragma warning( pop )

#endif
