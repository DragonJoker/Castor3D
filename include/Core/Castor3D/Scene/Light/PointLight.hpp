/*
See LICENSE file in root folder
*/
#ifndef ___C3D_POINT_LIGHT_H___
#define ___C3D_POINT_LIGHT_H___

#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Scene/Light/LightCategory.hpp"

#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/Math/LuminousIntensity.hpp>
#include <CastorUtils/Math/SquareMatrix.hpp>

namespace c3d
{
	class PointLight
		: public LightCategory
	{
	private:
		PointLight( bool & dirty
			, Function< void() > const & markParentDirty );

	public:
		/**
		 *\copydoc		LightCategory::instantiate
		 */
		C3D_API LightInstanceUPtr instantiate( SceneNode & node
			, Function< bool() > isParentEnabled )override;
		/**
		 *\~english
		 *\brief		Creation function, used by Factory.
		 *\param[in]	dirty			Used to tell the owner some changes have occured.
		 *\param[in]	markParentDirty	Callback to call when changes have occured.
		 *\return		A light source.
		 *\~french
		 *\brief		Fonction de création utilisée par Factory.
		 *\param[in]	dirty			Utilisé pour dire au parent que des changements ont eu lieu.
		 *\param[in]	markParentDirty	Callback à appeler lorsque des changements ont eu lieu.
		 *\return		Une source lumineuse.
		 */
		C3D_API static LightCategoryUPtr create( bool & dirty
			, Function< void() > const & markParentDirty );
		/**
		 *\~english
		 *\return		The vertices needed to draw the mesh materialising the ligh's volume of effect.
		 *\~french
		 *\return		Les sommets nécessaires au dessin du maillage représentant le volume d'effet de la lumière.
		 */
		C3D_API static Point3fArray const & generateVertices();
		/**
		*\~english
		*name
		*	Mutators.
		*\~french
		*name
		*	Mutateurs.
		*/
		/**@{*/
		void setAttenuation( Point3f const & value )
		{
			setRange( getMaxDistance( getColour(), getIntensity(), value ) );
		}

		void setRange( float value )
		{
			m_range = value;
		}

		void setIntensity( LuminousIntensity const & value )
		{
			m_intensity = value;
		}
		/**@}*/
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

		LuminousIntensity const & getIntensity()const noexcept
		{
			return m_intensity;
		}
		/**@}*/

	private:
		void doUpdate()override;
		void doAccept( ConfigurationVisitorBase & vis )override;
		void doCloneInto( LightCategory & output )const override;

	private:
		friend class Scene;

		GroupChangeTracked< float > m_range;
		GroupChangeTracked< LuminousIntensity > m_intensity;
	};

	class PointLightInstance
		: public LightInstance
	{
	public:
		using ShadowData = PointShadowData;
		static constexpr uint32_t ShadowDataSize = uint32_t( ashes::getAlignedSize( sizeof( ShadowData ), LightMbrAlign ) );
		static constexpr uint32_t ShadowDataComponents = ShadowDataSize / LightMbrAlign;

		static constexpr uint32_t LightDataSize = uint32_t( ashes::getAlignedSize( sizeof( LightData ), LightMbrAlign ) );
		static constexpr uint32_t LightDataComponents = LightDataSize / LightMbrAlign;

	public:
		C3D_API PointLightInstance( SceneNode & node
			, PointLight & category
			, Function< void() > markParentDirty
			, Function< bool() > isParentEnabled );
		/**
		 *\~english
		 *\brief		Puts the shadow data into the given buffer.
		 *\param[out]	data	Receives the light's shadow data.
		 *\~french
		 *\brief		Met les données d'ombre dans le buffer donné.
		 *\param[out]	data	Reçoit les données d'ombres de la source lumineuse.
		 */
		C3D_API void fillShadowBuffer( AllShadowData & data )const override;

		Matrix4x4f const & getViewMatrix( CubeMapFace face )const noexcept
		{
			return m_lightViews[size_t( face )];
		}

	private:
		void doUpdate()override;
		void doUpdateShadow( Camera const & viewCamera
			, Camera * lightCamera
			, int32_t index )override;
		void doFillLightBuffer( Point4f * data )const override;
		void doCloneInto( LightInstance & output )const override;

	private:
		GroupChangeTracked< Point3f > m_position;
		Array< Matrix4x4f, size_t( CubeMapFace::eCount ) > m_lightViews;
	};
}

#endif
