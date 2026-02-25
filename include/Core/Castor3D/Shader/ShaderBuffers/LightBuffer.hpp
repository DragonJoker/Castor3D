/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightBuffer_H___
#define ___C3D_LightBuffer_H___

#include "Castor3D/Scene/Light/LightModule.hpp"

#include "Castor3D/Shader/ShaderBuffer.hpp"

#include <CastorUtils/Design/ArrayView.hpp>

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <mutex>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

namespace c3d
{
	class LightBuffer
		: public ShaderBufferHolder
	{
	public:
		using LightsData = ArrayView< Point4f >;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	device	The GPU device.
		 *\param[in]	count	The max passes count.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	device	Le device GPU.
		 *\param[in]	count	Le nombre maximal de passes.
		 */
		C3D_API LightBuffer( RenderDevice const & device
		, crg::ResourcesCache & resources
			, uint32_t count );
		/**
		 *\~english
		 *\brief		Adds a light source to the buffer.
		 *\param[in]	light	The light source.
		 *\~french
		 *\brief		Ajoute une source lumineuse au tampon.
		 *\param[in]	light	La source lumineuse.
		 */
		C3D_API void addLight( LightInstance & light );
		/**
		 *\~english
		 *\brief		Removes a light source from the buffer.
		 *\param[in]	light	The light source.
		 *\~french
		 *\brief		Supprime une source lumineuse du tampon.
		 *\param[in]	light	La source lumineuse.
		 */
		C3D_API void removeLight( LightInstance & light );
		/**
		 *\~english
		 *\brief			Updates the buffer CPU wise.
		 *\param[in,out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour le tampon au niveau CPU.
		 *\param[in,out]	updater	Les données de mise à jour.
		 */
		C3D_API void update( CpuUpdater const & updater );
		/**
		 *\~english
		 *\brief			Uploads the buffer to VRAM.
		 *\param[in,out]	uploader	Receives the upload requests.
		 *\~french
		 *\brief			Uploade le tampon en VRAM.
		 *\param[in,out]	uploader	Reçoit les requêtes d'upload.
		 */
		C3D_API void upload( UploadData & uploader );
		/**
		 *\~english
		 *\param[in]	type	The light type.
		 *\return		The number of light sources of the given type, in the buffer.
		 *\~french
		 *\brief		Crée le descriptor write pour le buffer de sources lumineuses.
		 *\param[in]	type	Le type de lumière.
		 *\return		Le nombre de sources lumineuses du type donné, dans le buffer.
		 */
		C3D_API uint32_t getLightsBufferCount( LightType type )const noexcept;

		LightInstancesArray getLightInstances( LightType type )const
		{
			return m_typeSortedLights[size_t( type )];
		}

	private:
		Pair< uint32_t, uint32_t > doGetOffsetIndex( LightInstance const & light )const;
		void doMarkNextDirty( LightType type
			, uint32_t index );
		uint32_t doGetBufferEnd( LightType type )const noexcept;

	private:
		Array< LightInstancesArray, size_t( LightType::eCount ) > m_typeSortedLights;
		Vector< LightInstance * > m_dirty;
		Map< LightInstance *, OnLightChangedConnection > m_connections;
		Vector< uint32_t > m_lightSizes;
		LightsData m_data;
		mutable Mutex m_mutex;
		bool m_wasDirty{};
	};
}

#endif
