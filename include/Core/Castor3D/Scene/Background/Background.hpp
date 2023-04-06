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

namespace castor3d
{
	class SceneBackground
		: public castor::OwnedBy< Engine >
		, public castor::Named
	{
	public:
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
		*/
		C3D_API explicit SceneBackground( Engine & engine
			, Scene & scene
			, castor::String const & name
			, castor::String type
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
		*\param[in] device
		*	The current device.
		*\~french
		*\brief
		*	Fonction de nettoyage.
		*\param[in] device
		*	Le device actuel.
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
		C3D_API virtual void accept( PipelineVisitor & visitor ) = 0;
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
		*\param hdrConfigUbo
		*	The HDR configuration UBO.
		*\param sceneUbo
		*	The scene UBO.
		*\param clearColour
		*	\p true to clear the colour result.
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
		*\param hdrConfigUbo
		*	L'UBO de configuration HDR.
		*\param sceneUbo
		*	L'UBO de scène.
		*\param clearColour
		*	\p true pour vider le résultat de couleur.
		*\param[out] backgroundPass
		*	Reçoit la passe de rendu du fond.
		*/
		C3D_API virtual crg::FramePass & createBackgroundPass( crg::FramePassGroup & graph
			, RenderDevice const & device
			, ProgressBar * progress
			, VkExtent2D const & size
			, crg::ImageViewIdArray const & colour
			, crg::ImageViewIdArray const & depth
			, crg::ImageViewId const * depthObj
			, UniformBufferOffsetT< ModelBufferConfiguration > const & modelUbo
			, CameraUbo const & cameraUbo
			, HdrConfigUbo const & hdrConfigUbo
			, SceneUbo const & sceneUbo
			, bool clearColour
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
			, crg::ImageViewIdArray const & targetImage
			, uint32_t & index )const;
		/**
		*\~english
		*\brief
		*	Adds the background specific bindings.
		*\param	bindings
		*	Receives the bindings.
		*\param	index
		*	The bindings start index.
		*\~french
		*\brief
		*	Ajoute les bindings spécifiques au fond.
		*\param	bindings
		*	Reçoit les bindings.
		*\param	index
		*	L'indice de départ des bindings.
		*/
		C3D_API void addBindings( PipelineFlags const & flags
			, ashes::VkDescriptorSetLayoutBindingArray & bindings
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
		C3D_API void addDescriptors( PipelineFlags const & flags
			, ashes::WriteDescriptorSetArray & descriptorWrites
			, crg::ImageViewIdArray const & targetImage
			, uint32_t & index )const;
		/**
		*\~english
		*\return
		*	The background model name.
		*\~french
		*\return
		*	Le nom du modèle de fond.
		*/
		C3D_API virtual castor::String const & getModelName()const;
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
		C3D_API virtual bool write( castor::String const & tabs
			, castor::Path const & folder
			, castor::StringStream & stream )const = 0;
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		virtual bool isDepthSampled()const
		{
			return false;
		}

		virtual bool hasScattering()const
		{
			return false;
		}

		Scene const & getScene()const
		{
			return m_scene;
		}

		Scene & getScene()
		{
			return m_scene;
		}

		castor::String const & getType()const
		{
			return m_type;
		}

		TextureLayout const & getTexture()const
		{
			CU_Require( m_texture );
			return *m_texture;
		}

		TextureLayout & getTexture()
		{
			CU_Require( m_texture );
			return *m_texture;
		}

		ashes::Image const & getImage()const
		{
			CU_Require( m_texture );
			return m_texture->getTexture();
		}

		ashes::ImageView const & getView()const
		{
			CU_Require( m_texture );
			return m_texture->getDefaultView().getSampledView();
		}

		bool isHdr()const
		{
			return m_hdr;
		}

		bool isSRGB()const
		{
			return m_srgb;
		}

		bool hasIbl()const
		{
			return m_ibl != nullptr;
		}

		bool isInitialised()const
		{
			return m_initialised;
		}

		IblTextures const & getIbl()const
		{
			CU_Require( m_ibl );
			return *m_ibl;
		}

		Sampler const & getSampler()const
		{
			auto result = m_sampler.lock();
			CU_Require( result );
			return *result;
		}
		/**@}*/

	private:
		virtual bool doInitialise( RenderDevice const & device ) = 0;
		virtual void doCleanup() = 0;
		virtual void doCpuUpdate( CpuUpdater & updater )const = 0;
		virtual void doGpuUpdate( GpuUpdater & updater )const = 0;
		virtual void doAddPassBindings( crg::FramePass & pass
			, crg::ImageViewIdArray const & targetImage
			, uint32_t & index )const = 0;
		virtual void doAddBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings
			, uint32_t & index )const = 0;
		virtual void doAddDescriptors( ashes::WriteDescriptorSetArray & descriptorWrites
			, crg::ImageViewIdArray const & targetImage
			, uint32_t & index )const = 0;

	public:
		OnBackgroundChanged onChanged;

	protected:
		Scene & m_scene;
		castor::String m_type;
		std::atomic_bool m_initialised{ false };
		bool m_hdr{ true };
		bool m_srgb{ false };
		Texture m_textureId;
		TextureLayoutUPtr m_texture;
		SamplerResPtr m_sampler;
		std::unique_ptr< IblTextures > m_ibl;
		bool m_hasIBLSupport;
	};
}

#endif
