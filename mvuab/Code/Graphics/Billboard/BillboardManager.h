#ifndef INC_BILLBOARDMANAGER_H_
#define INC_BILLBOARDMANAGER_H_

#include "Utils/Defines.h"
#include "Utils\Manager.h"
#include "Utils/TemplatedVectorMapManager.h"
#include "XML/XMLTreeNode.h"
#include "BillboardCore.h"

class CRenderableVertexs;
class CGraphicsManager;

class CBillboardManager : public CTemplatedVectorMapManager<CBillboardCore>, public CManager
{
public:

  CBillboardManager( const CXMLTreeNode& atts );
  virtual ~CBillboardManager();

  void CreateBillBoardGeometry();
  void DestroyBillBoardGeometry();

  void Init();
  void Render();
  void Update();

  void LoadInstances( const std::string& aFileName );
  void FlushInstances();

private:
  static CRenderableVertexs* sRV;
  CGraphicsManager         * mGM;
};

#endif // INC_CAMERAMANAGER_H_