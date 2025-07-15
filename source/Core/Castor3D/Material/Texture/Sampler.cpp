#include "Castor3D/Material/Texture/Sampler.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Miscellaneous/DebugName.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/FileParser/FileParser.hpp>

CU_ImplementSmartPtr( c3d, Sampler )

namespace c3d
{
	namespace sampler
	{
		static String getName( ComparisonFunc value )
		{
			switch ( value )
			{
			case ComparisonFunc::eNever:
				return cuT( "Nev" );
			case ComparisonFunc::eLess:
				return cuT( "Lss" );
			case ComparisonFunc::eEqual:
				return cuT( "Eq" );
			case ComparisonFunc::eLessOrEqual:
				return cuT( "LEq" );
			case ComparisonFunc::eGreater:
				return cuT( "Gtr" );
			case ComparisonFunc::eNotEqual:
				return cuT( "NEq" );
			case ComparisonFunc::eGreaterOrEqual:
				return cuT( "GEq" );
			case ComparisonFunc::eAlways:
				return cuT( "Alw" );
			default:
				assert( false && "Unsupported ComparisonFunc." );
				return cuT( "Unsupported ComparisonFunc" );
			}
		}

		static String getName( FilterMode value )
		{
			switch ( value )
			{
			case FilterMode::eNearest:
				return cuT( "Near" );
			case FilterMode ::eLinear:
				return cuT( "Lin" );
			default:
				assert( false && "Unsupported FilterMode." );
				return cuT( "Unsupported FilterMode." );
			}
		}

		static String getName( MipmapMode value )
		{
			switch ( value )
			{
			case MipmapMode::eNearest:
				return cuT( "Near" );
			case MipmapMode::eLinear:
				return cuT( "Lin" );
			default:
				assert( false && "Unsupported MipmapMode." );
				return cuT( "Unsupported MipmapMode" );
			}
		}

		static String getName( WrapMode value )
		{
			switch ( value )
			{
			case WrapMode::eRepeat:
				return cuT( "Rep" );
			case WrapMode::eMirroredRepeat:
				return cuT( "MRep" );
			case WrapMode::eClampToEdge:
				return cuT( "CtE" );
			case WrapMode::eClampToBorder:
				return cuT( "CtB" );
			case WrapMode::eMirrorClampToEdge:
				return cuT( "MCtE" );
			default:
				assert( false && "Unsupported WrapMode." );
				return cuT( "Unsupported WrapMode." );
			}
		}

		static String getName( BorderColour value )
		{
			switch ( value )
			{
			case BorderColour::eFloatTransparentBlack:
				return cuT( "Ftb" );
			case BorderColour::eIntTransparentBlack:
				return cuT( "Itb" );
			case BorderColour::eFloatOpaqueBlack:
				return cuT( "Fob" );
			case BorderColour::eIntOpaqueBlack:
				return cuT( "Iob" );
			case BorderColour::eFloatOpaqueWhite:
				return cuT( "Fow" );
			case BorderColour::eIntOpaqueWhite:
				return cuT( "Iow" );
			default:
				assert( false && "Unsupported BorderColour." );
				return cuT( "Unsupported BorderColour." );
			}
		}

		static CU_ImplementAttributeParserBlock( parserMinFilter, SamplerContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( auto sampler = blockContext->sampler )
			{
				sampler->setMinFilter( FilterMode( params[0]->get< uint32_t >() ) );
			}
			else
			{
				CU_ParsingError( cuT( "No sampler initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserMagFilter, SamplerContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( auto sampler = blockContext->sampler )
			{
				sampler->setMagFilter( FilterMode( params[0]->get< uint32_t >() ) );
			}
			else
			{
				CU_ParsingError( cuT( "No sampler initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserMipFilter, SamplerContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( auto sampler = blockContext->sampler )
			{
				sampler->setMipFilter( MipmapMode( params[0]->get< uint32_t >() ) );
			}
			else
			{
				CU_ParsingError( cuT( "No sampler initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserMinLod, SamplerContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( auto sampler = blockContext->sampler )
			{
				float rValue = -1000;
				params[0]->get( rValue );

				if ( rValue >= -1000 && rValue <= 1000 )
				{
					sampler->setMinLod( rValue );
				}
				else
				{
					CU_ParsingError( cuT( "LOD out of bounds [-1000,1000] : " ) + string::toString( rValue ) );
				}
			}
			else
			{
				CU_ParsingError( cuT( "No sampler initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserMaxLod, SamplerContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( auto sampler = blockContext->sampler )
			{
				float rValue = 1000;
				params[0]->get( rValue );

				if ( rValue >= -1000 && rValue <= 1000 )
				{
					sampler->setMaxLod( rValue );
				}
				else
				{
					CU_ParsingError( cuT( "LOD out of bounds [-1000,1000] : " ) + string::toString( rValue ) );
				}
			}
			else
			{
				CU_ParsingError( cuT( "No sampler initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserLodBias, SamplerContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( auto sampler = blockContext->sampler )
			{
				float rValue = 1000;
				params[0]->get( rValue );

				if ( rValue >= -1000 && rValue <= 1000 )
				{
					sampler->setLodBias( rValue );
				}
				else
				{
					CU_ParsingError( cuT( "LOD out of bounds [-1000,1000] : " ) + string::toString( rValue ) );
				}
			}
			else
			{
				CU_ParsingError( cuT( "No sampler initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserUWrapMode, SamplerContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( auto sampler = blockContext->sampler )
			{
				sampler->setWrapS( WrapMode( params[0]->get< uint32_t >() ) );
			}
			else
			{
				CU_ParsingError( cuT( "No sampler initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserVWrapMode, SamplerContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( auto sampler = blockContext->sampler )
			{
				sampler->setWrapT( WrapMode( params[0]->get< uint32_t >() ) );
			}
			else
			{
				CU_ParsingError( cuT( "No sampler initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserWWrapMode, SamplerContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( auto sampler = blockContext->sampler )
			{
				sampler->setWrapR( WrapMode( params[0]->get< uint32_t >() ) );
			}
			else
			{
				CU_ParsingError( cuT( "No sampler initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserBorderColour, SamplerContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( auto sampler = blockContext->sampler )
			{
				sampler->setBorderColour( BorderColour( params[0]->get< uint32_t >() ) );
			}
			else
			{
				CU_ParsingError( cuT( "No sampler initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserAnisotropicFiltering, SamplerContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( auto sampler = blockContext->sampler )
			{
				sampler->enableAnisotropicFiltering( params[0]->get< bool >() );
			}
			else
			{
				CU_ParsingError( cuT( "No sampler initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserMaxAnisotropy, SamplerContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( auto sampler = blockContext->sampler )
			{
				float rValue = 1000;
				params[0]->get( rValue );
				sampler->setMaxAnisotropy( rValue );
			}
			else
			{
				CU_ParsingError( cuT( "No sampler initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserComparisonMode, SamplerContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( auto sampler = blockContext->sampler )
			{
				sampler->enableCompare( bool( params[0]->get< uint32_t >() ) );
			}
			else
			{
				CU_ParsingError( cuT( "No sampler initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserComparisonFunc, SamplerContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( auto sampler = blockContext->sampler )
			{
				sampler->setCompareOp( ComparisonFunc( params[0]->get< uint32_t >() ) );
			}
			else
			{
				CU_ParsingError( cuT( "No sampler initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserEnd, SamplerContext )
		{
			auto sampler = blockContext->sampler;

			if ( !blockContext->ownSampler
				&& !sampler )
			{
				CU_ParsingError( cuT( "No sampler initialised." ) );
			}
			else
			{
				log::info << "Loaded sampler [" << blockContext->sampler->getName() << "]" << std::endl;

				if ( blockContext->ownSampler )
				{
					getEngine( *blockContext )->addSampler( blockContext->ownSampler->getName()
						, blockContext->ownSampler
						, true );
				}

				blockContext->sampler = {};
			}
		}
		CU_EndAttributePop()
	}

	SamplerObs createSampler( Engine & engine
		, String const & baseName
		, FilterMode filter
		, ImageSubresourceRange const * range )
	{
		String const name = baseName
			+ cuT( "_" ) + makeString( getName( filter ) )
			+ ( range
				? cuT( "_" ) + string::toString( range->baseMipLevel ) + cuT( "_" ) + string::toString( range->levelCount )
				: String{} );
		SamplerObs sampler{};

		if ( engine.hasSampler( name ) )
		{
			sampler = engine.findSampler( name );
		}
		else
		{
			ashes::SamplerCreateInfo createInfo
			{
				0u,
				convert( filter ),
				convert( filter ),
				VK_SAMPLER_MIPMAP_MODE_NEAREST,
				VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
				VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
				VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
				0.0f, // mipLodBias
				VK_FALSE, // anisotropyEnable
				0.0f, // maxAnisotropy
				VK_FALSE, // compareEnable
				VK_COMPARE_OP_ALWAYS, // compareOp
				( range
					? float( range->baseMipLevel )
					: -1000.0f ), // minLod
				( range
					? float( range->baseMipLevel + range->levelCount )
					: 1000.0f ), // maxLod
			};
			auto resource = engine.createSampler( name
				, engine
				, c3d::move( createInfo ) );
			sampler = engine.addSampler( name
				, resource
				, false );
		}

		sampler->initialise( engine.getRenderSystem()->getRenderDevice() );
		return sampler;
	}

	String getSamplerName( ComparisonFunc compareOp
		, FilterMode minFilter
		, FilterMode magFilter
		, MipmapMode mipFilter
		, WrapMode U
		, WrapMode V
		, WrapMode W
		, BorderColour borderColor )
	{
		return sampler::getName( compareOp )
			+ sampler::getName( minFilter )
			+ sampler::getName( magFilter )
			+ sampler::getName( mipFilter )
			+ cuT( "U" ) + sampler::getName( U )
			+ cuT( "V" ) + sampler::getName( V )
			+ cuT( "W" ) + sampler::getName( W )
			+ sampler::getName( borderColor );
	}

	//*********************************************************************************************

	Sampler::Sampler( String const & name
		, Engine & engine )
		: Named{ name }
		, OwnedBy< Engine >{ engine }
	{
		CU_Require( m_info.sType == VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO );
	}
	
	Sampler::Sampler( String const & name
		, Engine & engine
		, ashes::SamplerCreateInfo const & createInfo )
		: Named{ name }
		, OwnedBy< Engine >{ engine }
		, m_info{ static_cast< VkSamplerCreateInfo const & >( createInfo ) }
	{
		CU_Require( m_info.sType == VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO );
	}

	bool Sampler::initialise( RenderDevice const & device )
	{
		if ( !m_initialised
			&& !m_sampler
			&& !m_initialising.exchange( true ) )
		{
			CU_Require( m_info.sType == VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO );
			m_info.maxAnisotropy = std::min( m_info.maxAnisotropy, device.properties.limits.maxSamplerAnisotropy );
			m_sampler = device->createSampler( toUtf8( getName() )
				, m_info );
			m_initialised = true;
			m_initialising = false;
		}

		return true;
	}

	void Sampler::cleanup()
	{
		m_initialised = false;
		m_sampler.reset();
	}

	void Sampler::addParsers( AttributeParsers & result )
	{
		BlockParserContextT< SamplerContext > context{ result, CSCNSection::eSampler };

		context.addParser( cuT( "min_filter" ), sampler::parserMinFilter, { makeParameter< ParameterType::eCheckedText, FilterMode >() } );
		context.addParser( cuT( "mag_filter" ), sampler::parserMagFilter, { makeParameter< ParameterType::eCheckedText, FilterMode >() } );
		context.addParser( cuT( "mip_filter" ), sampler::parserMipFilter, { makeParameter< ParameterType::eCheckedText, MipmapMode >() } );
		context.addParser( cuT( "min_lod" ), sampler::parserMinLod, { makeParameter< ParameterType::eFloat >() } );
		context.addParser( cuT( "max_lod" ), sampler::parserMaxLod, { makeParameter< ParameterType::eFloat >() } );
		context.addParser( cuT( "lod_bias" ), sampler::parserLodBias, { makeParameter< ParameterType::eFloat >() } );
		context.addParser( cuT( "u_wrap_mode" ), sampler::parserUWrapMode, { makeParameter< ParameterType::eCheckedText, WrapMode >() } );
		context.addParser( cuT( "v_wrap_mode" ), sampler::parserVWrapMode, { makeParameter< ParameterType::eCheckedText, WrapMode >() } );
		context.addParser( cuT( "w_wrap_mode" ), sampler::parserWWrapMode, { makeParameter< ParameterType::eCheckedText, WrapMode >() } );
		context.addParser( cuT( "border_colour" ), sampler::parserBorderColour, { makeParameter< ParameterType::eCheckedText, BorderColour >() } );
		context.addParser( cuT( "anisotropic_filtering" ), sampler::parserAnisotropicFiltering, { makeParameter< ParameterType::eBool >() } );
		context.addParser( cuT( "max_anisotropy" ), sampler::parserMaxAnisotropy, { makeParameter< ParameterType::eFloat >() } );
		context.addParser( cuT( "comparison_mode" ), sampler::parserComparisonMode, { makeParameter< ParameterType::eCheckedText, LimitedType< ComparisonFunc > >() } );
		context.addParser( cuT( "comparison_func" ), sampler::parserComparisonFunc, { makeParameter< ParameterType::eCheckedText, ComparisonFunc >() } );
		context.addPopParser( cuT( "}" ), sampler::parserEnd );
	}
}
