#include "tgaimage.h"
#include "read_obj.h"
#include "vec.h"
#include <limits>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const int height = 400;
const int width = 400;

Obj obj;

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

Vec3f barycentric(Vec3f A, Vec3f B, Vec3f C, Vec3f P) {
    //Vec3f s[2];
    Vec3f eq_x(C.x-A.x, B.x-A.x, A.x-P.x);
    Vec3f eq_y(C.y-A.y, B.y-A.y, A.y-P.y);
    // solve linear equation in two unknows 

    Vec3f u = eq_x^eq_y;//u,v
    if (std::abs(u.z)>1e-2) // dont forget that u.z is integer. If it is zero then triangle ABC is degenerate
        return Vec3f(1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z); // (1-u-v), u, v
    return Vec3f(-1,1,1); // in this case generate negative coordinates, it will be thrown away by the rasterizator
}


bool edge_equation(int x0,int y0,int x1,int y1,int x2,int y2, int cur_x, int cur_y){
    int eq_cur = (y2-y1)*cur_x + (x1-x2)*cur_y + (x2*y1 - x1*y2);
    int eq_another = (y2-y1)*x0 + (x1-x2)*y0 + (x2*y1 - x1*y2);
    // if(eq_cur*eq_another>=0) return true;
    // else return false; 
    if(eq_cur>=0 && eq_another>=0)return true;
    if(eq_cur<=0 && eq_another<=0)return true;
    return false;
}

void fill_triangle(float *zbuffer,Vec3f p0,Vec3f p1,Vec3f p2,
                    TGAImage &image,Vec3f t0,Vec3f t1,Vec3f t2, Vec3f n0,Vec3f n1,Vec3f n2){
    // find bounding box of the triangle
    int bb_top = p0.y > p1.y ? (p0.y > p2.y ? p0.y : p2.y) : (p1.y > p2.y ? p1.y : p2.y) ;
    int bb_bottom = p0.y < p1.y ? (p0.y < p2.y ? p0.y : p2.y) : (p1.y < p2.y ? p1.y : p2.y) ;
    int bb_right = p0.x > p1.x ? (p0.x > p2.x ? p0.x : p2.x) : (p1.x > p2.x ? p1.x : p2.x) ;
    int bb_left = p0.x < p1.x ? (p0.x < p2.x ? p0.x : p2.x) : (p1.x < p2.x ? p1.x : p2.x) ;
    Vec3f P;
    for(int i=bb_left; i<=bb_right; i++){
        for(int j=bb_bottom; j<=bb_top; j++){
            // Vec3f pA(x0,y0,z0);
            // Vec3f pB(x1,y1,z1);
            // Vec3f pC(x2,y2,z2);
            
            P.x = i;
            P.y = j;
            Vec3f bc = barycentric(p0,p1,p2,P);
            if (bc.x<0 || bc.y<0 || bc.z<0) continue;
            // if(edge_equation(x0,y0,x1,y1,x2,y2,i,j) && 
            //    edge_equation(x1,y1,x2,y2,x0,y0,i,j) &&
            //    edge_equation(x2,y2,x1,y1,x0,y0,i,j)){
            //     image.set(i, j, color);
            // }
            P.z = 0;
            P.z += p0.z*bc.x + p1.z*bc.y + p2.z*bc.z;

            Vec2f texture_p(0,0);

            texture_p.x = t0.x*bc.x + t1.x*bc.y + t2.x*bc.z;
            texture_p.y = t0.y*bc.x + t1.y*bc.y + t2.y*bc.z;
            
            
            if (zbuffer[int(P.x+P.y*width)]<P.z) {
                //std::cout<<P.z<<" "<<zbuffer[int(P.x+P.y*width)]<<std::endl;
                //if(i==290 && j==200) std::cout<<" "<<i<<" "<<j<<" "<<P.z<<" "<<zbuffer[int(P.x+P.y*width)]<<std::endl;
                zbuffer[int(P.x+P.y*width)] = P.z;
                //if(i==290 && j==200) std::cout<<" "<<i<<" "<<j<<" "<<P.z<<" "<<zbuffer[int(P.x+P.y*width)]<<std::endl;
                //std::cout<<P.z<<" "<<zbuffer[int(P.x+P.y*width)]<<std::endl;

                TGAColor color = obj.diffuse(texture_p);
                image.set(P.x, P.y, color);
            }
            else{
                //std::cout<<P<<std::endl;
            }
        }
    }
}

void draw_wireframe(std::string obj_path,TGAImage &image,int height, int width){
    Obj obj;
    obj.read_obj(obj_path);
    for(int i=0;i<obj.f_list.size(); i++){
        
        float x0 = obj.v_list[obj.f_list[i][0]][0];
        //std::cout<<obj.f_list[i]<<" "<<obj.f_list[i+1]<<std::endl;
        x0 = (x0+1.)*width/2.;
        float y0 = obj.v_list[obj.f_list[i][0]][1];
        y0 = (y0+1.)*height/2.;
        float x1 = obj.v_list[obj.f_list[i][1]][0];
        x1 = (x1+1.)*width/2.;
        float y1 = obj.v_list[obj.f_list[i][1]][1];
        y1 = (y1+1.)*height/2.;
        float x2 = obj.v_list[obj.f_list[i][2]][0];
        x2 = (x2+1.)*width/2.;
        float y2 = obj.v_list[obj.f_list[i][2]][1];
        y2 = (y2+1.)*height/2.;


        
        //std::cout<<x0<<" "<<x1<<" "<<x2<<std::endl;
        draw_triangle(x0,y0,x1,y1,x2,y2,image,white);


    }

}

void draw_meshface(std::string obj_path,TGAImage &image,int height, int width){

    float *zbuffer = new float[width*height];
    for (int i=width*height;i>=0; i--){
        zbuffer[i] = -std::numeric_limits<float>::max();
    }
    
    obj.read_obj(obj_path);
    for(int i=0;i<obj.f_list.size(); i++){
        
        float x0 = obj.v_list[obj.f_list[i][0]][0];
        //x0 = (x0+1.)*width/2.;
        float y0 = obj.v_list[obj.f_list[i][0]][1];
        //y0 = (y0+1.)*height/2.;
        float z0 = obj.v_list[obj.f_list[i][0]][2];
        //z0 = (z0+1.)*height/2.;
        float x1 = obj.v_list[obj.f_list[i][1]][0];
        //x1 = (x1+1.)*width/2.;
        float y1 = obj.v_list[obj.f_list[i][1]][1];
        //y1 = (y1+1.)*height/2.;
        float z1 = obj.v_list[obj.f_list[i][1]][2];
        //z1 = (z1+1.)*height/2.;
        float x2 = obj.v_list[obj.f_list[i][2]][0];
        //x2 = (x2+1.)*width/2.;
        float y2 = obj.v_list[obj.f_list[i][2]][1];
        //y2 = (y2+1.)*height/2.;
        float z2 = obj.v_list[obj.f_list[i][2]][2];
        //z2 = (z2+1.)*height/2.;

        Vec3f p0(x0,y0,z0);
        Vec3f p1(x1,y1,z1);
        Vec3f p2(x2,y2,z2);

        Vec3f n = (p2-p0)^(p1-p0);
        n.normalize();
        //std::cout<<p2<<std::endl;
        // Vec3f n_tmp ( n.x*(1/n.norm()) ,n.y*(1/n.norm()), n.z*(1/n.norm()) );
        //n = n*(1/n.norm());
        //std::cout<<n.x<<" "<<(1/n.norm())<<" "<<n_tmp.x<<std::endl;
        Vec3f light(0.,0.,-1.);
        float intensity = n*light;
        
        Vec3f t0(obj.vt_list[obj.f_vt_list[i][0]][0],obj.vt_list[obj.f_vt_list[i][0]][1],obj.vt_list[obj.f_vt_list[i][0]][2]);
        Vec3f t1(obj.vt_list[obj.f_vt_list[i][1]][0],obj.vt_list[obj.f_vt_list[i][1]][1],obj.vt_list[obj.f_vt_list[i][1]][2]);
        Vec3f t2(obj.vt_list[obj.f_vt_list[i][2]][0],obj.vt_list[obj.f_vt_list[i][2]][1],obj.vt_list[obj.f_vt_list[i][2]][2]);

        Vec3f n0(obj.vn_list[obj.f_list[i][0]][0],obj.vn_list[obj.f_list[i][0]][1],obj.vn_list[obj.f_list[i][0]][2]);
        Vec3f n1(obj.vn_list[obj.f_list[i][0]][0],obj.vn_list[obj.f_list[i][0]][1],obj.vn_list[obj.f_list[i][0]][2]);
        Vec3f n2(obj.vn_list[obj.f_list[i][0]][0],obj.vn_list[obj.f_list[i][0]][1],obj.vn_list[obj.f_list[i][0]][2]);
        
        //std::cout<<t0<<std::endl;

        if(intensity>0){
            p0.x = (p0.x+1.)*width/2.+.5;
            p0.y = (p0.y+1.)*height/2.+.5;
            p1.x = (p1.x+1.)*width/2.+.5;
            p1.y = (p1.y+1.)*height/2.+.5;
            p2.x = (p2.x+1.)*width/2.+.5;
            p2.y = (p2.y+1.)*height/2.+.5;
            // Vec2f cur_uv(t0.x,t0.y);
            // TGAColor cur_color = obj.diffuse(cur_uv);
            fill_triangle(zbuffer,p0,p1,p2,image,t0,t1,t2,n0,n1,n2);
        }
        

    }

}

int main(int argc, char** argv) {

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

