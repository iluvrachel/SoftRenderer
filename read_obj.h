/*
 * @Author: your name
 * @Date: 2020-03-03 15:43:15
 * @LastEditTime: 2020-03-06 23:13:43
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \SoftRenderer\read_obj.h
 */
#ifndef __OBJ_H__
#define __OBJ_H__

#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iostream>

class Obj {
    public: 
        int vn = 0;
        int fn = 0;
        // std::vector<float> vx_list;
        // std::vector<float> vy_list;
        std::vector<std::vector<float>> v_list;
        std::vector<std::vector<int>> f_list;

        Obj();
        void read_obj(std::string filename);

};

Obj::Obj(){
    
}


void Obj::read_obj(std::string file_name){
    std::ifstream in;
    in.open(file_name.c_str(), std::ios::in);
    std::string line;
    std::string str;
    float x,y,z;
    int ix,iy,iz;

    while(!in.eof())
	{
        getline(in,line);
        if(line.size() == 0) continue;
		if(line[0] == '#') 	 continue;
        else if(line[0] == 'v'){
			if(line[1] == 't'){
                    continue;
				}
			else if(line[1] == 'n'){
                    continue;
				}
			else{
                    std::istringstream IL(line);
					IL >> str >> x >> y >> z;
                    std::vector<float> cur_v;
                    cur_v.push_back(x);
                    cur_v.push_back(y);
                    cur_v.push_back(z);
                    //std::cout<<cur_v[0]<<std::endl;
                    v_list.push_back(cur_v);
				}
		}
		else if(line[0] == 'f'){
			std::istringstream IL(line);
			char c;
            IL >> str;
            // while(IL  >> ix >> c>> iy >> c >>iz){
            //     ix--;
            //     f_list.push_back(ix);
            //     std::cout<<ix<<std::endl;
            // }
            std::vector<int> cur_f;	
			for(int i = 0;i < 3;i++){
 
				IL  >> ix >> c>> iy >> c >>iz ;
                cur_f.push_back(ix-1);	
			}
            //std::cout<<cur_f[0]<<std::endl;
            f_list.push_back(cur_f);
             
        }    
    }
    vn = v_list.size();
    fn = f_list.size();
    in.close();
}

#endif //__OBJ_H__