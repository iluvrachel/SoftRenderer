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

void draw_wireframe(std::string obj_path,TGAImage &image){
    Obj obj;
    obj.read_obj(obj_path);
    for(int i=0;i<obj.f_list.size(); i+=3){
        
        float x0 = obj.vx_list[obj.f_list[i]];
        //std::cout<<obj.f_list[i]<<" "<<obj.f_list[i+1]<<std::endl;
        x0 = (x0+1.)*400/2.;
        float y0 = obj.vy_list[obj.f_list[i]];
        y0 = (y0+1.)*400/2.;
        float x1 = obj.vx_list[obj.f_list[i+1]];
        x1 = (x1+1.)*400/2.;
        float y1 = obj.vy_list[obj.f_list[i+1]];
        y1 = (y1+1.)*400/2.;
        float x2 = obj.vx_list[obj.f_list[i+2]];
        x2 = (x2+1.)*400/2.;
        float y2 = obj.vy_list[obj.f_list[i+2]];
        y2 = (y2+1.)*400/2.;

        
        //std::cout<<x0<<" "<<x1<<" "<<x2<<std::endl;

        line(x0,y0,x1,y1,image,white);
        line(x2,y2,x1,y1,image,white);
        line(x0,y0,x2,y2,image,white);

    }

}

int main(int argc, char** argv) {
	TGAImage image(400, 400, TGAImage::RGB);
	image.set(52, 41, red);

    //line(50,80,10,10,image,white);
    draw_wireframe("wawa.obj",image);

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    system("pause");
	return 0;
}

