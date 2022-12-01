using Castor3D;
using System;
using System.Windows;

namespace CastorViewerSharp
{
    public class NodeState
    {
        public NodeState(SceneNode node)
        {
            m_node = node;
            m_originalPosition = m_node.Position;
            m_originalOrientation = m_node.Orientation;
            m_originalXAngle = m_originalOrientation.Pitch;
            m_originalYAngle = m_originalOrientation.Yaw;
            m_originalZAngle = m_originalOrientation.Roll;
            m_originalZAngle.Degrees = 0.0f;
            m_xAngle = m_originalXAngle;
            m_yAngle = m_originalYAngle;
            m_zAngle = m_originalZAngle;
        }

        public void reset(float speed)
        {
            m_minSpeed = -speed;
            m_maxSpeed = speed;
            m_node.Orientation = m_originalOrientation;
            m_node.Position = m_originalPosition;
        }

        public bool update()
        {
            var xAngle = m_xAngle.Degrees;
            var yAngle = m_yAngle.Degrees;
            m_xAngle.Degrees += m_xAngularVelocity;
            m_yAngle.Degrees += m_yAngularVelocity;
            m_xAngularVelocity = doUpdateVelocity(m_xAngularVelocity, -MaxAngularSpeed, MaxAngularSpeed);
            m_yAngularVelocity = doUpdateVelocity(m_yAngularVelocity, -MaxAngularSpeed, MaxAngularSpeed);

            var translate = new Vector3D();
            translate.Set(m_xScalarVelocity, m_yScalarVelocity, m_zScalarVelocity);
            m_xScalarVelocity = doUpdateVelocity(m_xScalarVelocity, -MaxScalarSpeed, MaxScalarSpeed);
            m_yScalarVelocity = doUpdateVelocity(m_yScalarVelocity, -MaxScalarSpeed, MaxScalarSpeed);
            m_zScalarVelocity = doUpdateVelocity(m_zScalarVelocity, -MaxScalarSpeed, MaxScalarSpeed);

            bool result = xAngle != m_xAngle.Degrees
                || yAngle != m_yAngle.Degrees;

            if (translate.X != 0.0f
                || translate.Y != 0.0f
                || translate.Z != 0.0f)
            {
                var orientation = m_node.Orientation;
                var right = new Vector3D();
                var up = new Vector3D();
                var front = new Vector3D();
                right.Set(1.0f, 0.0f, 0.0f);
                up.Set(0.0f, 1.0f, 0.0f);
                front.Set(0.0f, 0.0f, 1.0f);
                right = orientation.Transform(right);
                up = orientation.Transform(up);
                front = orientation.Transform(front);
                translate = right.Mul(translate.X)
                    .CompAdd(up.Mul(translate.Y))
                    .CompAdd(front.Mul(translate.Z));
                result = true;
            }

            if (result)
            {
                m_node.Translate(translate);

                var xAxis = new Vector3D { };
                xAxis.Set(1.0f, 0.0f, 0.0f);
                var pitch = new Quaternion { };
                pitch.FromAxisAngle(xAxis, m_xAngle);

                var yAxis = new Vector3D { };
                yAxis.Set(0.0f, 1.0f, 0.0f);
                var yaw = new Quaternion { };
                yaw.FromAxisAngle(yAxis, m_yAngle);

                var zAxis = new Vector3D { };
                zAxis.Set(0.0f, 0.0f, 1.0f);
                var roll = new Quaternion { };
                roll.FromAxisAngle(zAxis, m_zAngle);

                m_node.Orientation = roll.Mul(yaw.Mul(pitch));
            }

            return result;
        }

        public void addAngularVelocity(float x, float y)
        {
            m_xAngularVelocity += x;
            m_yAngularVelocity += y;
        }

        public void addScalarVelocity(float x, float y, float z)
        {
            m_xScalarVelocity += x;
            m_yScalarVelocity += y;
            m_zScalarVelocity += z;
        }

        private float doUpdateVelocity(float velocity
            , float rangeMin
            , float rangeMax)
        {
            var result = velocity / 1.2f;

            if (Math.Abs(result) < 0.2f)
            {
                result = 0.0f;
            }

            if (result > rangeMax)
            {
                result = rangeMax;
            }

            if (result < rangeMin)
            {
                result = rangeMin;
            }

            return result;
        }

        SceneNode m_node;
        Vector3D m_originalPosition;
        Quaternion m_originalOrientation;
        Angle m_originalXAngle;
        Angle m_originalYAngle;
        Angle m_originalZAngle;
        Angle m_xAngle;
        Angle m_yAngle;
        Angle m_zAngle;
        float m_minSpeed;
        float m_maxSpeed;
        float m_xAngularVelocity;
        float m_yAngularVelocity;
        float m_xScalarVelocity;
        float m_yScalarVelocity;
        float m_zScalarVelocity;
        float MaxAngularSpeed = 5.0f;
	    float MaxScalarSpeed = 20.0f;
    }
}
