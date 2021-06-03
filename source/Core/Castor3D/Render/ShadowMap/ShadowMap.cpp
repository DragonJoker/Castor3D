#include "Castor3D/Render/ShadowMap/ShadowMap.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapPass.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <CastorUtils/Graphics/RgbaColour.hpp>

#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Sync/Semaphore.hpp>
#include <ashespp/Image/ImageView.hpp>

CU_ImplementCUSmartPtr( castor3d, ShadowMap )

namespace castor3d
{
	ShadowMap::ShadowMap( RenderDevice const & device
		, Scene & scene
		, LightType lightType
		, ShadowMapResult result
		, uint32_t count )
		: OwnedBy< Engine >{ *scene.getEngine() }
		, m_device{ device }
		, m_scene{ scene }
		, m_lightType{ lightType }
		, m_name{ castor::string::snakeToCamelCase( getName( lightType ) ) + "SM" }
		, m_result{ std::move( result ) }
		, m_count{ count }
	{
	}

	void ShadowMap::accept( PipelineVisitorBase & visitor )
	{
		for ( uint32_t i = 1u; i < uint32_t( SmTexture::eCount ); ++i )
		{
			uint32_t index = 0u;

			for ( auto & view : m_result[SmTexture( i )].subViews )
			{
				visitor.visit( m_name + getName( SmTexture( i ) ) + cuT( "L" ) + castor::string::toString( index++ )
					, view
					, ( ashes::isDepthOrStencilFormat( view.data->info.format )
						? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
						: VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL )
					, TextureFactors{}.invert( true ) );
			}
		}
	}

	void ShadowMap::setPass( uint32_t index
		, ShadowMapPass * pass )
	{
		m_passes[index].pass = pass;
	}

	ashes::VkClearValueArray const & ShadowMap::getClearValues()const
	{
		static ashes::VkClearValueArray const result
		{
			[]()
			{
				ashes::VkClearValueArray result;

				for ( uint32_t i = 0u; i < uint32_t( SmTexture::eCount ); ++i )
				{
					result.push_back( getClearValue( SmTexture( i ) ) );
				}

				return result;
			}()
		};
		return result;
	}

	ashes::Sampler const & ShadowMap::getSampler( SmTexture texture
		, uint32_t index )const
	{
		return *m_result[texture].sampler;
	}

	crg::ImageViewId ShadowMap::getView( SmTexture texture
		, uint32_t index )const
	{
		return m_result[texture].wholeView;
	}

	crg::ImageViewIdArray ShadowMap::getViews( SmTexture texture
		, uint32_t index )const
	{
		return m_result[texture].subViews;
	}
}
