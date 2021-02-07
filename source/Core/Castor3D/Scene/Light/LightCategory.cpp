#include "Castor3D/Scene/Light/LightCategory.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Scene/Light/Light.hpp"

#include <CastorUtils/Graphics/PixelBuffer.hpp>

#include <ShaderWriter/Source.hpp>

using namespace castor;

namespace castor3d
{
	LightCategory::TextWriter::TextWriter( String const & tabs )
		: castor::TextWriter< LightCategory >{ tabs }
	{
	}

	bool LightCategory::TextWriter::operator()( LightCategory const & light, TextFile & file )
	{
		log::info << m_tabs << cuT( "Writing Light " ) << light.getLight().getName() << std::endl;
		bool result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "light \"" ) + light.getLight().getName() + cuT( "\"\n" ) ) > 0
			&& file.writeText( m_tabs + cuT( "{\n" ) ) > 0;
		castor::TextWriter< LightCategory >::checkError( result, "LightCategory name" );

		if ( result )
		{
			result = MovableObject::TextWriter{ m_tabs + cuT( "\t" ) }( light.getLight(), file );
		}

		if ( result )
		{
			result = file.writeText( m_tabs + cuT( "\ttype " ) + castor3d::getName( light.getLightType() ) + cuT( "\n" ) ) > 0;
			castor::TextWriter< LightCategory >::checkError( result, "LightCategory type" );
		}

		if ( result )
		{
			result = file.writeText( m_tabs + cuT( "\tcolour " ) ) > 0
				&& Point3f::TextWriter( String{} )( light.getColour(), file )
				&& file.writeText( cuT( "\n" ) ) > 0;
			castor::TextWriter< LightCategory >::checkError( result, "LightCategory colour" );
		}

		if ( result )
		{
			result = file.writeText( m_tabs + cuT( "\tintensity " ) ) > 0
				&& Point2f::TextWriter( String{} )( light.getIntensity(), file )
				&& file.writeText( cuT( "\n" ) ) > 0;
			castor::TextWriter< LightCategory >::checkError( result, "LightCategory intensity" );
		}

		result = file.writeText( m_tabs + cuT( "\tshadows\n" ) ) > 0
			&& file.writeText( m_tabs + cuT( "\t{\n" ) ) > 0
			&& file.writeText( m_tabs + cuT( "\t\tproducer " ) + ( light.getLight().isShadowProducer() ? String{ "true" } : String{ "false" } ) + cuT( "\n" ) ) > 0
			&& file.writeText( m_tabs + cuT( "\t\tfilter " ) + castor3d::getName( light.getLight().getShadowType() ) + cuT( "\n" ) ) > 0
			&& file.writeText( m_tabs + cuT( "\t\tglobal_illumination " ) + castor3d::getName( light.getLight().getGlobalIlluminationType() ) + cuT( "\n" ) ) > 0;
		castor::TextWriter< LightCategory >::checkError( result, "LightCategory shadow" );

		if ( result && light.getVolumetricSteps() )
		{
			result = file.writeText( m_tabs + cuT( "\t\tvolumetric_steps " )
					+ string::toString( light.getVolumetricSteps(), std::locale{ "C" } ) + cuT( "\n" ) ) > 0
				&& file.writeText( m_tabs + cuT( "\t\tvolumetric_scattering " )
					+ string::toString( light.getVolumetricScatteringFactor(), std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
			castor::TextWriter< LightCategory >::checkError( result, "LightCategory shadow volumetric steps" );
		}

		result = result && RsmConfig::TextWriter( m_tabs + cuT( "\t\t" ) )( light.getLight().getRsmConfig(), file );
		castor::TextWriter< LightCategory >::checkError( result, "LightCategory shadow RSM config" );
		result = result && LpvConfig::TextWriter( m_tabs + cuT( "\t\t" ) )( light.getLight().getLpvConfig(), file );
		castor::TextWriter< LightCategory >::checkError( result, "LightCategory shadow LPV config" );

		result = file.writeText( m_tabs + cuT( "\t\traw_config\n" ) ) > 0
			&& file.writeText( m_tabs + cuT( "\t\t{\n" ) ) > 0
			&& file.writeText( m_tabs + cuT( "\t\t\tmin_offset " ) + castor::string::toString( light.getShadowOffsets()[0] ) + cuT( "\n" ) ) > 0
			&& file.writeText( m_tabs + cuT( "\t\t\tmax_slope_offset " ) + castor::string::toString( light.getShadowOffsets()[1] ) + cuT( "\n" ) ) > 0
			&& file.writeText( m_tabs + cuT( "\t\t}\n" ) ) > 0;

		result = file.writeText( m_tabs + cuT( "\t\tpcf_config\n" ) ) > 0
			&& file.writeText( m_tabs + cuT( "\t\t{\n" ) ) > 0
			&& file.writeText( m_tabs + cuT( "\t\t\tmin_offset " ) + castor::string::toString( light.getShadowOffsets()[2] ) + cuT( "\n" ) ) > 0
			&& file.writeText( m_tabs + cuT( "\t\t\tmax_slope_offset " ) + castor::string::toString( light.getShadowOffsets()[3] ) + cuT( "\n" ) ) > 0
			&& file.writeText( m_tabs + cuT( "\t\t}\n" ) ) > 0;

		result = file.writeText( m_tabs + cuT( "\t\tvsm_config\n" ) ) > 0
			&& file.writeText( m_tabs + cuT( "\t\t{\n" ) ) > 0
			&& file.writeText( m_tabs + cuT( "\t\t\tvariance_max " ) + castor::string::toString( light.getShadowVariance()[0] ) + cuT( "\n" ) ) > 0
			&& file.writeText( m_tabs + cuT( "\t\t\tvariance_bias " ) + castor::string::toString( light.getShadowVariance()[1] ) + cuT( "\n" ) ) > 0
			&& file.writeText( m_tabs + cuT( "\t\t}\n" ) ) > 0;

		result = result && file.writeText( m_tabs + cuT( "\t}\n" ) ) > 0;
		castor::TextWriter< LightCategory >::checkError( result, "LightCategory shadow" );

		return result;
	}

	//*************************************************************************************************

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
		doCopyComponent( getShadowOffsets(), buffer );
		doCopyComponent( getShadowVariance(), buffer );
		doBind( buffer );
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
