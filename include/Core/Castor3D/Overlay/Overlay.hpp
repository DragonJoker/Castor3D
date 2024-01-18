/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Overlay_H___
#define ___C3D_Overlay_H___

#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Scene/SceneModule.hpp"

#include "Castor3D/Overlay/OverlayCategory.hpp"

#include <CastorUtils/Data/TextWriter.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>

namespace castor3d
{
	class Overlay final
		: public castor::OwnedBy< Engine >
	{
		friend class castor::ResourceCacheT< Overlay
			, castor::String
			, OverlayCacheTraits >;

	public:
		using iterator = OverlayPtrArray::iterator;
		using const_iterator = OverlayPtrArray::const_iterator;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	name	The overlay name.
		 *\param[in]	engine	The engine.
		 *\param[in]	type	The overlay type.
		 *\param[in]	scene	The scene holding the overlay.
		 *\param[in]	parent	The parent overlay (if any).
		 *\param[in]	level	The overlay base level.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	name	Le nom de l'incrustation.
		 *\param[in]	engine	Le moteur.
		 *\param[in]	type	Le type de l'incrustation.
		 *\param[in]	scene	La scène parent.
		 *\param[in]	parent	L'incrustation parente.
		 *\param[in]	level	Le niveau de base de l'incrustation.
		 */
		C3D_API Overlay( castor::String const & name
			, Engine & engine
			, OverlayType type
			, Scene * scene
			, OverlayRPtr parent
			, uint32_t level = 0u );
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\param[in]	type	The overlay type.
		 *\param[in]	scene	The scene holding the overlay.
		 *\param[in]	parent	The parent overlay (if any).
		 *\param[in]	level	The overlay base level.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 *\param[in]	type	Le type de l'incrustation.
		 *\param[in]	scene	La scène parent.
		 *\param[in]	parent	L'incrustation parente.
		 *\param[in]	level	Le niveau de base de l'incrustation.
		 */
		C3D_API Overlay( Engine & engine
			, OverlayType type
			, Scene * scene
			, OverlayRPtr parent
			, uint32_t level = 0u );
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\param[in]	type	The overlay type.
		 *\param[in]	level	The overlay base level.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 *\param[in]	type	Le type de l'incrustation.
		 *\param[in]	level	Le niveau de base de l'incrustation.
		 */
		C3D_API Overlay( Engine & engine
			, OverlayType type
			, uint32_t level = 0u );
		/**
		 *\~english
		 *\param[in]	level	The wanted level
		 *\return		The childs count at given level.
		 *\~french
		 *\param[in]	level	Le niveau voulu.
		 *\return		Le compte des enfants du niveau donné.
		 */
		C3D_API uint32_t getChildrenCount( uint32_t level )const;
		/**
		 *\~english
		 *\return		The panel overlay.
		 *\~french
		 *\return		L'incrustation panneau.
		 */
		C3D_API PanelOverlayRPtr getPanelOverlay()const;
		/**
		 *\~english
		 *\return		The border panel overlay.
		 *\~french
		 *\return		L'incrustation panneau borduré.
		 */
		C3D_API BorderPanelOverlayRPtr getBorderPanelOverlay()const;
		/**
		 *\~english
		 *\return		The text overlay.
		 *\~french
		 *\return		L'incrustation texte.
		 */
		C3D_API TextOverlayRPtr getTextOverlay()const;
		/**
		 *\~english
		 *\return		The visibility status.
		 *\~french
		 *\return		Le statut de visibilité.
		 */
		C3D_API bool isVisible()const;
		/**
		 *\~english
		 *\return		The siplayability status.
		 *\~french
		 *\return		Le statut d'affichabilité.
		 */
		C3D_API bool isDisplayable()const;
		/**
		 *\~english
		 *\return		The hierarchy level for this overlay
		 *\~french
		 *\return		Le niveau dans la hiérarchie de cet overlay.
		 */
		C3D_API uint32_t computeLevel()const;
		/**
		*\~english
		*\name
		*	Getters.
		*\~french
		*\name
		*	Accesseurs.
		*/
		/**@{*/
		OverlayCategory & getCategory()const noexcept
		{
			return *m_category;
		}

		castor::Position getAbsolutePosition( castor::Size const & size )const noexcept
		{
			return m_category->getAbsolutePosition( size );
		}

		castor::Size getAbsoluteSize( castor::Size const & size )const noexcept
		{
			return m_category->getAbsoluteSize( size );
		}

		castor::Point2d getAbsolutePosition()const noexcept
		{
			return m_category->getAbsolutePosition();
		}

		castor::Point2d getAbsoluteSize()const noexcept
		{
			return m_category->getAbsoluteSize();
		}

		bool isSizeChanged()const noexcept
		{
			return m_category->isSizeChanged();
		}

		bool isPositionChanged()const noexcept
		{
			return m_category->isPositionChanged();
		}

		castor::String const & getName()const noexcept
		{
			return m_name;
		}

		castor::Point2d const & getRelativePosition()const noexcept
		{
			return m_category->getRelativePosition();
		}

		castor::Point2d const & getRelativeSize()const noexcept
		{
			return m_category->getRelativeSize();
		}

		OverlayType getType()const noexcept
		{
			return m_category->getType();
		}

		OverlayRPtr getParent()const noexcept
		{
			return m_parent;
		}

		MaterialObs getMaterial()const noexcept
		{
			return m_category->getMaterial();
		}

		SceneRPtr getScene()const noexcept
		{
			return m_scene;
		}

		uint32_t getIndex()const noexcept
		{
			return m_category->getIndex();
		}

		uint32_t getLevel()const noexcept
		{
			return m_category->getLevel();
		}
		/**@}*/
		/**
		*\~english
		*\name
		*	Array access to childs.
		*\~french
		*\name
		*	Acces au tableau d'enfants.
		*/
		/**@{*/
		size_t size()const noexcept
		{
			return uint32_t( m_children.size() );
		}

		bool empty()const noexcept
		{
			return m_children.empty();
		}

		iterator begin()
		{
			return m_children.begin();
		}

		const_iterator begin()const noexcept
		{
			return m_children.begin();
		}

		iterator end()
		{
			return m_children.end();
		}

		const_iterator end()const noexcept
		{
			return m_children.end();
		}
		/**@}*/
		/**
		*\~english
		*\name
		*	Mutators.
		*\~french
		*\name
		*	Mutateurs.
		*/
		/**@{*/
		void setVisible( bool val )
		{
			m_category->setVisible( val );
		}

		void setMaterial( MaterialObs material )
		{
			m_category->setMaterial( material );
		}

		void setRelativePosition( castor::Point2d const & position )
		{
			m_category->setRelativePosition( position );
		}

		void setRelativeSize( castor::Point2d const & size )
		{
			m_category->setRelativeSize( size );
		}

		void setPixelPosition( castor::Position const & position )
		{
			m_category->setPixelPosition( position );
		}

		void setPixelSize( castor::Size const & size )
		{
			m_category->setPixelSize( size );
		}

		void rename( castor::StringView name )
		{
			m_name = name;
		}

		void reparent( OverlayRPtr parent )
		{
			m_parent = parent;
		}
		/**@}*/

	private:
		void addChild( OverlayRPtr overlay );
		void removeChild( OverlayRPtr overlay );
		void clear()noexcept;

	private:
		castor::String m_name;
		OverlayRPtr m_parent{};
		OverlayPtrArray m_children;
		OverlayCategoryUPtr m_category;
		Scene * m_scene;
		RenderSystem * m_renderSystem;
	};

	struct RootContext;
	struct SceneContext;

	struct OverlayContext
	{
		struct OverlayPtr
		{
			OverlayUPtr uptr{};
			OverlayRPtr rptr{};
		};
		SceneContext * scene{};
		RootContext * root{};
		OverlayPtr overlay;
		std::vector< OverlayPtr > parentOverlays{};
	};

	C3D_API Engine * getEngine( OverlayContext const & context );
}

namespace castor
{
	template<>
	struct ParserEnumTraits< castor3d::TextWrappingMode >
	{
		static inline StringView constexpr Name = cuT( "TextWrappingMode" );
		static inline UInt32StrMap const Values = []()
			{
				UInt32StrMap result;
				result = castor3d::getEnumMapT< castor3d::TextWrappingMode >();
				return result;
			}( );
	};

	template<>
	struct ParserEnumTraits< castor3d::BorderPosition >
	{
		static inline StringView constexpr Name = cuT( "BorderPosition" );
		static inline UInt32StrMap const Values = []()
			{
				UInt32StrMap result;
				result = castor3d::getEnumMapT< castor3d::BorderPosition >();
				return result;
			}( );
	};

	template<>
	struct ParserEnumTraits< castor3d::VAlign >
	{
		static inline StringView constexpr Name = cuT( "VAlign" );
		static inline UInt32StrMap const Values = []()
			{
				UInt32StrMap result;
				result = castor3d::getEnumMapT< castor3d::VAlign >();
				return result;
			}( );
	};

	template<>
	struct ParserEnumTraits< castor3d::HAlign >
	{
		static inline StringView constexpr Name = cuT( "HAlign" );
		static inline UInt32StrMap const Values = []()
			{
				UInt32StrMap result;
				result = castor3d::getEnumMapT< castor3d::HAlign >();
				return result;
			}( );
	};

	template<>
	struct ParserEnumTraits< castor3d::TextTexturingMode >
	{
		static inline StringView constexpr Name = cuT( "TextTexturingMode" );
		static inline UInt32StrMap const Values = []()
			{
				UInt32StrMap result;
				result = castor3d::getEnumMapT< castor3d::TextTexturingMode >();
				return result;
			}( );
	};

	template<>
	struct ParserEnumTraits< castor3d::TextLineSpacingMode >
	{
		static inline StringView constexpr Name = cuT( "TextLineSpacingMode" );
		static inline UInt32StrMap const Values = []()
			{
				UInt32StrMap result;
				result = castor3d::getEnumMapT< castor3d::TextLineSpacingMode >();
				return result;
			}( );
	};
}

#endif
