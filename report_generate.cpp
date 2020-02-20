#include "report_generate.h"
#include <QString>
#include <QJsonDocument>
#include <QFile>
#include <QJsonObject>
#include <QTextDocument>
#include <QtPrintSupport/QPrinter>
#include <QVector>
#include <QMessageBox>

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
    foreach(QString str,root.keys())
    {
        html+="<div align=left>""<b>""Название прецедента:</b> "+str+"</div>";
        QJsonValue jsvalue = root.value(str);
        QStringList keys=jsvalue.toObject().keys();
        if (!keys.empty())
        {
            html+="<div align=left>""<b>""Действуюущее лицо:</b> "+jsvalue.toObject().value("active_face").toString()+"</div>";
            html+="<div align=left>""<b>""Цель:</b> "+jsvalue.toObject().value("goal").toString()+"</div>";
            html+="<div align=left>""<b>""Предусловие:</b> "+jsvalue.toObject().value("uslovie").toString()+"</div>";
            html+="<div align=left>""<img src="+directory+"/robustness/"+str+".png alt='отсутствует диаграмма для "+str+
                    "' width=650 height=400 > ""</div>";
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
                html+="</ol>""</div>""<img src='"+directory+"/sequence/"+str+name+".png' alt='отсутствует диаграмма для "+str+name+
                        "' width=650 height=400>""</div>";
            }
        }
    }
    json.close();
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
