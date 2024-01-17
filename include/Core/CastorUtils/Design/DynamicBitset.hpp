/*
See LICENSE file in root folder
*/
#ifndef ___CU_DynamicBitset_H___
#define ___CU_DynamicBitset_H___

#include "CastorUtils/Design/DesignModule.hpp"

namespace castor
{
	template< typename BlockTypeT >
	class DynamicBitsetT
	{
		class Bit;

	public:
		/**
		*\~english
		*name
		*	Construction/Destruction.
		*\~french
		*name
		*	Construction/Destruction.
		*/
		/**@{*/
		DynamicBitsetT();
		explicit DynamicBitsetT( size_t size, bool value = false );
		explicit DynamicBitsetT( String const & bits );
		explicit DynamicBitsetT( char const * bits );
		DynamicBitsetT( char const * bits, size_t size );
		/**@}*/
		/**
		*\~english
		*name
		*	Bitset functions.
		*\~french
		*name
		*	Fonctions d'ensemble de bits.
		*/
		/**@{*/
		void set( size_t bit, bool value = true );
		bool get( size_t bit )const;
		BlockTypeT getBlock( size_t index )const;
		void reset();
		void resize( size_t size, bool value );
		size_t getSize()const;
		size_t getBlockCount()const;
		bool none()const;
		bool any()const;
		bool all()const;
		/**@}*/
		/**
		*\~english
		*name
		*	Array access.
		*\~french
		*name
		*	Accès de tableau.
		*/
		/**@{*/
		Bit operator[]( size_t index );
		bool operator[]( size_t index )const;
		/**@}*/
		/**
		*\~english
		*name
		*	Bit operations.
		*\~french
		*name
		*	Opérations bit à bit.
		*/
		/**@{*/
		DynamicBitsetT & operator<<=( int value );
		DynamicBitsetT & operator>>=( int value );
		DynamicBitsetT & operator&=( DynamicBitsetT const & value );
		DynamicBitsetT & operator|=( DynamicBitsetT const & value );
		DynamicBitsetT & operator^=( DynamicBitsetT const & value );
		DynamicBitsetT operator~()const;
		/**@}*/
		String toString()const;

		using BlockType = BlockTypeT;
		static constexpr BlockTypeT fullBitMask = std::numeric_limits< BlockTypeT >::max();
		static constexpr size_t bitsPerBlock = sizeof( BlockTypeT ) * 8u;

	private:
		void doResetExtraBits();
		BlockTypeT doGetLastBlockMask()const;

	private:
		std::vector< BlockTypeT > m_blocks;
		size_t m_bitCount;
	};
	/**
	*\~english
	*name
	*	Logic operators.
	*\~french
	*name
	*	Opérateurs logiques.
	*/
	/**@{*/
	template< typename BlockTypeT >
	inline bool operator==( DynamicBitsetT< BlockTypeT > const & lhs
		, DynamicBitsetT< BlockTypeT > const & rhs );
	template< typename BlockTypeT >
	inline bool operator!=( DynamicBitsetT< BlockTypeT > const & lhs
		, DynamicBitsetT< BlockTypeT > const & rhs );
	/**@}*/
	/**
	*\~english
	*name
	*	Bit operations.
	*\~french
	*name
	*	Opérations bit à bit.
	*/
	/**@{*/
	template< typename BlockTypeT >
	inline DynamicBitsetT< BlockTypeT > operator<<( DynamicBitsetT< BlockTypeT > const & lhs
		, int rhs );
	template< typename BlockTypeT >
	inline DynamicBitsetT< BlockTypeT > operator>>( DynamicBitsetT< BlockTypeT > const & lhs
		, int rhs );
	template< typename BlockTypeT >
	inline DynamicBitsetT< BlockTypeT > operator&( DynamicBitsetT< BlockTypeT > const & lhs
		, DynamicBitsetT< BlockTypeT > const & rhs );
	template< typename BlockTypeT >
	inline DynamicBitsetT< BlockTypeT > operator|( DynamicBitsetT< BlockTypeT > const & lhs
		, DynamicBitsetT< BlockTypeT > const & rhs );
	template< typename BlockTypeT >
	inline DynamicBitsetT< BlockTypeT > operator^( DynamicBitsetT< BlockTypeT > const & lhs
		, DynamicBitsetT< BlockTypeT > const & rhs );
	/**@}*/
	/**
	*\~english
	*\brief
	*	Bit class, coming from a bitset.
	*\~french
	*\brief
	*	Bit dynamique, avec un type de bloc configurable.
	*/
	template< typename BlockTypeT >
	class DynamicBitsetT< BlockTypeT >::Bit
		: public NonMovable
	{
		friend DynamicBitsetT< BlockTypeT >;

	private:
		Bit( BlockTypeT & block
			, BlockTypeT mask )
			: m_block{ block }
			, m_mask{ mask }
		{
		}

	public:
		Bit & operator=( bool val );
		Bit & operator=( Bit const & bit );

		void set( bool val = true );

		template< bool BadCall = true >
		void * operator&()const;

		explicit operator bool()const;

		Bit & operator|=( bool value );
		Bit & operator&=( bool value );
		Bit & operator^=( bool value );

	private:
		BlockTypeT & m_block;
		BlockTypeT m_mask;
	};
	/**
	*\~english
	*name
	*	Logic operators.
	*\~french
	*name
	*	Opérateurs logiques.
	*/
	/**@{*/
	template< typename BlockTypeT >
	inline bool operator==( typename DynamicBitsetT< BlockTypeT >::Bit const & lhs
		, typename DynamicBitsetT< BlockTypeT >::Bit const & rhs );
	template< typename BlockTypeT >
	inline bool operator!=( typename DynamicBitsetT< BlockTypeT >::Bit const & lhs
		, typename DynamicBitsetT< BlockTypeT >::Bit const & rhs );
	/**@}*/
	/**
	*\~english
	*name
	*	Bit operations.
	*\~french
	*name
	*	Opérations bit à bit.
	*/
	/**@{*/
	template< typename BlockTypeT >
	typename DynamicBitsetT< BlockTypeT >::Bit operator|( typename DynamicBitsetT< BlockTypeT >::Bit const & lhs
		, bool rhs );
	template< typename BlockTypeT >
	typename DynamicBitsetT< BlockTypeT >::Bit operator&( typename DynamicBitsetT< BlockTypeT >::Bit const & lhs
		, bool rhs );
	template< typename BlockTypeT >
	typename DynamicBitsetT< BlockTypeT >::Bit operator^( typename DynamicBitsetT< BlockTypeT >::Bit const & lhs
		, bool rhs );
	/**@}*/
}

#include "DynamicBitset.inl"

#endif
