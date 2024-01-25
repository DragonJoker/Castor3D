/*
See LICENSE file in root folder
*/
#ifndef ___CU_BoundingContainer_H___
#define ___CU_BoundingContainer_H___

#include "CastorUtils/Graphics/GraphicsModule.hpp"

#include "CastorUtils/Math/Point.hpp"

namespace castor
{
	template< uint8_t Dimension >
	class BoundingContainer
	{
	public:
		using MyPoint = Point< float, Dimension >;
		CU_API BoundingContainer() = default;
		CU_API BoundingContainer( BoundingContainer const & ) = default;
		CU_API BoundingContainer( BoundingContainer && )noexcept = default;
		CU_API BoundingContainer & operator=( BoundingContainer const & ) = default;
		CU_API BoundingContainer & operator=( BoundingContainer && )noexcept = default;
		CU_API virtual ~BoundingContainer()noexcept = default;
		/**
		 *\~english
		 *\brief		Constructor from center.
		 *\param[in]	center	The center.
		 *\~french
		 *\brief		Constructeur à partir du centre.
		 *\param[in]	center	Le centre.
		 */
		explicit BoundingContainer( MyPoint center )
			: m_center{ castor::move( center ) }
		{
		}
		/**
		 *\~english
		 *\brief		Tests if a vertex is within the container, id est inside it but not on it's limits.
		 *\param[in]	point	The vertex to test.
		 *\return		\p true if the vertex is within the container, false if not.
		 *\~french
		 *\brief		Teste si un point est contenu dans le conteneur (mais pas à la limite).
		 *\param[in]	point	Le point à tester.
		 *\return		\p true si le point est dans le conteneur.
		 */
		CU_API virtual bool isWithin( MyPoint const & point )const = 0;
		/**
		 *\~english
		 *\brief		Tests if a vertex is on the limits of this container, and not within.
		 *\param[in]	point	The vertex to test.
		 *\return		\p true if the vertex is on the limits of the container, false if not.
		 *\~french
		 *\brief		Teste si un point est sur la limite du conteneur, et pas dedans.
		 *\param[in]	point	Le point à tester.
		 *\return		\p true si le point est sur la limite.
		 */
		CU_API virtual bool isOnLimits( MyPoint const & point )const = 0;
		/**
		 *\~english
		 *\return		This container's center.
		 *\~french
		 *\return		Le centre de ce conteneur.
		 */
		MyPoint const & getCenter()const
		{
			return m_center;
		}

	protected:
		void setCenter( MyPoint center )noexcept
		{
			m_center = castor::move( center );
		}

	private:
		//!\~english	The center of the container.
		//!\~french		Le centre de ce conteneur.
		MyPoint m_center{};
	};
}

#endif
