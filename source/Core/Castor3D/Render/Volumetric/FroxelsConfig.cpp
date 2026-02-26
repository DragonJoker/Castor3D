#include "Castor3D/Render/Volumetric/FroxelsConfig.hpp"

#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>

namespace c3d
{
	namespace fxlcfg
	{
		struct FroxelsContext
		{
			FroxelsConfig froxelsConfig{};
			RenderTargetRPtr renderTarget{};
		};

		static CU_ImplementAttributeParserNewBlock( parserRenderTargetFroxels, TargetContext, FroxelsContext )
		{
			if ( !blockContext->renderTarget )
				CU_ParsingError( cuT( "Render target not initialised." ) );
			else
				newBlockContext->renderTarget = blockContext->renderTarget;
		}
		CU_EndAttributePushNewBlock( CSCNSection::eFroxels )

		static CU_ImplementAttributeParserBlock( parserFroxelsSampleCountX, FroxelsContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter." ) );
			else
				params[0]->get( blockContext->froxelsConfig.sampleCountX );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserFroxelsSampleCountY, FroxelsContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter." ) );
			else
				params[0]->get( blockContext->froxelsConfig.sampleCountY );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserFroxelsSampleCountMinZ, FroxelsContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter." ) );
			else
				params[0]->get( blockContext->froxelsConfig.sampleCountMinZ );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserFroxelsSampleCountMaxZ, FroxelsContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter." ) );
			else
				params[0]->get( blockContext->froxelsConfig.sampleCountMaxZ );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserFroxelsBlurFilterSize, FroxelsContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter." ) );
			else
			{
				blockContext->froxelsConfig.blurFilterSize = RangedValue< u32 >{ params[0]->get< u32 >()
					, blockContext->froxelsConfig.blurFilterSize.value().range() };
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserFroxelsEnd, FroxelsContext )
		{
			blockContext->renderTarget->setFroxelsConfig( blockContext->froxelsConfig );
		}
		CU_EndAttributePop()
	}

	FroxelsConfig::FroxelsConfig()
		: debugDisplay{ dirty, FroxelDebugDisplay::eNone }
		, sampleCountX{ dirty, 1u }
		, sampleCountY{ dirty, 1u }
		, sampleCountMinZ{ dirty, 4u }
		, sampleCountMaxZ{ dirty, 4u }
		, blurFilterSize{ dirty, { 8u, makeRange( 4u, 64u ) } }
	{
	}

	void FroxelsConfig::accept( ConfigurationVisitorBase & visitor )
	{
		static StringArray debugDisplayNames{ cuT( "None" )
			, cuT( "Froxels AABB" ) };

		visitor.visit( cuT( "Froxels" ) );
		visitor.visit( cuT( "Samples X" ), sampleCountX );
		visitor.visit( cuT( "Samples Y" ), sampleCountY );
		visitor.visit( cuT( "Min Samples Z" ), sampleCountMinZ );
		visitor.visit( cuT( "Max Samples Z" ), sampleCountMaxZ );
		visitor.visit( cuT( "Blur Filter Size" ), blurFilterSize );
		visitor.visit( cuT( "[Debug] Display AABBs" )
			, debugDisplay
			, debugDisplayNames
			, ConfigurationVisitorBase::OnEnumValueChangeT< FroxelDebugDisplay >( [this]( FroxelDebugDisplay, FroxelDebugDisplay newV )
				{
					debugDisplay = newV;
				} ) );
	}

	void FroxelsConfig::addParsers( AttributeParsers & result )
	{
		BlockParserContextT< TargetContext > targetContext{ result, CSCNSection::eRenderTarget };
		BlockParserContextT< fxlcfg::FroxelsContext > clustersContext{ result, CSCNSection::eFroxels, CSCNSection::eRenderTarget };

		targetContext.addPushParser( cuT( "froxels" ), CSCNSection::eFroxels, fxlcfg::parserRenderTargetFroxels );
		clustersContext.addParser( cuT( "samples_x" ), fxlcfg::parserFroxelsSampleCountX, { makeDefaultedParameter< ParameterType::eUInt32 >( 1u ) } );
		clustersContext.addParser( cuT( "samples_y" ), fxlcfg::parserFroxelsSampleCountY, { makeDefaultedParameter< ParameterType::eUInt32 >( 1u ) } );
		clustersContext.addParser( cuT( "samples_min_z" ), fxlcfg::parserFroxelsSampleCountMinZ, { makeDefaultedParameter< ParameterType::eUInt32 >( 4u ) } );
		clustersContext.addParser( cuT( "samples_max_z" ), fxlcfg::parserFroxelsSampleCountMaxZ, { makeDefaultedParameter< ParameterType::eUInt32 >( 4u ) } );
		clustersContext.addParser( cuT( "blur_filter_size" ), fxlcfg::parserFroxelsBlurFilterSize, { makeDefaultedParameter< ParameterType::eUInt32 >( 8u ) } );
		clustersContext.addPopParser( cuT( "}" ), fxlcfg::parserFroxelsEnd );
	}
}
