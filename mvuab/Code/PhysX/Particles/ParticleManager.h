#ifndef _PARTICLE_MANAGER_H
#define _PARTICLE_MANAGER_H

#include "XML\XMLTreeNode.h"
#include "Particles\InstanceParticle.h"
#include "Utils\Manager.h"
#include "Utils\TemplatedVectorMapManager.h"

#include <string>
#include <vector>

class CParticleEmitter;
class CCubeEmitter;
class CSphereEmitter;

class CParticleManager: public CManager, CTemplatedVectorMapManager<CInstanceParticle>
{
public:
  CParticleManager();
  CParticleManager( CXMLTreeNode& atts );
  ~CParticleManager();

  void Init();
  void Update(){};
  void Render();
  void Refresh();
};

#endif