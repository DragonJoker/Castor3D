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
		 *\brief		Retrieves the vertex shader source for this light pass.
		 *\param[in]	sceneFlags	The scene flags.
		 *\return		The source.
		 *\~french
		 *\brief		Récupère le source du vertex shader pour cette passe lumineuse.
		 *\param[in]	sceneFlags	Les indicateurs de scène.
		 *\return		Le source.
		 */
		static ShaderPtr getVertexShaderSource( LightType lightType );
		/**
		 *\~english
		 *\brief		Retrieves the pixel shader source for this light pass.
		 *\param[in]	sceneFlags	The scene flags.
		 *\param[in]	lightType	The light source type.
		 *\param[in]	shadowType	The shadow type.
		 *\param[in]	rsm			Tells if RSM are to be generated.
		 *\return		The source.
		 *\~french
		 *\brief		Récupère le source du pixel shader pour cette passe lumineuse.
		 *\param[in]	sceneFlags	Les indicateurs de scène.
		 *\param[in]	lightType	Le type de source lumineuse.
		 *\param[in]	shadowType	Le type d'ombres.
		 *\param[in]	rsm			Dit si les RSM doivent être générées.
		 *\return		Le source.
		 */
		static ShaderPtr getPixelShaderSource( MaterialType materialType
			, RenderSystem const & renderSystem
			, SceneFlags const & sceneFlags
			, LightType lightType
			, ShadowType shadowType
			, bool shadows );
	};
}

#endif
