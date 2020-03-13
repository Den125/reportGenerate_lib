#include "report_generate.h"
#include <QString>
#include <QJsonDocument>
#include <QFile>
#include <QJsonObject>
#include <QTextDocument>
#include <QtPrintSupport/QPrinter>
#include <QVector>
#include <QMessageBox>
#include <QDebug>

void generatePdf(QString directory)
{
    QString html;
    QFile json(directory+"//description.json");
    if (!(json.open(QIODevice::ReadOnly)))
         return;
    QJsonDocument doc = QJsonDocument::fromJson(json.readAll());
    if (doc.isNull())
    {
        QMessageBox error(QMessageBox::Critical,"Ошибка открытия",
                          "Ошибка открытия JSON!!!");
        error.exec();
        return;
    }
    QJsonObject root = doc.object();
    html+="<h1 align=center>"+directory.split('/').last()+"</h1>";
    foreach(QString str,root.keys())
    {
        html+="<div align=left>""<b>""Название прецедента:</b> "+str+"</div>";
        QJsonValue jsvalue = root.value(str);
        QStringList keys=jsvalue.toObject().keys();
        if (!keys.empty())
        {
            QSizeF picture_size;
            html+="<div align=left>""<b>""Действуюущее лицо:</b> "+jsvalue.toObject().value("active_face").toString()+"</div>";
            html+="<div align=left>""<b>""Цель:</b> "+jsvalue.toObject().value("goal").toString()+"</div>";
            html+="<div align=left>""<b>""Предусловие:</b> "+jsvalue.toObject().value("uslovie").toString()+"</div>";
            picture_size=get_image_size(directory+"/robustness/"+str+".png");
            html+="<div align=left>""<img src="+directory+"/robustness/"+str+".png alt='отсутствует диаграмма для "+str+
                    "'width="+QString::number(picture_size.width())+
                    " height="+QString::number(picture_size.height())+">""</div>";
            int index_seq=0;
            for (;index_seq<keys.count()-3;index_seq++)
            {
                QString name;
                QJsonValue seq_list=jsvalue.toObject().value("list_seq"+QString::number(index_seq));
                if (seq_list.toObject().value("type_seq").toString()=="main")
                {
                    html+="<div align=left>""<b>""Главная последовательность:</b> ";
                    name="_main";
                }
                else if (seq_list.toObject().value("type_seq").toString()=="alt")
                {
                    html+="<div align=left>""<b>""Альтернативная последовательность</b> ( "+seq_list.toObject().value("usl_seq").toString()+"):";
                    name="_alt"+QString::number(index_seq);
                }
                html+="<div align=left>""<ol>";
                int index_elem=0;
                QJsonValue list_seq=seq_list.toObject().value("seq");
                if (!(list_seq.toObject().keys().empty()))
                {
                    for (;index_elem<list_seq.toObject().keys().count();index_elem++)
                    {
                        QJsonValue elem_seq=list_seq.toObject().value(QString::number(index_elem));
                        html+="<li>"+elem_seq.toString()+"</li>";
                    }
                }
                picture_size=get_image_size(directory+"/sequence/"+str+name+".png");
                html+="</ol>""</div>""<br>""<img src='"+directory+"/sequence/"+str+name+".png' alt='отсутствует диаграмма для "+str+name+
                        "' width="+QString::number(picture_size.width())+
                        " height="+QString::number(picture_size.height())+">""</div>";
            }
        }
    }
    json.close();
    html+="<h2 align=center>Диаграмма классов</h2>";
    html+="<div align=left><img src='"+directory+"/classes/classes.png'></div>";
    QTextDocument document;
    document.setHtml(html);
    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPaperSize(QPrinter::A4);
    printer.setOutputFileName(directory+"/"+directory.split('/').last()+".pdf");
    printer.setPageMargins(QMarginsF(15, 15, 15, 15));
    document.print(&printer);
    QMessageBox complete(QMessageBox::Information,"Успешно",
                      "Файл успешно создан. Вы можете найти его в директории своего проекта");
    complete.exec();
}

QSizeF get_image_size(QString picture_path)
{
    QPixmap picture (picture_path);
    QSizeF size(picture.width(),picture.height());\
    if (size.width()/297==1.0 && size.height()/420==1.0)
    {
    }
    else if (size.width()/297<1 || size.height()/420<1)
    {
        size.setWidth(size.width() * std::min(size.width()/297,size.height()/420));
        size.setHeight(size.height() * std::min(size.width()/297,size.height()/420));
    }
    else if (size.width()/297>1 || size.height()/420>1)
    {
        size.setWidth(size.width() / std::min(size.width()/297,size.height()/420));
        size.setHeight(size.height() / std::min(size.width()/297,size.height()/420));
    }
    return size;
}
