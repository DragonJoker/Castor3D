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
		 *\param[in]	scene		The scene.
		 *\param[in]	lightType	The light source type.
		 *\param[in]	result		The shadow map pass result.
		 *\param[in]	passes		The passes used to render map.
		 *\param[in]	count		The passes count.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	scene		La scène.
		 *\param[in]	lightType	Le type de source lumineuse.
		 *\param[in]	result		Le résultat de la passe de shadow map.
		 *\param[in]	passes		Les passes utilisées pour rendre cette texture.
		 *\param[in]	count		Le nombre de passes.
		 */
		C3D_API ShadowMap( RenderDevice const & device
			, Scene & scene
			, LightType lightType
			, ShadowMapResult result
			, std::vector< PassData > passes
			, uint32_t count );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~ShadowMap();
		/**
		 *\~english
		 *\brief		Initialises the frame buffer and light type specific data.
		 *\param[in]	device	The GPU device.
		 *\~french
		 *\brief		Initialise le frame buffer et les données spécifiques au type de source lumineuse.
		 *\param[in]	device	Le device GPU.
		 */
		C3D_API bool initialise();
		/**
		*\~english
		*\brief
		*	Visitor acceptance function.
		*\param visitor
		*	The visitor.
		*\~french
		*\brief
		*	Fonction d'acceptation de visiteur.
		*\param visitor
		*	Le visiteur.
		*/
		C3D_API void accept( PipelineVisitorBase & visitor );
		/**
		 *\~english
		 *\brief			Updates the render pass, CPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau CPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API virtual void update( CpuUpdater & updater ) = 0;
		/**
		 *\~english
		 *\brief			Updates the render pass, GPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau GPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API virtual void update( GpuUpdater & updater ) = 0;
		/**
		 *\~english
		 *\brief		Renders the light's shadow map.
		 *\param[in]	device	The GPU device.
		 *\param[out]	toWait	The semaphore from previous render pass.
		 *\param[out]	index	The map index.
		 *\~french
		 *\brief		Dessine la shadow map de la lumière.
		 *\param[in]	device	Le device GPU.
		 *\param[out]	toWait	Le sémaphore de la précédente passe de rendu.
		 *\param[out]	index	L'indice de la texture.
		 */
		C3D_API ashes::Semaphore const & render( RenderDevice const & device
			, ashes::Semaphore const & toWait
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

		ShadowMapResult const & getShadowPassResult()const
		{
			return m_result;
		}
		
		ShadowMapResult & getShadowPassResult()
		{
			return m_result;
		}

		uint32_t getCount()const
		{
			return m_count;
		}

		bool isInitialised()const
		{
			return m_initialised;
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
		C3D_API virtual void doInitialise( RenderDevice const & device ) = 0;
		/**
		 *\copydoc		castor3d::ShadowMap::render
		 */
		C3D_API virtual ashes::Semaphore const & doRender( RenderDevice const & device
			, ashes::Semaphore const & toWait
			, uint32_t index ) = 0;
		/**
		 *\~english
		 *\brief		Checks if all passes for given map index are up to date.
		 *\~french
		 *\brief		Vérifie si toutes les passes pour l'index de map donné sont à jour.
		 */
		C3D_API virtual bool isUpToDate( uint32_t index )const = 0;

	protected:
		RenderDevice const & m_device;
		Scene & m_scene;
		LightType m_lightType;
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
