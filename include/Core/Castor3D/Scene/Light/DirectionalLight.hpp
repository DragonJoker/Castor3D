/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DIRECTIONAL_LIGHT_H___
#define ___C3D_DIRECTIONAL_LIGHT_H___

#include "Castor3D/Scene/Light/LightCategory.hpp"

#include <CastorUtils/Math/Illumination.hpp>
#include <CastorUtils/Math/SquareMatrix.hpp>

namespace c3d
{
	struct DirectionalLightCascade
	{
		Matrix4x4f viewMatrix;
		Matrix4x4f projMatrix;
		Matrix4x4f viewProjMatrix;
		Point2f splitDepthScale;

	private:
		friend bool operator==( DirectionalLightCascade const & lhs
			, DirectionalLightCascade  const & rhs )noexcept
		{
			return lhs.viewMatrix == rhs.viewMatrix
				&& lhs.projMatrix == rhs.projMatrix
				&& lhs.splitDepthScale == rhs.splitDepthScale;
		}
	};

	class DirectionalLight
		: public LightCategory
	{
	private:
		friend class Scene;

	private:
		DirectionalLight( bool & dirty
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
		 *\name Mutators.
		 *\~french
		 *\name Mutateurs.
		 **/
		/**@{*/
		void setIllumination( Illumination const & value )
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
		Illumination const & getIllumination()const noexcept
		{
			return m_illumination;
		}
		/**@}*/

	private:
		void doUpdate()override;
		void doAccept( ConfigurationVisitorBase & vis )override;
		void doCloneInto( LightCategory & output )const override;

	private:
		GroupChangeTracked< Illumination > m_illumination;
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
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		Point3f const & getDirection()const
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

		Matrix4x4f const & getProjMatrix( uint32_t cascadeIndex )const
		{
			return m_cascades[cascadeIndex].projMatrix;
		}

		Matrix4x4f const & getViewMatrix( uint32_t cascadeIndex )const
		{
			return m_cascades[cascadeIndex].viewMatrix;
		}

		Matrix4x4f const & getLightSpaceTransform( uint32_t cascadeIndex )const
		{
			return m_cascades[cascadeIndex].viewProjMatrix;
		}
		/**@}*/

	private:
		void doUpdate()override;
		void doUpdateShadow( Camera const & viewCamera
			, Camera * lightCamera
			, int32_t index )override;
		void doFillLightBuffer( Point4f * data )const override;
		void doCloneInto( LightInstance & output )const override;

	private:
		Point3f m_direction;
		Vector< Cascade > m_cascades;
		Vector< Cascade > m_prvCascades;
	};
}

#endif
