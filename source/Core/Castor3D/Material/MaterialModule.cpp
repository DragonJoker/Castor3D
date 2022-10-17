#include "Castor3D/Material/MaterialModule.hpp"

namespace castor3d
{
	VkFormat convert( castor::PixelFormat format )
	{
		return VkFormat( format );
	}

	castor::PixelFormat convert( VkFormat format )
	{
		return castor::PixelFormat( format );
	}

	bool operator==( TextureCombine const & lhs, TextureCombine const & rhs )
	{
		return lhs.configCount == rhs.configCount
			&& lhs.flags == rhs.flags;
	}

	TextureFlagsSet::const_iterator checkFlag( TextureCombine const & lhs
		, PassComponentTextureFlag rhs )
	{
		CU_Require( splitTextureFlag( rhs ).second.size() == 1u );
		return lhs.flags.find( rhs );
	}

	bool hasAny( TextureFlagsSet const & lhs
		, PassComponentTextureFlag rhs )
	{
		auto split = splitTextureFlag( rhs );
		auto rhsComponentId = split.first;
		auto rhsTextureFlag = split.second;
		auto it = std::find_if( lhs.begin()
			, lhs.end()
			, [rhsComponentId, rhsTextureFlag]( PassComponentTextureFlag const & lookup )
			{
				auto [lhsComponentId, lhsTextureFlag] = splitTextureFlag( lookup );
				return lhsComponentId == rhsComponentId
					&& castor::hasAny( lhsTextureFlag, rhsTextureFlag );
			} );
		return it != lhs.end();
	}

	bool hasAny( TextureCombine const & lhs
		, PassComponentTextureFlag rhs )
	{
		return hasAny( lhs.flags, rhs );
	}

	bool hasIntersect( PassComponentTextureFlag lhs
		, PassComponentTextureFlag rhs )
	{
		auto [lhsComponentId, lhsTextureFlag] = splitTextureFlag( lhs );
		auto [rhsComponentId, rhsTextureFlag] = splitTextureFlag( rhs );
		return lhsComponentId == rhsComponentId
			&& ( lhsTextureFlag & rhsTextureFlag ) != 0;
	}

	void remFlags( TextureCombine & lhs
		, PassComponentTextureFlag rhs )
	{
		auto it = lhs.flags.find( rhs );

		if ( it != lhs.flags.end() )
		{
			lhs.flags.erase( it );
		}
	}

	void remFlags( TextureCombine & lhs
		, TextureFlagsSet const & rhs )
	{
		for ( auto flag : rhs )
		{
			remFlags( lhs, flag );
		}
	}

	void addFlags( TextureCombine & lhs
		, PassComponentTextureFlag rhs )
	{
		lhs.flags.insert( rhs );
	}

	void addFlags( TextureCombine & lhs
		, TextureFlagsSet const & rhs )
	{
		for ( auto flag : rhs )
		{
			addFlags( lhs, flag );
		}
	}

	bool contains( TextureCombine const & cont
		, TextureCombine const & test )
	{
		auto result = cont.configCount >= test.configCount;

		for ( auto flag : test.flags )
		{
			if ( result )
			{
				result = cont.flags.end() != checkFlag( cont, flag );
			}
		}

		return result;
	}
}
