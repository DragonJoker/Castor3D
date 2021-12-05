/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Spotlight_H___
#define ___C3D_Spotlight_H___

#include "Castor3D/Scene/Light/LightCategory.hpp"

#include <CastorUtils/Design/ChangeTracked.hpp>
#include <CastorUtils/Math/Angle.hpp>
#include <CastorUtils/Math/SquareMatrix.hpp>

namespace castor3d
{
	class SpotLight
		: public LightCategory
	{
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
		C3D_API explicit SpotLight( Light & light );

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
		 *\param[in,out]	lightCamera	The camera that receives the light spot data.
		 *\param[in]		index		The shadow map index.
		 *\~french
		 *\brief			Met à jour les information d'ombre.
		 *\param[in,out]	lightCamera	La caméra qui reçoit les données de spot de la lumière.
		 *\param[in]		index		L'indice de la shadow map.
		 */
		C3D_API void updateShadow( Camera & lightCamera
			, int32_t index );
		/**
		 *\~english
		 *\name Mutators.
		 *\~french
		 *\name Mutateurs.
		 **/
		/**@{*/
		C3D_API void setAttenuation( castor::Point3f const & value );
		C3D_API void setExponent( float value );
		C3D_API void setCutOff( castor::Angle const & value );
		/**@}*/
		/**
		 *\~english
		 *\name Getters.
		 *\~french
		 *\name Accesseurs.
		 **/
		/**@{*/
		castor::Point3f const & getDirection()const
		{
			return m_direction;
		}

		castor::Matrix4x4f const & getViewMatrix()const
		{
			return m_lightView.value();
		}

		castor::Matrix4x4f const & getProjectionMatrix()const
		{
			return m_lightProj.value();
		}

		castor::Matrix4x4f const & getLightSpaceTransform()const
		{
			return m_lightSpace;
		}

		castor::Point3f const & getAttenuation()const
		{
			return m_attenuation.value();
		}

		float getExponent()const
		{
			return m_exponent.value();
		}

		castor::Angle const & getCutOff()const
		{
			return m_cutOff.value();
		}
		/**@}*/

	private:
		void updateNode( SceneNode const & node )override;
		void doFillBuffer( LightBuffer::LightData & data )const override;

	private:
		bool m_dirtyData{ false };
		bool m_dirtyShadow{ false };
		castor::GroupChangeTracked< castor::Point3f > m_attenuation;
		castor::GroupChangeTracked< float > m_exponent;
		castor::GroupChangeTracked< castor::Angle > m_cutOff;
		castor::GroupChangeTracked< castor::Matrix4x4f > m_lightView;
		castor::GroupChangeTracked< castor::Matrix4x4f > m_lightProj;
		castor::Matrix4x4f m_lightSpace;
		castor::Point3f m_direction;
	};
}

#endif
