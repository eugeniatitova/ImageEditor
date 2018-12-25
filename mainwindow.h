#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QtCore/QMap>

#include "params.h"

QT_BEGIN_NAMESPACE
class QAction;
class QStatusBar;
class QTabWidget;
class ToolBar;
class ImageScene;
class QLabel;
class QUndoGroup;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QStringList filePaths, QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

private:
    void initializeMainMenu();
    void initializeStatusBar();
    void initializeToolBar();
    void initializeTabWidget();

    void initializeNewTab(const bool &isOpen = false, const QString &filePath = "");

    ImageScene* getCurrentImageArea();
    ImageScene* getImageAreaByIndex(int index);
    bool closeAllTabs();
    bool isSomethingModified();

    void updateShortcuts();

    QStatusBar *mStatusBar;
    QTabWidget *mTabWidget;
    ToolBar *mToolbar;
    QLabel *mSizeLabel, *mPosLabel, *mColorPreviewLabel, *mColorRGBLabel;

    QMap<ToolsEnum, QAction*> mInstrumentsActMap;
    QAction *mSaveAction, *mSaveAsAction, *mCloseAction, *mPrintAction,
            *mUndoAction, *mRedoAction, *mCopyAction, *mCutAction,
            *mNewAction, *mOpenAction, *mExitAction, *mPasteAction, *mZoomInAction, *mZoomOutAction;
    QMenu *mInstrumentsMenu, *mToolsMenu;
    QUndoGroup *mUndoStackGroup;
    bool mPrevInstrumentSetted;
private slots:
    void activateTab(const int &index);
    void setNewSizeToSizeLabel(const QSize &size);
    void setNewPosToPosLabel(const QPoint &pos);
    void setCurrentPipetteColor(const QColor &color);
    void clearStatusBarColor();
    void setInstrumentChecked(ToolsEnum instrument);
    void newAct();
    void openAct();
    void helpAct();
    void saveAct();
    void saveAsAct();
    void copyAct();
    void pasteAct();
    void cutAct();
    void settingsAct();
    void resizeImageAct();
    void resizeCanvasAct();
    void rotateLeftImageAct();
    void rotateRightImageAct();
    void zoomInAct();
    void zoomOutAct();
    void advancedZoomAct();
    void closeTabAct();
    void closeTab(int index);
    void setAllInstrumentsUnchecked(QAction *action);
    void instumentsAct(bool state);
    void enableActions(int index);
    void enableCopyCutActions(bool enable);
    void clearImageSelection();
    void restorePreviousInstrument();
    void setInstrument(ToolsEnum instrument);
signals:
    void sendInstrumentChecked(ToolsEnum);
};

#endif // MAINWINDOW_H
