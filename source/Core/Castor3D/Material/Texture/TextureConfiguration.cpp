#include "Castor3D/Material/Texture/TextureConfiguration.hpp"

#include "Castor3D/Render/RenderModule.hpp"

#include <CastorUtils/Graphics/PixelFormat.hpp>
#include <CastorUtils/Miscellaneous/Hash.hpp>

namespace castor3d
{
	//*********************************************************************************************

	namespace texconf
	{
		static void updateStartIndex( castor::PixelFormat format
			, TextureFlagConfiguration & config )
		{
			if ( config.componentsMask )
			{
				auto components = getPixelComponents( config.componentsMask );

				switch ( components.size() )
				{
				case 1:
					config.startIndex = castor::getComponentIndex( *components.begin(), format );
					break;
				case 3:
					if ( config.componentsMask & 0xFF000000 )
					{
						config.startIndex = 1;
					}
					else
					{
						config.startIndex = 0;
					}
					break;
				default:
					CU_Failure( "Invalid component count for a texture component flag" );
					config.startIndex = 0;
					break;
				}
			}
		}

		static void merge( bool lhs
			, bool & rhs )
		{
			rhs = lhs || rhs;
		}

		static void merge( float lhs
			, float & rhs
			, float ref )
		{
			if ( lhs != rhs )
			{
				rhs = ( lhs == ref
					? rhs
					: lhs );
			}
		}
	}

	//*********************************************************************************************

	bool operator==( TextureTransform const & lhs
		, TextureTransform const & rhs )noexcept
	{
		return lhs.translate == rhs.translate
			&& lhs.rotate == rhs.rotate
			&& lhs.scale == rhs.scale;
	}

	//*********************************************************************************************

	bool shallowEqual( TextureFlagConfiguration const & lhs
		, TextureFlagConfiguration const & rhs )
	{
		return lhs.flag == rhs.flag
			&& lhs.componentsMask == rhs.componentsMask;
	}

	bool operator==( TextureFlagConfiguration const & lhs
		, TextureFlagConfiguration const & rhs )noexcept
	{
		return lhs.flag == rhs.flag
			&& lhs.componentsMask == rhs.componentsMask
			&& lhs.startIndex == rhs.startIndex;
	}

	//*********************************************************************************************

	bool shallowEqual( TextureConfiguration const & lhs
		, TextureConfiguration const & rhs )
	{
		return shallowEqual( lhs.components[0], rhs.components[0] )
			&& shallowEqual( lhs.components[1], rhs.components[1] )
			&& shallowEqual( lhs.components[2], rhs.components[2] )
			&& shallowEqual( lhs.components[3], rhs.components[3] );
	}

	bool operator==( TextureConfiguration const & lhs
		, TextureConfiguration const & rhs )
	{
		return lhs.components[0] == rhs.components[0]
			&& lhs.components[1] == rhs.components[1]
			&& lhs.components[2] == rhs.components[2]
			&& lhs.components[3] == rhs.components[3]
			&& lhs.normalFactor == rhs.normalFactor
			&& lhs.heightFactor == rhs.heightFactor
			&& lhs.normalDirectX == rhs.normalDirectX
			&& lhs.needsYInversion == rhs.needsYInversion
			&& lhs.normal2Channels == rhs.normal2Channels
			&& lhs.transform.translate == rhs.transform.translate
			&& lhs.transform.rotate == rhs.transform.rotate
			&& lhs.transform.scale == rhs.transform.scale;
	}

	//*********************************************************************************************

	size_t getHash( TextureFlagConfiguration const & config )
	{
		size_t result = std::hash< PassComponentTextureFlag >{}( config.flag );
		castor::hashCombine( result, config.startIndex );
		castor::hashCombine( result, config.componentsMask );
		return result;
	}

	size_t getHash( TextureConfiguration const & config )
	{
		auto flags = getFlags( config );
		size_t result{};

		for ( auto flag : flags )
		{
			castor::hashCombine( result, flag );
		}

		castor::hashCombine( result, config.normalFactor );
		castor::hashCombine( result, config.heightFactor );
		castor::hashCombine( result, config.normalDirectX );
		castor::hashCombine( result, config.needsYInversion );
		castor::hashCombine( result, config.normal2Channels );
		castor::hashCombine( result, config.transform.translate->x );
		castor::hashCombine( result, config.transform.translate->y );
		castor::hashCombine( result, config.transform.rotate.radians() );
		castor::hashCombine( result, config.transform.scale->x );
		castor::hashCombine( result, config.transform.scale->y );
		return result;
	}

	TextureFlagsSet getFlags( TextureConfiguration const & config )
	{
		TextureFlagsSet result;

		for ( auto & conf : config.components )
		{
			if ( conf.componentsMask )
			{
				result.insert( conf.flag );
			}
		}

		return result;
	}

	castor::PixelComponents getPixelComponents( uint32_t mask )
	{
		castor::PixelComponents result;

		if ( mask & 0xFF000000 )
		{
			result |= castor::PixelComponent::eAlpha;
		}

		if ( mask & 0x00FF0000 )
		{
			result |= castor::PixelComponent::eRed;
		}

		if ( mask & 0x0000FF00 )
		{
			result |= castor::PixelComponent::eGreen;
		}

		if ( mask & 0x000000FF )
		{
			result |= castor::PixelComponent::eBlue;
		}

		return result;
	}

	castor::PixelComponents getPixelComponents( TextureConfiguration const & config )
	{
		castor::PixelComponents result;
		result |= getPixelComponents( config.components[0].componentsMask );
		result |= getPixelComponents( config.components[1].componentsMask );
		result |= getPixelComponents( config.components[2].componentsMask );
		result |= getPixelComponents( config.components[3].componentsMask );
		return result;
	}

	void updateIndices( castor::PixelFormat format
		, TextureConfiguration & config )
	{
		texconf::updateStartIndex( format, config.components[0] );
		texconf::updateStartIndex( format, config.components[1] );
		texconf::updateStartIndex( format, config.components[2] );
		texconf::updateStartIndex( format, config.components[3] );
	}

	TextureFlagConfigurations::const_iterator checkFlag( TextureFlagConfigurations const & lhs
		, PassComponentTextureFlag rhs )
	{
		CU_Require( splitTextureFlag( rhs ).second.size() == 1u );
		auto it = std::find_if( lhs.begin()
			, lhs.end()
			, [rhs]( TextureFlagConfiguration const & lookup )
			{
				return lookup.flag == rhs;
			} );
		return it;
	}

	TextureFlagConfigurations::iterator checkFlag( TextureFlagConfigurations & lhs
		, PassComponentTextureFlag rhs )
	{
		CU_Require( splitTextureFlag( rhs ).second.size() == 1u );
		auto it = std::find_if( lhs.begin()
			, lhs.end()
			, [rhs]( TextureFlagConfiguration const & lookup )
			{
				return lookup.flag == rhs;
			} );
		return it;
	}

	bool hasAny( TextureFlagConfigurations const & lhs
		, PassComponentTextureFlag rhs )
	{
		auto [id, flags] = splitTextureFlag( rhs );
		auto rhsPassIndex = id;
		auto rhsTextureFlag = flags;
		auto it = std::find_if( lhs.begin()
			, lhs.end()
			, [rhsPassIndex, rhsTextureFlag]( TextureFlagConfiguration const & lookup )
			{
				auto [lhsPassIndex, lhsTextureFlag] = splitTextureFlag( lookup.flag );
				return lhsPassIndex == rhsPassIndex
					&& castor::hasAny( lhsTextureFlag, rhsTextureFlag );
			} );
		return it != lhs.end();
	}

	bool removeFlag( TextureConfiguration & config
		, PassComponentTextureFlag rhs )
	{
		bool found{};

		if ( auto it = checkFlag( config.components, rhs );
			it != config.components.end() )
		{
			found = true;
			auto nxt = std::next( it );

			while ( nxt != config.components.end() )
			{
				*it = castor::move( *nxt );
				++it;
				++nxt;
			}
		}

		return found;
	}

	void removeFlagConfiguration( TextureConfiguration & config
		, TextureFlagConfiguration const & flagConfiguration )
	{
		removeFlag( config, flagConfiguration.flag );
	}

	void addFlagConfiguration( TextureConfiguration & config
		, TextureFlagConfiguration flagConfiguration )
	{
		auto it = checkFlag( config.components, flagConfiguration.flag );

		if ( it == config.components.end() )
		{
			it = findFirstEmpty( config );

			if ( it == config.components.end() )
			{
				CU_Exception( "Can't add component to this texture configuration" );
			}
		}

		*it = castor::move( flagConfiguration );
	}

	TextureFlagConfiguration & getFlagConfiguration( TextureConfiguration & configuration
		, PassComponentTextureFlag textureFlag )
	{
		auto it = checkFlag( configuration.components, textureFlag );

		if ( it == configuration.components.end() )
		{
			CU_Exception( "Component texture flag was not found in the texture configuration" );
		}

		return *it;
	}

	uint32_t getComponentsMask( TextureConfiguration const & configuration
		, PassComponentTextureFlag textureFlag )
	{
		auto it = checkFlag( configuration.components, textureFlag );
		return it != configuration.components.end()
			? it->componentsMask
			: 0u;
	}

	PassComponentTextureFlag getEnabledFlag( TextureConfiguration const & configuration )
	{
		auto it = std::find_if( configuration.components.begin()
			, configuration.components.end()
			, []( TextureFlagConfiguration const & lookup )
			{
				return lookup.componentsMask != 0u;
			} );
		CU_Require( it != configuration.components.end() );
		return it->flag;
	}

	void mergeConfigs( TextureConfiguration const & lhs
		, TextureConfiguration & rhs )
	{
		mergeConfigsBase( lhs, rhs );
		auto rhsit = findFirstEmpty( rhs );

		if ( rhsit == rhs.components.end() )
		{
			// Can't add any configuration.
			CU_Require( rhsit == rhs.components.begin() );
			return;
		}

		auto lhsit = findFirstNonEmpty( lhs );

		if ( lhsit == lhs.components.end() )
		{
			// No configuration to add.
			return;
		}

		while ( lhsit != lhs.components.end()
			&& rhsit != rhs.components.end() )
		{
			*rhsit = *lhsit;
			++lhsit;
			++rhsit;
		}

		// We should have been able to merge all configuration components
		CU_Require( lhsit == lhs.components.end()
			|| lhsit->flag == 0 );
	}

	void mergeConfigsBase( TextureConfiguration const & lhs
		, TextureConfiguration & rhs )
	{
		texconf::merge( lhs.needsYInversion, rhs.needsYInversion );
		texconf::merge( lhs.normalDirectX, rhs.normalDirectX );
		texconf::merge( lhs.normal2Channels, rhs.normal2Channels );

		texconf::merge( lhs.heightFactor, rhs.heightFactor, 0.1f );
		texconf::merge( lhs.normalFactor, rhs.normalFactor, 1.0f );

		rhs.textureSpace |= lhs.textureSpace;
	}

	TextureFlagConfigurations::const_iterator findFirstEmpty( TextureConfiguration const & config )
	{
		return std::find_if( config.components.begin()
			, config.components.end()
			, []( TextureFlagConfiguration const & lookup )
			{
				return lookup.flag == 0
					|| lookup.componentsMask == 0;
			} );
	}

	TextureFlagConfigurations::iterator findFirstEmpty( TextureConfiguration & config )
	{
		return std::find_if( config.components.begin()
			, config.components.end()
			, []( TextureFlagConfiguration const & lookup )
			{
				return lookup.flag == 0
					|| lookup.componentsMask == 0;
			} );
	}

	TextureFlagConfigurations::const_iterator findFirstNonEmpty( TextureConfiguration const & config )
	{
		return std::find_if( config.components.begin()
			, config.components.end()
			, []( TextureFlagConfiguration const & lookup )
			{
				return lookup.flag != 0
					&& lookup.componentsMask != 0;
			} );
	}

	TextureFlagConfigurations::iterator findFirstNonEmpty( TextureConfiguration & config )
	{
		return std::find_if( config.components.begin()
			, config.components.end()
			, []( TextureFlagConfiguration const & lookup )
			{
				return lookup.flag != 0
					&& lookup.componentsMask != 0;
			} );
	}

	//*********************************************************************************************
}
