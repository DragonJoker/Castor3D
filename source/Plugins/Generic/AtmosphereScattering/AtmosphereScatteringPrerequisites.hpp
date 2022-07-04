/*
See LICENSE file in root folder
*/
#ifndef ___C3DAS_AtmosphereScatteringPrerequisites_H___
#define ___C3DAS_AtmosphereScatteringPrerequisites_H___

#include <Castor3D/Castor3DModule.hpp>
#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <RenderGraph/FrameGraphPrerequisites.hpp>

namespace atmosphere_scattering
{
	class AtmosphereBackground;
	class AtmosphereBackgroundModel;
	class AtmosphereScatteringUbo;
	class CameraUbo;
	class CloudsUbo;
	class WeatherUbo;

	struct AtmosphereModel;
	struct CloudsModel;
	struct ScatteringModel;

	struct AtmosphereData;
	struct CameraData;
	struct CloudsData;
	struct WeatherData;

	template< template< typename DataT > typename WrapperT >
	struct AtmosphereScatteringConfigT;
	using AtmosphereScatteringConfig = AtmosphereScatteringConfigT< crg::RawTypeT >;
	using CheckedAtmosphereScatteringConfig = AtmosphereScatteringConfigT< castor::GroupChangeTracked >;

	template< template< typename DataT > typename WrapperT >
	struct CloudsConfigT;
	using CloudsConfig = CloudsConfigT< crg::RawTypeT >;
	using CheckedCloudsConfig = CloudsConfigT< castor::GroupChangeTracked >;

	template< template< typename DataT > typename WrapperT >
	struct WeatherConfigT;
	using WeatherConfig = WeatherConfigT< crg::RawTypeT >;
	using CheckedWeatherConfig = WeatherConfigT< castor::GroupChangeTracked >;
}

#endif
