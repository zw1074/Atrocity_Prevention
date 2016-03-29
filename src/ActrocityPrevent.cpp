#include <fstream>
#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include <math.h>
using namespace std;

void split(vector<string> &tokens, const string &text, char sep) {
    tokens.clear();
    size_t start = 0, end = 0;
    while ((end = text.find(sep, start)) != string::npos) {
        tokens.push_back(text.substr(start, end - start));
        start = end + 1;
    }
    tokens.push_back(text.substr(start));
}

const int regions = 3466;
const int day_period = 7052;

class MassAtrocityPredictor2016
{
public:
    vector<vector<vector<float> > > Actiondata;
    vector<vector<float> > Actropydata;
    vector<int> countryregion;
    int recent_day_region[12] = {7,21,42,84,168,364,728,992,1456,2184,2912,10000};
    int recent_day_country[9] = {3,7,28,42,84,168,252,728,2912};
    float svm_param[36] = {-0.0098699999999979672, -0.074159999999995063, -0.092269999999994523, -0.075570000000004856, -0.012580000000005632, 0.077019999999932559, -0.10016000000006514, -0.21321000000014345, 0.10786999999991058, 0.056269999999906208, 0.24076999999979343, -0.14587000000013878, 0.008660000000000322, 0.0092200000000010451, -0.020229999999999519, -0.010850000000002559, 0.050429999999979942, 0.16141999999996515, 0.23244999999994168, 0.013170000000000015, -0.0090000000000051931, -0.024420000000004157, -0.0081299999999998544, -0.19358999999999557, 0.10162999999999282, -0.039689999999998109, -0.0011800000000000846, 0.068599999999994055, -0.10177999999999963, -0.05260999999999811, -0.077159999999998424, -0.025459999999998973, -0.000029999999999999306, -0.0034299999999998902, -0.0058899999999998095, -0.04801999999999846};

    MassAtrocityPredictor2016() : countryregion(regions), Actiondata(day_period) {}
    void load_action(vector<string>& data, int day_id) {
        vector<float> singleline;
        int i_ascii;

        for (int i = 0; i< data.size(); i++) {
            if (data[i] != "") {
                vector<string> x;
                split(x, data[i], ' ');
                for(int j=0; j<18; j++) {
                    switch(j){
                        case 0:
                        case 4:
                        case 5:
                        case 9:
                        case 14:
                            if(x[j].compare("_") == 0) singleline.push_back(0.0);
                            else singleline.push_back(stof(x[j]));
                            break;
                        case 10:
                            if(x[j].compare("_") == 0) singleline.push_back(0.0);
                            i_ascii = x[j][0] - 'a' + 1;
                            singleline.push_back((float)i_ascii);
                            break;
                        case 16:
                            singleline.push_back(stof(x[j]));
                            break;
                        default:
                            break;
                    }
                }
                Actiondata[day_id - 1].push_back(singleline);
                singleline.clear();
            }
        }
    }

    void load_actrocity(vector<string>& data) {
        vector<float> singleline;
        for (int i = 0; i< data.size(); i++) {
            vector<string> x;
            split(x, data[i], ',');
            for(int j = 0; j<7; j++)
            {
                switch(j) {
                    case 0:
                    case 2:
                    case 3:
                        if(x[j].compare("") == 0) singleline.push_back(0.0);
                        else singleline.push_back(stof(x[j]));
                        break;
                    default:
                        break;
                }
            }
            Actropydata.push_back(singleline);
            singleline.clear();
        }
    }

    void load_region(vector<string>& data) {
        int country_id = 1;
        for(int i = 0; i<data.size(); i++) {
            if (data[i] == "\r" || data[i] == "") {country_id++;}
            else {
                vector<string> x;
                split(x, data[i], ',');
                for (int j = 0; j< x.size(); j++) {
                    cout << x[j] << endl;
                    countryregion[stoi(x[j])-1] = country_id;
                }
                country_id++;
            }
        }
    }

    vector<float> get_Feature(int region_id, int day_id) {
        vector<float> features(36,0);
        int country_id = countryregion[region_id - 1];

        // Get column 1-12
        for (int j = 0; j<12; j++) {
            vector<int> day_used(day_id,0);
            float days = 0;
            for (int i = 0; i<Actropydata.size();i++) {
                if (Actropydata[i][0] < day_id&& Actropydata[i][0]>= (day_id - recent_day_region[j]) && day_used[(float) Actropydata[i][0] - 1] == 0) {
                    if (Actropydata[i][2] == region_id) {
                        days += 1;
                        day_used[(float) Actropydata[i][0] - 1] = 1;
                    }
                }
            }
            features[j] = days;
        }

        // Get column 13-21
        for (int j = 0; j<9; j++) {
            vector<int> day_used(day_id,0);
            float days = 0;
            for (int i =0; i<Actropydata.size();i++) {
                if (Actropydata[i][0] < day_id&& Actropydata[i][0]>= (day_id - recent_day_country[j]) && day_used[(int) Actropydata[i][0] - 1] == 0) {
                    if (Actropydata[i][2] == region_id || Actropydata[i][1] == country_id) {
                        days += 1;
                        day_used[(int) Actropydata[i][0] - 1] = 1;
                    }
                }
            }
            features[j + 12] = days;
        }

        // Get column 22
        float day_range = day_id;
        for (int i=0; i<Actropydata.size();i++) {
            if (Actropydata[i][0] < day_id && (day_id - Actropydata[i][0]) < day_range) {
                if (Actropydata[i][2] == region_id) {
                    day_range = day_id - Actropydata[i][0];
                }
            }
        }
        features[21] = day_range;

        // Get column 23
        day_range = day_id;
        for (int i=0; i<Actropydata.size();i++) {
            if (Actropydata[i][0] < day_id && (day_id - Actropydata[i][0]) < day_range) {
                if (Actropydata[i][2] == region_id || Actropydata[i][1] == country_id) {
                    day_range = day_id - Actropydata[i][0];
                }
            }
        }
        features[22] = day_range;

        // Get column 24-36
        for (int cur_day = day_id-63; cur_day<day_id; cur_day++) {
            for (int i = 0; i< Actiondata[cur_day].size(); i++) {
                if (Actiondata[cur_day][i][5] == region_id) {
                    features[23] += 1;
                    if ((Actiondata[cur_day][i][0] == 0 && Actiondata[cur_day][i][2] != 0 ) || (Actiondata[cur_day][i][0] != 0 && Actiondata[cur_day][i][2] == 0) ) {
                        features[24] += 1;
                        if (Actiondata[cur_day][i][1] == region_id || Actiondata[cur_day][i][3] == region_id) {
                            features[26] += 1;
                        }
                    }
                    else if (Actiondata[cur_day][i][0] != 0 && Actiondata[cur_day][i][2] != 0 ) {
                        features[25] += 1;
                        if (Actiondata[cur_day][i][1] == Actiondata[cur_day][i][3] && Actiondata[cur_day][i][1] == region_id) {
                            features[26] += 1;
                        }
                    }
                    if (Actiondata[cur_day][i][6]>15) {
                        features[27] += 1;
                    }
                    switch((int)Actiondata[cur_day][i][4]) {
                        case 2:
                            features[28] += 1;
                            break;
                        case 3:
                            features[29] += 1;
                            break;
                        case 4:
                            features[30] += 1;
                            break;
                        case 6:
                            features[31] += 1;
                            break;
                        case 8:
                            features[32] += 1;
                            break;
                        case 11:
                            features[33] += 1;
                            break;
                        case 14:
                            features[34] += 1;
                            break;
                        case 19:
                            features[35] += 1;
                            break;
                        default:
                            break;
                    }
                }
            }
        }
        return features;
    }

    double get_Prediction(vector<float> features) {
        double result = 0;
        for (int i = 0; i < 36; i++) {
            result += svm_param[i]*features[i];
        }
        return 1.0/(1.0 + exp(-result)) * 10.0;
    }


    int receiveData(int dataSourceId, int dayID, vector<string> data) {
        switch(dataSourceId) {
            case 0:
                load_actrocity(data);
                break;
            case 1:
                load_action(data, dayID);
                break;
            case 3:
                load_region(data);
                break;
            default:
                break;
        }
        return 0;
    }

    vector<double> predictAtrocities(int dayID) {
        vector<double> results(3466);
        for(int region_id = 1; region_id <= regions; region_id++ ) {
            results[region_id-1] = get_Prediction(get_Feature(region_id, dayID));
        }
        return results;
    }
};
