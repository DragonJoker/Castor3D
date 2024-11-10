/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RsmGIPass_HPP___
#define ___C3D_RsmGIPass_HPP___

#include "ReflectiveShadowMapsModule.hpp"
#include "Castor3D/Cache/CacheModule.hpp"

#include "Castor3D/Buffer/GpuBufferOffset.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapModule.hpp"
#include "Castor3D/Render/Opaque/OpaqueModule.hpp"
#include "Castor3D/Render/Passes/RenderQuad.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"
#include "Castor3D/Shader/Ubos/RsmConfigUbo.hpp"

#include <ShaderAST/Shader.hpp>

namespace castor3d
{
	class RsmGIPass
		: public castor::Named
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	graph			The runnable graph.
		 *\param[in]	previousPasses	The passes this one depends on.
		 *\param[in]	device			The GPU device.
		 *\param[in]	lightType		The light source type.
		 *\param[in]	shadowBuffer	The buffer containing the shadowing data.
		 *\param[in]	size			The render area dimensions.
		 *\param[in]	cameraUbo		The camera configuration UBO.
		 *\param[in]	depthObj		The depth and objects ID image.
		 *\param[in]	nmlOcc			The normals and occlusion image.
		 *\param[in]	smResult		The shadow map.
		 *\param[in]	result			The result.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	graph			Le runnable graph.
		 *\param[in]	previousPasses	Les passes dont celle-ci dépend.
		 *\param[in]	device			Le device GPU.
		 *\param[in]	lightType		Le type de source lumineuse.
		 *\param[in]	shadowBuffer	Le buffer contenant les données d'ombrage.
		 *\param[in]	size			Les dimensions de la zone de rendu.
		 *\param[in]	cameraUbo		L'UBO de configuration de la caméra.
		 *\param[in]	depthObj		L'image contenant les profondeurs et ID d'objets.
		 *\param[in]	nmlOcc			L'image contenant les normales et occlusions.
		 *\param[in]	smResult		La shadow map.
		 *\param[in]	result			Le résultat.
		 */
		C3D_API RsmGIPass( crg::FrameGraph & graph
			, crg::FramePassArray const & previousPasses
			, RenderDevice const & device
			, LightType lightType
			, ShadowBuffer const & shadowBuffer
			, VkExtent3D const & size
			, CameraUbo const & cameraUbo
			, crg::ImageViewId const & depthObj
			, crg::ImageViewId const & nmlOcc
			, ShadowMapResult const & smResult
			, TextureArray const & result );
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( ConfigurationVisitorBase & visitor );
		C3D_API void update( Light const & light );

		RsmConfigUbo const & getConfigUbo()const
		{
			return m_rsmConfigUbo;
		}

		GpuBufferOffsetT< castor::Point4f > const & getSamplesSsbo()const
		{
			return m_rsmSamplesSsbo;
		}

		crg::FramePass const & getPass()const
		{
			return *m_pass;
		}

	protected:
		C3D_API void doSubInitialise();
		C3D_API void doSubRecordInto( crg::RecordContext & context
			, VkCommandBuffer commandBuffer
			, uint32_t index );

	private:
		RsmConfigUbo m_rsmConfigUbo;
		GpuBufferOffsetT< castor::Point4f > m_rsmSamplesSsbo;
		ShaderModule m_vertexShader;
		ShaderModule m_pixelShader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		crg::FramePass const * m_pass;
	};
}

#endif
