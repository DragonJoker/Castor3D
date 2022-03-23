#include "CastorUtils/FileParser/ParserParameterBase.hpp"

#include "CastorUtils/FileParser/ParserParameter.hpp"
#include "CastorUtils/FileParser/ParserParameterTypeException.hpp"

namespace castor
{
	namespace prsparam
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
		prsparam::doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, int8_t & value )
	{
		prsparam::doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, uint8_t & value )
	{
		prsparam::doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, int16_t & value )
	{
		prsparam::doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, uint16_t & value )
	{
		prsparam::doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, int32_t & value )
	{
		prsparam::doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, uint32_t & value )
	{
		prsparam::doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, int64_t & value )
	{
		prsparam::doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, uint64_t & value )
	{
		prsparam::doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, float & value )
	{
		prsparam::doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, double & value )
	{
		prsparam::doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, long double & value )
	{
		prsparam::doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, String & value )
	{
		prsparam::doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, Path & value )
	{
		prsparam::doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, PixelFormat & value )
	{
		prsparam::doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, Point2i & value )
	{
		prsparam::doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, Point3i & value )
	{
		prsparam::doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, Point4i & value )
	{
		prsparam::doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, Point2ui & value )
	{
		prsparam::doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, Point3ui & value )
	{
		prsparam::doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, Point4ui & value )
	{
		prsparam::doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, Point2f & value )
	{
		prsparam::doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, Point3f & value )
	{
		prsparam::doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, Point4f & value )
	{
		prsparam::doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, Point2d & value )
	{
		prsparam::doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, Point3d & value )
	{
		prsparam::doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, Point4d & value )
	{
		prsparam::doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, Size & value )
	{
		prsparam::doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, Position & value )
	{
		prsparam::doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, Rectangle & value )
	{
		prsparam::doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, RgbColour & value )
	{
		prsparam::doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, RgbaColour & value )
	{
		prsparam::doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, HdrRgbColour & value )
	{
		prsparam::doGetParameterValue( parameter, value );
	}

	void getParameterValue( ParserParameterBase const & parameter
		, HdrRgbaColour & value )
	{
		prsparam::doGetParameterValue( parameter, value );
	}
}

