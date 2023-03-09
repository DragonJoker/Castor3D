/*
See LICENSE file in root folder
*/
#ifndef ___C3D_OverlayCategory_H___
#define ___C3D_OverlayCategory_H___

#include "Castor3D/Overlay/OverlayVisitor.hpp"
#include "Castor3D/Material/MaterialModule.hpp"
#include "Castor3D/Render/Overlays/OverlaysModule.hpp"

#include <CastorUtils/Data/TextWriter.hpp>
#include <CastorUtils/Graphics/Position.hpp>
#include <CastorUtils/Graphics/Size.hpp>

namespace castor3d
{
	class OverlayCategory
	{
	public:
		/**
		\~english
		\brief		Holds specific vertex data for an Overlay
		\~french
		\brief		Contient les données spécifiques de sommet pour un Overlay
		*/
		struct Vertex
		{
			castor::Point2f coords;
			castor::Point2f texture;

			Vertex * data()
			{
				return this;
			}
		};
		CU_DeclareVector( Vertex, Vertex );

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	type		The overlay type
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	type		Le type de l'incrustation
		 */
		C3D_API explicit OverlayCategory( OverlayType type );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~OverlayCategory();
		/**
		 *\~english
		 *\brief		Updates the overlay position, size...
		 *\~french
		 *\brief		Met à jour la position, taille...
		 */
		C3D_API void update( OverlayRenderer const & renderer );
		/**
		 *\~english
		 *\brief		Draws the overlay
		 *\~french
		 *\brief		Dessine l'incrustation
		 */
		C3D_API virtual void accept( OverlayVisitor & renderer )const = 0;
		/**
		 *\~english
		 *\brief		Sets the material
		 *\param[in]	material	The new value
		 *\~french
		 *\brief		Définit le matériau
		 *\param[in]	material	La nouvelle valeur
		 */
		C3D_API virtual void setMaterial( MaterialRPtr material );
		/**
		 *\~english
		 *\brief		Retrieves the overlay name
		 *\return		The value
		 *\~french
		 *\brief		Récupère le nom de l'incrustation
		 *\return		La valeur
		 */
		C3D_API castor::String const & getOverlayName()const;
		/**
		 *\~english
		 *\brief		Retrieves the absolute overlay position, in pixels
		 *\param[in]	size	The screen size
		 *\return		The position
		 *\~french
		 *\brief		Récupère la position absolue de l'incrustation, en pixels
		 *\param[in]	size	La taille de l'écran
		 *\return		La position
		 */
		C3D_API castor::Position getAbsolutePosition( castor::Size const & size )const;
		/**
		 *\~english
		 *\brief		Retrieves the absolute overlay size, in pixels
		 *\param[in]	size	The screen size
		 *\return		The size
		 *\~french
		 *\brief		Récupère la taille absolue de l'incrustation, en pixels
		 *\param[in]	size	La taille de l'écran
		 *\return		La taille
		 */
		C3D_API castor::Size getAbsoluteSize( castor::Size const & size )const;
		/**
		 *\~english
		 *\param[in]	size	The render size
		 *\return		The ratio between given dimensions and the dimensions used when computing relative position from pixel position.
		 *\~french
		 *\param[in]	size	La taille du rendu
		 *\return		Le ratio entre les dimensions données et les dimensions utilisées lors du calcul de la position relative depuis la position en pixels.
		 */
		C3D_API castor::Point2d getRenderRatio( castor::Size const & size )const;
		/**
		 *\~english
		 *\brief		Retrieves the absolute overlay position
		 *\return		The position
		 *\~french
		 *\brief		Récupère la position absolue de l'incrustation
		 *\return		La position
		 */
		C3D_API castor::Point2d getAbsolutePosition()const;
		/**
		 *\~english
		 *\brief		Retrieves the absolute overlay size
		 *\return		The size
		 *\~french
		 *\brief		Récupère la taille absolue de l'incrustation
		 *\return		La taille
		 */
		C3D_API castor::Point2d getAbsoluteSize()const;
		/**
		 *\~english
		 *\return		\p true if this overlay's or one of its parents' size has changed.
		 *\~french
		 *\return		\p true si les dimensions de cette incrustation ou d'un de ses parents ont changé.
		 */
		C3D_API bool isSizeChanged()const;
		/**
		 *\~english
		 *\return		\p true if this overlay's or one of its parents' position has changed.
		 *\~french
		 *\return		\p true si la position de cette incrustation ou d'un de ses parents a changé.
		 */
		C3D_API bool isPositionChanged()const;
		/**
		 *\~english
		 *\brief		Retrieves the overlay size
		 *\return		The value
		 *\~french
		 *\brief		Récupère la taille de l'incrustation
		 *\return		La valeur
		 */
		C3D_API castor::Size computePixelSize()const;
		/**
		 *\~english
		 *\brief		Retrieves the overlay position
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position de l'incrustation
		 *\return		La valeur
		 */
		C3D_API castor::Position computePixelPosition()const;
		/**
		*\~english
		*\name
		*	Getters.
		*\~french
		*\name
		*	Accesseurs.
		*/
		/**@{*/
		C3D_API virtual bool isChanged()const noexcept
		{
			return false;
		}

		castor::Point4d const & getClientArea()const noexcept
		{
			return m_clientArea;
		}

		castor::Point2d const & getRelativeSize()const noexcept
		{
			return m_relSize;
		}

		castor::Point2d const & getRelativePosition()const noexcept
		{
			return m_relPosition;
		}

		OverlayType getType()const noexcept
		{
			return m_type;
		}

		bool isVisible()const noexcept
		{
			return m_visible;
		}

		MaterialRPtr getMaterial()const noexcept
		{
			return m_material;
		}

		uint32_t getIndex()const noexcept
		{
			return m_index;
		}

		uint32_t getLevel()const noexcept
		{
			return m_level;
		}

		Overlay const & getOverlay()const noexcept
		{
			return *m_overlay;
		}

		Overlay & getOverlay()noexcept
		{
			return *m_overlay;
		}

		castor::Point4d const & getUV()const noexcept
		{
			return m_uv;
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
		void setRelativePosition( castor::Point2d const & value
			, bool notify = true )noexcept
		{
			m_relPosition = value;
			m_positionChanged = true;

			if ( notify )
			{
				notifyPositionChanged();
			}
		}

		void setRelativeSize( castor::Point2d const & value
			, bool notify = true )noexcept
		{
			m_relSize = value;
			m_relSize->x = std::max( 0.0, m_relSize->x );
			m_relSize->y = std::max( 0.0, m_relSize->y );
			m_sizeChanged = true;

			if ( notify )
			{
				notifySizeChanged();
			}
		}

		void setPixelPosition( castor::Position const & value
			, bool notify = true )noexcept
		{
			m_pxPosition = value;
			m_positionChanged = true;

			if ( notify )
			{
				notifyPositionChanged();
			}
		}

		void setPixelSize( castor::Size const & value
			, bool notify = true )noexcept
		{
			m_pxSize = value;
			m_sizeChanged = true;

			if ( notify )
			{
				notifySizeChanged();
			}
		}

		void setOverlay( Overlay * value )noexcept
		{
			m_overlay = value;
		}

		void setVisible( bool value )noexcept
		{
			m_visible = value;
		}

		void setOrder( uint32_t level, uint32_t index )noexcept
		{
			m_index = index;
			m_level = level;
		}

		void setUV( castor::Point4d const & value )noexcept
		{
			m_uv = value;
		}
		/**@}*/

	protected:
		/**
		 *\~english
		 *\brief		Notifies overlay's children of a position change.
		 *\~french
		 *\brief		Notifie les enfants de l'incrustation d'un changement de position.
		 */
		C3D_API void notifyPositionChanged()noexcept;
		/**
		 *\~english
		 *\brief		Notifies overlay's children of a size change.
		 *\~french
		 *\brief		Notifie les enfants de l'incrustation d'un changement de dimensions.
		 */
		C3D_API void notifySizeChanged()noexcept;
		/**
		 *\~english
		 *\brief		Updates the overlay position, taking care of wanted pixel position.
		 *\~french
		 *\brief		Met à jour la position de l'incrustation, en prenant en compte la la position en pixel voulue.
		 */
		C3D_API void updatePosition( OverlayRenderer const & renderer );
		/**
		 *\~english
		 *\brief		Updates the overlay size, taking care of wanted pixel size.
		 *\~french
		 *\brief		Met à jour la taille de l'incrustation, en prenant en compte la taille en pixel voulue.
		 */
		C3D_API void updateSize( OverlayRenderer const & renderer );
		/**
		 *\~english
		 *\brief		Updates the scissor for this overlay
		 *\~french
		 *\brief		Met à jour le scissor pour cette incrustation
		 */
		C3D_API void updateClientArea( OverlayRenderer const & renderer );

	private:
		/**
		 *\~english
		 *\return		The screen or parent's size.
		 *\~french
		 *\return		La taille de l'écran ou du parent.
		 */
		castor::Point2d doGetParentSize()const;
		/**
		 *\~english
		 *\brief		Updates the overlay position, taking care of wanted pixel position.
		 *\~french
		 *\brief		Met à jour la position de l'incrustation, en prenant en compte la la position en pixel voulue.
		 */
		virtual void doUpdatePosition( OverlayRenderer const & renderer )
		{
		}
		/**
		 *\~english
		 *\brief		Updates the overlay size, taking care of wanted pixel size.
		 *\~french
		 *\brief		Met à jour la taille de l'incrustation, en prenant en compte la taille en pixel voulue.
		 */
		virtual void doUpdateSize( OverlayRenderer const & renderer )
		{
		}
		/**
		 *\~english
		 *\brief		Updates the overlay position, size...
		 *\~french
		 *\brief		Met à jour la position, taille...
		 */
		virtual void doUpdate( OverlayRenderer const & renderer ) = 0;

	protected:
		//!\~english	The overlay.
		//!\~french		L'incrustation.
		Overlay * m_overlay{ nullptr };
		//!\~english	The relative position (to parent or screen).
		//!\~french		La position relative (au parent ou à l'écran).
		castor::Point2d m_relPosition;
		//!\~english	The relative size (to parent or screen).
		//!\~french		La taille relative (à l'écran ou au parent).
		castor::Point2d m_relSize;
		//!\~english	The relative position (to parent or screen), in pixels.
		//!\~french		La position relative (à l'écran ou au parent), en pixels.
		std::optional< castor::Position > m_pxPosition;
		//!\~english	The absolute size in pixels.
		//!\~french		La taille absolue en pixels.
		std::optional< castor::Size > m_pxSize;
		//!\~english	The size used to compute relative position from pixel position.
		//!\~french		La taille utilisée pour calculer la position relative depuis la position en pixels.
		castor::Size m_computeSize;
		//!\~english	The visibility.
		//!\~french		La visibilité.
		bool m_visible{ true };
		//!\~english	The material used by the overlay.
		//!\~french		Le matériau utilisé par l'incrustation.
		MaterialRPtr m_material{};
		//!\~english	The overlay index.
		//!\~french		L'index de l'overlay.
		uint32_t m_index{ 0 };
		//!\~english	The overlay level.
		//!\~french		Le niveau de l'overlay.
		uint32_t m_level{ 0 };
		//!\~english	The overlay type.
		//!\~french		Le type de l'incrustation.
		OverlayType m_type;
		//!\~english	Tells if this overlay's size has changed..
		//!\~french		Dit si les dimensions de cette incrustation ont changé.
		bool m_sizeChanged{ true };
		//!\~english	Tells if this overlay's position has changed.
		//!\~french		Dit si la position de cette incrustation a changé..
		bool m_positionChanged{ true };
		//!\~english	The UV for the panel.
		//!\~french		Les UV du panneau.
		castor::Point4d m_uv{ 0.0, 0.0, 1.0, 1.0 };
		//!\~english	The area where children can be drawn.
		//!\~french		La zone où les enfants peuvent se dessiner.
		castor::Point4d m_clientArea{};
	};
}

#endif
