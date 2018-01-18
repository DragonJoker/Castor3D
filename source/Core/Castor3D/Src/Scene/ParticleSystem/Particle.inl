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
	struct ElementTyper< renderer::AttributeFormat::eFloat >
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
	struct ElementTyper< renderer::AttributeFormat::eVec2 >
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
	struct ElementTyper< renderer::AttributeFormat::eVec3 >
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
	struct ElementTyper< renderer::AttributeFormat::eVec4 >
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
	struct ElementTyper< renderer::AttributeFormat::eColour >
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
	struct ElementTyper< renderer::AttributeFormat::eInt >
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
	struct ElementTyper< renderer::AttributeFormat::eIVec2 >
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
	struct ElementTyper< renderer::AttributeFormat::eIVec3 >
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
	struct ElementTyper< renderer::AttributeFormat::eIVec4 >
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
	struct ElementTyper< renderer::AttributeFormat::eUInt >
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
	struct ElementTyper< renderer::AttributeFormat::eUIVec2 >
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
	struct ElementTyper< renderer::AttributeFormat::eUIVec3 >
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
	struct ElementTyper< renderer::AttributeFormat::eUIVec4 >
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
	struct ElementTyper< renderer::AttributeFormat::eMat2 >
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
	struct ElementTyper< renderer::AttributeFormat::eMat3 >
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
	struct ElementTyper< renderer::AttributeFormat::eMat4 >
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

	template< renderer::AttributeFormat Type >
	inline typename ElementTyper< Type >::Type parseValue( castor::String const & p_value )
	{
		return ElementTyper< Type >::parse( p_value );
	}

	inline void parseValue( castor::String const & p_value, renderer::AttributeFormat p_type, Particle & p_particle, uint32_t p_index )
	{
		switch ( p_type )
		{
		case renderer::AttributeFormat::eFloat:
			{
				auto value = parseValue< renderer::AttributeFormat::eFloat >( p_value );
				p_particle.setValue< renderer::AttributeFormat::eFloat >( p_index, value );
			}
			break;

		case renderer::AttributeFormat::eVec2:
			{
				auto value = parseValue< renderer::AttributeFormat::eVec2 >( p_value );
				p_particle.setValue< renderer::AttributeFormat::eVec2 >( p_index, value );
			}
			break;

		case renderer::AttributeFormat::eVec3:
			{
				auto value = parseValue< renderer::AttributeFormat::eVec3 >( p_value );
				p_particle.setValue< renderer::AttributeFormat::eVec3 >( p_index, value );
			}
			break;

		case renderer::AttributeFormat::eVec4:
			{
				auto value = parseValue< renderer::AttributeFormat::eVec4 >( p_value );
				p_particle.setValue< renderer::AttributeFormat::eVec4 >( p_index, value );
			}
			break;

		case renderer::AttributeFormat::eColour:
			{
				auto value = parseValue< renderer::AttributeFormat::eColour >( p_value );
				p_particle.setValue< renderer::AttributeFormat::eColour >( p_index, value );
			}
			break;

		case renderer::AttributeFormat::eInt:
			{
				auto value = parseValue< renderer::AttributeFormat::eInt >( p_value );
				p_particle.setValue< renderer::AttributeFormat::eInt >( p_index, value );
			}
			break;

		case renderer::AttributeFormat::eIVec2:
			{
				auto value = parseValue< renderer::AttributeFormat::eIVec2 >( p_value );
				p_particle.setValue< renderer::AttributeFormat::eIVec2 >( p_index, value );
			}
			break;

		case renderer::AttributeFormat::eIVec3:
			{
				auto value = parseValue< renderer::AttributeFormat::eIVec3 >( p_value );
				p_particle.setValue< renderer::AttributeFormat::eIVec3 >( p_index, value );
			}
			break;

		case renderer::AttributeFormat::eIVec4:
			{
				auto value = parseValue< renderer::AttributeFormat::eIVec4 >( p_value );
				p_particle.setValue< renderer::AttributeFormat::eIVec4 >( p_index, value );
			}
			break;

		case renderer::AttributeFormat::eUInt:
			{
				auto value = parseValue< renderer::AttributeFormat::eUInt >( p_value );
				p_particle.setValue< renderer::AttributeFormat::eUInt >( p_index, value );
			}
			break;

		case renderer::AttributeFormat::eUIVec2:
			{
				auto value = parseValue< renderer::AttributeFormat::eUIVec2 >( p_value );
				p_particle.setValue< renderer::AttributeFormat::eUIVec2 >( p_index, value );
			}
			break;

		case renderer::AttributeFormat::eUIVec3:
			{
				auto value = parseValue< renderer::AttributeFormat::eUIVec3 >( p_value );
				p_particle.setValue< renderer::AttributeFormat::eUIVec3 >( p_index, value );
			}
			break;

		case renderer::AttributeFormat::eUIVec4:
			{
				auto value = parseValue< renderer::AttributeFormat::eUIVec4 >( p_value );
				p_particle.setValue< renderer::AttributeFormat::eUIVec4 >( p_index, value );
			}
			break;

		case renderer::AttributeFormat::eMat2:
		{
			auto value = parseValue< renderer::AttributeFormat::eMat2 > (p_value);
			p_particle.setValue< renderer::AttributeFormat::eMat2 > (p_index, value);
		}
		break;

		case renderer::AttributeFormat::eMat3:
		{
			auto value = parseValue< renderer::AttributeFormat::eMat3 > (p_value);
			p_particle.setValue< renderer::AttributeFormat::eMat3 > (p_index, value);
		}
		break;

		case renderer::AttributeFormat::eMat4:
			{
				auto value = parseValue< renderer::AttributeFormat::eMat4 >( p_value );
				p_particle.setValue< renderer::AttributeFormat::eMat4 >( p_index, value );
			}
			break;
		}
	}

	template< renderer::AttributeFormat Type >
	inline void Particle::setValue( uint32_t p_index, typename ElementTyper< Type >::Type const & p_value )
	{
		REQUIRE( p_index < m_description.size() );
		auto it = m_description.begin() + p_index;
		REQUIRE( it->m_dataType == Type );
		std::memcpy( &m_data[it->m_offset], ElementTyper< Type >::getPointer( p_value ), sizeof( p_value ) );
	}

	template< renderer::AttributeFormat Type >
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
