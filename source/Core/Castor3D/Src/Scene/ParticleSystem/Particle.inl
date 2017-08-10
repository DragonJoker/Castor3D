namespace castor3d
{
	template< typename T, size_t Count >
	void ParseArray( castor::String const & p_src, T * p_dst )
	{
		auto array = castor::string::split( p_src, cuT( " \t" ), Count - 1, false );

		if ( array.size() >= Count )
		{
			for ( size_t i = 0; i < Count; ++i )
			{
				castor::string::parse( array[i], p_dst[i] );
			}
		}
	}

	template<>
	struct ElementTyper< ElementType::eFloat >
	{
		using Type = float;

		static inline Type parse( castor::String const & p_value )
		{
			Type result;
			castor::string::parse( p_value, result );
			return result;
		}

		static inline float const * getPointer( Type const & p_value )
		{
			return &p_value;
		}

		static inline float * getPointer( Type & p_value )
		{
			return &p_value;
		}
	};

	template<>
	struct ElementTyper< ElementType::eVec2 >
	{
		using Type = castor::Point2f;

		static inline Type parse( castor::String const & p_value )
		{
			Type result;
			ParseArray< float, 2 >( p_value, result.ptr() );
			return result;
		}

		static inline float const * getPointer( Type const & p_value )
		{
			return p_value.constPtr();
		}

		static inline float * getPointer( Type & p_value )
		{
			return p_value.ptr();
		}
	};

	template<>
	struct ElementTyper< ElementType::eVec3 >
	{
		using Type = castor::Point3f;

		static inline Type parse( castor::String const & p_value )
		{
			Type result;
			ParseArray< float, 3 >( p_value, result.ptr() );
			return result;
		}

		static inline float const * getPointer( Type const & p_value )
		{
			return p_value.constPtr();
		}

		static inline float * getPointer( Type & p_value )
		{
			return p_value.ptr();
		}
	};

	template<>
	struct ElementTyper< ElementType::eVec4 >
	{
		using Type = castor::Point4f;

		static inline Type parse( castor::String const & p_value )
		{
			Type result;
			ParseArray< float, 4 >( p_value, result.ptr() );
			return result;
		}

		static inline float const * getPointer( Type const & p_value )
		{
			return p_value.constPtr();
		}

		static inline float * getPointer( Type & p_value )
		{
			return p_value.ptr();
		}
	};

	template<>
	struct ElementTyper< ElementType::eColour >
	{
		using Type = uint32_t;

		static inline Type parse( castor::String const & p_value )
		{
			Type result{};
			return result;
		}

		static inline uint32_t const * getPointer( Type const & p_value )
		{
			return &p_value;
		}

		static inline uint32_t * getPointer( Type & p_value )
		{
			return &p_value;
		}
	};

	template<>
	struct ElementTyper< ElementType::eInt >
	{
		using Type = int;

		static inline Type parse( castor::String const & p_value )
		{
			Type result;
			castor::string::parse( p_value, result );
			return result;
		}

		static inline int const * getPointer( Type const & p_value )
		{
			return &p_value;
		}

		static inline int * getPointer( Type & p_value )
		{
			return &p_value;
		}
	};

	template<>
	struct ElementTyper< ElementType::eIVec2 >
	{
		using Type = castor::Point2i;

		static inline Type parse( castor::String const & p_value )
		{
			Type result;
			ParseArray< int, 2 >( p_value, result.ptr() );
			return result;
		}

		static inline int const * getPointer( Type const & p_value )
		{
			return p_value.constPtr();
		}

		static inline int * getPointer( Type & p_value )
		{
			return p_value.ptr();
		}
	};

	template<>
	struct ElementTyper< ElementType::eIVec3 >
	{
		using Type = castor::Point3i;

		static inline Type parse( castor::String const & p_value )
		{
			Type result;
			ParseArray< int, 3 >( p_value, result.ptr() );
			return result;
		}

		static inline int const * getPointer( Type const & p_value )
		{
			return p_value.constPtr();
		}

		static inline int * getPointer( Type & p_value )
		{
			return p_value.ptr();
		}
	};

	template<>
	struct ElementTyper< ElementType::eIVec4 >
	{
		using Type = castor::Point4i;

		static inline Type parse( castor::String const & p_value )
		{
			Type result;
			ParseArray< int, 4 >( p_value, result.ptr() );
			return result;
		}

		static inline int const * getPointer( Type const & p_value )
		{
			return p_value.constPtr();
		}

		static inline int * getPointer( Type & p_value )
		{
			return p_value.ptr();
		}
	};

	template<>
	struct ElementTyper< ElementType::eUInt >
	{
		using Type = uint32_t;

		static inline Type parse( castor::String const & p_value )
		{
			Type result;
			castor::string::parse( p_value, result );
			return result;
		}

		static inline uint32_t const * getPointer( Type const & p_value )
		{
			return &p_value;
		}

		static inline uint32_t * getPointer( Type & p_value )
		{
			return &p_value;
		}
	};

	template<>
	struct ElementTyper< ElementType::eUIVec2 >
	{
		using Type = castor::Point2ui;

		static inline Type parse( castor::String const & p_value )
		{
			Type result;
			ParseArray< uint32_t, 2 >( p_value, result.ptr() );
			return result;
		}

		static inline uint32_t const * getPointer( Type const & p_value )
		{
			return p_value.constPtr();
		}

		static inline uint32_t * getPointer( Type & p_value )
		{
			return p_value.ptr();
		}
	};

	template<>
	struct ElementTyper< ElementType::eUIVec3 >
	{
		using Type = castor::Point3ui;

		static inline Type parse( castor::String const & p_value )
		{
			Type result;
			ParseArray< uint32_t, 3 >( p_value, result.ptr() );
			return result;
		}

		static inline uint32_t const * getPointer( Type const & p_value )
		{
			return p_value.constPtr();
		}

		static inline uint32_t * getPointer( Type & p_value )
		{
			return p_value.ptr();
		}
	};

	template<>
	struct ElementTyper< ElementType::eUIVec4 >
	{
		using Type = castor::Point4ui;

		static inline Type parse( castor::String const & p_value )
		{
			Type result;
			ParseArray< uint32_t, 4 >( p_value, result.ptr() );
			return result;
		}

		static inline uint32_t const * getPointer( Type const & p_value )
		{
			return p_value.constPtr();
		}

		static inline uint32_t * getPointer( Type & p_value )
		{
			return p_value.ptr();
		}
	};

	template<>
	struct ElementTyper< ElementType::eMat2 >
	{
		using Type = castor::Matrix2x2f;

		static inline Type parse( castor::String const & p_value )
		{
			Type result;
			ParseArray< float, 4 >( p_value, result.ptr() );
			return result;
		}

		static inline float const * getPointer( Type const & p_value )
		{
			return p_value.constPtr();
		}

		static inline float * getPointer( Type & p_value )
		{
			return p_value.ptr();
		}
	};

	template<>
	struct ElementTyper< ElementType::eMat3 >
	{
		using Type = castor::Matrix3x3f;

		static inline Type parse( castor::String const & p_value )
		{
			Type result;
			ParseArray< float, 9 >( p_value, result.ptr() );
			return result;
		}

		static inline float const * getPointer( Type const & p_value )
		{
			return p_value.constPtr();
		}

		static inline float * getPointer( Type & p_value )
		{
			return p_value.ptr();
		}
	};

	template<>
	struct ElementTyper< ElementType::eMat4 >
	{
		using Type = castor::Matrix4x4f;

		static inline Type parse( castor::String const & p_value )
		{
			Type result;
			ParseArray< float, 16 >( p_value, result.ptr() );
			return result;
		}

		static inline float const * getPointer( Type const & p_value )
		{
			return p_value.constPtr();
		}

		static inline float * getPointer( Type & p_value )
		{
			return p_value.ptr();
		}
	};

	template< ElementType Type >
	inline typename ElementTyper< Type >::Type parseValue( castor::String const & p_value )
	{
		return ElementTyper< Type >::parse( p_value );
	}

	inline void parseValue( castor::String const & p_value, ElementType p_type, Particle & p_particle, uint32_t p_index )
	{
		switch ( p_type )
		{
		case ElementType::eFloat:
			{
				auto value = parseValue< ElementType::eFloat >( p_value );
				p_particle.setValue< ElementType::eFloat >( p_index, value );
			}
			break;

		case ElementType::eVec2:
			{
				auto value = parseValue< ElementType::eVec2 >( p_value );
				p_particle.setValue< ElementType::eVec2 >( p_index, value );
			}
			break;

		case ElementType::eVec3:
			{
				auto value = parseValue< ElementType::eVec3 >( p_value );
				p_particle.setValue< ElementType::eVec3 >( p_index, value );
			}
			break;

		case ElementType::eVec4:
			{
				auto value = parseValue< ElementType::eVec4 >( p_value );
				p_particle.setValue< ElementType::eVec4 >( p_index, value );
			}
			break;

		case ElementType::eColour:
			{
				auto value = parseValue< ElementType::eColour >( p_value );
				p_particle.setValue< ElementType::eColour >( p_index, value );
			}
			break;

		case ElementType::eInt:
			{
				auto value = parseValue< ElementType::eInt >( p_value );
				p_particle.setValue< ElementType::eInt >( p_index, value );
			}
			break;

		case ElementType::eIVec2:
			{
				auto value = parseValue< ElementType::eIVec2 >( p_value );
				p_particle.setValue< ElementType::eIVec2 >( p_index, value );
			}
			break;

		case ElementType::eIVec3:
			{
				auto value = parseValue< ElementType::eIVec3 >( p_value );
				p_particle.setValue< ElementType::eIVec3 >( p_index, value );
			}
			break;

		case ElementType::eIVec4:
			{
				auto value = parseValue< ElementType::eIVec4 >( p_value );
				p_particle.setValue< ElementType::eIVec4 >( p_index, value );
			}
			break;

		case ElementType::eUInt:
			{
				auto value = parseValue< ElementType::eUInt >( p_value );
				p_particle.setValue< ElementType::eUInt >( p_index, value );
			}
			break;

		case ElementType::eUIVec2:
			{
				auto value = parseValue< ElementType::eUIVec2 >( p_value );
				p_particle.setValue< ElementType::eUIVec2 >( p_index, value );
			}
			break;

		case ElementType::eUIVec3:
			{
				auto value = parseValue< ElementType::eUIVec3 >( p_value );
				p_particle.setValue< ElementType::eUIVec3 >( p_index, value );
			}
			break;

		case ElementType::eUIVec4:
			{
				auto value = parseValue< ElementType::eUIVec4 >( p_value );
				p_particle.setValue< ElementType::eUIVec4 >( p_index, value );
			}
			break;

		case ElementType::eMat2:
		{
			auto value = parseValue< ElementType::eMat2 > (p_value);
			p_particle.setValue< ElementType::eMat2 > (p_index, value);
		}
		break;

		case ElementType::eMat3:
		{
			auto value = parseValue< ElementType::eMat3 > (p_value);
			p_particle.setValue< ElementType::eMat3 > (p_index, value);
		}
		break;

		case ElementType::eMat4:
			{
				auto value = parseValue< ElementType::eMat4 >( p_value );
				p_particle.setValue< ElementType::eMat4 >( p_index, value );
			}
			break;
		}
	}

	template< ElementType Type >
	inline void Particle::setValue( uint32_t p_index, typename ElementTyper< Type >::Type const & p_value )
	{
		REQUIRE( p_index < m_description.size() );
		auto it = m_description.begin() + p_index;
		REQUIRE( it->m_dataType == Type );
		std::memcpy( &m_data[it->m_offset], ElementTyper< Type >::getPointer( p_value ), sizeof( p_value ) );
	}

	template< ElementType Type >
	inline typename ElementTyper< Type >::Type Particle::getValue( uint32_t p_index )const
	{
		REQUIRE( p_index < m_description.size() );
		auto it = m_description.begin() + p_index;
		REQUIRE( it->m_dataType == Type );
		typename ElementTyper< Type >::Type result{};
		std::memcpy( ElementTyper< Type >::getPointer( result ), &m_data[it->m_offset], sizeof( result ) );
		return result;
	}
}
