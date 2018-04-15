#include "imageprocessing.h"

ImageProcessing::ImageProcessing() :
    faceWidth(FACE_WIDTH),
    faceHeight(FACE_HEIGHT),
    lbpcascade_frontalface(LBP_FF),
    newUserName(NEWUSER)
{

}

cv::Mat ImageProcessing::QImage2cvMat(QImage image)
{
    cv::Mat mat;
    qDebug() << image.format();
    switch(image.format())
    {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32_Premultiplied:
        mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
        break;
    case QImage::Format_RGB888:
        mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
        cv::cvtColor(mat, mat, CV_BGR2RGB);
        break;
    case QImage::Format_Indexed8:
        mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
        break;
    default:
        break;
    }
    return mat;
}

// 图片切割和保存；
// 传入：图片、用户名、图片名id、保存路径的引用；
// 返回：检测到几个人脸；
int ImageProcessing::ImageCutAndSave(QImage qimg, QString userName, int id, QString &imgPath)
{
    std::vector<cv::Rect> faces;
    Mat img_gray;
    Mat img(QImage2cvMat(qimg));

    // 灰度化
    cv::cvtColor(img, img_gray, COLOR_BGR2GRAY);
    // 均衡化
    cv::equalizeHist(img_gray, img_gray);

    // 分类器加载路径
    QDir rootDir = programRootDir();
    QDir classifierDir(rootDir);
    classifierDir.cd(SRC);

    // 人脸图片保存路径
    QDir faceDir(rootDir);
    faceDir.cd(DATA);

    if(!userName.isEmpty())
        newUserName = userName;

    // 创建并进入userName文件夹
    if(!faceDir.exists(newUserName))
        faceDir.mkdir(newUserName);
    faceDir.cd(newUserName);

    // 加载分类器
    CascadeClassifier face_cascade;
    face_cascade.load(classifierDir.absolutePath().toStdString() + "/" + lbpcascade_frontalface);

    // 检测人脸
    face_cascade.detectMultiScale(img_gray, faces, 1.1, 3, CV_HAAR_DO_ROUGH_SEARCH, Size(100, 100));

    if(faces.size() == 1)
    {
        Mat faceROI = img(faces[0]);
        Mat userFace;
        if (faceROI.cols > 100)
        {
            // 切割，重置大小
            resize(faceROI, userFace, Size(faceWidth, faceHeight), 0, 0);
            std::string str(faceDir.absolutePath().toStdString() + "/" + std::to_string(id) + "." + IMG_FORMAT);
            imwrite(str, userFace);
            imgPath = QString::fromStdString(str);
        }
    }

    return faces.size();
}


