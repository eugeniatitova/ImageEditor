#ifndef IMAGEEDITTOOLS_H
#define IMAGEEDITTOOLS_H

#include <QtCore/QObject>
#include <QImage>

QT_BEGIN_NAMESPACE
class ImageScene;
class QSize;
QT_END_NAMESPACE

class ImageEditTools : public QObject
{ 
  Q_OBJECT

public:
  explicit ImageEditTools(ImageScene *pImageScene, QObject *parent);
  ~ImageEditTools();
  void resizeImage();
  void resizeCanvas(int width, int height, bool flag = false);
  void rotateImage(bool flag);
  bool zoomImage(qreal factor);
private:
  ImageScene *mPImageScene;
  qreal mZoomFactor;

signals:
  void sendNewImageSize(const QSize&);
};

#endif // IMAGEEDITTOOLS_H
