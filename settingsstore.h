#ifndef SETTINGSSTORE_H
#define SETTINGSSTORE_H

#include <QColor>
#include <QtCore/QSize>
#include <QtCore/QString>
#include <QtCore/QMap>
#include <QFont>

#include "params.h"

class SettingsStore
{
public:
  //Возвращает единственный экземпляр класса
  static SettingsStore* Instance();

  inline QColor getPrimaryColor() { return mPrimaryColor; }
  inline void setPrimaryColor(const QColor &color) { mPrimaryColor = color; }

  inline QColor getSecondaryColor() { return mSecondaryColor; }
  inline void setSecondaryColor(const QColor &color) { mSecondaryColor = color; }

  inline int getPenSize() { return mPenSize; }
  inline void setPenSize(const int &size) { mPenSize = size; }

  inline ToolsEnum getTool() { return mCurrentTool; }
  inline void setTool(const ToolsEnum &instrument) { mCurrentTool = instrument; mIsResetCurve = true; }

  inline ToolsEnum getPreviousTool() { return mPreviousTool; }
  inline void setPreviousTool(const ToolsEnum &instrument) { mPreviousTool = instrument; }

  inline QSize getBaseSize() { return mBaseSize; }
  inline void setBaseSize(const QSize &baseSize) { mBaseSize = baseSize; }

  inline bool getIsAutoSave() { return mIsAutoSave; }
  inline void setIsAutoSave(const bool &isAutoSave) { mIsAutoSave = isAutoSave; }

  inline int getAutoSaveInterval() { return mAutoSaveTime; }
  inline void setAutoSaveInterval(const int &interval) { mAutoSaveTime = interval; }

  inline int getHistoryDepth() { return mHistoryDepth; }
  inline void setHistoryDepth(const int &historyDepth) { mHistoryDepth = historyDepth; }

  inline bool getIsRestoreWindowSize() { return mIsRestoreWindowSize; }
  inline void setIsRestoreWindowSize(const bool &isRestoreWindowSize) { mIsRestoreWindowSize = isRestoreWindowSize; }

  inline bool getIsAskCanvasSize() { return mIsAskCanvasSize; }
  inline void setIsAskCanvasSize(const bool &isAskCanvasSize) { mIsAskCanvasSize = isAskCanvasSize; }

  inline bool getIsInitialized() { return mIsInitialized; }
  inline void setIsInitialized(const bool &isInitialized = true) { mIsInitialized = isInitialized; }

  inline QString getLastFilePath() { return mLastFilePath; }
  inline void setLastFilePath(const QString &lastFilePath) { mLastFilePath = lastFilePath; }

  inline QSize getWindowSize() { return mWindowSize; }
  inline void setWindowSize(const QSize &winSize) { mWindowSize = winSize; }

  inline QFont getTextFont() { return mTextFont; }
  inline void setTextFont(const QFont textFont) { mTextFont = textFont; }

  //Необходима для корректной работы кривой Безье
  inline void setResetCurve(bool b) { mIsResetCurve = b; }
  inline bool isResetCurve() { return mIsResetCurve; }

  void loadSetting();
  void saveSettings();
  void readSize();
  void writeSize();

private:
  SettingsStore();
  SettingsStore(SettingsStore const&){}

  static SettingsStore* pInstance;
  QColor mPrimaryColor,
         mSecondaryColor;
  int mPenSize;
  ToolsEnum mCurrentTool, mPreviousTool;
  QSize mBaseSize, mWindowSize;
  bool mIsAutoSave, mIsRestoreWindowSize, mIsAskCanvasSize, mIsInitialized;
  bool mIsResetCurve; //Корректирует работу кривой Безье
  int mAutoSaveTime, mHistoryDepth;
  QString mLastFilePath;
  QFont mTextFont;
};

#endif // SETTINGSSTORE_H
