#ifndef ___Date___
#define ___Date___

#include <wchar.h>

namespace Castor
{	namespace Utils
{
	class Date
	{
	protected:
		time_t m_time;

	public:
		typedef enum
		{
			jan,
			feb,
			mar,
			apr,
			may,
			jun,
			jul,
			aug,
			sep,
			oct,
			nov,
			dec
		}
		eMONTH;

	public:
		Date( int iYear, eMONTH eMonth, int iDay);
		Date( struct tm * p_pTm = NULL);
		void ToCLibTm( struct tm * p_pTm)const;
		void FromCLibTm( struct tm * p_pTm);

		int GetYear();
		eMONTH GetMonth();
		int GetMonthDay();
		int GetYearDay();
		int GetWeekDay();
	};
}
}

std::ostream & operator <<( std::ostream & p_osStream, const Castor::Utils::Date & p_dtDate);
std::istream & operator >>( std::istream & p_isStream, Castor::Utils::Date & p_dtDate);

#endif
