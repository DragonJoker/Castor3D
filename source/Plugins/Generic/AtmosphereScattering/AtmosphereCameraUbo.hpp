/*
See LICENSE file in root folder
*/
#ifndef ___C3DAS_AtmosphereCameraUbo_H___
#define ___C3DAS_AtmosphereCameraUbo_H___

#include "AtmosphereScatteringPrerequisites.hpp"

#include <Castor3D/Castor3DModule.hpp>
#include <Castor3D/Render/RenderModule.hpp>

#include <Castor3D/Buffer/UniformBufferOffset.hpp>

#include <ShaderWriter/BaseTypes/Float.hpp>
#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
#include <ShaderWriter/MatTypes/Mat4.hpp>
#include <ShaderWriter/VecTypes/Vec4.hpp>

namespace atmosphere_scattering
{
	struct CameraConfig
	{
		castor::Matrix4x4f camInvViewProj;
		castor::Matrix4x4f objInvViewProj;
		castor::Point3f position;
	};

	struct CameraData
		: public sdw::StructInstance
	{
		CameraData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled );

		SDW_DeclStructInstance( , CameraData );

		static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache );

		sdw::Vec4 camProjToWorld( sdw::Vec4 const & pos )const;
		sdw::Vec4 objProjToWorld( sdw::Vec4 const & pos )const;

	private:
		using sdw::StructInstance::getMember;
		using sdw::StructInstance::getMemberArray;

	private:
		sdw::Mat4 camInvViewProj;
		sdw::Mat4 objInvViewProj;
	public:
		sdw::Vec3 position;
	};

	class CameraUbo
	{
	private:
		using Configuration = CameraConfig;

	public:
		explicit CameraUbo( castor3d::RenderDevice const & device );
		~CameraUbo();
		void cpuUpdate( castor3d::Camera const & camera
			, bool isSafeBanded );

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
		static const castor::String Buffer;
		static const castor::String Data;

	private:
		castor3d::RenderDevice const & m_device;
		castor3d::UniformBufferOffsetT< Configuration > m_ubo;
	};
}

#define C3D_Camera( writer, binding, set )\
	auto cameraBuffer = writer.declUniformBuffer<>( atmosphere_scattering::CameraUbo::Buffer, binding, set );\
	auto c3d_cameraData = cameraBuffer.declMember< atmosphere_scattering::CameraData >( atmosphere_scattering::CameraUbo::Data );\
	cameraBuffer.end()

#endif