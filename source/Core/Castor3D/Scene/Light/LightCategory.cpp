#include "Castor3D/Scene/Light/LightCategory.hpp"
#include "Castor3D/Scene/Light/Light.hpp"

#include <CastorUtils/Graphics/PixelBuffer.hpp>

#include <ShaderWriter/Source.hpp>

using namespace castor;

namespace castor3d
{
	LightCategory::TextWriter::TextWriter( String const & p_tabs )
		: castor::TextWriter< LightCategory >{ p_tabs }
	{
	}

	bool LightCategory::TextWriter::operator()( LightCategory const & p_light, TextFile & p_file )
	{
		Logger::logInfo( m_tabs + cuT( "Writing Light " ) + p_light.getLight().getName() );
		bool result = p_file.writeText( cuT( "\n" ) + m_tabs + cuT( "light \"" ) + p_light.getLight().getName() + cuT( "\"\n" ) ) > 0
			&& p_file.writeText( m_tabs + cuT( "{\n" ) ) > 0;
		castor::TextWriter< LightCategory >::checkError( result, "LightCategory name" );

		if ( result )
		{
			result = MovableObject::TextWriter{ m_tabs + cuT( "\t" ) }( p_light.getLight(), p_file );
		}

		if ( result )
		{
			result = p_file.writeText( m_tabs + cuT( "\ttype " ) + castor3d::getName( p_light.getLightType() ) + cuT( "\n" ) ) > 0;
			castor::TextWriter< LightCategory >::checkError( result, "LightCategory type" );
		}

		if ( result )
		{
			result = p_file.writeText( m_tabs + cuT( "\tcolour " ) ) > 0
				&& Point3f::TextWriter( String{} )( p_light.getColour(), p_file )
				&& p_file.writeText( cuT( "\n" ) ) > 0;
			castor::TextWriter< LightCategory >::checkError( result, "LightCategory colour" );
		}

		if ( result )
		{
			result = p_file.writeText( m_tabs + cuT( "\tintensity " ) ) > 0
				&& Point2f::TextWriter( String{} )( p_light.getIntensity(), p_file )
				&& p_file.writeText( cuT( "\n" ) ) > 0;
			castor::TextWriter< LightCategory >::checkError( result, "LightCategory intensity" );
		}

		if ( result && p_light.getLight().isShadowProducer() )
		{
			result = p_file.writeText( m_tabs + cuT( "\tshadows\n" ) ) > 0
				&& p_file.writeText( m_tabs + cuT( "\t{\n" ) ) > 0
				&& p_file.writeText( m_tabs + cuT( "\t\tproducer true\n" ) ) > 0
				&& p_file.writeText( m_tabs + cuT( "\t\tfilter " ) + castor3d::getName( p_light.getLight().getShadowType() ) + cuT( "\n" ) ) > 0;

			if ( result && p_light.getVolumetricSteps() )
			{
				result = p_file.writeText( m_tabs + cuT( "\t\tvolumetric_steps " )
						+ string::toString( p_light.getVolumetricSteps(), std::locale{ "C" } ) + cuT( "\n" ) ) > 0
					&& p_file.writeText( m_tabs + cuT( "\t\tvolumetric_scattering " )
						+ string::toString( p_light.getVolumetricScatteringFactor(), std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
			}

			result = result && p_file.writeText( m_tabs + cuT( "\t}\n" ) ) > 0;
			castor::TextWriter< LightCategory >::checkError( result, "LightCategory shadow producer" );
		}

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
		doCopyComponent( getShadowOffsets(), getShadowVariance(), buffer );
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
