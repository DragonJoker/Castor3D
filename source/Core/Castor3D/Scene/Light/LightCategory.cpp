#include "Castor3D/Scene/Light/LightCategory.hpp"

#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Scene/Light/Light.hpp"

#include <CastorUtils/Graphics/PixelBuffer.hpp>

#include <ShaderWriter/Source.hpp>

CU_ImplementSmartPtr( castor3d, LightCategory )
CU_ImplementSmartPtr( castor3d, LightInstance )

namespace castor3d
{
	//*********************************************************************************************

	LightCategory::LightCategory( LightType lightType
		, bool & dirty
		, castor::Function< void() > changedCallback )
		: m_dirty{ dirty }
		, m_changedCallback{ castor::move( changedCallback ) }
		, m_lightType{ lightType }
		, m_colour{ m_dirty, { 1.0, 1.0, 1.0 }, m_changedCallback }
	{
	}

	void LightCategory::update()
	{
		doUpdate();
		m_currentGlobalIllumination = m_shadows.globalIllumination;
		m_currentShadowCaster = m_shadows.enabled;
	}

	void LightCategory::accept( ConfigurationVisitorBase & vis )
	{
		vis.visit( cuT( "Colour" ), m_colour );
		doAccept( vis );
		m_shadows.accept( vis, getLightType() );
	}

	void LightCategory::cloneInto( LightCategory & output )const
	{
		output.m_shadows = m_shadows;
		output.m_currentShadowCaster = m_currentShadowCaster.load();
		output.m_currentGlobalIllumination = m_currentGlobalIllumination.load();
		output.m_colour = m_colour;
		doCloneInto( output );
	}

	//*********************************************************************************************

	LightInstance::LightInstance( SceneNode & node
		, bool & dirty
		, castor::Function< void() > onGpuChanged
		, LightCategory & category )
		: m_dirty{ dirty }
		, m_node{ &node }
		, m_category{ category }
		, m_onGpuChanged{ castor::move( onGpuChanged ) }
	{
	}

	void LightInstance::update( CpuUpdater & /*updater*/ )
	{
		m_node->update();
		m_category.update();
		doUpdate();
		m_onGpuChanged();
		m_dirty = false;
	}

	bool LightInstance::updateShadow( Camera const & viewCamera
		, Camera * lightCamera
		, int32_t index )
	{
		setShadowMapIndex( index );
		return doUpdateShadow( viewCamera, lightCamera, index );
	}

	void LightInstance::cloneInto( LightInstance & output )const
	{
		doCloneInto( output );
	}

	void LightInstance::fillLightBuffer( bool enabled
		, uint32_t index
		, VkDeviceSize offset
		, castor::Point4f * data )
	{
		m_bufferIndex = index;
		m_bufferOffset = offset;

		auto & base = *reinterpret_cast< LightData * >( data->ptr() );
		base.colour = m_category.getColour();
		base.shadowMapIndex = float( getShadowMapIndex() );
		base.enabled = ( ( enabled && m_node->isVisible() ) ? 1.0f : 0.0f );
		doFillLightBuffer( data );
	}

	castor::String const & LightInstance::getName()const noexcept
	{
		return m_node->getName();
	}

	Scene * LightInstance::getScene()const noexcept
	{
		return m_node->getScene();
	}

	void LightInstance::doFillBaseShadowData( BaseShadowData & data )const
	{
		data.shadowType = uint32_t( m_category.isShadowProducer()
			? m_category.getShadowType()
			: ShadowType::eNone );
		data.pcfFilterSize = float( m_category.getShadowPcfFilterSize().value() );
		data.pcfSampleCount = m_category.getShadowPcfSampleCount().value();

		data.rawShadowsOffsets = m_category.getShadowRawOffsets();
		data.pcfShadowsOffsets = m_category.getShadowPcfOffsets();

		data.vsmMinVariance = m_category.getVsmMinVariance();
		data.vsmLightBleedingReduction = m_category.getVsmLightBleedingReduction();
		data.volumetricSteps = m_category.getVolumetricSteps();
		data.volumetricScattering = m_category.getVolumetricScatteringFactor();
	}

	//*********************************************************************************************
}
