/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ShaderTextureConfigurationBufferModule_H___
#define ___C3D_ShaderTextureConfigurationBufferModule_H___

#include "Castor3D/Shader/ShaderModule.hpp"

#include <CastorUtils/Design/Signal.hpp>
#include <CastorUtils/Math/SquareMatrix.hpp>

namespace castor3d
{
	/**@name Shader */
	//@{
	/**@name Buffers */
	//@{

	/**
	*\~english
	*\brief
	*	ShaderBuffer holding the light sources data.
	*\~french
	*\brief
	*	ShaderBuffer contenant les données des sources lumineuses.
	*/
	class LightBuffer;
	/**
	*\~english
	*\brief
	*	ShaderBuffer holding the Passes data.
	*\~french
	*\brief
	*	ShaderBuffer contenant les données des Pass.
	*/
	class PassBuffer;
	/**
	*\~english
	*\brief
	*	ShaderBuffer holding the Subsurface Scattering profiles data.
	*\~french
	*\brief
	*	ShaderBuffer contenant les données des profils de Subsurface Scattering.
	*/
	class SssProfileBuffer;
	/**
	*\~english
	*\brief
	*	ShaderBuffer holding the TextureConfigurations data.
	*\~french
	*\brief
	*	ShaderBuffer contenant les données des TextureConfiguration.
	*/
	class TextureConfigurationBuffer;
	/**
	*\~english
	*\brief
	*	ShaderBuffer holding the TextureAnimations data.
	*\~french
	*\brief
	*	ShaderBuffer contenant les données des TextureAnimation.
	*/
	class TextureAnimationBuffer;

	struct GpuDataBufferOffset;
	using GpuDataBufferOffsetPtr = std::unique_ptr< GpuDataBufferOffset >;

	using GpuDataBufferOffsetModifyFunc = std::function< void( GpuDataBufferOffset const & ) >;
	using GpuDataBufferOffsetModifySignal = castor::SignalT< GpuDataBufferOffsetModifyFunc >;
	using GpuDataBufferOffsetModifyConnection = castor::ConnectionT< GpuDataBufferOffsetModifySignal >;


	CU_DeclareCUSmartPtr( castor3d, LightBuffer, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, PassBuffer, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, SssProfileBuffer, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, TextureConfigurationBuffer, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, TextureAnimationBuffer, C3D_API );

	struct LightBufferData
	{
		struct Float2
		{
			Float2 & operator=( float rhs )
			{
				x = rhs;
				return *this;
			}

			Float2 & operator=( castor::Point2f const & rhs )
			{
				x = rhs->x;
				y = rhs->y;
				return *this;
			}

			float x{};
			float y{};
		};

		struct Float3
		{
			Float3 & operator=( float rhs )
			{
				x = rhs;
				return *this;
			}

			Float3 & operator=( castor::Point2f const & rhs )
			{
				x = rhs->x;
				y = rhs->y;
				return *this;
			}

			Float3 & operator=( castor::Point3f const & rhs )
			{
				x = rhs->x;
				y = rhs->y;
				z = rhs->z;
				return *this;
			}

			float x{};
			float y{};
			float z{};
		};

		struct Float4
		{
			Float4 & operator=( float rhs )
			{
				x = rhs;
				return *this;
			}

			Float4 & operator=( castor::Point2f const & rhs )
			{
				x = rhs->x;
				y = rhs->y;
				return *this;
			}

			Float4 & operator=( castor::Point3f const & rhs )
			{
				x = rhs->x;
				y = rhs->y;
				z = rhs->z;
				return *this;
			}

			Float4 & operator=( castor::Point4f const & rhs )
			{
				x = rhs->x;
				y = rhs->y;
				z = rhs->z;
				w = rhs->w;
				return *this;
			}

			float x{};
			float y{};
			float z{};
			float w{};
		};

		struct Float4x4
		{
			Float4x4 & operator=( castor::Matrix4x4f const & rhs )
			{
				c0 = rhs[0];
				c1 = rhs[1];
				c2 = rhs[2];
				c3 = rhs[3];
				return *this;
			}

			Float4 c0{};
			Float4 c1{};
			Float4 c2{};
			Float4 c3{};
		};

		union SpecLight
		{
			struct Directional
			{
				Float3 direction;
				float cascadeCount;
				Float4 splitDepths;
				Float4 splitScales;
				std::array< Float4x4, 4u > transforms;
			} directional;
			struct Point
			{
				Float3 position;
				float pad0;
				Float3 attenuation;
				float pad1;
				Float4 pad2;
				std::array< Float4x4, 4u > pad3;
			} point;
			struct Spot
			{
				Float3 position;
				float exponent;
				Float3 attenuation;
				float innerCutoff;
				Float3 direction;
				float outerCutoff;
				Float4x4 transform;
				std::array< Float4x4, 3u > pad;
			} spot;
		};

		Float3 colour;
		float shadowMapIndex;
		Float2 intensity;
		float farPlane;
		float shadowType;
		Float2 rawShadowsOffsets;
		Float2 pcfShadowsOffsets;
		Float2 vsmShadowVariance;
		float volumetricSteps;
		float volumetricScattering;
		SpecLight specific;
	};
	static_assert( sizeof( LightBufferData::SpecLight::Directional ) == sizeof( LightBufferData::SpecLight::Spot ) );
	static_assert( sizeof( LightBufferData::SpecLight::Directional ) == sizeof( LightBufferData::SpecLight::Point ) );
	static constexpr uint32_t LightBufferDataSize = sizeof( LightBufferData );

	//@}
	//@}
}

#endif
