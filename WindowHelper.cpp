#include "WindowHelper.h"

WindowHelper::WindowHelper()
{
}

void WindowHelper::sleep(int msecs)
{
    QTime dieTime = QTime::currentTime().addMSecs(msecs);
    while(QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}
