/*
 * @Author: your name
 * @Date: 2020-03-03 15:43:15
 * @LastEditTime: 2020-04-05 21:14:49
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
#include "vec.h"
#include "tgaimage.h"

class Obj {
    public: 
        int vn = 0;
        int fn = 0;
        // std::vector<float> vx_list;
        // std::vector<float> vy_list;
        std::vector<std::vector<float>> v_list;
        std::vector<std::vector<float>> vt_list;
        std::vector<std::vector<float>> vn_list;
        std::vector<std::vector<int>> f_list;
        std::vector<std::vector<int>> f_vt_list;
        TGAImage diffusemap;

        Obj();
        void read_obj(std::string filename);
        void load_texture(const char* filename,TGAImage& img);
        TGAColor diffuse(Vec2f uv);

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
                std::istringstream IL(line);
				IL >> str >> x >> y >> z;
                std::vector<float> cur_vt;
                cur_vt.push_back(x);
                cur_vt.push_back(y);
                cur_vt.push_back(z);
                vt_list.push_back(cur_vt);

			}
			else if(line[1] == 'n'){
                std::istringstream IL(line);
				IL >> str >> x >> y >> z;
                std::vector<float> cur_vn;
                cur_vn.push_back(x);
                cur_vn.push_back(y);
                cur_vn.push_back(z);
                vn_list.push_back(cur_vn);
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
            std::vector<int> cur_f;	
            std::vector<int> cur_vt;
			for(int i = 0;i < 3;i++){
 
				IL  >> ix >> c>> iy >> c >>iz ;
                cur_f.push_back(ix-1);
                cur_vt.push_back(iy-1);
			}
            //std::cout<<cur_f[0]<<std::endl;
            f_list.push_back(cur_f);
            f_vt_list.push_back(cur_vt);
            
             
        }    
    }
    vn = v_list.size();
    fn = f_list.size();
    load_texture("african_head_diffuse.tga",diffusemap);
    in.close();
}

void Obj::load_texture(const char* filename,TGAImage& img){
    std::cout << "loading " <<(img.read_tga_file(filename) ? "ok": "failed") << std::endl;
    img.flip_vertically();
}

TGAColor Obj::diffuse(Vec2f uv){
    Vec2i uvwh(uv.x*diffusemap.get_width(), uv.y*diffusemap.get_height());//find the pixel color by turning the uv coordinate to xy coordinate
    //std::cout<<uv.x<<" "<<uv.y<<std::endl;
    //std::cout<<uvwh.x<<" "<<uvwh.y<<std::endl;
    return diffusemap.get(uvwh.x,uvwh.y);
}
#endif //__OBJ_H__