#include "GuiCommon/System/NodeState.hpp"

#include <Castor3D/Event/Frame/FrameListener.hpp>
#include <Castor3D/Event/Frame/CpuFunctorEvent.hpp>
#include <Castor3D/Scene/SceneNode.hpp>

namespace GuiCommon
{
	namespace
	{
		float getValue( castor::Angle const & value )
		{
			return value.degrees();
		}

		float getValue( float const & value )
		{
			return value;
		}

		template< typename T >
		castor::RangedValue< T > doUpdateVelocity( castor::RangedValue< T > velocity
			, castor::Nanoseconds const & tslf )
		{
			static T const zero{};
			static castor::Nanoseconds const tick{ 25_ms };
			auto ret = velocity.value() / ( 1.0f + ( 0.2f * float( tslf.count() ) / float( tick.count() ) ) );

			if ( std::abs( getValue( ret ) ) < 0.002f )
			{
				ret = zero;
			}

			return castor::RangedValue< T >( ret, velocity.range() );
		}
	}

	NodeState::NodeState( castor3d::FrameListener & listener
		, castor3d::SceneNodeRPtr node
		, bool camera )
		: m_listener{ listener }
		, m_node{ node }
		, m_originalPosition{ node->getPosition() }
		, m_originalOrientation{ node->getOrientation() }
		, m_originalAngles
		{
			m_originalOrientation.getPitch(),
			m_originalOrientation.getYaw(),
			camera ? castor::Angle{} : m_originalOrientation.getRoll(),
		}
		, m_angles{ m_originalAngles }
		, m_isCamera{ camera }
	{
	}

	void NodeState::start()
	{
		m_timer.getElapsed();
		m_running = true;
		doUpdate();
	}

	void NodeState::stop()
	{
		m_running = false;
	}

	void NodeState::reset( float speed )
	{
		setMaxSpeed( speed );
		m_timer.getElapsed();
		m_angularVelocityX = 0.0_degrees;
		m_angularVelocityY = 0.0_degrees;
		m_angles = m_originalAngles;
		m_scalarVelocityX = 0.0f;
		m_scalarVelocityY = 0.0f;
		m_scalarVelocityZ = 0.0f;

		m_listener.postEvent( castor3d::makeCpuFunctorEvent( castor3d::CpuEventType::ePostCpuStep
			, [this]()
			{
				m_node->setOrientation( m_originalOrientation );
				m_node->setPosition( m_originalPosition );
			} ) );
	}

	void NodeState::setMaxSpeed( float speed )
	{
		m_scalarVelocityX.updateRange( castor::makeRange( -speed, speed ) );
		m_scalarVelocityY.updateRange( castor::makeRange( -speed, speed ) );
		m_scalarVelocityZ.updateRange( castor::makeRange( -speed, speed ) );
	}

	bool NodeState::doUpdateVelocities( castor::Point3f & translate
		, Angles & angles )
	{
		static castor::Nanoseconds const tick{ 2_ms };

		auto lock = castor::makeUniqueLock( m_mutex );
		auto tslf = m_timer.getElapsed();
		auto ratio = float( tslf.count() ) / float( tick.count() );

		angles = m_angles;
		m_angles[0] += m_angularVelocityX.value() * ratio;
		m_angles[1] += m_angularVelocityY.value() * ratio;
		m_angularVelocityX = doUpdateVelocity( m_angularVelocityX, tslf );
		m_angularVelocityY = doUpdateVelocity( m_angularVelocityY, tslf );

		translate[0] = m_scalarVelocityX.value() * ratio;
		translate[1] = m_scalarVelocityY.value() * ratio;
		translate[2] = m_scalarVelocityZ.value() * ratio;
		m_scalarVelocityX = doUpdateVelocity( m_scalarVelocityX, tslf );
		m_scalarVelocityY = doUpdateVelocity( m_scalarVelocityY, tslf );
		m_scalarVelocityZ = doUpdateVelocity( m_scalarVelocityZ, tslf );

		bool result{ angles[0] != m_angles[0] || angles[1] != m_angles[1] };

		if ( translate != castor::Point3f{} )
		{
			auto orientation = m_node->getOrientation();
			castor::Point3f right{ 1.0f, 0.0f, 0.0f };
			castor::Point3f up{ 0.0f, 1.0f, 0.0f };
			castor::Point3f front{ 0.0f, 0.0f, 1.0f };
			orientation.transform( right, right );
			orientation.transform( up, up );
			orientation.transform( front, front );
			translate = ( right * translate[0] ) + ( up * translate[1] ) + ( front * translate[2] );
			result = true;
		}

		if ( result )
		{
			angles = m_angles;
		}

		return result;
	}

	void NodeState::doUpdate()
	{
		m_listener.postEvent( castor3d::makeCpuFunctorEvent( castor3d::CpuEventType::ePostCpuStep
			, [this]()
			{
				castor::Point3f translate;
				Angles angles;

				if ( doUpdateVelocities( translate, angles ) )
				{
					m_node->translate( translate );

					castor::Quaternion pitch{ castor::Quaternion::fromAxisAngle( castor::Point3f{ 1.0, 0.0, 0.0 }, angles[0] ) };
					castor::Quaternion yaw{ castor::Quaternion::fromAxisAngle( castor::Point3f{ 0.0, 1.0, 0.0 }, angles[1] ) };
					castor::Quaternion roll{ castor::Quaternion::fromAxisAngle( castor::Point3f{ 0.0, 0.0, 1.0 }, angles[2] ) };
					m_node->setOrientation( roll * yaw * pitch );
				}

				if ( m_running )
				{
					doUpdate();
				}
			} ) );
	}

	void NodeState::setAngularVelocity( castor::Point2f const & value )noexcept
	{
		auto lock = castor::makeUniqueLock( m_mutex );
		m_angularVelocityX = castor::Angle::fromDegrees( value[0] );
		m_angularVelocityY = castor::Angle::fromDegrees( value[1] );
	}

	 void NodeState::setScalarVelocity( castor::Point3f const & value )noexcept
	{
		 auto lock = castor::makeUniqueLock( m_mutex );
		m_scalarVelocityX = value[0];
		m_scalarVelocityY = value[1];
		m_scalarVelocityZ = value[2];
	}

	void NodeState::addAngularVelocity( castor::Point2f const & value )noexcept
	{
		auto lock = castor::makeUniqueLock( m_mutex );
		m_angularVelocityX += castor::Angle::fromDegrees( value[0] );
		m_angularVelocityY += castor::Angle::fromDegrees( value[1] );
	}

	void NodeState::addScalarVelocity( castor::Point3f const & value )noexcept
	{
		auto lock = castor::makeUniqueLock( m_mutex );
		m_scalarVelocityX += value[0];
		m_scalarVelocityY += value[1];
		m_scalarVelocityZ += value[2];
	}

	void NodeState::pitch( castor::Angle const & value )noexcept
	{
		auto lock = castor::makeUniqueLock( m_mutex );
		m_angles[0] += value;
		m_angularVelocityX = 0.00_degrees;
		m_listener.postEvent( castor3d::makeCpuFunctorEvent( castor3d::CpuEventType::ePostCpuStep
			, [this, value]()
			{
				castor::Quaternion orientation{ m_node->getOrientation() };
				orientation *= castor::Quaternion::fromAxisAngle( castor::Point3f{ 1.0f, 0.0f, 0.0f }, value );
				m_node->setOrientation( orientation );
			} ) );
	}

	void NodeState::yaw( castor::Angle const & value )noexcept
	{
		auto lock = castor::makeUniqueLock( m_mutex );
		m_angles[1] += value;
		m_angularVelocityY = 0.00_degrees;
		m_listener.postEvent( castor3d::makeCpuFunctorEvent( castor3d::CpuEventType::ePostCpuStep
			, [this, value]()
			{
				castor::Quaternion orientation{ m_node->getOrientation() };
				orientation *= castor::Quaternion::fromAxisAngle( castor::Point3f{ 0.0f, 1.0f, 0.0f }, value );
				m_node->setOrientation( orientation );
			} ) );
	}
}
