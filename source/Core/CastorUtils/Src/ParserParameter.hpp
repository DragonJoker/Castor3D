/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___CASTOR_PARSER_PARAMETER_H___
#define ___CASTOR_PARSER_PARAMETER_H___

#include "ParserParameterBase.hpp"

#include "Colour.hpp"
#include "Path.hpp"
#include "Point.hpp"
#include "Position.hpp"
#include "Rectangle.hpp"
#include "Size.hpp"

namespace Castor
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		26/03/2013
	\version	0.7.0
	\~english
	\brief		Specified parser parameter
	\~french
	\brief		Parmètre de parseur spécifié
	\remark
	*/
	template< ePARAMETER_TYPE Type > class ParserParameter;

	/*!
	\author 	Sylvain DOREMUS
	\date 		26/03/2013
	\version	0.7.0
	\~english
	\brief		ParserParameter specialisation for ePARAMETER_TYPE_TEXT
	\~french
	\brief		Specialisation de ParserParameter pour ePARAMETER_TYPE_TEXT
	\remark
	*/
	template<> class ParserParameter< ePARAMETER_TYPE_TEXT >
		: public ParserParameterBase
	{
	public:
		inline ParserParameter( String const & p_functionName );
		virtual ePARAMETER_TYPE GetType();
		virtual xchar const * GetStrType();
		virtual bool Parse( String & p_strParams );

	public:
		typedef String value_type;
		value_type m_value;
	};

	/*!
	\author 	Sylvain DOREMUS
	\date 		26/03/2013
	\version	0.7.0
	\~english
	\brief		ParserParameter specialisation for ePARAMETER_TYPE_NAME
	\~french
	\brief		Specialisation de ParserParameter pour ePARAMETER_TYPE_NAME
	\remark
	*/
	template<> class ParserParameter< ePARAMETER_TYPE_NAME >
		: public ParserParameter< ePARAMETER_TYPE_TEXT >
	{
	public:
		inline ParserParameter( String const & p_functionName );
		virtual ePARAMETER_TYPE GetType();
		virtual ePARAMETER_TYPE GetBaseType();
		virtual xchar const * GetStrType();
		virtual bool Parse( String & p_strParams );
	};

	/*!
	\author 	Sylvain DOREMUS
	\date 		26/03/2013
	\version	0.7.0
	\~english
	\brief		ParserParameter specialisation for ePARAMETER_TYPE_PATH
	\~french
	\brief		Specialisation de ParserParameter pour ePARAMETER_TYPE_PATH
	\remark
	*/
	template<> class ParserParameter< ePARAMETER_TYPE_PATH >
		: public ParserParameterBase
	{
	public:
		inline ParserParameter( String const & p_functionName );
		virtual ePARAMETER_TYPE GetType();
		virtual xchar const * GetStrType();
		virtual bool Parse( String & p_strParams );

	public:
		typedef Path value_type;
		value_type m_value;
	};

	/*!
	\author 	Sylvain DOREMUS
	\date 		26/03/2013
	\version	0.7.0
	\~english
	\brief		ParserParameter specialisation for ePARAMETER_TYPE_BOOL
	\~french
	\brief		Specialisation de ParserParameter pour ePARAMETER_TYPE_BOOL
	\remark
	*/
	template<> class ParserParameter< ePARAMETER_TYPE_BOOL >
		: public ParserParameterBase
	{
	public:
		inline ParserParameter( String const & p_functionName );
		inline ePARAMETER_TYPE GetType();
		inline xchar const * GetStrType();
		inline bool Parse( String & p_strParams );

	public:
		typedef bool value_type;
		value_type m_value;
	};

	/*!
	\author 	Sylvain DOREMUS
	\date 		26/03/2013
	\version	0.7.0
	\~english
	\brief		ParserParameter specialisation for ePARAMETER_TYPE_INT8
	\~french
	\brief		Specialisation de ParserParameter pour ePARAMETER_TYPE_INT8
	\remark
	*/
	template<> class ParserParameter< ePARAMETER_TYPE_INT8 >
		: public ParserParameterBase
	{
	public:
		inline ParserParameter( String const & p_functionName );
		inline ePARAMETER_TYPE GetType();
		inline xchar const * GetStrType();
		inline bool Parse( String & p_strParams );

	public:
		typedef int8_t value_type;
		value_type m_value;
	};

	/*!
	\author 	Sylvain DOREMUS
	\date 		26/03/2013
	\version	0.7.0
	\~english
	\brief		ParserParameter specialisation for ePARAMETER_TYPE_INT16
	\~french
	\brief		Specialisation de ParserParameter pour ePARAMETER_TYPE_INT16
	\remark
	*/
	template<> class ParserParameter< ePARAMETER_TYPE_INT16 >
		: public ParserParameterBase
	{
	public:
		inline ParserParameter( String const & p_functionName );
		inline ePARAMETER_TYPE GetType();
		inline xchar const * GetStrType();
		inline bool Parse( String & p_strParams );

	public:
		typedef int16_t value_type;
		value_type m_value;
	};

	/*!
	\author 	Sylvain DOREMUS
	\date 		26/03/2013
	\version	0.7.0
	\~english
	\brief		ParserParameter specialisation for ePARAMETER_TYPE_INT32
	\~french
	\brief		Specialisation de ParserParameter pour ePARAMETER_TYPE_INT32
	\remark
	*/
	template<> class ParserParameter< ePARAMETER_TYPE_INT32 >
		: public ParserParameterBase
	{
	public:
		inline ParserParameter( String const & p_functionName );
		inline ePARAMETER_TYPE GetType();
		inline xchar const * GetStrType();
		inline bool Parse( String & p_strParams );

	public:
		typedef int32_t value_type;
		value_type m_value;
	};

	/*!
	\author 	Sylvain DOREMUS
	\date 		26/03/2013
	\version	0.7.0
	\~english
	\brief		ParserParameter specialisation for ePARAMETER_TYPE_INT64
	\~french
	\brief		Specialisation de ParserParameter pour ePARAMETER_TYPE_INT64
	\remark
	*/
	template<> class ParserParameter< ePARAMETER_TYPE_INT64 >
		: public ParserParameterBase
	{
	public:
		inline ParserParameter( String const & p_functionName );
		inline ePARAMETER_TYPE GetType();
		inline xchar const * GetStrType();
		inline bool Parse( String & p_strParams );

	public:
		typedef int64_t value_type;
		value_type m_value;
	};

	/*!
	\author 	Sylvain DOREMUS
	\date 		26/03/2013
	\version	0.7.0
	\~english
	\brief		ParserParameter specialisation for ePARAMETER_TYPE_UINT8
	\~french
	\brief		Specialisation de ParserParameter pour ePARAMETER_TYPE_UINT8
	\remark
	*/
	template<> class ParserParameter< ePARAMETER_TYPE_UINT8 >
		: public ParserParameterBase
	{
	public:
		inline ParserParameter( String const & p_functionName );
		inline ePARAMETER_TYPE GetType();
		inline xchar const * GetStrType();
		inline bool Parse( String & p_strParams );

	public:
		typedef uint8_t value_type;
		value_type m_value;
	};

	/*!
	\author 	Sylvain DOREMUS
	\date 		26/03/2013
	\version	0.7.0
	\~english
	\brief		ParserParameter specialisation for ePARAMETER_TYPE_UINT16
	\~french
	\brief		Specialisation de ParserParameter pour ePARAMETER_TYPE_UINT16
	\remark
	*/
	template<> class ParserParameter< ePARAMETER_TYPE_UINT16 >
		: public ParserParameterBase
	{
	public:
		inline ParserParameter( String const & p_functionName );
		inline ePARAMETER_TYPE GetType();
		inline xchar const * GetStrType();
		inline bool Parse( String & p_strParams );

	public:
		typedef uint16_t value_type;
		value_type m_value;
	};

	/*!
	\author 	Sylvain DOREMUS
	\date 		26/03/2013
	\version	0.7.0
	\~english
	\brief		ParserParameter specialisation for ePARAMETER_TYPE_UINT32
	\~french
	\brief		Specialisation de ParserParameter pour ePARAMETER_TYPE_UINT32
	\remark
	*/
	template<> class ParserParameter< ePARAMETER_TYPE_UINT32 >
		: public ParserParameterBase
	{
	public:
		inline ParserParameter( String const & p_functionName );
		inline ePARAMETER_TYPE GetType();
		inline xchar const * GetStrType();
		inline bool Parse( String & p_strParams );

	public:
		typedef uint32_t value_type;
		value_type m_value;
	};

	/*!
	\author 	Sylvain DOREMUS
	\date 		26/03/2013
	\version	0.7.0
	\~english
	\brief		ParserParameter specialisation for ePARAMETER_TYPE_CHECKED_TEXT
	\~french
	\brief		Specialisation de ParserParameter pour ePARAMETER_TYPE_CHECKED_TEXT
	\remark
	*/
	template<> class ParserParameter< ePARAMETER_TYPE_CHECKED_TEXT >
		: public ParserParameter< ePARAMETER_TYPE_UINT32 >
	{
	public:
		inline ParserParameter( String const & p_functionName, UIntStrMap const & p_mapValues );
		inline ePARAMETER_TYPE GetType();
		inline ePARAMETER_TYPE GetBaseType();
		inline xchar const * GetStrType();
		inline bool Parse( String & p_strParams );

	public:
		UIntStrMap m_mapValues;
	};

	/*!
	\author 	Sylvain DOREMUS
	\date 		26/03/2013
	\version	0.7.0
	\~english
	\brief		ParserParameter specialisation for ePARAMETER_TYPE_BITWISE_ORED_CHECKED_TEXT
	\~french
	\brief		Specialisation de ParserParameter pour ePARAMETER_TYPE_BITWISE_ORED_CHECKED_TEXT
	\remark
	*/
	template<> class ParserParameter< ePARAMETER_TYPE_BITWISE_ORED_CHECKED_TEXT >
		: public ParserParameter< ePARAMETER_TYPE_UINT32 >
	{
	public:
		inline ParserParameter( String const & p_functionName, UIntStrMap const & p_mapValues );
		inline ePARAMETER_TYPE GetType();
		inline ePARAMETER_TYPE GetBaseType();
		inline xchar const * GetStrType();
		inline bool Parse( String & p_strParams );

	public:
		UIntStrMap m_mapValues;
	};

	/*!
	\author 	Sylvain DOREMUS
	\date 		26/03/2013
	\version	0.7.0
	\~english
	\brief		ParserParameter specialisation for ePARAMETER_TYPE_UINT64
	\~french
	\brief		Specialisation de ParserParameter pour ePARAMETER_TYPE_UINT64
	\remark
	*/
	template<> class ParserParameter< ePARAMETER_TYPE_UINT64 >
		: public ParserParameterBase
	{
	public:
		inline ParserParameter( String const & p_functionName );
		inline ePARAMETER_TYPE GetType();
		inline xchar const * GetStrType();
		inline bool Parse( String & p_strParams );

	public:
		typedef uint64_t value_type;
		value_type m_value;
	};

	/*!
	\author 	Sylvain DOREMUS
	\date 		26/03/2013
	\version	0.7.0
	\~english
	\brief		ParserParameter specialisation for ePARAMETER_TYPE_FLOAT
	\~french
	\brief		Specialisation de ParserParameter pour ePARAMETER_TYPE_FLOAT
	\remark
	*/
	template<> class ParserParameter< ePARAMETER_TYPE_FLOAT >
		: public ParserParameterBase
	{
	public:
		inline ParserParameter( String const & p_functionName );
		inline ePARAMETER_TYPE GetType();
		inline xchar const * GetStrType();
		inline bool Parse( String & p_strParams );

	public:
		typedef float value_type;
		value_type m_value;
	};

	/*!
	\author 	Sylvain DOREMUS
	\date 		26/03/2013
	\version	0.7.0
	\~english
	\brief		ParserParameter specialisation for ePARAMETER_TYPE_DOUBLE
	\~french
	\brief		Specialisation de ParserParameter pour ePARAMETER_TYPE_DOUBLE
	\remark
	*/
	template<> class ParserParameter< ePARAMETER_TYPE_DOUBLE >
		: public ParserParameterBase
	{
	public:
		inline ParserParameter( String const & p_functionName );
		inline ePARAMETER_TYPE GetType();
		inline xchar const * GetStrType();
		inline bool Parse( String & p_strParams );

	public:
		typedef double value_type;
		value_type m_value;
	};

	/*!
	\author 	Sylvain DOREMUS
	\date 		26/03/2013
	\version	0.7.0
	\~english
	\brief		ParserParameter specialisation for ePARAMETER_TYPE_LONGDOUBLE
	\~french
	\brief		Specialisation de ParserParameter pour ePARAMETER_TYPE_LONGDOUBLE
	\remark
	*/
	template<> class ParserParameter< ePARAMETER_TYPE_LONGDOUBLE >
		: public ParserParameterBase
	{
	public:
		inline ParserParameter( String const & p_functionName );
		inline ePARAMETER_TYPE GetType();
		inline xchar const * GetStrType();
		inline bool Parse( String & p_strParams );

	public:
		typedef long double value_type;
		value_type m_value;
	};

	/*!
	\author 	Sylvain DOREMUS
	\date 		26/03/2013
	\version	0.7.0
	\~english
	\brief		ParserParameter specialisation for ePARAMETER_TYPE_PIXELFORMAT
	\~french
	\brief		Specialisation de ParserParameter pour ePARAMETER_TYPE_PIXELFORMAT
	\remark
	*/
	template<> class ParserParameter< ePARAMETER_TYPE_PIXELFORMAT >
		: public ParserParameterBase
	{
	public:
		inline ParserParameter( String const & p_functionName );
		inline ePARAMETER_TYPE GetType();
		inline xchar const * GetStrType();
		inline bool Parse( String & p_strParams );

	public:
		typedef ePIXEL_FORMAT value_type;
		value_type m_value;
	};

	/*!
	\author 	Sylvain DOREMUS
	\date 		26/03/2013
	\version	0.7.0
	\~english
	\brief		ParserParameter specialisation for ePARAMETER_TYPE_POINT2I
	\~french
	\brief		Specialisation de ParserParameter pour ePARAMETER_TYPE_POINT2I
	\remark
	*/
	template<> class ParserParameter< ePARAMETER_TYPE_POINT2I >
		: public ParserParameterBase
	{
	public:
		inline ParserParameter( String const & p_functionName );
		inline ePARAMETER_TYPE GetType();
		inline xchar const * GetStrType();
		inline bool Parse( String & p_strParams );

	public:
		typedef Point2i value_type;
		value_type m_value;
	};

	/*!
	\author 	Sylvain DOREMUS
	\date 		26/03/2013
	\version	0.7.0
	\~english
	\brief		ParserParameter specialisation for ePARAMETER_TYPE_POINT3I
	\~french
	\brief		Specialisation de ParserParameter pour ePARAMETER_TYPE_POINT3I
	\remark
	*/
	template<> class ParserParameter< ePARAMETER_TYPE_POINT3I >
		: public ParserParameterBase
	{
	public:
		inline ParserParameter( String const & p_functionName );
		inline ePARAMETER_TYPE GetType();
		inline xchar const * GetStrType();
		inline bool Parse( String & p_strParams );

	public:
		typedef Point3i value_type;
		value_type m_value;
	};

	/*!
	\author 	Sylvain DOREMUS
	\date 		26/03/2013
	\version	0.7.0
	\~english
	\brief		ParserParameter specialisation for ePARAMETER_TYPE_POINT4I
	\~french
	\brief		Specialisation de ParserParameter pour ePARAMETER_TYPE_POINT4I
	\remark
	*/
	template<> class ParserParameter< ePARAMETER_TYPE_POINT4I >
		: public ParserParameterBase
	{
	public:
		inline ParserParameter( String const & p_functionName );
		inline ePARAMETER_TYPE GetType();
		inline xchar const * GetStrType();
		inline bool Parse( String & p_strParams );

	public:
		typedef Point4i value_type;
		value_type m_value;
	};

	/*!
	\author 	Sylvain DOREMUS
	\date 		26/03/2013
	\version	0.7.0
	\~english
	\brief		ParserParameter specialisation for ePARAMETER_TYPE_POINT2F
	\~french
	\brief		Specialisation de ParserParameter pour ePARAMETER_TYPE_POINT2F
	\remark
	*/
	template<> class ParserParameter< ePARAMETER_TYPE_POINT2F >
		: public ParserParameterBase
	{
	public:
		inline ParserParameter( String const & p_functionName );
		inline ePARAMETER_TYPE GetType();
		inline xchar const * GetStrType();
		inline bool Parse( String & p_strParams );

	public:
		typedef Point2f value_type;
		value_type m_value;
	};

	/*!
	\author 	Sylvain DOREMUS
	\date 		26/03/2013
	\version	0.7.0
	\~english
	\brief		ParserParameter specialisation for ePARAMETER_TYPE_POINT3F
	\~french
	\brief		Specialisation de ParserParameter pour ePARAMETER_TYPE_POINT3F
	\remark
	*/
	template<> class ParserParameter< ePARAMETER_TYPE_POINT3F >
		: public ParserParameterBase
	{
	public:
		inline ParserParameter( String const & p_functionName );
		inline ePARAMETER_TYPE GetType();
		inline xchar const * GetStrType();
		inline bool Parse( String & p_strParams );

	public:
		typedef Point3f value_type;
		value_type m_value;
	};

	/*!
	\author 	Sylvain DOREMUS
	\date 		26/03/2013
	\version	0.7.0
	\~english
	\brief		ParserParameter specialisation for ePARAMETER_TYPE_POINT4F
	\~french
	\brief		Specialisation de ParserParameter pour ePARAMETER_TYPE_POINT4F
	\remark
	*/
	template<> class ParserParameter< ePARAMETER_TYPE_POINT4F >
		: public ParserParameterBase
	{
	public:
		inline ParserParameter( String const & p_functionName );
		inline ePARAMETER_TYPE GetType();
		inline xchar const * GetStrType();
		inline bool Parse( String & p_strParams );

	public:
		typedef Point4f value_type;
		value_type m_value;
	};

	/*!
	\author 	Sylvain DOREMUS
	\date 		26/03/2013
	\version	0.7.0
	\~english
	\brief		ParserParameter specialisation for ePARAMETER_TYPE_POINT2D
	\~french
	\brief		Specialisation de ParserParameter pour ePARAMETER_TYPE_POINT2D
	\remark
	*/
	template<> class ParserParameter< ePARAMETER_TYPE_POINT2D >
		: public ParserParameterBase
	{
	public:
		inline ParserParameter( String const & p_functionName );
		inline ePARAMETER_TYPE GetType();
		inline xchar const * GetStrType();
		inline bool Parse( String & p_strParams );

	public:
		typedef Point2d value_type;
		value_type m_value;
	};

	/*!
	\author 	Sylvain DOREMUS
	\date 		26/03/2013
	\version	0.7.0
	\~english
	\brief		ParserParameter specialisation for ePARAMETER_TYPE_POINT3D
	\~french
	\brief		Specialisation de ParserParameter pour ePARAMETER_TYPE_POINT3D
	\remark
	*/
	template<> class ParserParameter< ePARAMETER_TYPE_POINT3D >
		: public ParserParameterBase
	{
	public:
		inline ParserParameter( String const & p_functionName );
		inline ePARAMETER_TYPE GetType();
		inline xchar const * GetStrType();
		inline bool Parse( String & p_strParams );

	public:
		typedef Point3d value_type;
		value_type m_value;
	};

	/*!
	\author 	Sylvain DOREMUS
	\date 		26/03/2013
	\version	0.7.0
	\~english
	\brief		ParserParameter specialisation for ePARAMETER_TYPE_POINT4D
	\~french
	\brief		Specialisation de ParserParameter pour ePARAMETER_TYPE_POINT4D
	\remark
	*/
	template<> class ParserParameter< ePARAMETER_TYPE_POINT4D >
		: public ParserParameterBase
	{
	public:
		inline ParserParameter( String const & p_functionName );
		inline ePARAMETER_TYPE GetType();
		inline xchar const * GetStrType();
		inline bool Parse( String & p_strParams );

	public:
		typedef Point4d value_type;
		value_type m_value;
	};

	/*!
	\author 	Sylvain DOREMUS
	\date 		26/03/2013
	\version	0.7.0
	\~english
	\brief		ParserParameter specialisation for ePARAMETER_TYPE_SIZE
	\~french
	\brief		Specialisation de ParserParameter pour ePARAMETER_TYPE_SIZE
	\remark
	*/
	template<> class ParserParameter< ePARAMETER_TYPE_SIZE >
		: public ParserParameterBase
	{
	public:
		inline ParserParameter( String const & p_functionName );
		inline ePARAMETER_TYPE GetType();
		inline xchar const * GetStrType();
		inline bool Parse( String & p_strParams );

	public:
		typedef Size value_type;
		value_type m_value;
	};

	/*!
	\author 	Sylvain DOREMUS
	\date 		26/03/2013
	\version	0.7.0
	\~english
	\brief		ParserParameter specialisation for ePARAMETER_TYPE_POSITION
	\~french
	\brief		Specialisation de ParserParameter pour ePARAMETER_TYPE_POSITION
	\remark
	*/
	template<> class ParserParameter< ePARAMETER_TYPE_POSITION >
		: public ParserParameterBase
	{
	public:
		inline ParserParameter( String const & p_functionName );
		inline ePARAMETER_TYPE GetType();
		inline xchar const * GetStrType();
		inline bool Parse( String & p_strParams );

	public:
		typedef Position value_type;
		value_type m_value;
	};

	/*!
	\author 	Sylvain DOREMUS
	\date 		26/03/2013
	\version	0.7.0
	\~english
	\brief		ParserParameter specialisation for ePARAMETER_TYPE_RECTANGLE
	\~french
	\brief		Specialisation de ParserParameter pour ePARAMETER_TYPE_RECTANGLE
	\remark
	*/
	template<> class ParserParameter< ePARAMETER_TYPE_RECTANGLE >
		: public ParserParameterBase
	{
	public:
		inline ParserParameter( String const & p_functionName );
		inline ePARAMETER_TYPE GetType();
		inline xchar const * GetStrType();
		inline bool Parse( String & p_strParams );

	public:
		typedef Rectangle value_type;
		value_type m_value;
	};

	/*!
	\author 	Sylvain DOREMUS
	\date 		26/03/2013
	\version	0.7.0
	\~english
	\brief		ParserParameter specialisation for ePARAMETER_TYPE_COLOUR
	\~french
	\brief		Specialisation de ParserParameter pour ePARAMETER_TYPE_COLOUR
	\remark
	*/
	template<> class ParserParameter< ePARAMETER_TYPE_COLOUR >
		: public ParserParameterBase
	{
	public:
		inline ParserParameter( String const & p_functionName );
		inline ePARAMETER_TYPE GetType();
		inline xchar const * GetStrType();
		inline bool Parse( String & p_strParams );

	public:
		typedef Colour value_type;
		value_type m_value;
	};
}

#include "ParserParameter.inl"

#endif
