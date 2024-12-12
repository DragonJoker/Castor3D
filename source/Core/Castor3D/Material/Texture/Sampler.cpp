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

CU_ImplementSmartPtr( castor3d, Sampler )

namespace castor3d
{
	namespace sampler
	{
		static castor::String getName( VkCompareOp value )
		{
			switch ( value )
			{
			case VK_COMPARE_OP_NEVER:
				return cuT( "Nev" );
			case VK_COMPARE_OP_LESS:
				return cuT( "Lss" );
			case VK_COMPARE_OP_EQUAL:
				return cuT( "Eq" );
			case VK_COMPARE_OP_LESS_OR_EQUAL:
				return cuT( "LEq" );
			case VK_COMPARE_OP_GREATER:
				return cuT( "Gtr" );
			case VK_COMPARE_OP_NOT_EQUAL:
				return cuT( "NEq" );
			case VK_COMPARE_OP_GREATER_OR_EQUAL:
				return cuT( "GEq" );
			case VK_COMPARE_OP_ALWAYS:
				return cuT( "Alw" );
			default:
				assert( false && "Unsupported VkCompareOp." );
				return cuT( "Unsupported VkCompareOp" );
			}
		}

		static castor::String getName( VkFilter value )
		{
			switch ( value )
			{
			case VK_FILTER_NEAREST:
				return cuT( "Near" );
			case VK_FILTER_LINEAR:
				return cuT( "Lin" );
			case VK_FILTER_CUBIC_IMG:
				return cuT( "Cub" );
			default:
				assert( false && "Unsupported VkFilter." );
				return cuT( "Unsupported VkFilter" );
			}
		}

		static castor::String getName( VkSamplerMipmapMode value )
		{
			switch ( value )
			{
			case VK_SAMPLER_MIPMAP_MODE_NEAREST:
				return cuT( "Near" );
			case VK_SAMPLER_MIPMAP_MODE_LINEAR:
				return cuT( "Lin" );
			default:
				assert( false && "Unsupported VkSamplerMipmapMode." );
				return cuT( "Unsupported VkSamplerMipmapMode" );
			}
		}

		static castor::String getName( VkSamplerAddressMode value )
		{
			switch ( value )
			{
			case VK_SAMPLER_ADDRESS_MODE_REPEAT:
				return cuT( "Rep" );
			case VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT:
				return cuT( "MRep" );
			case VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE:
				return cuT( "CtE" );
			case VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER:
				return cuT( "CtB" );
			case VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE:
				return cuT( "MCtE" );
			default:
				assert( false && "Unsupported VkSamplerAddressMode." );
				return cuT( "Unsupported VkSamplerAddressMode" );
			}
		}

		static castor::String getName( VkBorderColor value )
		{
			switch ( value )
			{
			case VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK:
				return cuT( "Ftb" );
			case VK_BORDER_COLOR_INT_TRANSPARENT_BLACK:
				return cuT( "Itb" );
			case VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK:
				return cuT( "Fob" );
			case VK_BORDER_COLOR_INT_OPAQUE_BLACK:
				return cuT( "Iob" );
			case VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE:
				return cuT( "Fow" );
			case VK_BORDER_COLOR_INT_OPAQUE_WHITE:
				return cuT( "Iow" );
			case VK_BORDER_COLOR_FLOAT_CUSTOM_EXT:
				return cuT( "Fc" );
			case VK_BORDER_COLOR_INT_CUSTOM_EXT:
				return cuT( "Ic" );
			default:
				assert( false && "Unsupported VkBorderColor." );
				return cuT( "Unsupported VkBorderColor" );
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
				sampler->setMinFilter( VkFilter( params[0]->get< uint32_t >() ) );
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
				sampler->setMagFilter( VkFilter( params[0]->get< uint32_t >() ) );
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
				sampler->setMipFilter( VkSamplerMipmapMode( params[0]->get< uint32_t >() ) );
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
					CU_ParsingError( cuT( "LOD out of bounds [-1000,1000] : " ) + castor::string::toString( rValue ) );
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
					CU_ParsingError( cuT( "LOD out of bounds [-1000,1000] : " ) + castor::string::toString( rValue ) );
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
					CU_ParsingError( cuT( "LOD out of bounds [-1000,1000] : " ) + castor::string::toString( rValue ) );
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
				sampler->setWrapS( VkSamplerAddressMode( params[0]->get< uint32_t >() ) );
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
				sampler->setWrapT( VkSamplerAddressMode( params[0]->get< uint32_t >() ) );
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
				sampler->setWrapR( VkSamplerAddressMode( params[0]->get< uint32_t >() ) );
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
				sampler->setBorderColour( VkBorderColor( params[0]->get< uint32_t >() ) );
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
				sampler->setCompareOp( VkCompareOp( params[0]->get< uint32_t >() ) );
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
		, castor::String const & baseName
		, VkFilter filter
		, VkImageSubresourceRange const * range )
	{
		castor::String const name = baseName
			+ cuT( "_" ) + castor::makeString( ashes::getName( filter ) )
			+ ( range
				? cuT( "_" ) + castor::string::toString( range->baseMipLevel ) + cuT( "_" ) + castor::string::toString( range->levelCount )
				: castor::String{} );
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
				filter,
				filter,
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
				, castor::move( createInfo ) );
			sampler = engine.addNewSampler( name
				, resource
				, false );
		}

		sampler->initialise( engine.getRenderSystem()->getRenderDevice() );
		return sampler;
	}

	castor::String getSamplerName( VkCompareOp compareOp
		, VkFilter minFilter
		, VkFilter magFilter
		, VkSamplerMipmapMode mipFilter
		, VkSamplerAddressMode U
		, VkSamplerAddressMode V
		, VkSamplerAddressMode W
		, VkBorderColor borderColor )
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

	Sampler::Sampler( castor::String const & name
		, Engine & engine )
		: castor::Named{ name }
		, castor::OwnedBy< Engine >{ engine }
	{
		CU_Require( m_info.sType == VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO );
	}
	
	Sampler::Sampler( castor::String const & name
		, Engine & engine
		, ashes::SamplerCreateInfo const & createInfo )
		: castor::Named{ name }
		, castor::OwnedBy< Engine >{ engine }
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
			m_sampler = device->createSampler( castor::toUtf8( getName() )
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

	void Sampler::addParsers( castor::AttributeParsers & result )
	{
		using namespace castor;
		BlockParserContextT< SamplerContext > context{ result, CSCNSection::eSampler };

		context.addParser( cuT( "min_filter" ), sampler::parserMinFilter, { makeParameter< ParameterType::eCheckedText, VkFilter >() } );
		context.addParser( cuT( "mag_filter" ), sampler::parserMagFilter, { makeParameter< ParameterType::eCheckedText, VkFilter >() } );
		context.addParser( cuT( "mip_filter" ), sampler::parserMipFilter, { makeParameter< ParameterType::eCheckedText, VkSamplerMipmapMode >() } );
		context.addParser( cuT( "min_lod" ), sampler::parserMinLod, { makeParameter< ParameterType::eFloat >() } );
		context.addParser( cuT( "max_lod" ), sampler::parserMaxLod, { makeParameter< ParameterType::eFloat >() } );
		context.addParser( cuT( "lod_bias" ), sampler::parserLodBias, { makeParameter< ParameterType::eFloat >() } );
		context.addParser( cuT( "u_wrap_mode" ), sampler::parserUWrapMode, { makeParameter< ParameterType::eCheckedText, VkSamplerAddressMode >() } );
		context.addParser( cuT( "v_wrap_mode" ), sampler::parserVWrapMode, { makeParameter< ParameterType::eCheckedText, VkSamplerAddressMode >() } );
		context.addParser( cuT( "w_wrap_mode" ), sampler::parserWWrapMode, { makeParameter< ParameterType::eCheckedText, VkSamplerAddressMode >() } );
		context.addParser( cuT( "border_colour" ), sampler::parserBorderColour, { makeParameter< ParameterType::eCheckedText, VkBorderColor >() } );
		context.addParser( cuT( "anisotropic_filtering" ), sampler::parserAnisotropicFiltering, { makeParameter< ParameterType::eBool >() } );
		context.addParser( cuT( "max_anisotropy" ), sampler::parserMaxAnisotropy, { makeParameter< ParameterType::eFloat >() } );
		context.addParser( cuT( "comparison_mode" ), sampler::parserComparisonMode, { makeParameter< ParameterType::eCheckedText, LimitedType< VkCompareOp > >() } );
		context.addParser( cuT( "comparison_func" ), sampler::parserComparisonFunc, { makeParameter< ParameterType::eCheckedText, VkCompareOp >() } );
		context.addPopParser( cuT( "}" ), sampler::parserEnd );
	}
}
