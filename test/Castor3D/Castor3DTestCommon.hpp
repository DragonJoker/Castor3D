/* See LICENSE file in root folder */
#ifndef ___C3DT_COMMON_H___
#define ___C3DT_COMMON_H___

#include <Castor3D/Animation/Animable.hpp>
#include <Castor3D/Animation/Animation.hpp>
#include <Castor3D/Cache/AnimatedObjectGroupCache.hpp>
#include <Castor3D/Cache/GeometryCache.hpp>
#include <Castor3D/Cache/LightCache.hpp>
#include <Castor3D/Engine.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Model/Mesh/Animation/MeshMorphTarget.hpp>
#include <Castor3D/Model/Mesh/Mesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/SkinComponent.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/LineMapping.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/BaseDataComponent.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/TriFaceMapping.hpp>
#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Model/Skeleton/Animation/SkeletonAnimation.hpp>
#include <Castor3D/Model/Skeleton/Animation/SkeletonAnimationKeyFrame.hpp>
#include <Castor3D/Model/Skeleton/Animation/SkeletonAnimationObject.hpp>
#include <Castor3D/Model/Skeleton/BoneNode.hpp>
#include <Castor3D/Model/Skeleton/Skeleton.hpp>
#include <Castor3D/Model/Skeleton/VertexBoneData.hpp>
#include <Castor3D/Render/Viewport.hpp>
#include <Castor3D/Scene/Animation/AnimatedObject.hpp>
#include <Castor3D/Scene/Animation/AnimatedObjectGroup.hpp>
#include <Castor3D/Scene/Animation/AnimatedSkeleton.hpp>
#include <Castor3D/Scene/Animation/AnimationInstance.hpp>
#include <Castor3D/Scene/Animation/Skeleton/SkeletonAnimationInstance.hpp>
#include <Castor3D/Scene/Animation/Skeleton/SkeletonAnimationInstanceObject.hpp>
#include <Castor3D/Scene/Animation/Skeleton/SkeletonAnimationModule.hpp>
#include <Castor3D/Scene/Camera.hpp>
#include <Castor3D/Scene/Geometry.hpp>
#include <Castor3D/Scene/Light/DirectionalLight.hpp>
#include <Castor3D/Scene/Light/Light.hpp>
#include <Castor3D/Scene/Light/PointLight.hpp>
#include <Castor3D/Scene/Light/SpotLight.hpp>
#include <Castor3D/Scene/MovableObject.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/SceneNode.hpp>

#include <CastorUtils/Design/ArrayView.hpp>
#include <CastorUtils/Design/ChangeTracked.hpp>
#include <CastorUtils/Math/Point.hpp>

#include <UnitTest.hpp>

namespace Testing
{
	//*********************************************************************************************

	using c3d::operator<<;
	using c3d::operator<<;

	template<>
	struct Stringifier< c3d::Scene >
	{
		static std::string get( c3d::Scene const & value )
		{
			std::stringstream stream;
			stream << c3d::toUtf8( value.getName() );
			return stream.str();
		}
	};

	template<>
	struct Stringifier< c3d::SceneNode >
	{
		static std::string get( c3d::SceneNode const & value )
		{
			std::stringstream stream;
			stream << c3d::toUtf8( value.getName() );
			return stream.str();
		}
	};

	template<>
	struct Stringifier< c3d::MovableObject >
	{
		static std::string get( c3d::MovableObject const & value )
		{
			std::stringstream stream;
			stream << c3d::toUtf8( value.getName() );
			return stream.str();
		}
	};

	template<>
	struct Stringifier< c3d::Geometry >
	{
		static std::string get( c3d::Geometry const & value )
		{
			std::stringstream stream;
			stream << c3d::toUtf8( value.getName() );
			return stream.str();
		}
	};

	template<>
	struct Stringifier< c3d::Light >
	{
		static std::string get( c3d::Light const & value )
		{
			std::stringstream stream;
			stream << c3d::toUtf8( value.getName() );
			return stream.str();
		}
	};

	template<>
	struct Stringifier< c3d::Camera >
	{
		static std::string get( c3d::Camera const & value )
		{
			std::stringstream stream;
			stream << c3d::toUtf8( value.getName() );
			return stream.str();
		}
	};

	template<>
	struct Stringifier< c3d::Material >
	{
		static std::string get( c3d::Material const & value )
		{
			std::stringstream stream;
			stream << c3d::toUtf8( value.getName() );
			return stream.str();
		}
	};

	template<>
	struct Stringifier< c3d::Mesh >
	{
		static std::string get( c3d::Mesh const & value )
		{
			std::stringstream stream;
			stream << c3d::toUtf8( value.getName() );
			return stream.str();
		}
	};

	template<>
	struct Stringifier< c3d::BoneNode >
	{
		static std::string get( c3d::BoneNode const & value )
		{
			std::stringstream stream;
			stream << c3d::toUtf8( value.getName() );
			return stream.str();
		}
	};

	template<>
	struct Stringifier< c3d::SkeletonNode >
	{
		static std::string get( c3d::SkeletonNode const & value )
		{
			std::stringstream stream;
			stream << c3d::toUtf8( value.getName() );
			return stream.str();
		}
	};

	template<>
	struct Stringifier< c3d::Face >
	{
		static std::string get( c3d::Face const & value )
		{
			std::stringstream stream;
			stream << value[0] << " " << value[1] << " " << value[2];
			return stream.str();
		}
	};

	template<>
	struct Stringifier< c3d::Line >
	{
		static std::string get( c3d::Line const & value )
		{
			std::stringstream stream;
			stream << value[0] << " " << value[1];
			return stream.str();
		}
	};

	template<>
	struct Stringifier< c3d::Animation >
	{
		static std::string get( c3d::Animation const & value )
		{
			std::stringstream stream;
			stream << c3d::toUtf8( value.getName() );
			return stream.str();
		}
	};

	template<>
	struct Stringifier< c3d::SkeletonAnimation >
	{
		static std::string get( c3d::SkeletonAnimation const & value )
		{
			std::stringstream stream;
			stream << c3d::toUtf8( value.getName() );
			return stream.str();
		}
	};

	template<>
	struct Stringifier< c3d::SkeletonAnimationObject >
	{
		static std::string get( c3d::SkeletonAnimationObject const & value )
		{
			std::stringstream stream;
			stream << c3d::toUtf8( value.getName() );
			return stream.str();
		}
	};

	template<>
	struct Stringifier< c3d::AnimatedObjectGroup >
	{
		static std::string get( c3d::AnimatedObjectGroup const & value )
		{
			std::stringstream stream;
			stream << c3d::toUtf8( value.getName() );
			return stream.str();
		}
	};

	template<>
	struct Stringifier< c3d::AnimatedObject >
	{
		static std::string get( c3d::AnimatedObject const & value )
		{
			std::stringstream stream;
			stream << c3d::toUtf8( value.getName() );
			return stream.str();
		}
	};

	template<>
	struct Stringifier< c3d::AnimationInstance >
	{
		static std::string get( c3d::AnimationInstance const & value )
		{
			std::stringstream stream;
			stream << c3d::toUtf8( value.getAnimation().getName() );
			return stream.str();
		}
	};

	template<>
	struct Stringifier< c3d::InterpolatorType >
	{
		static std::string get( c3d::InterpolatorType const & value )
		{
			static c3d::Map< c3d::InterpolatorType, std::string > Names
			{
				{ c3d::InterpolatorType::eNearest, "Nearest" },
				{ c3d::InterpolatorType::eLinear, "Linear" },
			};
			return Names[value];
		}
	};

	template<>
	struct Stringifier< c3d::AnimationType >
	{
		static std::string get( c3d::AnimationType const & value )
		{
			static c3d::Map< c3d::AnimationType, std::string > Names
			{
				{ c3d::AnimationType::eSceneNode, "SceneNode" },
				{ c3d::AnimationType::eMesh, "Mesh" },
				{ c3d::AnimationType::eSkeleton, "Skeleton" },
			};
			return Names[value];
		}
	};

	template<>
	struct Stringifier< c3d::SkeletonNodeType >
	{
		static std::string get( c3d::SkeletonNodeType const & value )
		{
			static c3d::Map< c3d::SkeletonNodeType, std::string > Names
			{
				{ c3d::SkeletonNodeType::eBone, "Bone" },
				{ c3d::SkeletonNodeType::eNode, "Node" },
			};
			return Names[value];
		}
	};

	template<>
	struct Stringifier< c3d::AnimationState >
	{
		static std::string get( c3d::AnimationState const & value )
		{
			static c3d::Map< c3d::AnimationState, std::string > Names
			{
				{ c3d::AnimationState::ePlaying, "Playing" },
				{ c3d::AnimationState::eStopped, "Stopped" },
				{ c3d::AnimationState::ePaused, "Paused" },
			};
			return Names[value];
		}
	};

	template<>
	struct Stringifier< c3d::ViewportType >
	{
		static std::string get( c3d::ViewportType const & value )
		{
			static c3d::Map< c3d::ViewportType, std::string > Names
			{
				{ c3d::ViewportType::eOrtho, "Ortho" },
				{ c3d::ViewportType::ePerspective, "Perspective" },
				{ c3d::ViewportType::eInfinitePerspective, "Infinite Perspective" },
				{ c3d::ViewportType::eFrustum, "Frustum" },
			};
			return Names[value];
		}
	};

	template<>
	struct Stringifier< c3d::SubmeshData >
	{
		static std::string get( c3d::SubmeshData const & value )
		{
			static c3d::Map< c3d::SubmeshData, std::string > Names
			{
				{ c3d::SubmeshData::eIndex, "eIndex" },
				{ c3d::SubmeshData::ePositions, "ePositions" },
				{ c3d::SubmeshData::eNormals, "eNormals" },
				{ c3d::SubmeshData::eTangents, "eTangents" },
				{ c3d::SubmeshData::eBitangents, "eBitangents" },
				{ c3d::SubmeshData::eTexcoords0, "eTexcoords0" },
				{ c3d::SubmeshData::eTexcoords1, "eTexcoords1" },
				{ c3d::SubmeshData::eTexcoords2, "eTexcoords2" },
				{ c3d::SubmeshData::eTexcoords3, "eTexcoords3" },
				{ c3d::SubmeshData::eColours, "eColours" },
				{ c3d::SubmeshData::eSkin, "eSkin" },
				{ c3d::SubmeshData::ePassMasks, "ePassMasks" },
				{ c3d::SubmeshData::eVelocity, "eVelocity" },
				{ c3d::SubmeshData::eMeshlets, "eMeshlets" },
			};
			return Names[value];
		}
	};

	template<>
	struct Stringifier< c3d::MorphFlag >
	{
		static std::string get( c3d::MorphFlag const & value )
		{
			static c3d::Map< c3d::MorphFlag, std::string > Names
			{
				{ c3d::MorphFlag::ePositions, "ePositions" },
				{ c3d::MorphFlag::eNormals, "eNormals" },
				{ c3d::MorphFlag::eTangents, "eTangents" },
				{ c3d::MorphFlag::eBitangents, "eBitangents" },
				{ c3d::MorphFlag::eTexcoords0, "eTexcoords0" },
				{ c3d::MorphFlag::eTexcoords1, "eTexcoords1" },
				{ c3d::MorphFlag::eTexcoords2, "eTexcoords2" },
				{ c3d::MorphFlag::eTexcoords3, "eTexcoords3" },
				{ c3d::MorphFlag::eColours, "eColours" },
			};
			return Names[value];
		}
	};

	template<>
	struct Stringifier< c3d::ShaderFlag >
	{
		static std::string get( c3d::ShaderFlag const & value )
		{
			static c3d::Map< c3d::ShaderFlag, std::string > Names
			{
				{ c3d::ShaderFlag::eNormal, "eNormal" },
				{ c3d::ShaderFlag::eTangentSpace, "eTangentSpace" },
				{ c3d::ShaderFlag::eVelocity, "eVelocity" },
				{ c3d::ShaderFlag::eWorldSpace, "eWorldSpace" },
				{ c3d::ShaderFlag::eViewSpace, "eViewSpace" },
				{ c3d::ShaderFlag::eDepth, "eDepth" },
				{ c3d::ShaderFlag::eVisibility, "eVisibility" },
				{ c3d::ShaderFlag::ePicking, "ePicking" },
				{ c3d::ShaderFlag::eLighting, "eLighting" },
				{ c3d::ShaderFlag::eShadowMapDirectional, "eShadowMapDirectional" },
				{ c3d::ShaderFlag::eShadowMapSpot, "eShadowMapSpot" },
				{ c3d::ShaderFlag::eShadowMapPoint, "eShadowMapPoint" },
				{ c3d::ShaderFlag::eVsmShadowMap, "eVsmShadowMap" },
				{ c3d::ShaderFlag::eRsmShadowMap, "eRsmShadowMap" },
				{ c3d::ShaderFlag::eEnvironmentMapping, "eEnvironmentMapping" },
				{ c3d::ShaderFlag::eGeometry, "eGeometry" },
				{ c3d::ShaderFlag::eTessellation, "eTessellation" },
				{ c3d::ShaderFlag::eForceTexCoords, "eForceTexCoords" },
				{ c3d::ShaderFlag::eColour, "eColour" },
			};
			return Names[value];
		}
	};

	template<>
	struct Stringifier< c3d::ProgramFlag >
	{
		static std::string get( c3d::ProgramFlag const & value )
		{
			static c3d::Map< c3d::ProgramFlag, std::string > Names
			{
				{ c3d::ProgramFlag::eInstantiation, "eInstantiation" },
				{ c3d::ProgramFlag::eBillboards, "eBillboards" },
				{ c3d::ProgramFlag::eFrontCulled, "eFrontCulled" },
				{ c3d::ProgramFlag::eHasMesh, "eHasMesh" },
				{ c3d::ProgramFlag::eHasTask, "eHasTask" },
			};
			return Names[value];
		}
	};

	template<>
	struct Stringifier< c3d::SubmeshComponent >
	{
		static std::string get( c3d::SubmeshComponent const & value )
		{
			return c3d::toUtf8( value.getType() );
		}
	};

	template<>
	struct Stringifier< c3d::TriFaceMapping >
	{
		static std::string get( c3d::TriFaceMapping const & )
		{
			return std::string{ "c3d::TriFaceMapping" };
		}
	};

	template<>
	struct Stringifier< c3d::LineMapping >
	{
		static std::string get( c3d::LineMapping const & )
		{
			return std::string{ "c3d::LineMapping" };
		}
	};

	template<>
	struct Stringifier< c3d::VertexBoneData::Ids >
	{
		static std::string get( c3d::VertexBoneData::Ids const & value )
		{
			std::string result;
			std::string sep;

			for ( auto & v : value )
			{
				result += sep + toString( v );
				sep = ", ";
			}

			return result;
		}
	};

	template<>
	struct Stringifier< c3d::VertexBoneData::Weights >
	{
		static std::string get( c3d::VertexBoneData::Weights const & value )
		{
			std::string result;
			std::string sep;

			for ( auto & v : value )
			{
				result += sep + toString( v );
				sep = ", ";
			}

			return result;
		}
	};

	template<>
	struct Stringifier< c3d::AllocationStats >
	{
		static std::string get( c3d::AllocationStats const & value )
		{
			return toString( value.available ) + cuT( "/" ) + toString( value.total );
		}
	};

	template<>
	struct Stringifier< c3d::DeviceCounts >
	{
		static std::string get( c3d::DeviceCounts const & )
		{
			return std::string{ "c3d::DeviceCounts" };
		}
	};

	template<>
	struct Stringifier< c3d::EngineCounts >
	{
		static std::string get( c3d::EngineCounts const & )
		{
			return std::string{ "c3d::EngineCounts" };
		}
	};

	template<>
	struct Stringifier< c3d::Animable >
	{
		static std::string get( c3d::Animable const & )
		{
			return std::string{ "c3d::Animable" };
		}
	};

	template<>
	struct Stringifier< c3d::LightCategory >
	{
		static std::string get( c3d::LightCategory const & )
		{
			return std::string{ "c3d::LightCategory" };
		}
	};

	template<>
	struct Stringifier< c3d::DirectionalLight >
	{
		static std::string get( c3d::DirectionalLight const & )
		{
			return std::string{ "c3d::DirectionalLight" };
		}
	};

	template<>
	struct Stringifier< c3d::PointLight >
	{
		static std::string get( c3d::PointLight const & )
		{
			return std::string{ "c3d::PointLight" };
		}
	};

	template<>
	struct Stringifier< c3d::SpotLight >
	{
		static std::string get( c3d::SpotLight const & )
		{
			return std::string{ "c3d::SpotLight" };
		}
	};

	template<>
	struct Stringifier< c3d::DirectionalLightInstance >
	{
		static std::string get( c3d::DirectionalLightInstance const & )
		{
			return std::string{ "c3d::DirectionalLightInstance" };
		}
	};

	template<>
	struct Stringifier< c3d::PointLightInstance >
	{
		static std::string get( c3d::PointLightInstance const & )
		{
			return std::string{ "c3d::PointLightInstance" };
		}
	};

	template<>
	struct Stringifier< c3d::SpotLightInstance >
	{
		static std::string get( c3d::SpotLightInstance const & )
		{
			return std::string{ "c3d::SpotLightInstance" };
		}
	};

	template<>
	struct Stringifier< c3d::Viewport >
	{
		static std::string get( c3d::Viewport const & )
		{
			return std::string{ "c3d::Viewport" };
		}
	};

	template<>
	struct Stringifier< c3d::Pass >
	{
		static std::string get( c3d::Pass const & )
		{
			return std::string{ "c3d::Pass" };
		}
	};

	template<>
	struct Stringifier< c3d::Submesh >
	{
		static std::string get( c3d::Submesh const & )
		{
			return std::string{ "c3d::Submesh" };
		}
	};

	template<>
	struct Stringifier< c3d::Skeleton >
	{
		static std::string get( c3d::Skeleton const & )
		{
			return std::string{ "c3d::Skeleton" };
		}
	};

	template<>
	struct Stringifier< c3d::AnimationKeyFrame >
	{
		static std::string get( c3d::AnimationKeyFrame const & )
		{
			return std::string{ "c3d::AnimationKeyFrame" };
		}
	};

	template<>
	struct Stringifier< c3d::MeshMorphTarget >
	{
		static std::string get( c3d::MeshMorphTarget const & )
		{
			return std::string{ "c3d::MeshMorphTarget" };
		}
	};

	template<>
	struct Stringifier< c3d::SkeletonAnimationKeyFrame >
	{
		static std::string get( c3d::SkeletonAnimationKeyFrame const & )
		{
			return std::string{ "c3d::SkeletonAnimationKeyFrame" };
		}
	};

	template<>
	struct Stringifier< c3d::SkeletonAnimationInstance >
	{
		static std::string get( c3d::SkeletonAnimationInstance const & )
		{
			return std::string{ "c3d::SkeletonAnimationInstance" };
		}
	};

	template<>
	struct Stringifier< c3d::SkeletonAnimationInstanceObject >
	{
		static std::string get( c3d::SkeletonAnimationInstanceObject const & )
		{
			return std::string{ "c3d::SkeletonAnimationInstanceObject" };
		}
	};

	template<>
	struct Stringifier< c3d::SkeletonAnimationInstanceKeyFrame >
	{
		static std::string get( c3d::SkeletonAnimationInstanceKeyFrame const & )
		{
			return std::string{ "c3d::SkeletonAnimationInstanceKeyFrame" };
		}
	};

	template<>
	struct Stringifier< c3d::PositionsComponent >
	{
		static std::string get( c3d::PositionsComponent const & )
		{
			return std::string{ "c3d::PositionsComponent" };
		}
	};

	template<>
	struct Stringifier< c3d::NormalsComponent >
	{
		static std::string get( c3d::NormalsComponent const & )
		{
			return std::string{ "c3d::NormalsComponent" };
		}
	};

	template<>
	struct Stringifier< c3d::TangentsComponent >
	{
		static std::string get( c3d::TangentsComponent const & )
		{
			return std::string{ "c3d::TangentsComponent" };
		}
	};

	template<>
	struct Stringifier< c3d::BitangentsComponent >
	{
		static std::string get( c3d::BitangentsComponent const & )
		{
			return std::string{ "c3d::BitangentsComponent" };
		}
	};

	template<>
	struct Stringifier< c3d::Texcoords0Component >
	{
		static std::string get( c3d::Texcoords0Component const & )
		{
			return std::string{ "c3d::Texcoords0Component" };
		}
	};

	template<>
	struct Stringifier< c3d::Texcoords1Component >
	{
		static std::string get( c3d::Texcoords1Component const & )
		{
			return std::string{ "c3d::Texcoords1Component" };
		}
	};

	template<>
	struct Stringifier< c3d::Texcoords2Component >
	{
		static std::string get( c3d::Texcoords2Component const & )
		{
			return std::string{ "c3d::Texcoords2Component" };
		}
	};

	template<>
	struct Stringifier< c3d::Texcoords3Component >
	{
		static std::string get( c3d::Texcoords3Component const & )
		{
			return std::string{ "c3d::Texcoords3Component" };
		}
	};

	template<>
	struct Stringifier< c3d::ColoursComponent >
	{
		static std::string get( c3d::ColoursComponent const & )
		{
			return std::string{ "c3d::ColoursComponent" };
		}
	};

	template<>
	struct Stringifier< c3d::SkinComponent >
	{
		static std::string get( c3d::SkinComponent const & )
		{
			return std::string{ "c3d::SkinComponent" };
		}
	};

	template<>
	struct Stringifier< c3d::VertexBoneData >
	{
		static std::string get( c3d::VertexBoneData const & )
		{
			return std::string{ "c3d::VertexBoneData" };
		}
	};

	template<>
	struct Stringifier< c3d::InterleavedVertex >
	{
		static std::string get( c3d::InterleavedVertex const & )
		{
			return std::string{ "c3d::InterleavedVertex" };
		}
	};

	template< typename FlagT >
	struct Stringifier< c3d::FlagCombination< FlagT > >
	{
		static inline std::string toString( c3d::FlagCombination< FlagT > const & values )
		{
			int i = ( sizeof( FlagT ) * 8 ) - 1;
			std::string sep;
			std::string result;

			while ( i >= 0 )
			{
				if ( auto value = FlagT( 0x0000000000000001 << i );
					c3d::checkFlag( values, value ) )
				{
					result += sep + toString( value );
					sep = " | ";
				}

				--i;
			}

			if ( result.empty() )
			{
				result = "0";
			}

			return result;
		}
	};

	template< typename ValueT, uint32_t ColumnsT, uint32_t RowsT >
	struct Stringifier< c3d::Matrix< ValueT, ColumnsT, RowsT > >
	{
		static std::string get( c3d::Matrix< ValueT, ColumnsT, RowsT > const & value )
		{
			std::stringstream stream;
			stream.precision( 10 );

			for ( uint32_t i = 0; i < ColumnsT; i++ )
			{
				for ( uint32_t j = 0; j < RowsT; j++ )
				{
					stream << std::setw( 15 ) << std::right << value[i][j];
				}

				stream << std::endl;
			}

			return stream.str();
		}
	};

	template< typename ValueT, uint32_t CountT >
	struct Stringifier< c3d::SquareMatrix< ValueT, CountT > >
	{
		static std::string get( c3d::SquareMatrix< ValueT, CountT > const & value )
		{
			std::stringstream stream;
			stream.precision( 10 );

			for ( uint32_t i = 0; i < CountT; i++ )
			{
				for ( uint32_t j = 0; j < CountT; j++ )
				{
					stream << std::setw( 15 ) << std::right << value[i][j];
				}

				stream << std::endl;
			}

			return stream.str();
		}
	};

	template< typename ValueT, uint32_t CountT >
	struct Stringifier< c3d::Point< ValueT, CountT > >
	{
		static std::string get( c3d::Point< ValueT, CountT > const & value )
		{
			std::stringstream stream;
			stream.precision( 10 );

			for ( uint32_t i = 0; i < CountT; i++ )
			{
				stream << std::setw( 15 ) << std::right << value[i];
			}

			return stream.str();
		}
	};

	template< typename ValueT >
	struct Stringifier< c3d::AngleT< ValueT > >
	{
		static std::string get( c3d::AngleT< ValueT > const & value )
		{
			std::stringstream stream;
			stream << value.degrees();
			return stream.str();
		}
	};

	template<>
	struct Stringifier< c3d::LuminousIntensity >
	{
		static std::string get( c3d::LuminousIntensity const & value )
		{
			std::stringstream stream;
			stream << value.candela();
			return stream.str();
		}
	};

	template<>
	struct Stringifier< c3d::Illumination >
	{
		static std::string get( c3d::Illumination const & value )
		{
			std::stringstream stream;
			stream << value.lux();
			return stream.str();
		}
	};

	template< typename ValueT >
	struct Stringifier< c3d::QuaternionT< ValueT > >
	{
		static std::string get( c3d::QuaternionT< ValueT > const & value )
		{
			std::stringstream stream;
			stream << value;
			return stream.str();
		}
	};

	template< typename ValueT >
	struct Stringifier< c3d::ChangeTracked< ValueT > >
	{
		static std::string get( c3d::ChangeTracked< ValueT > const & value )
		{
			std::stringstream stream;
			stream << toString( *value );
			return stream.str();
		}
	};

	template< typename ValueT >
	struct Stringifier< c3d::ArrayView< ValueT > >
	{
		static std::string get( c3d::ArrayView< ValueT > const & values )
		{
			std::stringstream stream;
			stream << values.size() << ":";

			for ( auto & value : values )
			{
				stream << " " << toString( value );
			}

			return stream.str();
		}
	};

	//*********************************************************************************************

	class C3DTestCase
		: public TestCase
	{
	public:
		C3DTestCase( std::string const & name
			, c3d::Engine & engine );

	protected:
		using TestCase::compare;
		bool compare( c3d::Angle const & lhs, c3d::Angle const & rhs );
		bool compare( c3d::Quaternion const & lhs, c3d::Quaternion const & rhs );
		bool compare( c3d::Scene const & lhs, c3d::Scene const & rhs );
		bool compare( c3d::SceneNode const & lhs, c3d::SceneNode const & rhs );
		bool compare( c3d::Animable const & lhs, c3d::Animable const & rhs );
		bool compare( c3d::MovableObject const & lhs, c3d::MovableObject const & rhs );
		bool compare( c3d::Camera const & lhs, c3d::Camera const & rhs );
		bool compare( c3d::Light const & lhs, c3d::Light const & rhs );
		bool compare( c3d::Geometry const & lhs, c3d::Geometry const & rhs );
		bool compare( c3d::LightCategory const & lhs, c3d::LightCategory const & rhs );
		bool compare( c3d::DirectionalLight const & lhs, c3d::DirectionalLight const & rhs );
		bool compare( c3d::PointLight const & lhs, c3d::PointLight const & rhs );
		bool compare( c3d::SpotLight const & lhs, c3d::SpotLight const & rhs );
		bool compare( c3d::LightInstance const & lhs, c3d::LightInstance const & rhs );
		bool compare( c3d::DirectionalLightInstance const & lhs, c3d::DirectionalLightInstance const & rhs );
		bool compare( c3d::PointLightInstance const & lhs, c3d::PointLightInstance const & rhs );
		bool compare( c3d::SpotLightInstance const & lhs, c3d::SpotLightInstance const & rhs );
		bool compare( c3d::Viewport const & lhs, c3d::Viewport const & rhs );
		bool compare( c3d::Mesh const & lhs, c3d::Mesh const & rhs );
		bool compare( c3d::Submesh const & lhs, c3d::Submesh const & rhs );
		bool compare( c3d::SubmeshComponent const & lhs, c3d::SubmeshComponent const & rhs );
		bool compare( c3d::SkinComponent const & lhs, c3d::SkinComponent const & rhs );
		bool compare( c3d::PositionsComponent const & lhs, c3d::PositionsComponent const & rhs );
		bool compare( c3d::NormalsComponent const & lhs, c3d::NormalsComponent const & rhs );
		bool compare( c3d::TangentsComponent const & lhs, c3d::TangentsComponent const & rhs );
		bool compare( c3d::BitangentsComponent const & lhs, c3d::BitangentsComponent const & rhs );
		bool compare( c3d::Texcoords0Component const & lhs, c3d::Texcoords0Component const & rhs );
		bool compare( c3d::Texcoords1Component const & lhs, c3d::Texcoords1Component const & rhs );
		bool compare( c3d::Texcoords2Component const & lhs, c3d::Texcoords2Component const & rhs );
		bool compare( c3d::Texcoords3Component const & lhs, c3d::Texcoords3Component const & rhs );
		bool compare( c3d::ColoursComponent const & lhs, c3d::ColoursComponent const & rhs );
		bool compare( c3d::Face const & lhs, c3d::Face const & rhs );
		bool compare( c3d::TriFaceMapping const & lhs, c3d::TriFaceMapping const & rhs );
		bool compare( c3d::Line const & lhs, c3d::Line const & rhs );
		bool compare( c3d::LineMapping const & lhs, c3d::LineMapping const & rhs );
		bool compare( c3d::Skeleton const & lhs, c3d::Skeleton const & rhs );
		bool compare( c3d::SkeletonNode const & lhs, c3d::SkeletonNode const & rhs );
		bool compare( c3d::BoneNode const & lhs, c3d::BoneNode const & rhs );
		bool compare( c3d::Animation const & lhs, c3d::Animation const & rhs );
		bool compare( c3d::SkeletonAnimation const & lhs, c3d::SkeletonAnimation const & rhs );
		bool compare( c3d::SkeletonAnimationObject const & lhs, c3d::SkeletonAnimationObject const & rhs );
		bool compare( c3d::AnimationKeyFrame const & lhs, c3d::AnimationKeyFrame const & rhs );
		bool compare( c3d::MeshMorphTarget const & lhs, c3d::MeshMorphTarget const & rhs );
		bool compare( c3d::SkeletonAnimationKeyFrame const & lhs, c3d::SkeletonAnimationKeyFrame const & rhs );
		bool compare( c3d::AnimatedObjectGroup const & lhs, c3d::AnimatedObjectGroup const & rhs );
		bool compare( c3d::AnimatedObject const & lhs, c3d::AnimatedObject const & rhs );
		bool compare( c3d::AnimationInstance const & lhs, c3d::AnimationInstance const & rhs );
		bool compare( c3d::SkeletonAnimationInstance const & lhs, c3d::SkeletonAnimationInstance const & rhs );
		bool compare( c3d::SkeletonAnimationInstanceObject const & lhs, c3d::SkeletonAnimationInstanceObject const & rhs );
		bool compare( c3d::SkeletonAnimationInstanceKeyFrame const & lhs, c3d::SkeletonAnimationInstanceKeyFrame const & rhs );
		bool compare( c3d::VertexBoneData const & lhs, c3d::VertexBoneData const & rhs );
		bool compare( c3d::VertexBoneData::Ids const & lhs, c3d::VertexBoneData::Ids const & rhs );
		bool compare( c3d::VertexBoneData::Weights const & lhs, c3d::VertexBoneData::Weights const & rhs );
		bool compare( c3d::InterleavedVertex const & lhs, c3d::InterleavedVertex const & rhs );
		bool compare( c3d::AllocationStats const & lhs, c3d::AllocationStats const & rhs );
		bool compare( c3d::DeviceCounts const & lhs, c3d::DeviceCounts const & rhs );
		bool compare( c3d::EngineCounts const & lhs, c3d::EngineCounts const & rhs );

		template< typename Value >
		inline bool compare( c3d::ArrayView< Value > const & lhs, c3d::ArrayView< Value > const & rhs )
		{
			bool result{ lhs.size() == rhs.size() };

			if ( result )
			{
				for ( size_t i = 0u; i < lhs.size(); ++i )
				{
					result = result && this->compare( lhs[i], rhs[i] );
				}
			}

			return result;
		}

		template< typename Value >
		inline bool compare( c3d::Vector< Value > const & lhs, c3d::Vector< Value > const & rhs )
		{
			auto result = ( lhs.size() == rhs.size() );

			if ( result )
			{
				for ( size_t i = 0u; i < lhs.size(); ++i )
				{
					result = result && this->compare( lhs[i], rhs[i] );
				}
			}

			return result;
		}

		template< typename Value, size_t N, size_t M >
		inline bool compare( c3d::Array< Value, N > const & lhs, c3d::Array< Value, M > const & rhs )
		{
			auto result = ( lhs.size() == rhs.size() );

			if ( result )
			{
				for ( size_t i = 0u; i < lhs.size(); ++i )
				{
					result = result && this->compare( lhs[i], rhs[i] );
				}
			}

			return result;
		}

	protected:
		void doRegisterTest( std::string const & name
			, TestFunction test )override;

	protected:
		c3d::Engine & m_engine;
		c3d::Path m_testDataFolder;
	};

	//************************************************************************************************
}

#endif
