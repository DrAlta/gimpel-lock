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
#include "stdafx.h"
#include "GLText.h"
#include <vector>

using namespace std;

extern bool redraw_edit_window;

int max_console_lines = 5;
int console_line_height = 20;

int console_start_x = 10;
int console_start_y = 10;

float console_text_color[4] = {0,1,0,1};
float help_text_color[4] = {1,1,0,1};

bool print_help = false;

void Print_Help();

class CONSOLE_LINE
{
public:
	CONSOLE_LINE()
	{
	}
	~CONSOLE_LINE()
	{
	}
	char text[256];
};

vector<CONSOLE_LINE> console_lines;

bool Init_Console()
{
	return true;
}

bool Free_Console()
{
	console_lines.clear();
	return true;
}

bool Clear_Console()
{
	Clear_Static_Text_Items();
	console_lines.clear();
	return true;
}

bool Update_Console()
{
	Clear_Static_Text_Items();
	int n = console_lines.size();
	int height = console_start_y+(n*console_line_height);
	for(int i = 0;i<n;i++)
	{
		Add_Static_Text_Item(console_start_x, height, console_text_color, console_lines[i].text);
		height -= console_line_height;
	}
	if(print_help)
	{
		Print_Help();
	}
	return true;
}

bool Scroll_Console()
{
	int n = console_lines.size();
	for(int i = 1;i<n;i++)
	{
		strcpy(console_lines[i-1].text, console_lines[i].text);
	}
	return true;
}

bool Print_To_Console(const char *fmt, ...)
{
	int n = console_lines.size();
	if(n>=max_console_lines)
	{
		Scroll_Console();
	}
	else
	{
		CONSOLE_LINE cl;
		console_lines.push_back(cl);
		n++;
	}
	char text[512];
	va_list		ap;
	if (fmt == NULL)return false;
	va_start(ap, fmt);
	vsprintf(text, fmt, ap);
	va_end(ap);
	strcpy(console_lines[n-1].text, text);
	Update_Console();
	redraw_edit_window = true;
	return true;
}


bool Toggle_Print_Help()
{
	print_help = !print_help;
	Update_Console();
	return true;
}

bool Close_Print_Help()
{
	print_help = false;
	Update_Console();
	return true;
}

//FIXTHIS add better info or eliminate
void Print_Help()
{
	int x = 300;
	int y = 580;
	int spacing = 20;
	Add_Static_Text_Item(x-10, y, help_text_color, "Navigation Mode:");y-=spacing;
	Add_Static_Text_Item(x, y, help_text_color, "Left Mouse Click and Drag: Move Image");y-=spacing;
	Add_Static_Text_Item(x, y, help_text_color, "Right Mouse Click and Drag: Zoom In / Out");y-=spacing*2;

	Add_Static_Text_Item(x-10, y, help_text_color, "Edit Mode:");y-=spacing;
	Add_Static_Text_Item(x, y, help_text_color, "Left Mouse Click: Add Selection Point");y-=spacing;
	Add_Static_Text_Item(x, y, help_text_color, "Right Mouse Click: Toggle Edge Detection On/Off");y-=spacing;
	Add_Static_Text_Item(x, y, help_text_color, "Ctrl-Z or U Key: Undo");y-=spacing;
	Add_Static_Text_Item(x, y, help_text_color, "Ctrl-Y or R Key: Redo");y-=spacing;
	Add_Static_Text_Item(x, y, help_text_color, "Spacebar or Enter Key: Complete Selection");y-=spacing;
	Add_Static_Text_Item(x, y, help_text_color, "Esc or Delete Key: Cancel Selection");y-=spacing*2;

	Add_Static_Text_Item(x-10, y, help_text_color, "All Modes:");y-=spacing;
	Add_Static_Text_Item(x, y, help_text_color, "TAB Key: Toggle Edit / Navigation Modes");y-=spacing;
	Add_Static_Text_Item(x, y, help_text_color, "Up Arrow: Scroll Image Up");y-=spacing;
	Add_Static_Text_Item(x, y, help_text_color, "Down Arrow: Scroll Image Down");y-=spacing;
	Add_Static_Text_Item(x, y, help_text_color, "Left Arrow: Scroll Image Left");y-=spacing;
	Add_Static_Text_Item(x, y, help_text_color, "Right Arrow: Scroll Image Right");y-=spacing;
	Add_Static_Text_Item(x, y, help_text_color, "+: Zoom In");y-=spacing;
	Add_Static_Text_Item(x, y, help_text_color, "-: Zoom Out");y-=spacing;
	Add_Static_Text_Item(x, y, help_text_color, "Delete Key: Delete Selected Layer(s)");y-=spacing;
	Add_Static_Text_Item(x, y, help_text_color, "M Key: Merge Selected Layer(s)");y-=spacing;
	Add_Static_Text_Item(x, y, help_text_color, "A Key: Toggle Edge Detection On/Off");y-=spacing;
	Add_Static_Text_Item(x, y, help_text_color, "S Key: Click To Select Layer(s)");y-=spacing;
	Add_Static_Text_Item(x, y, help_text_color, "F Key: Toggle \"Freeze\" For Selected Layer(s)");y-=spacing;
	Add_Static_Text_Item(x, y, help_text_color, "E Key: Toggle Edge View");y-=spacing;
	Add_Static_Text_Item(x, y, help_text_color, "R Key: Reset View");y-=spacing;
	Add_Static_Text_Item(x, y, help_text_color, "C Key: Click To Modify Layer Area");y-=spacing;
	Add_Static_Text_Item(x, y, help_text_color, "Ctrl Key: Temporarily Switch Edit / Navigation Modes");y-=spacing;
}

