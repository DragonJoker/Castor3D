/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextureConfigurationBuffer_H___
#define ___C3D_TextureConfigurationBuffer_H___

#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Shader/ShaderBuffer.hpp"

#include <CastorUtils/Design/ArrayView.hpp>

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <mutex>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

namespace c3d
{
	class TextureConfigurationBuffer
		: public ShaderBufferHolder
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\param[in]	device	The GPU device.
		 *\param[in]	count	The max configurations count.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 *\param[in]	device	Le device GPU.
		 *\param[in]	count	Le nombre maximal de configurations.
		 */
		C3D_API TextureConfigurationBuffer( Engine & engine
			, RenderDevice const & device
			, uint32_t count );
		/**
		 *\~english
		 *\brief		Adds a configuration to the buffer.
		 *\param[in]	unit	The texture.
		 *\~french
		 *\brief		Ajoute une configuration au tampon.
		 *\param[in]	unit	La texture.
		 */
		C3D_API uint32_t addTextureConfiguration( TextureUnit & unit );
		/**
		 *\~english
		 *\brief		Removes a configuration from the buffer.
		 *\param[in]	unit	The texture.
		 *\~french
		 *\brief		Supprime une configuration du tampon.
		 *\param[in]	unit	La texture.
		 */
		C3D_API void removeTextureConfiguration( TextureUnit & unit )noexcept;
		/**
		 *\~english
		 *\brief		Updates the configurations buffer.
		 *\~french
		 *\brief		Met à jour le tampon de configurations.
		 */
		C3D_API void update( UploadData & uploader );

	public:
		struct Data
		{
			Point3f translate{};
			float rotateU{};
			Point3f scale{};
			float rotateV{};
			Point4f tileSet{};
			float normalGMult{};
			float normalFactor{};
			uint32_t normal2Chan{};
			float heightFactor{};
			uint32_t isTransformAnim{};
			uint32_t isTileAnim{};
			uint32_t needsYInv{};
			uint32_t texcoordSet{};
			uint32_t needsXInv{};
			uint32_t needsZInv{};
			uint32_t pad0{};
			uint32_t pad1{};
		};

		using TextureConfigurationsData = ArrayView< Data >;

		static uint32_t constexpr DataSize = uint32_t( sizeof( Data ) );
		static_assert( DataSize == 96u );

	private:
		Vector< TextureConfiguration > m_configurations;
		Vector< TextureUnit const * > m_dirty;
		Vector< OnTextureUnitChangedConnection > m_connections;
		TextureConfigurationsData m_data;
		Mutex m_mutex;
	};
}

#endif
