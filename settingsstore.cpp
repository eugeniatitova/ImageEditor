#include "settingsstore.h"

#include <QtCore/QSettings>

SettingsStore* SettingsStore::pInstance = nullptr;

SettingsStore* SettingsStore::Instance()
{
    if(!pInstance)
        pInstance = new SettingsStore;

    return pInstance;
}

SettingsStore::SettingsStore()
{
  mPrimaryColor = Qt::black;
  mSecondaryColor = Qt::white;
  mPenSize = 1;
  mTextFont = QFont("Times", 12);
  mCurrentTool = NONE_INSTRUMENT;
  mPreviousTool = NONE_INSTRUMENT;
  mIsInitialized = false;
  loadSetting();
  readSize();
}

void SettingsStore::loadSetting()
{
    QSettings settings;
    mBaseSize = settings.value("/Settings/BaseSize", QSize(400, 300)).toSize();
    mIsAutoSave = settings.value("/Settings/IsAutoSave", false).toBool();
    mAutoSaveTime = settings.value("/Settings/AutoSaveTime", 300).toInt();
    mHistoryDepth = settings.value("/Settings/HistoryDepth", 40).toInt();
    mIsRestoreWindowSize = settings.value("/Settings/IsRestoreWindowSize", true).toBool();
    mIsAskCanvasSize = settings.value("/Settings/IsAskCanvasSize", true).toBool();
}

void SettingsStore::saveSettings()
{
    QSettings settings;
    settings.setValue("/Settings/BaseSize", mBaseSize);
    settings.setValue("/Settings/IsAutoSave", mIsAutoSave);
    settings.setValue("/Settings/AutoSaveTime", mAutoSaveTime);
    settings.setValue("/Settings/HistoryDepth", mHistoryDepth);
    settings.setValue("/Settings/IsRestoreWindowSize", mIsRestoreWindowSize);
    settings.setValue("/Settings/IsAskCanvasSize", mIsAskCanvasSize);
}

void SettingsStore::readSize()
{
    QSettings settings;
    mWindowSize = settings.value("/State/WindowSize", QSize()).toSize();
}

void SettingsStore::writeSize()
{
    QSettings settings;
    if (mWindowSize.isValid())
        settings.setValue("/State/WindowSize", mWindowSize);
}
