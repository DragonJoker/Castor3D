#include "Castor3D/Scene/Animation/Skeleton/SkeletonAnimationInstanceKeyFrame.hpp"

#include "Castor3D/Animation/Interpolator.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationKeyFrame.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Animation/AnimatedSkeleton.hpp"
#include "Castor3D/Scene/Animation/Skeleton/SkeletonAnimationInstance.hpp"
#include "Castor3D/Scene/Animation/Skeleton/SkeletonAnimationInstanceObject.hpp"

namespace c3d
{
	namespace sklanminst
	{
		Matrix4x4f interpolate( Matrix4x4f const & prev, Matrix4x4f const & curr, float ratio
			, Interpolator< float > const & floatInterpolator )
		{
			Matrix4x4f result{};
			for ( u32 i = 0u; i < 16u; ++i )
				result.ptr()[i] = floatInterpolator.interpolate( prev.constPtr()[i], curr.constPtr()[i], ratio );
			return result;
		}

		BoundingBox interpolate( BoundingBox const & prev, BoundingBox const & curr, float ratio
			, Interpolator< Point3f > const & vecInterpolator )
		{
			return { vecInterpolator.interpolate( prev.getMin(), curr.getMin(), ratio )
				, vecInterpolator.interpolate( prev.getMax(), curr.getMax(), ratio ) };
		}

		SubmeshBoundingBoxList interpolate( SubmeshBoundingBoxList const & prev, SubmeshBoundingBoxList const & curr, float ratio
			, Interpolator< Point3f > const & vecInterpolator )
		{
			CU_Require( prev.size() == curr.size() );
			SubmeshBoundingBoxList result;
			result.reserve( prev.size() );
			auto itPrev = prev.begin();
			auto itCurr = curr.begin();

			while ( itPrev != prev.end() && itCurr != curr.end() )
			{
				CU_Require( itPrev->first == itCurr->first );
				result.emplace_back( itPrev->first, interpolate( itPrev->second, itCurr->second, ratio, vecInterpolator ) );
				++itPrev;
				++itCurr;
			}

			return result;
		}
	}

	SkeletonAnimationInstanceKeyFrame::SkeletonAnimationInstanceKeyFrame( SkeletonAnimationInstance & skeletonAnimation
		, SkeletonAnimationKeyFrame const & keyFrame
		, AnimatedSkeleton & skeleton )
		: OwnedBy< SkeletonAnimationInstance >{ skeletonAnimation }
		, m_skeleton{ skeleton }
		, m_keyFrame{ keyFrame }
	{
		for ( auto const & object : skeletonAnimation )
		{
			auto it = m_keyFrame.find( object->getObject() );
			m_objects.emplace_back( object.get()
				, ( it != m_keyFrame.end()
					? it->cumulative
					: Matrix4x4f{ Matrix4x4f::getIdentity() } ) );
		}

		m_boxes = m_keyFrame.computeBoundingBoxes( m_skeleton.getMesh(), m_skeleton.getSkeleton() );
	}

	void SkeletonAnimationInstanceKeyFrame::apply()const
	{
		for ( auto const & [object, transform] : m_objects )
		{
			object->update( transform );
		}

		m_skeleton.getGeometry().updateContainers( m_boxes );
	}

	void SkeletonAnimationInstanceKeyFrame::interpolate( SkeletonAnimationInstanceKeyFrame const & prev, Milliseconds const & timeMs
		, Interpolator< float > const & floatInterpolator, Interpolator< Point3f > const & vecInterpolator )const
	{
		auto itCurr = m_objects.begin();
		auto itPrev = prev.m_objects.begin();
		CU_Require( m_objects.size() == prev.m_objects.size() );
		float ratio = float( double( timeMs.count() ) / double( ( getTimeIndex() - prev.getTimeIndex() ).count() ) );

		while ( itCurr != m_objects.end() && itPrev != prev.m_objects.end() )
		{
			auto transform = sklanminst::interpolate( itPrev->second, itCurr->second, ratio, floatInterpolator );
			CU_Require( itCurr->first == itPrev->first );
			itCurr->first->update( transform );

			++itPrev;
			++itCurr;
		}

		m_skeleton.getGeometry().updateContainers( sklanminst::interpolate( prev.m_boxes, m_boxes, ratio, vecInterpolator ) );
	}
}
