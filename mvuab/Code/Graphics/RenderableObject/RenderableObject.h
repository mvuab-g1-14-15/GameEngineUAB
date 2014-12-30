#pragma once

#include "Object3D.h"
#include "Utils\Name.h"
#include "GraphicsManager.h"

class CRenderableObject : public CObject3D, public CName
{
public:
    CRenderableObject();
    virtual ~CRenderableObject();

    virtual void Update(float ElapsedTime) {}
    virtual void Render() = 0;

    virtual unsigned int GetNumVertex   () { return 0; }
    virtual unsigned int GetNumFaces    () { return 0; }
    virtual unsigned int GetNumDraws    () { return 0; }
};