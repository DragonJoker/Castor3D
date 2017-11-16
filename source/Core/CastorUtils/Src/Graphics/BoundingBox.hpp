/*
See LICENSE file in root folder
*/
#ifndef ___CU_BoundingBox_H___
#define ___CU_BoundingBox_H___

#include "BoundingContainer.hpp"

namespace castor
{
	/*!
	\author		Sylvain DOREMUS
	\date		14/02/2010
	\~english
	\brief		Bounding box class.
	\~french
	\brief		Classe de bounding box.
	*/
	class BoundingBox
		: public BoundingContainer3D
	{
	public:
		CU_API BoundingBox( BoundingBox const & rhs ) = default;
		CU_API BoundingBox( BoundingBox && rhs ) = default;
		CU_API BoundingBox & operator=( BoundingBox const & rhs ) = default;
		CU_API BoundingBox & operator=( BoundingBox && rhs ) = default;
		/**
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructeur.
		 */
		CU_API BoundingBox();
		/**
		 *\~english
		 *\brief		Constructor from min and max extents.
		 *\param[in]	min	The min extent.
		 *\param[in]	max	The max extent.
		 *\~french
		 *\brief		Constructeur à partir des points min et max.
		 *\param[in]	min	Le point minimal.
		 *\param[in]	max	Le point maximal.
		 */
		CU_API BoundingBox( Point3r const & min, Point3r const & max );
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
		CU_API bool isWithin( Point3r const & point )const override;
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
		CU_API bool isOnLimits( Point3r const & point )const override;
		/**
		 *\~english
		 *\brief		Reinitialises the combo box to the given limits.
		 *\param[in]	min	The min extent.
		 *\param[in]	max	The max extent.
		 *\~french
		 *\brief		Réinitialise la boîte aux limites données.
		 *\param[in]	min	Le point minimal.
		 *\param[in]	max	Le point maximal.
		 */
		CU_API void load( Point3r const & min, Point3r const & max );
		/**
		 *\~english
		 *\brief		Retrieves the axis aligned cube box for given tranformations.
		 *\param[in]	transformations	The transformation matrix.
		 *\~french
		 *\brief		Récupère la boîte alignée sur les axes, pour les transformations données.
		 *\param[in]	transformations	La matrice de transformation.
		 */
		CU_API BoundingBox getAxisAligned( Matrix4x4r const & transformations )const;
		/**
		 *\~english
		 *\return		The min extent.
		 *\~french
		 *\return		Le point minimal.
		 */
		inline Point3r const & getMin()const
		{
			return m_min;
		}
		/**
		 *\~english
		 *\return		The max extent.
		 *\~french
		 *\return		Le point maximal.
		 */
		inline Point3r const & getMax()const
		{
			return m_max;
		}

	private:
		//!\~english	The min extent of the bounding box.
		//!\~french		Le point minimal de la bounding box.
		Point3r m_min;
		//!\~english	The max extent of the bounding box.
		//\~french		Le point maximal de la bounding box
		Point3r m_max;
	};
}

#endif
