#ifndef _BOSS_ENEMY_H
#define _BOSS_ENEMY_H

#include "Enemy.h"

#pragma once

class CStateMachine;

class CBossEnemy : public CEnemy
{
public:
    CBossEnemy(CXMLTreeNode &Node, CStateMachine* aStateMachine);
    ~CBossEnemy();
	void Init(){};
    void Update();
    void Render();
};


#endif

