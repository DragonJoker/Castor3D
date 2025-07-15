#include "Castor3D/Scene/Light/LightCategory.hpp"

#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"

CU_ImplementSmartPtr( c3d, LightCategory )
CU_ImplementSmartPtr( c3d, LightInstance )

namespace c3d
{
	//*********************************************************************************************

	LightCategory::LightCategory( LightType lightType
		, bool & dirty
		, Function< void() > markParentDirty )
		: m_dirty{ dirty }
		, m_markParentDirty{ c3d::move( markParentDirty ) }
		, m_lightType{ lightType }
		, m_colour{ m_dirty, { 1.0, 1.0, 1.0 }, m_markParentDirty }
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
		, LightCategory & category
		, Function< void() > markParentDirty
		, Function< bool() > isParentEnabled )
		: m_node{ &node }
		, m_category{ category }
		, m_markParentDirty{ c3d::move( markParentDirty ) }
		, m_isParentEnabled{ c3d::move( isParentEnabled ) }
	{
	}

	void LightInstance::update( [[maybe_unused]] CpuUpdater const & updater )
	{
		m_node->update();
		m_category.update();
		doUpdate();

		m_markParentDirty();
		onGpuChanged( *this );
		m_dirtyShadows = m_dirty;
		m_dirty = false;
	}

	bool LightInstance::updateShadow( Camera const & viewCamera
		, Camera * lightCamera
		, int32_t index )
	{
		setShadowMapIndex( index );
		doUpdateShadow( viewCamera, lightCamera, index );
		auto result = m_dirtyShadows;
		m_dirtyShadows = false;
		return result;
	}

	void LightInstance::cloneInto( LightInstance & output )const
	{
		doCloneInto( output );
	}

	void LightInstance::fillLightBuffer( uint32_t index
		, VkDeviceSize offset
		, Point4f * data )
	{
		m_bufferIndex = index;
		m_bufferOffset = offset;

		auto & base = *reinterpret_cast< LightData * >( data->ptr() );
		base.colour = m_category.getColour();
		base.shadowMapIndex = float( getShadowMapIndex() );
		base.enabled = ( ( m_isParentEnabled() && m_node->isVisible() ) ? 1.0f : 0.0f );
		doFillLightBuffer( data );
	}

	String const & LightInstance::getName()const noexcept
	{
		return m_node->getName();
	}

	Scene * LightInstance::getScene()const noexcept
	{
		return m_node->getScene();
	}

	DirectionalLightRPtr LightInstance::getDirectionalLight()const
	{
		CU_Require( m_category.getLightType() == LightType::eDirectional );
		return &static_cast< DirectionalLight & >( m_category );
	}

	PointLightRPtr LightInstance::getPointLight()const
	{
		CU_Require( m_category.getLightType() == LightType::ePoint );
		return &static_cast< PointLight & >( m_category );
	}

	SpotLightRPtr LightInstance::getSpotLight()const
	{
		CU_Require( m_category.getLightType() == LightType::eSpot );
		return &static_cast< SpotLight & >( m_category );
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
