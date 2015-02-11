#ifndef _ENTITY_H
#define _ENTITY_H

#include "Component.h"

#include <map>

class CEntity
{
public:
	CEntity(void);
	virtual ~CEntity(void);

	void AddComponent(CComponent*);
	void RemoveComponent(const ComponentType);

  CComponent* GetComponent(const ComponentType);

private:
	std::map<ComponentType, CComponent*> m_Components;
};

#endif //_ENTITY_H