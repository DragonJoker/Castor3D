/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DIRECTIONAL_LIGHT_H___
#define ___C3D_DIRECTIONAL_LIGHT_H___

#include "Castor3D/Scene/Light/LightCategory.hpp"

#include <CastorUtils/Math/Illumination.hpp>
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
	private:
		friend class Scene;

	private:
		DirectionalLight( bool & dirty
			, castor::Function< void() > const & markParentDirty );

	public:
		/**
		 *\copydoc		LightCategory::instantiate
		 */
		C3D_API LightInstanceUPtr instantiate( SceneNode & node
			, castor::Function< bool() > isParentEnabled )override;
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
			, castor::Function< void() > const & markParentDirty );
		/**
		 *\~english
		 *\name Mutators.
		 *\~french
		 *\name Mutateurs.
		 **/
		/**@{*/
		void setIllumination( castor::Illumination const & value )
		{
			m_illumination = value;
		}
		/**@}*/
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		castor::Illumination const & getIllumination()const noexcept
		{
			return m_illumination;
		}
		/**@}*/

	private:
		void doUpdate()override;
		void doAccept( ConfigurationVisitorBase & vis )override;
		void doCloneInto( LightCategory & output )const override;

	private:
		castor::GroupChangeTracked< castor::Illumination > m_illumination;
	};

	class DirectionalLightInstance
		: public LightInstance
	{
	public:
		using Cascade = DirectionalLightCascade;

		using ShadowData = DirectionalShadowData;
		static constexpr uint32_t ShadowDataSize = uint32_t( ashes::getAlignedSize( sizeof( ShadowData ), LightMbrAlign ) );
		static constexpr uint32_t ShadowDataComponents = ShadowDataSize / LightMbrAlign;

		using LightData = LightInstance::LightData;
		static constexpr uint32_t LightDataSize = uint32_t( ashes::getAlignedSize( sizeof( LightData ), LightMbrAlign ) );
		static constexpr uint32_t LightDataComponents = LightDataSize / LightMbrAlign;

	public:
		C3D_API DirectionalLightInstance( SceneNode & node
			, DirectionalLight & category
			, castor::Function< void() > markParentDirty
			, castor::Function< bool() > isParentEnabled );
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
		void doUpdate()override;
		void doUpdateShadow( Camera const & viewCamera
			, Camera * lightCamera
			, int32_t index )override;
		void doFillLightBuffer( castor::Point4f * data )const override;
		void doCloneInto( LightInstance & output )const override;

	private:
		castor::Point3f m_direction;
		castor::Vector< Cascade > m_cascades;
		castor::Vector< Cascade > m_prvCascades;
	};
}

#endif
