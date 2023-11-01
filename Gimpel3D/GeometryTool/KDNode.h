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
#ifndef KD_TREE_NODE_H
#define KD_TREE_NODE_H



class KD_NODE
{
public:
	KD_NODE()
	{
		pos_branch = 0;
		neg_branch = 0;
	}
	~KD_NODE()
	{
	}
	int axis, axis2, axis3;
	int node_index;
	float split_pos[3];
	float ray_start[3];
	float aabb_lo[3];
	float aabb_hi[3];
	KD_NODE *pos_branch, *neg_branch;
};



__forceinline bool Get_Near_Far_Nodes(float *end, float *dir, KD_NODE *node, KD_NODE **near_node, KD_NODE **far_node)
{
	(*near_node) = (*far_node) = 0;
	if(node->axis==-1)return false;
	float c = node->split_pos[node->axis];
	if(node->ray_start[node->axis]>c)
	{
		//on positive side
		if(node->pos_branch)
		{
			memcpy(node->pos_branch->ray_start, node->ray_start, sizeof(float)*3);
			(*near_node) = node->pos_branch;
		}
		//process opposite side first
		if(dir[node->axis]<0)
		{
			//ray might cross to negative side, save it
			if(node->neg_branch)
			{
				float d2 = c-end[node->axis];
				if(d2>0)
				{
					float d1 = node->ray_start[node->axis]-c;
					float t = d1+d2;
					float p1 = d1/t;//percent of end pos
					float p2 = d2/t;//percent of start pos
					node->neg_branch->ray_start[0] = (node->ray_start[0]*p2)+(end[0]*p1);
					node->neg_branch->ray_start[1] = (node->ray_start[1]*p2)+(end[1]*p1);
					node->neg_branch->ray_start[2] = (node->ray_start[2]*p2)+(end[2]*p1);
					float ta = node->neg_branch->ray_start[node->axis2];
					float tb = node->neg_branch->ray_start[node->axis3];
					if(dir[node->axis2]>0){if(ta>node->neg_branch->aabb_hi[node->axis2])return true;}
					else {if(ta<node->neg_branch->aabb_lo[node->axis2])return true;}
					if(dir[node->axis3]>0){if(tb>node->neg_branch->aabb_hi[node->axis3])return true;}
					else {if(tb<node->neg_branch->aabb_lo[node->axis3])return true;}
					{
						(*far_node) = node->neg_branch;
					}
				}
			}
		}
	}
	else
	{
		//on negative side
		if(node->neg_branch)
		{
			memcpy(node->neg_branch->ray_start, node->ray_start, sizeof(float)*3);
			(*near_node) = node->neg_branch;
		}
		if(dir[node->axis]>0)
		{
			//ray might cross to positive side, save it
			if(node->pos_branch)
			{
				float d2 = end[node->axis]-c;
				if(d2>0)
				{
					float d1 = c-node->ray_start[node->axis];
					float t = d1+d2;
					float p1 = d1/t;//percent of end pos
					float p2 = d2/t;//percent of start pos
					node->pos_branch->ray_start[0] = (node->ray_start[0]*p2)+(end[0]*p1);
					node->pos_branch->ray_start[1] = (node->ray_start[1]*p2)+(end[1]*p1);
					node->pos_branch->ray_start[2] = (node->ray_start[2]*p2)+(end[2]*p1);
					float ta = node->pos_branch->ray_start[node->axis2];
					float tb = node->pos_branch->ray_start[node->axis3];
					if(dir[node->axis2]>0){if(ta>node->pos_branch->aabb_hi[node->axis2])return true;}
					else {if(ta<node->pos_branch->aabb_lo[node->axis2])return true;}
					if(dir[node->axis3]>0){if(tb>node->pos_branch->aabb_hi[node->axis3])return true;}
					else {if(tb<node->pos_branch->aabb_lo[node->axis3])return true;}
					{
						(*far_node) = node->pos_branch;
					}
				}
			}
		}
	}
	return true;
}










#endif