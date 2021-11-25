/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SceneBackground_H___
#define ___C3D_SceneBackground_H___

#include "BackgroundModule.hpp"

#include "Castor3D/Render/PBR/IblTextures.hpp"
#include "Castor3D/Shader/Ubos/HdrConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelUbo.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"

#include <ashespp/Buffer/VertexBuffer.hpp>
#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/Pipeline/GraphicsPipeline.hpp>
#include <ashespp/Pipeline/PipelineLayout.hpp>
#include <ashespp/Sync/Semaphore.hpp>

namespace castor3d
{
	class SceneBackground
		: public castor::OwnedBy< Engine >
		, public castor::Named
	{
	public:
		enum DescriptorIdx : uint32_t
		{
			MtxUboIdx = 0u,
			MdlMtxUboIdx = 1u,
			HdrCfgUboIdx = 2u,
			SceneUboIdx = 3u,
			SkyBoxImgIdx = 4u,
		};

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
			, BackgroundType type );
		/**
		*\~english
		*\brief
		*	Destructor.
		*\~french
		*\brief
		*	Destructeur.
		*/
		C3D_API virtual ~SceneBackground() = default;
		/**
		*\~english
		*\brief
		*	Initialisation function.
		*\param[in] device
		*	The current device.
		*\param[in] hdrConfigUbo
		*	The HDR configuration UBO.
		*\return
		*	\p true if ok.
		*\~french
		*\brief
		*	Fonction d'initialisation.
		*\param[in] device
		*	Le device actuel.
		*\param[in] hdrConfigUbo
		*	L'UBO de configuration HDR.
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
		C3D_API void cleanup( RenderDevice const & device );
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
		*\return
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
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		Scene const & getScene()const
		{
			return m_scene;
		}

		Scene & getScene()
		{
			return m_scene;
		}

		BackgroundType getType()const
		{
			return m_type;
		}

		Texture const & getTextureId()const
		{
			return m_textureId;
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

	public:
		OnBackgroundChanged onChanged;

	protected:
		Scene & m_scene;
		BackgroundType m_type;
		std::atomic_bool m_initialised{ false };
		bool m_hdr{ true };
		bool m_srgb{ false };
		Texture m_textureId;
		TextureLayoutSPtr m_texture;
		SamplerResPtr m_sampler;
		std::unique_ptr< IblTextures > m_ibl;
	};
}

#endif
