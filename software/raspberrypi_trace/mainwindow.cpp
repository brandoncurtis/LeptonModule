#include "mainwindow.h"

#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QFile>

#include "LeptonThread.h"

#include <stdio.h>
#include <time.h>
#include <QDateTime>

int MainWindow::snapshotCount = 0;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , rawData(LeptonThread::FrameWords)
    , rgbImage(LeptonThread::FrameWidth, LeptonThread::FrameHeight, QImage::Format_RGB888) {


    QWidget *mainWidget = new QWidget();
    setCentralWidget(mainWidget);

    layout = new QGridLayout();
    mainWidget->setLayout(layout);

    imageLabel = new QLabel();
    layout->addWidget(imageLabel, 0, 0, Qt::AlignCenter);

    QPixmap filler(ImageWidth, ImageHeight); filler.fill(Qt::red);
    imageLabel->setPixmap(filler);

    thread = new LeptonThread();
    connect(thread, SIGNAL(updateImage(unsigned short *,int,int)), this, SLOT(updateImage(unsigned short *,int,int)));

    QPushButton *refreshButton = new QPushButton("Refresh View");
    layout->addWidget(refreshButton, 1, 0, Qt::AlignCenter);
    connect(refreshButton, SIGNAL(clicked()), this, SLOT(updateScreen()));

    thread->start();
}

// Rainbow: const int colormap[] = {1, 3, 74, 0, 3, 74, 0, 3, 75, 0, 3, 75, 0, 3, 76, 0, 3, 76, 0, 3, 77, 0, 3, 79, 0, 3, 82, 0, 5, 85, 0, 7, 88, 0, 10, 91, 0, 14, 94, 0, 19, 98, 0, 22, 100, 0, 25, 103, 0, 28, 106, 0, 32, 109, 0, 35, 112, 0, 38, 116, 0, 40, 119, 0, 42, 123, 0, 45, 128, 0, 49, 133, 0, 50, 134, 0, 51, 136, 0, 52, 137, 0, 53, 139, 0, 54, 142, 0, 55, 144, 0, 56, 145, 0, 58, 149, 0, 61, 154, 0, 63, 156, 0, 65, 159, 0, 66, 161, 0, 68, 164, 0, 69, 167, 0, 71, 170, 0, 73, 174, 0, 75, 179, 0, 76, 181, 0, 78, 184, 0, 79, 187, 0, 80, 188, 0, 81, 190, 0, 84, 194, 0, 87, 198, 0, 88, 200, 0, 90, 203, 0, 92, 205, 0, 94, 207, 0, 94, 208, 0, 95, 209, 0, 96, 210, 0, 97, 211, 0, 99, 214, 0, 102, 217, 0, 103, 218, 0, 104, 219, 0, 105, 220, 0, 107, 221, 0, 109, 223, 0, 111, 223, 0, 113, 223, 0, 115, 222, 0, 117, 221, 0, 118, 220, 1, 120, 219, 1, 122, 217, 2, 124, 216, 2, 126, 214, 3, 129, 212, 3, 131, 207, 4, 132, 205, 4, 133, 202, 4, 134, 197, 5, 136, 192, 6, 138, 185, 7, 141, 178, 8, 142, 172, 10, 144, 166, 10, 144, 162, 11, 145, 158, 12, 146, 153, 13, 147, 149, 15, 149, 140, 17, 151, 132, 22, 153, 120, 25, 154, 115, 28, 156, 109, 34, 158, 101, 40, 160, 94, 45, 162, 86, 51, 164, 79, 59, 167, 69, 67, 171, 60, 72, 173, 54, 78, 175, 48, 83, 177, 43, 89, 179, 39, 93, 181, 35, 98, 183, 31, 105, 185, 26, 109, 187, 23, 113, 188, 21, 118, 189, 19, 123, 191, 17, 128, 193, 14, 134, 195, 12, 138, 196, 10, 142, 197, 8, 146, 198, 6, 151, 200, 5, 155, 201, 4, 160, 203, 3, 164, 204, 2, 169, 205, 2, 173, 206, 1, 175, 207, 1, 178, 207, 1, 184, 208, 0, 190, 210, 0, 193, 211, 0, 196, 212, 0, 199, 212, 0, 202, 213, 1, 207, 214, 2, 212, 215, 3, 215, 214, 3, 218, 214, 3, 220, 213, 3, 222, 213, 4, 224, 212, 4, 225, 212, 5, 226, 212, 5, 229, 211, 5, 232, 211, 6, 232, 211, 6, 233, 211, 6, 234, 210, 6, 235, 210, 7, 236, 209, 7, 237, 208, 8, 239, 206, 8, 241, 204, 9, 242, 203, 9, 244, 202, 10, 244, 201, 10, 245, 200, 10, 245, 199, 11, 246, 198, 11, 247, 197, 12, 248, 194, 13, 249, 191, 14, 250, 189, 14, 251, 187, 15, 251, 185, 16, 252, 183, 17, 252, 178, 18, 253, 174, 19, 253, 171, 19, 254, 168, 20, 254, 165, 21, 254, 164, 21, 255, 163, 22, 255, 161, 22, 255, 159, 23, 255, 157, 23, 255, 155, 24, 255, 149, 25, 255, 143, 27, 255, 139, 28, 255, 135, 30, 255, 131, 31, 255, 127, 32, 255, 118, 34, 255, 110, 36, 255, 104, 37, 255, 101, 38, 255, 99, 39, 255, 93, 40, 255, 88, 42, 254, 82, 43, 254, 77, 45, 254, 69, 47, 254, 62, 49, 253, 57, 50, 253, 53, 52, 252, 49, 53, 252, 45, 55, 251, 39, 57, 251, 33, 59, 251, 32, 60, 251, 31, 60, 251, 30, 61, 251, 29, 61, 251, 28, 62, 250, 27, 63, 250, 27, 65, 249, 26, 66, 249, 26, 68, 248, 25, 70, 248, 24, 73, 247, 24, 75, 247, 25, 77, 247, 25, 79, 247, 26, 81, 247, 32, 83, 247, 35, 85, 247, 38, 86, 247, 42, 88, 247, 46, 90, 247, 50, 92, 248, 55, 94, 248, 59, 96, 248, 64, 98, 248, 72, 101, 249, 81, 104, 249, 87, 106, 250, 93, 108, 250, 95, 109, 250, 98, 110, 250, 100, 111, 251, 101, 112, 251, 102, 113, 251, 109, 117, 252, 116, 121, 252, 121, 123, 253, 126, 126, 253, 130, 128, 254, 135, 131, 254, 139, 133, 254, 144, 136, 254, 151, 140, 255, 158, 144, 255, 163, 146, 255, 168, 149, 255, 173, 152, 255, 176, 153, 255, 178, 155, 255, 184, 160, 255, 191, 165, 255, 195, 168, 255, 199, 172, 255, 203, 175, 255, 207, 179, 255, 211, 182, 255, 216, 185, 255, 218, 190, 255, 220, 196, 255, 222, 200, 255, 225, 202, 255, 227, 204, 255, 230, 206, 255, 233, 208};
const int colormap[] = {255, 255, 255, 253, 253, 253, 251, 251, 251, 249, 249, 249, 247, 247, 247, 245, 245, 245, 243, 243, 243, 241, 241, 241, 239, 239, 239, 237, 237, 237, 235, 235, 235, 233, 233, 233, 231, 231, 231, 229, 229, 229, 227, 227, 227, 225, 225, 225, 223, 223, 223, 221, 221, 221, 219, 219, 219, 217, 217, 217, 215, 215, 215, 213, 213, 213, 211, 211, 211, 209, 209, 209, 207, 207, 207, 205, 205, 205, 203, 203, 203, 201, 201, 201, 199, 199, 199, 197, 197, 197, 195, 195, 195, 193, 193, 193, 191, 191, 191, 189, 189, 189, 187, 187, 187, 185, 185, 185, 183, 183, 183, 181, 181, 181, 179, 179, 179, 177, 177, 177, 175, 175, 175, 173, 173, 173, 171, 171, 171, 169, 169, 169, 167, 167, 167, 165, 165, 165, 163, 163, 163, 161, 161, 161, 159, 159, 159, 157, 157, 157, 155, 155, 155, 153, 153, 153, 151, 151, 151, 149, 149, 149, 147, 147, 147, 145, 145, 145, 143, 143, 143, 141, 141, 141, 139, 139, 139, 137, 137, 137, 135, 135, 135, 133, 133, 133, 131, 131, 131, 129, 129, 129, 126, 126, 126, 124, 124, 124, 122, 122, 122, 120, 120, 120, 118, 118, 118, 116, 116, 116, 114, 114, 114, 112, 112, 112, 110, 110, 110, 108, 108, 108, 106, 106, 106, 104, 104, 104, 102, 102, 102, 100, 100, 100, 98, 98, 98, 96, 96, 96, 94, 94, 94, 92, 92, 92, 90, 90, 90, 88, 88, 88, 86, 86, 86, 84, 84, 84, 82, 82, 82, 80, 80, 80, 78, 78, 78, 76, 76, 76, 74, 74, 74, 72, 72, 72, 70, 70, 70, 68, 68, 68, 66, 66, 66, 64, 64, 64, 62, 62, 62, 60, 60, 60, 58, 58, 58, 56, 56, 56, 54, 54, 54, 52, 52, 52, 50, 50, 50, 48, 48, 48, 46, 46, 46, 44, 44, 44, 42, 42, 42, 40, 40, 40, 38, 38, 38, 36, 36, 36, 34, 34, 34, 32, 32, 32, 30, 30, 30, 28, 28, 28, 26, 26, 26, 24, 24, 24, 22, 22, 22, 20, 20, 20, 18, 18, 18, 16, 16, 16, 14, 14, 14, 12, 12, 12, 10, 10, 10, 8, 8, 8, 6, 6, 6, 4, 4, 4, 2, 2, 2, 0, 0, 0, 0, 0, 9, 2, 0, 16, 4, 0, 24, 6, 0, 31, 8, 0, 38, 10, 0, 45, 12, 0, 53, 14, 0, 60, 17, 0, 67, 19, 0, 74, 21, 0, 82, 23, 0, 89, 25, 0, 96, 27, 0, 103, 29, 0, 111, 31, 0, 118, 36, 0, 120, 41, 0, 121, 46, 0, 122, 51, 0, 123, 56, 0, 124, 61, 0, 125, 66, 0, 126, 71, 0, 127, 76, 1, 128, 81, 1, 129, 86, 1, 130, 91, 1, 131, 96, 1, 132, 101, 1, 133, 106, 1, 134, 111, 1, 135, 116, 1, 136, 121, 1, 136, 125, 2, 137, 130, 2, 137, 135, 3, 137, 139, 3, 138, 144, 3, 138, 149, 4, 138, 153, 4, 139, 158, 5, 139, 163, 5, 139, 167, 5, 140, 172, 6, 140, 177, 6, 140, 181, 7, 141, 186, 7, 141, 189, 10, 137, 191, 13, 132, 194, 16, 127, 196, 19, 121, 198, 22, 116, 200, 25, 111, 203, 28, 106, 205, 31, 101, 207, 34, 95, 209, 37, 90, 212, 40, 85, 214, 43, 80, 216, 46, 75, 218, 49, 69, 221, 52, 64, 223, 55, 59, 224, 57, 49, 225, 60, 47, 226, 64, 44, 227, 67, 42, 228, 71, 39, 229, 74, 37, 230, 78, 34, 231, 81, 32, 231, 85, 29, 232, 88, 27, 233, 92, 24, 234, 95, 22, 235, 99, 19, 236, 102, 17, 237, 106, 14, 238, 109, 12, 239, 112, 12, 240, 116, 12, 240, 119, 12, 241, 123, 12, 241, 127, 12, 242, 130, 12, 242, 134, 12, 243, 138, 12, 243, 141, 13, 244, 145, 13, 244, 149, 13, 245, 152, 13, 245, 156, 13, 246, 160, 13, 246, 163, 13, 247, 167, 13, 247, 171, 13, 248, 175, 14, 248, 178, 15, 249, 182, 16, 249, 185, 18, 250, 189, 19, 250, 192, 20, 251, 196, 21, 251, 199, 22, 252, 203, 23, 252, 206, 24, 253, 210, 25, 253, 213, 27, 254, 217, 28, 254, 220, 29, 255, 224, 30, 255, 227, 39, 255, 229, 53, 255, 231, 67, 255, 233, 81, 255, 234, 95, 255, 236, 109, 255, 238, 123, 255, 240, 137, 255, 242, 151, 255, 244, 165, 255, 246, 179, 255, 248, 193, 255, 249, 207, 255, 251, 221, 255, 253, 235, 255, 255, 24};

void MainWindow::updateImage(unsigned short *data, int minValue, int maxValue){
    // Record the raw data and min/max values
    memcpy(&rawData[0], data, 2*LeptonThread::FrameWords);
    
    qDebug() << "rawMin:";
    qDebug() << rawMin;
    qDebug() << "minValue:";
    qDebug() << minValue;
    qDebug() << "rawMax:";
    qDebug() << rawMax;
    qDebug() << "maxValue:";
    qDebug() << maxValue;
    qDebug() << "";
    
    //if ( !(rawMin == minValue && rawMax == maxValue) )
    if ( freshData )
    {
        rawMin = minValue; rawMax = maxValue;
        
        qDebug() << "rawMin:";
        qDebug() << rawMin;
        qDebug() << "minValue:";
        qDebug() << minValue;
        qDebug() << "rawMax:";
        qDebug() << rawMax;
        qDebug() << "maxValue:";
        qDebug() << maxValue;
        qDebug() << "";

        saveSnapshot();
        qDebug() << "snapshot saved";
        //updateScreen();
        //qDebug() << "screen updated";
    }

    // Map "rawData" to rgb values in "rgbImage" via the colormap
    // BC: NEVER MIND THIS
    //int diff = maxValue - minValue + 1;
    //for (int y = 0; y < LeptonThread::FrameHeight; ++y) {
    //    for (int x = 0; x < LeptonThread::FrameWidth; ++x) {
    //        int baseValue = rawData[LeptonThread::FrameWidth*y + x]; // take input value in [0, 65536)
    //        int scaledValue = 256*(baseValue - minValue)/diff; // map value to interval [0, 256), and set the pixel to its color value above
    //        rgbImage.setPixel(x, y, qRgb(colormap[3*scaledValue], colormap[3*scaledValue+1], colormap[3*scaledValue+2]));
    //    }
    //}

    // Update the on-screen image
    // BC: NEVER MIND THIS
    //QPixmap pixmap = QPixmap::fromImage(rgbImage).scaled(ImageWidth, ImageHeight, Qt::KeepAspectRatio);
    //QPainter painter(&pixmap);
    // ... mark up pixmap, if so desired
    //imageLabel->setPixmap(pixmap);

    sleep(1); // delay between loops in seconds
}

void MainWindow::updateScreen(){
    // WANT THE THING? PRESS THE BUTTON.
    // Map "rawData" to rgb values in "rgbImage" via the colormap
    int diff = rawMax - rawMin + 1;
    for (int y = 0; y < LeptonThread::FrameHeight; ++y) {
        for (int x = 0; x < LeptonThread::FrameWidth; ++x) {
            int baseValue = rawData[LeptonThread::FrameWidth*y + x]; // take input value in [0, 65536)
            int scaledValue = 256*(baseValue - rawMin)/diff; // map value to interval [0, 256), and set the pixel to its color value above
            rgbImage.setPixel(x, y, qRgb(colormap[3*scaledValue], colormap[3*scaledValue+1], colormap[3*scaledValue+2]));
        }
    }

    // Update the on-screen image
    QPixmap pixmap = QPixmap::fromImage(rgbImage).scaled(ImageWidth, ImageHeight, Qt::KeepAspectRatio);
    QPainter painter(&pixmap);
    // ... mark up pixmap, if so desired
    imageLabel->setPixmap(pixmap);
}

void MainWindow::saveSnapshot() {
    // Stamp with snapshotcount
    //++snapshotCount;
    //char buf [10];
    //sprintf( buf, "%08u", snapshotCount );

    // Stamp with timestamp
    //time_t rawtime;
    //struct tm * timeinfo;
    //time ( &rawtime );
    //timeinfo = localtime ( &rawtime );
    //char buf [50];
    //sprintf( buf, "%025s", asctime (timeinfo) );


    QDateTime dt = QDateTime::currentDateTime();
    dt.setTimeSpec(Qt::UTC);  // or Qt::OffsetFromUTC for offset from UTC
    //qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate);
    //char buf [sizeof "2016-01-01T012:00:00.000000"];
    //sprintf( buf, "%025s", QDateTime::currentDateTime().toString(Qt::ISODate) );

    // Raw file format: binary, one word for min value, one for max value, then 80*60 words of raw data
    QFile rawFile(QString("thermograph_%1.bin").arg( QDateTime::currentDateTime().toString(Qt::ISODate) ));
    rawFile.open(QIODevice::Truncate | QIODevice::ReadWrite);
    QDataStream rawOut(&rawFile);
    rawOut << rawMin << rawMax;
    rawOut.writeRawData((char *) &rawData[0], 2*rawData.size());
    rawFile.close();

    // JPG image, top quality
    // BC: NEVER MIND THIS
    // rgbImage.save(QString("rgb%1.jpg").arg(snapshotCount), "JPG", 100);
}
