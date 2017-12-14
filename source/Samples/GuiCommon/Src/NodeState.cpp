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
		float getValue( Angle const & value )
		{
			return value.degrees();
		}

		float getValue( float const & value )
		{
			return value;
		}

		template< typename T >
		RangedValue< T > doUpdateVelocity( RangedValue< T > velocity )
		{
			static T const zero{};
			auto ret = velocity.value() / 1.2f;

			if ( std::abs( getValue( ret ) ) < 0.2f )
			{
				ret = zero;
			}

			return RangedValue< T >( ret, velocity.range() );
		}
	}

	NodeState::NodeState( FrameListener & listener
		, SceneNodeSPtr node )
		: m_listener{ listener }
		, m_node{ node }
		, m_originalOrientation{ node->getOrientation() }
		, m_originalPosition{ node->getPosition() }
		, m_originalAngles
		{
			m_originalOrientation.getPitch(),
			m_originalOrientation.getYaw(),
		}
		, m_angles{ m_originalAngles }
	{
	}

	void NodeState::reset( float speed )
	{
		setMaxSpeed( speed );
		m_angularVelocityX = 0.0_degrees;
		m_angularVelocityY = 0.0_degrees;
		m_angles = m_originalAngles;
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

	void NodeState::setMaxSpeed( float speed )
	{
		m_angularVelocityX.updateRange( makeRange( Angle::fromDegrees( -speed * 2 )
			, Angle::fromDegrees( speed * 2 ) ) );
		m_angularVelocityY.updateRange( makeRange( Angle::fromDegrees( -speed * 2 )
			, Angle::fromDegrees( speed * 2 ) ) );
		m_scalarVelocityX.updateRange( makeRange( -speed, speed ) );
		m_scalarVelocityY.updateRange( makeRange( -speed, speed ) );
		m_scalarVelocityZ.updateRange( makeRange( -speed, speed ) );
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

				Quaternion pitch{ Quaternion::fromAxisAngle( Point3r{ 1.0, 0.0, 0.0 }, angles[0] ) };
				Quaternion yaw{ Quaternion::fromAxisAngle( Point3r{ 0.0, 1.0, 0.0 }, angles[1] ) };
				m_node->setOrientation( yaw * pitch );
			} ) );
		}

		return result;
	}

	void NodeState::setAngularVelocity( Point2r const & value )noexcept
	{
		m_angularVelocityX = Angle::fromDegrees( value[0] );
		m_angularVelocityY = Angle::fromDegrees( value[1] );
	}

	 void NodeState::setScalarVelocity( Point3r const & value )noexcept
	{
		m_scalarVelocityX = value[0];
		m_scalarVelocityY = value[1];
		m_scalarVelocityZ = value[2];
	}

	void NodeState::addAngularVelocity( Point2r const & value )noexcept
	{
		m_angularVelocityX += Angle::fromDegrees( value[0] );
		m_angularVelocityY += Angle::fromDegrees( value[1] );
	}

	void NodeState::addScalarVelocity( Point3r const & value )noexcept
	{
		m_scalarVelocityX += value[0];
		m_scalarVelocityY += value[1];
		m_scalarVelocityZ += value[2];
	}
}
