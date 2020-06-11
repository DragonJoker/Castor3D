/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ShadowMapResult_H___
#define ___C3D_ShadowMapResult_H___

#include "ShadowMapModule.hpp"

#include "Castor3D/Render/GBuffer.hpp"

namespace castor3d
{
	class ShadowMapResult
		: public GBufferT< SmTexture >
	{
	public:
		/**
		*\~english
		*\brief
		*	Initialises deferred rendering related stuff.
		*\param[in] engine
		*	The engine.
		*\param[in] depthTexture
		*	The depth texture.
		*\param[in] velocityTexture
		*	The velocity texture.
		*\~french
		*\brief
		*	Initialise les données liées au deferred rendering.
		*\param[in] engine
		*	Le moteur.
		*\param[in] depthTexture
		*	La texture de profondeur.
		*\param[in] velocityTexture
		*	La texture de vélocité.
		*/
		C3D_API ShadowMapResult( Engine & engine
			, castor::String const & prefix
			, VkImageCreateFlags createFlags
			, castor::Size const & size
			, uint32_t layerCount );

	private:
		TextureUnitArray m_owned;
	};
}

#endif
