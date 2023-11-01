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
#ifndef SKIN_STATIC_LABEL_H
#define SKIN_STATIC_LABEL_H

class CSkinStatic : public CStatic
{
public:
	CSkinStatic()
	{
	}
	~CSkinStatic()
	{
	}
	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
};







#endif
