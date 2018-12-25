#ifndef CUSTOMUNDOCMD_H
#define CUSTOMUNDOCMD_H

#include <QUndoCommand>
#include <QImage>

#include "imagescene.h"

class CustomUndoCmd : public QUndoCommand
{
public:
  CustomUndoCmd(const QImage* img, ImageScene &imgScene, QUndoCommand *parent = 0);

  virtual void undo();
  virtual void redo();
public:
  QImage mPrevImg;
  QImage mCurrImg;
  ImageScene& mImageScene;
};

#endif // CUSTOMUNDOCMD_H
