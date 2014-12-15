#ifndef _EASY_ENEMY_H
#define _EASY_ENEMY_H

#include "Enemy.h"

#pragma once

class CEasyEnemy : public CEnemy
{
public:
    CEasyEnemy(CXMLTreeNode &Node);
    ~CEasyEnemy();

    void Update(float32 deltaTime);
    void Render();
};


#endif

