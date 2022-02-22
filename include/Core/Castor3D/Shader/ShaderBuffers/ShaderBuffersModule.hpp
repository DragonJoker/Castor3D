/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ShaderTextureConfigurationBufferModule_H___
#define ___C3D_ShaderTextureConfigurationBufferModule_H___

#include "Castor3D/Shader/ShaderModule.hpp"

#include <CastorUtils/Design/Signal.hpp>

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
	CU_DeclareCUSmartPtr( castor3d, TextureConfigurationBuffer, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, TextureAnimationBuffer, C3D_API );

	//@}
	//@}
}

#endif
