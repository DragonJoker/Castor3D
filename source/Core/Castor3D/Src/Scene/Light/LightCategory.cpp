#include "LightCategory.hpp"
#include "Light.hpp"

#include <Graphics/PixelBuffer.hpp>

#include <GlslSource.hpp>

using namespace Castor;

namespace Castor3D
{
	LightCategory::TextWriter::TextWriter( String const & p_tabs )
		: Castor::TextWriter< LightCategory >{ p_tabs }
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

		Logger::LogInfo( m_tabs + cuT( "Writing Light " ) + p_light.GetLight().GetName() );
		bool result = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "light \"" ) + p_light.GetLight().GetName() + cuT( "\"\n" ) ) > 0
						&& p_file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;
		Castor::TextWriter< LightCategory >::CheckError( result, "LightCategory name" );

		if ( result )
		{
			result = MovableObject::TextWriter{ m_tabs + cuT( "\t" ) }( p_light.GetLight(), p_file );
		}

		if ( result )
		{
			result = p_file.WriteText( m_tabs + cuT( "\ttype " ) + type[p_light.GetLightType()] + cuT( "\n" ) ) > 0;
			Castor::TextWriter< LightCategory >::CheckError( result, "LightCategory type" );
		}

		if ( result )
		{
			result = p_file.WriteText( m_tabs + cuT( "\tcolour " ) ) > 0
					   && Point3f::TextWriter( String{} )( p_light.GetColour(), p_file )
					   && p_file.WriteText( cuT( "\n" ) ) > 0;
			Castor::TextWriter< LightCategory >::CheckError( result, "LightCategory colour" );
		}

		if ( result )
		{
			result = p_file.WriteText( m_tabs + cuT( "\tintensity " ) ) > 0
					   && Point2f::TextWriter( String{} )( p_light.GetIntensity(), p_file )
					   && p_file.WriteText( cuT( "\n" ) ) > 0;
			Castor::TextWriter< LightCategory >::CheckError( result, "LightCategory intensity" );
		}

		if ( result && p_light.GetLight().IsShadowProducer() )
		{
			result = p_file.WriteText( m_tabs + cuT( "\tshadow_producer true\n" ) ) > 0;
			Castor::TextWriter< LightCategory >::CheckError( result, "LightCategory shadow producer" );
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

	void LightCategory::Bind( Castor::PxBufferBase & p_texture, uint32_t p_index )const
	{
		uint32_t offset = 0u;
		DoCopyComponent( GetColour(), p_index, offset, p_texture );
		DoCopyComponent( GetIntensity(), p_index, offset, p_texture );
		DoBind( p_texture, p_index, offset );
	}

	void LightCategory::DoCopyComponent( Point2f const & p_component, uint32_t p_index, uint32_t & p_offset, PxBufferBase & p_data )const
	{
		uint8_t * pDst = &( *p_data.get_at( p_index * GLSL::MaxLightComponentsCount + p_offset++, 0u ) );
		std::memcpy( pDst, p_component.const_ptr(), 2 * sizeof( float ) );
	}

	void LightCategory::DoCopyComponent( Point3f const & p_component, uint32_t p_index, uint32_t & p_offset, PxBufferBase & p_data )const
	{
		uint8_t * pDst = &( *p_data.get_at( p_index * GLSL::MaxLightComponentsCount + p_offset++, 0u ) );
		std::memcpy( pDst, p_component.const_ptr(), 3 * sizeof( float ) );
	}

	void LightCategory::DoCopyComponent( Point4f const & p_component, uint32_t p_index, uint32_t & p_offset, PxBufferBase & p_data )const
	{
		uint8_t * pDst = &( *p_data.get_at( p_index * GLSL::MaxLightComponentsCount + p_offset++, 0u ) );
		std::memcpy( pDst, p_component.const_ptr(), 4 * sizeof( float ) );
	}

	void LightCategory::DoCopyComponent( ConstCoords4f const & p_component, uint32_t p_index, uint32_t & p_offset, PxBufferBase & p_data )const
	{
		uint8_t * pDst = &( *p_data.get_at( p_index * GLSL::MaxLightComponentsCount + p_offset++, 0u ) );
		std::memcpy( pDst, p_component.const_ptr(), 4 * sizeof( float ) );
	}

	void LightCategory::DoCopyComponent( Coords4f const & p_component, uint32_t p_index, uint32_t & p_offset, PxBufferBase & p_data )const
	{
		uint8_t * pDst = &( *p_data.get_at( p_index * GLSL::MaxLightComponentsCount + p_offset++, 0u ) );
		std::memcpy( pDst, p_component.const_ptr(), 4 * sizeof( float ) );
	}

	void LightCategory::DoCopyComponent( Castor::Matrix4x4f const & p_component, uint32_t p_index, uint32_t & p_offset, Castor::PxBufferBase & p_data )const
	{
		DoCopyComponent( p_component[0], p_index, p_offset, p_data );
		DoCopyComponent( p_component[1], p_index, p_offset, p_data );
		DoCopyComponent( p_component[2], p_index, p_offset, p_data );
		DoCopyComponent( p_component[3], p_index, p_offset, p_data );
	}

	void LightCategory::DoCopyComponent( int32_t const & p_component, uint32_t p_index, uint32_t & p_offset, Castor::PxBufferBase & p_data )const
	{
		uint8_t * pDst = &( *p_data.get_at( p_index * GLSL::MaxLightComponentsCount + p_offset++, 0u ) );
		std::memcpy( pDst, &p_component, sizeof( int32_t ) );
	}
}
