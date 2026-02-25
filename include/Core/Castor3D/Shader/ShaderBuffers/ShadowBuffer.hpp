/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ShadowBuffer_H___
#define ___C3D_ShadowBuffer_H___

#include "Castor3D/Scene/Light/LightModule.hpp"

#include "Castor3D/Shader/Ubos/Ubo.hpp"

namespace c3d
{
	class ShadowBuffer
		: public UboT< AllShadowData >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	device	The GPU device.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	device	Le device GPU.
		 */
		C3D_API explicit ShadowBuffer( RenderDevice const & device );
		/**
		 *\~english
		 *\return		The buffer data.
		 *\~french
		 *\brief		Lees données du buffer.
		 */
		auto & getData()
		{
			return UboT< AllShadowData >::getData();
		}
	};
}

#endif
