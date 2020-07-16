#include "RunGuard.h"

#include <QApplication>
#include <QPushButton>
#include <QObject>
#include <QSharedMemory>
#include <QSystemSemaphore>
#include <QCryptographicHash>
#include <QDebug>
#include <QProcess>

QString generateKeyHash( const QString& key, const QString& salt )
{
  QByteArray data;
  data.append( key.toUtf8() );
  data.append( salt.toUtf8() );
  data = QCryptographicHash::hash( data, QCryptographicHash::Sha1 ).toHex();
  return data;
}

RunGuard::RunGuard(const QString& key)
      : key(key)
      , memLockKey(generateKeyHash(key, "_memLockKey"))
      , sharedmemKey(generateKeyHash(key, "_sharedmemKey"))
      , sharedMem(sharedmemKey)
      , memLock(memLockKey, 1)
  {

  }

  RunGuard::~RunGuard()
  {
      release();
  }

  bool RunGuard::tryToRun()
  {
      memLock.acquire();

      if (sharedMem.attach())
      {
          memLock.release();
          return false;
      }

      sharedMem.create(sizeof(quint64));
      memLock.release();
      return true;
  }

  void RunGuard::release()
  {
      memLock.acquire();
      if (sharedMem.isAttached())
          sharedMem.detach();
      memLock.release();
  }
