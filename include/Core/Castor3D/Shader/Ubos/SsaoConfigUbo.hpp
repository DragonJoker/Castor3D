/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SsaoConfigUbo_H___
#define ___C3D_SsaoConfigUbo_H___

#include "UbosModule.hpp"
#include "Castor3D/Render/Ssao/SsaoModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"

#include <CastorUtils/Math/SquareMatrix.hpp>

#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
#include <ShaderWriter/MatTypes/Mat4.hpp>

namespace castor3d
{
	namespace shader
	{
		struct SsaoConfigData
			: public sdw::StructInstance
		{
			C3D_API SsaoConfigData( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled );
			SDW_DeclStructInstance( C3D_API, SsaoConfigData );

			C3D_API static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache );
			C3D_API static std::unique_ptr< sdw::Struct > declare( sdw::ShaderWriter & writer );

			sdw::Vec4 projInfo;
			sdw::Int numSamples;
			sdw::Int numSpiralTurns;
			sdw::Float projScale;
			sdw::Float radius;
			sdw::Float invRadius;
			sdw::Float radius2;
			sdw::Float invRadius2;
			sdw::Float bias;
			sdw::Float intensity;
			sdw::Float intensityDivR6;
			sdw::Float farPlaneZ;
			sdw::Float edgeSharpness;
			sdw::Int blurStepSize;
			sdw::Int blurRadius;
			sdw::Int highQuality;
			sdw::Int blurHighQuality;
			sdw::Int logMaxOffset;
			sdw::Int maxMipLevel;
			sdw::Float minRadius;
			sdw::Int variation;
			sdw::Int bendStepCount;
			sdw::Float bendStepSize;

		private:
			using sdw::StructInstance::getMember;
			using sdw::StructInstance::getMemberArray;
		};
	}

	class SsaoConfigUbo
	{
	private:
		struct Configuration
		{
			//   vec4(-2.0f / (width*P[0][0]),
			//		  -2.0f / (height*P[1][1]),
			//		  ( 1.0f - P[0][2]) / P[0][0],
			//		  ( 1.0f + P[1][2]) / P[1][1])
			//
			//	where P is the projection matrix that maps camera space points
			//	to [-1, 1] x [-1, 1].  That is, SsaoConfigUbo::getProjectUnitMatrix().
			castor::Point4f projInfo;
			// Integer number of samples to take at each pixel.
			int32_t numSamples;
			// This is the number of turns around the circle that the spiral pattern makes.
			// This should be prime to prevent taps from lining up.
			int32_t numSpiralTurns;
			// The height in pixels of a 1m object if viewed from 1m away.
			// You can compute it from your projection matrix.  The actual value is just
			// a scale factor on radius; you can simply hardcode this to a constant (~500)
			// and make your radius value unitless (...but resolution dependent.)
			float projScale;
			// World-space AO radius in scene units (r).  e.g., 1.0m.
			float radius;
			// 1 / radius.
			float invRadius;
			// Squared radius.
			float radius2;
			// 1 / (squared radius).
			float invRadius2;
			// Bias to avoid AO in smooth corners, e.g., 0.01m.
			float bias;
			// intensity.
			float intensity;
			// intensity / radius ^ 6.
			float intensityDivR6;
			// Used for preventing AO computation on the sky (at infinite depth) and defining the CS Z to bilateral depth key scaling.
			// This need not match the real far plane.
			float farPlaneZ;
			// Increase to make depth edges crisper. Decrease to reduce flicker.
			float edgeSharpness;
			// Step in 2-pixel intervals since we already blurred against neighbors in the
			// first AO pass.  This constant can be increased while R decreases to improve
			// performance at the expense of some dithering artifacts.
			// 
			// Morgan found that a scale of 3 left a 1-pixel checkerboard grid that was
			// unobjectionable after shading was applied but eliminated most temporal incoherence
			// from using small numbers of sample taps.
			int32_t blurStepSize;
			// Filter radius in pixels. This will be multiplied by blurStepSize.
			int32_t blurRadius;
			int32_t highQuality;
			int32_t blurHighQuality;
			// If using depth mip levels, the log of the maximum pixel offset before we need to switch to a lower
			// miplevel to maintain reasonable spatial locality in the cache
			// If this number is too small (< 3), too many taps will land in the same pixel, and we'll get bad variance that manifests as flashing.
			// If it is too high (> 5), we'll get bad performance because we're not using the MIP levels effectively
			int32_t logMaxOffset;
			// This must be less than or equal to LineariseDepthPass::MaxMipLevel.
			int32_t maxMipLevel;
			// pixels
			float minRadius;
			int32_t variation;
			// The bending normals ray steps count.
			int32_t bendStepCount;
			// The bending normals ray step size.
			float bendStepSize;
		};

	public:
		C3D_API SsaoConfigUbo( SsaoConfigUbo const & rhs ) = delete;
		C3D_API SsaoConfigUbo & operator=( SsaoConfigUbo const & rhs ) = delete;
		C3D_API SsaoConfigUbo( SsaoConfigUbo && rhs ) = default;
		C3D_API SsaoConfigUbo & operator=( SsaoConfigUbo && rhs ) = delete;
		C3D_API explicit SsaoConfigUbo( RenderDevice const & device );
		C3D_API ~SsaoConfigUbo();
		/**
		 *\~english
		 *\brief		Updates the UBO content.
		 *\param[in]	config	The SSAO configuration.
		 *\param[in]	camera	The viewing camera.
		 *\~french
		 *\brief		Met à jour le contenu de l'UBO.
		 *\param[in]	config	La configuratio du SSAO.
		 *\param[in]	camera	La caméra de rendu.
		 */
		C3D_API void cpuUpdate( SsaoConfig const & config
			, Camera const & camera );

		void createPassBinding( crg::FramePass & pass
			, uint32_t binding )const
		{
			return m_ubo.createPassBinding( pass, "SSAOCfg", binding );
		}

		void createSizedBinding( ashes::DescriptorSet & descriptorSet
			, VkDescriptorSetLayoutBinding const & layoutBinding )const
		{
			return m_ubo.createSizedBinding( descriptorSet, layoutBinding );
		}

		UniformBufferOffsetT< Configuration > const & getUbo()const
		{
			return m_ubo;
		}

	public:
		C3D_API static castor::String const BufferSsaoConfig;
		C3D_API static castor::String const SsaoConfigData;

	private:
		RenderDevice const & m_device;
		UniformBufferOffsetT< Configuration > m_ubo;
	};
}

#define C3D_SsaoConfig( Writer, Binding, Set )\
	sdw::Ubo ssaoConfig{ Writer\
		, castor3d::SsaoConfigUbo::BufferSsaoConfig\
		, uint32_t( Binding )\
		, uint32_t( Set )\
		, ast::type::MemoryLayout::eStd140\
		, true };\
	auto c3d_ssaoConfigData = ssaoConfig.declMember< castor3d::shader::SsaoConfigData >( castor3d::SsaoConfigUbo::SsaoConfigData );\
	ssaoConfig.end()

#endif
