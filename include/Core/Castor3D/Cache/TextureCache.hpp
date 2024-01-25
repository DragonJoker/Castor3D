/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextureCache_H___
#define ___C3D_TextureCache_H___

#include "CacheModule.hpp"
#include "Castor3D/Material/Pass/PassModule.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Material/Texture/TextureSourceInfo.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <atomic>
#include <mutex>
#include <unordered_set>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

namespace castor3d
{
	class TextureUnitCache
		: public castor::OwnedBy< Engine >
	{
	public:
		struct ThreadData
		{
			explicit ThreadData( TextureData & pdata )
				: data{ &pdata }
			{
			}

			TextureData * data;
			Texture * texture{};
			std::atomic_bool interrupted{ false };
			std::atomic_bool expected{ false };
		};

	public:
		/**
		 *\name
		 *	Construction / Destruction.
		 */
		/**@{*/
		C3D_API TextureUnitCache( Engine & engine
			, crg::ResourcesCache & resources );
		/**@}*/
		/**
		 *\~english
		 *\name
		 *	Texture combinations registration.
		 *\~french
		 *\name
		 *	Enregistrement de combinaisons de textures.
		 */
		/**@{*/
		C3D_API TextureCombine registerTextureCombine( Pass const & pass );
		C3D_API TextureCombineID registerTextureCombine( TextureCombine & combine );
		C3D_API TextureCombineID getTextureCombineID( TextureCombine const & combine )const;
		C3D_API TextureCombine getTextureCombine( TextureCombineID id )const;
		/**@}*/
		/**
		 *\~english
		 *\name
		 *	General functions.
		 *\~french
		 *\name
		 *	Fonctions générales.
		 */
		/**@{*/
		C3D_API void initialise( RenderDevice const & device );
		C3D_API void cleanup();
		C3D_API void stopLoad();

		C3D_API void update( GpuUpdater & updater );
		C3D_API void upload( UploadData & uploader );

		C3D_API void notifyPassChange( Pass & pass );
		C3D_API void preparePass( Pass & pass );

		C3D_API void clear();
		/**@}*/
		/**
		 *\~english
		 *\name
		 *	Texture handling functions.
		 *\~french
		 *\name
		 *	Fonctions de gestion de textures.
		 */
		/**@{*/
		C3D_API TextureData & getSourceData( TextureSourceInfo const & sourceInfo );
		/**@}*/
		/**
		 *\~english
		 *\name
		 *	Texture unit handling functions.
		 *\~french
		 *\name
		 *	Fonctions de gestion d'unités de texture.
		 */
		/**@{*/
		C3D_API TextureUnitRPtr getTextureUnit( TextureUnitData & unitData );
		C3D_API TextureUnitData & getSourceData( TextureSourceInfo const & sourceInfo
			, PassTextureConfig const & config
			, TextureAnimationUPtr animation );
		/**@}*/
		/**
		 *\name
		 *	Getters.
		 */
		/**@{*/
		ashes::DescriptorSetLayout * getDescriptorLayout()const
		{
			return m_bindlessTexLayout.get();
		}

		ashes::DescriptorPool * getDescriptorPool()const
		{
			return m_bindlessTexPool.get();
		}

		ashes::DescriptorSet * getDescriptorSet()const
		{
			return m_bindlessTexSet.get();
		}

		auto begin()const
		{
			return m_datas.begin();
		}

		auto end()const
		{
			return m_datas.end();
		}
		/**@}*/

	private:
		Texture const * doGetTexture( TextureData & data
			, castor::Function< void( TextureData const & , Texture const *, bool ) > const & onEndCpuLoad );
		void doInitTexture( ThreadData & data );
		void doUpload( ThreadData & data );
		void doAddWrite( TextureUnit & unit );
		void doUpdateWrite( TextureUnit const & unit );
		ThreadData & doCreateThreadData( TextureData & data );
		ThreadData & doFindThreadData( TextureData & data );
		void doDestroyThreadData( ThreadData & data );
		castor::Vector< TextureUnit * > doListTextureUnits( Texture const * texture );

		bool hasBindless()const
		{
			return m_bindlessTexLayout && m_bindlessTexSet;
		}

	private:
		crg::ResourcesCache & m_resources;
		castor::CheckedMutex m_dirtyMtx;
		castor::UnorderedSet< Pass * > m_dirty;
		castor::CheckedMutex m_loadMtx;
		castor::CheckedMutex m_uploadMtx;
		castor::Vector< castor::RawUniquePtr< ThreadData > > m_loading;
		castor::UnorderedMap< size_t, TextureUPtr > m_loaded;
		castor::UnorderedMap< size_t, TextureUnitUPtr > m_loadedUnits;
		ashes::DescriptorSetLayoutPtr m_bindlessTexLayout;
		ashes::DescriptorPoolPtr m_bindlessTexPool;
		ashes::DescriptorSetPtr m_bindlessTexSet;
		castor::Mutex m_dirtyWritesMtx;
		castor::Vector< ashes::WriteDescriptorSet > m_dirtyWrites;
		castor::Map< TextureUnit const *, OnTextureUnitChangedConnection > m_units;
		castor::Map< size_t, TextureDataUPtr > m_datas;
		castor::Map< size_t, TextureUnitDataUPtr > m_unitDatas;
		std::atomic_bool m_initialised{};
		castor::Vector< TextureUnit * > m_pendingUnits;
		mutable castor::Vector< TextureCombine > m_texturesCombines;
		castor::Map< TextureData *, Texture * > m_toUpload;
		castor::Map< Texture const *, castor::Vector< TextureUnit * > > m_unitsToAdd;
	};
}

#endif
