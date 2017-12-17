/*
See LICENSE file in root folder
*/
#ifndef ___GL_DIRECT_TEXTURE_STORAGE_H___
#define ___GL_DIRECT_TEXTURE_STORAGE_H___

#include "GlRenderSystemPrerequisites.hpp"

namespace GlRender
{
#if !defined( NDEBUG )
#	define GlDebug_Decl( type, name )\
	type name;
#	define GlDebug_Set( name, value )\
	name = value;
#	define GlDebug_Check( name, value )\
	REQUIRE( name == value );
#else
#	define GlDebug_Decl( type, name )
#	define GlDebug_Set( name, value )
#	define GlDebug_Check( name, value )
#endif
	/*!
	\author		Sylvain doremus.
	\version	0.8.0
	\date		12/10/2015
	\brief		Class used to handle texture storage buffer, using directly the texture (for static textures).
	*/
	class GlDirectTextureStorageTraits
	{
	public:
		/**
		 *\brief		Constructor.
		 */
		explicit GlDirectTextureStorageTraits( castor3d::TextureStorage & p_storage );
		/**
		 *\brief		Destructor.
		 */
		~GlDirectTextureStorageTraits();
		/**
		 *\copydoc		castor3d::TextureStorage::Bind
		 */
		void bind( castor3d::TextureStorage const & p_storage, uint32_t p_index )const;
		/**
		 *\copydoc		castor3d::TextureStorage::Unbind
		 */
		void unbind( castor3d::TextureStorage const & p_storage, uint32_t p_index )const;
		/**
		 *\copydoc		castor3d::TextureStorage::Lock
		 */
		uint8_t * lock( castor3d::TextureStorage & p_storage, castor3d::AccessTypes const & p_lock, uint32_t p_index );
		/**
		 *\copydoc		castor3d::TextureStorage::Unlock
		 */
		void unlock( castor3d::TextureStorage & p_storage, bool p_modified, uint32_t p_index );
		/**
		 *\copydoc		castor3d::TextureStorage::Fill
		 */
		void fill( castor3d::TextureStorage & p_storage, castor3d::TextureImage const & p_image );

	private:
		GlDebug_Decl( uint32_t, m_allocatedSize )
	};
}

#endif
