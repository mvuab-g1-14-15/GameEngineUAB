#ifndef OMNI_LIGHT_H
#define OMNI_LIGHT_H

#include "Light.h"

#include "XML\XMLTreeNode.h"

class COmniLight : public CLight
{
public:
  COmniLight( );
  COmniLight( CXMLTreeNode node );
  virtual void Render();
  virtual void SetShadowMap( CGraphicsManager* GM );
};

#endif // OMNI_LIGHT_H
