/*
See LICENSE file in root folder
*/
#ifndef ___C3DSMAA_SmaaUbo_H___
#define ___C3DSMAA_SmaaUbo_H___

#include "SmaaConfig.hpp"

#include <Castor3D/Buffer/UniformBufferOffset.hpp>

#include <CastorUtils/Math/Point.hpp>

#include <ShaderWriter/BaseTypes/Float.hpp>
#include <ShaderWriter/BaseTypes/Int.hpp>
#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>
#include <ShaderWriter/VecTypes/Vec4.hpp>

namespace smaa
{
	enum class Bindings : uint32_t
	{
		SmaaUboIdx,
	};

	struct SmaaUboConfiguration
	{
		c3d::Point4f rtMetrics;
		c3d::Point4f predication;
		c3d::Point4f subsampleIndices;
		c3d::Point4f searchSizes;
		c3d::Point4f areaTexPixelSizeAndLocalContrast;
		c3d::Point4f areaTexSizesReprojWS;
		c3d::Point4i maxsSearchSteps;
		c3d::Point4i tweaks;
	};

	struct SmaaData
		: public sdw::StructInstanceHelperT< "C3D_SmaaData"
			, sdw::type::MemoryLayout::eStd140
			, sdw::Vec4Field< "rtMetrics" >
			, sdw::FloatField< "threshold" >
			, sdw::FloatField< "predicationThreshold" >
			, sdw::FloatField< "predicationScale" >
			, sdw::FloatField< "predicationStrength" >
			, sdw::Vec4Field< "subsampleIndices" >
			, sdw::Vec2Field< "searchTexSize" >
			, sdw::Vec2Field< "searchTexPackedSize" >
			, sdw::Vec2Field< "areaTexPixelSize" >
			, sdw::FloatField< "localContrastAdaptationFactor" >
			, sdw::IntField< "cornerRounding" >
			, sdw::FloatField< "areaTexMaxDistance" >
			, sdw::FloatField< "areaTexMaxDistanceDiag" >
			, sdw::FloatField< "areaTexSubtexSize" >
			, sdw::FloatField< "reprojectionWeightScale" >
			, sdw::IntField< "maxSearchSteps" >
			, sdw::IntField< "maxSearchStepsDiag" >
			, sdw::IntField< "pad0" >
			, sdw::IntField< "pad1" >
			, sdw::IntField< "disableCornerDetection" >
			, sdw::IntField< "disableDiagonalDetection" >
			, sdw::IntField< "enableReprojection" >
			, sdw::IntField< "pad2" > >
	{
		SDW_DeclStructInstance( , SmaaData );

		SmaaData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled );

		sdw::Vec4 rtMetrics;
		sdw::Float threshold;
		sdw::Float predicationThreshold;
		sdw::Float predicationScale;
		sdw::Float predicationStrength;
		sdw::Vec4 subsampleIndices;
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
		explicit SmaaUbo( c3d::RenderDevice const & device );
		~SmaaUbo();

		void cpuUpdate( c3d::Size const & renderSize
			, SmaaConfig const & config );

		template< typename BindingT >
		void createPassBinding( crg::FramePass & pass
			, BindingT binding )const
		{
			return m_ubo.createPassBinding( pass, binding );
		}

		void createSizedBinding( ashes::DescriptorSet & descriptorSet
			, VkDescriptorSetLayoutBinding const & layoutBinding )const
		{
			return m_ubo.createSizedBinding( descriptorSet, layoutBinding );
		}

		c3d::UniformBufferOffsetT< Configuration > const & getUbo()const
		{
			return m_ubo;
		}

	public:
		static const c3d::MbString Buffer;
		static const c3d::MbString Data;

	private:
		c3d::RenderDevice const & m_device;
		c3d::UniformBufferOffsetT< Configuration > m_ubo;
	};
}

#define C3D_Smaa( writer, binding, set )\
	sdw::UniformBuffer smaaBuffer{ writer\
		, smaa::SmaaUbo::Buffer\
		, uint32_t( binding )\
		, set\
		, ast::type::MemoryLayout::eStd140\
		, true };\
	auto c3d_smaaData = smaaBuffer.declMember< smaa::SmaaData >( smaa::SmaaUbo::Data );\
	smaaBuffer.end()

#endif
