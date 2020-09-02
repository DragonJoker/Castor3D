#include "Castor3D/Buffer/UniformBufferPools.hpp"

#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"

#include <ashespp/Buffer/StagingBuffer.hpp>
#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Core/Device.hpp>

#include <algorithm>

namespace castor3d
{
	UniformBufferPools::UniformBufferPools( RenderSystem & renderSystem )
		: castor::OwnedBy< RenderSystem >{ renderSystem }
		, m_matrixUboPool{ renderSystem, "Matrix" }
		, m_hdrConfigUboPool{ renderSystem, "HDRConfig" }
		, m_rsmConfigUboPool{ renderSystem, "RSMConfig" }
		, m_modelUboPool{ renderSystem, "Model" }
		, m_modelMatrixUboPool{ renderSystem, "ModelMatrix" }
		, m_billboardUboPool{ renderSystem, "Billboard" }
		, m_pickingUboPool{ renderSystem, "Picking" }
		, m_texturesUboPool{ renderSystem, "Textures" }
		, m_shadowMapUboPool{ renderSystem, "ShadowMap" }
		, m_skinningUboPool{ renderSystem, "Skinning" }
		, m_morphingUboPool{ renderSystem, "Morphing" }
	{
	}

	UniformBufferPools::~UniformBufferPools()
	{
		m_matrixUboPool.cleanup();
		m_hdrConfigUboPool.cleanup();
		m_rsmConfigUboPool.cleanup();
		m_modelUboPool.cleanup();
		m_modelMatrixUboPool.cleanup();
		m_billboardUboPool.cleanup();
		m_pickingUboPool.cleanup();
		m_texturesUboPool.cleanup();
		m_shadowMapUboPool.cleanup();
		m_skinningUboPool.cleanup();
		m_morphingUboPool.cleanup();
	}

	void UniformBufferPools::upload( ashes::CommandBuffer const & cb )const
	{
		m_matrixUboPool.upload( cb );
		m_hdrConfigUboPool.upload( cb );
		m_rsmConfigUboPool.upload( cb );
		m_modelUboPool.upload( cb );
		m_modelMatrixUboPool.upload( cb );
		m_billboardUboPool.upload( cb );
		m_pickingUboPool.upload( cb );
		m_texturesUboPool.upload( cb );
		m_shadowMapUboPool.upload( cb );
		m_skinningUboPool.upload( cb );
		m_morphingUboPool.upload( cb );
	}
}
