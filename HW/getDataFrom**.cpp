
    string fileStr="/home/zst_data_to_mcf/posture_obj_";
    string objectID_str=to_string(objectID);
    fileStr+=objectID_str;
    fileStr+=".txt";

    cout<<fileStr<<endl;
    //cout<<"worldCoorObjectTrackers.size()="<<worldCoorObjectTrackers.size()<<endl;
    string positionStr;
    vector <string> positionStrVec;
    string NEStr;
    vector <string> NEStrVec;

    for (auto iter:positionResults) {
        positionStr="frame="+to_string(iter.first)
                +",origin_x="+to_string(iter.second.x)+",origin_y="+to_string(iter.second.y)
                +",origin_width="+to_string(iter.second.width)+",origin_height="+to_string(iter.second.height);
        //cout << positionStr << endl;
        positionStrVec.push_back(positionStr);
    }

    for (auto iter:worldCoorObjectTrackers) {
        NEStr="frame="+to_string(iter.first)+",N="+to_string(iter.second.at(LeftDown).N)
            +",E="+to_string(iter.second.at(LeftDown).E);
        //cout << NEStr << endl;
        NEStrVec.push_back(NEStr);
    }


    std::fstream outFile;
    outFile.open(fileStr,ios::out|ios::app);
    outFile<<"Direction="<<cameraDirection;
    outFile<<",Image.width="<<imageWidth;
    outFile<<",Image.height="<<imageHeight<<endl;
    for(int i=0;i<positionStrVec.size();i++)
        outFile<<positionStrVec[i]<<endl;
    for(int i=0;i<NEStrVec.size();i++)
        outFile<<NEStrVec[i]<<endl;
    outFile<<"angle="<<ret<<endl;

    outFile.close();


#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;
bool fileExist(const string& name)
{
    ifstream ifs(name);
    if(ifs.is_open()) {
        ifs.close();
        return true;
    }
    else
        return false;
}
int main(){
    for(int i=0;i<40;i++){
        string inFileStr="/home/zst_data_to_mcf/posture_obj_";
        string objectID_str=to_string(i);
        inFileStr+=objectID_str;
        inFileStr+=".txt";

        fstream inFile;
        if(!fileExist(inFileStr))
            continue;

        string outFileStr="/home/zst_data_to_mcf_version1/posture_obj_";
        outFileStr+=to_string(i);
        outFileStr+=".txt";
        fstream outFile;
        outFile.open(outFileStr,ios::out);
        //cout<<"filestr="<<fileStr<<endl;
        inFile.open(inFileStr);
        string message;
        vector<string> vec;
        while(!inFile.eof()){
            getline(inFile, message, '\n');
            if(message[0]=='f')
            {
                long n=count(vec.begin(),vec.end(),message);
                if(n>0)//exits frame and points
                    continue;
                vec.push_back(message);
                outFile<<message<<endl;
                continue;
            }
            outFile<<message<<endl;
        }
        inFile.close();
        outFile.close();
    }
    cout<<"end"<<endl;
    return 0;
}
