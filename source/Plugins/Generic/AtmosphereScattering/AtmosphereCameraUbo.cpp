#include "AtmosphereScattering/AtmosphereCameraUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/Viewport.hpp>
#include <Castor3D/Scene/Camera.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/SceneNode.hpp>
#include <Castor3D/Buffer/UniformBufferPool.hpp>

#include <ShaderWriter/Source.hpp>

namespace atmosphere_scattering
{
	//*********************************************************************************************

	CameraData::CameraData( sdw::ShaderWriter & writer
		, ast::expr::ExprPtr expr
		, bool enabled )
		: StructInstanceHelperT{ writer, std::move( expr ), enabled }
	{
	}

	sdw::Vec4 CameraData::camProjToWorld( sdw::Vec4 const & pos )const
	{
		return camInvViewProj() * pos;
	}

	sdw::Vec4 CameraData::objProjToWorld( sdw::Vec4 const & pos )const
	{
		return objInvViewProj() * pos;
	}

	//************************************************************************************************

	castor::String const CameraUbo::Buffer = cuT( "C3D_ATM_Camera" );
	castor::String const CameraUbo::Data = cuT( "d" );

	CameraUbo::CameraUbo( castor3d::RenderDevice const & device
		, bool & dirty )
		: m_device{ device }
		, m_ubo{ device.uboPool->getBuffer< Configuration >( 0u ) }
		, m_position{ dirty }
		, m_orientation{ dirty }
	{
	}

	CameraUbo::~CameraUbo()
	{
		m_device.uboPool->putBuffer( m_ubo );
	}

	void CameraUbo::cpuUpdate( castor3d::Camera const & camera
		, bool isSafeBanded
		, castor::Point3f const & sunDirection
		, castor::Vector3f const & planetPosition )
	{
		auto node = camera.getParent();
		auto & engine = *node->getScene()->getEngine();
		auto position = node->getDerivedPosition() - planetPosition;
		auto orientation = node->getDerivedOrientation();
		auto length = castor::Length::fromUnit( 1.0f, engine.getLengthUnit() );

		auto kmPosition = position.kilometres();
		m_position = kmPosition;
		m_orientation = orientation;

		auto right{ castor::Vector3f::fromKilometres( castor::Point3f{ 1.0, 0.0, 0.0 } ) };
		auto up{ castor::Vector3f::fromKilometres( castor::Point3f{ 0.0, 1.0, 0.0 } ) };
		orientation.transform( right, right );
		orientation.transform( up, up );
		auto front{ castor::point::cross( right, up ) };
		up = castor::point::cross( front, right );

		position += planetPosition;
		auto proj = camera.getRescaledProjection( length.kilometres(), isSafeBanded );
		castor::Matrix4x4f view;
		castor::matrix::lookAt( view
			, position.kilometres()
			, ( position + front ).kilometres()
			, up.kilometres() );
		auto viewProj = proj * view;

		auto & data = m_ubo.getData();
		data.position = m_position;
		data.camInvViewProj = viewProj.getInverse();

		viewProj = camera.getProjection( isSafeBanded ) * camera.getView();
		data.objInvViewProj = viewProj.getInverse();

		data.lightDotCameraFront = castor::point::dot( sunDirection
			, castor::point::getNormalised( front.kilometres() ) );
		data.isLightInFront = data.lightDotCameraFront > 0.2f ? 1 : 0;
	}

	//************************************************************************************************
}
