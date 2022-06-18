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
		: sdw::StructInstance{ writer, std::move( expr ), enabled }
		, camInvViewProj{ getMember< sdw::Mat4 >( "camInvViewProj" ) }
		, objInvViewProj{ getMember< sdw::Mat4 >( "objInvViewProj" ) }
		, position{ getMember< sdw::Vec3 >( "position" ) }
	{
	}

	ast::type::BaseStructPtr CameraData::makeType( ast::type::TypesCache & cache )
	{
		auto result = cache.getStruct( sdw::type::MemoryLayout::eStd140
			, "CameraData" );

		if ( result->empty() )
		{
			result->declMember( "camInvViewProj", sdw::type::Kind::eMat4x4F, sdw::type::NotArray );
			result->declMember( "objInvViewProj", sdw::type::Kind::eMat4x4F, sdw::type::NotArray );
			result->declMember( "position", sdw::type::Kind::eVec3F, sdw::type::NotArray );
		}

		return result;
	}

	sdw::Vec4 CameraData::camProjToWorld( sdw::Vec4 const & pos )const
	{
		return camInvViewProj * pos;
	}

	sdw::Vec4 CameraData::objProjToWorld( sdw::Vec4 const & pos )const
	{
		return objInvViewProj * pos;
	}

	//************************************************************************************************

	castor::String const CameraUbo::Buffer = cuT( "Camera" );
	castor::String const CameraUbo::Data = cuT( "c3d_cameraData" );

	CameraUbo::CameraUbo( castor3d::RenderDevice const & device )
		: m_device{ device }
		, m_ubo{ device.uboPool->getBuffer< Configuration >( 0u ) }
	{
	}

	CameraUbo::~CameraUbo()
	{
		m_device.uboPool->putBuffer( m_ubo );
	}

	void CameraUbo::cpuUpdate( castor3d::Camera const & camera
		, bool isSafeBanded )
	{
		auto node = camera.getParent();

		// Convert from meters to kilometers
		auto position = node->getDerivedPosition() / 1000.0f;

		auto orientation = node->getDerivedOrientation();
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
		data.camInvViewProj = viewProj.getInverse();

		viewProj = camera.getProjection( isSafeBanded ) * camera.getView();
		data.objInvViewProj = viewProj.getInverse();
		data.position = position;
	}

	//************************************************************************************************
}
