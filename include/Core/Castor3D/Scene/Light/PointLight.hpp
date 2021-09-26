/*
See LICENSE file in root folder
*/
#ifndef ___C3D_POINT_LIGHT_H___
#define ___C3D_POINT_LIGHT_H___

#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Scene/Light/LightCategory.hpp"

#include <CastorUtils/Design/ChangeTracked.hpp>
#include <CastorUtils/Math/SquareMatrix.hpp>

namespace castor3d
{
	class PointLight
		: public LightCategory
	{

	private:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	light	The parent Light.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	light	La Light parente.
		 */
		C3D_API explicit PointLight( Light & light );

	public:
		/**
		 *\~english
		 *\brief		Creation function, used by Factory.
		 *\param[in]	light	The parent Light.
		 *\return		A light source.
		 *\~french
		 *\brief		Fonction de création utilisée par Factory.
		 *\param[in]	light	La Light parente.
		 *\return		Une source lumineuse.
		 */
		C3D_API static LightCategoryUPtr create( Light & light );
		/**
		 *\~english
		 *\return		The vertices needed to draw the mesh materialising the ligh's volume of effect.
		 *\~french
		 *\return		Les sommets nécessaires au dessin du maillage représentant le volume d'effet de la lumière.
		 */
		C3D_API static castor::Point3fArray const & generateVertices();
		/**
		 *\copydoc		castor3d::LightCategory::update
		 */
		C3D_API void update()override;
		/**
		 *\~english
		 *\brief			Updates the shadow informations.
		 *\param[in]		index		The shadow map index.
		 *\~french
		 *\brief			Met à jour les information d'ombre.
		 *\param[in]		index		L'indice de la shadow map.
		 */
		C3D_API void updateShadow( int32_t index = -1 );
		/**
		 *\~english
		 *\brief		Sets attenuation components.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit les composantes d'atténuation.
		 *\param[in]	value	La nouvelle valeur.
		 */
		C3D_API void setAttenuation( castor::Point3f const & value );
		/**
		 *\~english
		 *\name Getters.
		 *\~french
		 *\name Accesseurs.
		 **/
		/**@{*/
		castor::Point3f const & getAttenuation()const
		{
			return m_attenuation.value();
		}

		castor::Matrix4x4f const & getViewMatrix( CubeMapFace face )const
		{
			return m_lightViews[size_t( face )];
		}
		/**@}*/

	private:
		void updateNode( SceneNode const & node )override;
		void doBind( castor::Point4f * buffer )const override;

	private:
		friend class Scene;
		//!\~english	The attenuation components : constant, linear and quadratic.
		//!\~french		Les composantes d'attenuation : constante, linéaire et quadratique.
		castor::ChangeTracked< castor::Point3f > m_attenuation{ castor::Point3f{ 1.0f, 0.0f, 0.0f } };
		std::array< castor::Matrix4x4f, size_t( CubeMapFace::eCount ) > m_lightViews;
	};
}

#endif
