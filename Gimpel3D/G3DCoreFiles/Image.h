/*  Gimpel3D 2D/3D Stereo Converter
    Copyright (C) 2008-2011  Daniel René Dever (Gimpel)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef IMAGE_H
#define IMAGE_H


unsigned int Get_GL_Texture(char *file);
void Free_GL_Texture(unsigned int id);

unsigned char* Get_Image_Data(char *file, int *width, int *height, int *bits);

unsigned char* Get_RGB_Image_Data(char *file, int *width, int *height);//, int *bits);
unsigned char* Replace_RGB_Image_Data(unsigned char *buffer, int old_width, int old_height, char *file, int *width, int *height);//, int *bits);

unsigned char* Get_Grayscale_Image_Data(char *file, int *width, int *height, int *bits);
bool Save_Image(unsigned char *rgb, int w, int h, char *file);
bool Save_Grayscale_Image(unsigned char *img, int w, int h, char *file);

unsigned char* Get_Grayscale_Image_Data(char *file, int *width, int *height, int *bits);

bool Grayscale_To_RGB(unsigned char *dst, unsigned char *src, int num);
bool RGB_To_Grayscale(unsigned char *dst, unsigned char *src, int num);
bool RGB_To_Single_Channel(unsigned char *dst, unsigned char *src, int num, int channel);

bool RGB_To_Bordered_Single_Channel(int border_size, unsigned char *dst, unsigned char *src, int w, int h, int channel);
bool RGB_To_Bordered_Grayscale(int border_size, unsigned char *dst, unsigned char *src, int w, int h);

bool RGB_To_RGBA(unsigned char *dst, unsigned char *src, int num);

unsigned int Create_GL_Texture(int width, int height, int bits, unsigned char *data, int filter, bool clamp);
unsigned int Create_Frame_Texture(int width, int height, unsigned char *data);
unsigned int Create_32Bit_Frame_Texture(int width, int height, unsigned char *data);

bool Edge_Detection(unsigned char *image, unsigned char *edge_image, int w, int h, float *contrast_map);

bool Save_Clean_Image(float *rgb, int w, int h, char *file);

bool Convert_Image_To_Float(float *dst, unsigned char *src, int total);

bool Save_Clean_Image(unsigned char *rgb, int w, int h, char *file);

#endif
