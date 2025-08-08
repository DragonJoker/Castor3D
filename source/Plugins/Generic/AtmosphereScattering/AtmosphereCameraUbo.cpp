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
		: StructInstanceHelperT{ writer, c3d::move( expr ), enabled }
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

	c3d::MbString const CameraUbo::Buffer = "C3D_ATM_Camera";
	c3d::MbString const CameraUbo::Data = "d";

	CameraUbo::CameraUbo( c3d::RenderDevice const & device
		, bool & dirty )
		: m_device{ device }
		, m_ubo{ device.uboPool->getBuffer< Configuration >( c3d::MemoryPropertyFlags::eNone ) }
		, m_position{ dirty }
		, m_orientation{ dirty }
	{
	}

	CameraUbo::~CameraUbo()
	{
		m_device.uboPool->putBuffer( m_ubo );
	}

	void CameraUbo::cpuUpdate( c3d::Size const & renderSize
		, c3d::Camera const & camera
		, bool isSafeBanded
		, c3d::Point3f const & sunDirection
		, c3d::Vector3f const & planetPosition )
	{
		auto node = camera.getParent();
		auto & engine = *node->getScene()->getEngine();
		auto position = c3d::Vector3f::fromUnit( node->getDerivedPosition(), engine.getLengthUnit() ) - planetPosition;
		auto orientation = node->getDerivedOrientation();
		auto length = c3d::Length::fromUnit( 1.0f, engine.getLengthUnit() );

		auto kmPosition = position.kilometres();
		m_position = kmPosition;
		m_orientation = orientation;

		auto right{ c3d::Vector3f::fromKilometres( c3d::Point3f{ 1.0, 0.0, 0.0 } ) };
		auto up{ c3d::Vector3f::fromKilometres( c3d::Point3f{ 0.0, 1.0, 0.0 } ) };
		orientation.transform( right, right );
		orientation.transform( up, up );
		auto front{ c3d::point::cross( right, up ) };
		up = c3d::point::cross( front, right );

		auto proj = camera.getRescaledProjection( renderSize, length.kilometres(), isSafeBanded );

		auto & data = m_ubo.getData();
		data.position = m_position;

		position += planetPosition;
		c3d::Matrix4x4f view;
		c3d::matrix::lookAt( view
			, position.kilometres()
			, ( position + front ).kilometres()
			, up.kilometres() );
		auto viewProj = proj * view;
		data.camInvViewProj = viewProj.getInverse();

		viewProj = camera.getProjection( renderSize, isSafeBanded ) * camera.getView();
		data.objInvViewProj = viewProj.getInverse();

		data.lightDotCameraFront = c3d::point::dot( sunDirection
			, c3d::point::getNormalised( front.kilometres() ) );
		data.isLightInFront = data.lightDotCameraFront > 0.2f ? 1 : 0;
	}

	//************************************************************************************************
}
