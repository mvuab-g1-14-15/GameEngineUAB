#ifndef _BillboardInstance_H
#define _BillboardInstance_H

#include "Utils\Name.h"
#include "Utils\Defines.h"
#include "Object3D.h"

#include "Math\Vector3.h"
#include "Math\Matrix44.h"

class CTexture;
class CRenderableVertexs;
class CEffectTechnique;
class CGraphicsManager;

class CBillboardInstance : public CName, public CObject3D
{
    public:
        CBillboardInstance();
        virtual ~CBillboardInstance();

        static void CreateBillboardInstanceGeometry();
        static void DestroyBillboardInstanceGeometry();

        bool Init(const CXMLTreeNode& atts);
        void SetAngle( float aAngle ) { mAngle = aAngle; }

        void Render( CRenderableVertexs* aRV, CGraphicsManager* aGM, CEffectTechnique* aTechnique );
        void Update();

    private:
      float mAngle;
};

#endif //_BillboardInstance_H
