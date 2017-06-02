#include "NodeState.hpp"

#include <Event/Frame/FrameListener.hpp>
#include <Event/Frame/FunctorEvent.hpp>
#include <Scene/SceneNode.hpp>

using namespace Castor;
using namespace Castor3D;

namespace GuiCommon
{
	namespace
	{
		float GetValue( Angle const & p_value )
		{
			return p_value.degrees();
		}

		float GetValue( float const & p_value )
		{
			return p_value;
		}

		template< typename T >
		RangedValue< T > doUpdateVelocity( RangedValue< T > p_velocity )
		{
			static T const l_zero{};
			auto l_ret = p_velocity.value() / 1.2f;

			if ( std::abs( GetValue( l_ret ) ) < 0.2f )
			{
				l_ret = l_zero;
			}

			return RangedValue< T >( l_ret, p_velocity.range() );
		}
	}

	NodeState::NodeState( FrameListener & p_listener
		, Castor3D::SceneNodeSPtr p_node )
		: m_listener{ p_listener }
		, m_node{ p_node }
		, m_originalOrientation{ p_node->GetOrientation() }
		, m_originalPosition{ p_node->GetPosition() }
	{
	}

	void NodeState::Reset( float p_speed )
	{
		SetMaxSpeed( p_speed );
		m_angularVelocityX = 0.0_degrees;
		m_angularVelocityY = 0.0_degrees;
		m_angles = Angles{ { 0.0_radians, 0.0_radians } };
		m_scalarVelocityX = 0.0f;
		m_scalarVelocityY = 0.0f;
		m_scalarVelocityZ = 0.0f;

		m_listener.PostEvent( MakeFunctorEvent( EventType::ePostRender
			, [this]()
			{
				m_node->SetOrientation( m_originalOrientation );
				m_node->SetPosition( m_originalPosition );
			} ) );
	}

	void NodeState::SetMaxSpeed( float p_speed )
	{
		m_angularVelocityX.update_range( make_range( Angle::from_degrees( -p_speed * 2 )
			, Angle::from_degrees( p_speed * 2 ) ) );
		m_angularVelocityY.update_range( make_range( Angle::from_degrees( -p_speed * 2 )
			, Angle::from_degrees( p_speed * 2 ) ) );
		m_scalarVelocityX.update_range( make_range( -p_speed, p_speed ) );
		m_scalarVelocityY.update_range( make_range( -p_speed, p_speed ) );
		m_scalarVelocityZ.update_range( make_range( -p_speed, p_speed ) );
	}

	bool NodeState::Update()
	{
		auto l_angles = m_angles;
		m_angles[0] += m_angularVelocityX.value();
		m_angles[1] += m_angularVelocityY.value();
		m_angularVelocityX = doUpdateVelocity( m_angularVelocityX );
		m_angularVelocityY = doUpdateVelocity( m_angularVelocityY );
		Point3r l_translate;
		l_translate[0] = m_scalarVelocityX.value();
		l_translate[1] = m_scalarVelocityY.value();
		l_translate[2] = m_scalarVelocityZ.value();
		m_scalarVelocityX = doUpdateVelocity( m_scalarVelocityX );
		m_scalarVelocityY = doUpdateVelocity( m_scalarVelocityY );
		m_scalarVelocityZ = doUpdateVelocity( m_scalarVelocityZ );

		bool l_result{ l_angles[0] != m_angles[0] || l_angles[1] != m_angles[1] };

		if ( l_translate != Point3r{} )
		{
			auto l_orientation = m_node->GetOrientation();
			Point3r l_right{ 1.0_r, 0.0_r, 0.0_r };
			Point3r l_up{ 0.0_r, 1.0_r, 0.0_r };
			Point3r l_front{ 0.0_r, 0.0_r, 1.0_r };
			l_orientation.transform( l_right, l_right );
			l_orientation.transform( l_up, l_up );
			l_orientation.transform( l_front, l_front );
			l_translate = ( l_right * l_translate[0] ) + ( l_up * l_translate[1] ) + ( l_front * l_translate[2] );
			l_result = true;
		}

		if ( l_result )
		{
			l_angles = m_angles;
			m_listener.PostEvent( MakeFunctorEvent( EventType::ePostRender
			, [this, l_translate, l_angles]()
			{
				m_node->Translate( l_translate );

				Quaternion l_x{ Quaternion::from_axis_angle( Point3r{ 1.0, 0.0, 0.0 }, l_angles[0] ) };
				Quaternion l_y{ Quaternion::from_axis_angle( Point3r{ 0.0, 1.0, 0.0 }, l_angles[1] ) };
				m_node->SetOrientation( m_originalOrientation * l_y * l_x );
			} ) );
		}

		return l_result;
	}

	void NodeState::SetAngularVelocity( Castor::Point2r const & p_value )noexcept
	{
		m_angularVelocityX = Castor::Angle::from_degrees( p_value[0] );
		m_angularVelocityY = Castor::Angle::from_degrees( p_value[1] );
	}

	 void NodeState::SetScalarVelocity( Castor::Point3r const & p_value )noexcept
	{
		m_scalarVelocityX = p_value[0];
		m_scalarVelocityY = p_value[1];
		m_scalarVelocityZ = p_value[2];
	}

	void NodeState::AddAngularVelocity( Castor::Point2r const & p_value )noexcept
	{
		m_angularVelocityX += Castor::Angle::from_degrees( p_value[0] );
		m_angularVelocityY += Castor::Angle::from_degrees( p_value[1] );
	}

	void NodeState::AddScalarVelocity( Castor::Point3r const & p_value )noexcept
	{
		m_scalarVelocityX += p_value[0];
		m_scalarVelocityY += p_value[1];
		m_scalarVelocityZ += p_value[2];
	}
}
