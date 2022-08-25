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
#include <Castor3D/Model/Mesh/Submesh/Component/LinesMapping.hpp>
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

	using castor::operator<<;
	using castor3d::operator<<;

	template<>
	struct Stringifier< castor3d::Scene >
	{
		static std::string get( castor3d::Scene const & value )
		{
			std::stringstream stream;
			stream << value.getName();
			return stream.str();
		}
	};

	template<>
	struct Stringifier< castor3d::SceneNode >
	{
		static std::string get( castor3d::SceneNode const & value )
		{
			std::stringstream stream;
			stream << value.getName();
			return stream.str();
		}
	};

	template<>
	struct Stringifier< castor3d::MovableObject >
	{
		static std::string get( castor3d::MovableObject const & value )
		{
			std::stringstream stream;
			stream << value.getName();
			return stream.str();
		}
	};

	template<>
	struct Stringifier< castor3d::Geometry >
	{
		static std::string get( castor3d::Geometry const & value )
		{
			std::stringstream stream;
			stream << value.getName();
			return stream.str();
		}
	};

	template<>
	struct Stringifier< castor3d::Light >
	{
		static std::string get( castor3d::Light const & value )
		{
			std::stringstream stream;
			stream << value.getName();
			return stream.str();
		}
	};

	template<>
	struct Stringifier< castor3d::Camera >
	{
		static std::string get( castor3d::Camera const & value )
		{
			std::stringstream stream;
			stream << value.getName();
			return stream.str();
		}
	};

	template<>
	struct Stringifier< castor3d::Material >
	{
		static std::string get( castor3d::Material const & value )
		{
			std::stringstream stream;
			stream << value.getName();
			return stream.str();
		}
	};

	template<>
	struct Stringifier< castor3d::Mesh >
	{
		static std::string get( castor3d::Mesh const & value )
		{
			std::stringstream stream;
			stream << value.getName();
			return stream.str();
		}
	};

	template<>
	struct Stringifier< castor3d::BoneNode >
	{
		static std::string get( castor3d::BoneNode const & value )
		{
			std::stringstream stream;
			stream << value.getName();
			return stream.str();
		}
	};

	template<>
	struct Stringifier< castor3d::SkeletonNode >
	{
		static std::string get( castor3d::SkeletonNode const & value )
		{
			std::stringstream stream;
			stream << value.getName();
			return stream.str();
		}
	};

	template<>
	struct Stringifier< castor3d::Face >
	{
		static std::string get( castor3d::Face const & value )
		{
			std::stringstream stream;
			stream << value[0] << " " << value[1] << " " << value[2];
			return stream.str();
		}
	};

	template<>
	struct Stringifier< castor3d::Line >
	{
		static std::string get( castor3d::Line const & value )
		{
			std::stringstream stream;
			stream << value[0] << " " << value[1];
			return stream.str();
		}
	};

	template<>
	struct Stringifier< castor3d::Animation >
	{
		static std::string get( castor3d::Animation const & value )
		{
			std::stringstream stream;
			stream << value.getName();
			return stream.str();
		}
	};

	template<>
	struct Stringifier< castor3d::SkeletonAnimation >
	{
		static std::string get( castor3d::SkeletonAnimation const & value )
		{
			std::stringstream stream;
			stream << value.getName();
			return stream.str();
		}
	};

	template<>
	struct Stringifier< castor3d::SkeletonAnimationObject >
	{
		static std::string get( castor3d::SkeletonAnimationObject const & value )
		{
			std::stringstream stream;
			stream << value.getName();
			return stream.str();
		}
	};

	template<>
	struct Stringifier< castor3d::AnimatedObjectGroup >
	{
		static std::string get( castor3d::AnimatedObjectGroup const & value )
		{
			std::stringstream stream;
			stream << value.getName();
			return stream.str();
		}
	};

	template<>
	struct Stringifier< castor3d::AnimatedObject >
	{
		static std::string get( castor3d::AnimatedObject const & value )
		{
			std::stringstream stream;
			stream << value.getName();
			return stream.str();
		}
	};

	template<>
	struct Stringifier< castor3d::AnimationInstance >
	{
		static std::string get( castor3d::AnimationInstance const & value )
		{
			std::stringstream stream;
			stream << value.getAnimation().getName();
			return stream.str();
		}
	};

	template<>
	struct Stringifier< castor3d::InterpolatorType >
	{
		static std::string get( castor3d::InterpolatorType const & value )
		{
			static std::map< castor3d::InterpolatorType, std::string > Names
			{
				{ castor3d::InterpolatorType::eNearest, "Nearest" },
				{ castor3d::InterpolatorType::eLinear, "Linear" },
			};
			return Names[value];
		}
	};

	template<>
	struct Stringifier< castor3d::AnimationType >
	{
		static std::string get( castor3d::AnimationType const & value )
		{
			static std::map< castor3d::AnimationType, std::string > Names
			{
				{ castor3d::AnimationType::eSceneNode, "SceneNode" },
				{ castor3d::AnimationType::eMesh, "Mesh" },
				{ castor3d::AnimationType::eSkeleton, "Skeleton" },
			};
			return Names[value];
		}
	};

	template<>
	struct Stringifier< castor3d::SkeletonNodeType >
	{
		static std::string get( castor3d::SkeletonNodeType const & value )
		{
			static std::map< castor3d::SkeletonNodeType, std::string > Names
			{
				{ castor3d::SkeletonNodeType::eBone, "Bone" },
				{ castor3d::SkeletonNodeType::eNode, "Node" },
			};
			return Names[value];
		}
	};

	template<>
	struct Stringifier< castor3d::AnimationState >
	{
		static std::string get( castor3d::AnimationState const & value )
		{
			static std::map< castor3d::AnimationState, std::string > Names
			{
				{ castor3d::AnimationState::ePlaying, "Playing" },
				{ castor3d::AnimationState::eStopped, "Stopped" },
				{ castor3d::AnimationState::ePaused, "Paused" },
			};
			return Names[value];
		}
	};

	template<>
	struct Stringifier< castor3d::ViewportType >
	{
		static std::string get( castor3d::ViewportType const & value )
		{
			static std::map< castor3d::ViewportType, std::string > Names
			{
				{ castor3d::ViewportType::eOrtho, "Ortho" },
				{ castor3d::ViewportType::ePerspective, "Perspective" },
				{ castor3d::ViewportType::eFrustum, "Frustum" },
			};
			return Names[value];
		}
	};

	template<>
	struct Stringifier< castor3d::SubmeshFlag >
	{
		static std::string get( castor3d::SubmeshFlag const & value )
		{
			static std::map< castor3d::SubmeshFlag, std::string > Names
			{
				{ castor3d::SubmeshFlag::eIndex, "eIndex" },
				{ castor3d::SubmeshFlag::ePositions, "ePositions" },
				{ castor3d::SubmeshFlag::eNormals, "eNormals" },
				{ castor3d::SubmeshFlag::eTangents, "eTangents" },
				{ castor3d::SubmeshFlag::eTexcoords0, "eTexcoords0" },
				{ castor3d::SubmeshFlag::eTexcoords1, "eTexcoords1" },
				{ castor3d::SubmeshFlag::eTexcoords2, "eTexcoords2" },
				{ castor3d::SubmeshFlag::eTexcoords3, "eTexcoords3" },
				{ castor3d::SubmeshFlag::eColours, "eColours" },
				{ castor3d::SubmeshFlag::eSkin, "eSkin" },
			};
			return Names[value];
		}
	};

	template<>
	struct Stringifier< castor3d::MorphFlag >
	{
		static std::string get( castor3d::MorphFlag const & value )
		{
			static std::map< castor3d::MorphFlag, std::string > Names
			{
				{ castor3d::MorphFlag::ePositions, "ePositions" },
				{ castor3d::MorphFlag::eNormals, "eNormals" },
				{ castor3d::MorphFlag::eTangents, "eTangents" },
				{ castor3d::MorphFlag::eTexcoords0, "eTexcoords0" },
				{ castor3d::MorphFlag::eTexcoords1, "eTexcoords1" },
				{ castor3d::MorphFlag::eTexcoords2, "eTexcoords2" },
				{ castor3d::MorphFlag::eTexcoords3, "eTexcoords3" },
				{ castor3d::MorphFlag::eColours, "eColours" },
			};
			return Names[value];
		}
	};

	template<>
	struct Stringifier< castor3d::ShaderFlag >
	{
		static std::string get( castor3d::ShaderFlag const & value )
		{
			static std::map< castor3d::ShaderFlag, std::string > Names
			{
				{ castor3d::ShaderFlag::eNormal, "eNormal" },
				{ castor3d::ShaderFlag::eTangentSpace, "eTangentSpace" },
				{ castor3d::ShaderFlag::eVelocity, "eVelocity" },
				{ castor3d::ShaderFlag::eWorldSpace, "eWorldSpace" },
				{ castor3d::ShaderFlag::eViewSpace, "eViewSpace" },
				{ castor3d::ShaderFlag::eDepth, "eDepth" },
				{ castor3d::ShaderFlag::eVisibility, "eVisibility" },
				{ castor3d::ShaderFlag::ePicking, "ePicking" },
				{ castor3d::ShaderFlag::eLighting, "eLighting" },
				{ castor3d::ShaderFlag::eShadowMapDirectional, "eShadowMapDirectional" },
				{ castor3d::ShaderFlag::eShadowMapSpot, "eShadowMapSpot" },
				{ castor3d::ShaderFlag::eShadowMapPoint, "eShadowMapPoint" },
				{ castor3d::ShaderFlag::eVsmShadowMap, "eVsmShadowMap" },
				{ castor3d::ShaderFlag::eRsmShadowMap, "eRsmShadowMap" },
				{ castor3d::ShaderFlag::eEnvironmentMapping, "eEnvironmentMapping" },
				{ castor3d::ShaderFlag::eGeometry, "eGeometry" },
				{ castor3d::ShaderFlag::eTessellation, "eTessellation" },
				{ castor3d::ShaderFlag::eForceTexCoords, "eForceTexCoords" },
			};
			return Names[value];
		}
	};

	template<>
	struct Stringifier< castor3d::ProgramFlag >
	{
		static std::string get( castor3d::ProgramFlag const & value )
		{
			static std::map< castor3d::ProgramFlag, std::string > Names
			{
				{ castor3d::ProgramFlag::eInstantiation, "eInstantiation" },
				{ castor3d::ProgramFlag::eBillboards, "eBillboards" },
				{ castor3d::ProgramFlag::eInvertNormals, "eInvertNormals" },
				{ castor3d::ProgramFlag::eHasMesh, "eHasMesh" },
				{ castor3d::ProgramFlag::eHasTask, "eHasTask" },
			};
			return Names[value];
		}
	};

	template<>
	struct Stringifier< castor3d::SubmeshComponent >
	{
		static std::string get( castor3d::SubmeshComponent const & value )
		{
			return castor::string::stringCast< char >( value.getType() );
		}
	};

	template<>
	struct Stringifier< castor3d::TriFaceMapping >
	{
		static std::string get( castor3d::TriFaceMapping const & value )
		{
			return std::string{ "castor3d::TriFaceMapping" };
		}
	};

	template<>
	struct Stringifier< castor3d::LinesMapping >
	{
		static std::string get( castor3d::LinesMapping const & value )
		{
			return std::string{ "castor3d::LinesMapping" };
		}
	};

	template<>
	struct Stringifier< castor3d::VertexBoneData::Ids >
	{
		static std::string get( castor3d::VertexBoneData::Ids const & value )
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
	struct Stringifier< castor3d::VertexBoneData::Weights >
	{
		static std::string get( castor3d::VertexBoneData::Weights const & value )
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
	struct Stringifier< castor3d::Animable >
	{
		static std::string get( castor3d::Animable const & value )
		{
			return std::string{ "castor3d::Animable" };
		}
	};

	template<>
	struct Stringifier< castor3d::LightCategory >
	{
		static std::string get( castor3d::LightCategory const & value )
		{
			return std::string{ "castor3d::LightCategory" };
		}
	};

	template<>
	struct Stringifier< castor3d::DirectionalLight >
	{
		static std::string get( castor3d::DirectionalLight const & value )
		{
			return std::string{ "castor3d::DirectionalLight" };
		}
	};

	template<>
	struct Stringifier< castor3d::PointLight >
	{
		static std::string get( castor3d::PointLight const & value )
		{
			return std::string{ "castor3d::PointLight" };
		}
	};

	template<>
	struct Stringifier< castor3d::SpotLight >
	{
		static std::string get( castor3d::SpotLight const & value )
		{
			return std::string{ "castor3d::SpotLight" };
		}
	};

	template<>
	struct Stringifier< castor3d::Viewport >
	{
		static std::string get( castor3d::Viewport const & value )
		{
			return std::string{ "castor3d::Viewport" };
		}
	};

	template<>
	struct Stringifier< castor3d::Pass >
	{
		static std::string get( castor3d::Pass const & value )
		{
			return std::string{ "castor3d::Pass" };
		}
	};

	template<>
	struct Stringifier< castor3d::Submesh >
	{
		static std::string get( castor3d::Submesh const & value )
		{
			return std::string{ "castor3d::Submesh" };
		}
	};

	template<>
	struct Stringifier< castor3d::Skeleton >
	{
		static std::string get( castor3d::Skeleton const & value )
		{
			return std::string{ "castor3d::Skeleton" };
		}
	};

	template<>
	struct Stringifier< castor3d::AnimationKeyFrame >
	{
		static std::string get( castor3d::AnimationKeyFrame const & value )
		{
			return std::string{ "castor3d::AnimationKeyFrame" };
		}
	};

	template<>
	struct Stringifier< castor3d::MeshMorphTarget >
	{
		static std::string get( castor3d::MeshMorphTarget const & value )
		{
			return std::string{ "castor3d::MeshMorphTarget" };
		}
	};

	template<>
	struct Stringifier< castor3d::SkeletonAnimationKeyFrame >
	{
		static std::string get( castor3d::SkeletonAnimationKeyFrame const & value )
		{
			return std::string{ "castor3d::SkeletonAnimationKeyFrame" };
		}
	};

	template<>
	struct Stringifier< castor3d::SkeletonAnimationInstance >
	{
		static std::string get( castor3d::SkeletonAnimationInstance const & value )
		{
			return std::string{ "castor3d::SkeletonAnimationInstance" };
		}
	};

	template<>
	struct Stringifier< castor3d::SkeletonAnimationInstanceObject >
	{
		static std::string get( castor3d::SkeletonAnimationInstanceObject const & value )
		{
			return std::string{ "castor3d::SkeletonAnimationInstanceObject" };
		}
	};

	template<>
	struct Stringifier< castor3d::SkeletonAnimationInstanceKeyFrame >
	{
		static std::string get( castor3d::SkeletonAnimationInstanceKeyFrame const & value )
		{
			return std::string{ "castor3d::SkeletonAnimationInstanceKeyFrame" };
		}
	};

	template<>
	struct Stringifier< castor3d::PositionsComponent >
	{
		static std::string get( castor3d::PositionsComponent const & value )
		{
			return std::string{ "castor3d::PositionsComponent" };
		}
	};

	template<>
	struct Stringifier< castor3d::NormalsComponent >
	{
		static std::string get( castor3d::NormalsComponent const & value )
		{
			return std::string{ "castor3d::NormalsComponent" };
		}
	};

	template<>
	struct Stringifier< castor3d::TangentsComponent >
	{
		static std::string get( castor3d::TangentsComponent const & value )
		{
			return std::string{ "castor3d::TangentsComponent" };
		}
	};

	template<>
	struct Stringifier< castor3d::Texcoords0Component >
	{
		static std::string get( castor3d::Texcoords0Component const & value )
		{
			return std::string{ "castor3d::Texcoords0Component" };
		}
	};

	template<>
	struct Stringifier< castor3d::Texcoords1Component >
	{
		static std::string get( castor3d::Texcoords1Component const & value )
		{
			return std::string{ "castor3d::Texcoords1Component" };
		}
	};

	template<>
	struct Stringifier< castor3d::Texcoords2Component >
	{
		static std::string get( castor3d::Texcoords2Component const & value )
		{
			return std::string{ "castor3d::Texcoords2Component" };
		}
	};

	template<>
	struct Stringifier< castor3d::Texcoords3Component >
	{
		static std::string get( castor3d::Texcoords3Component const & value )
		{
			return std::string{ "castor3d::Texcoords3Component" };
		}
	};

	template<>
	struct Stringifier< castor3d::ColoursComponent >
	{
		static std::string get( castor3d::ColoursComponent const & value )
		{
			return std::string{ "castor3d::ColoursComponent" };
		}
	};

	template<>
	struct Stringifier< castor3d::SkinComponent >
	{
		static std::string get( castor3d::SkinComponent const & value )
		{
			return std::string{ "castor3d::SkinComponent" };
		}
	};

	template<>
	struct Stringifier< castor3d::VertexBoneData >
	{
		static std::string get( castor3d::VertexBoneData const & value )
		{
			return std::string{ "castor3d::VertexBoneData" };
		}
	};

	template<>
	struct Stringifier< castor3d::InterleavedVertex >
	{
		static std::string get( castor3d::InterleavedVertex const & value )
		{
			return std::string{ "castor3d::InterleavedVertex" };
		}
	};

	template< typename FlagT >
	struct Stringifier< castor::FlagCombination< FlagT > >
	{
		static inline std::string toString( castor::FlagCombination< FlagT > const & values )
		{
			int i = ( sizeof( FlagT ) * 8 ) - 1;
			std::string sep;
			std::string result;

			while ( i >= 0 )
			{
				auto value = FlagT( 0x0000000000000001 << i );

				if ( castor::checkFlag( values, value ) )
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
	struct Stringifier< castor::Matrix< ValueT, ColumnsT, RowsT > >
	{
		static std::string get( castor::Matrix< ValueT, ColumnsT, RowsT > const & value )
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
	struct Stringifier< castor::SquareMatrix< ValueT, CountT > >
	{
		static std::string get( castor::SquareMatrix< ValueT, CountT > const & value )
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
	struct Stringifier< castor::Point< ValueT, CountT > >
	{
		static std::string get( castor::Point< ValueT, CountT > const & value )
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
	struct Stringifier< castor::AngleT< ValueT > >
	{
		static std::string get( castor::AngleT< ValueT > const & value )
		{
			std::stringstream stream;
			stream << value.degrees();
			return stream.str();
		}
	};

	template< typename ValueT >
	struct Stringifier< castor::QuaternionT< ValueT > >
	{
		static std::string get( castor::QuaternionT< ValueT > const & value )
		{
			std::stringstream stream;
			stream << value;
			return stream.str();
		}
	};

	template< typename ValueT >
	struct Stringifier< castor::ChangeTracked< ValueT > >
	{
		static std::string get( castor::ChangeTracked< ValueT > const & value )
		{
			std::stringstream stream;
			stream << toString( *value );
			return stream.str();
		}
	};

	template< typename ValueT >
	struct Stringifier< castor::ArrayView< ValueT > >
	{
		static std::string get( castor::ArrayView< ValueT > const & values )
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
			, castor3d::Engine & engine );

	protected:
		void doCleanupEngine();
		using TestCase::compare;
		bool compare( castor::Angle const & p_a, castor::Angle const & p_b );
		bool compare( castor::Quaternion const & p_a, castor::Quaternion const & p_b );
		bool compare( castor3d::Scene const & p_a, castor3d::Scene const & p_b );
		bool compare( castor3d::SceneNode const & p_a, castor3d::SceneNode const & p_b );
		bool compare( castor3d::Animable const & p_a, castor3d::Animable const & p_b );
		bool compare( castor3d::MovableObject const & p_a, castor3d::MovableObject const & p_b );
		bool compare( castor3d::Camera const & p_a, castor3d::Camera const & p_b );
		bool compare( castor3d::Light const & p_a, castor3d::Light const & p_b );
		bool compare( castor3d::Geometry const & p_a, castor3d::Geometry const & p_b );
		bool compare( castor3d::LightCategory const & p_a, castor3d::LightCategory const & p_b );
		bool compare( castor3d::DirectionalLight const & p_a, castor3d::DirectionalLight const & p_b );
		bool compare( castor3d::PointLight const & p_a, castor3d::PointLight const & p_b );
		bool compare( castor3d::SpotLight const & p_a, castor3d::SpotLight const & p_b );
		bool compare( castor3d::Viewport const & p_a, castor3d::Viewport const & p_b );
		bool compare( castor3d::Mesh const & p_a, castor3d::Mesh const & p_b );
		bool compare( castor3d::Submesh const & p_a, castor3d::Submesh const & p_b );
		bool compare( castor3d::SubmeshComponent const & p_a, castor3d::SubmeshComponent const & p_b );
		bool compare( castor3d::SkinComponent const & p_a, castor3d::SkinComponent const & p_b );
		bool compare( castor3d::PositionsComponent const & p_a, castor3d::PositionsComponent const & p_b );
		bool compare( castor3d::NormalsComponent const & p_a, castor3d::NormalsComponent const & p_b );
		bool compare( castor3d::TangentsComponent const & p_a, castor3d::TangentsComponent const & p_b );
		bool compare( castor3d::Texcoords0Component const & p_a, castor3d::Texcoords0Component const & p_b );
		bool compare( castor3d::Texcoords1Component const & p_a, castor3d::Texcoords1Component const & p_b );
		bool compare( castor3d::Texcoords2Component const & p_a, castor3d::Texcoords2Component const & p_b );
		bool compare( castor3d::Texcoords3Component const & p_a, castor3d::Texcoords3Component const & p_b );
		bool compare( castor3d::ColoursComponent const & p_a, castor3d::ColoursComponent const & p_b );
		bool compare( castor3d::Face const & p_a, castor3d::Face const & p_b );
		bool compare( castor3d::TriFaceMapping const & p_a, castor3d::TriFaceMapping const & p_b );
		bool compare( castor3d::Line const & p_a, castor3d::Line const & p_b );
		bool compare( castor3d::LinesMapping const & p_a, castor3d::LinesMapping const & p_b );
		bool compare( castor3d::Skeleton const & p_a, castor3d::Skeleton const & p_b );
		bool compare( castor3d::SkeletonNode const & p_a, castor3d::SkeletonNode const & p_b );
		bool compare( castor3d::BoneNode const & p_a, castor3d::BoneNode const & p_b );
		bool compare( castor3d::Animation const & p_a, castor3d::Animation const & p_b );
		bool compare( castor3d::SkeletonAnimation const & p_a, castor3d::SkeletonAnimation const & p_b );
		bool compare( castor3d::SkeletonAnimationObject const & p_a, castor3d::SkeletonAnimationObject const & p_b );
		bool compare( castor3d::AnimationKeyFrame const & p_a, castor3d::AnimationKeyFrame const & p_b );
		bool compare( castor3d::MeshMorphTarget const & p_a, castor3d::MeshMorphTarget const & p_b );
		bool compare( castor3d::SkeletonAnimationKeyFrame const & p_a, castor3d::SkeletonAnimationKeyFrame const & p_b );
		bool compare( castor3d::AnimatedObjectGroup const & p_a, castor3d::AnimatedObjectGroup const & p_b );
		bool compare( castor3d::AnimatedObject const & p_a, castor3d::AnimatedObject const & p_b );
		bool compare( castor3d::AnimationInstance const & p_a, castor3d::AnimationInstance const & p_b );
		bool compare( castor3d::SkeletonAnimationInstance const & p_a, castor3d::SkeletonAnimationInstance const & p_b );
		bool compare( castor3d::SkeletonAnimationInstanceObject const & p_a, castor3d::SkeletonAnimationInstanceObject const & p_b );
		bool compare( castor3d::SkeletonAnimationInstanceKeyFrame const & p_a, castor3d::SkeletonAnimationInstanceKeyFrame const & p_b );
		bool compare( castor3d::VertexBoneData const & p_a, castor3d::VertexBoneData const & p_b );
		bool compare( castor3d::VertexBoneData::Ids const & p_a, castor3d::VertexBoneData::Ids const & p_b );
		bool compare( castor3d::VertexBoneData::Weights const & p_a, castor3d::VertexBoneData::Weights const & p_b );
		bool compare( castor3d::InterleavedVertex const & p_a, castor3d::InterleavedVertex const & p_b );

		template< typename Value >
		inline bool compare( castor::ArrayView< Value > const & p_a, castor::ArrayView< Value > const & p_b )
		{
			bool result{ p_a.size() == p_b.size() };

			for ( size_t i = 0u; result && i < p_a.size(); ++i )
			{
				result = this->compare( p_a[i], p_b[i] );
			}

			return result;
		}

		template< typename Value >
		inline bool compare( std::vector< Value > const & p_a, std::vector< Value > const & p_b )
		{
			auto result = ( p_a.size() == p_b.size() );

			for ( size_t i = 0u; result && i < p_a.size(); ++i )
			{
				result = this->compare( p_a[i], p_b[i] );
			}

			return result;
		}

		template< typename Value, size_t N, size_t M >
		inline bool compare( std::array< Value, N > const & p_a, std::array< Value, M > const & p_b )
		{
			auto result = ( p_a.size() == p_b.size() );

			for ( size_t i = 0u; result && i < p_a.size(); ++i )
			{
				result = this->compare( p_a[i], p_b[i] );
			}

			return result;
		}

	protected:
		castor3d::Engine & m_engine;
		castor::Path m_testDataFolder;
	};

	//************************************************************************************************
}

#endif
