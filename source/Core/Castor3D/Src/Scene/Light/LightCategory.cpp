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
			{ LightType::Directional, cuT( "directional" ) },
			{ LightType::Point, cuT( "point" ) },
			{ LightType::Spot, cuT( "spot" ) },
		};

		Logger::LogInfo( m_tabs + cuT( "Writing Light " ) + p_light.GetLight().GetName() );
		bool l_return = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "light \"" ) + p_light.GetLight().GetName() + cuT( "\"\n" ) ) > 0
						&& p_file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;
		Castor::TextWriter< LightCategory >::CheckError( l_return, "LightCategory name" );

		if ( l_return )
		{
			l_return = MovableObject::TextWriter{ m_tabs }( p_light.GetLight(), p_file );
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
		, m_positionType( 0.0, 0.0, 1.0, float( p_lightType ) )
	{
	}

	LightCategory::~LightCategory()
	{
	}

	void LightCategory::DoBindComponent( Point3f const & p_component, int p_index, int & p_offset, PxBufferBase & p_data )const
	{
		uint8_t * l_pDst = &( *p_data.get_at( p_index * GLSL::LightComponentsCount + p_offset++, 0 ) );
		std::memcpy( l_pDst, p_component.const_ptr(), 3 * sizeof( float ) );
	}

	void LightCategory::DoBindComponent( Point4f const & p_component, int p_index, int & p_offset, PxBufferBase & p_data )const
	{
		uint8_t * l_pDst = &( *p_data.get_at( p_index * GLSL::LightComponentsCount + p_offset++, 0 ) );
		std::memcpy( l_pDst, p_component.const_ptr(), 4 * sizeof( float ) );
	}

	void LightCategory::DoBindComponent( Coords4f const & p_component, int p_index, int & p_offset, PxBufferBase & p_data )const
	{
		uint8_t * l_pDst = &( *p_data.get_at( p_index * GLSL::LightComponentsCount + p_offset++, 0 ) );
		std::memcpy( l_pDst, p_component.const_ptr(), 4 * sizeof( float ) );
	}

	void LightCategory::DoBindComponent( Castor::Matrix4x4f const & p_component, int p_index, int & p_offset, Castor::PxBufferBase & p_data )const
	{
		DoBindComponent( p_component[0], p_index, p_offset, p_data );
		DoBindComponent( p_component[1], p_index, p_offset, p_data );
		DoBindComponent( p_component[2], p_index, p_offset, p_data );
		DoBindComponent( p_component[3], p_index, p_offset, p_data );
	}
}
