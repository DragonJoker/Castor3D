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
		inline DynamicBitsetT();
		inline explicit DynamicBitsetT( size_t size, bool value = false );
		inline explicit DynamicBitsetT( String const & bits );
		inline explicit DynamicBitsetT( char const * bits );
		inline DynamicBitsetT( char const * bits, size_t size );
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
		inline void set( size_t bit, bool value = true );
		inline bool get( size_t bit )const;
		inline BlockTypeT getBlock( size_t index )const;
		inline void reset();
		inline void resize( size_t size, bool value );
		inline size_t getSize()const;
		inline size_t getBlockCount()const;
		inline bool none()const;
		inline bool any()const;
		inline bool all()const;
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
		inline Bit operator[]( size_t index );
		inline bool operator[]( size_t index )const;
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
		inline DynamicBitsetT & operator<<=( int value );
		inline DynamicBitsetT & operator>>=( int value );
		inline DynamicBitsetT & operator&=( DynamicBitsetT const & value );
		inline DynamicBitsetT & operator|=( DynamicBitsetT const & value );
		inline DynamicBitsetT & operator^=( DynamicBitsetT const & value );
		inline DynamicBitsetT operator~()const;
		/**@}*/
		inline String toString()const;

		using BlockType = BlockTypeT;
		static constexpr BlockTypeT fullBitMask = std::numeric_limits< BlockTypeT >::max();
		static constexpr size_t bitsPerBlock = sizeof( BlockTypeT ) * 8u;

	private:
		inline void doResetExtraBits();
		inline BlockTypeT doGetLastBlockMask()const;

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
	{
		friend DynamicBitsetT< BlockTypeT >;

	private:
		inline Bit( BlockTypeT & block
			, BlockTypeT mask )
			: m_block{ block }
			, m_mask{ mask }
		{
		}

	public:
		inline Bit & operator=( bool val );
		inline Bit & operator=( Bit const & bit );

		inline void set( bool val = true );

		template< bool BadCall = true >
		inline void * operator&() const;

		explicit operator bool()const;

		inline Bit & operator|=( bool value );
		inline Bit & operator&=( bool value );
		inline Bit & operator^=( bool value );

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
