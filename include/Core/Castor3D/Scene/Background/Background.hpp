/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SceneBackground_H___
#define ___C3D_SceneBackground_H___

#include "BackgroundModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include "Castor3D/Render/Passes/BackgroundPassBase.hpp"
#include "Castor3D/Render/PBR/IblTextures.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"

namespace c3d
{
	class SceneBackground
		: public OwnedBy< Engine >
		, public Named
	{
	public:
		static uint32_t constexpr VisiblePassIndex = 0u;
		static uint32_t constexpr IrradiancePassIndex = 1u;
		static uint32_t constexpr HiddenPassIndex = 2u;
		static uint32_t constexpr PassCount = 3u;

		OnBackgroundChanged onChanged;
		/**
		*\~english
		*\brief
		*	Constructor.
		*\param engine
		*	The engine.
		*\param scene
		*	The parent scene.
		*\param name
		*	The background name.
		*\param type
		*	The background type.
		*\param hasIBLSupport
		*	Tells if the background supports IBL.
		*\~french
		*\brief
		*	Constructeur.
		*\param engine
		*	Le moteur.
		*\param scene
		*	La scène parente.
		*\param name
		*	Le nom du fond.
		*\param type
		*	Le type de fond.
		*\param hasIBLSupport
		*	Dit si le fond supporte l'IBL.
		*/
		C3D_API explicit SceneBackground( Engine & engine
			, Scene & scene
			, String const & name
			, String type
			, bool hasIBLSupport );
		/**
		*\~english
		*\brief
		*	Destructor.
		*\~french
		*\brief
		*	Destructeur.
		*/
		C3D_API virtual ~SceneBackground();
		/**
		*\~english
		*\brief
		*	Initialisation function.
		*\param[in] device
		*	The current device.
		*\return
		*	\p true if ok.
		*\~french
		*\brief
		*	Fonction d'initialisation.
		*\param[in] device
		*	Le device actuel.
		*\return
		*	\p true if ok.
		*/
		C3D_API bool initialise( RenderDevice const & device );
		/**
		*\~english
		*\brief
		*	Cleanup function.
		*\~french
		*\brief
		*	Fonction de nettoyage.
		*/
		C3D_API void cleanup();
		/**
		*\~english
		*\brief
		*	Updates the background, CPU side.
		*\param[in] updater
		*	The update data.
		*\~french
		*\brief
		*	Met à jour le fond, niveau CPU.
		*\param[in] updater
		*	Les données d'update.
		*/
		C3D_API void update( CpuUpdater & updater )const;
		/**
		*\~english
		*\brief
		*	Updates the background, GPU side.
		*\param[in] updater
		*	The update data.
		*\~french
		*\brief
		*	Met à jour le fond, niveau GPU.
		*\param[in] updater
		*	Les données d'update.
		*/
		C3D_API void update( GpuUpdater & updater )const;
		/**
		 *\~english
		 *\brief		Uploads image data to the texture buffer.
		 *\~french
		 *\brief		Upload les données d'image dans le buffer de la texture.
		 */
		C3D_API void upload( UploadData & uploader );
		/**
		*\~english
		*\brief
		*	Notifies a change to apply on GPU for the background.
		*\~french
		*\brief
		*	Notifie un changement affectant le GPU pour le fond.
		*/
		C3D_API void notifyChanged();
		/**
		*\~english
		*\brief
		*	Visitor acceptance function.
		*\param	visitor
		*	The visitor.
		*\~french
		*\brief
		*	Fonction d'acceptation de visiteur.
		*\param visitor
		*	Le visiteur.
		*/
		C3D_API virtual void accept( BackgroundVisitor & visitor ) = 0;
		/**
		*\~english
		*\brief
		*	Visitor acceptance function.
		*\param	visitor
		*	The visitor.
		*\~french
		*\brief
		*	Fonction d'acceptation de visiteur.
		*\param visitor
		*	Le visiteur.
		*/
		C3D_API virtual void accept( ConfigurationVisitorBase & visitor ) = 0;
		/**
		*\~english
		*\brief
		*	Creates the background render pass.
		*\param graph
		*	The runnable graph.
		*\param device
		*	The GPU device.
		*\param progress
		*	The optional progress bar.
		*\param size
		*	The render area dimensions.
		*\param colour
		*	The colour result.
		*\param depth
		*	The optional depth image.
		*\param depthObj
		*	The optional depths and objects image.
		*\param modelUbo
		*	The model UBO.
		*\param cameraUbo
		*	The matrix UBO.
		*\param renderUbo
		*	The render configuration UBO.
		*\param sceneUbo
		*	The scene UBO.
		*\param clearColour
		*	\p true to clear the colour result.
		*\param clearDepth
		*	\p true to clear the depth result.
		*\param forceVisible
		*	\p true to force display of background.
		*\param[out] backgroundPass
		*	Receives the background pass.
		*\~french
		*\brief
		*	Crée la passe de rendu du fond.
		*\param graph
		*	Le runnable graph.
		*\param device
		*	Le device GPU.
		*\param progress
		*	La barre de progression, optionnelle.
		*\param size
		*	Les dimensions de la zone de rendu.
		*\param colour
		*	Le résultat de couleur.
		*\param depth
		*	L'image de profondeur, optionnelle.
		*\param depthObj
		*	L'image de profondeurs de d'objets, optionnelle.
		*\param modelUbo
		*	L'UBO de modèle.
		*\param cameraUbo
		*	L'UBO de matrices.
		*\param renderUbo
		*	L'UBO de configuration du rendu.
		*\param sceneUbo
		*	L'UBO de scène.
		*\param clearColour
		*	\p true pour vider le résultat de couleur.
		*\param clearDepth
		*	\p true pour vider le résultat de profondeur.
		*\param forceVisible
		*	\p true pour forcer l'affichage du fond.
		*\param[out] backgroundPass
		*	Reçoit la passe de rendu du fond.
		*/
		C3D_API virtual void createBackgroundPass( crg::FramePassGroup & graph
			, RenderDevice const & device
			, ProgressBar * progress
			, Extent2D const & size
			, Texture & colour
			, Texture * depth
			, Texture const * depthObj
			, UniformBufferOffsetT< ModelBufferConfiguration > const & modelUbo
			, CameraUbo const & cameraUbo
			, RenderUbo const & renderUbo
			, SceneUbo const & sceneUbo
			, bool clearColour
			, bool clearDepth
			, bool forceVisible
			, BackgroundPassBase *& backgroundPass );
		/**
		*\~english
		*\brief
		*	Adds the background specific bindings to a frame pass.
		*\param	pass
		*	Receives the bindings.
		*\param	targetImage
		*	The image this pass renders to.
		*\param	index
		*	The bindings start index.
		*\~french
		*\brief
		*	Ajoute les bindings spécifiques au fond à une passe de frame.
		*\param	pass
		*	Reçoit les bindings.
		*\param	targetImage
		*	L'image dans laquelle cette passe fait son rendu.
		*\param	index
		*	L'indice de départ des bindings.
		*/
		C3D_API void addPassBindings( crg::FramePass & pass
			, Texture * targetImage
			, uint32_t & index )const;
		/**
		*\~english
		*\brief
		*	Adds the background specific bindings.
		*\param	bindings
		*	Receives the bindings.
		*\param	shaderStages
		*	The impacted shader stages.
		*\param	index
		*	The bindings start index.
		*\~french
		*\brief
		*	Ajoute les bindings spécifiques au fond.
		*\param	bindings
		*	Reçoit les bindings.
		*\param	shaderStages
		*	Les shader stages impactés.
		*\param	index
		*	L'indice de départ des bindings.
		*/
		C3D_API void addLayoutBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings
			, VkShaderStageFlags shaderStages
			, uint32_t & index )const;
		/**
		*\~english
		*\brief
		*	Adds the background specific descriptors.
		*\param	descriptorWrites
		*	Receives the descriptors.
		*\param	targetImage
		*	The image this pass renders to.
		*\param	index
		*	The descriptors start index.
		*\~french
		*\brief
		*	Ajoute les descripteurs spécifiques au fond.
		*\param	descriptorWrites
		*	Reçoit les descripteurs.
		*\param	targetImage
		*	L'image dans laquelle cette passe fait son rendu.
		*\param	index
		*	L'indice de départ des descripteurs.
		*/
		C3D_API void addDescriptorWrites( ashes::WriteDescriptorSetArray & descriptorWrites
			, Texture * targetImage
			, uint32_t & index )const;
		/**
		*\~english
		*\return
		*	The background model name.
		*\~french
		*\return
		*	Le nom du modèle de fond.
		*/
		C3D_API virtual String const & getModelName()const;
		C3D_API BackgroundModelID getModelID()const;
		/**
		*\~english
		*\brief
		*	Writes the background to a text stream.
		*\param	tabs
		*	The current indentation.
		*\param	folder
		*	The working folder.
		*\param	stream
		*	The text stream.
		*\~french
		*\brief
		*	Ecrit le fond dans un flux texte.
		*\param	tabs
		*	L'indentation actuelle.
		*\param	folder
		*	Le dossier de travail.
		*\param	stream
		*	Le flux.
		*/
		C3D_API virtual bool write( String const & tabs
			, Path const & folder
			, StringStream & stream )const = 0;
		/**
		*\~english
		*name
		*	Mutators.
		*\~french
		*name
		*	Mutateurs.
		*/
		/**@{*/
		void setVisible( bool v )noexcept
		{
			m_visible = v;
			m_passIndex = ( m_visible
				? VisiblePassIndex
				: ( isIrradianceShown()
					? IrradiancePassIndex
					: HiddenPassIndex ) );
		}
		/**
		*\~english
		*name
		*	Mutators.
		*\~french
		*name
		*	Mutateurs.
		*/
		/**@{*/
		void showIrradiance( bool v )noexcept
		{
			m_showIrradiance = v;
			m_visible = false;
			m_passIndex = ( m_visible
				? VisiblePassIndex
				: ( m_showIrradiance
					? IrradiancePassIndex
					: HiddenPassIndex ) );
		}
		/**@}*/
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		virtual bool isDepthSampled()const noexcept
		{
			return false;
		}

		virtual bool hasScattering()const noexcept
		{
			return false;
		}

		Scene & getScene()const noexcept
		{
			return m_scene;
		}

		String const & getType()const noexcept
		{
			return m_type;
		}

		TextureLayout & getTexture()const noexcept
		{
			CU_Require( m_texture );
			return *m_texture;
		}

		ashes::Image const & getImage()const
		{
			CU_Require( m_texture );
			return m_texture->getTexture();
		}

		bool isHdr()const noexcept
		{
			return m_hdr;
		}

		bool isSRGB()const noexcept
		{
			return m_srgb;
		}

		bool hasIbl()const noexcept
		{
			return m_ibl != nullptr;
		}

		bool isInitialised()const noexcept
		{
			return m_initialised;
		}

		bool isVisible()const noexcept
		{
			return m_visible;
		}

		bool isIrradianceShown()const noexcept
		{
			return m_hasIBLSupport && m_showIrradiance;
		}

		IblTextures const & getIbl()const noexcept
		{
			CU_Require( m_ibl );
			return *m_ibl;
		}

		Sampler const & getSampler()const noexcept
		{
			CU_Require( m_sampler );
			return *m_sampler;
		}

		uint32_t const & getPassIndex( bool forceVisible )const noexcept
		{
			return forceVisible
				? VisiblePassIndex
				: m_passIndex;
		}
		/**@}*/

	protected:
		Scene & m_scene;
		String m_type;
		std::atomic_bool m_initialised{ false };
		bool m_hdr{ true };
		bool m_srgb{ false };
		Texture m_textureId;
		TextureLayoutUPtr m_texture;
		SamplerObs m_sampler{};
		RawUniquePtr< IblTextures > m_ibl;
		bool m_hasIBLSupport;
		bool m_visible{ true };
		bool m_showIrradiance{ false };
		uint32_t m_passIndex{ 0u };
		bool m_needsUpload{};

		C3D_API static PxBufferBaseUPtr adaptBuffer( PxBufferBase const & buffer
			, String const & name
			, bool generateMips );
		C3D_API static ImageUPtr loadImage( Engine & engine
			, String const & name
			, Path const & folder
			, Path const & relative
			, bool generateMips );

	private:
		virtual bool doInitialise( RenderDevice const & device ) = 0;
		virtual void doCleanup() = 0;
		virtual void doCpuUpdate( CpuUpdater & updater )const = 0;
		virtual void doGpuUpdate( GpuUpdater & updater )const = 0;
		virtual void doUpload( UploadData & uploader ) = 0;
		virtual void doAddPassBindings( crg::FramePass & pass
			, Texture * targetImage
			, uint32_t & index )const = 0;
		virtual void doAddLayoutBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings
			, VkShaderStageFlags shaderStages
			, uint32_t & index )const = 0;
		virtual void doAddDescriptorWrites( ashes::WriteDescriptorSetArray & descriptorWrites
			, Texture * targetImage
			, uint32_t & index )const = 0;
	};
}

#endif
