/*
See LICENSE file in root folder
*/
#ifndef ___CU_Resource_HPP___
#define ___CU_Resource_HPP___

#include "CastorUtils/Design/DesignModule.hpp"

#include "CastorUtils/Design/Collection.hpp"
#include "CastorUtils/Design/Named.hpp"

namespace castor
{
	template< class ResType >
	class Resource
		: public Named
	{
	protected:
		using MyCollection = Collection< ResType, String >;

		Resource( MyCollection & collection
			, String const & name );
		explicit Resource( String const & name );
		~Resource() = default;

	public:
		Resource( Resource const & other ) = default;
		Resource( Resource && other ) = default;
		Resource & operator=( Resource const & other ) = default;
		Resource & operator=( Resource && other ) = default;
		/**
		 *\~english
		 *\remarks		Tells the collection the new resource name
		 *\param[in]	name	The new name
		 *\~french
		 *\remarks		Dit à la collection le nouveau de la ressource
		 *\param[in]	name	Le nouveau nom
		 */
		void changeName( String const & name );

	private:
		MyCollection * m_collection;
	};
}

#include "Resource.inl"

#endif
