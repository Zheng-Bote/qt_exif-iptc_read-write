#include <QCoreApplication>
#include <QDirIterator>
#include <QImage>

#include <iostream>
#include <exiv2/exiv2.hpp>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString path = "/home/zb_bamboo/Dokumente/2021";
    QString replacePath = "/home/zb_bamboo/Dokumente/";

    /*
    // Dirs & Sub-Dirs
    QDirIterator it(path, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString dir = it.next();
        qDebug() << dir;
    }
*/

    Exiv2::XmpParser::initialize();
    ::atexit(Exiv2::XmpParser::terminate);
#ifdef EXV_ENABLE_BMFF
    Exiv2::enableBMFF();
#endif



    std::cout << "\n#####\nFiles\n" << std::endl;
    // pics
    QDirIterator picsFileIt(path, {"*.jpg", "*.jpeg", "*.png", "*.bmp"}, QDir::Files, QDirIterator::Subdirectories);
    while (picsFileIt.hasNext()) {
        QFile f(picsFileIt.next());

        // path/to/file
        std::cout << "# " << f.fileName().toUtf8().toStdString() << std::endl;

        // filename
        QFileInfo fileInfo(f.fileName());
        std::cout << "## " << fileInfo.completeBaseName().toStdString() << " " << f.size() / 1024 << "Kb" << std::endl;

        // file extension
        std::cout << "## " << fileInfo.completeSuffix().toStdString() << std::endl;

        // only dir
        QDir d = QFileInfo(f).absoluteDir();
        std::cout << "### " << d.absolutePath().toStdString() << std::endl;

        // cutted dir
        QString fPath = d.absolutePath();
        fPath.remove(replacePath);
        std::cout << "#### " << fPath.toStdString() << std::endl;

        // create sub dir
        if(d.mkdir("web") == 0){
            std::cout << "#### " << "= dir created" << std::endl;

            // copy file w/o extension
            QFile::copy(f.fileName(), d.absolutePath() + "/web/" + fileInfo.completeBaseName());

            QString fImg = d.absolutePath() + "/web/" + fileInfo.completeBaseName();

            QImage image(fImg);

            std::cout << "img size: " << image.width() << "x" << image.height() << std::endl;

            // OK:: convert img to webp
            image.save(fImg + ".webp", "WEBP");


            // exif iptc
            QString webpImg = fImg + ".webp";
            std::cout << "##### EXIF\n" << f.fileName().toUtf8().toStdString() << std::endl;
            //std::cout << "##### EXIF\n" << webpImg.toStdString() << std::endl;

            // OK:
            //auto exif_image = Exiv2::ImageFactory::open(webpImg.toStdString());
            auto exif_image = Exiv2::ImageFactory::open(f.fileName().toUtf8().toStdString());
            exif_image->readMetadata();

            Exiv2::IptcData& iptcData = exif_image->iptcData();

            // write Data
            auto wexif_image = Exiv2::ImageFactory::open(webpImg.toUtf8().toStdString());
            iptcData["Iptc.Application2.Headline"] = "The headline I am";
            wexif_image->setIptcData(iptcData);
            wexif_image->writeMetadata();

            if (iptcData.empty()) {
                std::string error("da faela");
                error += ": No IPTC data found in the file";
                //throw Exiv2::Error(Exiv2::ErrorCode::kerErrorMessage, error);
            }
            else {
                auto end = iptcData.end();
                for (auto md = iptcData.begin(); md != end; ++md) {
                    std::cout << std::setw(44) << std::setfill(' ') << std::left << md->key() << " "
                              << "0x" << std::setw(4) << std::setfill('0') << std::right << std::hex << md->tag() << " " << std::setw(9)
                              << std::setfill(' ') << std::left << md->typeName() << " " << std::dec << std::setw(3)
                              << std::setfill(' ') << std::right << md->count() << "  " << std::dec << md->value() << std::endl;
                }

            }

            Exiv2::ExifData& exifData = exif_image->exifData();

            // write Data
            auto wwexif_image = Exiv2::ImageFactory::open(webpImg.toUtf8().toStdString());
            exifData["Exif.Image.Copyright"] = "Robert Zheng";
            wwexif_image->setExifData(exifData);
            wwexif_image->writeMetadata();

            if (exifData.empty()) {
                std::string error("da faela");
                error += ": No EXIF data found in the file";
                //throw Exiv2::Error(Exiv2::ErrorCode::kerErrorMessage, error);
            }
            else {
                auto end = exifData.end();
                for (auto md = exifData.begin(); md != end; ++md) {
                    std::cout << std::setw(44) << std::setfill(' ') << std::left << md->key() << " "
                              << "0x" << std::setw(4) << std::setfill('0') << std::right << std::hex << md->tag() << " " << std::setw(9)
                              << std::setfill(' ') << std::left << md->typeName() << " " << std::dec << std::setw(3)
                              << std::setfill(' ') << std::right << md->count() << "  " << std::dec << md->value() << std::endl;
                }
            }


        }
    }

    //exit(0);

    exit(EXIT_SUCCESS);

}







