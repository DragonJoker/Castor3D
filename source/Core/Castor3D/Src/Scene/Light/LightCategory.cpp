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

	LightCategory::LightCategory( LightType p_lightType, Light & p_light )
		: m_lightType{ p_lightType }
		, m_light{ p_light }
	{
	}

	LightCategory::~LightCategory()
	{
	}

	void LightCategory::bind( castor::PxBufferBase & p_texture, uint32_t index )const
	{
		uint32_t offset = 0u;
		doCopyComponent( getColour(), index, offset, p_texture );
		doCopyComponent( getIntensity(), getFarPlane(), index, offset, p_texture );
		doBind( p_texture, index, offset );
	}

	void LightCategory::doCopyComponent( Point2f const & component
		, uint32_t index
		, uint32_t & offset
		, PxBufferBase & data )const
	{
		uint8_t * dst = &( *data.getAt( index * shader::MaxLightComponentsCount + offset++, 0u ) );
		std::memcpy( dst, component.constPtr(), 2 * sizeof( float ) );
	}

	void LightCategory::doCopyComponent( Point2f const & components
		, float component
		, uint32_t index
		, uint32_t & offset
		, PxBufferBase & data )const
	{
		uint8_t * dst = &( *data.getAt( index * shader::MaxLightComponentsCount + offset++, 0u ) );
		std::memcpy( dst, components.constPtr(), 2 * sizeof( float ) );
		dst += 2 * sizeof( float );
		std::memcpy( dst, &component, sizeof( float ) );
	}

	void LightCategory::doCopyComponent( Point3f const & component
		, uint32_t index
		, uint32_t & offset
		, PxBufferBase & data )const
	{
		uint8_t * dst = &( *data.getAt( index * shader::MaxLightComponentsCount + offset++, 0u ) );
		std::memcpy( dst, component.constPtr(), 3 * sizeof( float ) );
	}

	void LightCategory::doCopyComponent( Point3f const & components
		, float component
		, uint32_t index
		, uint32_t & offset
		, PxBufferBase & data )const
	{
		uint8_t * dst = &( *data.getAt( index * shader::MaxLightComponentsCount + offset++, 0u ) );
		std::memcpy( dst, components.constPtr(), 3 * sizeof( float ) );
		dst += 3 * sizeof( float );
		std::memcpy( dst, &component, sizeof( float ) );
	}

	void LightCategory::doCopyComponent( Point4f const & component
		, uint32_t index
		, uint32_t & offset
		, PxBufferBase & data )const
	{
		uint8_t * dst = &( *data.getAt( index * shader::MaxLightComponentsCount + offset++, 0u ) );
		std::memcpy( dst, component.constPtr(), 4 * sizeof( float ) );
	}

	void LightCategory::doCopyComponent( ConstCoords4f const & component
		, uint32_t index
		, uint32_t & offset
		, PxBufferBase & data )const
	{
		uint8_t * dst = &( *data.getAt( index * shader::MaxLightComponentsCount + offset++, 0u ) );
		std::memcpy( dst, component.constPtr(), 4 * sizeof( float ) );
	}

	void LightCategory::doCopyComponent( Coords4f const & component
		, uint32_t index
		, uint32_t & offset
		, PxBufferBase & data )const
	{
		uint8_t * dst = &( *data.getAt( index * shader::MaxLightComponentsCount + offset++, 0u ) );
		std::memcpy( dst, component.constPtr(), 4 * sizeof( float ) );
	}

	void LightCategory::doCopyComponent( castor::Matrix4x4f const & component
		, uint32_t index
		, uint32_t & offset
		, castor::PxBufferBase & data )const
	{
		doCopyComponent( component[0], index, offset, data );
		doCopyComponent( component[1], index, offset, data );
		doCopyComponent( component[2], index, offset, data );
		doCopyComponent( component[3], index, offset, data );
	}

	void LightCategory::doCopyComponent( int32_t const & component
		, uint32_t index
		, uint32_t & offset
		, castor::PxBufferBase & data )const
	{
		uint8_t * dst = &( *data.getAt( index * shader::MaxLightComponentsCount + offset++, 0u ) );
		std::memcpy( dst, &component, sizeof( int32_t ) );
	}
}
