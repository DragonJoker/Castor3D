#include "Castor3D/Scene/Light/LightCategory.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Scene/Light/Light.hpp"

#include <CastorUtils/Graphics/PixelBuffer.hpp>

#include <ShaderWriter/Source.hpp>

using namespace castor;

namespace castor3d
{
	LightCategory::LightCategory( LightType lightType, Light & light )
		: m_lightType{ lightType }
		, m_light{ light }
	{
	}

	LightCategory::~LightCategory()
	{
	}

	void LightCategory::bind( Point4f * buffer )const
	{
		doCopyComponent( getColour(), float( m_shadowMapIndex ), buffer );
		doCopyComponent( getIntensity(), getFarPlane(), float( getLight().getShadowType() ), buffer );
		doCopyComponent( float( getVolumetricSteps() ), getVolumetricScatteringFactor(), 0.0f, 0.0f, buffer );
		doCopyComponent( getShadowRawOffsets(), getShadowPcfOffsets(), buffer );
		doCopyComponent( getShadowVariance(), buffer );
		doBind( buffer );
	}

	uint32_t LightCategory::getVolumetricSteps()const
	{
		return m_light.getVolumetricSteps();
	}

	float LightCategory::getVolumetricScatteringFactor()const
	{
		return m_light.getVolumetricScatteringFactor();
	}

	castor::Point2f const & LightCategory::getShadowRawOffsets()const
	{
		return m_light.getShadowRawOffsets();
	}

	castor::Point2f const & LightCategory::getShadowPcfOffsets()const
	{
		return m_light.getShadowPcfOffsets();
	}

	castor::Point2f const & LightCategory::getShadowVariance()const
	{
		return m_light.getShadowVariance();
	}

	Shadow const & LightCategory::getShadowConfig()const
	{
		return m_light.getShadowConfig();
	}

	RsmConfig const & LightCategory::getRsmConfig()const
	{
		return m_light.getRsmConfig();
	}

	LpvConfig const & LightCategory::getLpvConfig()const
	{
		return m_light.getLpvConfig();
	}

	void LightCategory::setVolumetricSteps( uint32_t value )
	{
		m_light.setVolumetricSteps( value );
	}

	void LightCategory::setVolumetricScatteringFactor( float value )
	{
		m_light.setVolumetricScatteringFactor( value );
	}

	void LightCategory::setRawMinOffset( float value )
	{
		m_light.setRawMinOffset( value );
	}

	void LightCategory::setRawMaxSlopeOffset( float value )
	{
		m_light.setRawMaxSlopeOffset( value );
	}

	void LightCategory::setPcfMinOffset( float value )
	{
		m_light.setPcfMinOffset( value );
	}

	void LightCategory::setPcfMaxSlopeOffset( float value )
	{
		m_light.setPcfMaxSlopeOffset( value );
	}

	void LightCategory::setVsmMaxVariance( float value )
	{
		m_light.setVsmMaxVariance( value );
	}

	void LightCategory::setVsmVarianceBias( float value )
	{
		m_light.setVsmVarianceBias( value );
	}

	void LightCategory::doCopyComponent( Point2f const & component
		, Point4f *& buffer )const
	{
		( *buffer )[0] = component[0];
		( *buffer )[1] = component[1];
		++buffer;
	}

	void LightCategory::doCopyComponent( castor::Point2f const & components1
		, castor::Point2f const & components2
		, castor::Point4f *& buffer )const
	{
		( *buffer )[0] = components1[0];
		( *buffer )[1] = components1[1];
		( *buffer )[2] = components2[0];
		( *buffer )[3] = components2[1];
		++buffer;
	}

	void LightCategory::doCopyComponent( Point2f const & components
		, float component
		, Point4f *& buffer )const
	{
		( *buffer )[0] = components[0];
		( *buffer )[1] = components[1];
		( *buffer )[2] = component;
		++buffer;
	}

	void LightCategory::doCopyComponent( Point2f const & components
		, float component1
		, float component2
		, Point4f *& buffer )const
	{
		( *buffer )[0] = components[0];
		( *buffer )[1] = components[1];
		( *buffer )[2] = component1;
		( *buffer )[3] = component2;
		++buffer;
	}

	void LightCategory::doCopyComponent( float component0
		, float component1
		, float component2
		, float component3
		, castor::Point4f *& buffer )const
	{
		( *buffer )[0] = component0;
		( *buffer )[1] = component1;
		( *buffer )[2] = component2;
		( *buffer )[3] = component3;
		++buffer;
	}

	void LightCategory::doCopyComponent( Point3f const & component
		, Point4f *& buffer )const
	{
		( *buffer )[0] = component[0];
		( *buffer )[1] = component[1];
		( *buffer )[2] = component[2];
		++buffer;
	}

	void LightCategory::doCopyComponent( Point3f const & components
		, float component
		, Point4f *& buffer )const
	{
		( *buffer )[0] = components[0];
		( *buffer )[1] = components[1];
		( *buffer )[2] = components[2];
		( *buffer )[3] = component;
		++buffer;
	}

	void LightCategory::doCopyComponent( Point4f const & component
		, Point4f *& buffer )const
	{
		( *buffer )[0] = component[0];
		( *buffer )[1] = component[1];
		( *buffer )[2] = component[2];
		( *buffer )[3] = component[3];
		++buffer;
	}

	void LightCategory::doCopyComponent( ConstCoords4f const & component
		, Point4f *& buffer )const
	{
		( *buffer )[0] = component[0];
		( *buffer )[1] = component[1];
		( *buffer )[2] = component[2];
		( *buffer )[3] = component[3];
		++buffer;
	}

	void LightCategory::doCopyComponent( Coords4f const & component
		, Point4f *& buffer )const
	{
		( *buffer )[0] = component[0];
		( *buffer )[1] = component[1];
		( *buffer )[2] = component[2];
		( *buffer )[3] = component[3];
		++buffer;
	}

	void LightCategory::doCopyComponent( castor::Matrix4x4f const & component
		, Point4f *& buffer )const
	{
		doCopyComponent( component[0], buffer );
		doCopyComponent( component[1], buffer );
		doCopyComponent( component[2], buffer );
		doCopyComponent( component[3], buffer );
	}

	void LightCategory::doCopyComponent( int32_t const & component
		, Point4f *& buffer )const
	{
		( *buffer )[0] = float( component );
		++buffer;
	}
}
