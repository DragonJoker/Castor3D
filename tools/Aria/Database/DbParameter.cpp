#include "Aria/Database/DbParameter.hpp"

namespace aria::db
{
	namespace
	{
		bool areTypesCompatibleSet( FieldType typeFrom, FieldType typeTo )
		{
			bool result = typeFrom == typeTo;

			if ( !result )
			{
				switch ( typeFrom )
				{
				case FieldType::eNULL:
					result = false;
					break;

				case FieldType::eBIT:
					result = ( typeTo == FieldType::eSINT32 ) || ( typeTo == FieldType::eSINT64 )
						|| ( typeTo == FieldType::eUINT32 ) || ( typeTo == FieldType::eUINT64 )
						|| ( typeTo == FieldType::eVARCHAR ) || ( typeTo == FieldType::eTEXT );
					break;

				case FieldType::eSINT32:
					result = ( typeTo == FieldType::eBIT ) || ( typeTo == FieldType::eSINT64 )
						|| ( typeTo == FieldType::eUINT32 ) || ( typeTo == FieldType::eUINT64 )
						|| ( typeTo == FieldType::eFLOAT32 ) || ( typeTo == FieldType::eFLOAT64 );
					break;

				case FieldType::eSINT64:
					result = ( typeTo == FieldType::eBIT ) || ( typeTo == FieldType::eUINT64 );
					break;

				case FieldType::eUINT32:
					result = ( typeTo == FieldType::eSINT32 ) || ( typeTo == FieldType::eSINT64 )
						|| ( typeTo == FieldType::eBIT ) || ( typeTo == FieldType::eUINT64 )
						|| ( typeTo == FieldType::eFLOAT32 ) || ( typeTo == FieldType::eFLOAT64 );
					break;

				case FieldType::eUINT64:
					result = ( typeTo == FieldType::eSINT64 ) || ( typeTo == FieldType::eBIT );
					break;

				case FieldType::eFLOAT32:
					result = ( typeTo == FieldType::eSINT32 ) || ( typeTo == FieldType::eSINT64 )
						|| ( typeTo == FieldType::eUINT32 ) || ( typeTo == FieldType::eUINT64 )
						|| ( typeTo == FieldType::eFLOAT32 ) || ( typeTo == FieldType::eFLOAT64 );
					break;

				case FieldType::eFLOAT64:
					result = ( typeTo == FieldType::eSINT32 ) || ( typeTo == FieldType::eSINT64 )
						|| ( typeTo == FieldType::eUINT32 ) || ( typeTo == FieldType::eUINT64 )
						|| ( typeTo == FieldType::eFLOAT32 ) || ( typeTo == FieldType::eFLOAT64 );
					break;

				case FieldType::eCHAR:
					result = ( typeTo == FieldType::eVARCHAR ) || ( typeTo == FieldType::eTEXT );
					break;

				case FieldType::eVARCHAR:
					result = ( typeTo == FieldType::eCHAR ) || ( typeTo == FieldType::eTEXT );
					break;

				case FieldType::eTEXT:
					result = ( typeTo == FieldType::eCHAR ) || ( typeTo == FieldType::eVARCHAR );
					break;

				case FieldType::eDATE:
					result = ( typeTo == FieldType::eDATE ) || ( typeTo == FieldType::eDATETIME );
					break;

				case FieldType::eDATETIME:
					result = ( typeTo == FieldType::eDATE ) || ( typeTo == FieldType::eDATETIME ) || ( typeTo == FieldType::eTIME );
					break;

				case FieldType::eTIME:
					result = ( typeTo == FieldType::eTIME ) || ( typeTo == FieldType::eDATETIME );
					break;

				case FieldType::eBINARY:
					result = ( typeTo == FieldType::eBINARY ) || ( typeTo == FieldType::eVARBINARY ) || ( typeTo == FieldType::eBLOB );
					break;

				case FieldType::eVARBINARY:
					result = ( typeTo == FieldType::eBINARY ) || ( typeTo == FieldType::eVARBINARY ) || ( typeTo == FieldType::eBLOB );
					break;

				case FieldType::eBLOB:
					result = ( typeTo == FieldType::eBINARY ) || ( typeTo == FieldType::eVARBINARY ) || ( typeTo == FieldType::eBLOB );
					break;

				default:
					result = false;
					break;
				}
			}

			return result;
		}
	}

	static const std::string ERROR_DB_INCOMPATIBLE_TYPES = "Incompatible types between values, parameter: ";
	static const std::string ERROR_DB_PARAMETER_SETVALUE_TYPE = "Type error while setting value for the parameter: ";

	Parameter::Parameter( Connection & connection
		, ValuedObjectInfos infos
		, unsigned short index
		, ParameterType parameterType
		, std::unique_ptr< ValueUpdater > updater )
		: ValuedObject{ connection, std::move( infos ) }
		, m_index{ index }
		, m_parameterType{ parameterType }
		, m_updater{ std::move( updater ) }
	{
		doCreateValue();
	}

	Parameter::~Parameter()
	{
	}

	void Parameter::setNull()
	{
		getObjectValue().setNull();
		m_updater->update( *this );
	}

	void Parameter::setValue( const ValuedObject & object )
	{
		if ( !object.getObjectValue().isNull() && !areTypesCompatibleSet( object.getType(), getType() ) )
		{
			std::string errMsg = ERROR_DB_INCOMPATIBLE_TYPES + this->getName();
			throw std::runtime_error{ errMsg };
		}

		setValue( object.getType(), object.getObjectValue() );
	}

	void Parameter::setValue( FieldType type
		, ValueBase const & value )
	{
		if ( value.isNull() )
		{
			setNull();
		}
		else
		{
			switch ( type )
			{
			case FieldType::eBIT:
				doSetValue( static_cast< ValueT< FieldType::eBIT > const & >( value ).getValue() );
				break;

			case FieldType::eSINT32:
				doSetValue( static_cast< ValueT< FieldType::eSINT32 > const & >( value ).getValue() );
				break;

			case FieldType::eSINT64:
				doSetValue( static_cast< ValueT< FieldType::eSINT64 > const & >( value ).getValue() );
				break;

			case FieldType::eUINT32:
				doSetValue( static_cast< ValueT< FieldType::eUINT32 > const & >( value ).getValue() );
				break;

			case FieldType::eUINT64:
				doSetValue( static_cast< ValueT< FieldType::eUINT64 > const & >( value ).getValue() );
				break;

			case FieldType::eFLOAT32:
				doSetValue( static_cast< ValueT< FieldType::eFLOAT32 > const & >( value ).getValue() );
				break;

			case FieldType::eFLOAT64:
				doSetValue( static_cast< ValueT< FieldType::eFLOAT64 > const & >( value ).getValue() );
				break;

			case FieldType::eCHAR:
				doSetValue( static_cast< ValueT< FieldType::eCHAR > const & >( value ).getValue() );
				break;

			case FieldType::eVARCHAR:
				doSetValue( static_cast< ValueT< FieldType::eVARCHAR > const & >( value ).getValue() );
				break;

			case FieldType::eTEXT:
				doSetValue( static_cast< ValueT< FieldType::eTEXT > const & >( value ).getValue() );
				break;

			case FieldType::eDATE:
				doSetValue( static_cast< ValueT< FieldType::eDATE > const & >( value ).getValue() );
				break;

			case FieldType::eDATETIME:
				doSetValue( static_cast< ValueT< FieldType::eDATETIME > const & >( value ).getValue() );
				break;

			case FieldType::eTIME:
				doSetValue( static_cast< ValueT< FieldType::eTIME > const & >( value ).getValue() );
				break;

			case FieldType::eBINARY:
				doSetValue( static_cast< ValueT< FieldType::eBINARY > const & >( value ).getValue() );
				break;

			case FieldType::eVARBINARY:
				doSetValue( static_cast< ValueT< FieldType::eVARBINARY > const & >( value ).getValue() );
				break;

			case FieldType::eBLOB:
				doSetValue( static_cast< ValueT< FieldType::eBLOB > const & >( value ).getValue() );
				break;

			default:
				throw std::runtime_error{ ERROR_DB_PARAMETER_SETVALUE_TYPE + this->getName() };
				break;
			}

			m_updater->update( *this );
		}
	}
}

