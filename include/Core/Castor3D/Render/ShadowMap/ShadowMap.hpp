/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ShadowMap_H___
#define ___C3D_ShadowMap_H___

#include "ShadowMapModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/Passes/PassesModule.hpp"

#include "Castor3D/Material/MaterialModule.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Render/Culling/SceneCuller.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapResult.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"

#include <RenderGraph/FrameGraph.hpp>
#include <RenderGraph/RunnableGraph.hpp>

namespace castor3d
{
	class ShadowMap
		: public castor::OwnedBy< Engine >
	{
	public:
		struct PassData
		{
			explicit PassData( SceneCullerUPtr culler )
				: ownCuller{ std::move( culler ) }
				, culler{ ownCuller.get() }
				, pass{ nullptr }
			{
			}

			PassData( ViewportUPtr viewport
				, SceneCullerUPtr culler )
				: viewport{ std::move( viewport ) }
				, ownCuller{ std::move( culler ) }
				, culler{ ownCuller.get() }
				, pass{ nullptr }
			{
			}

			explicit PassData( SceneCuller * culler )
				: ownCuller{ nullptr }
				, culler{ culler }
				, pass{ nullptr }
			{
			}

			PassData()
				: ownCuller{ nullptr }
				, culler{ nullptr }
				, pass{ nullptr }
			{
			}

			ViewportUPtr viewport;
			FrustumUPtr frustum;
			SceneCullerUPtr ownCuller;
			SceneCuller * culler;
			ShadowMapPass * pass;
		};
		using PassDataPtr = std::unique_ptr< PassData >;

		struct Passes
		{
			std::vector< PassDataPtr > passes;
			std::vector< std::unique_ptr< crg::FrameGraph > > graphs;
			std::vector< crg::RunnableGraphPtr > runnables;
			std::vector< GaussianBlurUPtr > blurs;
		};

		struct AllPasses
		{
			std::vector< std::unique_ptr< CameraUbo > > cameraUbos;
			std::vector< CameraUPtr > cameras;
			Passes staticNodes;
			Passes otherNodes;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	resources	The render graph resources handler.
		 *\param[in]	device		The GPU device.
		 *\param[in]	scene		The scene.
		 *\param[in]	lightType	The light source type.
		 *\param[in]	createFlags	The result image create flags.
		 *\param[in]	size		The result image dimensions.
		 *\param[in]	layerCount	The result image layers count.
		 *\param[in]	count		The passes count.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	resources	Le gestionnaire de ressources du render graph.
		 *\param[in]	device		Le device GPU.
		 *\param[in]	scene		La scène.
		 *\param[in]	lightType	Le type de source lumineuse.
		 *\param[in]	createFlags	Les flags de créqation de l'image résultat.
		 *\param[in]	size		Les dimensions de l'image résultat.
		 *\param[in]	layerCount	Le nombres de layers de l'image résultat.
		 *\param[in]	count		Le nombre de passes.
		 */
		C3D_API ShadowMap( crg::ResourcesCache & resources
			, RenderDevice const & device
			, Scene & scene
			, LightType lightType
			, VkImageCreateFlags createFlags
			, castor::Size const & size
			, uint32_t layerCount
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
		C3D_API void accept( ConfigurationVisitorBase & visitor );
		/**
		 *\~english
		 *\brief			Updates the render pass, CPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau CPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( CpuUpdater & updater );
		/**
		 *\~english
		 *\brief			Updates the render pass, GPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau GPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( GpuUpdater & updater );
		/**
		 *\~english
		 *\brief		Renders the shadow map.
		 *\param[in]	toWait	The semaphores to wait.
		 *\param[in]	queue	The queue receiving the render commands.
		 *\param[in]	index	The index of the layer to update.
		 *\return		The semaphores signaled by this render.
		 *\~french
		 *\brief		Dessine la texture d'ombres.
		 *\param[in]	toWait	Les sémaphores à attendre.
		 *\param[in]	queue	The queue recevant les commandes de dessin.
		 *\param[in]	index	L'index de la layer à dessiner.
		 *\return		Les sémaphores signalés par ce dessin.
		 */
		C3D_API crg::SemaphoreWaitArray render( crg::SemaphoreWaitArray const & toWait
			, ashes::Queue const & queue
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
		C3D_API virtual crg::ImageViewId getView( SmTexture texture
			, uint32_t index = 0u )const;
		C3D_API virtual crg::ImageViewIdArray getViews( SmTexture texture
			, uint32_t index = 0u )const;

		ShadowMapResult const & getShadowPassResult( bool isStatic )const
		{
			return isStatic ? m_staticsResult : m_result;
		}

		ShadowMapResult & getShadowPassResult( bool isStatic )
		{
			return isStatic ? m_staticsResult : m_result;
		}

		Scene & getScene()const
		{
			return m_scene;
		}

		uint32_t getCount()const
		{
			return m_count;
		}
		/**@}*/

	protected:
		C3D_API void doRegisterGraphIO( crg::FrameGraph & graph
			, bool vsm
			, bool rsm
			, bool isStatic )const;
		C3D_API bool doEnableCopyStatic( uint32_t index )const;
		C3D_API bool doEnableBlur( uint32_t index )const;

	private:
		crg::FramePassArray doCreatePasses( crg::FrameGraph & graph
			, crg::FramePassArray const & previousPasses
			, uint32_t index
			, bool vsm
			, bool rsm
			, bool isStatic
			, Passes & passes );

		C3D_API virtual crg::FramePassArray doCreatePass( crg::FrameGraph & graph
			, crg::FramePassArray const & previousPasses
			, uint32_t index
			, bool vsm
			, bool rsm
			, bool isStatic
			, Passes & passes ) = 0;
		C3D_API virtual bool doIsUpToDate( uint32_t index
			, Passes const & passes )const = 0;
		C3D_API virtual void doSetUpToDate( uint32_t index
			, Passes & passes ) = 0;
		C3D_API virtual void doUpdate( CpuUpdater & updater
			, Passes & passes ) = 0;
		C3D_API virtual void doUpdate( GpuUpdater & updater
			, Passes & passes ) = 0;
		C3D_API virtual uint32_t doGetMaxCount()const = 0;

	protected:
		RenderDevice const & m_device;
		crg::ResourcesCache & m_resources;
		Scene & m_scene;
		castor::String m_name;
		LightType m_lightType;
		ShadowMapResult m_staticsResult;
		ShadowMapResult m_result;
		uint32_t m_count;
		std::set< std::reference_wrapper< GeometryBuffers > > m_geometryBuffers;
		std::array< AllPasses, 4u > m_passes;
		uint32_t m_passesIndex{};
	};
}

#endif
