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
		 *\brief		Retrieves the pixel shader source for a light type.
		 *\param[in]	passType		The material pass type ID.
		 *\param[in]	renderSystem	The render system.
		 *\param[in]	sceneFlags		The scene flags.
		 *\param[in]	lightType		The light source type.
		 *\param[in]	shadowType		The shadow type.
		 *\param[in]	shadows			Tells if shadows are used.
		 *\return		The source.
		 *\~french
		 *\brief		Récupère le source du fragment shader pour un type de source lumineuse.
		 *\param[in]	passType		L'ID du type de material pass.
		 *\param[in]	renderSystem	Le render system.
		 *\param[in]	sceneFlags		Les indicateurs de scène.
		 *\param[in]	lightType		Le type de source lumineuse.
		 *\param[in]	shadowType		Le type d'ombres.
		 *\param[in]	shadows			Dit si les ombres sont utilisées.
		 *\return		Le source.
		 */
		static ShaderPtr getPixelShaderSource( PassTypeID passType
			, RenderSystem const & renderSystem
			, SceneFlags const & sceneFlags
			, LightType lightType
			, ShadowType shadowType
			, bool shadows );
	};
}

#endif
