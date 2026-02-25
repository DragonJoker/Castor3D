/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextureAnimationBuffer_H___
#define ___C3D_TextureAnimationBuffer_H___

#include "Castor3D/Material/Texture/Animation/TextureAnimationModule.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Scene/Animation/AnimationModule.hpp"
#include "Castor3D/Shader/ShaderBuffer.hpp"

#include <CastorUtils/Design/ArrayView.hpp>

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <atomic>
#include <mutex>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

namespace c3d
{
	class TextureAnimationBuffer
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
		C3D_API TextureAnimationBuffer( Engine & engine
			, RenderDevice const & device
			, uint32_t count );
		/**
		 *\~english
		 *\brief		Adds a configuration to the buffer.
		 *\param[in]	texture	The texture.
		 *\~french
		 *\brief		Ajoute une configuration au tampon.
		 *\param[in]	texture	La texture.
		 */
		C3D_API void addTextureAnimation( AnimatedTexture const & texture );
		/**
		 *\~english
		 *\brief		Removes a configuration from the buffer.
		 *\param[in]	texture	The texture.
		 *\~french
		 *\brief		Supprime une configuration du tampon.
		 *\param[in]	texture	La texture.
		 */
		C3D_API void removeTextureAnimation( AnimatedTexture const & texture )noexcept;
		/**
		 *\~english
		 *\brief		Updates the configurations buffer.
		 *\~french
		 *\brief		Met à jour le tampon de configurations.
		 */
		C3D_API void update( UploadData & uploader );

	public:
		using TextureAnimationsData = ArrayView< TextureAnimationData >;
		static uint32_t constexpr DataSize = uint32_t( sizeof( TextureAnimationData ) );

	private:
		TextureAnimationsData m_data;
		Vector< AnimatedTexture const * > m_animations;
		Mutex m_mutex;
		std::atomic_uint32_t m_count{ 0u };
	};
}

#endif
