#include "Castor3D/Shader/Ubos/CameraUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Render/Frustum.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <ashespp/Buffer/StagingBuffer.hpp>

#include <ShaderWriter/Source.hpp>

namespace castor3d
{
	//*********************************************************************************************

	namespace shader
	{
		sdw::Vec4 CameraData::projToView( sdw::Vec4 const & psPosition )const
		{
			return invProjection() * psPosition;
		}

		sdw::Vec4 CameraData::viewToProj( sdw::Vec4 const & vsPosition )const
		{
			return projection() * vsPosition;
		}

		sdw::Vec4 CameraData::worldToCurView( sdw::Vec4 const & wsPosition )const
		{
			return curView() * wsPosition;
		}

		sdw::Vec4 CameraData::worldToPrvView( sdw::Vec4 const & wsPosition )const
		{
			return prvView() * wsPosition;
		}

		sdw::Vec4 CameraData::curViewToWorld( sdw::Vec4 const & vsPosition )const
		{
			return invCurView() * vsPosition;
		}

		sdw::Vec4 CameraData::prvViewToWorld( sdw::Vec4 const & vsPosition )const
		{
			return invPrvView() * vsPosition;
		}

		sdw::Vec4 CameraData::worldToCurProj( sdw::Vec4 const & wsPosition )const
		{
			return curViewProj() * wsPosition;
		}

		sdw::Vec4 CameraData::worldToPrvProj( sdw::Vec4 const & wsPosition )const
		{
			return prvViewProj() * wsPosition;
		}

		sdw::Vec2 CameraData::viewToScreenUV( Utils & utils
			, sdw::Vec4 vsPosition )const
		{
			return utils.clipToScreen( viewToProj( vsPosition ) ).xy();
		}

		sdw::Vec2 CameraData::worldToCurScreenUV( Utils & utils
			, sdw::Vec4 wsPosition )const
		{
			return utils.clipToScreen( worldToCurProj( wsPosition ) ).xy();
		}

		sdw::Vec2 CameraData::worldToPrvScreenUV( Utils & utils
			, sdw::Vec4 wsPosition )const
		{
			return utils.clipToScreen( worldToPrvProj( wsPosition ) ).xy();
		}

		sdw::Vec3 CameraData::projToView( Utils & utils
			, sdw::Vec2 const & texCoord
			, sdw::Float const & depth )const
		{
			return utils.calcVSPosition( texCoord, depth, invProjection() );
		}

		sdw::Vec4 CameraData::curProjToWorld( sdw::Vec4 const & position )const
		{
			return invCurViewProj() * position;
		}

		sdw::Vec3 CameraData::curProjToWorld( Utils & utils
			, sdw::Vec2 const & texCoord
			, sdw::Float const & depth )const
		{
			return utils.calcWSPosition( texCoord, depth, invCurViewProj() );
		}

		sdw::Vec3 CameraData::prvProjToWorld( Utils & utils
			, sdw::Vec2 const & texCoord
			, sdw::Float const & depth )const
		{
			return utils.calcWSPosition( texCoord, depth, invPrvViewProj() );
		}

		sdw::Vec3 CameraData::getCurViewRight()const
		{
			return vec3( curView()[0][0], curView()[1][0], curView()[2][0] );
		}

		sdw::Vec3 CameraData::getPrvViewRight()const
		{
			return vec3( prvView()[0][0], prvView()[1][0], prvView()[2][0] );
		}

		sdw::Vec3 CameraData::getCurViewUp()const
		{
			return vec3( curView()[0][1], curView()[1][1], curView()[2][1] );
		}

		sdw::Vec3 CameraData::getPrvViewUp()const
		{
			return vec3( prvView()[0][1], prvView()[1][1], prvView()[2][1] );
		}

		sdw::Vec3 CameraData::getCurViewCenter()const
		{
			return curView()[3].xyz();
		}

		sdw::Vec3 CameraData::getPrvViewCenter()const
		{
			return prvView()[3].xyz();
		}

		sdw::Mat4 CameraData::getInvViewProjMtx()const
		{
			return invCurViewProj();
		}

		void CameraData::jitter( sdw::Vec4 & csPosition )const
		{
			csPosition.xy() -= jitter() * csPosition.w();
		}

		sdw::Vec3 CameraData::transformCamera( sdw::Mat3 const & transform )const
		{
			return transform * position();
		}

		sdw::Vec3 CameraData::getPosToCamera( sdw::Vec3 const & wsPosition )const
		{
			return position() - wsPosition;
		}

		sdw::Vec3 CameraData::getCameraToPos( sdw::Vec3 const & wsPosition )const
		{
			return wsPosition - position();
		}

		sdw::Vec2 CameraData::depthPlanes()const
		{
			return vec2( nearPlane(), farPlane() );
		}

		sdw::Vec2 CameraData::calcTexCoord( Utils & utils
			, sdw::Vec2 const & fragCoord )const
		{
			return utils.calcTexCoord( fragCoord
				, vec2( renderSize() ) );
		}

		sdw::Vec3 CameraData::readNormal( sdw::Vec3 const & input )const
		{
			return -( transpose( invCurView() ) * vec4( input, 1.0_f ) ).xyz();
		}

		sdw::Vec3 CameraData::writeNormal( sdw::Vec3 const & input )const
		{
			return ( transpose( invCurView() ) * vec4( -input, 1.0_f ) ).xyz();
		}
	}

	//*********************************************************************************************

	CameraUbo::CameraUbo( RenderDevice const & device )
		: m_device{ device }
		, m_ubo{ m_device.uboPool->getBuffer< Configuration >( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT ) }
	{
	}

	CameraUbo::~CameraUbo()
	{
		m_device.uboPool->putBuffer( m_ubo );
	}

	CameraUbo::Configuration & CameraUbo::cpuUpdate( Camera const & camera
		, castor::Matrix4x4f const & view
		, castor::Matrix4x4f const & projection
		, castor::Size const & size
		, castor::Point2f const & jitter )
	{
		auto & configuration = cpuUpdate( view
			, projection
			, camera.getFrustum()
			, size
			, jitter );
		configuration.position = camera.getParent()->getDerivedPosition();
		configuration.gamma = camera.getHdrConfig().gamma;
		configuration.nearPlane = camera.getNear();
		configuration.farPlane = camera.getFar();

		return configuration;
	}

	CameraUbo::Configuration & CameraUbo::cpuUpdate( Camera const & camera
		, bool safeBanded
		, castor::Matrix4x4f const & view
		, castor::Matrix4x4f const & projection
		, castor::Point2f const & jitter )
	{
		return cpuUpdate( camera
			, view
			, projection
			, ( safeBanded
				? getSafeBandedSize( camera.getSize() )
				: camera.getSize() )
			, jitter );
	}

	CameraUbo::Configuration & CameraUbo::cpuUpdate( Camera const & camera
		, bool safeBanded
		, castor::Size const & size
		, castor::Point2f const & jitter )
	{
		return cpuUpdate( camera
			, camera.getView()
			, camera.getProjection( safeBanded )
			, size
			, jitter );
	}

	CameraUbo::Configuration & CameraUbo::cpuUpdate( Camera const & camera
		, bool safeBanded
		, castor::Point2f const & jitter )
	{
		return cpuUpdate( camera
			, safeBanded
			, ( safeBanded
				? getSafeBandedSize( camera.getSize() )
				: camera.getSize() )
			, jitter );
	}

	CameraUbo::Configuration & CameraUbo::cpuUpdate( castor::Matrix4x4f const & view
		, castor::Matrix4x4f const & projection
		, Frustum const & frustum
		, castor::Size const & size
		, castor::Point2f const & jitter )
	{
		auto & configuration = cpuUpdate( projection );
		configuration.prvView = configuration.curView;
		configuration.invPrvView = configuration.invCurView;
		configuration.prvViewProj = configuration.curViewProj;
		configuration.invPrvViewProj = configuration.invCurViewProj;
		configuration.curView = view;
		configuration.invCurView = view.getInverse();
		configuration.projection = projection;
		configuration.invProjection = projection.getInverse();
		configuration.curViewProj = projection * view;
		configuration.invCurViewProj = ( configuration.curViewProj ).getInverse();
		configuration.size = { size.getWidth(), size.getHeight() };
		configuration.jitter = { jitter->x, jitter->y };

		auto itSrc = frustum.getPlanes().begin();
		auto itDst = configuration.frustumPlanes.begin();

		while ( itSrc != frustum.getPlanes().end() )
		{
			*itDst = { itSrc->getNormal()->x, itSrc->getNormal()->y, itSrc->getNormal()->z, itSrc->getDistance() };
			++itSrc;
			++itDst;
		}

		return configuration;
	}

	CameraUbo::Configuration & CameraUbo::cpuUpdate( castor::Matrix4x4f const & projection )
	{
		CU_Require( m_ubo );
		auto & configuration = m_ubo.getData();
		configuration.projection = projection;
		configuration.invProjection = projection.getInverse();
		return configuration;
	}

	//*********************************************************************************************
}
