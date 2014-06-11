//
//  main.cpp
//  Recommender
//
//  Created by Cho-Yeung Lam on 9/6/14.
//  Copyright (c) 2014 Cho-Yeung Lam. All rights reserved.
//

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <ctime>
#include <cstdlib>
#include <iomanip>
using namespace std;

#define NUM_USER 24983
#define NUM_MOVIE 101
#define NUM_CATEGORIES 150
#define NUM_ITERATION 2000

double ratings[NUM_MOVIE][NUM_USER] = {0};
double h[NUM_MOVIE][NUM_USER];
double x[NUM_MOVIE][NUM_CATEGORIES] = {1.0};      // x0 == 1, x1~x5
double theta[NUM_USER][NUM_CATEGORIES+1] = {1.0};  // theta0~theta5
double lamda;
double alpha;

#pragma mark - Read file
void split(const string& src, const string& delim, double target[][NUM_USER])
{
    if (src == "") return;
    
    string str = src;
    string::size_type start = 0, index;
    string id_num, user_id, item_id, rating;
    
    index = str.find_first_of(delim, start);
    id_num = str.substr(start, index-start);
    
    start = str.find_first_not_of(delim, index);
    index = str.find_first_of(delim, start);
    user_id = str.substr(start, index-start);
    int user = atoi(user_id.c_str());
    if (user >= NUM_USER) return; // exempt if exceed the range
    
    start = str.find_first_not_of(delim, index);
    index = str.find_first_of(delim, start);
    item_id = str.substr(start, index-start);
    int item = atoi(item_id.c_str());
    if (item >= NUM_MOVIE) return; // exempt if exceed the range
    
    start = str.find_first_not_of(delim, index);
    index = str.find_first_of("\r", start);
    rating = str.substr(start, index-start);
    
    target[item-1][user-1] = atof(rating.c_str());
}

void readfile(string path, double target[][NUM_USER])
{
    ifstream infile(path, ios::in);
    string word;
    string delim(" ");
    string textline;
    bool isfirstline = true;
    if(infile.good())
    {
        while(!infile.fail())
        {
            getline(infile, textline);
            if (isfirstline) {
                isfirstline = false;
                continue;
            }
            split(textline, delim, target);
        }
    }
    infile.close();
}

#pragma mark - Calculation

double calVariance()
{
    int i, j;
    double cnt = 0;
    double var = 0;
    for (i = 0; i < NUM_MOVIE; i++) {
        for (j = 0; j < NUM_USER; j++) {
            if (ratings[i][j] != 0) {
                cnt++;
                var += (h[i][j] - ratings[i][j]) * (h[i][j] - ratings[i][j]);
            }
        }
    }
    var = var / cnt;
    return var;
}

void refreshH()
{
    int i, j, k;
    for (i = 0; i < NUM_MOVIE; i++) {
        for (j = 0; j < NUM_USER; j++) {
            h[i][j] = theta[j][0];
            for (k = 0; k < NUM_CATEGORIES; k++)
                h[i][j] += theta[j][k+1]*x[i][k];
        }
    }
}

void calculate(int times)
{
    int i, j, k;
    time_t start_t, end_t;
    double zeroVar = calVariance();
    cout << "[zeroVar] " << zeroVar << endl;
    
    start_t = time(NULL);
    
    // 2. calculate theta
    for (j = 0; j < NUM_USER; j++) {
        for (k = 0; k < NUM_CATEGORIES+1; k++) {
            int sum = 0;
            for (i = 0; i < NUM_MOVIE; i++) {
                if (ratings[i][j] != 0)
                    sum += (h[i][j] - ratings[i][j]) * x[i][k];
            }
            theta[j][k] = theta[j][k] - alpha * (sum + lamda * theta[j][k]);
        }
    }
    refreshH();
    double firstVar = calVariance();
    
    // 1.calculate x
    for (i = 0; i < NUM_MOVIE; i++) {
        for (k = 0; k < NUM_CATEGORIES; k++) {
            int sum = 0;
            for (j = 0; j < NUM_USER; j++) {
                if (ratings[i][j] != 0)
                    sum += (h[i][j] - ratings[i][j]) * theta[j][k];
            }
            x[i][k] = x[i][k] - alpha * (sum + lamda * x[i][k]);
        }
        refreshH();
    }
    
    
    end_t = time(NULL);
    
    
    double secondVar = calVariance();
    cout << "[Time used] " << difftime(end_t, start_t) << endl;
    cout << "[firstVar] " << firstVar << endl;
    cout << "[secondVar] " << secondVar << endl;
    cout << "[firstVar - secondVar] " << firstVar - secondVar << endl;
}

#pragma mark - Testing

void writeforeachrow(ofstream &outfile, const string& src, const string& delim)
{
    if (src == "") return;
    string str = src;
    string::size_type start = 0, index;
    string id_num, user_id, item_id, rating;
    
    index = str.find_first_of(delim, start);
    id_num = str.substr(start, index-start);
    outfile << id_num << ",";
    
    start = str.find_first_not_of(delim, index);
    index = str.find_first_of(delim, start);
    user_id = str.substr(start, index-start);
    outfile << user_id << ",";
    int user = atoi(user_id.c_str());
    
    start = str.find_first_not_of(delim, index);
    index = str.find_first_of("r", start);
    item_id = str.substr(start, index-start);
    outfile << item_id << ",";
    int item = atoi(item_id.c_str());
    
    double result = theta[item-1][0];
    for (int k = 0; k < NUM_CATEGORIES; k++)
        result += theta[item-1][k+1]*x[user-1][k];
    
    stringstream convert;
    convert << result;
    outfile << convert.str();
}


void testandwrite(string readpath, string writepath)
{
    ifstream infile(readpath, ios::in);
    ofstream outfile(writepath, ios::out);
    string word;
    string delim(" ");
    string textline;
    bool isfirstline = true;
    if(infile.good())
    {
        while(!infile.fail())
        {
            getline(infile, textline);
            if (isfirstline) {
                isfirstline = false;
                outfile << "id,user_id,item_id,rating\r";
                continue;
            }
            writeforeachrow(outfile, textline, delim);
        }
    }
    infile.close();
}

int main()
{
    srand(time(NULL));
    for (int i = 0; i < NUM_MOVIE; i++)
        for (int k = 0; k < NUM_CATEGORIES; k++) {
            x[i][k] = (double)(rand() % 10) / 100.0f;
        }
    
    for (int j = 0; j < NUM_USER; j++)
        for (int k = 0; k <= NUM_CATEGORIES+1; k++) {
            theta[j][k] = (double)(rand() % 10) / 100.0f;
        }
    
    refreshH();
    lamda = 100;
    alpha = 0.0001;
    readfile("/Users/apple/Developer/Recommender/Recommender/train.txt", ratings);
    cout << "adsf" << endl;
    for (int cnt = 1; cnt <= NUM_ITERATION; cnt++) {
        cout << "TIMES - " << cnt << endl;
        calculate(cnt);
    }
    testandwrite("/Users/apple/Developer/Recommender/Recommender/test.txt", "/Users/apple/Developer/Recommender/Recommender/result.csv");
    
    
    return 0;
}

