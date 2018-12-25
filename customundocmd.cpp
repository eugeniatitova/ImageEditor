#include "customundocmd.h"

CustomUndoCmd::CustomUndoCmd(const QImage *img, ImageScene &imgScene, QUndoCommand *parent)
  : QUndoCommand(parent), mPrevImg(*img), mImageScene(imgScene)
{
  mCurrImg = mPrevImg;
}
void CustomUndoCmd::undo()
{
    mImageScene.clearSelection();
    mCurrImg = *(mImageScene.getImage());
    mImageScene.setImage(mPrevImg);
    mImageScene.update();
    mImageScene.saveChanges();
}

void CustomUndoCmd::redo()
{
    mImageScene.setImage(mCurrImg);
    mImageScene.update();
    mImageScene.saveChanges();
}
