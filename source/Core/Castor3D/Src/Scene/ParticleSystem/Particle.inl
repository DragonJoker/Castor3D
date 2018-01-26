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
	struct ElementTyper< renderer::AttributeFormat::eFloat >
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
	struct ElementTyper< renderer::AttributeFormat::eVec2f >
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
	struct ElementTyper< renderer::AttributeFormat::eVec3f >
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
	struct ElementTyper< renderer::AttributeFormat::eVec4f >
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
	struct ElementTyper< renderer::AttributeFormat::eColour >
	{
		using Type = uint32_t;

		static inline Type parse( castor::String const & value )
		{
			Type result{};
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
	struct ElementTyper< renderer::AttributeFormat::eInt >
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
	struct ElementTyper< renderer::AttributeFormat::eVec2i >
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
	struct ElementTyper< renderer::AttributeFormat::eVec3i >
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
	struct ElementTyper< renderer::AttributeFormat::eVec4i >
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
	struct ElementTyper< renderer::AttributeFormat::eUInt >
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
	struct ElementTyper< renderer::AttributeFormat::eVec2ui >
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
	struct ElementTyper< renderer::AttributeFormat::eVec3ui >
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
	struct ElementTyper< renderer::AttributeFormat::eVec4ui >
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
	struct ElementTyper< renderer::AttributeFormat::eMat2f >
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
	struct ElementTyper< renderer::AttributeFormat::eMat3f >
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
	struct ElementTyper< renderer::AttributeFormat::eMat4f >
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

	template< renderer::AttributeFormat Type >
	inline typename ElementTyper< Type >::Type parseValue( castor::String const & value )
	{
		return ElementTyper< Type >::parse( value );
	}

	inline void parseValue( castor::String const & textValue
		, renderer::AttributeFormat type
		, Particle & particle
		, uint32_t index )
	{
		switch ( type )
		{
		case renderer::AttributeFormat::eFloat:
			{
				auto value = parseValue< renderer::AttributeFormat::eFloat >( textValue );
				particle.setValue< renderer::AttributeFormat::eFloat >( index, value );
			}
			break;

		case renderer::AttributeFormat::eVec2f:
			{
				auto value = parseValue< renderer::AttributeFormat::eVec2f >( textValue );
				particle.setValue< renderer::AttributeFormat::eVec2f >( index, value );
			}
			break;

		case renderer::AttributeFormat::eVec3f:
			{
				auto value = parseValue< renderer::AttributeFormat::eVec3f >( textValue );
				particle.setValue< renderer::AttributeFormat::eVec3f >( index, value );
			}
			break;

		case renderer::AttributeFormat::eVec4f:
			{
				auto value = parseValue< renderer::AttributeFormat::eVec4f >( textValue );
				particle.setValue< renderer::AttributeFormat::eVec4f >( index, value );
			}
			break;

		case renderer::AttributeFormat::eColour:
			{
				auto value = parseValue< renderer::AttributeFormat::eColour >( textValue );
				particle.setValue< renderer::AttributeFormat::eColour >( index, value );
			}
			break;

		case renderer::AttributeFormat::eInt:
			{
				auto value = parseValue< renderer::AttributeFormat::eInt >( textValue );
				particle.setValue< renderer::AttributeFormat::eInt >( index, value );
			}
			break;

		case renderer::AttributeFormat::eVec2i:
			{
				auto value = parseValue< renderer::AttributeFormat::eVec2i >( textValue );
				particle.setValue< renderer::AttributeFormat::eVec2i >( index, value );
			}
			break;

		case renderer::AttributeFormat::eVec3i:
			{
				auto value = parseValue< renderer::AttributeFormat::eVec3i >( textValue );
				particle.setValue< renderer::AttributeFormat::eVec3i >( index, value );
			}
			break;

		case renderer::AttributeFormat::eVec4i:
			{
				auto value = parseValue< renderer::AttributeFormat::eVec4i >( textValue );
				particle.setValue< renderer::AttributeFormat::eVec4i >( index, value );
			}
			break;

		case renderer::AttributeFormat::eUInt:
			{
				auto value = parseValue< renderer::AttributeFormat::eUInt >( textValue );
				particle.setValue< renderer::AttributeFormat::eUInt >( index, value );
			}
			break;

		case renderer::AttributeFormat::eVec2ui:
			{
				auto value = parseValue< renderer::AttributeFormat::eVec2ui >( textValue );
				particle.setValue< renderer::AttributeFormat::eVec2ui >( index, value );
			}
			break;

		case renderer::AttributeFormat::eVec3ui:
			{
				auto value = parseValue< renderer::AttributeFormat::eVec3ui >( textValue );
				particle.setValue< renderer::AttributeFormat::eVec3ui >( index, value );
			}
			break;

		case renderer::AttributeFormat::eVec4ui:
			{
				auto value = parseValue< renderer::AttributeFormat::eVec4ui >( textValue );
				particle.setValue< renderer::AttributeFormat::eVec4ui >( index, value );
			}
			break;

		case renderer::AttributeFormat::eMat2f:
		{
			auto value = parseValue< renderer::AttributeFormat::eMat2f >( textValue );
			particle.setValue< renderer::AttributeFormat::eMat2f > (index, value);
		}
		break;

		case renderer::AttributeFormat::eMat3f:
		{
			auto value = parseValue< renderer::AttributeFormat::eMat3f >( textValue );
			particle.setValue< renderer::AttributeFormat::eMat3f > (index, value);
		}
		break;

		case renderer::AttributeFormat::eMat4f:
			{
				auto value = parseValue< renderer::AttributeFormat::eMat4f >( textValue );
				particle.setValue< renderer::AttributeFormat::eMat4f >( index, value );
			}
			break;
		}
	}

	template< renderer::AttributeFormat Type >
	inline void Particle::setValue( uint32_t index, typename ElementTyper< Type >::Type const & value )
	{
		REQUIRE( index < m_description.size() );
		auto it = m_description.begin() + index;
		REQUIRE( it->m_dataType == Type );
		std::memcpy( &m_data[it->m_offset], ElementTyper< Type >::getPointer( value ), sizeof( value ) );
	}

	template< renderer::AttributeFormat Type >
	inline typename ElementTyper< Type >::Type Particle::getValue( uint32_t index )const
	{
		REQUIRE( index < m_description.size() );
		auto it = m_description.begin() + index;
		REQUIRE( it->m_dataType == Type );
		typename ElementTyper< Type >::Type result{};
		std::memcpy( ElementTyper< Type >::getPointer( result ), &m_data[it->m_offset], sizeof( result ) );
		return result;
	}
}
