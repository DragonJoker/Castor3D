/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderWindow_H___
#define ___C3D_RenderWindow_H___

#include "Event/Frame/FrameListener.hpp"
#include "RenderToTexture/RenderQuad.hpp"
#include "Shader/Ubos/MatrixUbo.hpp"

#include <Design/Named.hpp>
#include <Design/OwnedBy.hpp>
#include <Graphics/Size.hpp>

#include <Core/Device.hpp>
#include <Core/SwapChain.hpp>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Render window representation.
	\remark		Manages a window where you can render a scene.
	\~french
	\brief		Implémentation d'une fenêtre de rendu.
	\remark		Gère une fenêtre dans laquelle une scène peut être rendue
	*/
	class RenderWindow
		: public castor::OwnedBy< Engine >
		, public castor::Named
	{
	public:
		/*!
		\author		Sylvain DOREMUS
		\date		14/02/2010
		\~english
		\brief		RenderTarget loader.
		\~english
		\brief		Loader de RenderTarget.
		*/
		class TextWriter
			: public castor::TextWriter< RenderWindow >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			C3D_API explicit TextWriter( castor::String const & tabs );
			/**
			 *\~english
			 *\brief		Writes a render window into a text file.
			 *\param[in]	window	the render window.
			 *\param[in]	file	the file.
			 *\~french
			 *\brief		Ecrit une fenêtre de rendu dans un fichier texte.
			 *\param[in]	window	La fenêtre de rendu.
			 *\param[in]	file	Le fichier.
			 */
			C3D_API bool operator()( RenderWindow const & window, castor::TextFile & file )override;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	engine	The engine.
		 *\param[in]	name	The window name.
		 *\~french
		 *\brief		Constructor
		 *\param[in]	engine	Le moteur.
		 *\param[in]	name	Le nom de la fenêtre.
		 */
		C3D_API RenderWindow( castor::String const & name
			, Engine & engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~RenderWindow();
		/**
		 *\~english
		 *\brief		Sets the handle, initialises the window.
		 *\param[in]	size	The window size.
		 *\param[in]	handle	The handle.
		 *\return		\p false if any problem occured
		 *\~french
		 *\brief		Définit l'identifiant de la fenêtre, initialise la fenêtre.
		 *\param[in]	size	Les dimensions de la fenêtre.
		 *\param[in]	handle	Le handle.
		 *\return		\p false si un problème quelconque a été rencontré.
		 */
		C3D_API bool initialise( castor::Size const & size
			, renderer::WindowHandle && handle );
		/**
		 *\~english
		 *\brief		Cleans up the instance.
		 *\~french
		 *\brief		Nettoie l'instance.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Renders one frame.
		 *\param[in]	force	Forces the rendering.
		 *\~english
		 *\brief		Renders one frame.
		 *\param[in]	force	Dit si on force le rendu.
		 */
		C3D_API void render( bool force );
		/**
		 *\~english
		 *\brief		Resizes the window.
		 *\param[in]	x, y	The new size.
		 *\~french
		 *\brief		Redimensionne la fenêtre.
		 *\param[in]	x, y	Les nouvelles dimensions.
		 */
		C3D_API void resize( uint32_t x, uint32_t y );
		/**
		 *\~english
		 *\brief		Resizes the window.
		 *\param[in]	size	The new size.
		 *\~french
		 *\brief		Redimensionne la fenêtre.
		 *\param[in]	size	Les nouvelles dimensions.
		 */
		C3D_API void resize( castor::Size const & size );
		/**
		 *\~english
		 *\brief		Sets the camera.
		 *\param[in]	camera	The camera.
		 *\~french
		 *\brief		Définit la caméra.
		 *\param[in]	camera	La caméra.
		 */
		C3D_API void setCamera( CameraSPtr camera );
		/**
		 *\~english
		 *\brief		Changes fullscreen status.
		 *\param[in]	value	The new status.
		 *\~french
		 *\brief		Change le statut de plein écran.
		 *\param[in]	value	Le nouveau statut.
		 */
		C3D_API void enableFullScreen( bool value );
		/**
		 *\~english
		 *\return		The scene.
		 *\~french
		 *\return		La scène.
		 */
		C3D_API SceneSPtr getScene()const;
		/**
		 *\~english
		 *\return		The camera.
		 *\~french
		 *\return		La caméra.
		 */
		C3D_API CameraSPtr getCamera()const;
		/**
		 *\~english
		 *\return		The ViewportType.
		 *\~french
		 *\return		Le ViewportType.
		 */
		C3D_API ViewportType getViewportType()const;
		/**
		 *\~english
		 *\brief		Sets the ViewportType.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit le ViewportType.
		 *\param[in]	value	La nouvelle valeur.
		 */
		C3D_API void setViewportType( ViewportType value );
		/**
		 *\~english
		 *\brief		Sets the Scene.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit la Scene.
		 *\param[in]	value	La nouvelle valeur.
		 */
		C3D_API void setScene( SceneSPtr value );
		/**
		 *\~english
		 *\return		The window dimensions.
		 *\~french
		 *\return		Les dimensions de la fenêtre.
		 */
		C3D_API castor::Size getSize()const;
		/**
		 *\~english
		 *\return		\p true if stereo is used.
		 *\~french
		 *\return		\p true si le rendu stéréo est utilisé.
		 */
		C3D_API bool isUsingStereo()const;
		/**
		 *\~english
		 *\brief		Defines the stereo status.
		 *\param[in]	stereo	\p true if stereo is to be used.
		 *\~french
		 *\brief		Définit le statut d'utilisation stéréo.
		 *\param[in]	stereo	\p true si le rendu stéréo est à utiliser.
		 */
		C3D_API void setStereo( bool stereo );
		/**
		 *\~english
		 *\brief		Retrieves the intra ocular distance
		 *\return		The intra ocular distance
		 *\~french
		 *\brief		Récupère la distance inter oculaire
		 *\return		La distance inter oculaire
		 */
		C3D_API real getIntraOcularDistance()const;
		/**
		 *\~english
		 *\brief		Defines the intra ocular distance.
		 *\param[in]	intraOcularDistance	The intra ocular distance.
		 *\~french
		 *\brief		Définit la distance inter oculaire.
		 *\param[in]	intraOcularDistance	La distance inter oculaire.
		 */
		C3D_API void setIntraOcularDistance( real intraOcularDistance );
		/**
		 *\~english
		 *\return		The window pixel format.
		 *\~french
		 *\return		Le format des pixels de la fenêtre.
		 */
		C3D_API renderer::Format getPixelFormat()const;
		/**
		 *\~english
		 *\brief		Sets the window pixel format.
		 *\param[in]	value	The new window pixel format.
		 *\~french
		 *\brief		Définit le format des pixels de la fenêtre.
		 *\param[in]	value	Le nouveau format des pixels de la fenêtre.
		 */
		C3D_API void setPixelFormat( renderer::Format value );
		/**
		 *\~english
		 *\brief		Adds a scene rendered through this technique.
		 *\param[in]	scene	The scene.
		 *\param[in]	camera	The camera through which the scene is viewed.
		 *\~french
		 *\brief		Ajoute une scène dessinée via cette technique.
		 *\param[in]	scene	La scène.
		 *\param[in]	camera	La caméra à travers laquelle la scène est vue.
		 */
		C3D_API void addPickingScene( Scene & scene );
		/**
		 *\~english
		 *\brief		Picks a geometry at given mouse position.
		 *\param[in]	position	The position in the pass.
		 *\return		PickNodeType::eNone if nothing was picked.
		 *\~french
		 *\brief		Sélectionne la géométrie à la position de souris donnée.
		 *\param[in]	position	La position dans la passe.
		 *\return		PickNodeType si rien n'a été pické.
		 */
		C3D_API PickNodeType pick( castor::Position const & position );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		inline uint32_t getIndex()const
		{
			return m_index;
		}

		inline bool isInitialised()const
		{
			return m_initialised;
		}

		inline FrameListenerSPtr getListener()const
		{
			return m_listener.lock();
		}

		inline renderer::Device const & getDevice()const
		{
			REQUIRE( m_device );
			return *m_device;
		}

		inline RenderTargetSPtr getRenderTarget()const
		{
			return m_renderTarget.lock();
		}

		inline bool isVSyncEnabled()const
		{
			return m_vsync;
		}

		inline bool isFullscreen()const
		{
			return m_fullscreen;
		}

		inline castor::PxBufferBaseSPtr getSavedFrame()const
		{
			return m_saveBuffer;
		}

		C3D_API GeometrySPtr getPickedGeometry()const;
		C3D_API BillboardBaseSPtr getPickedBillboard()const;
		C3D_API SubmeshSPtr getPickedSubmesh()const;
		C3D_API uint32_t getPickedFace()const;
		/**@}*/
		/**
		*\~english
		*name
		*	Mutators.
		*\~french
		*name
		*	Mutateurs.
		*/
		/**@{*/
		inline void setRenderTarget( RenderTargetSPtr value )
		{
			m_renderTarget = value;
		}

		inline void setDevice( renderer::DevicePtr value )
		{
			m_device = std::move( value );
		}

		inline void enableVSync( bool value )
		{
			m_vsync = value;
		}

		inline void setFullscreen( bool value )
		{
			m_fullscreen = value;
		}

		inline void enableSaveFrame()
		{
			m_toSave = true;
		}
		/**@}*/

	private:
		void doCreateProgram();
		void doCreateSwapChainDependent();
		bool doPrepareFrames();
		void doResetSwapChain();
		void doCleanup( bool enableDevice );

	private:
		static uint32_t s_nbRenderWindows;
		uint32_t m_index;
		renderer::DevicePtr m_device;
		renderer::SwapChainPtr m_swapChain;
		renderer::RenderPassPtr m_renderPass;
		renderer::StagingBufferPtr m_stagingBuffer;
		renderer::CommandBufferPtr m_transferCommandBuffer;
		std::vector< renderer::FrameBufferPtr > m_frameBuffers;
		renderer::CommandBufferPtrArray m_commandBuffers;
		renderer::SignalConnection< renderer::SwapChain::OnReset > m_swapChainReset;
		renderer::ShaderStageStateArray m_program;
		RenderQuadUPtr m_renderQuad;
		RenderTargetWPtr m_renderTarget;
		OverlayRendererSPtr m_overlayRenderer;
		FrameListenerWPtr m_listener;
		bool m_initialised{ false };
		bool m_vsync{ false };
		bool m_fullscreen{ false };
		castor::Size m_size;
		bool m_toSave{ false };
		bool m_dirty{ true };
		castor::PxBufferBaseSPtr m_saveBuffer;
		PickingPassSPtr m_pickingPass;
	};
}

#endif
