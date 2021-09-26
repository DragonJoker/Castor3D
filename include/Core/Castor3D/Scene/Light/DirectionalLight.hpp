/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DIRECTIONAL_LIGHT_H___
#define ___C3D_DIRECTIONAL_LIGHT_H___

#include "Castor3D/Scene/Light/LightCategory.hpp"

#include <CastorUtils/Math/SquareMatrix.hpp>

namespace castor3d
{
	struct DirectionalLightCascade
	{
		castor::Matrix4x4f viewMatrix;
		castor::Matrix4x4f projMatrix;
		castor::Matrix4x4f viewProjMatrix;
		castor::Point2f splitDepthScale;
	};

	C3D_API bool operator==( DirectionalLightCascade const & lhs
		, DirectionalLightCascade  const & rhs );

	inline bool operator!=( DirectionalLightCascade const & lhs
		, DirectionalLightCascade  const & rhs )
	{
		return !( lhs == rhs );
	}

	class DirectionalLight
		: public LightCategory
	{
	public:
		using Cascade = DirectionalLightCascade;

	private:
		friend class Scene;

	private:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	light	The parent Light.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	light	La Light parente.
		 */
		C3D_API explicit DirectionalLight( Light & light );

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
		 *\copydoc		castor3d::LightCategory::update
		 */
		C3D_API void update()override;
		/**
		 *\~english
		 *\brief		Updates the shadow cascades informations.
		 *\param[in]	sceneCamera		The viewer camera.
		 *\return		\p false if nothing changed.
		 *\~french
		 *\brief		Met à jour les information de shadow cascades.
		 *\param[in]	sceneCamera		La caméra de la scène.
		 *\return		\p false si rien n'a changé.
		 */
		C3D_API bool updateShadow( Camera const & sceneCamera );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		castor::Point3f const & getDirection()const
		{
			return m_direction;
		}

		float getSplitDepth( uint32_t cascadeIndex )const
		{
			return m_cascades[cascadeIndex].splitDepthScale->x;
		}

		float getSplitScale( uint32_t cascadeIndex )const
		{
			return m_cascades[cascadeIndex].splitDepthScale->y;
		}

		castor::Matrix4x4f const & getProjMatrix( uint32_t cascadeIndex )const
		{
			return m_cascades[cascadeIndex].projMatrix;
		}

		castor::Matrix4x4f const & getViewMatrix( uint32_t cascadeIndex )const
		{
			return m_cascades[cascadeIndex].viewMatrix;
		}

		castor::Matrix4x4f const & getLightSpaceTransform( uint32_t cascadeIndex )const
		{
			return m_cascades[cascadeIndex].viewProjMatrix;
		}
		/**@}*/

	private:
		void updateNode( SceneNode const & node )override;
		void doBind( castor::Point4f * buffer )const override;

	private:
		castor::Point3f m_direction;
		std::vector< Cascade > m_cascades;
		std::vector< Cascade > m_prvCascades;
	};
}

#endif
