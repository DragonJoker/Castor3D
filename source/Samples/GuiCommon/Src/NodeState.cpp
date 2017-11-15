#include "NodeState.hpp"

#include <Event/Frame/FrameListener.hpp>
#include <Event/Frame/FunctorEvent.hpp>
#include <Scene/SceneNode.hpp>

using namespace castor;
using namespace castor3d;

namespace GuiCommon
{
	namespace
	{
		float getValue( Angle const & p_value )
		{
			return p_value.degrees();
		}

		float getValue( float const & p_value )
		{
			return p_value;
		}

		template< typename T >
		RangedValue< T > doUpdateVelocity( RangedValue< T > p_velocity )
		{
			static T const zero{};
			auto ret = p_velocity.value() / 1.2f;

			if ( std::abs( getValue( ret ) ) < 0.2f )
			{
				ret = zero;
			}

			return RangedValue< T >( ret, p_velocity.range() );
		}
	}

	NodeState::NodeState( FrameListener & p_listener
		, castor3d::SceneNodeSPtr p_node )
		: m_listener{ p_listener }
		, m_node{ p_node }
		, m_originalOrientation{ p_node->getOrientation() }
		, m_originalPosition{ p_node->getPosition() }
	{
	}

	void NodeState::reset( float p_speed )
	{
		setMaxSpeed( p_speed );
		m_angularVelocityX = 0.0_degrees;
		m_angularVelocityY = 0.0_degrees;
		m_angles = Angles{ { 0.0_radians, 0.0_radians } };
		m_scalarVelocityX = 0.0f;
		m_scalarVelocityY = 0.0f;
		m_scalarVelocityZ = 0.0f;

		m_listener.postEvent( makeFunctorEvent( EventType::ePostRender
			, [this]()
			{
				m_node->setOrientation( m_originalOrientation );
				m_node->setPosition( m_originalPosition );
			} ) );
	}

	void NodeState::setMaxSpeed( float p_speed )
	{
		m_angularVelocityX.updateRange( makeRange( Angle::fromDegrees( -p_speed * 2 )
			, Angle::fromDegrees( p_speed * 2 ) ) );
		m_angularVelocityY.updateRange( makeRange( Angle::fromDegrees( -p_speed * 2 )
			, Angle::fromDegrees( p_speed * 2 ) ) );
		m_scalarVelocityX.updateRange( makeRange( -p_speed, p_speed ) );
		m_scalarVelocityY.updateRange( makeRange( -p_speed, p_speed ) );
		m_scalarVelocityZ.updateRange( makeRange( -p_speed, p_speed ) );
	}

	bool NodeState::update()
	{
		auto angles = m_angles;
		m_angles[0] += m_angularVelocityX.value();
		m_angles[1] += m_angularVelocityY.value();
		m_angularVelocityX = doUpdateVelocity( m_angularVelocityX );
		m_angularVelocityY = doUpdateVelocity( m_angularVelocityY );
		Point3r translate;
		translate[0] = m_scalarVelocityX.value();
		translate[1] = m_scalarVelocityY.value();
		translate[2] = m_scalarVelocityZ.value();
		m_scalarVelocityX = doUpdateVelocity( m_scalarVelocityX );
		m_scalarVelocityY = doUpdateVelocity( m_scalarVelocityY );
		m_scalarVelocityZ = doUpdateVelocity( m_scalarVelocityZ );

		bool result{ angles[0] != m_angles[0] || angles[1] != m_angles[1] };

		if ( translate != Point3r{} )
		{
			auto orientation = m_node->getOrientation();
			Point3r right{ 1.0_r, 0.0_r, 0.0_r };
			Point3r up{ 0.0_r, 1.0_r, 0.0_r };
			Point3r front{ 0.0_r, 0.0_r, 1.0_r };
			orientation.transform( right, right );
			orientation.transform( up, up );
			orientation.transform( front, front );
			translate = ( right * translate[0] ) + ( up * translate[1] ) + ( front * translate[2] );
			result = true;
		}

		if ( result )
		{
			angles = m_angles;
			m_listener.postEvent( makeFunctorEvent( EventType::ePostRender
			, [this, translate, angles]()
			{
				m_node->translate( translate );

				Quaternion x{ Quaternion::fromAxisAngle( Point3r{ 1.0, 0.0, 0.0 }, angles[0] ) };
				Quaternion y{ Quaternion::fromAxisAngle( Point3r{ 0.0, 1.0, 0.0 }, angles[1] ) };
				m_node->setOrientation( m_originalOrientation * y * x );
			} ) );
		}

		return result;
	}

	void NodeState::setAngularVelocity( castor::Point2r const & p_value )noexcept
	{
		m_angularVelocityX = castor::Angle::fromDegrees( p_value[0] );
		m_angularVelocityY = castor::Angle::fromDegrees( p_value[1] );
	}

	 void NodeState::setScalarVelocity( castor::Point3r const & p_value )noexcept
	{
		m_scalarVelocityX = p_value[0];
		m_scalarVelocityY = p_value[1];
		m_scalarVelocityZ = p_value[2];
	}

	void NodeState::addAngularVelocity( castor::Point2r const & p_value )noexcept
	{
		m_angularVelocityX += castor::Angle::fromDegrees( p_value[0] );
		m_angularVelocityY += castor::Angle::fromDegrees( p_value[1] );
	}

	void NodeState::addScalarVelocity( castor::Point3r const & p_value )noexcept
	{
		m_scalarVelocityX += p_value[0];
		m_scalarVelocityY += p_value[1];
		m_scalarVelocityZ += p_value[2];
	}
}
