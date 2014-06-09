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
#include <stdlib.h>
using namespace std;

void split(const string& src, const string& delim, vector<string>& dest)
{
    string str = src;
    string::size_type start = 0, index;
    string id_num, user_id, item_id, rating;
    
    index = str.find_first_of(delim, start);    //在str中查找(起始：start) delim的任意字符的第一次出现的位置
    id_num = str.substr(start, index-start);
    
    start = str.find_first_not_of(delim, index);
    index = str.find_first_of(delim, start);
    user_id = str.substr(start, index-start);
    
    start = str.find_first_not_of(delim, index);
    index = str.find_first_of(delim, start);
    item_id = str.substr(start, index-start);
    
    start = str.find_first_not_of(delim, index);
    index = str.find_first_of("\r", start);
    rating = str.substr(start, index-start);
    
    cout << atof(id_num.c_str()) << " " << atof(user_id.c_str()) << " " << atof(item_id.c_str()) << " " << atof(rating.c_str()) << " " << endl;
    
    return;
}


int main()
{
    ifstream infile("/Users/apple/Developer/Recommender/Recommender/train.txt", ios::in);
    vector<string> results;
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
                isfirstline = false;    // skip the first line
                continue;
            }
            split(textline, delim, results);
        }
    }
    infile.close();
    
    vector<string>::iterator iter = results.begin();
    while(iter != results.end())
    {
        cout<<*iter++<<endl;
    }
    
    return 0;
}