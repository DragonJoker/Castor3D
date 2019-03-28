#include "CastorUtils/FileParser/ParserParameterBase.hpp"

#include "CastorUtils/FileParser/ParserParameter.hpp"
#include "CastorUtils/FileParser/ParserParameterTypeException.hpp"

namespace castor
{
	namespace
	{
		template< typename T >
		void doGetParameterValue( ParserParameterBase const & parameter
			, T & value )
		{
			static ParameterType constexpr given = ParserValueTyper< T >::Type;
			auto expected = parameter.getBaseType();

			if ( given != expected )
			{
				throw ParserParameterTypeException< given >( expected );
			}

			value = static_cast< ParserParameter< given > const & >( parameter ).m_value;
		}
	}

	void getParameterValue( ParserParameterBase const & parameter
		, bool & value )
	{
		doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, int8_t & value )
	{
		doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, uint8_t & value )
	{
		doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, int16_t & value )
	{
		doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, uint16_t & value )
	{
		doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, int32_t & value )
	{
		doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, uint32_t & value )
	{
		doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, int64_t & value )
	{
		doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, uint64_t & value )
	{
		doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, float & value )
	{
		doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, double & value )
	{
		doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, long double & value )
	{
		doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, String & value )
	{
		doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, Path & value )
	{
		doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, PixelFormat & value )
	{
		doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, Point2i & value )
	{
		doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, Point3i & value )
	{
		doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, Point4i & value )
	{
		doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, Point2f & value )
	{
		doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, Point3f & value )
	{
		doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, Point4f & value )
	{
		doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, Point2d & value )
	{
		doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, Point3d & value )
	{
		doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, Point4d & value )
	{
		doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, Size & value )
	{
		doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, Position & value )
	{
		doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, Rectangle & value )
	{
		doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, RgbColour & value )
	{
		doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, RgbaColour & value )
	{
		doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, HdrRgbColour & value )
	{
		doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, HdrRgbaColour & value )
	{
		doGetParameterValue( parameter, value );
	}
}

