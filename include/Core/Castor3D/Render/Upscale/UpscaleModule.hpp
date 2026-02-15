/*
See LICENSE file in root folder
*/
#ifndef ___C3D_UpscaleModule_H___
#define ___C3D_UpscaleModule_H___

#include "Castor3D/Castor3DModule.hpp"

#include <CastorUtils/FileParser/FileParserModule.hpp>

namespace c3d
{

	enum class UpscalingPerfQualityMode
	{
		eMaxPerf,
		eBalanced,
		eMaxQuality,
		eUltraPerformance,
		eUltraQuality,
		eDLAA,
		CU_ScopedEnumBounds( eMaxPerf, eDLAA )
	};
	C3D_API String getName( UpscalingPerfQualityMode v );

	enum class UpscalingRenderPreset
	{
		eDefault,
		eJ,
		eK,
		eL,
		eM,
		CU_ScopedEnumBounds( eDefault, eM )
	};
	C3D_API String getName( UpscalingRenderPreset v );
	/**
	*\~english
	*\brief
	*	Holds upscaling configuration.
	*\~french
	*\brief
	*	Contient la configuration d'upscaling.
	*/
	struct UpscaleConfig
	{
		bool enabled{ false };
		UpscalingPerfQualityMode perfQualityMode{ UpscalingPerfQualityMode::eMaxPerf };
		UpscalingRenderPreset preset{ UpscalingRenderPreset::eDefault };
		u32 basePhaseCount{ 8u };
	};
	/**
	*\~english
	*\brief
	*	Wraps DLSS or other upscaling SDK.
	*\~french
	*\brief
	*	Enrobe le DLSS ou tout autre SDK d'upscaling.
	*/
	class UpscalingWrapper;
	/**
	*\~english
	*\brief
	*	Wraps DLSS or other upscaling SDK.
	*\~french
	*\brief
	*	Encapsule le DLSS ou tout autre SDK d'upscaling.
	*/
	class UpscalingInstance;
	/**
	*\~english
	*\brief
	*	Frame graph pass for upscaling.
	*\~french
	*\brief
	*	La psse de frame graph pour l'upscaling.
	*/
	class UpscalingFramePass;

	/** @cond !Doxygen */
	CU_DeclareSmartPtr( c3d, UpscalingInstance, C3D_API );
	CU_DeclareSmartPtr( c3d, UpscalingWrapper, C3D_API );
	/** @endcond */

	template<>
	struct ParserEnumTraits< UpscalingPerfQualityMode >
	{
		static inline xchar const * const Name = cuT( "UpscalingPerfQualityMode" );
		static inline UInt32StrMap const Values = []()
			{
				UInt32StrMap result;
				result = getEnumMapT< UpscalingPerfQualityMode >();
				return result;
			}();
	};
}

#endif
