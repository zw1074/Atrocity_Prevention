/*
 * Extract data as vector object.
 * Actiondata: a number of matrix. Each of it is like the file.
 * Actropydata: 暴行matrix.
 * Maybe, I need to make a hashtable for it?
 * NaN is converted to 0.
 */
#include <fstream>
#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include <thrust/host_vector.h>
#include <thrust/device_vector.h>
using namespace std;

vector<string> &split(const string &s, char delim, vector<string> &elems) {
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


vector<string> split(const string &s, char delim) {
    vector<string> elems;
    split(s, delim, elems);
    return elems;
}

int main(){
    vector<vector<vector<float> > > Actiondata;
    vector<vector<float> > Actropydata;
    vector<vector<float> > date;
    vector<float> singleline;
    vector<int> countryregion(3466);
    vector<string> x;
    vector<string> y;
    string filepath;
    string line;
    ifstream myfile;
    int i_ascii;
    // Read data (Action data)
    cout<< "==> Reading action data."<< endl;
    for(int i = 0; i<5114;i++)
    {
        filepath = ("ActionsData/data" + to_string(i+10000) +".txt");
        myfile.open(filepath);
        if (myfile.is_open())
        {
            while (getline (myfile, line))
            {
                vector<string> x = split(line, ' ');
                for(int j=0; j<18; j++)
                {
                    switch(j){
                        case 0:
                        case 1:
                        case 3:
                        case 4:
                        case 5:
                        case 6:
                        case 8:
                        case 9:
                        case 11:
                        case 13:
                        case 14:
                            if(x[j].compare("_") == 0) singleline.push_back(0.0);
                            else singleline.push_back(stof(x[j]));
                            break;
                        case 2:
                        case 7:
                        case 12:
                            if(x[j].compare("_") == 0) {singleline.push_back(500.0);singleline.push_back(500.0);}
                            else {
                                vector<string> y = split(x[j], ',');
                                singleline.push_back(stof(y[0]));
                                singleline.push_back(stof(y[1]));
                            }
                            break;
                        case 10:
                            if(x[j].compare("_") == 0) singleline.push_back(0.0);
                            i_ascii = x[j][0] - 'a' + 1;
                            singleline.push_back((float)i_ascii);
                            break;
                        case 15:
                            if(x[j].compare("_") == 0) singleline.push_back(0.0);
                            else if (x[j].compare("t") == 0) singleline.push_back(1.0);
                            else singleline.push_back(-1.0);
                            break;
                        case 16:
                        case 17:
                            singleline.push_back(stof(x[j]));
                            break;
                    }
                }
                date.push_back(singleline);
                singleline.clear();
            }
        }
        Actiondata.push_back(date);
        date.clear();
        myfile.close();
    }
    //Extract data (暴行)
    cout<< "==> Reading Actrocity data."<<endl;
    filepath = "groundtruth_example.txt";
    myfile.open(filepath);
    if (myfile.is_open())
    {
        while (getline (myfile, line))
        {
            vector<string> x = split(line, ',');
            for(int j = 0; j<7; j++)
            {
                if(x[j].compare("") == 0) singleline.push_back(0.0);
                else singleline.push_back(stof(x[j]));
            }
            Actropydata.push_back(singleline);
            singleline.clear();
        }
    }
    myfile.close();
    // Extract country-region data.
    cout<< "==> Extracting country-region data."<<endl;
    filepath = "countryregions.txt";
    myfile.open(filepath);
    int country_id = 1;
    if (myfile.is_open()) {
        while (getline (myfile, line)) {
            if (line == "\r") {country_id++;}
            else {
                vector<string> x = split(line, ',');
                for (int j = 0; j< x.size(); j++) {
                    countryregion[stoi(x[j])-1] = country_id;
                }
                country_id++;
            }
        }
    }
    myfile.close();

    // Generating training set and validating set with the portion of 0.8.
    cout<< "==> generating training set and validation set."<<endl;

    // Generate a permutation index.
    vector<int> myvector(270*3466);
    for (int i=0;i<3466*270;i++) {myvector[i] = i;}
    random_shuffle ( myvector.begin(), myvector.end() );
    thrust::device_vector<int> d_myvector;
    int N = 100000;

    vector<int> myvector2;
    for (int i = 0; i<Actropydata.size();i++) {
        if (Actropydata[i][0] * Actropydata[i][3] != 0) {
            myvector2.push_back((int) (Actropydata[i][3] - 1 + ((5113-Actropydata[i][0]) - 1)*3466));
        }
    }
    int j = 1;
    int size = myvector2.size();
    for (int i =0; i< myvector.size(); i++) {
        if (find(myvector2.begin(),myvector2.end(), myvector[i]) == myvector2.end()) {
            myvector2.push_back(myvector[i]);
            j++;
        }
        if (j > N - size) break;
    }
    random_shuffle ( myvector2.begin(), myvector2.end() );
    d_myvector = myvector2;
    cout<<d_myvector.size()<<endl;

    thrust::device_vector<float> train_set(N*36,0);
    thrust::device_vector<float> y_value(N,-1.0);
    int recent_day_region[12] = {7,21,42,84,168,364,728,992,1456,2184,2912,10000};
    int recent_day_country[9] = {3,7,28,42,84,168,252,728,2912};
    // thrust::device_vector<int> recent_day_region = {7,21,42,84,168,364,728,992,1456,2184,2912,10000};
    // thrust::device_vector<int> recent_day_country = {3,7,28,42,84,168,252,728,2912};
    for (int index = 0; index<N; index++) {
        int k = d_myvector[index]/3466 + 1;
        int region_id = d_myvector[index]%3466 + 1;
        int day_id = 5113 - k;
        int country_id = countryregion[region_id - 1];

        // Get column 1-12
        for (int j = 0; j<12; j++) {
            thrust::device_vector<int> day_used(5114,0);
            float days = 0;
            for (int i = 0; i<Actropydata.size();i++) {
                if (Actropydata[i][0] < day_id&& Actropydata[i][0]>= (day_id - recent_day_region[j]) && day_used[(float) Actropydata[i][0] - 1] == 0) {
                    if (Actropydata[i][3] == region_id) {
                        days += 1;
                        day_used[(float) Actropydata[i][0] - 1] = 1;
                    }
                }
            }
            train_set[(index)*36 + j] = days;
        }

        // Get column 13-21
        for (int j = 0; j<9; j++) {
            thrust::device_vector<int> day_used(5114,0);
            float days = 0;
            for (int i =0; i<Actropydata.size();i++) {
                if (Actropydata[i][0] < day_id&& Actropydata[i][0]>= (day_id - recent_day_country[j]) && day_used[(int) Actropydata[i][0] - 1] == 0) {
                    if (Actropydata[i][3] == region_id || Actropydata[i][2] == country_id) {
                        days += 1;
                        day_used[(int) Actropydata[i][0] - 1] = 1;
                    }
                }
            }
            train_set[(index)*36 + j + 12] = days;
        }

        // Get column 22
        float day_range = day_id;
        for (int i=0; i<Actropydata.size();i++) {
            if (Actropydata[i][0] < day_id && (day_id - Actropydata[i][0]) < day_range) {
                if (Actropydata[i][3] == region_id) {
                    day_range = day_id - Actropydata[i][0];
                }
            }
        }
        train_set[(index)*36 + 21] = day_range;

        // Get column 23
        day_range = day_id;
        for (int i=0; i<Actropydata.size();i++) {
            if (Actropydata[i][0] < day_id && (day_id - Actropydata[i][0]) < day_range) {
                if (Actropydata[i][3] == region_id || Actropydata[i][3] == country_id) {
                    day_range = day_id - Actropydata[i][0];
                }
            }
        }
        train_set[(index)*36 + 22] = day_range;

        // Get column 24-36
        for (int cur_day = day_id-63; cur_day<day_id; cur_day++) {
            for (int i = 0; i< Actiondata[cur_day].size(); i++) {
                if (Actiondata[cur_day][i][14] == region_id) {
                    train_set[(index)*36 + 23] += 1;
                    if ((Actiondata[cur_day][i][0] == 0 && Actiondata[cur_day][i][5] != 0 ) || (Actiondata[cur_day][i][0] != 0 && Actiondata[cur_day][i][5] == 0) ) {
                        train_set[(index)*36 + 24] += 1;
                        if (Actiondata[cur_day][i][4] == region_id || Actiondata[cur_day][i][9] == region_id) {
                            train_set[(index)*36 + 26] += 1;
                        }
                    }
                    else if (Actiondata[cur_day][i][0] != 0 && Actiondata[cur_day][i][5] != 0 ) {
                        train_set[(index)*36 + 25] += 1;
                        if (Actiondata[cur_day][i][4] == Actiondata[cur_day][i][9] && Actiondata[cur_day][i][4] == region_id) {
                            train_set[(index)*36 + 26] += 1;
                        }
                    }
                    if (Actiondata[cur_day][i][16]>15) {
                        train_set[(index)*36 + 27] += 1;
                    }
                    switch((int)Actiondata[cur_day][i][10]) {
                        case 2:
                            train_set[(index)*36 + 28] += 1;
                            break;
                        case 3:
                            train_set[(index)*36 + 29] += 1;
                            break;
                        case 4:
                            train_set[(index)*36 + 30] += 1;
                            break;
                        case 6:
                            train_set[(index)*36 + 31] += 1;
                            break;
                        case 8:
                            train_set[(index)*36 + 32] += 1;
                            break;
                        case 11:
                            train_set[(index)*36 + 33] += 1;
                            break;
                        case 14:
                            train_set[(index)*36 + 34] += 1;
                            break;
                        case 19:
                            train_set[(index)*36 + 35] += 1;
                            break;
                        default:
                            break;
                    }
                }
            }
        }
        for (int j = 0; j< Actropydata.size();j++) {
            if (Actropydata[j][0]==(float)day_id && Actropydata[j][3] == (float) region_id) {y_value[index] = 1.0;break;}
        }
        cout<<"fininsh region: "<<region_id<<endl;
    }
    cout<<"finish"<<endl;
    // Output the train_set and y_value to file.
    filepath = "X_train.txt";
    ofstream myoutput1(filepath);
    // for (int k = 1; k<= 270; k++) {
    //     for (int region_id = 1; region_id <= 3466; region_id ++) {
    //         for (int i = 0; i<36; i++) {
    //             if (i != 35){
    //                 myoutput1 << train_set[(index)*36 + i] << ',';
    //             }
    //             else {
    //                 myoutput1 << train_set[((k-1)*3466 + region_id-1)*36 + i] << '\n';
    //             }
    //         }
    //     }
    // }
    for (int index = 0; index<N; index++){
        for (int i = 0; i<36; i++){
            if (i != 35){
                myoutput1 << train_set[index*36 + i] << ',';
            }
            else {
                myoutput1 << train_set[index*36 + i] << '\n';
            }
        }
    }
    filepath = "y_train.txt";
    ofstream myoutput2(filepath);
    for (int i=0; i<y_value.size(); i++) {
        myoutput2 << y_value[i] << '\n';
    }
    myoutput2.close();
    return 0;
}
