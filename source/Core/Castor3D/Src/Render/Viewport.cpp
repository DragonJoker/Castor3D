#include "Viewport.hpp"

#include "Engine.hpp"

using namespace castor;

namespace castor3d
{
	Viewport::TextWriter::TextWriter( String const & p_tabs )
		: castor::TextWriter< Viewport >{ p_tabs }
	{
	}

	bool Viewport::TextWriter::operator()( Viewport const & p_viewport, TextFile & p_file )
	{
		bool result = p_file.writeText( cuT( "\n" ) + m_tabs + cuT( "viewport\n" ) ) > 0
						&& p_file.writeText( m_tabs + cuT( "{\n" ) ) > 0;

		if ( result )
		{
			result = p_file.writeText( m_tabs + cuT( "\ttype " ) + Viewport::string_type[size_t( p_viewport.getType() )] + cuT( "\n" ) ) > 0;
			castor::TextWriter< Viewport >::checkError( result, "Viewport type" );
		}

		if ( result )
		{
			if ( p_viewport.getType() == ViewportType::eOrtho || p_viewport.getType() == ViewportType::eFrustum )
			{
				result = p_file.writeText( m_tabs + cuT( "\tnear " ) + string::toString( p_viewport.getNear() ) + cuT( "\n" ) ) > 0
						   && p_file.writeText( m_tabs + cuT( "\tfar " ) + string::toString( p_viewport.getFar() ) + cuT( "\n" ) ) > 0
						   && p_file.writeText( m_tabs + cuT( "\tleft " ) + string::toString( p_viewport.getLeft() ) + cuT( "\n" ) ) > 0
						   && p_file.writeText( m_tabs + cuT( "\tright " ) + string::toString( p_viewport.getRight() ) + cuT( "\n" ) ) > 0
						   && p_file.writeText( m_tabs + cuT( "\ttop " ) + string::toString( p_viewport.getTop() ) + cuT( "\n" ) ) > 0
						   && p_file.writeText( m_tabs + cuT( "\tbottom " ) + string::toString( p_viewport.getBottom() ) + cuT( "\n" ) ) > 0;
			}
			else
			{
				result = p_file.writeText( m_tabs + cuT( "\tnear " ) + string::toString( p_viewport.getNear() ) + cuT( "\n" ) ) > 0
						   && p_file.writeText( m_tabs + cuT( "\taspect_ratio " ) + string::toString( p_viewport.getRatio() ) + cuT( "\n" ) ) > 0
						   && p_file.writeText( m_tabs + cuT( "\tfar " ) + string::toString( p_viewport.getFar() ) + cuT( "\n" ) ) > 0
						   && p_file.writeText( m_tabs + cuT( "\tfov_y " ) + string::toString( p_viewport.getFovY().degrees() ) + cuT( "\n" ) ) > 0;
			}

			castor::TextWriter< Viewport >::checkError( result, "Viewport values" );
		}

		if ( result )
		{
			result = p_file.writeText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return result;
	}

	//*************************************************************************************************

	const std::array< String, size_t( ViewportType::eCount ) > Viewport::string_type = { cuT( "ortho" ), cuT( "perspective" ), cuT( "frustum" ) };

	Viewport::Viewport( Engine const & engine
		, ViewportType type
		, castor::Angle const & fovY
		, real aspect
		, real left
		, real right
		, real bottom
		, real top
		, real near
		, real far )
		: m_engine{ engine }
		, m_type{ type }
		, m_size{}
		, m_modified{ true }
		, m_fovY{ m_modified, fovY }
		, m_ratio{ m_modified, aspect }
		, m_left{ m_modified, left }
		, m_right{ m_modified, right }
		, m_bottom{ m_modified, bottom }
		, m_top{ m_modified, top }
		, m_near{ m_modified, near }
		, m_far{ m_modified, far }
		, m_viewport{ 1u, 1u, 0, 0 }
		, m_scissor{ 0, 0, 1u, 1u }
	{
		if ( m_type != ViewportType::eOrtho && !m_near )
		{
			m_near = real( 0.1 ); // not zero or we have a Z fight (due to depth buffer precision)
		}
	}

	Viewport::Viewport( Engine const & engine )
		: Viewport{ engine, ViewportType::eOrtho, Angle{}, 1, 0, 1, 0, 1, 0, 1 }
	{
	}

	Viewport::~Viewport()
	{
	}

	bool Viewport::update()
	{
		bool result = false;

		if ( isModified() )
		{
			switch ( m_type )
			{
			case castor3d::ViewportType::eOrtho:
				m_projection = m_engine.getRenderSystem()->getOrtho( m_left
					, m_right
					, m_bottom
					, m_top
					, m_near
					, m_far );
				break;

			case castor3d::ViewportType::ePerspective:
				m_projection = m_engine.getRenderSystem()->getPerspective( m_fovY.value().radians()
					, m_ratio
					, m_near
					, m_far );
				break;

			case castor3d::ViewportType::eFrustum:
				m_projection = m_engine.getRenderSystem()->getFrustum( m_left
					, m_right
					, m_bottom
					, m_top
					, m_near
					, m_far );
				break;

			default:
				break;
			}

			m_modified = false;
			result = true;
		}

		return result;
	}

	void Viewport::setPerspective( Angle const & fovY
		, real aspect
		, real near
		, real far )
	{
		m_type = ViewportType::ePerspective;
		m_fovY = fovY;
		m_ratio = aspect;
		m_left = 0;
		m_right = 1;
		m_bottom = 0;
		m_top = 1;
		m_near = near;
		m_far = far;
		m_modified = true;
	}

	void Viewport::setFrustum( real left
		, real right
		, real bottom
		, real top
		, real near
		, real far )
	{
		m_type = ViewportType::eFrustum;
		m_fovY = Angle{};
		m_ratio = 0;
		m_left = left;
		m_right = right;
		m_bottom = bottom;
		m_top = top;
		m_near = near;
		m_far = far;
		m_modified = true;
	}

	void Viewport::setOrtho( real left
		, real right
		, real bottom
		, real top
		, real near
		, real far )
	{
		m_type = ViewportType::eOrtho;
		m_fovY = Angle{};
		m_ratio = 0;
		m_left = left;
		m_right = right;
		m_bottom = bottom;
		m_top = top;
		m_near = near;
		m_far = far;
		m_modified = true;
	}

	void Viewport::resize( const castor::Size & value )
	{
		m_size = value;
		m_viewport = renderer::Viewport{ m_size[0], m_size[1], 0, 0 };
		m_scissor = renderer::Scissor{ 0, 0, m_size[0], m_size[1] };
	}
}
