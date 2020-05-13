/*
See LICENSE file in root folder
*/
#ifndef ___C3D_OpaquePassResult_H___
#define ___C3D_OpaquePassResult_H___

#include "OpaqueModule.hpp"

#include "Castor3D/Render/GBuffer.hpp"

namespace castor3d
{
	class OpaquePassResult
		: public GBuffer
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
		C3D_API OpaquePassResult( Engine & engine
			, ashes::Image const & depthTexture
			, ashes::Image const & velocityTexture );

	private:
		std::vector< ashes::ImagePtr > m_owned;
	};
}

#endif
