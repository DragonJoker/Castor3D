/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ShaderPassBufferModule_H___
#define ___C3D_ShaderPassBufferModule_H___

#include "Castor3D/Shader/ShaderModule.hpp"

namespace castor3d
{
	/**@name Shader */
	//@{
	/**@name PassBuffer */
	//@{

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
	*	ShaderBuffer holding the Metallic Roughness PBR passes data.
	*\~french
	*\brief
	*	ShaderBuffer contenant les données des passes PBR Metallic Roughness.
	*/
	class MetallicRoughnessPassBuffer;
	/**
	*\~english
	*\brief
	*	ShaderBuffer holding the Phong passes data.
	*\~french
	*\brief
	*	ShaderBuffer contenant les données des passes Phong.
	*/
	class PhongPassBuffer;
	/**
	*\~english
	*\brief
	*	ShaderBuffer holding the Specular Glossiness PBR passes data.
	*\~french
	*\brief
	*	ShaderBuffer contenant les données des passes PBR Specular Glossiness.
	*/
	class SpecularGlossinessPassBuffer;

	//@}
	//@}
}

#endif
