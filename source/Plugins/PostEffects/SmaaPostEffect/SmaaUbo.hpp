/*
See LICENSE file in root folder
*/
#ifndef ___C3DSMAA_SmaaUbo_H___
#define ___C3DSMAA_SmaaUbo_H___

#include "SmaaConfig.hpp"

#include <Castor3D/Buffer/UniformBufferOffset.hpp>
#include <Castor3D/Render/PostEffect/PostEffectSurface.hpp>

#include <CastorUtils/Math/Point.hpp>

#include <ShaderWriter/BaseTypes/Float.hpp>
#include <ShaderWriter/BaseTypes/Int.hpp>
#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
#include <ShaderWriter/VecTypes/Vec4.hpp>

namespace smaa
{
	enum Idx : uint32_t
	{
		SmaaUboIdx,
	};

	struct SmaaUboConfiguration
	{
		castor::Point4f rtMetrics;
		castor::Point4f predication;
		castor::Point4f subsampleIndices;
		castor::Point4f searchSizes;
		castor::Point4f areaTexPixelSizeAndLocalContrast;
		castor::Point4f areaTexSizesReprojWS;
		castor::Point4i maxsSearchSteps;
		castor::Point4i tweaks;
	};

	struct SmaaData
		: public sdw::StructInstance
	{
	public:
		SmaaData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled );
		SDW_DeclStructInstance( , SmaaData );

		static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache );

	public:
		// Struct members
		sdw::Vec4 rtMetrics;
		sdw::Vec4 predication;
		sdw::Vec4 subsampleIndices;
		sdw::Vec4 searchSizes;
		sdw::Vec4 areaTexPixelSizeAndLocalContrast;
		sdw::Vec4 areaTexSizesReprojWS;
		sdw::IVec4 maxsSearchSteps;
		sdw::IVec4 tweaks;

		// Computed from members
		sdw::Float threshold;
		sdw::Float predicationThreshold;
		sdw::Float predicationScale;
		sdw::Float predicationStrength;
		sdw::Vec2 searchTexSize;
		sdw::Vec2 searchTexPackedSize;
		sdw::Vec2 areaTexPixelSize;
		sdw::Float localContrastAdaptationFactor;
		sdw::Int cornerRounding;
		sdw::Float areaTexMaxDistance;
		sdw::Float areaTexMaxDistanceDiag;
		sdw::Float areaTexSubtexSize;
		sdw::Float reprojectionWeightScale;
		sdw::Int maxSearchSteps;
		sdw::Int maxSearchStepsDiag;
		sdw::Int disableCornerDetection;
		sdw::Int disableDiagonalDetection;
		sdw::Int enableReprojection;
		sdw::Float cornerRoundingNorm;
		sdw::Float depthThreshold;

	private:
		using sdw::StructInstance::getMember;
		using sdw::StructInstance::getMemberArray;
	};

	class SmaaUbo
	{
	public:
		using Configuration = SmaaUboConfiguration;

	public:
		SmaaUbo( SmaaUbo const & rhs ) = delete;
		SmaaUbo & operator=( SmaaUbo const & rhs ) = delete;
		SmaaUbo( SmaaUbo && rhs ) = default;
		SmaaUbo & operator=( SmaaUbo && rhs ) = delete;
		explicit SmaaUbo( castor3d::RenderDevice const & device );
		~SmaaUbo();

		void cpuUpdate( castor::Size const & renderSize
			, SmaaConfig const & config );

		void createPassBinding( crg::FramePass & pass
			, uint32_t binding )const
		{
			return m_ubo.createPassBinding( pass, "SMAA", binding );
		}

		void createSizedBinding( ashes::DescriptorSet & descriptorSet
			, VkDescriptorSetLayoutBinding const & layoutBinding )const
		{
			return m_ubo.createSizedBinding( descriptorSet, layoutBinding );
		}

		castor3d::UniformBufferOffsetT< Configuration > const & getUbo()const
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

#define C3D_Smaa( writer, binding, set )\
	sdw::Ubo smaaBuffer{ writer\
		, smaa::SmaaUbo::Buffer\
		, binding\
		, set\
		, ast::type::MemoryLayout::eStd140\
		, true };\
	auto c3d_smaaData = smaaBuffer.declMember< smaa::SmaaData >( smaa::SmaaUbo::Data );\
	smaaBuffer.end()

#endif
