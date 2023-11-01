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
#ifndef SKIN_CONTROL_EDGE_H
#define SKIN_CONTROL_EDGE_H

extern CPen control_edge_lightest_pen;
extern CPen control_edge_lighter_pen;
extern CPen control_edge_darker_pen;
extern CPen control_edge_darkest_pen;

__forceinline void DrawSkinControlEdge(CDC *pDC, RECT *rect, bool raised)
{
	POINT points[3];
	if(raised)
	{
		pDC->SelectObject(&control_edge_lighter_pen);
		points[0].x = rect->left;
		points[0].y = rect->bottom-2;
		points[1].x = rect->left;
		points[1].y = rect->top;
		points[2].x = rect->right;
		points[2].y = rect->top;
		pDC->Polyline(points, 3);

		pDC->SelectObject(&control_edge_lightest_pen);
		points[0].x++;
		points[0].y--;
		points[1].x++;
		points[1].y++;
		points[2].x--;
		points[2].y++;
		pDC->Polyline(points, 3);		

		pDC->SelectObject(&control_edge_darker_pen);
		points[0].x = rect->left+1;
		points[0].y = rect->bottom-2;
		points[1].x = rect->right-2;
		points[1].y = rect->bottom-2;
		points[2].x = rect->right-2;
		points[2].y = rect->top+1;
		pDC->Polyline(points, 3);

		pDC->SelectObject(&control_edge_darkest_pen);
		points[0].x = rect->left;
		points[0].y = rect->bottom-1;
		points[1].x = rect->right-1;
		points[1].y = rect->bottom-1;
		points[2].x = rect->right-1;
		points[2].y = rect->top;
		pDC->Polyline(points, 3);
}
else
{
		pDC->SelectObject(&control_edge_darker_pen);
		points[0].x = rect->left;
		points[0].y = rect->bottom-2;
		points[1].x = rect->left;
		points[1].y = rect->top;
		points[2].x = rect->right;
		points[2].y = rect->top;
		pDC->Polyline(points, 3);

		pDC->SelectObject(&control_edge_darkest_pen);
		points[0].x++;
		points[0].y--;
		points[1].x++;
		points[1].y++;
		points[2].x--;
		points[2].y++;
		pDC->Polyline(points, 3);

		pDC->SelectObject(&control_edge_lighter_pen);
		points[0].x = rect->left+1;
		points[0].y = rect->bottom-2;
		points[1].x = rect->right-2;
		points[1].y = rect->bottom-2;
		points[2].x = rect->right-2;
		points[2].y = rect->top+1;
		pDC->Polyline(points, 3);

		pDC->SelectObject(&control_edge_lightest_pen);
		points[0].x = rect->left;
		points[0].y = rect->bottom-1;
		points[1].x = rect->right-1;
		points[1].y = rect->bottom-1;
		points[2].x = rect->right-1;
		points[2].y = rect->top;
		pDC->Polyline(points, 3);
	}
}



#endif