/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DeferredLightPass_H___
#define ___C3D_DeferredLightPass_H___

#include "LightingModule.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapModule.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"

namespace castor3d
{
	class LightPass
	{
	public:
		/**
		 *\~english
		 *\brief		Retrieves the vertex shader source for a light type.
		 *\param[in]	lightType	The light type.
		 *\return		The source.
		 *\~french
		 *\brief		Récupère le source du vertex shader pour un type de source lumineuse.
		 *\param[in]	lightType	Les indicateurs de scène.
		 *\return		Le source.
		 */
		static ShaderPtr getVertexShaderSource( LightType lightType );
		/**
		 *\~english
		 *\brief		Retrieves the pixel shader source for a clustered lighting.
		 *\param[in]	lightingModelId		The lighting model ID.
		 *\param[in]	backgroundModelId	The background model ID.
		 *\param[in]	scene				The scene.
		 *\param[in]	debugConfig			The debug data.
		 *\param[in]	sceneFlags			The scene flags.
		 *\param[in]	targetSize			The target image size.
		 *\return		The source.
		 *\~french
		 *\brief		Récupère le source du fragment shader pour l'éclairage par clusters.
		 *\param[in]	lightingModelId		L'ID de modèle d'éclairage.
		 *\param[in]	backgroundModelId	L'ID de modèle de fond.
		 *\param[in]	scene				La scène.
		 *\param[in]	debugConfig			Les données de debug.
		 *\param[in]	sceneFlags			Les indicateurs de scène.
		 *\param[in]	targetSize			Les dimensions de l'image cible.
		 *\return		Le source.
		 */
		static ShaderPtr getPixelShaderSource( LightingModelID lightingModelId
			, BackgroundModelID backgroundModelId
			, Scene const & scene
			, DebugConfig & debugConfig
			, SceneFlags const & sceneFlags
			, VkExtent2D const & targetSize );
		/**
		 *\~english
		 *\brief		Retrieves the pixel shader source for a light type.
		 *\param[in]	lightingModelId		The lighting model ID.
		 *\param[in]	backgroundModelId	The background model ID.
		 *\param[in]	scene				The scene.
		 *\param[in]	debugConfig			The debug data.
		 *\param[in]	lightType			The light source type.
		 *\param[in]	shadowType			The shadow type.
		 *\param[in]	shadows				Tells if shadows are used.
		 *\param[in]	targetSize			The target image size.
		 *\return		The source.
		 *\~french
		 *\brief		Récupère le source du fragment shader pour un type de source lumineuse.
		 *\param[in]	lightingModelId		L'ID de modèle d'éclairage.
		 *\param[in]	backgroundModelId	L'ID de modèle de fond.
		 *\param[in]	scene				La scène.
		 *\param[in]	debugConfig			Les données de debug.
		 *\param[in]	sceneFlags			Les indicateurs de scène.
		 *\param[in]	lightType			Le type de source lumineuse.
		 *\param[in]	shadowType			Le type d'ombres.
		 *\param[in]	shadows				Dit si les ombres sont utilisées.
		 *\param[in]	targetSize			Les dimensions de l'image cible.
		 *\return		Le source.
		 */
		static ShaderPtr getPixelShaderSource( LightingModelID lightingModelId
			, BackgroundModelID backgroundModelId
			, Scene const & scene
			, DebugConfig & debugConfig
			, LightType lightType
			, ShadowType shadowType
			, bool shadows
			, VkExtent2D const & targetSize );
	};
}

#endif
