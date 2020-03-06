#include "tgaimage.h"
#include "read_obj.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) { 
    bool steep = false; 
    if (std::abs(x0-x1)<std::abs(y0-y1)) { 
        std::swap(x0, y0); 
        std::swap(x1, y1); 
        steep = true; 
    } 
    if (x0>x1) { 
        std::swap(x0, x1); 
        std::swap(y0, y1); 
    } 
    int dx = x1-x0; 
    int dy = y1-y0; 
    int derror2 = std::abs(dy)*2; 
    int error2 = 0; 
    int y = y0; 
    for (int x=x0; x<=x1; x++) { 
        if (steep) { 
            image.set(y, x, color); 
        } else { 
            image.set(x, y, color); 
        } 
        error2 += derror2; 
        if (error2 > dx) { 
            y += (y1>y0?1:-1); 
            error2 -= dx*2; 
        } 
    } 
} 

void draw_triangle(int x0,int y0,int x1,int y1,int x2,int y2,TGAImage &image,TGAColor color){
    line(x0,y0,x1,y1,image,color);
    line(x2,y2,x1,y1,image,color);
    line(x0,y0,x2,y2,image,color);
}

void swap(int& x0, int& y0, int& x1, int& y1){
    int tmp_x = x0;
    int tmp_y = y0;
    x0 = x1;
    y0 = y1;
    x1 = tmp_x;
    y1 = tmp_y;
}

// void triangle(int x0,int y0,int x1,int y1,int x2,int y2,TGAImage &image,TGAColor color,int width,int height){
//     //assume that the order from bottom to top is y0, y1, y2
//     if(y0>y1) swap(x0,y0,x1,y1);
//     if(y1>y2) swap(x1,y1,x2,y2);
//     if(y0>y2) swap(x0,y0,x2,y2);
//     // scanning
//     for(int i=y0;i<y1;i++){
//         int dy
//     }

// }

bool edge_equation(int x0,int y0,int x1,int y1,int x2,int y2, int cur_x, int cur_y){
    int eq_cur = (y2-y1)*cur_x + (x1-x2)*cur_y + (x2*y1 - x1*y2);
    int eq_another = (y2-y1)*x0 + (x1-x2)*y0 + (x2*y1 - x1*y2);
    // if(eq_cur*eq_another>=0) return true;
    // else return false; 
    if(eq_cur>=0 && eq_another>=0)return true;
    if(eq_cur<=0 && eq_another<=0)return true;
    return false;
}

void fill_triangle(int x0,int y0,int x1,int y1,int x2,int y2,TGAImage &image,TGAColor color){
    // find bounding box of the triangle
    int bb_top = y0 > y1 ? (y0 > y2 ? y0 : y2) : (y1 > y2 ? y1 : y2) +1;
    int bb_bottom = y0 < y1 ? (y0 < y2 ? y0 : y2) : (y1 < y2 ? y1 : y2) -1;
    int bb_right = x0 > x1 ? (x0 > x2 ? x0 : x2) : (x1 > x2 ? x1 : x2) +1;
    int bb_left = x0 < x1 ? (x0 < x2 ? x0 : x2) : (x1 < x2 ? x1 : x2) -1;
    for(int i=bb_left; i<=bb_right; i++){
        for(int j=bb_bottom; j<=bb_top; j++){
            if(edge_equation(x0,y0,x1,y1,x2,y2,i,j) && 
               edge_equation(x1,y1,x2,y2,x0,y0,i,j) &&
               edge_equation(x2,y2,x1,y1,x0,y0,i,j)){
                //std::cout<<i<<" "<<j<<std::endl;
                image.set(i, j, color);
            }
        }
    }
}

void draw_wireframe(std::string obj_path,TGAImage &image,int height, int width){
    Obj obj;
    obj.read_obj(obj_path);
    for(int i=0;i<obj.f_list.size(); i+=3){
        
        float x0 = obj.vx_list[obj.f_list[i]];
        //std::cout<<obj.f_list[i]<<" "<<obj.f_list[i+1]<<std::endl;
        x0 = (x0+1.)*width/2.;
        float y0 = obj.vy_list[obj.f_list[i]];
        y0 = (y0+1.)*height/2.;
        float x1 = obj.vx_list[obj.f_list[i+1]];
        x1 = (x1+1.)*width/2.;
        float y1 = obj.vy_list[obj.f_list[i+1]];
        y1 = (y1+1.)*height/2.;
        float x2 = obj.vx_list[obj.f_list[i+2]];
        x2 = (x2+1.)*width/2.;
        float y2 = obj.vy_list[obj.f_list[i+2]];
        y2 = (y2+1.)*height/2.;

        
        //std::cout<<x0<<" "<<x1<<" "<<x2<<std::endl;
        draw_triangle(x0,y0,x1,y1,x2,y2,image,white);


    }

}

void draw_meshface(std::string obj_path,TGAImage &image,int height, int width){
    Obj obj;
    obj.read_obj(obj_path);
    for(int i=0;i<obj.f_list.size(); i+=3){
        
        float x0 = obj.vx_list[obj.f_list[i]];
        //std::cout<<obj.f_list[i]<<" "<<obj.f_list[i+1]<<std::endl;
        x0 = (x0+1.)*width/2.;
        float y0 = obj.vy_list[obj.f_list[i]];
        y0 = (y0+1.)*height/2.;
        float x1 = obj.vx_list[obj.f_list[i+1]];
        x1 = (x1+1.)*width/2.;
        float y1 = obj.vy_list[obj.f_list[i+1]];
        y1 = (y1+1.)*height/2.;
        float x2 = obj.vx_list[obj.f_list[i+2]];
        x2 = (x2+1.)*width/2.;
        float y2 = obj.vy_list[obj.f_list[i+2]];
        y2 = (y2+1.)*height/2.;

        
        //std::cout<<x0<<" "<<x1<<" "<<x2<<std::endl;
        fill_triangle(x0,y0,x1,y1,x2,y2,image,TGAColor(rand()%255, rand()%255, rand()%255, 255));


    }

}

int main(int argc, char** argv) {
    int height = 400;
    int width = 400;
	TGAImage image(height, width, TGAImage::RGB);
	image.set(52, 41, red);

    //line(50,80,10,10,image,white);
    //draw_wireframe("wawa.obj",image,height,width);
    //triangle(10,10,200,190,390,10,image,white,400,400);
    draw_meshface("wawa.obj",image,height,width);

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    system("pause");
	return 0;
}

