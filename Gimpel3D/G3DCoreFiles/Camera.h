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
#ifndef CAMERA_H
#define CAMERA_H

void Set_Camera(float *pos, float *rot);
void Get_Camera(float *pos, float *rot);
void Set_Target_Camera(float *pos, float *rot);

extern bool camera_moving;
void Update_Camera();

void Move_Forward(float f);
void Move_Back(float f);
void Move_Up(float f);
void Move_Down(float f);
void Move_Left(float f);
void Move_Right(float f);
void Turn_Up(float f);
void Turn_Down(float f);
void Turn_Left(float f);
void Turn_Right(float f);
void Spin_Left(float f);
void Spin_Right(float f);

extern float camera_pos[3];
extern float camera_rot[3];
extern float camera_front[3];
extern float camera_up[3];
void Get_Matrices();
void Update_Camera_Dir();

extern double modelview_matrix[16];
extern double projection_matrix[16];
extern int viewport[8];

void Set_GL_Camera_Transform();


#endif