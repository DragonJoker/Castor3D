/*
See LICENSE file in root folder
*/
#ifndef ___C3D_OverlayCategory_H___
#define ___C3D_OverlayCategory_H___

#include "Castor3D/Overlay/OverlayVisitor.hpp"
#include "Castor3D/Material/MaterialModule.hpp"

#include <CastorUtils/Data/TextWriter.hpp>
#include <CastorUtils/Graphics/Position.hpp>
#include <CastorUtils/Graphics/Size.hpp>

namespace castor3d
{
	class OverlayCategory
	{
	public:
		/**
		\author 	Sylvain DOREMUS
		\date 		28/11/2014
		\version	0.7.0
		\~english
		\brief		Holds specific vertex data for an Overlay
		\~french
		\brief		Contient les données spécifiques de sommet pour un Overlay
		*/
		struct Vertex
		{
			castor::Point2f coords;
			castor::Point2f texture;
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
		C3D_API virtual void setMaterial( MaterialSPtr material );
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
		C3D_API castor::Point2f getRenderRatio( castor::Size const & size )const;
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
		 *\return		\p true if this overlay's has changed.
		 *\~french
		 *\return		\p true si cette incrustation a changé.
		 */
		C3D_API virtual bool isChanged()const
		{
			return false;
		}
		/**
		 *\~english
		 *\brief		Sets the overlay position
		 *\param[in]	position	The new value
		 *\~french
		 *\brief		Définit la position de l'incrustation
		 *\param[in]	position	La nouvelle valeur
		 */
		inline void setPosition( castor::Point2d const & position )
		{
			m_position = position;
			m_positionChanged = true;
		}
		/**
		 *\~english
		 *\brief		Sets the overlay size
		 *\param[in]	size	The new value
		 *\~french
		 *\brief		Définit la taille de l'incrustation
		 *\param[in]	size	La nouvelle valeur
		 */
		inline void setSize( castor::Point2d const & size )
		{
			m_ptSize = size;
			m_sizeChanged = true;
		}
		/**
		 *\~english
		 *\brief		Retrieves the overlay size
		 *\return		The value
		 *\~french
		 *\brief		Récupère la taille de l'incrustation
		 *\return		La valeur
		 */
		inline castor::Point2d const & getSize()const
		{
			return m_ptSize;
		}
		/**
		 *\~english
		 *\brief		Retrieves the overlay position
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position de l'incrustation
		 *\return		La valeur
		 */
		inline castor::Point2d const & getPosition()const
		{
			return m_position;
		}
		/**
		 *\~english
		 *\brief		Retrieves the overlay size
		 *\return		The value
		 *\~french
		 *\brief		Récupère la taille de l'incrustation
		 *\return		La valeur
		 */
		inline castor::Size const & getPixelSize()const
		{
			return m_size;
		}
		/**
		 *\~english
		 *\brief		Retrieves the overlay position
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position de l'incrustation
		 *\return		La valeur
		 */
		inline castor::Position const & getPixelPosition()const
		{
			return m_pxPosition;
		}
		/**
		 *\~english
		 *\brief		Retrieves the overlay size
		 *\return		The value
		 *\~french
		 *\brief		Récupère la taille de l'incrustation
		 *\return		La valeur
		 */
		inline castor::Point2d & getSize()
		{
			return m_ptSize;
		}
		/**
		 *\~english
		 *\brief		Retrieves the overlay position
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position de l'incrustation
		 *\return		La valeur
		 */
		inline castor::Point2d & getPosition()
		{
			return m_position;
		}
		/**
		 *\~english
		 *\brief		Retrieves the overlay size
		 *\return		The value
		 *\~french
		 *\brief		Récupère la taille de l'incrustation
		 *\return		La valeur
		 */
		inline castor::Size & getPixelSize()
		{
			return m_size;
		}
		/**
		 *\~english
		 *\brief		Retrieves the overlay position
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position de l'incrustation
		 *\return		La valeur
		 */
		inline castor::Position & getPixelPosition()
		{
			return m_pxPosition;
		}
		/**
		 *\~english
		 *\brief		Retrieves the overlay type
		 *\return		The value
		 *\~french
		 *\brief		Récupère le type de l'incrustation
		 *\return		La valeur
		 */
		inline OverlayType getType()const
		{
			return m_type;
		}
		/**
		 *\~english
		 *\brief		Retrieves the visibility status
		 *\return		The value
		 *\~french
		 *\brief		Récupère le statut de visibilité
		 *\return		La valeur
		 */
		inline bool isVisible()const
		{
			return m_visible;
		}
		/**
		 *\~english
		 *\brief		Retrieves the material
		 *\return		The value
		 *\~french
		 *\brief		Récupère le matériau
		 *\return		La valeur
		 */
		inline MaterialSPtr getMaterial()const
		{
			return m_pMaterial.lock();
		}
		/**
		 *\~english
		 *\brief		Retrieves the index
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'indice
		 *\return		La valeur
		 */
		inline int getIndex()const
		{
			return m_index;
		}
		/**
		 *\~english
		 *\brief		Retrieves the level
		 *\return		The value
		 *\~french
		 *\brief		Récupère le niveau
		 *\return		La valeur
		 */
		inline int getLevel()const
		{
			return m_level;
		}
		/**
		 *\~english
		 *\brief		Retrieves the overlay
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'incrustation
		 *\return		La valeur
		 */
		inline Overlay const & getOverlay()const
		{
			return *m_pOverlay;
		}
		/**
		 *\~english
		 *\brief		Retrieves the overlay
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'incrustation
		 *\return		La valeur
		 */
		inline Overlay & getOverlay()
		{
			return *m_pOverlay;
		}
		/**
		 *\~english
		 *\brief		Sets the overlay
		 *\param[in]	value	The new value
		 *\~french
		 *\brief		Définit l'incrustation
		 *\param[in]	value	La nouvelle valeur
		 */
		inline void setOverlay( Overlay * value )
		{
			m_pOverlay = value;
		}
		/**
		 *\~english
		 *\brief		Sets the visibility status
		 *\param[in]	value	The new value
		 *\~french
		 *\brief		Définit le statut de visibilité
		 *\param[in]	value	La nouvelle valeur
		 */
		inline void setVisible( bool value )
		{
			m_visible = value;
		}
		/**
		 *\~english
		 *\brief		Sets the overlay position
		 *\param[in]	value	The new value
		 *\~french
		 *\brief		Définit la position de l'incrustation
		 *\param[in]	value	La nouvelle valeur
		 */
		inline void setPixelPosition( castor::Position const & value )
		{
			m_pxPosition = value;
			m_positionChanged = true;
		}
		/**
		 *\~english
		 *\brief		Sets the overlay size
		 *\param[in]	value	The new value
		 *\~french
		 *\brief		Définit la taille de l'incrustation
		 *\param[in]	value	La nouvelle valeur
		 */
		inline void setPixelSize( castor::Size const & value )
		{
			m_size = value;
			m_sizeChanged = true;
		}
		/**
		 *\~english
		 *\brief		Sets the overlay order
		 *\param[in]	index	The new index
		 *\param[in]	level	The new level
		 *\~french
		 *\brief		Définit l'ordre de l'incrustation
		 *\param[in]	index	Le nouvel indice
		 *\param[in]	level	Le nouveau niveau
		 */
		inline void setOrder( int index, int level )
		{
			m_index = index;
			m_level = level;
		}
		/**
		 *\~english
		 *\brief		Sets the overlay UV
		 *\param[in]	value	The new value (left, top, right and bottom)
		 *\~french
		 *\brief		Définit Les UV de l'incrustation
		 *\param[in]	value	La nouvelle valeur (gauche, haut, droite, bas)
		 */
		inline void setUV( castor::Point4d const & value )
		{
			m_uv = value;
		}
		/**
		 *\~english
		 *\brief		Retrieves the overlay UV
		 *\return		The value (left, top, right and bottom)
		 *\~french
		 *\brief		Récupère Les UV de l'incrustation
		 *\return		La valeur (gauche, haut, droite, bas)
		 */
		inline castor::Point4d const & getUV()const
		{
			return m_uv;
		}

	protected:
		/**
		 *\~english
		 *\return		The screen or parent's size.
		 *\~french
		 *\return		La taille de l'écran ou du parent.
		 */
		castor::Point2d doGetTotalSize( OverlayRenderer const & renderer )const;
		/**
		 *\~english
		 *\brief		Updates the overlay position, taking care of wanted pixel position.
		 *\~french
		 *\brief		Met à jour la position de l'incrustation, en prenant en compte la la position en pixel voulue.
		 */
		virtual void doUpdatePosition( OverlayRenderer const & renderer );
		/**
		 *\~english
		 *\brief		Updates the overlay size, taking care of wanted pixel size.
		 *\~french
		 *\brief		Met à jour la taille de l'incrustation, en prenant en compte la taille en pixel voulue.
		 */
		virtual void doUpdateSize( OverlayRenderer const & renderer );
		/**
		 *\~english
		 *\brief		Updates the overlay position, size...
		 *\~french
		 *\brief		Met à jour la position, taille...
		 */
		virtual void doUpdate( OverlayRenderer const & renderer ) {}
		/**
		 *\~english
		 *\brief		Updates the vertex buffer.
		 *\param[in]	size	The render target size.
		 *\~french
		 *\brief		Met à jour le tampon de sommets.
		 *\param[in]	size	Les dimensions de la cible de rendu.
		 */
		virtual void doUpdateBuffer( castor::Size const & size ) = 0;

	protected:
		//!\~english	The overlay.
		//!\~french		L'incrustation.
		Overlay * m_pOverlay{ nullptr };
		//!\~english	The relative position (to parent or screen).
		//!\~french		La position relative (au parent ou à l'écran).
		castor::Point2d m_position;
		//!\~english	The relative size (to parent or screen).
		//!\~french		La taille relative (à l'écran ou au parent).
		castor::Point2d m_ptSize;
		//!\~english	The relative position (to parent or screen), in pixels.
		//!\~french		La position relative (à l'écran ou au parent), en pixels.
		castor::Position m_pxPosition;
		//!\~english	The absolute size in pixels.
		//!\~french		La taille absolue en pixels.
		castor::Size m_size;
		//!\~english	The size used to compute relative position from pixel position.
		//!\~french		La taille utilisée pour calculer la position relative depuis la position en pixels.
		castor::Size m_computeSize;
		//!\~english	The visibility.
		//!\~french		La visibilité.
		bool m_visible{ true };
		//!\~english	The material used by the overlay.
		//!\~french		Le matériau utilisé par l'incrustation.
		MaterialWPtr m_pMaterial;
		//!\~english	The overlay index.
		//!\~french		L'index de l'overlay.
		int m_index{ 0 };
		//!\~english	The overlay level.
		//!\~french		Le niveau de l'overlay.
		int m_level{ 0 };
		//!\~english	The material name.
		//!\~french		Le nom du matériau.
		castor::String m_strMatName;
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
	};
}

#endif
