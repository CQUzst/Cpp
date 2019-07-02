#include <iostream>
#include <fstream>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>
using namespace std;
int main(){
    std::fstream outFile;
    string str="D://myfile.txt";
    outFile.open(str,ios::out|ios::app);


    char numStr[5];
    string numS="";
    for(int i=0;i<18920;i++){//18920
        sprintf(numStr,"%05d",i);
        numS=numStr;
        string fileStr="D://teamWork//json//";
        fileStr+=numS;
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

        QJsonObject jsonObject = document.object();

        if (jsonObject.contains(QStringLiteral("object"))) {
            QJsonValue arrayValue = jsonObject.value(QStringLiteral("object"));
            if (arrayValue.isArray()) {
                QJsonArray array = arrayValue.toArray();
                for (int i = 0; i < array.size(); i++) {
                    QJsonValue nameArray = array.at(i);
                    QJsonObject subObject = nameArray.toObject();
                    QJsonValue subValue = subObject.value(QStringLiteral("rect"));
                    QJsonArray subArray = subValue.toArray();
                    for (int i = 0; i < subArray.size()-1; i++) {
                        //cout<< subArray.at(i).toInt()<<" ";
                        outFile<<subArray.at(i).toInt()<<",";
                    }
                    if(subArray.size()>0)
                        outFile<<subArray.at(subArray.size()-1).toInt();
                    outFile<<"    ";
                }
            }
        }
        outFile<<endl;
        cout<<"json "<<numS<<" is processing"<<endl;

    }
    outFile.close();
    cout<<"end~"<<endl;
    return 0;
}
