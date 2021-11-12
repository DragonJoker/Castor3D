#include "SmaaPostEffect/SmaaUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/UniformBufferPools.hpp>
#include <Castor3D/Render/RenderDevice.hpp>

#include <ShaderWriter/Writer.hpp>

namespace smaa
{
	//*********************************************************************************************

	SmaaData::SmaaData( sdw::ShaderWriter & writer
		, ast::expr::ExprPtr expr
		, bool enabled )
		: sdw::StructInstance{ writer, std::move( expr ), enabled }
		, rtMetrics{ getMember< sdw::Vec4 >( "rtMetrics" ) }
		, predication{ getMember< sdw::Vec4 >( "predication" ) }
		, subsampleIndices{ getMember< sdw::Vec4 >( "subsampleIndices" ) }
		, searchSizes{ getMember< sdw::Vec4 >( "searchSizes" ) }
		, areaTexPixelSizeAndLocalContrast{ getMember< sdw::Vec4 >( "areaTexPixelSizeAndLocalContrast" ) }
		, areaTexSizesReprojWS{ getMember< sdw::Vec4 >( "areaTexSizesReprojWS" ) }
		, maxsSearchSteps{ getMember< sdw::IVec4 >( "maxsSearchSteps" ) }
		, tweaks{ getMember< sdw::IVec4 >( "tweaks" ) }
		, threshold{ predication.x() }
		, predicationThreshold{ predication.y() }
		, predicationScale{ predication.z() }
		, predicationStrength{ predication.w() }
		, searchTexSize{ searchSizes.xy() }
		, searchTexPackedSize{ searchSizes.zw() }
		, areaTexPixelSize{ areaTexPixelSizeAndLocalContrast.xy() }
		, localContrastAdaptationFactor{ areaTexPixelSizeAndLocalContrast.z() }
		, cornerRounding{ writer.cast< sdw::Int >( areaTexPixelSizeAndLocalContrast.w() ) }
		, areaTexMaxDistance{ areaTexSizesReprojWS.x() }
		, areaTexMaxDistanceDiag{ areaTexSizesReprojWS.y() }
		, areaTexSubtexSize{ areaTexSizesReprojWS.z() }
		, reprojectionWeightScale{ areaTexSizesReprojWS.w() }
		, maxSearchSteps{ maxsSearchSteps.x() }
		, maxSearchStepsDiag{ maxsSearchSteps.y() }
		, disableCornerDetection{ tweaks.x() }
		, disableDiagonalDetection{ tweaks.y() }
		, enableReprojection{ tweaks.z() }
		, cornerRoundingNorm{ writer.cast< sdw::Float >( cornerRounding ) / 100.0_f }
		, depthThreshold{ threshold * 0.1_f }
	{
	}

	ast::type::BaseStructPtr SmaaData::makeType( ast::type::TypesCache & cache )
	{
		auto result = cache.getStruct( ast::type::MemoryLayout::eStd140
			, "C3D_SmaaData" );

		if ( result->empty() )
		{
			result->declMember( "rtMetrics", ast::type::Kind::eVec4F );
			result->declMember( "predication", ast::type::Kind::eVec4F );
			result->declMember( "subsampleIndices", ast::type::Kind::eVec4F );
			result->declMember( "searchSizes", ast::type::Kind::eVec4F );
			result->declMember( "areaTexPixelSizeAndLocalContrast", ast::type::Kind::eVec4F );
			result->declMember( "areaTexSizesReprojWS", ast::type::Kind::eVec4F );
			result->declMember( "maxsSearchSteps", ast::type::Kind::eVec4I );
			result->declMember( "tweaks", ast::type::Kind::eVec4I );
		}

		return result;
	}

	//*********************************************************************************************

	castor::String const SmaaUbo::Buffer = cuT( "SmaaBuffer" );
	castor::String const SmaaUbo::Data = cuT( "c3d_smaaData" );

	SmaaUbo::SmaaUbo( castor3d::RenderDevice const & device )
		: m_device{ device }
		, m_ubo{ m_device.uboPools->getBuffer< SmaaUboConfiguration >( VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT ) }
	{
	}

	SmaaUbo::~SmaaUbo()
	{
		m_device.uboPools->putBuffer< SmaaUboConfiguration >( m_ubo );
	}

	void SmaaUbo::cpuUpdate( castor::Size const & renderSize
		, SmaaConfig const & config )
	{
		auto & data = m_ubo.getData();
		data.rtMetrics = { 1.0f / float( renderSize.getWidth() )
			, 1.0f / float( renderSize.getHeight() )
			, float( renderSize.getWidth() )
			, float( renderSize.getHeight() ) };
		data.predication = { config.data.threshold
			, config.data.predicationThreshold
			, config.data.predicationScale
			, config.data.predicationStrength };
		data.subsampleIndices = config.subsampleIndices[config.subsampleIndex];
		data.searchSizes = { config.data.searchTexSize->x
			, config.data.searchTexSize->y
			, config.data.searchTexPackedSize->x
			, config.data.searchTexPackedSize->y };
		data.areaTexPixelSizeAndLocalContrast = { config.data.areaTexPixelSize->x
			, config.data.areaTexPixelSize->y
			, config.data.localContrastAdaptationFactor
			, config.data.cornerRounding };
		data.areaTexSizesReprojWS = { config.data.areaTexMaxDistance
			, config.data.areaTexMaxDistanceDiag
			, config.data.areaTexSubtexSize
			, config.data.reprojectionWeightScale };
		data.maxsSearchSteps = { config.data.maxSearchSteps
			, config.data.maxSearchStepsDiag
			, 0
			, 0 };
		data.tweaks = { config.data.disableCornerDetection ? 1 : 0
			, config.data.disableDiagonalDetection ? 1 : 0
			, config.data.enableReprojection ? 1 : 0
			, 0 };
	}

	//*********************************************************************************************
}
