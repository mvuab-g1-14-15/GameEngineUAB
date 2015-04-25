#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "Artemis/Component.h"
#include "Math/Vector3.h"

class CTransform : public artemis::Component
{
public:
    Math::Vect3f m_Position;
    Math::Vect3f m_Rotation;
    Math::Vect3f m_Scale;

    CTransform( Math::Vect3f pos, Math::Vect3f rot, Math::Vect3f scale )
    {
        m_Position = pos;
        m_Rotation = rot;
        m_Scale = scale;
    }
};

class CVelocity : public artemis::Component
{
public:
    Math::Vect3f m_Direction;
    float m_Speed;

    CVelocity( Math::Vect3f dir, float speed )
    {
        m_Direction = dir;
        m_Speed = speed;
    }
};

#endif //COMPONENTS_H