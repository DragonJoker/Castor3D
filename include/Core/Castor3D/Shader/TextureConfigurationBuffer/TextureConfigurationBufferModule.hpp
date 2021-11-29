/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ShaderTextureConfigurationBufferModule_H___
#define ___C3D_ShaderTextureConfigurationBufferModule_H___

#include "Castor3D/Shader/ShaderModule.hpp"

namespace castor3d
{
	/**@name Shader */
	//@{
	/**@name TextureConfigurationBuffer */
	//@{

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

	CU_DeclareSmartPtr( TextureConfigurationBuffer );
	CU_DeclareSmartPtr( TextureAnimationBuffer );

	//@}
	//@}
}

#endif
