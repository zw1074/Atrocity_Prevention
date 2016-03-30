# Atrocity Prevention

Data: [`countryregion.txt`](https://drive.google.com/file/d/0Bxc6PUFWfKMzUWlEaU9Md0xnWFk/view), [`ActionData.zip`](https://drive.google.com/file/d/0Bxc6PUFWfKMzVXA1ZlJyUGVHUTA/view), [`groundtruth_example.txt`](https://drive.google.com/file/d/0Bxc6PUFWfKMzb29WMnM0VFVhZ0U/view)

Platform: C++, CUDA C++

Requirement: C++ version 4.8.1, [CUDA toolkit](https://developer.nvidia.com/cuda-toolkit)

## Abstract

This problem is one of the competition on [Topcoder.com](https://www.topcoder.com/my-dashboard/). You can find the detail [here](https://community.topcoder.com/longcontest/?module=ViewProblemStatement&rd=16682&pm=14189). The feature extraction method is mainly referred from [here](https://www.dropbox.com/sh/541rv30zem6pcqw/AABIAjaEgCkAKEmpG9mhzrwfa/place1/1-place.txt?dl=0). However, it does not give out the code, so I write my own version.

## CUDA

It is time-consuming to extract the feature, because on the social event part, we have to scan this huge thing again and again. So I did it by sevaral way. First, I just consturct the relevant part which means that when generate the database of social events, I would not store all of this. Second, I use the [thrust](http://docs.nvidia.com/cuda/thrust/#axzz44JNLJxh8) template library to create the X input in GPU memory. Thrust is very efficient for this task and most importantly, it is more high level compared to raw CUDA code. (e.g. You don't need to consider how to allocate the mission for each thread, thrust will automatically do this.) And also it has an object called `thrust::device_vector`. The usage of this object is similar to `std::vector`.

## How to run

This time, I am working on the `Makefile`. But if you want to test the code, you can just download the Data file above and run the code by typing (no cuda version)
```sh
$ cd src/; g++ -std=c++11 -fopenmp Data_extract.cpp -o Data.out; ./Data.out
```
or cuda version
```sh
$ cd src/; nvcc -std=c++11 Data_extract_cuda.cu -o Data.out; ./Data.out
```
If you don't have `nvcc`, you need to export the library path in your `.bashrc` file. Simply, you just need to add the following line in your `.bashrc` file and `source ~/.bashrc`.
```sh
export LD_LIBRARY_PATH=/usr/local/cuda/lib
export PATH=$PATH:/usr/local/cuda/bin
```

The `ActrocityPrevent.cpp` is the file that the competition requires. It builds up a class called `MassAtrocityPredictor2016`, and it has the following memeber:

```c++
class MassAtrocityPredictor2016 {
private:
    vector<vector<vector<float> > > Actiondata;
    vector<vector<float> > Actropydata;
    vector<int> countryregion;
    int recent_day_region[12];
    int recent_day_country[9];
    float svm_param[36];
    MassAtrocityPredictor2016() : countryregion(regions), Actiondata(day_period) {}
    void load_action(vector<string>& data, int day_id);
    void load_actrocity(vector<string>& data);
    void load_region(vector<string>& data);
    vector<float> get_Feature(int region_id, int day_id);
    double get_Prediction(vector<float> features);
public:
    int receiveData(int dataSourceId, int dayID, vector<string> data);
    vector<double> predictAtrocities(int dayID);
};
```

In fact, the optimal model for this problem is [random forest](https://en.wikipedia.org/wiki/Random_forest) (I test it on python by using the package [`sklearn`](http://scikit-learn.org/stable/index.html) and it has 96% accuracy!). But because of the time limit, I just made a pre-trained SVM model for this (You can find there is `svm_param[36]`). However I would try it out in the future.
