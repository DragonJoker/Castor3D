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

#pragma warning( push )
#pragma warning( disable:4365 )
#include <atomic>
#include <mutex>
#include <unordered_set>
#pragma warning( pop )

namespace castor3d
{
	class TextureUnitCache
		: public castor::OwnedBy< Engine >
	{
	public:
		struct ThreadData
		{
			ThreadData( TextureUnitData & pdata
				, TextureUnit & punit )
				: data{ &pdata }
				, unit{ &punit }
			{
			}

			TextureUnitData * data;
			TextureLayoutSPtr layout{};
			TextureUnit * unit{};
			std::atomic_bool interrupted{ false };
		};

	public:
		/**
		 *\name
		 *	Construction / Destruction.
		 */
		/**@{*/
		C3D_API explicit TextureUnitCache( Engine & engine );
		C3D_API ~TextureUnitCache();
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
		C3D_API TextureCombineID registerTextureCombine( TextureCombine combine );
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
		C3D_API TextureUnitSPtr getTexture( TextureUnitData & unitData );
		C3D_API TextureUnitData & getSourceData( TextureSourceInfo const & sourceInfo
			, PassTextureConfig const & config
			, AnimationUPtr animation );
		C3D_API TextureUnitData & mergeSources( TextureSourceInfo const & lhsSourceInfo
			, PassTextureConfig const & lhsPassConfig
			, uint32_t lhsSrcMask
			, uint32_t lhsDstMask
			, TextureSourceInfo const & rhsSourceInfo
			, PassTextureConfig const & rhsPassConfig
			, uint32_t rhsSrcMask
			, uint32_t rhsDstMask
			, castor::String const & name
			, TextureSourceInfo const & resultSourceInfo
			, PassTextureConfig const & resultPassConfig );
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
		/**@}*/

	private:
		void doCreateLayout( ThreadData & data
			, castor::String const & name );
		void doUpload( ThreadData & data );
		void doAddWrite( TextureUnit & unit );
		void doUpdateWrite( TextureUnit const & unit );
		ThreadData & doCreateThreadData( TextureUnitData & data
			, TextureUnit & unit );
		void doDestroyThreadData( ThreadData & data );

		bool hasBindless()const
		{
			return m_bindlessTexLayout && m_bindlessTexSet;
		}

	private:
		castor::CheckedMutex m_dirtyMtx;
		std::unordered_set< Pass * > m_dirty;
		castor::CheckedMutex m_loadMtx;
		std::vector< std::unique_ptr< ThreadData > > m_loading;
		std::unordered_map< size_t, TextureUnitSPtr > m_loaded;
		ashes::DescriptorSetLayoutPtr m_bindlessTexLayout;
		ashes::DescriptorPoolPtr m_bindlessTexPool;
		ashes::DescriptorSetPtr m_bindlessTexSet;
		std::mutex m_dirtyWritesMtx;
		std::vector< ashes::WriteDescriptorSet > m_dirtyWrites;
		std::map< TextureUnit const *, OnTextureUnitChangedConnection > m_units;
		std::map< size_t, TextureUnitDataUPtr > m_datas;
		std::atomic_bool m_initialised{};
		std::vector< TextureUnit * > m_pendingUnits;
		mutable std::vector< TextureCombine > m_texturesCombines;
	};
}

#endif
