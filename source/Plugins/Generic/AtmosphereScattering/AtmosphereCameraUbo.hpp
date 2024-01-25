/*
See LICENSE file in root folder
*/
#ifndef ___C3DAS_AtmosphereCameraUbo_H___
#define ___C3DAS_AtmosphereCameraUbo_H___

#include "AtmosphereScatteringPrerequisites.hpp"

#include <Castor3D/Castor3DModule.hpp>
#include <Castor3D/Render/RenderModule.hpp>

#include <Castor3D/Buffer/UniformBufferOffset.hpp>

#include <ShaderWriter/BaseTypes/Int.hpp>
#include <ShaderWriter/BaseTypes/Float.hpp>
#include <ShaderWriter/CompositeTypes/StructHelper.hpp>
#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>
#include <ShaderWriter/MatTypes/Mat4.hpp>
#include <ShaderWriter/VecTypes/Vec4.hpp>

namespace atmosphere_scattering
{
	struct CameraConfig
	{
		castor::Matrix4x4f camInvViewProj;
		castor::Matrix4x4f objInvViewProj;
		castor::Point3f position;
		float lightDotCameraFront;
		int32_t isLightInFront;
		castor::Point3i pad;
	};

	struct CameraData
		: public sdw::StructInstanceHelperT< "C3D_ATM_CameraData"
			, sdw::type::MemoryLayout::eStd140
			, sdw::Mat4Field< "camInvViewProj" >
			, sdw::Mat4Field< "objInvViewProj" >
			, sdw::Vec3Field< "position" >
			, sdw::FloatField< "lightDotCameraFront" >
			, sdw::IntField< "isLightInFront" >
			, sdw::IntField< "pad0" >
			, sdw::IntField< "pad1" >
			, sdw::IntField< "pad2" > >
	{
		CameraData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled );

		sdw::Vec4 camProjToWorld( sdw::Vec4 const & pos )const;
		sdw::Vec4 objProjToWorld( sdw::Vec4 const & pos )const;

		auto position()const { return getMember< "position" >(); }
		auto isLightInFront()const { return getMember< "isLightInFront" >(); }
		auto lightDotCameraFront()const { return getMember< "lightDotCameraFront" >(); }
		auto camInvViewProj()const { return getMember< "camInvViewProj" >(); }
		auto objInvViewProj()const { return getMember< "objInvViewProj" >(); }
	};

	class CameraUbo
	{
	private:
		using Configuration = CameraConfig;

	public:
		explicit CameraUbo( castor3d::RenderDevice const & device
			, bool & dirty );
		~CameraUbo();
		void cpuUpdate( castor3d::Camera const & camera
			, bool isSafeBanded
			, castor::Point3f const & sunDirection
			, castor::Vector3f const & planetPosition );

		void createPassBinding( crg::FramePass & pass
			, uint32_t binding )const
		{
			m_ubo.createPassBinding( pass, "CameraCfg", binding );
		}

		void createSizedBinding( ashes::DescriptorSet & descriptorSet
			, VkDescriptorSetLayoutBinding const & layoutBinding )const
		{
			return m_ubo.createSizedBinding( descriptorSet, layoutBinding );
		}

		ashes::WriteDescriptorSet getDescriptorWrite( uint32_t dstBinding
			, uint32_t dstArrayElement = 0u )const
		{
			return m_ubo.getDescriptorWrite( dstBinding, dstArrayElement );
		}

		castor3d::UniformBufferOffsetT< Configuration > const & getUbo()const
		{
			return m_ubo;
		}

		castor3d::UniformBufferOffsetT< Configuration > & getUbo()
		{
			return m_ubo;
		}

	public:
		static const castor::MbString Buffer;
		static const castor::MbString Data;

	private:
		castor3d::RenderDevice const & m_device;
		castor3d::UniformBufferOffsetT< Configuration > m_ubo;
		castor::GroupChangeTracked< castor::Point3f > m_position;
		castor::GroupChangeTracked< castor::Quaternion > m_orientation;
	};
}

#define ATM_Camera( writer, binding, set )\
	auto cameraBuffer = writer.declUniformBuffer<>( atmosphere_scattering::CameraUbo::Buffer, binding, set );\
	auto atm_cameraData = cameraBuffer.declMember< atmosphere_scattering::CameraData >( atmosphere_scattering::CameraUbo::Data );\
	cameraBuffer.end()

#endif
