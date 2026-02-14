#include "Uncharted2ToneMapping/Uncharted2ToneMapping.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Render/RenderDevice.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Scene/SceneFileParserData.hpp>
#include <Castor3D/Shader/Shaders/GlslBaseIO.hpp>
#include <Castor3D/Shader/Ubos/RenderUbo.hpp>
#include <Castor3D/Shader/Ubos/ColourGradingUbo.hpp>

#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/FileParser/FileParserContext.hpp>
#include <CastorUtils/Graphics/Size.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

#include <RenderGraph/RecordContext.hpp>

#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>

namespace Uncharted2
{
	namespace c3ds = c3d::shader;

	//*********************************************************************************************

	namespace parse
	{
		enum class Section : c3d::SectionId
		{
			eUncharted2 = c3d::makeSectionName( 'T', 'M', 'U', 'N', 'C', 'H', 'T', '2' ),
		};

		struct Context
		{
			c3d::TargetContext * target;
			Uncharted2UboConfiguration config;
		};

		static CU_ImplementAttributeParserNewBlock( parserRoot, c3d::TargetContext, Context )
		{
			if ( !blockContext->renderTarget )
				CU_ParsingError( cuT( "No target initialised. (Did you forget to set its size and format ?)" ) );
			else
				newBlockContext->target = blockContext;
		}
		CU_EndAttributePushNewBlock( Section::eUncharted2 )

		static CU_ImplementAttributeParserBlock( parserShoulderStrength, Context )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter." ) );
			else
				params[0]->get( blockContext->config.shoulderStrength );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserLinearStrength, Context )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter." ) );
			else
				params[0]->get( blockContext->config.linearStrength );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserLinearAngle, Context )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter." ) );
			else
				params[0]->get( blockContext->config.linearAngle );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserToeStrength, Context )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter." ) );
			else
				params[0]->get( blockContext->config.toeStrength );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserToeNumerator, Context )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter." ) );
			else
				params[0]->get( blockContext->config.toeNumerator );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserToeDenominator, Context )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter." ) );
			else
				params[0]->get( blockContext->config.toeDenominator );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserLinearWhitePointValue, Context )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter." ) );
			else
				params[0]->get( blockContext->config.linearWhitePointValue );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserExposureBias, Context )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter." ) );
			else
				params[0]->get( blockContext->config.exposureBias );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserEnd, Context )
		{
			if ( !blockContext->target )
				CU_ParsingError( cuT( "No target initialised. (Did you forget to set its size and format ?)" ) );
			else
			{
				blockContext->target->toneMappingParams.add( "shoulderStrength", blockContext->config.shoulderStrength );
				blockContext->target->toneMappingParams.add( "linearStrength", blockContext->config.linearStrength );
				blockContext->target->toneMappingParams.add( "linearAngle", blockContext->config.linearAngle );
				blockContext->target->toneMappingParams.add( "toeStrength", blockContext->config.toeStrength );
				blockContext->target->toneMappingParams.add( "toeNumerator", blockContext->config.toeNumerator );
				blockContext->target->toneMappingParams.add( "toeDenominator", blockContext->config.toeDenominator );
				blockContext->target->toneMappingParams.add( "linearWhitePointValue", blockContext->config.linearWhitePointValue );
				blockContext->target->toneMappingParams.add( "exposureBias", blockContext->config.exposureBias );
				blockContext->target->renderTarget->setToneMappingType( ToneMapping::Type );
			}
		}
		CU_EndAttributePop()
	}

	//*********************************************************************************************

	static ashes::DescriptorSetLayoutPtr createLayout( c3d::RenderDevice const & device )
	{
		ashes::VkDescriptorSetLayoutBindingArray bindings;
		uint32_t index{};
		c3d::addDescriptorSetLayoutBinding( bindings, index, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, 1u, nullptr );
		return device->createDescriptorSetLayout( bindings );
	}

	//*********************************************************************************************

	c3d::String ToneMapping::Type = cuT( "uncharted2" );
	c3d::MbString ToneMapping::Name = "Uncharted 2 Tone Mapping";

	ToneMapping::~ToneMapping()noexcept = default;

	ToneMapping::ToneMapping( c3d::ToneMapping const & parent
		, c3d::RenderDevice const & device
		, c3d::Parameters parameters )
		: c3d::ToneMappingImpl{ parent }
		, m_ubo{ device }
		, m_layout{ createLayout( device) }
		, m_pool{ c3d::makeRawUnique< ashes::DescriptorSetPool >( *device, *m_layout, 1u, true ) }
		, m_set{ m_pool->createDescriptorSet() }
	{
		m_ubo.createSizedBinding( *m_set, m_layout->getBinding( 0u ) );
		m_set->update();

		parameters.get( "shoulderStrength", m_configuration.shoulderStrength );
		parameters.get( "linearStrength", m_configuration.linearStrength );
		parameters.get( "linearAngle", m_configuration.linearAngle );
		parameters.get( "toeStrength", m_configuration.toeStrength );
		parameters.get( "toeNumerator", m_configuration.toeNumerator );
		parameters.get( "toeDenominator", m_configuration.toeDenominator );
		parameters.get( "linearWhitePointValue", m_configuration.linearWhitePointValue );
		parameters.get( "exposureBias", m_configuration.exposureBias );
	}

	void ToneMapping::getFragmentProgram( ast::ShaderBuilder & builder )
	{
		sdw::TraditionalGraphicsWriter writer{ builder };

		C3D_Render( writer, 0u, 0u );
		C3D_ColourGrading( writer, 1u, 0u );
		C3D_Uncharted2( writer, 0u, 1u );
		auto c3d_mapHdr = writer.declCombinedImg< FImg2DRgba16 >( "c3d_mapHdr", 2u, 0u );

		writer.implementEntryPointT< c3ds::Uv2FT, c3ds::Colour4FT >( [&writer, &c3d_colourGrading, &c3d_renderData, &c3d_mapHdr, &c3d_uncharted2Data]( sdw::FragmentInT< c3ds::Uv2FT > const & in
			, sdw::FragmentOutT< c3ds::Colour4FT > const & out )
			{
				auto hdrColor = writer.declLocale( "hdrColor"
					, c3d_colourGrading.colourGrade( c3d_mapHdr.sample( in.uv() ).rgb() ) );
				hdrColor *= vec3( c3d_uncharted2Data.exposureBias ); // Hardcoded Exposure Adjustment.

				auto current = writer.declLocale( "current"
					, c3d_uncharted2Data.toneMap( hdrColor * c3d_renderData.exposure() ) );

				auto whiteScale = writer.declLocale( "whiteScale"
					, vec3( 1.0_f ) / c3d_uncharted2Data.toneMap( vec3( c3d_uncharted2Data.linearWhitePointValue ) ) );
				auto colour = writer.declLocale( "colour"
					, current * whiteScale );

				out.colour() = vec4( c3d_renderData.applyGamma( colour ), 1.0_f );
			} );
	}

	void ToneMapping::accept( c3d::ConfigurationVisitor & visitor )
	{
		m_configuration.accept( visitor );
	}

	void ToneMapping::update()
	{
		m_ubo.update( m_configuration );
	}

	c3d::Vector< VkDescriptorSetLayout > ToneMapping::getDescriptorLayouts()const
	{
		c3d::Vector< VkDescriptorSetLayout > result;
		result.emplace_back( *m_layout );
		return result;
	}

	void ToneMapping::recordInto( crg::RecordContext const & context, VkCommandBuffer cb, uint32_t )const
	{
		VkDescriptorSet ds = *m_set;
		context->vkCmdBindDescriptorSets( cb, VK_PIPELINE_BIND_POINT_GRAPHICS, getPipelineLayout(), 1u, 1u, &ds, 0u, nullptr );
	}

	c3d::ToneMappingImplUPtr ToneMapping::create( c3d::ToneMapping const & parent, c3d::RenderDevice const & device, c3d::Parameters parameters )
	{
		return c3d::ToneMappingImplUPtr( c3d::makeRawUnique< ToneMapping >( parent, device, c3d::move( parameters ) ).release() );
	}

	c3d::AttributeParsers ToneMapping::createParsers()
	{
		c3d::AttributeParsers result;
		c3d::BlockParserContextT< c3d::TargetContext > targetCtx{ result, c3d::CSCNSection::eRenderTarget };
		c3d::BlockParserContextT< parse::Context > toneCtx{ result, parse::Section::eUncharted2 };

		targetCtx.addPushParser( cuT( "uncharted2" ), parse::Section::eUncharted2, parse::parserRoot );

		toneCtx.addParser( cuT( "shoulderStrength" ), parse::parserShoulderStrength, { c3d::makeParameter< c3d::ParameterType::eFloat >() } );
		toneCtx.addParser( cuT( "linearStrength" ), parse::parserLinearStrength, { c3d::makeParameter< c3d::ParameterType::eFloat >() } );
		toneCtx.addParser( cuT( "linearAngle" ), parse::parserLinearAngle, { c3d::makeParameter< c3d::ParameterType::eFloat >() } );
		toneCtx.addParser( cuT( "toeStrength" ), parse::parserToeStrength, { c3d::makeParameter< c3d::ParameterType::eFloat >() } );
		toneCtx.addParser( cuT( "toeNumerator" ), parse::parserToeNumerator, { c3d::makeParameter< c3d::ParameterType::eFloat >() } );
		toneCtx.addParser( cuT( "toeDenominator" ), parse::parserToeDenominator, { c3d::makeParameter< c3d::ParameterType::eFloat >() } );
		toneCtx.addParser( cuT( "linearWhitePointValue" ), parse::parserLinearWhitePointValue, { c3d::makeParameter< c3d::ParameterType::eFloat >() } );
		toneCtx.addParser( cuT( "exposureBias" ), parse::parserExposureBias, { c3d::makeParameter< c3d::ParameterType::eFloat >() } );
		toneCtx.addPopParser( cuT( "}" ), parse::parserEnd );

		return result;
	}

	c3d::StrSectionIdMap ToneMapping::createSections()
	{
		return
		{
			{ c3d::SectionId( parse::Section::eUncharted2 ), ToneMapping::Type },
		};
	}
}
