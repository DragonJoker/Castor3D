/*
See LICENSE file in root folder
*/
#ifndef ___CU_DynamicBitset_H___
#define ___CU_DynamicBitset_H___

#include "CastorUtils/Design/DesignModule.hpp"

namespace c3d
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
		DynamicBitsetT()noexcept;
		explicit DynamicBitsetT( size_t size, bool value = false );
		explicit DynamicBitsetT( MbStringView bits );
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
		void set( size_t bit, bool value = true )noexcept;
		bool get( size_t bit )const noexcept;
		BlockTypeT getBlock( size_t index )const noexcept;
		void reset()noexcept;
		void resize( size_t size, bool value );
		size_t getSize()const noexcept;
		size_t getBlockCount()const noexcept;
		bool none()const noexcept;
		bool any()const noexcept;
		bool all()const noexcept;
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
		Bit operator[]( size_t index )noexcept;
		bool operator[]( size_t index )const noexcept;
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
		DynamicBitsetT & operator<<=( int value )noexcept;
		DynamicBitsetT & operator>>=( int value )noexcept;
		DynamicBitsetT & operator&=( DynamicBitsetT const & value )noexcept;
		DynamicBitsetT & operator|=( DynamicBitsetT const & value )noexcept;
		DynamicBitsetT & operator^=( DynamicBitsetT const & value )noexcept;
		DynamicBitsetT operator~()const noexcept;
		/**@}*/
		String toString()const;

		using BlockType = BlockTypeT;
		static constexpr BlockTypeT fullBitMask = std::numeric_limits< BlockTypeT >::max();
		static constexpr size_t bitsPerBlock = sizeof( BlockTypeT ) * 8u;

	private:
		void doResetExtraBits()noexcept;
		BlockTypeT doGetLastBlockMask()const noexcept;

	private:
		Vector< BlockTypeT > m_blocks;
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
		, DynamicBitsetT< BlockTypeT > const & rhs )noexcept;
	template< typename BlockTypeT >
	inline bool operator!=( DynamicBitsetT< BlockTypeT > const & lhs
		, DynamicBitsetT< BlockTypeT > const & rhs )noexcept;
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
		, int rhs )noexcept;
	template< typename BlockTypeT >
	inline DynamicBitsetT< BlockTypeT > operator>>( DynamicBitsetT< BlockTypeT > const & lhs
		, int rhs )noexcept;
	template< typename BlockTypeT >
	inline DynamicBitsetT< BlockTypeT > operator&( DynamicBitsetT< BlockTypeT > const & lhs
		, DynamicBitsetT< BlockTypeT > const & rhs )noexcept;
	template< typename BlockTypeT >
	inline DynamicBitsetT< BlockTypeT > operator|( DynamicBitsetT< BlockTypeT > const & lhs
		, DynamicBitsetT< BlockTypeT > const & rhs )noexcept;
	template< typename BlockTypeT >
	inline DynamicBitsetT< BlockTypeT > operator^( DynamicBitsetT< BlockTypeT > const & lhs
		, DynamicBitsetT< BlockTypeT > const & rhs )noexcept;
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
		Bit( Bit const & bit ) = delete;
		Bit( Bit && bit )noexcept = delete;
		Bit( BlockTypeT & block
			, BlockTypeT mask )noexcept
			: m_block{ block }
			, m_mask{ mask }
		{
		}

	public:
		~Bit()noexcept = default;
		Bit & operator=( bool val )noexcept;
		Bit & operator=( Bit const & bit )noexcept;
		Bit & operator=( Bit && bit )noexcept;

		void set( bool val = true )noexcept;

		template< bool BadCall = true >
		Bit * operator&()const noexcept
		{
			static_assert( !BadCall, "Taking the address of a bit in a bitset is impossible." );
			return nullptr;
		}

		explicit operator bool()const noexcept;

		Bit & operator|=( bool value )noexcept;
		Bit & operator&=( bool value )noexcept;
		Bit & operator^=( bool value )noexcept;

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
		, typename DynamicBitsetT< BlockTypeT >::Bit const & rhs )noexcept;
	template< typename BlockTypeT >
	inline bool operator!=( typename DynamicBitsetT< BlockTypeT >::Bit const & lhs
		, typename DynamicBitsetT< BlockTypeT >::Bit const & rhs )noexcept;
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
		, bool rhs )noexcept;
	template< typename BlockTypeT >
	typename DynamicBitsetT< BlockTypeT >::Bit operator&( typename DynamicBitsetT< BlockTypeT >::Bit const & lhs
		, bool rhs )noexcept;
	template< typename BlockTypeT >
	typename DynamicBitsetT< BlockTypeT >::Bit operator^( typename DynamicBitsetT< BlockTypeT >::Bit const & lhs
		, bool rhs )noexcept;
	/**@}*/
}

#include "DynamicBitset.inl"

#endif
