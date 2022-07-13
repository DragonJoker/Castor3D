#include "AtmosphereScattering/AtmosphereCameraUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/Viewport.hpp>
#include <Castor3D/Scene/Camera.hpp>
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

	sdw::Vec4 CameraData::camWorldToProj( sdw::Vec4 const & pos )const
	{
		return camViewProj() * pos;
	}

	sdw::Vec4 CameraData::objProjToWorld( sdw::Vec4 const & pos )const
	{
		return objInvViewProj() * pos;
	}

	//************************************************************************************************

	castor::String const CameraUbo::Buffer = cuT( "Camera" );
	castor::String const CameraUbo::Data = cuT( "c3d_cameraData" );

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
		, castor::Point3f const & sunDirection
		, bool isSafeBanded )
	{
		auto node = camera.getParent();
		auto position = node->getDerivedPosition();
		auto orientation = node->getDerivedOrientation();
		m_position = position;
		m_orientation = orientation;

		// Convert from meters to kilometers
		position = position / 1000.0f;

		castor::Point3f right{ 1.0, 0.0, 0.0 };
		castor::Point3f up{ 0.0, 1.0, 0.0 };
		orientation.transform( right, right );
		orientation.transform( up, up );
		castor::Point3f front{ castor::point::cross( right, up ) };
		up = castor::point::cross( front, right );
		castor::Matrix4x4f view;
		castor::matrix::lookAt( view, position, position + front, up );

		auto viewProj = camera.getProjection( isSafeBanded ) * view;
		auto & data = m_ubo.getData();
		data.camViewProj = viewProj;
		data.camInvViewProj = viewProj.getInverse();
		data.camInvProj = camera.getProjection( isSafeBanded ).getInverse();
		data.camInvView = view.getInverse();

		viewProj = camera.getProjection( isSafeBanded ) * camera.getView();
		data.objInvViewProj = viewProj.getInverse();
		data.position = position;

		data.lightDotCameraFront = castor::point::dot( sunDirection
			, castor::point::getNormalised( front ) );
		data.isLightInFront = data.lightDotCameraFront > 0.2f ? 1 : 0;
	}

	//************************************************************************************************
}
