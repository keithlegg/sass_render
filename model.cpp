
#include <iostream>
#include <stdio.h>

#include <fstream>
#include <cstring>
#include <vector>
#include <stdlib.h> 

#include "math.h"
#include "include/Vectors.h"
#include "include/model.h"

using namespace std;


const int MAX_CHARS_PER_LINE = 512;
const int MAX_TOKENS_PER_LINE = 20;
const char* const DELIMITER = " ";

// tokens to parse out of .OBJ file
const char* v_chr = "v";  // vertex identifier in OBJ file 
const char* f_chr = "f";  // face identifier in OBJ file 



/******************************/

// FIND A BETTER PLACE FOR THIS 
// FRAMEBUFFER HAS A COPY AND NEEDS TO BE MOVED 

#define DEG_TO_RAD 0.0174532925
#define RAD_TO_DEG 57.29577951

double deg_to_rad ( double deg){
   return deg * DEG_TO_RAD;
}

double rad_to_deg ( double rad){
   return rad * RAD_TO_DEG;
}

/******************************/



int model::getnum_verts(){
   return sizeof( obj_pts ) / sizeof( obj_pts[0] );
}

int model::getnum_faces(){
   return sizeof( faces ) / sizeof( faces[0] );
}


/**********************/
void model::showinfo()
{
   cout << " loded vertex data  "<< model::vertex_count << endl;
   cout << " loded face data  "<< face_count << endl;
}


/**********************/
void model::show()
{
  
    int dnum = 100;

    int numv = getnum_verts();
    for (int xx=0;xx<numv;xx++){
        if (xx<dnum){
            cout << obj_pts[xx] << endl;
        }
    }


    int numf = getnum_faces();
    for (int xx=0;xx<numf;xx++){
        if (xx<dnum){
            // cout << faces[xx] << endl;
        }

    }

}


/**********************/
void model::save_obj( char* filename)
{
    ofstream myfile;
    myfile.open (filename);

    myfile << "#Exported with Keith's little graphics tool\n";
    myfile << "#number of verticies "<< model::vertex_count  <<"\n";
    myfile << "#number of faces     "<< face_count <<"\n";
    myfile <<"\n";

    for (int xx=0;xx<model::vertex_count;xx++){
        myfile << "v " << obj_pts[xx][0] <<" "<< obj_pts[xx][1] <<" "<< obj_pts[xx][2] <<"\n";
    }

    myfile <<"\n";
    
    if (face_count==0){
        cout << " error - no faces to export ";
    }

    cout << " face count is  " << face_count << endl;

    int ff = 0; 

    // export array of N sided faces
    for (int xx=0;xx<face_count;xx++)
    {
        myfile << "f ";
        for (ff=0; ff < faces[xx].size();ff++)
        {
            myfile << faces[xx][ff] << " "; 
        }
        myfile << "\n";
    }
        
    myfile.close();
    cout << endl << "obj file " << filename << " exported." << endl;

}


/**********************/
void model::load_obj(char* filename){
    ifstream fin;
    fin.open(filename); // open a file
    if (!fin.good()){ 
        cout << "NO FILE! "<< filename << endl;
        exit (EXIT_FAILURE); // exit if file not found
    }

    vtx_tmp.clear();
    fac_tmp.clear();


    while (!fin.eof())
    {
        char buf[MAX_CHARS_PER_LINE];
        fin.getline(buf, MAX_CHARS_PER_LINE);
        
        int i = 0;
        int n = 0; 
        fac_tmp.clear();

        const char* token[MAX_TOKENS_PER_LINE] = {};
        token[0] = strtok(buf, DELIMITER);

        //if line has data on it ...  
        if (token[0]) 
        {
            // walk the space delineated tokens 
            for (n=1; n < MAX_TOKENS_PER_LINE; n++)
            {
                token[n] = strtok(0, DELIMITER);
                if (!token[n]) break;  
            }

        }

        /******************************/

        // process the tokens
        for (i=1; i<n; i++)
        {

            // vertex tokens "v"
            if (!strcmp(token[0],v_chr))
            {
                // first 2 of 3 vertices
                if (vtx_cnt<=2){
                    vtx_tmp.push_back( atof(token[i]) );
                }
                // third vertex 
                if (vtx_cnt==3){
                    vtx_tmp.push_back( atof(token[i]) );
                    vtx_cnt=0;
                   
                    //load 3D point 
                    obj_pts[model::vertex_count].set(vtx_tmp[0],vtx_tmp[1],vtx_tmp[2]);
                    
                    vtx_tmp.clear();
                    model::vertex_count++;
                }
                vtx_cnt++;
            }
  
            /**********/
            // face tokens "f"
            if (!strcmp(token[0],f_chr))
            {
               
                // cout << " i is "<< i << " |  num_tok  " << n << endl; 

                // if end of line is hit - store loaded data and reset 
                if (i < n-1 ) 
                {
                    //keep adding fids  
                    fac_tmp.push_back( atof(token[i]) );fac_cnt++;
                }
                if (i == n-1 ) 
                {
                    //add one more fid before we stop to get lest 
                    fac_tmp.push_back( atof(token[i]) );fac_cnt++;

                    faces[face_count] = fac_tmp; 
                    fac_cnt = 1;     // fid count on line
                    fac_tmp.clear(); // tmp face buffer 
                    face_count++;    // total face count in object 
                }
            }
           
        }//iterate tokens
       
  
    }//iterate each line
}//load object


/**********************/
void model::make_cube(double scale){

    // vertices
    obj_pts[0].set(-scale, -scale,  scale);
    obj_pts[1].set(-scale, -scale, -scale);
    obj_pts[2].set( scale, -scale, -scale);
    obj_pts[3].set( scale, -scale,  scale);
    obj_pts[4].set(-scale,  scale,  scale);
    obj_pts[5].set(-scale,  scale, -scale);
    obj_pts[6].set( scale,  scale, -scale);
    obj_pts[7].set( scale,  scale,  scale);
   

    // faces - NOT zero indexed
    // faces[0].set(1,2,3,4);
    // faces[1].set(1,2,6,5);
    // faces[2].set(2,3,7,6);
    // faces[3].set(3,7,8,4);
    // faces[4].set(4,8,5,1);
    // faces[5].set(5,6,7,8);

    face_count = 6; //sizeof(vector3), etc etc 
    model::vertex_count = 8;
}

/**********************/
 void model::make_circle(int divs, double scale)
 {

    double a = 0;
    int vcnt = 0;
    int step = 360/divs;
    
    cout << "step is " << step << endl ;

    for (a=0;a<360;a=a+step)
    {
        cout << "a is " << a << endl ;
        
        //x axis 
        //obj_pts[vcnt].set(0,  sin(deg_to_rad(a))*scale, cos(deg_to_rad(a))*scale );

        //y axis 
        //obj_pts[vcnt].set( sin(deg_to_rad(a))*scale, 0, cos(deg_to_rad(a))*scale ); 

        //z axis 
        obj_pts[vcnt].set( sin(deg_to_rad(a))*scale, cos(deg_to_rad(a))*scale, 0 ); 
        vcnt++;
    } 

    // cout << faceindices[0] << faceindices[5];

    int i = 0; 

    // to get size of  (sizeof(faceindices)/sizeof(*faceindices)) 
    for (int i=0; i< vcnt ; i++) 
    {
        //fac_tmp.clear();
        fac_tmp.push_back(i+1);  

    }

    faces[0] = fac_tmp;
    face_count = 1;

    model::vertex_count = vcnt;
 }     

/**********************/
 void model::make_square(double scale)
 {
    // vertices - (3d vectors)
    obj_pts[0].set(-scale, 1,  scale);
    obj_pts[1].set( scale, 1,  scale);
    obj_pts[2].set( scale, 1, -scale);
    obj_pts[3].set(-scale, 1, -scale);

    // face - NOT zero indexed 
    //faces[0].set(1,2,3,4);

    face_count = 1;
    model::vertex_count =4;
 }    




