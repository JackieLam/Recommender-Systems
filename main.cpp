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
using namespace std;

#define NUM_USER 24983
#define NUM_MOVIE 101
#define NUM_CATEGORIES 5
#define NUM_ITERATION 10000

double ratings[NUM_MOVIE][NUM_USER] = {0};
double x[NUM_MOVIE][NUM_CATEGORIES] = {1.0};      // x0 == 1, x1~x5
double theta[NUM_USER][NUM_CATEGORIES+1] = {1.0};  // theta0~theta5
double lummda;
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

double theta_multi_x(int j, int i)
{
    double result = theta[j][0];
    for (int cnt = 0; cnt < NUM_CATEGORIES; cnt++)
        result += theta[j][cnt+1]*x[i][cnt];
    
    return result;
}

double Jtheta(int k, double itself, bool isX)
{
    int i, j;
    double result = 0.0f;
    double increment;
    for (j = 0; j < NUM_USER; j++) {
        for (i = 0; i < NUM_MOVIE; i++) {
            // judge whether already rated
            if (ratings[i][j] != 0) {
                if (isX)
                    increment = (theta_multi_x(j, i) - ratings[i][j]) * theta[j][k];
                else
                    increment = (theta_multi_x(j, i) - ratings[i][j]) * x[i][k];
                result += increment;
            }
        }
    }
    result += lummda * itself;
    return result;
}

void calculate()
{
    int i, j, k;
    for (j = 0; j < NUM_USER; j++) {
        for (k = 0; k < NUM_CATEGORIES; k++) {
            // theta.k(j)
            double tmp = alpha * Jtheta(k, theta[j][k], false);
            cout << "theta - " << tmp << endl;
            theta[j][k] = theta[j][k] - tmp;
        }
        for (i = 0; i < NUM_MOVIE; i++) {
            for (k = 0; k <= NUM_CATEGORIES; k++) {
                // x.k(i)
                double tmp = alpha * Jtheta(k, x[i][k], true);
                cout << "x - " << tmp << endl;
                x[i][k] = x[i][k] - tmp;
            }
        }
    }
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
    
    double result;
    result = theta_multi_x(item-1, user-1);
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
    for (int i = 0; i < NUM_MOVIE; i++)
        for (int k = 0; k < NUM_CATEGORIES; k++)
            x[i][k] = 0.001;
    
    for (int j = 0; j < NUM_USER; j++)
        for (int k = 0; k <= NUM_CATEGORIES+1; k++)
            theta[j][k] = 0.001;
    lummda = 0.02;
    alpha = 0.00001;
    readfile("/Users/apple/Developer/Recommender/Recommender/train.txt", ratings);
    //    for (int cnt = 0; cnt < NUM_ITERATION; cnt++) {
    //        cout << "Times - " << cnt << endl;
    calculate();
    //    }
    
    testandwrite("/Users/apple/Developer/Recommender/Recommender/test.txt", "/Users/apple/Developer/Recommender/Recommender/result.csv");
    
    
    return 0;
}