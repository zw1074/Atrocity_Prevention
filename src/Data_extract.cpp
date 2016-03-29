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
using namespace std;


float day_amount_region(int region_id, int day_id, vector<vector<float> > Actropydata, int day_X) {
    float days = 0.0;
    vector<int> day_used;
    for (int i =0; i<Actropydata.size();i++) {
        if (Actropydata[i][0] < day_id&& Actropydata[i][0]>= (day_id - day_X) && find(day_used.begin(),day_used.end(), (int) Actropydata[i][0]) == day_used.end()) {
            if (Actropydata[i][3] == region_id) {
                days += 1;
                day_used.push_back((int) Actropydata[i][0]);
            }
        }
    }
    return days;
}

float day_amount_country(int region_id, int day_id, vector<vector<float> > Actropydata, vector<int> countryregion, int day_X) {
    float days = 0.0;
    vector<int> day_used;
    int country_id = countryregion[region_id-1];
    for (int i =0; i<Actropydata.size();i++) {
        if (Actropydata[i][0] < day_id&& Actropydata[i][0]>= (day_id - day_X) && find(day_used.begin(),day_used.end(), (int) Actropydata[i][0]) == day_used.end()) {
            if (Actropydata[i][2] == country_id || Actropydata[i][3] == region_id) {
                days += 1;
                day_used.push_back((int) Actropydata[i][0]);
            }
        }
    }
    return days;
}

float day_range_region(int region_id, int day_id, vector<vector<float> > Actropydata) {
    vector<int> day_used;
    for (int i=0; i<Actropydata.size();i++) {
        if (Actropydata[i][0] < day_id && find(day_used.begin(),day_used.end(), (int) Actropydata[i][0]) == day_used.end()) {
            if (Actropydata[i][3] == region_id) {
                day_used.push_back((int) Actropydata[i][0]);
            }
        }
    }
    if (day_used.size()==0) {return (float) day_id;}
    else {return (float) (day_id - *max_element(day_used.begin(), day_used.end()));}
}

float day_range_country(int region_id, int day_id, vector<vector<float> > Actropydata, vector<int> countryregion) {
    vector<int> day_used;
    int country_id = countryregion[region_id - 1];
    for (int i=0; i<Actropydata.size();i++) {
        if (Actropydata[i][0] < day_id && find(day_used.begin(),day_used.end(), (int) Actropydata[i][0]) == day_used.end()) {
            if (Actropydata[i][2] == country_id || Actropydata[i][3] == region_id) {
                day_used.push_back((int) Actropydata[i][0]);
            }
        }
    }
    if (day_used.size()==0) {return (float) day_id;}
    else {return (float) (day_id - *max_element(day_used.begin(), day_used.end()));}
}

vector<float> social_event(int region_id, int day_id, vector<vector<vector<float> > > Actiondata) {
    vector<float> events(13,0);
    for (int cur_day = day_id-63; cur_day<day_id; cur_day++) {
        vector<vector<float> > this_day = Actiondata[cur_day];
        for (int i = 0; i< this_day.size(); i++) {
            if (this_day[i][14] == region_id) {
                events[0] += 1;
                if ((this_day[i][0] == 0.0 && this_day[i][5] != 0.0 ) || (this_day[i][0] != 0.0 && this_day[i][5] == 0.0) ) {
                    events[1] += 1;
                    if ((this_day[i][4] + this_day[i][9]) == region_id) {
                        events[3] += 1;
                    }
                }
                else if (this_day[i][0] != 0.0 && this_day[i][5] != 0.0 ) {
                    events[2] += 1;
                    if (this_day[i][4] == this_day[i][9] && this_day[i][4] == region_id) {
                        events[3] += 1;
                    }
                }
                if (this_day[i][16]>15) {
                    events[4] += 1;
                }
                switch((int)this_day[i][10]) {
                    case 2:
                        events[5] += 1;
                        break;
                    case 3:
                        events[6] += 1;
                        break;
                    case 4:
                        events[7] += 1;
                        break;
                    case 6:
                        events[8] += 1;
                        break;
                    case 8:
                        events[9] += 1;
                        break;
                    case 11:
                        events[10] += 1;
                        break;
                    case 14:
                        events[11] += 1;
                        break;
                    case 19:
                        events[12] += 1;
                        break;
                    default:
                        break;
                }
            }
        }
    }
    return events;
}

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
    vector<vector<float> > train_set(270*3466);
    vector<float> y_value(270*3466);
    vector<int> recent_day_region = {7,21,42,84,168,364,728,992,1456,2184,2912,10000};
    vector<int> recent_day_country = {3,7,28,42,84,168,252,728,2912};
    #pragma omp parallel for
    for (int i = 1;i<=270;i++) {
        int data_id = 5113 - i;
        for (int region_id = 1; region_id <= 3466; region_id ++) {
            vector<float> train_vec;
            int break_flag = 0;
            for (int j = 0; j<recent_day_region.size(); j++) {
                train_vec.push_back(day_amount_region(region_id, data_id, Actropydata, recent_day_region[j]));
            }
            for (int j = 0; j<recent_day_country.size(); j++) {
                train_vec.push_back(day_amount_country(region_id, data_id, Actropydata, countryregion, recent_day_country[j]));
            }
            train_vec.push_back(day_range_region(region_id, data_id, Actropydata));
            train_vec.push_back(day_range_country(region_id, data_id, Actropydata, countryregion));
            vector<float> social = social_event(region_id, data_id, Actiondata);
            train_vec.insert(train_vec.end(), social.begin(), social.end());
            train_set[i*3466+region_id-1] = train_vec;
            train_vec.clear();
            for (int j = 0; j< Actropydata.size();j++) {
                if (Actropydata[j][0]==(float)data_id && Actropydata[j][3] == (float) region_id) {y_value[i*3466 + region_id - 1] = 1.0;break_flag = 1;break;}
            }
            if (break_flag == 0) {y_value[i*3466 + region_id - 1] = -1.0;}
            cout<<"finish region"<<region_id<<endl;
        }
        cout<<"finish day "<<i<<endl;
    }

    // Output the train_set and y_value to file.
    filepath = "X_train.txt";
    ofstream myoutput(filepath);
    for (const auto& vt: train_set) {
        copy(vt.cbegin(), vt.cend(),
                ostream_iterator<float>(myoutput, " "));
        myoutput << '\n';
    }
    myoutput.close();
    filepath = "y_train.txt";
    ofstream myoutput2(filepath);
    for (int i=0; i<y_value.size(); i++) {
        myoutput2 << y_value[1] << '\n';
    }
    myoutput2.close();
    return 0;
}
