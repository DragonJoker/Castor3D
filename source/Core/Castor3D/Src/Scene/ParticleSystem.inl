#include "ParticleSystem.hpp"

#include <Design/ArrayView.hpp>

namespace Castor3D
{
	template< typename T, size_t Count >
	void ParseArray( Castor::String const & p_src, T * p_dst )
	{
		auto l_array = Castor::string::split( p_src, cuT( " \t" ), Count - 1, false );

		if ( l_array.size() >= Count )
		{
			for ( size_t i = 0; i < Count; ++i )
			{
				Castor::string::parse( l_array[i], p_dst[i] );
			}
		}
	}

	template<>
	struct ElementTyper< ElementType::eFloat >
	{
		using Type = float;

		static inline Type Parse( Castor::String const & p_value )
		{
			Type l_return;
			Castor::string::parse( p_value, l_return );
			return l_return;
		}

		static inline float const * GetPointer( Type const & p_value )
		{
			return &p_value;
		}
	};

	template<>
	struct ElementTyper< ElementType::eVec2 >
	{
		using Type = Castor::Point2f;

		static inline Type Parse( Castor::String const & p_value )
		{
			Type l_return;
			ParseArray< float, 2 >( p_value, l_return.ptr() );
			return l_return;
		}

		static inline float const * GetPointer( Type const & p_value )
		{
			return p_value.const_ptr();
		}
	};

	template<>
	struct ElementTyper< ElementType::eVec3 >
	{
		using Type = Castor::Point3f;

		static inline Type Parse( Castor::String const & p_value )
		{
			Type l_return;
			ParseArray< float, 3 >( p_value, l_return.ptr() );
			return l_return;
		}

		static inline float const * GetPointer( Type const & p_value )
		{
			return p_value.const_ptr();
		}
	};

	template<>
	struct ElementTyper< ElementType::eVec4 >
	{
		using Type = Castor::Point4f;

		static inline Type Parse( Castor::String const & p_value )
		{
			Type l_return;
			ParseArray< float, 4 >( p_value, l_return.ptr() );
			return l_return;
		}

		static inline float const * GetPointer( Type const & p_value )
		{
			return p_value.const_ptr();
		}
	};

	template<>
	struct ElementTyper< ElementType::eColour >
	{
		using Type = uint32_t;

		static inline Type Parse( Castor::String const & p_value )
		{
			Type l_return{};
			return l_return;
		}

		static inline uint32_t const * GetPointer( Type const & p_value )
		{
			return &p_value;
		}
	};

	template<>
	struct ElementTyper< ElementType::eInt >
	{
		using Type = int;

		static inline Type Parse( Castor::String const & p_value )
		{
			Type l_return;
			Castor::string::parse( p_value, l_return );
			return l_return;
		}

		static inline int const * GetPointer( Type const & p_value )
		{
			return &p_value;
		}
	};

	template<>
	struct ElementTyper< ElementType::eIVec2 >
	{
		using Type = Castor::Point2i;

		static inline Type Parse( Castor::String const & p_value )
		{
			Type l_return;
			ParseArray< int, 2 >( p_value, l_return.ptr() );
			return l_return;
		}

		static inline int const * GetPointer( Type const & p_value )
		{
			return p_value.const_ptr();
		}
	};

	template<>
	struct ElementTyper< ElementType::eIVec3 >
	{
		using Type = Castor::Point3i;

		static inline Type Parse( Castor::String const & p_value )
		{
			Type l_return;
			ParseArray< int, 3 >( p_value, l_return.ptr() );
			return l_return;
		}

		static inline int const * GetPointer( Type const & p_value )
		{
			return p_value.const_ptr();
		}
	};

	template<>
	struct ElementTyper< ElementType::eIVec4 >
	{
		using Type = Castor::Point4i;

		static inline Type Parse( Castor::String const & p_value )
		{
			Type l_return;
			ParseArray< int, 4 >( p_value, l_return.ptr() );
			return l_return;
		}

		static inline int const * GetPointer( Type const & p_value )
		{
			return p_value.const_ptr();
		}
	};

	template<>
	struct ElementTyper< ElementType::eUInt >
	{
		using Type = uint32_t;

		static inline Type Parse( Castor::String const & p_value )
		{
			Type l_return;
			Castor::string::parse( p_value, l_return );
			return l_return;
		}

		static inline uint32_t const * GetPointer( Type const & p_value )
		{
			return &p_value;
		}
	};

	template<>
	struct ElementTyper< ElementType::eUIVec2 >
	{
		using Type = Castor::Point2ui;

		static inline Type Parse( Castor::String const & p_value )
		{
			Type l_return;
			ParseArray< uint32_t, 2 >( p_value, l_return.ptr() );
			return l_return;
		}

		static inline uint32_t const * GetPointer( Type const & p_value )
		{
			return p_value.const_ptr();
		}
	};

	template<>
	struct ElementTyper< ElementType::eUIVec3 >
	{
		using Type = Castor::Point3ui;

		static inline Type Parse( Castor::String const & p_value )
		{
			Type l_return;
			ParseArray< uint32_t, 3 >( p_value, l_return.ptr() );
			return l_return;
		}

		static inline uint32_t const * GetPointer( Type const & p_value )
		{
			return p_value.const_ptr();
		}
	};

	template<>
	struct ElementTyper< ElementType::eUIVec4 >
	{
		using Type = Castor::Point4ui;

		static inline Type Parse( Castor::String const & p_value )
		{
			Type l_return;
			ParseArray< uint32_t, 4 >( p_value, l_return.ptr() );
			return l_return;
		}

		static inline uint32_t const * GetPointer( Type const & p_value )
		{
			return p_value.const_ptr();
		}
	};

	template<>
	struct ElementTyper< ElementType::eMat2 >
	{
		using Type = Castor::Matrix2x2f;

		static inline Type Parse( Castor::String const & p_value )
		{
			Type l_return;
			ParseArray< float, 4 >( p_value, l_return.ptr() );
			return l_return;
		}

		static inline float const * GetPointer( Type const & p_value )
		{
			return p_value.const_ptr();
		}
	};

	template<>
	struct ElementTyper< ElementType::eMat3 >
	{
		using Type = Castor::Matrix3x3f;

		static inline Type Parse( Castor::String const & p_value )
		{
			Type l_return;
			ParseArray< float, 9 >( p_value, l_return.ptr() );
			return l_return;
		}

		static inline float const * GetPointer( Type const & p_value )
		{
			return p_value.const_ptr();
		}
	};

	template<>
	struct ElementTyper< ElementType::eMat4 >
	{
		using Type = Castor::Matrix4x4f;

		static inline Type Parse( Castor::String const & p_value )
		{
			Type l_return;
			ParseArray< float, 16 >( p_value, l_return.ptr() );
			return l_return;
		}

		static inline float const * GetPointer( Type const & p_value )
		{
			return p_value.const_ptr();
		}
	};

	template< ElementType Type >
	inline typename ElementTyper< Type >::Type ParseValue( Castor::String const & p_value )
	{
		return ElementTyper< Type >::Parse( p_value );
	}

	inline void ParseValue( Castor::String const & p_value, ElementType p_type, Particle & p_particle, uint32_t p_index )
	{
		switch ( p_type )
		{
		case ElementType::eFloat:
			{
				auto l_value = ParseValue< ElementType::eFloat >( p_value );
				p_particle.SetValue< ElementType::eFloat >( p_index, l_value );
			}
			break;

		case ElementType::eVec2:
			{
				auto l_value = ParseValue< ElementType::eVec2 >( p_value );
				p_particle.SetValue< ElementType::eVec2 >( p_index, l_value );
			}
			break;

		case ElementType::eVec3:
			{
				auto l_value = ParseValue< ElementType::eVec3 >( p_value );
				p_particle.SetValue< ElementType::eVec3 >( p_index, l_value );
			}
			break;

		case ElementType::eVec4:
			{
				auto l_value = ParseValue< ElementType::eVec4 >( p_value );
				p_particle.SetValue< ElementType::eVec4 >( p_index, l_value );
			}
			break;

		case ElementType::eColour:
			{
				auto l_value = ParseValue< ElementType::eColour >( p_value );
				p_particle.SetValue< ElementType::eColour >( p_index, l_value );
			}
			break;

		case ElementType::eInt:
			{
				auto l_value = ParseValue< ElementType::eInt >( p_value );
				p_particle.SetValue< ElementType::eInt >( p_index, l_value );
			}
			break;

		case ElementType::eIVec2:
			{
				auto l_value = ParseValue< ElementType::eIVec2 >( p_value );
				p_particle.SetValue< ElementType::eIVec2 >( p_index, l_value );
			}
			break;

		case ElementType::eIVec3:
			{
				auto l_value = ParseValue< ElementType::eIVec3 >( p_value );
				p_particle.SetValue< ElementType::eIVec3 >( p_index, l_value );
			}
			break;

		case ElementType::eIVec4:
			{
				auto l_value = ParseValue< ElementType::eIVec4 >( p_value );
				p_particle.SetValue< ElementType::eIVec4 >( p_index, l_value );
			}
			break;

		case ElementType::eUInt:
			{
				auto l_value = ParseValue< ElementType::eUInt >( p_value );
				p_particle.SetValue< ElementType::eUInt >( p_index, l_value );
			}
			break;

		case ElementType::eUIVec2:
			{
				auto l_value = ParseValue< ElementType::eUIVec2 >( p_value );
				p_particle.SetValue< ElementType::eUIVec2 >( p_index, l_value );
			}
			break;

		case ElementType::eUIVec3:
			{
				auto l_value = ParseValue< ElementType::eUIVec3 >( p_value );
				p_particle.SetValue< ElementType::eUIVec3 >( p_index, l_value );
			}
			break;

		case ElementType::eUIVec4:
			{
				auto l_value = ParseValue< ElementType::eUIVec4 >( p_value );
				p_particle.SetValue< ElementType::eUIVec4 >( p_index, l_value );
			}
			break;

		case ElementType::eMat2:
		{
			auto l_value = ParseValue< ElementType::eMat2 > (p_value);
			p_particle.SetValue< ElementType::eMat2 > (p_index, l_value);
		}
		break;

		case ElementType::eMat3:
		{
			auto l_value = ParseValue< ElementType::eMat3 > (p_value);
			p_particle.SetValue< ElementType::eMat3 > (p_index, l_value);
		}
		break;

		case ElementType::eMat4:
			{
				auto l_value = ParseValue< ElementType::eMat4 >( p_value );
				p_particle.SetValue< ElementType::eMat4 >( p_index, l_value );
			}
			break;
		}
	}

	template< ElementType Type >
	inline void Particle::SetValue( uint32_t p_index, typename ElementTyper< Type >::Type const & p_value )
	{
		REQUIRE( p_index < m_description.size() );
		auto l_it = m_description.begin() + p_index;
		REQUIRE( l_it->m_dataType == Type );
		std::memcpy( &m_data[l_it->m_offset], ElementTyper< Type >::GetPointer( p_value ), sizeof( p_value ) );
	}
}
