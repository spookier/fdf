#include "fdf.h"
#include "mlx_int.h"

#define SCREEN_HEIGHT 	1080
#define SCREEN_WIDTH 	1080

#define SIZE_X 10
#define SIZE_Y 10

typedef float			t_v2f __attribute__((vector_size (8)));
typedef unsigned int	t_v2u __attribute__((vector_size (8)));
typedef int				t_v2i __attribute__((vector_size (8)));

void    my_mlx_pixel_put(t_data *data, t_v2i pos, int color)
{
    char    *dst;

    dst = data->addr + (pos[1] * data->line_length + pos[0] * (data->bits_per_pixel / 8));
    *(unsigned int*)dst = color;
}

double	linear_interpolation(double a, double b, double ratio)
{
	double	c;

	c = a * ratio + b * (1.0 - ratio);
	return (c);
}

void	bresnahan_put_line(t_data *data, t_v2i p1, t_v2i p2, int color)
{
	t_v2i	d;
	t_v2i	s;
	int		e;
	int		error;

	d = (t_v2i){abs(p2[0] - p1[0]), -abs(p2[1] - p1[1])};
	s = (t_v2i){0, 0};
	if (p1[0] < p2[0])
		s[0] = 1;
	else
		s[0] = -1;
	if (p1[1] < p2[1])
		s[1] = 1;
	else
		s[1] = -1;
	error = d[0] + d[1];
	while (1)
	{
		my_mlx_pixel_put(data, (t_v2i){p1[0], p1[1]}, color);
		if (p1[0] == p2[0] && p1[1] == p2[1])
			return ;
		e = 2 * error;
		if (e >= d[1])
		{
			if (p1[0] == p2[0])
				return ;
			error = error + d[1];
			p1[0] += s[0];
		}
		if (e <= d[0])
		{
			if (p1[1] == p2[1])
				return ;
			error = error + d[0];
			p1[1] += s[1];
		}
	}
}

void	put_line(t_data *data, t_v2i p1, t_v2i p2, int color)
{
	t_v2f	diff;
	t_v2f	start;
	int		count;

	diff = (t_v2f){p2[0], p2[1]} - (t_v2f){p1[0], p1[1]};
	if (fabsf(diff[0]) > fabsf(diff [1]))
	{
		count = fabsf(diff[0]);
		diff = diff / fabsf(diff[0]);
	}
	else
	{
		count = fabsf(diff[1]);
		diff = diff / fabsf(diff[1]);
	}
	start = (t_v2f){p1[0], p1[1]};
	while (count)
	{
		my_mlx_pixel_put(data, (t_v2i){start[0], start[1]}, color);
		start = start + diff;
		count--;
	}
}

int	key_hook(int keycode, t_data *data)
{
	if(keycode == ESC)
		exit(0);
	return (0);
}


void draw_grid(t_data *data, int lines, t_v2i size, t_v2i pos)
{
    int i;
    int x;
    int y;
    int spacing;

    spacing = size[0] / (lines + 1);
    x = 0;
    y = 0;
    i = 1;
    while(i <= lines || y <= lines)
    {
        x = i * spacing + pos[0];
        y = i * spacing + pos[1];
        put_line(data, (t_v2i){x, pos[1]}, (t_v2i){x, size[1] + pos[1]}, WHITE);
        put_line(data, (t_v2i){pos[0], y}, (t_v2i){size[0] + pos[0], y}, WHITE);
        i++;
        y++;
    }
}

void draw_grid_rotated(t_data *data, int lines, t_v2i size, t_v2i pos)
{
    int i;
    int x;
    int y;
    int spacing;
    t_v2i end;

    // Calculate the spacing between grid lines
    spacing = size[0] / (lines + 1);

    // Define the rotation angle in radians
    double angle = M_PI / 4.0;

    // Loop through the grid lines
    for (i = 1; i <= lines; i++) 
	{

        // Calculate the coordinates of the endpoints of the current grid line
        x = i * spacing;
        y = 0;
        double new_x = x * cos(angle) - y * sin(angle);
        double new_y = x * sin(angle) + y * cos(angle);
        t_v2i start = {pos[0] + (int)new_x, pos[1] + (int)(new_y * 0.5)};

        x = i * spacing;
        y = size[1];
        new_x = x * cos(angle) - y * sin(angle);
        new_y = x * sin(angle) + y * cos(angle);
        end[0] = new_x;
        end[1] = new_y * 0.5;
        end[0] += pos[0];
        end[1] += pos[1];

        // Draw the current grid line
        put_line(data, start, end, WHITE);

        // Calculate the coordinates of the endpoints of the current grid line
        x = 0;
        y = i * spacing;
        new_x = x * cos(angle) - y * sin(angle);
        new_y = x * sin(angle) + y * cos(angle);
        start[0] = pos[0] + (int)new_x;
        start[1] = pos[1] + (int)(new_y * 0.5);

        x = size[0];
        y = i * spacing;
        new_x = x * cos(angle) - y * sin(angle);
        new_y = x * sin(angle) + y * cos(angle);
        end[0] = new_x;
        end[1] = new_y * 0.5;
        end[0] += pos[0];
        end[1] += pos[1];

        // Draw the current grid line
        put_line(data, start, end, WHITE);
    }
}


int    main(void)
{
	void	*mlx;
	void	*mlx_win;
	t_data	img;
	int		**map;

	mlx = mlx_init();
	mlx_win = mlx_new_window(mlx, 1080, 1080, "fdf");
	img.img = mlx_new_image(mlx, 1080, 1080);
	img.addr = mlx_get_data_addr(img.img, &img.bits_per_pixel, &img.line_length, &img.endian);

	mlx_key_hook(mlx_win, key_hook, &img);


	draw_grid_rotated(&img, 15, (t_v2i){600, 600}, (t_v2i){500, 300});
	
	mlx_put_image_to_window(mlx, mlx_win, img.img, 0, 0);
	//usleep(100000);
	
	mlx_loop(mlx);
}
