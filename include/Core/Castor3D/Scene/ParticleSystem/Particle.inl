#include "Particle.hpp"

#include <CastorUtils/Math/SquareMatrix.hpp>
#include <CastorUtils/Math/Point.hpp>

namespace castor3d
{
	template< typename T, size_t Count >
	void ParseArray( castor::String const & src, T * dst )
	{
		auto array = castor::string::split( src, cuT( " \t" ), Count - 1, false );

		if ( array.size() >= Count )
		{
			for ( size_t i = 0; i < Count; ++i )
			{
				castor::string::parse( array[i], dst[i] );
			}
		}
	}

	template<>
	struct ElementTyper< ParticleFormat::eFloat >
	{
		using Type = float;

		static inline Type parse( castor::String const & value )
		{
			Type result;
			castor::string::parse( value, result );
			return result;
		}

		static inline float const * getPointer( Type const & value )
		{
			return &value;
		}

		static inline float * getPointer( Type & value )
		{
			return &value;
		}
	};

	template<>
	struct ElementTyper< ParticleFormat::eVec2f >
	{
		using Type = castor::Point2f;

		static inline Type parse( castor::String const & value )
		{
			Type result;
			ParseArray< float, 2 >( value, result.ptr() );
			return result;
		}

		static inline float const * getPointer( Type const & value )
		{
			return value.constPtr();
		}

		static inline float * getPointer( Type & value )
		{
			return value.ptr();
		}
	};

	template<>
	struct ElementTyper< ParticleFormat::eVec3f >
	{
		using Type = castor::Point3f;

		static inline Type parse( castor::String const & value )
		{
			Type result;
			ParseArray< float, 3 >( value, result.ptr() );
			return result;
		}

		static inline float const * getPointer( Type const & value )
		{
			return value.constPtr();
		}

		static inline float * getPointer( Type & value )
		{
			return value.ptr();
		}
	};

	template<>
	struct ElementTyper< ParticleFormat::eVec4f >
	{
		using Type = castor::Point4f;

		static inline Type parse( castor::String const & value )
		{
			Type result;
			ParseArray< float, 4 >( value, result.ptr() );
			return result;
		}

		static inline float const * getPointer( Type const & value )
		{
			return value.constPtr();
		}

		static inline float * getPointer( Type & value )
		{
			return value.ptr();
		}
	};

	template<>
	struct ElementTyper< ParticleFormat::eInt >
	{
		using Type = int;

		static inline Type parse( castor::String const & value )
		{
			Type result;
			castor::string::parse( value, result );
			return result;
		}

		static inline int const * getPointer( Type const & value )
		{
			return &value;
		}

		static inline int * getPointer( Type & value )
		{
			return &value;
		}
	};

	template<>
	struct ElementTyper< ParticleFormat::eVec2i >
	{
		using Type = castor::Point2i;

		static inline Type parse( castor::String const & value )
		{
			Type result;
			ParseArray< int, 2 >( value, result.ptr() );
			return result;
		}

		static inline int const * getPointer( Type const & value )
		{
			return value.constPtr();
		}

		static inline int * getPointer( Type & value )
		{
			return value.ptr();
		}
	};

	template<>
	struct ElementTyper< ParticleFormat::eVec3i >
	{
		using Type = castor::Point3i;

		static inline Type parse( castor::String const & value )
		{
			Type result;
			ParseArray< int, 3 >( value, result.ptr() );
			return result;
		}

		static inline int const * getPointer( Type const & value )
		{
			return value.constPtr();
		}

		static inline int * getPointer( Type & value )
		{
			return value.ptr();
		}
	};

	template<>
	struct ElementTyper< ParticleFormat::eVec4i >
	{
		using Type = castor::Point4i;

		static inline Type parse( castor::String const & value )
		{
			Type result;
			ParseArray< int, 4 >( value, result.ptr() );
			return result;
		}

		static inline int const * getPointer( Type const & value )
		{
			return value.constPtr();
		}

		static inline int * getPointer( Type & value )
		{
			return value.ptr();
		}
	};

	template<>
	struct ElementTyper< ParticleFormat::eUInt >
	{
		using Type = uint32_t;

		static inline Type parse( castor::String const & value )
		{
			Type result;
			castor::string::parse( value, result );
			return result;
		}

		static inline uint32_t const * getPointer( Type const & value )
		{
			return &value;
		}

		static inline uint32_t * getPointer( Type & value )
		{
			return &value;
		}
	};

	template<>
	struct ElementTyper< ParticleFormat::eVec2ui >
	{
		using Type = castor::Point2ui;

		static inline Type parse( castor::String const & value )
		{
			Type result;
			ParseArray< uint32_t, 2 >( value, result.ptr() );
			return result;
		}

		static inline uint32_t const * getPointer( Type const & value )
		{
			return value.constPtr();
		}

		static inline uint32_t * getPointer( Type & value )
		{
			return value.ptr();
		}
	};

	template<>
	struct ElementTyper< ParticleFormat::eVec3ui >
	{
		using Type = castor::Point3ui;

		static inline Type parse( castor::String const & value )
		{
			Type result;
			ParseArray< uint32_t, 3 >( value, result.ptr() );
			return result;
		}

		static inline uint32_t const * getPointer( Type const & value )
		{
			return value.constPtr();
		}

		static inline uint32_t * getPointer( Type & value )
		{
			return value.ptr();
		}
	};

	template<>
	struct ElementTyper< ParticleFormat::eVec4ui >
	{
		using Type = castor::Point4ui;

		static inline Type parse( castor::String const & value )
		{
			Type result;
			ParseArray< uint32_t, 4 >( value, result.ptr() );
			return result;
		}

		static inline uint32_t const * getPointer( Type const & value )
		{
			return value.constPtr();
		}

		static inline uint32_t * getPointer( Type & value )
		{
			return value.ptr();
		}
	};

	template<>
	struct ElementTyper< ParticleFormat::eMat2f >
	{
		using Type = castor::Matrix2x2f;

		static inline Type parse( castor::String const & value )
		{
			Type result;
			ParseArray< float, 4 >( value, result.ptr() );
			return result;
		}

		static inline float const * getPointer( Type const & value )
		{
			return value.constPtr();
		}

		static inline float * getPointer( Type & value )
		{
			return value.ptr();
		}
	};

	template<>
	struct ElementTyper< ParticleFormat::eMat3f >
	{
		using Type = castor::Matrix3x3f;

		static inline Type parse( castor::String const & value )
		{
			Type result;
			ParseArray< float, 9 >( value, result.ptr() );
			return result;
		}

		static inline float const * getPointer( Type const & value )
		{
			return value.constPtr();
		}

		static inline float * getPointer( Type & value )
		{
			return value.ptr();
		}
	};

	template<>
	struct ElementTyper< ParticleFormat::eMat4f >
	{
		using Type = castor::Matrix4x4f;

		static inline Type parse( castor::String const & value )
		{
			Type result;
			ParseArray< float, 16 >( value, result.ptr() );
			return result;
		}

		static inline float const * getPointer( Type const & value )
		{
			return value.constPtr();
		}

		static inline float * getPointer( Type & value )
		{
			return value.ptr();
		}
	};

	template< ParticleFormat TypeT >
	using ParticleElementTypeT = typename ElementTyper< TypeT >::Type;

	template< ParticleFormat Type >
	inline ParticleElementTypeT< Type > parseValue( castor::String const & value )
	{
		return ElementTyper< Type >::parse( value );
	}

	inline void parseValue( castor::String const & textValue
		, ParticleFormat type
		, Particle & particle
		, uint32_t index )
	{
		switch ( type )
		{
		case ParticleFormat::eFloat:
			{
				auto value = parseValue< ParticleFormat::eFloat >( textValue );
				particle.setValue< ParticleFormat::eFloat >( index, value );
			}
			break;

		case ParticleFormat::eVec2f:
			{
				auto value = parseValue< ParticleFormat::eVec2f >( textValue );
				particle.setValue< ParticleFormat::eVec2f >( index, value );
			}
			break;

		case ParticleFormat::eVec3f:
			{
				auto value = parseValue< ParticleFormat::eVec3f >( textValue );
				particle.setValue< ParticleFormat::eVec3f >( index, value );
			}
			break;

		case ParticleFormat::eVec4f:
			{
				auto value = parseValue< ParticleFormat::eVec4f >( textValue );
				particle.setValue< ParticleFormat::eVec4f >( index, value );
			}
			break;

		case ParticleFormat::eInt:
			{
				auto value = parseValue< ParticleFormat::eInt >( textValue );
				particle.setValue< ParticleFormat::eInt >( index, value );
			}
			break;

		case ParticleFormat::eVec2i:
			{
				auto value = parseValue< ParticleFormat::eVec2i >( textValue );
				particle.setValue< ParticleFormat::eVec2i >( index, value );
			}
			break;

		case ParticleFormat::eVec3i:
			{
				auto value = parseValue< ParticleFormat::eVec3i >( textValue );
				particle.setValue< ParticleFormat::eVec3i >( index, value );
			}
			break;

		case ParticleFormat::eVec4i:
			{
				auto value = parseValue< ParticleFormat::eVec4i >( textValue );
				particle.setValue< ParticleFormat::eVec4i >( index, value );
			}
			break;

		case ParticleFormat::eUInt:
			{
				auto value = parseValue< ParticleFormat::eUInt >( textValue );
				particle.setValue< ParticleFormat::eUInt >( index, value );
			}
			break;

		case ParticleFormat::eVec2ui:
			{
				auto value = parseValue< ParticleFormat::eVec2ui >( textValue );
				particle.setValue< ParticleFormat::eVec2ui >( index, value );
			}
			break;

		case ParticleFormat::eVec3ui:
			{
				auto value = parseValue< ParticleFormat::eVec3ui >( textValue );
				particle.setValue< ParticleFormat::eVec3ui >( index, value );
			}
			break;

		case ParticleFormat::eVec4ui:
			{
				auto value = parseValue< ParticleFormat::eVec4ui >( textValue );
				particle.setValue< ParticleFormat::eVec4ui >( index, value );
			}
			break;

		case ParticleFormat::eMat2f:
			{
				auto value = parseValue< ParticleFormat::eMat2f >( textValue );
				particle.setValue< ParticleFormat::eMat2f >( index, value );
			}
			break;

		case ParticleFormat::eMat3f:
			{
				auto value = parseValue< ParticleFormat::eMat3f >( textValue );
				particle.setValue< ParticleFormat::eMat3f >( index, value );
			}
			break;

		case ParticleFormat::eMat4f:
			{
				auto value = parseValue< ParticleFormat::eMat4f >( textValue );
				particle.setValue< ParticleFormat::eMat4f >( index, value );
			}
			break;

		default:
			break;
		}
	}

	template< ParticleFormat Type >
	inline void Particle::setValue( uint32_t index, ParticleElementTypeT< Type > const & value )
	{
		CU_Require( index < m_description.count() );
		auto it = m_description.begin() + index;
		CU_Require( it->m_dataType == Type );
		std::memcpy( &m_data[it->m_offset], ElementTyper< Type >::getPointer( value ), sizeof( value ) );
	}

	template< ParticleFormat Type >
	inline ParticleElementTypeT< Type > Particle::getValue( uint32_t index )const
	{
		CU_Require( index < m_description.count() );
		auto it = m_description.begin() + index;
		CU_Require( it->m_dataType == Type );
		ParticleElementTypeT< Type > result{};
		std::memcpy( ElementTyper< Type >::getPointer( result ), &m_data[it->m_offset], sizeof( result ) );
		return result;
	}
}
