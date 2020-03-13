#ifndef REPORT_GENERATE_H
#define REPORT_GENERATE_H

#include "report_generate_global.h"

#include <QSizeF>
#include <QPixmap>

extern "C" {
    Q_DECL_EXPORT void generatePdf(QString directory);
    QSizeF get_image_size(QString picture_path);
};

#endif // REPORT_GENERATE_H
