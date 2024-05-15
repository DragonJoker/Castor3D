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
	*	ShaderBuffer holding the light sources shadow data.
	*\~french
	*\brief
	*	ShaderBuffer contenant les données d'ombres des sources lumineuses.
	*/
	class ShadowBuffer;
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
	using GpuDataBufferOffsetPtr = castor::RawUniquePtr< GpuDataBufferOffset >;

	using GpuDataBufferOffsetModifyFunc = castor::Function< void( GpuDataBufferOffset const & ) >;
	using GpuDataBufferOffsetModifySignal = castor::SignalT< GpuDataBufferOffsetModifyFunc >;
	using GpuDataBufferOffsetModifyConnection = castor::ConnectionT< GpuDataBufferOffsetModifySignal >;

	CU_DeclareSmartPtr( castor3d, LightBuffer, C3D_API );
	CU_DeclareSmartPtr( castor3d, PassBuffer, C3D_API );
	CU_DeclareSmartPtr( castor3d, ShadowBuffer, C3D_API );
	CU_DeclareSmartPtr( castor3d, SssProfileBuffer, C3D_API );
	CU_DeclareSmartPtr( castor3d, TextureConfigurationBuffer, C3D_API );
	CU_DeclareSmartPtr( castor3d, TextureAnimationBuffer, C3D_API );

	struct ShaderBufferTypes
	{
		using Float1 = float;

		template< typename DataT >
		struct Data2T
		{
			Data2T & operator=( DataT rhs )
			{
				x = rhs;
				return *this;
			}

			Data2T & operator=( castor::Point2< DataT > const & rhs )
			{
				x = rhs->x;
				y = rhs->y;
				return *this;
			}

			DataT x{};
			DataT y{};
		};

		using Float2 = Data2T< Float1 >;

		template< typename DataT >
		struct Data3T
		{
			Data3T & operator=( DataT rhs )
			{
				x = rhs;
				return *this;
			}

			Data3T & operator=( castor::Point2< DataT > const & rhs )
			{
				x = rhs->x;
				y = rhs->y;
				return *this;
			}

			Data3T & operator=( castor::Point3< DataT > const & rhs )
			{
				x = rhs->x;
				y = rhs->y;
				z = rhs->z;
				return *this;
			}

			DataT x{};
			DataT y{};
			DataT z{};
		};

		using Float3 = Data3T< Float1 >;

		template< typename DataT >
		struct Data4T
		{
			Data4T & operator=( DataT rhs )
			{
				x = rhs;
				return *this;
			}

			Data4T & operator=( castor::Point2< DataT > const & rhs )
			{
				x = rhs->x;
				y = rhs->y;
				return *this;
			}

			Data4T & operator=( castor::Point3< DataT > const & rhs )
			{
				x = rhs->x;
				y = rhs->y;
				z = rhs->z;
				return *this;
			}

			Data4T & operator=( castor::Point4< DataT > const & rhs )
			{
				x = rhs->x;
				y = rhs->y;
				z = rhs->z;
				w = rhs->w;
				return *this;
			}

			DataT x{};
			DataT y{};
			DataT z{};
			DataT w{};
		};

		using Float4 = Data4T< Float1 >;

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
	};

	//@}
	//@}
}

#endif
