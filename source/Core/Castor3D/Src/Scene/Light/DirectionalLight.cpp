﻿#include "DirectionalLight.hpp"

#include "Render/Viewport.hpp"

using namespace Castor;

namespace Castor3D
{
	DirectionalLight::TextWriter::TextWriter( String const & p_tabs, DirectionalLight const * p_category )
		: LightCategory::TextWriter{ p_tabs }
		, m_category{ p_category }
	{
	}

	bool DirectionalLight::TextWriter::operator()( DirectionalLight const & p_light, TextFile & p_file )
	{
		bool result = LightCategory::TextWriter::operator()( p_light, p_file );

		if ( result )
		{
			result = p_file.WriteText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return result;
	}

	bool DirectionalLight::TextWriter::WriteInto( Castor::TextFile & p_file )
	{
		return ( *this )( *m_category, p_file );
	}

	//*************************************************************************************************

	DirectionalLight::DirectionalLight( Light & p_light )
		: LightCategory{ LightType::eDirectional, p_light }
	{
	}

	DirectionalLight::~DirectionalLight()
	{
	}

	LightCategoryUPtr DirectionalLight::Create( Light & p_light )
	{
		return std::unique_ptr< DirectionalLight >( new DirectionalLight{ p_light } );
	}

	void DirectionalLight::Update( Point3r const & p_target
		, Viewport & p_viewport
		, int32_t p_index )
	{
		auto node = GetLight().GetParent();
		node->Update();
		auto orientation = node->GetDerivedOrientation();
		Point3f position;
		Point3f up{ 0, 1, 0 };
		orientation.transform( up, up );
		matrix::look_at( m_lightSpace, position, position + m_direction, up );
		m_lightSpace = p_viewport.GetProjection() * m_lightSpace;
	}

	void DirectionalLight::UpdateNode( SceneNode const & p_node )
	{
		m_direction = Point3f{ 0, 0, 1 };
		p_node.GetDerivedOrientation().transform( m_direction, m_direction );
	}

	void DirectionalLight::DoBind( Castor::PxBufferBase & p_texture, uint32_t p_index, uint32_t & p_offset )const
	{
		DoCopyComponent( m_direction, p_index, p_offset, p_texture );
		DoCopyComponent( m_lightSpace, p_index, p_offset, p_texture );
	}
}
