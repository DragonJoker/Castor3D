/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ShadowMap_H___
#define ___C3D_ShadowMap_H___

#include "ShadowMapModule.hpp"

#include "Castor3D/Material/MaterialModule.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Render/Culling/SceneCuller.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapResult.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"

#include <ashespp/Sync/Fence.hpp>
#include <ashespp/Sync/Semaphore.hpp>

namespace castor3d
{
	class ShadowMap
		: public castor::OwnedBy< Engine >
	{
	public:
		struct PassData
		{
			std::unique_ptr< MatrixUbo > matrixUbo;
			CameraSPtr camera;
			SceneCullerUPtr culler;
			ShadowMapPassSPtr pass;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\param[in]	name	The shadow map name.
		 *\param[in]	result	The shadow map pass result.
		 *\param[in]	passes	The passes used to render map.
		 *\param[in]	count	The passes count.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 *\param[in]	name	Le nom de la shadow map.
		 *\param[in]	result	Le résultat de la passe de shadow map.
		 *\param[in]	passes	Les passes utilisées pour rendre cette texture.
		 *\param[in]	count	Le nombre de passes.
		 */
		C3D_API ShadowMap( Engine & engine
			, castor::String name
			, ShadowMapResult result
			, std::vector< PassData > passes
			, uint32_t count );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~ShadowMap() = default;
		/**
		 *\~english
		 *\brief		Initialises the frame buffer and light type specific data.
		 *\~french
		 *\brief		Initialise le frame buffer et les données spécifiques au type de source lumineuse.
		 */
		C3D_API bool initialise();
		/**
		 *\~english
		 *\brief		Cleans up the frame buffer and light type specific data.
		 *\~french
		 *\brief		Nettoie le frame buffer et les données spécifiques au type de source lumineuse.
		 */
		C3D_API void cleanup();
		/**
		*\~english
		*\brief
		*	Visitor acceptance function.
		*\~french
		*\brief
		*	Fonction d'acceptation de visiteur.
		*/
		C3D_API void accept( PipelineVisitorBase & visitor );
		/**
		 *\~english
		 *\brief		Updates the passes, selecting the lights that will project shadows.
		 *\remarks		Gather the render queues, for further update.
		 *\param[in]	camera	The viewer camera.
		 *\param[out]	queues	Receives the render queues needed for the rendering of the frame.
		 *\param[out]	light	The light source.
		 *\param[out]	index	The map index.
		 *\~french
		 *\brief		Met à jour les passes, en sélectionnant les lumières qui projetteront une ombre.
		 *\remarks		Récupère les files de rendu, pour mise à jour ultérieure.
		 *\param[in]	camera	La caméra de l'observateur.
		 *\param[out]	queues	Reçoit les files de rendu nécessaires pour le dessin de la frame.
		 *\param[out]	light	La source lumineuse.
		 *\param[out]	index	L'indice de la texture.
		 */
		C3D_API virtual void update( CpuUpdater & updater ) = 0;
		/**
		 *\~english
		 *\brief		Updates VRAM data.
		 *\param[out]	index	The map index.
		 *\~french
		 *\brief		Met à jour les données VRAM.
		 *\param[out]	index	L'indice de la texture.
		 */
		C3D_API virtual void update( GpuUpdater & updater ) = 0;
		/**
		 *\~english
		 *\brief		Renders the light's shadow map.
		 *\param[out]	toWait	The semaphore from previous render pass.
		 *\param[out]	index	The map index.
		 *\~french
		 *\brief		Dessine la shadow map de la lumière.
		 *\param[out]	toWait	Le sémaphore de la précédente passe de rendu.
		 *\param[out]	index	L'indice de la texture.
		 */
		C3D_API ashes::Semaphore const & render( ashes::Semaphore const & toWait
			, uint32_t index );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		C3D_API ashes::VkClearValueArray const & getClearValues()const;
		C3D_API ashes::Sampler const & getSampler( SmTexture texture
			, uint32_t index = 0u )const;
		C3D_API virtual ashes::ImageView const & getView( SmTexture texture
			, uint32_t index = 0u )const;

		inline ShadowMapResult const & getShadowPassResult()const
		{
			return m_result;
		}
		
		inline ShadowMapResult & getShadowPassResult()
		{
			return m_result;
		}

		inline uint32_t getCount()const
		{
			return m_count;
		}
		/**@}*/
	public:
		static constexpr TextureFlags textureFlags{ TextureFlag::eOpacity
			| TextureFlag::eNormal
			| TextureFlag::eAlbedo
			| TextureFlag::eSpecular
			| TextureFlag::eGlossiness
			| TextureFlag::eEmissive
			| TextureFlag::eTransmittance };

	private:
		/**
		 *\copydoc		castor3d::ShadowMap::initialise
		 */
		C3D_API virtual void doInitialise() = 0;
		/**
		 *\copydoc		castor3d::ShadowMap::cleanup
		 */
		C3D_API virtual void doCleanup() = 0;
		/**
		 *\copydoc		castor3d::ShadowMap::render
		 */
		C3D_API virtual ashes::Semaphore const & doRender( ashes::Semaphore const & toWait
			, uint32_t index ) = 0;
		/**
		 *\~english
		 *\brief		Checks if all passes for given map index are up to date.
		 *\~french
		 *\brief		Vérifie si toutes les passes pour l'index de map donné sont à jour.
		 */
		C3D_API virtual bool isUpToDate( uint32_t index )const = 0;

	protected:
		castor::String m_name;
		ashes::FencePtr m_fence;
		std::set< std::reference_wrapper< GeometryBuffers > > m_geometryBuffers;
		std::vector< PassData > m_passes;
		uint32_t m_count;
		ashes::SemaphorePtr m_finished;
		ShadowMapResult m_result;
		bool m_initialised{ false };
	};
}

#endif
