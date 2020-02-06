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
#ifndef ___C3D_RENDER_TARGET_H___
#define ___C3D_RENDER_TARGET_H___

#include "Castor3DPrerequisites.hpp"
#include "Miscellaneous/Parameter.hpp"
#include "Render/RenderInfo.hpp"
#include "Texture/TextureUnit.hpp"

#include <Design/OwnedBy.hpp>
#include <Graphics/Size.hpp>
#include <Design/OwnedBy.hpp>

namespace Castor3D
{
	template< typename KeyType >
	struct ElementProducer< RenderTarget, KeyType >
	{
		using Type = std::function< std::shared_ptr< RenderTarget >( KeyType const &, TargetType ) >;
	};
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
			 *\param[in]	p_tabs	The tabulations to put at the beginning of each line
			 *\~french
			 *\brief		Constructeur
			 *\param[in]	p_tabs	Les tabulations à mettre à chaque début de ligne
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
			bool Initialise( uint32_t p_index, Castor::Size const & p_size );
			void Cleanup();

			//!\~english The texture receiving the color render	\~french La texture recevant le rendu couleur
			TextureUnit m_colourTexture;
			//!\~english The frame buffer	\~french Le tampon d'image
			FrameBufferSPtr m_frameBuffer;
			//!\~english The attach between texture and main frame buffer	\~french L'attache entre la texture et le tampon principal
			TextureAttachmentSPtr m_colourAttach;

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
		C3D_API RenderTarget( Engine & p_pRoot, TargetType p_eTargetType = TargetType::eWindow );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~RenderTarget();
		/**
		 *\~english
		 *\brief		Renders one frame.
		 *\param[out]	p_info	Receives the render informations.
		 *\~french
		 *\brief		Dessine une frame.
		 *\param[out]	p_info	Reçoit les informations de rendu.
		 */
		C3D_API void Render( RenderInfo & p_info );
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
		 *\brief		Retrieves the ViewportType
		 *\return		The ViewportType
		 *\~french
		 *\brief		Récupère le ViewportType
		 *\return		Le ViewportType
		 */
		C3D_API ViewportType GetViewportType()const;
		/**
		 *\~english
		 *\brief		Sets the ViewportType
		 *\param[in]	val	The new ViewportType
		 *\~french
		 *\brief		Définit le ViewportType
		 *\param[in]	val	Le nouveau ViewportType
		 */
		C3D_API void SetViewportType( ViewportType val );
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
			return m_frameBuffer.m_frameBuffer;
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
			return m_frameBuffer.m_colourTexture;
		}
		/**
		 *\~english
		 *\brief		Retrieves the window pixel format
		 *\return		The window pixel format
		 *\~french
		 *\brief		Récupère le format des pixels de la fenêtre
		 *\return		Le format des pixels de la fenêtre
		 */
		inline Castor::PixelFormat GetPixelFormat()const
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
		inline void SetPixelFormat( Castor::PixelFormat val )
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
		inline TargetType GetTargetType()const
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
		C3D_API void DoRender( RenderInfo & p_info, stFRAME_BUFFER & p_fb, CameraSPtr p_pCamera );

	public:
		//!\~english The render target default sampler name	\~french Le nom du sampler par défaut pour la cible de rendu
		C3D_API static const Castor::String DefaultSamplerName;

	protected:
		//!\~english	The render target type.
		//!\~french		Type de RenderTarget.
		TargetType m_eTargetType;
		//!\~english	Tells if the target is initalised.
		//!\~french		Dit si la cible est initialisée.
		bool m_initialised;
		//!\~english	The target size.
		//!\~french		Les dimensions de la cible.
		Castor::Size m_size;
		//!\~english	The technique used to render this target.
		//!\~french		La technique utilisée pour rendre cette cible.
		RenderTechniqueSPtr m_renderTechnique;
		//!\~english	Tells whether or not to use multisampling.
		//!\~french		Dit si on utilise le multisampling ou pas.
		bool m_bMultisampling;
		//!\~english	Defines the samples count if multisampling is activated.
		//!\~french		Le nombre de samples utilisés pour le multisampling.
		int32_t m_samplesCount;
		//!\~english	The scene rendered in this render target.
		//!\~french		La scène rendue par cette RenderTarget.
		SceneWPtr m_pScene;
		//!\~english	The camera used to render the scene.
		//!\~french		La caméra utilisée pour rendre la scène.
		CameraWPtr m_pCamera;
		//!\~english	Frame buffer.
		//!\~french		tampon d'image.
		stFRAME_BUFFER m_frameBuffer;
		//!\~english	The target display format.
		//!\~french		Le format des pixels de la cible.
		Castor::PixelFormat m_pixelFormat;
		//!\~english	The number of actually created render targets.
		//!\~french		Le compte de render target actuellement créées.
		static uint32_t sm_uiCount;
		//!\~english	This render target's index.
		//!\~french		L'index de cette render target.
		uint32_t m_index;
		//!\~english	The render technique name.
		//!\~french		Le nom de la technique de rendu.
		Castor::String m_techniqueName;
		//!\~english	The render technique parameters.
		//!\~french		Les paramètres de la technique de rendu.
		Parameters m_techniqueParameters;
		//!\~english	The post effects.
		//!\~french		Les effets post rendu.
		PostEffectPtrArray m_postEffects;
		//!\~english	The tone mapping implementation.
		//!\~french		L'implémentation de mappage de ton.
		ToneMappingSPtr m_toneMapping;
	};
}

#endif
