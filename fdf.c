#include "fdf.h"
#include "mlx_int.h"
#include "math.h"

#define SCREEN_HEIGHT 	1080
#define SCREEN_WIDTH 	1080
#define MULTIPLIER		(0.125f)

#define SIZE_X 10
#define SIZE_Y 10
#define ZOOM 100


typedef float			t_v2f __attribute__((vector_size (8)));
typedef unsigned int	t_v2u __attribute__((vector_size (8)));
typedef int				t_v2i __attribute__((vector_size (8)));

void    my_mlx_pixel_put(t_data *data, t_v2i pos, int color)
{
    char    *dst;
	//if outside screen SECURITY
	if(pos[0] < 0 || pos[1] < 0 || pos[0] >= SCREEN_WIDTH || pos[1] >= SCREEN_HEIGHT)
		return ;
    dst = data->addr + (pos[1] * data->line_length + pos[0] * (data->bits_per_pixel / 8));
    *(unsigned int*)dst = color;
}

// double	linear_interpolation(double a, double b, double ratio)
// {
// 	double	c;

// 	c = a * ratio + b * (1.0 - ratio);
// 	return (c);
// }

// void	bresnahan_put_line(t_data *data, t_v2i p1, t_v2i p2, int color)
// {
// 	t_v2i	d;
// 	t_v2i	s;
// 	int		e;
// 	int		error;

// 	d = (t_v2i){abs(p2[0] - p1[0]), -abs(p2[1] - p1[1])};
// 	s = (t_v2i){0, 0};
// 	if (p1[0] < p2[0])
// 		s[0] = 1;
// 	else
// 		s[0] = -1;
// 	if (p1[1] < p2[1])
// 		s[1] = 1;
// 	else
// 		s[1] = -1;
// 	error = d[0] + d[1];
// 	while (1)
// 	{
// 		my_mlx_pixel_put(data, (t_v2i){p1[0], p1[1]}, color);
// 		if (p1[0] == p2[0] && p1[1] == p2[1])
// 			return ;
// 		e = 2 * error;
// 		if (e >= d[1])
// 		{
// 			if (p1[0] == p2[0])
// 				return ;
// 			error = error + d[1];
// 			p1[0] += s[0];
// 		}
// 		if (e <= d[0])
// 		{
// 			if (p1[1] == p2[1])
// 				return ;
// 			error = error + d[0];
// 			p1[1] += s[1];
// 		}
// 	}
// }

int		col_inter(int cola, int colb, float r)
{
	int	rgb1[3];
	int	rgb2[3];
	int res[3];

	rgb1[0] = (cola & 0xFF0000) >> 16;
	rgb1[1] = (cola & 0xFF00) >> 8;
	rgb1[2] = (cola & 0xFF);
	rgb2[0] = (colb & 0xFF0000) >> 16;
	rgb2[1] = (colb & 0xFF00) >> 8;
	rgb2[2] = (colb & 0xFF);
	res[0] = rgb1[0] * r + rgb2[0] * (1.0f - r);
	res[1] = rgb1[1] * r + rgb2[1] * (1.0f - r);
	res[2] = rgb1[2] * r + rgb2[2] * (1.0f - r);
	return ((res[0] << 16) + (res[1] << 8) + res[2]);
}

void	put_line(t_data *data, t_v2i p1, int cola, t_v2i p2, int colb)
{
	t_v2f	diff;
	t_v2f	start;
	int		count;
	int		max;

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
	max = count;
	while (count)
	{
		my_mlx_pixel_put(data, (t_v2i){start[0], start[1]}, col_inter(cola, colb, (float)count / max));
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


// void draw_grid(t_data *data, int lines, t_v2i size, t_v2i pos)
// {
//     int i;
//     int x;
//     int y;
//     int spacing;
//     spacing = size[0] / (lines + 1);
//     x = 0;
//     y = 0;
//     i = 1;
//     while(i <= lines || y <= lines)
//     {
//         x = i * spacing + pos[0];
//         y = i * spacing + pos[1];
//         put_line(data, (t_v2i){x, pos[1]}, (t_v2i){x, size[1] + pos[1]}, WHITE);
//         put_line(data, (t_v2i){pos[0], y}, (t_v2i){size[0] + pos[0], y}, WHITE);
//         i++;
//         y++;
//     }
// }


void draw_grid_rotated(t_data *data, int lines, t_v2i size, t_v2i pos)
{
    int i = 1;
    int x;
    int y;
    int spacing;
    t_v2i end;

    // Calculate the spacing between grid lines
    spacing = size[0] / (lines + 1);

    // Define the rotation angle in radians
    double angle = M_PI / 4.0;

    // Loop through the grid lines
    while (i <= lines) {

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
        put_line(data, start, WHITE, end, WHITE);

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
        put_line(data, start, WHITE, end, WHITE);
        
        i++;
    }
}

void	draw_rect(t_data *data, t_v2i start, t_v2i dim, int color)
{
	t_v2i	pos;

	pos[1] = 0;
	while(pos[1] < dim[1])
	{
		pos[0] = 0;
		while(pos[0] < dim[0])
		{
			my_mlx_pixel_put(data, start + pos, 0x00FFFFFF);
    		pos[0]++;
		}
    	pos[1]++;
	}
}





void init_map(int map[SIZE_X][SIZE_Y])
{

	//srand(time(NULL));
 	/* generate 0 or 1 randomly: probability 50%*/

	//fill map with random values
	for (int y = 0; y < SIZE_Y; y++)
		for (int x = 0; x < SIZE_X; x++)
		{
			map[x][y] = rand() % 24 - 12;
		}

	//print map
	printf("\n\n--------------------------------\n");
	for (int y = 0; y < SIZE_Y; y++)
	{
		for (int x = 0; x < SIZE_X; x++)
		{
			printf("%2d ", map[x][y]);
		}
		printf("\n");
	}
	printf("---------------------------------\n\n");

}


int	get_map(int map[SIZE_X][SIZE_Y], t_v2i size, t_v2i pos)
{
	if (pos[0] < 0 || pos[1] < 0 || pos[0] >= size[0] || pos[1] >= size[1])
		return (0);
	return (map[pos[0]][pos[1]]);
}

t_v2f	ft_orto(t_v2f vec, float z)
{
	t_v2f	ret;
	float	s;
	float	c;

	s = sinf(M_PI_4);
	c = cosf(M_PI_4);
	ret[0] = vec[0] * c - vec[1] * s;
	ret[1] = vec[0] * s + vec[1] * c;
	ret[1] -= z;
	ret[1] *= 0.5f;
	return (ret);
}

static t_v2i	v2ftoi(t_v2f vec)
{
	return ((t_v2i){vec[0], vec[1]});
}

void draw_map(t_data *img, int map[SIZE_X][SIZE_Y])
{
	t_v2i	pos;
	t_v2f	off;
	int		val[3];
	t_v2f	proj[3];
	t_v2f	res[3];

	pos[1] = 0;
	while (pos[1] < SIZE_Y) 
	{
		pos[0] = 0;
		while (pos[0] < SIZE_X) 
		{
			val[0] = get_map(map, (t_v2i){SIZE_X, SIZE_Y}, pos);

			val[1] = get_map(map, (t_v2i){SIZE_X, SIZE_Y}, pos + (t_v2i){1, 0});
			val[2] = get_map(map, (t_v2i){SIZE_X, SIZE_Y}, pos + (t_v2i){0, 1});

			off = (t_v2f){SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};

			proj[0] = (t_v2f){pos[0], pos[1]};
			proj[1] = proj[0] + (t_v2f){1, 0};
			proj[2] = proj[0] + (t_v2f){0, 1};

			res[0] = off + ft_orto(proj[0], val[0] * MULTIPLIER) * ZOOM;
			res[1] = off + ft_orto(proj[1], val[1] * MULTIPLIER) * ZOOM;
			res[2] = off + ft_orto(proj[2], val[2] * MULTIPLIER) * ZOOM;

			put_line(img, v2ftoi(res[0]), 0x00AEAE, v2ftoi(res[1]), 0xAE80AE);
			put_line(img, v2ftoi(res[0]), 0x00AEAE, v2ftoi(res[2]), 0xAE80AE);
			
			pos[0]++;
		}
		pos[1]++;
	}
}




int    main(void)
{
	void	*mlx;
	void	*mlx_win;
	t_data	img;
	//int		map[SIZE_X][SIZE_Y];

	int map[10][10] = { 
				{0,0,0,0,0,0,0,0,0,0},
				{0,0,0,0,0,0,0,0,0,0},
				{0,0,0,0,0,0,0,0,0,0},
				{0,0,0,0,0,0,-15,0,0,0},
				{156,20,2,10,-50,50,20,4,12,65},
				{0,0,0,0,0,0,0,0,0,0},
				{0,0,0,0,0,0,0,0,0,0},
				{0,0,0,0,0,0,0,0,0,0},
				{0,0,0,0,0,0,0,0,0,0},
				{0,0,0,0,0,0,0,0,0,0}};

init_map(map);

	mlx = mlx_init();
	mlx_win = mlx_new_window(mlx, 1080, 1080, "fdf");
	img.img = mlx_new_image(mlx, 1080, 1080);
	img.addr = mlx_get_data_addr(img.img, &img.bits_per_pixel, &img.line_length, &img.endian);
	mlx_key_hook(mlx_win, key_hook, &img);



	//draw_grid_rotated(&img, 10, (t_v2i){600, 600}, (t_v2i){500, 300});



	draw_map(&img, map);

	

	mlx_put_image_to_window(mlx, mlx_win, img.img, 0, 0);
	//usleep(100000);
	
	mlx_loop(mlx);
}
