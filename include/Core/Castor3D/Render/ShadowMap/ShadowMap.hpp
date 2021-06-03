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

#include <RenderGraph/RunnableGraph.hpp>

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
			ShadowMapPass * pass;
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
		C3D_API void setPass( uint32_t index
			, ShadowMapPass * pass );
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

		crg::FramePass const & getLastPass()const
		{
			return *m_lastPass;
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
		C3D_API virtual bool isUpToDate( uint32_t index )const = 0;

	protected:
		RenderDevice const & m_device;
		Scene & m_scene;
		LightType m_lightType;
		castor::String m_name;
		std::set< std::reference_wrapper< GeometryBuffers > > m_geometryBuffers;
		std::vector< PassData > m_passes;
		crg::FramePass const * m_lastPass;
		uint32_t m_count;
		ShadowMapResult m_result;
	};
}

#endif
