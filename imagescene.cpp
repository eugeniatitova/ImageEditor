#include "imagescene.h"
#include "settingsstore.h"
#include "customundocmd.h"

#include "tools/abstracttool.h"
#include "tools/penciltool.h"
#include "tools/linetool.h"
#include "tools/erasertool.h"
#include "tools/rectangletool.h"
#include "tools/ellipsetool.h"
#include "tools/filltool.h"
#include "tools/spraytool.h"
#include "tools/colorpickertool.h"
#include "tools/selectiontool.h"
#include "tools/curvelinetool.h"
#include "tools/texttool.h"
#include "widgets/resizeimagedialog.h"

#include <QApplication>
#include <QPainter>
#include <QFileDialog>
#include <QtCore/QDebug>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QtCore/QTimer>
#include <QImageReader>
#include <QImageWriter>
#include <QUndoStack>
#include <QtCore/QDir>
#include <QMessageBox>
#include <QClipboard>

ImageScene::ImageScene(const bool &isOpen, const QString &filePath, QWidget *parent) :
  QWidget(parent), mIsEdited(false), mIsPaint(false), mIsResize(false)
{
  setMouseTracking(true);

  mRightButtonPressed = false;
  mPath = QString();
  makeFormatsFilters();
  initImage();
  mZoomFactor = 1;

  mEditTools = new ImageEditTools(this, this->parent());

  mUndoStack = new QUndoStack(this);
  mUndoStack->setUndoLimit(SettingsStore::Instance()->getHistoryDepth());

  if(isOpen && filePath.isEmpty())
  {
      open();
  }
  else if(isOpen && !filePath.isEmpty())
  {
      open(filePath);
  }
  else
  {
      int width, height;
      width = SettingsStore::Instance()->getBaseSize().width();
      height = SettingsStore::Instance()->getBaseSize().height();
      if (SettingsStore::Instance()->getIsInitialized() &&
          SettingsStore::Instance()->getIsAskCanvasSize())
      {
          QClipboard *globalClipboard = QApplication::clipboard();
          QImage mClipboardImage = globalClipboard->image();
          if (!mClipboardImage.isNull())
          {
              width = mClipboardImage.width();
              height = mClipboardImage.height();
          }
          ResizeImageDialog resizeDialog(QSize(width, height), this);
          if(resizeDialog.exec() != QDialog::Accepted)
              return;
          QSize newSize = resizeDialog.getSizeAfter();
          width = newSize.width();
          height = newSize.height();
          mEditTools->resizeCanvas(width, height, false);
          mIsEdited = false;
      }
      QPainter *painter = new QPainter(mImage);
      painter->fillRect(0, 0, width, height, Qt::white);
      painter->end();

      resize(mImage->rect().right() + 6,
             mImage->rect().bottom() + 6);
      mPath = QString("");
  }

  QTimer *autoSaveTimer = new QTimer(this);
  autoSaveTimer->setInterval(SettingsStore::Instance()->getAutoSaveInterval() * 1000);
  connect(autoSaveTimer, SIGNAL(timeout()), this, SLOT(autoSave()));
  connect(mEditTools, SIGNAL(sendNewImageSize(QSize)), this, SIGNAL(sendNewImageSize(QSize)));

  autoSaveTimer->start();

  SelectionTool *selectionTool = new SelectionTool(this);
  connect(selectionTool, SIGNAL(sendEnableCopyCutActions(bool)), this, SIGNAL(sendEnableCopyCutActions(bool)));
  connect(selectionTool, SIGNAL(sendEnableSelectionInstrument(bool)), this, SIGNAL(sendEnableSelectionInstrument(bool)));

  // Instruments handlers
  mToolsHandlers.fill(0, (int)TOOLS_COUNT);
  mToolsHandlers[CURSOR] = selectionTool;
  mToolsHandlers[PEN] = new PencilTool(this);
  mToolsHandlers[LINE] = new LineTool(this);
  mToolsHandlers[ERASER] = new EraserTool(this);
  mToolsHandlers[RECTANGLE] = new RectangleTool(this);
  mToolsHandlers[ELLIPSE] = new EllipseTool(this);
  mToolsHandlers[FILL] = new FillTool(this);
  mToolsHandlers[SPRAY] = new SprayTool(this);
  mToolsHandlers[COLORPICKER] = new ColorPickerTool(this);
  mToolsHandlers[CURVELINE] = new CurveLineTool(this);
  mToolsHandlers[TEXT] = new TextTool(this);
}

ImageScene::~ImageScene()
{

}

void ImageScene::initImage()
{
    mImage = new QImage(SettingsStore::Instance()->getBaseSize(),
                        QImage::Format_ARGB32_Premultiplied);
}

void ImageScene::open()
{
    QString fileName(mPath);
    QFileDialog dialog(this, "Открыть изображение...", "", mOpenFilter);
    QString prevPath = SettingsStore::Instance()->getLastFilePath();

    if (!prevPath.isEmpty())
        dialog.selectFile(prevPath);
    else
        dialog.setDirectory(QDir::homePath());

    if (dialog.exec())
    {
        QStringList selectedFiles = dialog.selectedFiles();
        if (!selectedFiles.isEmpty())
        {
          open(selectedFiles.takeFirst());
        }
    }
}

void ImageScene::open(const QString &filePath)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    if(mImage->load(filePath))
    {
        *mImage = mImage->convertToFormat(QImage::Format_ARGB32_Premultiplied);
        mPath = filePath;
        SettingsStore::Instance()->setLastFilePath(filePath);
        resize(mImage->rect().right() + 6,
               mImage->rect().bottom() + 6);
        QApplication::restoreOverrideCursor();
    }
    else
    {
        qDebug()<<QString("Невозможно открыть файл %1").arg(filePath);
        QApplication::restoreOverrideCursor();
        QMessageBox::warning(this, "Ошибка открытия файла", tr("Невозможно открыть файл \"%1\".").arg(filePath));
    }
}

bool ImageScene::save()
{
    if(mPath.isEmpty())
    {
        return saveAs();
    }
    clearSelection();
    if (!mImage->save(mPath))
    {
        QMessageBox::warning(this, "Ошибка сохранения файла", tr("Невозможно сохранить файл \"%1\".").arg(mPath));
        return false;
    }
    mIsEdited = false;
    return true;
}

bool ImageScene::saveAs()
{
    bool result = true;
    QString filter;
    QString fileName(mPath);
    clearSelection();
    if(fileName.isEmpty())
    {
        fileName = QDir::homePath() + "/" + "Новое изображение" + ".png";
    }
    QString filePath = QFileDialog::getSaveFileName(this, "Сохранить изображение...", fileName, mSaveFilter,
                                                    &filter,
                                                    QFileDialog::DontUseNativeDialog);

    QApplication::setOverrideCursor(Qt::WaitCursor);
    if(!filePath.isEmpty())
    {
        QString extension;
        QString temp = filePath.split("/").last();
        if(temp.contains('.'))
        {
            temp = temp.split('.').last();
            if(QImageWriter::supportedImageFormats().contains(temp.toLatin1()))
                extension = temp;
            else
                extension = "png";
        }
        else
        {
            extension = filter.split('.').last().remove(')');
            filePath += '.' + extension;
        }

        if(mImage->save(filePath, extension.toLatin1().data()))
        {
            mPath = filePath;
            mIsEdited = false;
        }
        else
        {
            QMessageBox::warning(this, tr("Ошибка сохранения файла"), tr("Невозможно сохранить файл \"%1\".").arg(filePath));
            result = false;
        }
    }
    QApplication::restoreOverrideCursor();
    return result;
}

void ImageScene::autoSave()
{
    if(mIsEdited && !mPath.isEmpty() && SettingsStore::Instance()->getIsAutoSave())
    {
        if(mImage->save(mPath)) {
            mIsEdited = false;
        }
    }
}

void ImageScene::resizeImage()
{
    mEditTools->resizeImage();
    emit sendNewImageSize(mImage->size());
}

void ImageScene::resizeCanvas()
{
    mEditTools->resizeCanvas(mImage->width(), mImage->height(), true);
    emit sendNewImageSize(mImage->size());
}

void ImageScene::rotateImage(bool flag)
{
    mEditTools->rotateImage(flag);
    emit sendNewImageSize(mImage->size());
}

bool ImageScene::zoomImage(qreal factor)
{
    return mEditTools->zoomImage(factor);
}

void ImageScene::copyImage()
{
    SelectionTool *instrument = static_cast <SelectionTool*> (mToolsHandlers.at(CURSOR));
    instrument->copyImage(*this);
}

void ImageScene::pasteImage()
{
    if(SettingsStore::Instance()->getTool() != CURSOR)
        emit sendSetTool(CURSOR);
    SelectionTool *instrument = static_cast <SelectionTool*> (mToolsHandlers.at(CURSOR));
    instrument->pasteImage(*this);
}

void ImageScene::cutImage()
{
    SelectionTool *instrument = static_cast <SelectionTool*> (mToolsHandlers.at(CURSOR));
    instrument->cutImage(*this);
}

void ImageScene::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton &&
            event->pos().x() < mImage->rect().right() + 6 &&
            event->pos().x() > mImage->rect().right() &&
            event->pos().y() > mImage->rect().bottom() &&
            event->pos().y() < mImage->rect().bottom() + 6)
    {
        mIsResize = true;
        setCursor(Qt::SizeFDiagCursor);
    }
    else if(SettingsStore::Instance()->getTool() != NONE_INSTRUMENT)
    {
        mToolHandler = mToolsHandlers.at(SettingsStore::Instance()->getTool());
        mToolHandler->mousePressEvent(event, *this);
    }
}

void ImageScene::mouseMoveEvent(QMouseEvent *event)
{
    ToolsEnum instrument = SettingsStore::Instance()->getTool();
    mToolHandler = mToolsHandlers.at(SettingsStore::Instance()->getTool());
    if(mIsResize)
    {
         mEditTools->resizeCanvas(event->x(), event->y());
         emit sendNewImageSize(mImage->size());
    }
    else if(event->pos().x() < mImage->rect().right() + 6 &&
            event->pos().x() > mImage->rect().right() &&
            event->pos().y() > mImage->rect().bottom() &&
            event->pos().y() < mImage->rect().bottom() + 6)
    {
        setCursor(Qt::SizeFDiagCursor);
        if (qobject_cast<AbstractAreaSelecting*>(mToolHandler))
            return;
    }
    else if (!qobject_cast<AbstractAreaSelecting*>(mToolHandler))
    {
        restoreCursor();
    }
    if(event->pos().x() < mImage->width() &&
            event->pos().y() < mImage->height())
    {
        emit sendCursorPosition(event->pos());
    }

    if(instrument != NONE_INSTRUMENT)
    {
        mToolHandler->mouseMoveEvent(event, *this);
    }
}

void ImageScene::mouseReleaseEvent(QMouseEvent *event)
{
    if(mIsResize)
    {
       mIsResize = false;
       restoreCursor();
    }
    else if(SettingsStore::Instance()->getTool() != NONE_INSTRUMENT)
    {
        mToolHandler = mToolsHandlers.at(SettingsStore::Instance()->getTool());
        mToolHandler->mouseReleaseEvent(event, *this);
    }
}

void ImageScene::paintEvent(QPaintEvent *event)
{
    QPainter *painter = new QPainter(this);
    //QRect *rect = new QRect(event->rect());

    painter->setBrush(QBrush(QPixmap(":media/textures/transparent.jpg")));
    painter->drawRect(0, 0,
                      mImage->rect().right() - 1,
                      mImage->rect().bottom() - 1);

    painter->drawImage(event->rect(), *mImage, event->rect());

    painter->setPen(Qt::NoPen);
    painter->setBrush(QBrush(Qt::black));
    painter->drawRect(QRect(mImage->rect().right(),
                            mImage->rect().bottom(), 6, 6));

    painter->end();
}

void ImageScene::restoreCursor()
{
    switch(SettingsStore::Instance()->getTool())
    {
    case TOOLS_COUNT:
        break;
    case NONE_INSTRUMENT:
        mCurrentCursor = new QCursor(Qt::ArrowCursor);
        setCursor(*mCurrentCursor);
        break;
    case CURSOR:
        mCurrentCursor = new QCursor(Qt::CrossCursor);
        setCursor(*mCurrentCursor);
        break;
    case ERASER: case PEN:
        ImageScene::drawCursor();
        mCurrentCursor = new QCursor(*mPixmap);
        setCursor(*mCurrentCursor);
        break;
    case COLORPICKER:
        mPixmap = new QPixmap(":/media/instruments-icons/cursor_pipette.png");
        mCurrentCursor = new QCursor(*mPixmap);
        setCursor(*mCurrentCursor);
        break;
    case RECTANGLE: case ELLIPSE: case LINE: case CURVELINE: case TEXT:
        mCurrentCursor = new QCursor(Qt::CrossCursor);
        setCursor(*mCurrentCursor);
        break;
    case SPRAY:
        mPixmap = new QPixmap(":/media/instruments-icons/cursor_spray.png");
        mCurrentCursor = new QCursor(*mPixmap);
        setCursor(*mCurrentCursor);
        break;
    case FILL:
        mPixmap = new QPixmap(":/media/instruments-icons/cursor_fill.png");
        mCurrentCursor = new QCursor(*mPixmap);
        setCursor(*mCurrentCursor);
        break;
    }
}

void ImageScene::drawCursor()
{
    QPainter painter;
    mPixmap = new QPixmap(25, 25);
    QPoint center(13, 13);
    switch(SettingsStore::Instance()->getTool())
    {
    case NONE_INSTRUMENT: case LINE: case COLORPICKER: case  SPRAY:
    case FILL: case RECTANGLE: case ELLIPSE: case CURSOR: case TOOLS_COUNT:
    case CURVELINE: case TEXT:
        break;
    case PEN: case ERASER:
        mPixmap->fill(QColor(0, 0, 0, 0));
        break;
    }
    painter.begin(mPixmap);
    switch(SettingsStore::Instance()->getTool())
    {
    case NONE_INSTRUMENT: case LINE: case COLORPICKER: case  SPRAY:
    case FILL: case RECTANGLE: case ELLIPSE: case CURSOR: case TOOLS_COUNT:
    case CURVELINE: case TEXT:
        break;
    case PEN:
        if(mRightButtonPressed)
        {
            painter.setPen(QPen(SettingsStore::Instance()->getSecondaryColor()));
            painter.setBrush(QBrush(SettingsStore::Instance()->getSecondaryColor()));
        }
        else
        {
            painter.setPen(QPen(SettingsStore::Instance()->getPrimaryColor()));
            painter.setBrush(QBrush(SettingsStore::Instance()->getPrimaryColor()));
        }
        painter.drawEllipse(center, SettingsStore::Instance()->getPenSize()/2,
                        SettingsStore::Instance()->getPenSize()/2);
        break;
    case ERASER:
        painter.setBrush(QBrush(Qt::white));
        painter.drawEllipse(center, SettingsStore::Instance()->getPenSize()/2,
                        SettingsStore::Instance()->getPenSize()/2);
        break;
    }
    painter.setPen(Qt::black);
    painter.drawPoint(13, 13);
    painter.drawPoint(13, 3);
    painter.drawPoint(13, 5);
    painter.drawPoint(13, 21);
    painter.drawPoint(13, 23);
    painter.drawPoint(3, 13);
    painter.drawPoint(5, 13);
    painter.drawPoint(21, 13);
    painter.drawPoint(23, 13);
    painter.setPen(Qt::white);
    painter.drawPoint(13, 12);
    painter.drawPoint(13, 14);
    painter.drawPoint(12, 13);
    painter.drawPoint(14, 13);
    painter.drawPoint(13, 4);
    painter.drawPoint(13, 6);
    painter.drawPoint(13, 20);
    painter.drawPoint(13, 22);
    painter.drawPoint(4, 13);
    painter.drawPoint(6, 13);
    painter.drawPoint(20, 13);
    painter.drawPoint(22, 13);
    painter.end();
}

void ImageScene::makeFormatsFilters()
{
    QList<QByteArray> ba = QImageReader::supportedImageFormats();
    mOpenFilter = "All supported (";
    foreach (QByteArray temp, ba)
        mOpenFilter += "*." + temp + " ";
    mOpenFilter[mOpenFilter.length() - 1] = ')'; //delete last space
    mOpenFilter += ";;";

    if(ba.contains("png"))
        mOpenFilter += "Portable Network Graphics(*.png);;";
    if(ba.contains("bmp"))
        mOpenFilter += "Windows Bitmap(*.bmp);;";
    if(ba.contains("gif"))
        mOpenFilter += "Graphic Interchange Format(*.gif);;";
    if(ba.contains("jpg") || ba.contains("jpeg"))
        mOpenFilter += "Joint Photographic Experts Group(*.jpg *.jpeg);;";
    if(ba.contains("mng"))
        mOpenFilter += "Multiple-image Network Graphics(*.mng);;";
    if(ba.contains("pbm"))
        mOpenFilter += "Portable Bitmap(*.pbm);;";
    if(ba.contains("pgm"))
        mOpenFilter += "Portable Graymap(*.pgm);;";
    if(ba.contains("ppm"))
        mOpenFilter += "Portable Pixmap(*.ppm);;";
    if(ba.contains("tiff") || ba.contains("tif"))
        mOpenFilter += "Tagged Image File Format(*.tiff, *tif);;";
    if(ba.contains("xbm"))
        mOpenFilter += "X11 Bitmap(*.xbm);;";
    if(ba.contains("xpm"))
        mOpenFilter += "X11 Pixmap(*.xpm);;";
    if(ba.contains("svg"))
        mOpenFilter += "Scalable Vector Graphics(*.svg);;";

    mOpenFilter += "All Files(*.*)";

    ba = QImageWriter::supportedImageFormats();
    if(ba.contains("png"))
        mSaveFilter += "Portable Network Graphics(*.png)";
    if(ba.contains("bmp"))
        mSaveFilter += ";;Windows Bitmap(*.bmp)";
    if(ba.contains("jpg") || ba.contains("jpeg"))
        mSaveFilter += ";;Joint Photographic Experts Group(*.jpg)";
    if(ba.contains("ppm"))
        mSaveFilter += ";;Portable Pixmap(*.ppm)";
    if(ba.contains("tiff") || ba.contains("tif"))
        mSaveFilter += ";;Tagged Image File Format(*.tiff)";
    if(ba.contains("xbm"))
        mSaveFilter += ";;X11 Bitmap(*.xbm)";
    if(ba.contains("xpm"))
        mSaveFilter += ";;X11 Pixmap(*.xpm)";
}

void ImageScene::saveChanges()
{
    foreach (AbstractTool* instrument, mToolsHandlers)
    {
        if (AbstractAreaSelecting *selection = qobject_cast<AbstractAreaSelecting*>(instrument))
            selection->saveImageChanges(*this);
    }
}

void ImageScene::clearSelection()
{
    foreach (AbstractTool* instrument, mToolsHandlers)
    {
        if (AbstractAreaSelecting *selection = qobject_cast<AbstractAreaSelecting*>(instrument))
            selection->clearSelection(*this);
    }
}

void ImageScene::pushUndoCommand(CustomUndoCmd *command)
{
    if(command != 0)
        mUndoStack->push(command);
}
