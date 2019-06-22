#ifndef QTTOOPENCV_H
#define QTTOOPENCV_H

#include <QImage>
#include <QPixmap>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>

namespace QtToOpencv
{
    class ImageConversion
    {
        public:
        static QImage  cvMatToQImage( const cv::Mat &inMat );
        static QPixmap cvMatToQPixmap( const cv::Mat &inMat );
        static cv::Mat QImageToCvMat( const QImage &inImage, bool inCloneImageData = true );
        static cv::Mat QPixmapToCvMat( const QPixmap &inPixmap, bool inCloneImageData = true );
    };

}
#endif
