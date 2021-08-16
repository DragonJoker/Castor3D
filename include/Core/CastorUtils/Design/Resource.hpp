/*
See LICENSE file in root folder
*/
#ifndef ___CU_Resource_HPP___
#define ___CU_Resource_HPP___

#include "CastorUtils/Design/DesignModule.hpp"

#include "CastorUtils/Design/Named.hpp"
#include "CastorUtils/Design/Signal.hpp"

#include <atomic>
#include <functional>

namespace castor
{
	class Resource
		: public Named
	{
	protected:
		CU_API explicit Resource( String const & name );

	public:
		CU_API Resource( Resource const & other ) = default;
		CU_API Resource( Resource && other ) = default;
		CU_API Resource & operator=( Resource const & other ) = delete;
		CU_API Resource & operator=( Resource && other ) = delete;
		CU_API virtual ~Resource() = default;

		CU_API void initialise();
		CU_API void cleanup();
		CU_API void reinitialise();

		bool isInitialised()const
		{
			return m_initialised;
		}

		using OnInitialisedFunc = std::function< void( Resource const & ) >;
		using OnInitialised = Signal< OnInitialisedFunc >;
		using Connection = Connection< OnInitialised >;

		OnInitialised onInitialised;

	private:
		virtual void doInitialise() = 0;
		virtual void doCleanup() = 0;

	private:
		std::atomic_bool m_initialised;
	};
}

#endif
