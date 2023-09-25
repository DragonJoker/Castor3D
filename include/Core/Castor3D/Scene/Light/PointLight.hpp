/*
See LICENSE file in root folder
*/
#ifndef ___C3D_POINT_LIGHT_H___
#define ___C3D_POINT_LIGHT_H___

#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Scene/Light/LightCategory.hpp"

#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/Math/SquareMatrix.hpp>

namespace castor3d
{
	class PointLight
		: public LightCategory
	{
	public:
		using ShadowData = PointShadowData;
		static constexpr uint32_t ShadowDataSize = uint32_t( ashes::getAlignedSize( sizeof( ShadowData ), LightMbrAlign ) );
		static constexpr uint32_t ShadowDataComponents = ShadowDataSize / LightMbrAlign;

		static constexpr uint32_t LightDataSize = uint32_t( ashes::getAlignedSize( sizeof( LightData ), LightMbrAlign ) );
		static constexpr uint32_t LightDataComponents = LightDataSize / LightMbrAlign;

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
		C3D_API void updateShadow( int32_t index );
		/**
		 *\~english
		 *\brief		Puts the shadow data into the given buffer.
		 *\param[out]	data	Receives the light's shadow data.
		 *\~french
		 *\brief		Met les données d'ombre dans le buffer donné.
		 *\param[out]	data	Reçoit les données d'ombres de la source lumineuse.
		 */
		C3D_API void fillShadowBuffer( AllShadowData & data )const override;
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
		 *\brief		Sets range value.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit la valeur de ditance maximale.
		 *\param[in]	value	La nouvelle valeur.
		 */
		C3D_API void setRange( float value );
		/**
		 *\~english
		 *\name Getters.
		 *\~french
		 *\name Accesseurs.
		 **/
		/**@{*/
		float getRange()const noexcept
		{
			return m_range.value();
		}

		castor::Matrix4x4f const & getViewMatrix( CubeMapFace face )const noexcept
		{
			return m_lightViews[size_t( face )];
		}
		/**@}*/

	private:
		void doFillLightBuffer( castor::Point4f * data )const override;
		void doAccept( ConfigurationVisitorBase & vis )override;

	private:
		friend class Scene;
		bool m_dirtyData{ false };
		castor::GroupChangeTracked< float > m_range;
		castor::GroupChangeTracked< castor::Point3f > m_position;
		std::array< castor::Matrix4x4f, size_t( CubeMapFace::eCount ) > m_lightViews;
	};
}

#endif
