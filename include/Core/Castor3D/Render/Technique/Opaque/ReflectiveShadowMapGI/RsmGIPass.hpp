/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RsmGIPass_HPP___
#define ___C3D_RsmGIPass_HPP___

#include "ReflectiveShadowMapGIModule.hpp"
#include "Castor3D/Cache/CacheModule.hpp"

#include "Castor3D/Buffer/UniformBuffer.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapModule.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaqueModule.hpp"
#include "Castor3D/Render/ToTexture/RenderQuad.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"
#include "Castor3D/Shader/Ubos/RsmConfigUbo.hpp"

#include <ShaderAST/Shader.hpp>

namespace castor3d
{
	class RsmGIPass
		: public RenderQuad
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine			The engine.
		 *\param[in]	size			The render area dimensions.
		 *\param[in]	linearisedDepth	The linearised depth buffer.
		 *\param[in]	scene			The scene buffer.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine			Le moteur.
		 *\param[in]	size			Les dimensions de la zone de rendu.
		 *\param[in]	linearisedDepth	Le tampon de profondeur linéarisé.
		 *\param[in]	scene			Le tampon de scène.
		 */
		C3D_API RsmGIPass( Engine & engine
			, RenderDevice const & device
			, LightCache const & lightCache
			, LightType lightType
			, VkExtent2D const & size
			, GpInfoUbo const & gpInfo
			, OpaquePassResult const & gpResult
			, ShadowMapResult const & smResult
			, TextureUnitArray const & downscaleResult );
		/**
		 *\~english
		 *\brief		Renders the SSGI pass.
		 *\param[in]	toWait	The semaphore from the previous render pass.
		 *\~french
		 *\brief		Dessine la passe SSGI.
		 *\param[in]	toWait	Le sémaphore de la passe de rendu précédente.
		 */
		C3D_API ashes::Semaphore const & compute( ashes::Semaphore const & toWait )const;
		C3D_API CommandsSemaphore getCommands( RenderPassTimer const & timer
			, uint32_t index )const;
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( PipelineVisitorBase & visitor );
		C3D_API void update( Light const & light );

		TextureUnitArray const & getResult()const
		{
			return m_result;
		}

		RsmConfigUbo const & getConfigUbo()const
		{
			return m_rsmConfigUbo;
		}

		ashes::Buffer< castor::Point2f > const & getSamplesSsbo()const
		{
			return *m_rsmSamplesSsbo;
		}

	private:
		LightCache const & m_lightCache;
		OpaquePassResult const & m_gpResult;
		ShadowMapResult const & m_smResult;
		GpInfoUbo const & m_gpInfo;
		TextureUnitArray m_result;
		LightType m_lightType;
		ShaderModule m_vertexShader;
		ShaderModule m_pixelShader;
		RsmConfigUbo m_rsmConfigUbo;
		ashes::BufferPtr< castor::Point2f > m_rsmSamplesSsbo;
		ashes::RenderPassPtr m_renderPass;
		ashes::FrameBufferPtr m_frameBuffer;
		RenderPassTimerSPtr m_timer;
		CommandsSemaphore m_commands;
	};
}

#endif
