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
	struct ElementTyper< ElementType::Float >
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
	struct ElementTyper< ElementType::Vec2 >
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
	struct ElementTyper< ElementType::Vec3 >
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
	struct ElementTyper< ElementType::Vec4 >
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
	struct ElementTyper< ElementType::Colour >
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
	struct ElementTyper< ElementType::Int >
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
	struct ElementTyper< ElementType::IVec2 >
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
	struct ElementTyper< ElementType::IVec3 >
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
	struct ElementTyper< ElementType::IVec4 >
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
	struct ElementTyper< ElementType::UInt >
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
	struct ElementTyper< ElementType::UIVec2 >
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
	struct ElementTyper< ElementType::UIVec3 >
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
	struct ElementTyper< ElementType::UIVec4 >
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
	struct ElementTyper< ElementType::Mat2 >
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
	struct ElementTyper< ElementType::Mat3 >
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
	struct ElementTyper< ElementType::Mat4 >
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
		case ElementType::Float:
			{
				auto l_value = ParseValue< ElementType::Float >( p_value );
				p_particle.SetValue< ElementType::Float >( p_index, l_value );
			}
			break;

		case ElementType::Vec2:
			{
				auto l_value = ParseValue< ElementType::Vec2 >( p_value );
				p_particle.SetValue< ElementType::Vec2 >( p_index, l_value );
			}
			break;

		case ElementType::Vec3:
			{
				auto l_value = ParseValue< ElementType::Vec3 >( p_value );
				p_particle.SetValue< ElementType::Vec3 >( p_index, l_value );
			}
			break;

		case ElementType::Vec4:
			{
				auto l_value = ParseValue< ElementType::Vec4 >( p_value );
				p_particle.SetValue< ElementType::Vec4 >( p_index, l_value );
			}
			break;

		case ElementType::Colour:
			{
				auto l_value = ParseValue< ElementType::Colour >( p_value );
				p_particle.SetValue< ElementType::Colour >( p_index, l_value );
			}
			break;

		case ElementType::Int:
			{
				auto l_value = ParseValue< ElementType::Int >( p_value );
				p_particle.SetValue< ElementType::Int >( p_index, l_value );
			}
			break;

		case ElementType::IVec2:
			{
				auto l_value = ParseValue< ElementType::IVec2 >( p_value );
				p_particle.SetValue< ElementType::IVec2 >( p_index, l_value );
			}
			break;

		case ElementType::IVec3:
			{
				auto l_value = ParseValue< ElementType::IVec3 >( p_value );
				p_particle.SetValue< ElementType::IVec3 >( p_index, l_value );
			}
			break;

		case ElementType::IVec4:
			{
				auto l_value = ParseValue< ElementType::IVec4 >( p_value );
				p_particle.SetValue< ElementType::IVec4 >( p_index, l_value );
			}
			break;

		case ElementType::UInt:
			{
				auto l_value = ParseValue< ElementType::UInt >( p_value );
				p_particle.SetValue< ElementType::UInt >( p_index, l_value );
			}
			break;

		case ElementType::UIVec2:
			{
				auto l_value = ParseValue< ElementType::UIVec2 >( p_value );
				p_particle.SetValue< ElementType::UIVec2 >( p_index, l_value );
			}
			break;

		case ElementType::UIVec3:
			{
				auto l_value = ParseValue< ElementType::UIVec3 >( p_value );
				p_particle.SetValue< ElementType::UIVec3 >( p_index, l_value );
			}
			break;

		case ElementType::UIVec4:
			{
				auto l_value = ParseValue< ElementType::UIVec4 >( p_value );
				p_particle.SetValue< ElementType::UIVec4 >( p_index, l_value );
			}
			break;

		case ElementType::Mat2:
		{
			auto l_value = ParseValue< ElementType::Mat2 > (p_value);
			p_particle.SetValue< ElementType::Mat2 > (p_index, l_value);
		}
		break;

		case ElementType::Mat3:
		{
			auto l_value = ParseValue< ElementType::Mat3 > (p_value);
			p_particle.SetValue< ElementType::Mat3 > (p_index, l_value);
		}
		break;

		case ElementType::Mat4:
			{
				auto l_value = ParseValue< ElementType::Mat4 >( p_value );
				p_particle.SetValue< ElementType::Mat4 >( p_index, l_value );
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
