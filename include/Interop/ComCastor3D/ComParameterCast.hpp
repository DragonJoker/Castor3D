/* See LICENSE file in root folder */
#ifndef ___C3DCOM_COM_PARAMETER_CAST_H___
#define ___C3DCOM_COM_PARAMETER_CAST_H___

#include "ComCastor3D/ComCastor3DPrerequisites.hpp"
#include "ComCastor3D/ComUtils.hpp"

#include <CastorUtils/Math/RangedValue.hpp>
#include <CastorUtils/Graphics/ColourComponent.hpp>

namespace CastorCom::details
{
	template< typename LhsT, typename RhsT, typename LhsU, typename RhsU >
	static inline constexpr bool areCompatibleV = ( std::is_same_v< LhsT, LhsU > && std::is_same_v< RhsT, RhsU > )
		|| ( std::is_same_v< LhsT, RhsU > && std::is_same_v< RhsT, LhsU > );

	template< typename DstT, typename SrcT >
	inline DstT parameterCast( SrcT const & src )
	{
		static_assert( std::is_same_v< DstT, SrcT >
			|| areCompatibleV< SrcT, DstT, uint32_t, UINT >
			|| areCompatibleV< SrcT, DstT, uint16_t, UINT >
			|| areCompatibleV< SrcT, DstT, FLOAT, float >
			|| areCompatibleV< SrcT, DstT, FLOAT, double >
			|| areCompatibleV< SrcT, DstT, FLOAT, castor::ColourComponent >
			|| areCompatibleV< SrcT, DstT, FLOAT, castor::HdrColourComponent >
			|| areCompatibleV< SrcT, DstT, ePIXEL_FORMAT, castor::PixelFormat >
			|| areCompatibleV< SrcT, DstT, eMOVABLE_TYPE, castor3d::MovableType >
			|| areCompatibleV< SrcT, DstT, eLIGHT_TYPE, castor3d::LightType >
			|| areCompatibleV< SrcT, DstT, eVIEWPORT_TYPE, castor3d::ViewportType >
			|| areCompatibleV< SrcT, DstT, eBORDER_COLOUR, VkBorderColor >
			|| areCompatibleV< SrcT, DstT, eFILTER_MODE, VkFilter >
			|| areCompatibleV< SrcT, DstT, eIMAGE_TYPE, VkImageType >
			|| areCompatibleV< SrcT, DstT, eMIPMAP_MODE, VkSamplerMipmapMode >
			|| areCompatibleV< SrcT, DstT, eWRAP_MODE, VkSamplerAddressMode >
			|| areCompatibleV< SrcT, DstT, ePIXEL_FORMAT, VkFormat > );
		return DstT( src );
	}

	template< ComITypeT ITypeT >
	inline ComITypeGetDstTypeT< ITypeT > parameterCast( ITypeT * src )
	{
		return ComITypeGetDstTypeT< ITypeT >( static_cast< ComITypeCTypeT< ITypeT > * >( src )->getInternal() );
	}
	template< ComTypeT TypeT >
	inline ComTypeITypeT< TypeT > * parameterCast( ComTypeInternalMbrT< TypeT > src )
	{
		ComTypeITypeT< TypeT > ** result{};
		auto hr = ComTypeCTypeT< TypeT >::CreateInstance( result );

		if ( hr == S_OK )
		{
			static_cast< ComTypeCTypeT< TypeT > * >( *result )->setInternal( ComTypeSetInternalT< TypeT >( src ) );
		}

		return *result;
	}

	template< typename DstT, typename SrcT >
	inline DstT parameterCast( castor::RangedValue< SrcT > const & src )
	{
		return parameterCast< DstT >( src.value() );
	}

	template<>
	inline bool parameterCast< bool, boolean >( boolean const & src )
	{
		return src != 0;
	}
	template<>
	inline boolean parameterCast< boolean, bool >( bool const & src )
	{
		return src ? 1 : 0;
	}

	template<>
	inline BSTR parameterCast< BSTR, castor::String >( castor::String const & src )
	{
		return toBstr( src );
	}
	template<>
	inline castor::String parameterCast< castor::String, BSTR >( BSTR const & src )
	{
		return fromBstr( src );
	}

	template<>
	inline BSTR parameterCast< BSTR, castor::Path >( castor::Path const & src )
	{
		return toBstr( src );
	}
	template<>
	inline castor::Path parameterCast< castor::Path, BSTR >( BSTR const & src )
	{
		return castor::Path{ fromBstr( src ) };
	}
}

#endif
