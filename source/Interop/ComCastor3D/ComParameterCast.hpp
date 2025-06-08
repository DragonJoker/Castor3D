/* See LICENSE file in root folder */
#ifndef ___C3DCOM_COM_PARAMETER_CAST_H___
#define ___C3DCOM_COM_PARAMETER_CAST_H___

#include "ComCastor3D/ComUtils.hpp"

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
			|| areCompatibleV< SrcT, DstT, ePIXEL_FORMAT, C3D_PIXEL_FORMAT >
			|| areCompatibleV< SrcT, DstT, eLIGHT_TYPE, C3D_LIGHT_TYPE >
			|| areCompatibleV< SrcT, DstT, eVIEWPORT_TYPE, C3D_VIEWPORT_TYPE >
			|| areCompatibleV< SrcT, DstT, eKEYBOARD_KEY, C3D_KEYBOARD_KEY >
			|| areCompatibleV< SrcT, DstT, eOVERLAY_TYPE, C3D_OVERLAY_TYPE >
			|| areCompatibleV< SrcT, DstT, eSHADOW_TYPE, C3D_SHADOW_TYPE >
			|| areCompatibleV< SrcT, DstT, eBORDER_COLOUR, C3D_BORDER_COLOUR >
			|| areCompatibleV< SrcT, DstT, eFILTER_MODE, C3D_FILTER_MODE >
			|| areCompatibleV< SrcT, DstT, eIMAGE_TYPE, C3D_IMAGE_TYPE >
			|| areCompatibleV< SrcT, DstT, eINDEX_MAPPING_TYPE, C3D_INDEX_MAPPING_TYPE >
			|| areCompatibleV< SrcT, DstT, eMIPMAP_MODE, C3D_MIPMAP_MODE >
			|| areCompatibleV< SrcT, DstT, eCOMPARISON_FUNC, C3D_COMPARISON_FUNC >
			|| areCompatibleV< SrcT, DstT, eWRAP_MODE, C3D_WRAP_MODE > );
		return DstT( src );
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
	inline BSTR parameterCast< BSTR, std::string >( std::string const & src )
	{
		return stringToBstr( src );
	}
	template<>
	inline std::string parameterCast< std::string, BSTR >( BSTR const & src )
	{
		return bstrToString( src );
	}
}

#endif
