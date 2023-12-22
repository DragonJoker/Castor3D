#include "Castor3D/Material/Pass/Component/ComponentModule.hpp"

#include "Castor3D/Material/Pass/Component/PassComponent.hpp"
#include "Castor3D/Render/RenderModule.hpp"

namespace castor3d
{
	bool operator==( PassComponentCombine const & lhs, PassComponentCombine const & rhs )noexcept
	{
		return lhs.baseId == rhs.baseId
			&& lhs.flags == rhs.flags;
	}

	bool hasAny( PassComponentCombine const & lhs
		, PassComponentFlag rhs )noexcept
	{
		return hasAny( lhs.flags, rhs );
	}

	bool hasAny( PassComponentCombine const & lhs
		, std::vector< PassComponentFlag > const & rhs )noexcept
	{
		return std::any_of( rhs.begin()
			, rhs.end()
			, [&lhs]( PassComponentFlag const & lookup )
			{
				return hasAny( lhs, lookup );
			} );
	}

	void remFlags( PassComponentCombine & lhs
		, PassComponentFlag rhs )noexcept
	{
		auto it = lhs.flags.find( rhs );

		if ( it != lhs.flags.end() )
		{
			auto componentId = splitPassComponentFlag( *it ).first;
			lhs.flags.erase( it );
			lhs.flags.emplace( makePassComponentFlag( componentId, PassFlag::eNone ) );
		}
	}

	void remFlags( PassComponentCombine & lhs
		, PassComponentFlagsSet const & rhs )noexcept
	{
		for ( auto flag : rhs )
		{
			remFlags( lhs, flag );
		}
	}

	void addFlags( PassComponentCombine & lhs
		, PassComponentFlag rhs )noexcept
	{
		auto split = splitPassComponentFlag( rhs );
		auto componentId = split.first;
		auto rhsComponentFlag = split.second;
		auto it = std::find_if( lhs.flags.begin()
			, lhs.flags.end()
			, [componentId]( PassComponentFlag lookup )
			{
				return componentId == splitPassComponentFlag( lookup ).first;
			} );

		if ( it != lhs.flags.end() )
		{
			auto lhsComponentFlag = splitPassComponentFlag( *it ).second;
			lhs.flags.erase( it );
			lhs.flags.emplace( makePassComponentFlag( componentId
				, lhsComponentFlag | rhsComponentFlag ) );
		}
		else
		{
			lhs.flags.emplace( rhs );
		}
	}

	void addFlags( PassComponentCombine & lhs
		, PassComponentFlagsSet const & rhs )noexcept
	{
		for ( auto flag : rhs )
		{
			addFlags( lhs, flag );
		}
	}

	bool contains( PassComponentCombine const & cont
		, PassComponentFlag test )noexcept
	{
		auto split = splitPassComponentFlag( test );
		auto testComponentId = split.first;
		auto testComponentFlag = split.second;
		return std::any_of( cont.flags.begin()
			, cont.flags.end()
			, [testComponentId, testComponentFlag]( PassComponentFlag const & lookup )
			{
				auto [lookupComponentId, lookupComponentFlag] = splitPassComponentFlag( lookup );
				return lookupComponentId == testComponentId
					&& ( testComponentFlag == 0u
						|| castor::hasAll( lookupComponentFlag, testComponentFlag ) );
			} );
	}

	bool contains( PassComponentCombine const & cont
		, PassComponentCombine const & test )noexcept
	{
		if ( cont.baseId == test.baseId )
		{
			return true;
		}

		auto result = true;

		for ( auto flag : test.flags )
		{
			if ( result )
			{
				result = contains( cont, flag );
			}
		}

		return result;
	}
}
