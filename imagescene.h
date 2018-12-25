#ifndef IMAGESCENE_H
#define IMAGESCENE_H

#include "params.h"
#include "imageedittools.h"
#include <QWidget>
#include <QImage>

QT_BEGIN_NAMESPACE
class QUndoStack;
QT_END_NAMESPACE

class CustomUndoCmd;
class AbstractTool;

class ImageScene : public QWidget
{
  Q_OBJECT

public:
  explicit ImageScene(const bool &isOpen, const QString &filePath, QWidget *parent);
  ~ImageScene();

  bool save();
  bool saveAs();

  bool zoomImage(qreal factor);
  void resizeImage();
  void resizeCanvas();
  void rotateImage(bool flag);

  void copyImage();
  void pasteImage();
  void cutImage();
  void saveChanges();
  void clearSelection();
  void pushUndoCommand(CustomUndoCmd *command);

  void restoreCursor();

  inline QString getFileName() { return (mPath.isEmpty() ? mPath :
                                         mPath.split('/').last()); }
  inline QImage* getImage() { return mImage; }
  inline void setImage(const QImage &image) { *mImage = image; }

  inline void setEdited(bool flag) { mIsEdited = flag; }
  inline bool getEdited() { return mIsEdited; }

  inline void setZoomFactor(qreal factor) { mZoomFactor *= factor; }
  inline qreal getZoomFactor() { return mZoomFactor; }

  inline QUndoStack* getUndoStack() { return mUndoStack; }
  inline void setIsPaint(bool isPaint) { mIsPaint = isPaint; }
  inline bool isPaint() { return mIsPaint; }
  inline void emitPrimaryColorView() { emit sendPrimaryColorView(); }
  inline void emitSecondaryColorView() { emit sendSecondaryColorView(); }
  inline void emitColor(QColor &color) { emit sendColor(color); }
  inline void emitRestorePreviousTool() { emit sendRestorePreviousTool(); }

private:
  void initImage();
  void open();
  void open(const QString &filePath);
  void drawCursor();
  void makeFormatsFilters();

  QImage *mImage,  mImageCopy;
  ImageEditTools *mEditTools;
  QString mPath;
  QString mOpenFilter;
  QString mSaveFilter;
  bool mIsEdited, mIsPaint, mIsResize, mRightButtonPressed;
  QPixmap *mPixmap;
  QCursor *mCurrentCursor;
  qreal mZoomFactor;
  QUndoStack *mUndoStack;
  QVector<AbstractTool*> mToolsHandlers;
  AbstractTool *mToolHandler;

signals:
  void sendPrimaryColorView();
  void sendSecondaryColorView();
  void sendNewImageSize(const QSize&);
  void sendCursorPosition(const QPoint&);
  void sendColor(const QColor&);
  void sendRestorePreviousTool();
  void sendSetTool(ToolsEnum);
  void sendEnableCopyCutActions(bool enable);
  void sendEnableSelectionTool(bool enable);

private slots:
  void autoSave();

protected:
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  void paintEvent(QPaintEvent *event);
};

#endif // IMAGESCENE_H
