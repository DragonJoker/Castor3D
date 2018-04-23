#include "LightCategory.hpp"
#include "Light.hpp"

#include <Graphics/PixelBuffer.hpp>

#include <GlslSource.hpp>

using namespace castor;

namespace castor3d
{
	LightCategory::TextWriter::TextWriter( String const & p_tabs )
		: castor::TextWriter< LightCategory >{ p_tabs }
	{
	}

	bool LightCategory::TextWriter::operator()( LightCategory const & p_light, TextFile & p_file )
	{
		static std::map< LightType, String > type
		{
			{ LightType::eDirectional, cuT( "directional" ) },
			{ LightType::ePoint, cuT( "point" ) },
			{ LightType::eSpot, cuT( "spot" ) },
		};

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
			result = p_file.writeText( m_tabs + cuT( "\ttype " ) + type[p_light.getLightType()] + cuT( "\n" ) ) > 0;
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
			result = p_file.writeText( m_tabs + cuT( "\tshadow_producer true\n" ) ) > 0;
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
		uint32_t offset = 0u;
		doCopyComponent( getColour(), float( m_shadowMapIndex ), buffer );
		doCopyComponent( getIntensity(), getFarPlane(), buffer );
		doBind( buffer );
	}

	void LightCategory::doCopyComponent( Point2f const & component
		, Point4f *& buffer )const
	{
		( *buffer )[0] = component[0];
		( *buffer )[1] = component[1];
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
