#include <iostream>
#include <fstream>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include <QDebug>
using namespace std;
int main(){
    std::fstream outFile;
    string str="D://worldCoordinate.txt";
    outFile.open(str,ios::out|ios::app);


    for(int i=1;i<1497;i++){//1497

        string fileStr="D://data//save_online_data//online_data";
        fileStr+=to_string(i);
        fileStr+=".json";
        QString qs=QString::fromStdString(fileStr);
        //cout<<fileStr<<endl;
        QFile file(qs);
        file.open(QIODevice::ReadOnly|QIODevice::Text);
        QString value = file.readAll();
        file.close();

        QJsonParseError parseJsonErr;
        QJsonDocument document = QJsonDocument::fromJson(value.toUtf8(), &parseJsonErr);
        if (!(parseJsonErr.error == QJsonParseError::NoError)) {
            cout<<"read error"<<endl;
            return 0;
        }

        QJsonObject rootObj = document.object();
        if(rootObj.contains(QStringLiteral("track")))
        {
            QJsonValue arrayValue = rootObj.value(QStringLiteral("track"));
            if(arrayValue.isArray())
            {
                QJsonArray array = arrayValue.toArray();
                for(int i=0;i<array.size();i++)
                {
                    QJsonValue iconArray = array.at(i);
                    QJsonObject subObj = iconArray.toObject();
                    int id = subObj["trackID"].toInt();
                    QString plateNum = subObj["plateNumber"].toString();
                    string s=plateNum.toStdString();
                    double Nstring = subObj["N"].toDouble();
                    QString NStr=QString("%1").arg(Nstring,0,'G',13); //保留四位有效数字，会进行四舍五入
                    double Estring = subObj["E"].toDouble();
                    QString EStr=QString("%1").arg(Estring,0,'G',13); //保留四位有效数字，会进行四舍五入
                    string s1=NStr.toStdString();
                    string s2=EStr.toStdString();
                    if(s.find("BP319U")!=string::npos){
                        qDebug()<<"id="<<id<<"\tplateNum="<<plateNum<<"\tNStr="<<NStr<<"\tEStr="<<EStr<<endl;
                        outFile<<"id="<<id<<"\tplateNum="<<s<<"\tNStr="<<s1<<"\tEStr="<<s2<<endl;
                    }
                }
            }
        }
    }

    outFile.close();
    cout<<"end~"<<endl;
    return 0;
}
