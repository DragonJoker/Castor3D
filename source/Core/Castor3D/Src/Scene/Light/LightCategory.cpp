#include "LightCategory.hpp"
#include "Light.hpp"

#include <Log/Logger.hpp>
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
		static std::map< LightType, String > l_type
		{
			{ LightType::eDirectional, cuT( "directional" ) },
			{ LightType::ePoint, cuT( "point" ) },
			{ LightType::eSpot, cuT( "spot" ) },
		};

		Logger::LogInfo( m_tabs + cuT( "Writing Light " ) + p_light.GetLight().GetName() );
		bool l_return = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "light \"" ) + p_light.GetLight().GetName() + cuT( "\"\n" ) ) > 0
						&& p_file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;
		Castor::TextWriter< LightCategory >::CheckError( l_return, "LightCategory name" );

		if ( l_return )
		{
			l_return = MovableObject::TextWriter{ m_tabs + cuT( "\t" ) }( p_light.GetLight(), p_file );
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "\ttype " ) + l_type[p_light.GetLightType()] + cuT( "\n" ) ) > 0;
			Castor::TextWriter< LightCategory >::CheckError( l_return, "LightCategory type" );
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "\tcolour " ) ) > 0
					   && Point3f::TextWriter( String{} )( p_light.GetColour(), p_file )
					   && p_file.WriteText( cuT( "\n" ) ) > 0;
			Castor::TextWriter< LightCategory >::CheckError( l_return, "LightCategory colour" );
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "\tintensity " ) ) > 0
					   && Point3f::TextWriter( String{} )( p_light.GetIntensity(), p_file )
					   && p_file.WriteText( cuT( "\n" ) ) > 0;
			Castor::TextWriter< LightCategory >::CheckError( l_return, "LightCategory intensity" );
		}

		if ( l_return && p_light.GetLight().IsShadowProducer() )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "\tshadow_producer true\n" ) ) > 0;
			Castor::TextWriter< LightCategory >::CheckError( l_return, "LightCategory shadow producer" );
		}

		return l_return;
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
		uint32_t l_offset = 0u;
		DoBindComponent( GetColour(), p_index, l_offset, p_texture );
		DoBindComponent( GetIntensity(), p_index, l_offset, p_texture );
		DoBind( p_texture, p_index, l_offset );
	}

	void LightCategory::DoBindComponent( Point3f const & p_component, uint32_t p_index, uint32_t & p_offset, PxBufferBase & p_data )const
	{
		uint8_t * l_pDst = &( *p_data.get_at( p_index * GLSL::MaxLightComponentsCount + p_offset++, 0u ) );
		std::memcpy( l_pDst, p_component.const_ptr(), 3 * sizeof( float ) );
	}

	void LightCategory::DoBindComponent( Point4f const & p_component, uint32_t p_index, uint32_t & p_offset, PxBufferBase & p_data )const
	{
		uint8_t * l_pDst = &( *p_data.get_at( p_index * GLSL::MaxLightComponentsCount + p_offset++, 0u ) );
		std::memcpy( l_pDst, p_component.const_ptr(), 4 * sizeof( float ) );
	}

	void LightCategory::DoBindComponent( Coords4f const & p_component, uint32_t p_index, uint32_t & p_offset, PxBufferBase & p_data )const
	{
		uint8_t * l_pDst = &( *p_data.get_at( p_index * GLSL::MaxLightComponentsCount + p_offset++, 0u ) );
		std::memcpy( l_pDst, p_component.const_ptr(), 4 * sizeof( float ) );
	}

	void LightCategory::DoBindComponent( Castor::Matrix4x4f const & p_component, uint32_t p_index, uint32_t & p_offset, Castor::PxBufferBase & p_data )const
	{
		DoBindComponent( p_component[0], p_index, p_offset, p_data );
		DoBindComponent( p_component[1], p_index, p_offset, p_data );
		DoBindComponent( p_component[2], p_index, p_offset, p_data );
		DoBindComponent( p_component[3], p_index, p_offset, p_data );
	}

	void LightCategory::DoBindComponent( int32_t const & p_component, uint32_t p_index, uint32_t & p_offset, Castor::PxBufferBase & p_data )const
	{
		uint8_t * l_pDst = &( *p_data.get_at( p_index * GLSL::MaxLightComponentsCount + p_offset++, 0u ) );
		std::memcpy( l_pDst, &p_component, sizeof( int32_t ) );
	}
}
