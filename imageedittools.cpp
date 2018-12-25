#include "imageedittools.h"
#include "imagescene.h"
#include "widgets/resizeimagedialog.h"

#include <QImage>
#include <QPainter>
#include <QLabel>
#include <QTransform>
#include <QSize>
#include <QClipboard>
#include <QApplication>

ImageEditTools::ImageEditTools(ImageScene *pImageScene, QObject *parent) :
  QObject(parent)
{
  mPImageScene = pImageScene;
  mZoomFactor = 1;
}

ImageEditTools::~ImageEditTools() {}

void ImageEditTools::resizeCanvas(int width, int height, bool flag)
{
    if(flag)
    {
        ResizeImageDialog resizeDialog(QSize(width, height), qobject_cast<QWidget *>(this->parent()));
        if(resizeDialog.exec() == QDialog::Accepted)
        {
            QSize newSize = resizeDialog.getSizeAfter();
            width = newSize.width();
            height = newSize.height();
        } else {
            return;
        }
    }

    if(width < 1 || height < 1)
        return;
    QImage *tempImage = new QImage(width, height, QImage::Format_ARGB32_Premultiplied);
    QPainter painter(tempImage);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(Qt::white));
    painter.drawRect(QRect(0, 0, width, height));
    painter.drawImage(0, 0, *mPImageScene->getImage());
    painter.end();

    mPImageScene->setImage(*tempImage);

    mPImageScene->resize(mPImageScene->getImage()->rect().right() + 6,
                        mPImageScene->getImage()->rect().bottom() + 6);
    mPImageScene->setEdited(true);
    mPImageScene->clearSelection();
}
void ImageEditTools::resizeImage()
{
    ResizeImageDialog resizeImageDialog(mPImageScene->getImage()->size(), qobject_cast<QWidget *>(this->parent()));
    if(resizeImageDialog.exec() == QDialog::Accepted)
    {
        mPImageScene->setImage(mPImageScene->getImage()->scaled(resizeImageDialog.getSizeAfter()));
        mPImageScene->resize(mPImageScene->getImage()->rect().right() + 6,
                            mPImageScene->getImage()->rect().bottom() + 6);
        mPImageScene->setEdited(true);
        mPImageScene->clearSelection();
    }
}

void ImageEditTools::rotateImage(bool flag)
{
    QTransform transform;
    if(flag)
    {
        transform.rotate(90);
    }
    else
    {
        transform.rotate(-90);
    }
    mPImageScene->setImage(mPImageScene->getImage()->transformed(transform));
    mPImageScene->resize(mPImageScene->getImage()->rect().right() + 6,
                        mPImageScene->getImage()->rect().bottom() + 6);
    mPImageScene->update();
    mPImageScene->setEdited(true);
    mPImageScene->clearSelection();
}

bool ImageEditTools::zoomImage(qreal factor)
{
    mZoomFactor *= factor;
    if(mZoomFactor < 0.25)
    {
        mZoomFactor = 0.25;
        return false;
    }
    else if(mZoomFactor > 4)
    {
        mZoomFactor = 4;
        return false;
    }
    else
    {
        mPImageScene->setImage(mPImageScene->getImage()->transformed(QTransform::fromScale(factor, factor)));
        mPImageScene->resize((mPImageScene->rect().width())*factor, (mPImageScene->rect().height())*factor);
        emit sendNewImageSize(mPImageScene->size());
        mPImageScene->setEdited(true);
        mPImageScene->clearSelection();
        return true;
    }
}
