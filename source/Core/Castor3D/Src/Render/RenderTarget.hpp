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
#ifndef ___C3D_RENDER_TARGET_H___
#define ___C3D_RENDER_TARGET_H___

#include "Castor3DPrerequisites.hpp"
#include "Miscellaneous/Parameter.hpp"
#include "Texture/TextureUnit.hpp"

#include <OwnedBy.hpp>
#include <Size.hpp>
#include <OwnedBy.hpp>

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		Render target class
	\remark		A render target draws a scene in a frame buffer that can then be used by a window to have a direct render, or a texture to have offscreen rendering
	\~french
	\brief		Classe de cible de rendu (render target)
	\remark		Une render target dessine une scène dans un tampon d'image qui peut ensuite être utilisé dans une fenêtre pour un rendu direct, ou une texture pour un rendu hors écran
	*/
	class RenderTarget
		: public std::enable_shared_from_this< RenderTarget >
		, public Castor::OwnedBy< Engine >
	{
	public:
		/*!
		\author		Sylvain DOREMUS
		\date		14/02/2010
		\~english
		\brief		RenderTarget loader
		\~english
		\brief		Loader de RenderTarget
		*/
		class TextWriter
			: public Castor::TextWriter< RenderTarget >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\param[in]	p_tabs			The tabulations to put at the beginning of each line
			 *\param[in]	p_encodingMode	The file encoding mode
			 *\~french
			 *\brief		Constructeur
			 *\param[in]	p_tabs			Les tabulations à mettre à chaque début de ligne
			 *\param[in]	p_encodingMode	Le mode d'encodage du fichier
			 */
			C3D_API explicit TextWriter( Castor::String const & p_tabs );
			/**
			 *\~english
			 *\brief		Writes a render target into a text file
			 *\param[in]	p_target	the render target
			 *\param[in]	p_file		the file
			 *\~french
			 *\brief		Ecrit une cible de rendu dans un fichier texte
			 *\param[in]	p_target	La cible de rendu
			 *\param[in]	p_file		Le fichier
			 */
			C3D_API bool operator()( Castor3D::RenderTarget const & p_target, Castor::TextFile & p_file )override;
		};

	private:
		/*!
		\author		Sylvain DOREMUS
		\version	0.7.0.0
		\date		19/12/2012
		\~english
		\brief		Internal struct holding a complete frame buffer
		\~french
		\brief		Structure interne contenant un tampon d'image complet
		*/
		struct stFRAME_BUFFER
		{
		public:
			explicit stFRAME_BUFFER( RenderTarget & p_renderTarget );
			bool Create();
			void Destroy();
			bool Initialise( uint32_t p_index, Castor::Size const & p_size );
			void Cleanup();

			//!\~english The texture receiving the color render	\~french La texture recevant le rendu couleur
			TextureUnit m_colorTexture;
			//!\~english The frame buffer	\~french Le tampon d'image
			FrameBufferSPtr m_frameBuffer;
			//!\~english The attach between texture and main frame buffer	\~french L'attache entre la texture et le tampon principal
			TextureAttachmentSPtr m_pColorAttach;

		private:
			RenderTarget & m_renderTarget;
		};

	public:
		/**
		 *\~english
		 * Specified constructor
		 *\param[in]	p_pRoot			The engine root
		 *\param[in]	p_eTargetType	The render target type
		 *\~french
		 * Constructeur spécifié
		 *\param[in]	p_pRoot			Le moteur
		 *\param[in]	p_eTargetType	Le type de render target
		 */
		C3D_API RenderTarget( Engine & p_pRoot, eTARGET_TYPE p_eTargetType = eTARGET_TYPE_WINDOW );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~RenderTarget();
		/**
		 *\~english
		 *\brief		Renders one frame
		 *\param[in]	p_dFrameTime		The time elapsed since the last frame was rendered
		 *\~english
		 *\brief		Rend une image
		 *\param[in]	p_dFrameTime		Le temps écoulé depuis le rendu de la dernière frame
		 */
		C3D_API void Render( uint32_t p_dFrameTime );
		/**
		 *\~english
		 *\brief		Initialisation function
		 *\remarks		Initialises the buffers
		 *\param[in]	p_index	The base texture index
		 *\~french
		 *\brief		Fonction d'initialisation
		 *\remarks		Initialise les buffers
		 *\param[in]	p_index	L'index de texture de base
		 */
		C3D_API void Initialise( uint32_t p_index );
		/**
		 *\~english
		 *\brief		Cleanup function
		 *\~french
		 *\brief		Fonction de nettoyage
		 */
		C3D_API void Cleanup();
		/**
		 *\~english
		 *\brief		Sets the target dimensions.
		 *\remarks		This method must be called before initialisation, otherwise it will have no effect.
		 *\param[in]	p_size	The new dimensions.
		 *\~english
		 *\brief		Définit les dimensions la cible.
		 *\remarks		Cette méthode doit être appelée avant l'initialisation, sinon elle n'aura aucun effet.
		 *\param[in]	p_size	Les nouvelles dimensions.
		 */
		C3D_API void SetSize( Castor::Size const & p_size );
		/**
		 *\~english
		 *\brief		Defines the RenderTechnique.
		 *\param[in]	p_name			The RenderTechnique name.
		 *\param[in]	p_parameters	The RenderTechnique parameters.
		 *\~french
		 *\brief		Définit la RenderTechnique.
		 *\param[in]	p_name			Le nom de la RenderTechnique.
		 *\param[in]	p_parameters	Les paramètres de la RenderTechnique.
		 */
		C3D_API void SetTechnique( Castor::String const & p_name, Parameters const & p_parameters );
		/**
		 *\~english
		 *\brief		Retrieves the eVIEWPORT_TYPE
		 *\return		The eVIEWPORT_TYPE
		 *\~french
		 *\brief		Récupère le eVIEWPORT_TYPE
		 *\return		Le eVIEWPORT_TYPE
		 */
		C3D_API eVIEWPORT_TYPE GetViewportType()const;
		/**
		 *\~english
		 *\brief		Sets the eVIEWPORT_TYPE
		 *\param[in]	val	The new eVIEWPORT_TYPE
		 *\~french
		 *\brief		Définit le eVIEWPORT_TYPE
		 *\param[in]	val	Le nouveau eVIEWPORT_TYPE
		 */
		C3D_API void SetViewportType( eVIEWPORT_TYPE val );
		/**
		 *\~english
		 *\brief		Sets the camera
		 *\remarks		Defines also LEye and REye cameras
		 *\param[in]	p_pCamera	The new camera
		 *\~french
		 *\brief		Définit la caméra
		 *\remarks		Définit aussi les caméras des yeux gauche et droit
		 *\param[in]	p_pCamera	La nouvelle caméra
		 */
		C3D_API void SetCamera( CameraSPtr p_pCamera );
		/**
		 *\~english
		 *\brief		Defines the intra ocular distance
		 *\param[in]	p_rIod	The intra ocular distance
		 *\~french
		 *\brief		Définit la distance inter oculaire
		 *\param[in]	p_rIod	La distance inter oculaire
		 */
		C3D_API void SetIntraOcularDistance( real p_rIod );
		/**
		 *\~english
		 *\brief		Sets the tone mapping implementation type.
		 *\param[in]	p_name			The type.
		 *\param[in]	p_parameters	The parameters.
		 *\~french
		 *\brief		Définit le type d'implémentation de mappage de tons.
		 *\param[in]	p_name			Le type.
		 *\param[in]	p_parameters	Les paramètres.
		 */
		C3D_API void SetToneMappingType( Castor::String const & p_name, Parameters const & p_parameters );
		/**
		 *\~english
		 *\brief		Retrieves the intialisation status
		 *\return		The intialisation status
		 *\~french
		 *\brief		Récupère le statut de l'initialisation
		 *\return		Le statut de l'initialisation
		 */
		inline bool IsInitialised()const
		{
			return m_initialised;
		}
		/**
		 *\~english
		 *\brief		Retrieves the target size
		 *\return		The size
		 *\~english
		 *\brief		Récupère les dimensions de la cible
		 *\return		Les dimensions
		 */
		Castor::Size const & GetSize()const
		{
			return m_size;
		}
		/**
		 *\~english
		 *\brief		Retrieves the DepthStencilState
		 *\return		The DepthStencilState
		 *\~french
		 *\brief		Récupère le DepthStencilState
		 *\return		Le DepthStencilState
		 */
		inline DepthStencilStateSPtr GetDepthStencilState()const
		{
			return m_wpDepthStencilState.lock();
		}
		/**
		 *\~english
		 *\brief		Retrieves the RasteriserState
		 *\return		The RasteriserState
		 *\~french
		 *\brief		Récupère le RasteriserState
		 *\return		Le RasteriserState
		 */
		inline RasteriserStateSPtr GetRasteriserState()const
		{
			return m_wpRasteriserState.lock();
		}
		/**
		 *\~english
		 *\brief		Retrieves the RenderTechnique
		 *\return		The RenderTechnique
		 *\~french
		 *\brief		Récupère la RenderTechnique
		 *\return		La RenderTechnique
		 */
		inline RenderTechniqueSPtr GetTechnique()const
		{
			return m_renderTechnique;
		}
		/**
		 *\~english
		 *\brief		Defines the RenderTechnique
		 *\param[in]	p_pTechnique	The RenderTechnique
		 *\~french
		 *\brief		Définit la RenderTechnique
		 *\param[in]	p_pTechnique	La RenderTechnique
		 */
		inline void SetTechnique( RenderTechniqueSPtr p_pTechnique )
		{
			m_renderTechnique = p_pTechnique;
		}
		/**
		 *\~english
		 *\brief		Retrieves the multisampling status
		 *\return		The multisampling status
		 *\~french
		 *\brief		Récupère le statut de multisampling
		 *\return		Le statut de multisampling
		 */
		inline bool IsMultisampling()const
		{
			return m_bMultisampling;
		}
		/**
		 *\~english
		 *\brief		Retrieves the samples count
		 *\return		The samples count
		 *\~french
		 *\brief		Récupère le nombre de samples
		 *\return		Le nombre de samples
		 */
		inline int32_t GetSamplesCount()const
		{
			return m_samplesCount;
		}
		/**
		 *\~english
		 *\brief		Sets the samples count
		 *\param[in]	val	The new samples count
		 *\~french
		 *\brief		Définit le nombre de samples
		 *\param[in]	val	Le nouveau nombre de samples
		 */
		inline void	SetSamplesCount( int32_t val )
		{
			m_samplesCount = val;
		}
		/**
		 *\~english
		 *\brief		Retrieves the Scene
		 *\return		The Scene
		 *\~french
		 *\brief		Récupère la Scene
		 *\return		La Scene
		 */
		inline SceneSPtr GetScene()const
		{
			return m_pScene.lock();
		}
		/**
		 *\~english
		 *\brief		Retrieves the Camera
		 *\return		The Camera
		 *\~french
		 *\brief		Récupère la Camera
		 *\return		La Camera
		 */
		inline CameraSPtr GetCamera()const
		{
			return m_pCamera.lock();
		}
		/**
		 *\~english
		 *\brief		Retrieves the left eye camera
		 *\return		The camera
		 *\~french
		 *\brief		Récupère la camera de l'oeil gauche
		 *\return		La camera
		 */
		inline CameraSPtr GetCameraLEye()const
		{
			return m_pCameraLEye.lock();
		}
		/**
		 *\~english
		 *\brief		Retrieves the right eye camera
		 *\return		The camera
		 *\~french
		 *\brief		Récupère la camera de l'oeil droit
		 *\return		La camera
		 */
		inline CameraSPtr GetCameraREye()const
		{
			return m_pCameraREye.lock();
		}
		/**
		 *\~english
		 *\brief		Sets the Scene
		 *\param[in]	p_scene	The new Scene
		 *\~french
		 *\brief		Définit la Scene
		 *\param[in]	p_scene	La nouvelle Scene
		 */
		inline void	SetScene( SceneSPtr p_scene )
		{
			m_pScene = p_scene;
		}
		/**
		 *\~english
		 *\brief		Retrieves the frame buffer
		 *\return		The frame buffer
		 *\~french
		 *\brief		Récupère le tampon d'image
		 *\return		Le tampon d'image
		 */
		inline FrameBufferSPtr GetFrameBuffer()const
		{
			return m_pCurrentFrameBuffer.lock();
		}
		/**
		 *\~english
		 *\brief		Retrieves the texture
		 *\return		The texture
		 *\~french
		 *\brief		Récupère la texture
		 *\return		La texture
		 */
		inline TextureUnit const & GetTexture()const
		{
			return m_fbLeftEye.m_colorTexture;
		}
		/**
		 *\~english
		 *\brief		Retrieves the left eye frame buffer
		 *\return		The frame buffer
		 *\~french
		 *\brief		Récupère le tampon d'image oeil gauche
		 *\return		Le tampon d'image
		 */
		inline FrameBufferSPtr GetFrameBufferLEye()const
		{
			return m_fbLeftEye.m_frameBuffer;
		}
		/**
		 *\~english
		 *\brief		Retrieves the left eye texture
		 *\return		The texture
		 *\~french
		 *\brief		Récupère la texture oeil gauche
		 *\return		La texture
		 */
		inline TextureUnit const & GetTextureLEye()const
		{
			return m_fbLeftEye.m_colorTexture;
		}
		/**
		 *\~english
		 *\brief		Retrieves the right eye frame buffer
		 *\return		The frame buffer
		 *\~french
		 *\brief		Récupère le tampon d'image oeil droit
		 *\return		Le tampon d'image
		 */
		inline FrameBufferSPtr GetFrameBufferREye()const
		{
			return m_fbRightEye.m_frameBuffer;
		}
		/**
		 *\~english
		 *\brief		Retrieves the right eye texture
		 *\return		The texture
		 *\~french
		 *\brief		Récupère la texture oeil droit
		 *\return		La texture
		 */
		inline TextureUnit const & GetTextureREye()const
		{
			return m_fbRightEye.m_colorTexture;
		}
		/**
		 *\~english
		 *\brief		Retrieves the stereo status
		 *\return		\p true if stereo is used
		 *\~french
		 *\brief		Récupère le statut d'utilisation stéréo
		 *\return		\p true si le rendu stéréo est utilisé
		 */
		inline bool IsUsingStereo()const
		{
			return m_bStereo;
		}
		/**
		 *\~english
		 *\brief		Defines the stereo status
		 *\param[in]	p_bStereo	\p true if stereo is to be used
		 *\~french
		 *\brief		Définit le statut d'utilisation stéréo
		 *\param[in]	p_bStereo	\p true si le rendu stéréo est à utiliser
		 */
		inline void SetStereo( bool p_bStereo )
		{
			m_bStereo = p_bStereo;
		}
		/**
		 *\~english
		 *\brief		Retrieves the intra ocular distance
		 *\return		The intra ocular distance
		 *\~french
		 *\brief		Récupère la distance inter oculaire
		 *\return		La distance inter oculaire
		 */
		inline real GetIntraOcularDistance()const
		{
			return m_rIntraOcularDistance;
		}
		/**
		 *\~english
		 *\brief		Retrieves the window pixel format
		 *\return		The window pixel format
		 *\~french
		 *\brief		Récupère le format des pixels de la fenêtre
		 *\return		Le format des pixels de la fenêtre
		 */
		inline Castor::ePIXEL_FORMAT GetPixelFormat()const
		{
			return m_pixelFormat;
		}
		/**
		 *\~english
		 *\brief		Sets the window pixel format
		 *\param[in]	val	The new window pixel format
		 *\~french
		 *\brief		Définit le format des pixels de la fenêtre
		 *\param[in]	val	Le nouveau format des pixels de la fenêtre
		 */
		inline void SetPixelFormat( Castor::ePIXEL_FORMAT val )
		{
			m_pixelFormat = val;
		}
		/**
		 *\~english
		 *\brief		Retrieves the target type
		 *\return		The value
		 *\~french
		 *\brief		Récupère le type de cible
		 *\return		La valeur
		 */
		inline eTARGET_TYPE GetTargetType()const
		{
			return m_eTargetType;
		}
		/**
		 *\~english
		 *\brief		Adds a post effect to the list.
		 *\param[in]	p_effect	The effect.
		 *\~french
		 *\brief		Ajoute un effet post rendu à la liste.
		 *\param[in]	p_effect	L'effet.
		 */
		inline void AddPostEffect( PostEffectSPtr p_effect )
		{
			m_postEffects.push_back( p_effect );
		}
		/**
		 *\~english
		 *\return		The target index.
		 *\~french
		 *\return		L'indice de la cible.
		 */
		inline uint32_t GetIndex()const
		{
			return m_index;
		}
		/**
		 *\~english
		 *\return		The post effects array.
		 *\~french
		 *\return		Le tableau d'effets de post rendu.
		 */
		inline PostEffectPtrArray const & GetPostEffects()const
		{
			return m_postEffects;
		}
		/**
		 *\~english
		 *\return		The tone mapping implementation.
		 *\~french
		 *\return		L'implémentation de mappage de tons.
		 */
		inline ToneMappingSPtr GetToneMapping()const
		{
			return m_toneMapping;
		}

	private:
		C3D_API void DoRender( stFRAME_BUFFER & p_fb, CameraSPtr p_pCamera, uint32_t p_frameTime );

	public:
		//!\~english The render target default sampler name	\~french Le nom du sampler par défaut pour la cible de rendu
		C3D_API static const Castor::String DefaultSamplerName;

	protected:
		//!\~english The render target type	\~french Type de RenderTarget
		eTARGET_TYPE m_eTargetType;
		//!\~english Tells if the target is initalised	\~french Dit si la cible est initialisée
		bool m_initialised;
		//!\~english The target size	\~french Les dimensions de la cible
		Castor::Size m_size;
		//!\~english The technique used to render this target	\~french La technique utilisée pour rendre cette cible
		RenderTechniqueSPtr m_renderTechnique;
		//!\~english Tells whether or not to use multisampling	\~french Dit si on utilise le multisampling ou pas
		bool m_bMultisampling;
		//!\~english Defines the samples count if multisampling is activated	\~french Le nombre de samples utilisés pour le multisampling
		int32_t m_samplesCount;
		//!\~english The scene rendered in this render target	\~french La scène rendue par cette RenderTarget
		SceneWPtr m_pScene;
		//!\~english The camera used to render the scene	\~french La caméra utilisée pour rendre la scène
		CameraWPtr m_pCamera;
		//!\~english The left eye camera used to render the stereo scene	\~french La caméra de l'oeil gauche utilisée pour rendre la scène en stéréo
		CameraWPtr m_pCameraLEye;
		//!\~english The right eye camera used to render the stereo scene	\~french La caméra de l'oeil droit utilisée pour rendre la scène en stéréo
		CameraWPtr m_pCameraREye;
		//!\~english Tells the window uses stereoscopic rendering	\~french Dit si la fenêtre utilise un rendu stéréoscopique
		bool m_bStereo;
		//!\~english Intra ocular distance used in stereoscopic rendering	\~french Distance inter oculaire en rendu stéréoscopique
		real m_rIntraOcularDistance;
		//!\~english Frame buffer for left/middle eye	\~french tampon d'image pour l'oeil gauche/milieu
		stFRAME_BUFFER m_fbLeftEye;
		//!\~english Frame buffer for right eye	\~french Le tampon d'image pour l'oeil droit
		stFRAME_BUFFER m_fbRightEye;
		//!\~english The currently active frame buffer (useful in stereoscopic rendering)	\~french Le tampon d'image actuellement actif (utile en rendu stéréoscopique)
		FrameBufferWPtr m_pCurrentFrameBuffer;
		//!\~english The currently active camera (useful in stereoscopic rendering)	\~french La caméra actuellement active (utile en rendu stéréoscopique)
		CameraWPtr m_pCurrentCamera;
		//!\~english The target display format	\~french Le format des pixels de la cible
		Castor::ePIXEL_FORMAT m_pixelFormat;
		//!\~english The number of actually created render targets	\~french Le compte de render target actuellement créées
		static uint32_t sm_uiCount;
		//!\~english This render target's index	\~french L'index de cette render target
		uint32_t m_index;
		//!\~english The render technique name.	\~french Le nom de la technique de rendu.
		Castor::String m_techniqueName;
		//!\~english The render technique parameters.	\~french Les paramètres de la technique de rendu.
		Parameters m_techniqueParameters;
		//!\~english Depth and stencil buffers states	\~french Etats des buffers de profondeur et stencil
		DepthStencilStateWPtr m_wpDepthStencilState;
		//!\~english Rasteriser states	\~french Etats du rasteriser
		RasteriserStateWPtr m_wpRasteriserState;
		//!\~english The post effects.	\~french Les effets post rendu.
		PostEffectPtrArray m_postEffects;
		//!\~english The tone mapping implementation.	\~french L'implémentation de mappage de ton.
		ToneMappingSPtr m_toneMapping;
	};
}

#endif
