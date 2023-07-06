/*
See LICENSE file in root folder
*/
#ifndef ___C3D_FontTexture_H___
#define ___C3D_FontTexture_H___

#include "Castor3D/Overlay/OverlayModule.hpp"
#include "Castor3D/Material/Texture/TextureModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Shader/ShaderBuffers/ShaderBuffersModule.hpp"

#include <CastorUtils/Design/Resource.hpp>
#include <CastorUtils/Design/Signal.hpp>
#include <CastorUtils/Graphics/Font.hpp>
#include <CastorUtils/Graphics/FontCache.hpp>
#include <CastorUtils/Graphics/Position.hpp>

namespace castor3d
{
	C3D_API void postPreRenderGpuEvent( Engine & engine
		, std::function< void( RenderDevice const &, QueueData const & ) > event );
	C3D_API void postQueueRenderCpuEvent( Engine & engine
		, std::function< void() > event );

	template< typename ResourceT, template< typename ResT > typename PointerT >
	class DoubleBufferedResourceT
		: public castor::OwnedBy< Engine >
	{
	public:
		using Resource = ResourceT;
		using ResourcePtrT = PointerT< ResourceT >;
		using OnChangedFunction = std::function< void( DoubleBufferedResourceT const & ) >;
		using OnChanged = castor::SignalT< OnChangedFunction >;

	protected:
		DoubleBufferedResourceT( Engine & parent
			, ResourcePtrT back
			, ResourcePtrT front )
			: OwnedBy{ parent }
			, m_back{ std::move( back ) }
			, m_front{ std::move( front ) }
		{
		}

		C3D_API virtual ~DoubleBufferedResourceT()noexcept = default;

	public:
		//!\~english	The signal used to notify clients that this resource has changed.
		//!\~french		Signal utilisé pour notifier les clients que cette ressource a changé.
		OnChanged onResourceChanged;
		/**
		 *\~english
		 *\brief		Updates the resource.
		 *\~french
		 *\brief		Met à jour la ressource.
		 */
		void update( bool clean )
		{
			doRefresh( clean, m_first.exchange( false ) );
		}

		inline ResourcePtrT const & getResource()const
		{
			return m_front;
		}

	protected:
		ResourcePtrT m_back;
		ResourcePtrT m_front;
		/**
		 *\~english
		 *\brief		Initialises the texture.
		 *\param[in]	device		The GPU device.
		 *\param[in]	queueData	The queue receiving the GPU commands.
		 *\~french
		 *\brief		Initialise la texture.
		 *\param[in]	device		Le device GPU.
		 *\param[in]	queueData	La queue recevant les commandes GPU.
		 */
		void doInitialise( RenderDevice const & device
			, QueueData const & queueData )
		{
			initialiseResource( *m_back, device, queueData );
			initialiseResource( *m_front, device, queueData );
		}
		/**
		 *\~english
		 *\brief		Flushes the teture.
		 *\~french
		 *\brief		Nettoie la texture.
		 */
		void doCleanup()
		{
			cleanupResource( *m_front );
			cleanupResource( *m_back );
		}
		/**
		 *\~english
		 *\brief		Orders events to refresh the resource.
		 *\~french
		 *\brief		Ordonne les évènements pour rafraîchir la ressource.
		 */
		void doRefresh( bool clean
			, bool front )
		{
			auto & resource = front ? *m_front : *m_back;
			updateResource( resource, front );
			postPreRenderGpuEvent( *getEngine()
				, [this, clean, front, &resource]( RenderDevice const & device
					, QueueData const & queueData )
				{
					if ( clean )
					{
						cleanupResource( resource );
					}

					initialiseResource( resource
						, device
						, queueData );

					if ( !front )
					{
						postQueueRenderCpuEvent( *getEngine()
							, [this]()
							{
								std::swap( m_front, m_back );
								swapResources();
								onResourceChanged( *this );
							} );
					}
				} );
		}

	private:
		std::atomic_bool m_first{ true };

		C3D_API virtual void initialiseResource( Resource & resource
			, RenderDevice const & device
			, QueueData const & queueData ) = 0;
		C3D_API virtual void cleanupResource( Resource & resource ) = 0;
		C3D_API virtual void updateResource( Resource & resource
			, bool front ) = 0;
		C3D_API virtual void swapResources() = 0;
	};
	using DoubleBufferedTextureLayout = DoubleBufferedResourceT< TextureLayout, castor::UniquePtr >;

	class FontTexture
		: public DoubleBufferedTextureLayout
	{
	public:
		CU_DeclareMap( char32_t, castor::Position, GlyphPosition );

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\param[in]	font	The font.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 *\param[in]	font	La police.
		 */
		C3D_API FontTexture( Engine & engine
			, castor::FontResPtr font );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~FontTexture()noexcept override;
		/**
		 *\~english
		 *\brief		Initialises the texture.
		 *\param[in]	device		The GPU device.
		 *\param[in]	queueData	The queue receiving the GPU commands.
		 *\~french
		 *\brief		Initialise la texture.
		 *\param[in]	device		Le device GPU.
		 *\param[in]	queueData	La queue recevant les commandes GPU.
		 */
		C3D_API void initialise( RenderDevice const & device
			, QueueData const & queueData );
		/**
		 *\~english
		 *\brief		Flushes the teture.
		 *\~french
		 *\brief		Nettoie la texture.
		 */
		C3D_API void cleanup( RenderDevice const & device );
		/**
		 *\~english
		 *\brief		Uploads the glyphs info buffer.
		 *\~french
		 *\brief		Upload le buffer d'informations des glyphes.
		 */
		C3D_API void upload( ashes::CommandBuffer const & commandBuffer );
		/**
		 *\~english
		 *\brief		Converts text to glyph index array.
		 *\~french
		 *\brief		Convertit un texte en tableau d'index de glyphe.
		 */
		C3D_API castor::UInt32Array convert( castor::U32String const & text )const;
		/**
		 *\~english
		 *\brief		Retrieves the font name.
		 *\return		The value.
		 *\~french
		 *\brief		Récupère le nom de la police.
		 *\return		La valeur.
		 */
		C3D_API castor::String const & getFontName()const;
		/**
		 *\~english
		 *\brief		Retrieves the wanted glyph position.
		 *\param[in]	glyphChar	The glyph index.
		 *\return		The position.
		 *\~french
		 *\brief		Récupère la position de la glyphe voulue.
		 *\param[in]	glyphChar	L'indice de la glyphe.
		 *\return		La position.
		 */
		C3D_API castor::Position const & getGlyphPosition( char32_t glyphChar )const;
		/**
		 *\~english
		 *name Getters.
		 *\~french
		 *name Accesseurs.
		**/
		/**@{*/
		castor::FontRPtr getFont()const
		{
			return m_font;
		}

		FontGlyphBuffer const & getFontBuffer()const
		{
			return *m_buffer;
		}

		TextureLayoutRPtr getTexture()const
		{
			return getResource().get();
		}

		SamplerObs getSampler()const
		{
			return m_sampler;
		}
		/**@}*/
		/**
		 *\~english
		 *name Mutators.
		 *\~french
		 *name Mutateurs.
		**/
		/**@{*/
		void setId( uint32_t v )
		{
			m_id = v;
		}
		/**@}*/

	private:
		void initialiseResource( Resource & resource
			, RenderDevice const & device
			, QueueData const & queueData )override;
		void cleanupResource( Resource & resource )override;
		void updateResource( Resource & resource
			, bool front )override;
		void swapResources()override;

	private:
		castor::FontResPtr m_font{};
		SamplerObs m_sampler{};
		GlyphPositionMap m_frontGlyphsPositions;
		GlyphPositionMap m_backGlyphsPositions;
		uint32_t m_id;
		FontGlyphBufferUPtr m_buffer;
		std::map< char32_t, uint32_t > m_charIndices;
	};
}

#endif
