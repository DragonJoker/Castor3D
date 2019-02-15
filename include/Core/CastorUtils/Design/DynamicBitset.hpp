/*
See LICENSE file in root folder
*/
#ifndef ___CU_DynamicBitset_H___
#define ___CU_DynamicBitset_H___

#include "CastorUtils/CastorUtilsPrerequisites.hpp"

namespace castor
{
	/**
	*\~english
	*\brief
	*	Dynamic bitset class, with configurable block type.
	*\~french
	*\brief
	*	Classe de bitset dynamique, avec un type de bloc configurable.
	*/
	template< typename BlockType >
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
		inline BlockType getBlock( size_t index )const;
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

		static constexpr BlockType fullBitMask = std::numeric_limits< BlockType >::max();
		static constexpr size_t bitsPerBlock = sizeof( BlockType ) * 8u;

	private:
		inline void doResetExtraBits();
		inline BlockType doGetLastBlockMask()const;

	private:
		std::vector< BlockType > m_blocks;
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
	template< typename BlockType >
	inline bool operator==( DynamicBitsetT< BlockType > const & lhs
		, DynamicBitsetT< BlockType > const & rhs );
	template< typename BlockType >
	inline bool operator!=( DynamicBitsetT< BlockType > const & lhs
		, DynamicBitsetT< BlockType > const & rhs );
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
	template< typename BlockType >
	inline DynamicBitsetT< BlockType > operator<<( DynamicBitsetT< BlockType > const & lhs
		, int rhs );
	template< typename BlockType >
	inline DynamicBitsetT< BlockType > operator>>( DynamicBitsetT< BlockType > const & lhs
		, int rhs );
	template< typename BlockType >
	inline DynamicBitsetT< BlockType > operator&( DynamicBitsetT< BlockType > const & lhs
		, DynamicBitsetT< BlockType > const & rhs );
	template< typename BlockType >
	inline DynamicBitsetT< BlockType > operator|( DynamicBitsetT< BlockType > const & lhs
		, DynamicBitsetT< BlockType > const & rhs );
	template< typename BlockType >
	inline DynamicBitsetT< BlockType > operator^( DynamicBitsetT< BlockType > const & lhs
		, DynamicBitsetT< BlockType > const & rhs );
	/**@}*/
	/**
	*\~english
	*\brief
	*	Bit class, coming from a bitset.
	*\~french
	*\brief
	*	Bit dynamique, avec un type de bloc configurable.
	*/
	template< typename BlockType >
	class DynamicBitsetT< BlockType >::Bit
	{
		friend DynamicBitsetT< BlockType >;

	private:
		inline Bit( BlockType & block
			, BlockType mask )
			: m_block{ block }
			, m_mask{ mask }
		{
		}

	public:
		inline Bit( Bit const & bit ) = default;
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
		BlockType & m_block;
		BlockType m_mask;
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
	template< typename BlockType >
	inline bool operator==( typename DynamicBitsetT< BlockType >::Bit const & lhs
		, typename DynamicBitsetT< BlockType >::Bit const & rhs );
	template< typename BlockType >
	inline bool operator!=( typename DynamicBitsetT< BlockType >::Bit const & lhs
		, typename DynamicBitsetT< BlockType >::Bit const & rhs );
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
	template< typename BlockType >
	typename DynamicBitsetT< BlockType >::Bit operator|( typename DynamicBitsetT< BlockType >::Bit const & lhs
		, bool rhs );
	template< typename BlockType >
	typename DynamicBitsetT< BlockType >::Bit operator&( typename DynamicBitsetT< BlockType >::Bit const & lhs
		, bool rhs );
	template< typename BlockType >
	typename DynamicBitsetT< BlockType >::Bit operator^( typename DynamicBitsetT< BlockType >::Bit const & lhs
		, bool rhs );
	/**@}*/
}

#include "DynamicBitset.inl"

#endif
