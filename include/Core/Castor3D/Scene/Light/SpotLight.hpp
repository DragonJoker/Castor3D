/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Spotlight_H___
#define ___C3D_Spotlight_H___

#include "Castor3D/Scene/Light/LightCategory.hpp"

#include <CastorUtils/Design/ChangeTracked.hpp>
#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/Math/Angle.hpp>
#include <CastorUtils/Math/LuminousIntensity.hpp>
#include <CastorUtils/Math/SquareMatrix.hpp>

namespace castor3d
{
	class SpotLight
		: public LightCategory
	{
	private:
		friend class Scene;

	private:
		explicit SpotLight( bool & dirty
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
		 *\return		The vertices needed to draw the mesh materialising the ligh's volume of effect.
		 *\~french
		 *\return		Les sommets nécessaires au dessin du maillage représentant le volume d'effet de la lumière.
		 */
		C3D_API static castor::Point3fArray const & generateVertices( uint32_t angle );
		/**
		 *\~english
		 *\name Mutators.
		 *\~french
		 *\name Mutateurs.
		 **/
		/**@{*/
		C3D_API void setAttenuation( castor::Point3f const & value );
		C3D_API void setRange( float value );
		C3D_API void setExponent( float value );
		C3D_API void setIntensity( castor::LuminousIntensity const & value );
		C3D_API void setInnerCutOff( castor::Angle const & value );
		C3D_API void setOuterCutOff( castor::Angle const & value );
		/**@}*/
		/**
		 *\~english
		 *\name Getters.
		 *\~french
		 *\name Accesseurs.
		 **/
		/**@{*/
		float getExponent()const noexcept
		{
			return m_exponent.value();
		}

		castor::Angle const & getInnerCutOff()const noexcept
		{
			return m_innerCutOff.value();
		}

		castor::Angle const & getOuterCutOff()const noexcept
		{
			return m_outerCutOff.value();
		}

		float getRange()const noexcept
		{
			return m_range.value();
		}

		castor::LuminousIntensity const & getIntensity()const noexcept
		{
			return m_intensity;
		}
		/**@}*/

	private:
		void doUpdate()override;
		void doAccept( ConfigurationVisitorBase & vis )override;
		void doCloneInto( LightCategory & output )const override;

	private:
		castor::GroupChangeTracked< float > m_range;
		castor::GroupChangeTracked< float > m_exponent;
		castor::GroupChangeTracked< castor::LuminousIntensity > m_intensity;
		castor::GroupChangeTracked< castor::Angle > m_innerCutOff;
		castor::GroupChangeTracked< castor::Angle > m_outerCutOff;
	};

		class SpotLightInstance
			: public LightInstance
		{
		public:
			using ShadowData = SpotShadowData;
			static constexpr uint32_t ShadowDataSize = uint32_t( ashes::getAlignedSize( sizeof( ShadowData ), LightMbrAlign ) );
			static constexpr uint32_t ShadowDataComponents = ShadowDataSize / LightMbrAlign;

			struct LightData
				: LightInstance::LightData
			{
				Float3 direction;
				Float1 exponent;
				Float1 outerCutoffCos;
				Float1 innerCutoff;
				Float1 outerCutoff;
				Float1 innerCutoffSin;
				Float1 outerCutoffSin;
				Float1 innerCutoffCos;
				Float1 outerCutOffTan;
				Float1 pad0;
			};
			static constexpr uint32_t LightDataSize = uint32_t( ashes::getAlignedSize( sizeof( LightData ), LightMbrAlign ) );
			static constexpr uint32_t LightDataComponents = LightDataSize / LightMbrAlign;

		public:
			C3D_API SpotLightInstance( SceneNode & node
				, SpotLight & category
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
			 *\name Getters.
			 *\~french
			 *\name Accesseurs.
			 **/
			/**@{*/
			castor::Point3f const & getDirection()const noexcept
			{
				return m_direction;
			}

			castor::Matrix4x4f const & getViewMatrix()const noexcept
			{
				return m_lightView.value();
			}

			castor::Matrix4x4f const & getProjectionMatrix()const noexcept
			{
				return m_lightProj.value();
			}

			castor::Matrix4x4f const & getLightSpaceTransform()const noexcept
			{
				return m_lightSpace;
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
			castor::GroupChangeTracked< castor::Matrix4x4f > m_lightView;
			castor::GroupChangeTracked< castor::Matrix4x4f > m_lightProj;
			castor::Matrix4x4f m_lightSpace;
			castor::Point3f m_direction;
		};
}

#endif
