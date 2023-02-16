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



void draw_grid(t_data *data, int lines, t_v2i pos)
{
    int i;
    int x;
    int y;
    int spacing;

    spacing = pos[0] / (lines + 1);  
    x = 0;
    y = 0;
    i = 1;
    while(i <= lines || y <= lines)
    {
        x = i * spacing;
        y = i * spacing;
        put_line(data, (t_v2i){x, 0}, (t_v2i){x, pos[1]}, WHITE);
        put_line(data, (t_v2i){0, y}, (t_v2i){pos[0], y}, WHITE);
        i++;
        y++;
    }
}



void draw_grid2(t_data *data, int lines, t_v2i size)
{
    int i;
    int x;
    int y;
    int spacing;

    spacing = size[0] / (lines + 1);  // use pos.x instead of pos[0]
    x = 0;
    y = 0;
    i = 1;
    while(i <= lines || y <= lines)
    {
        x = i * spacing;
        y = i * spacing;
        put_line(data, (t_v2i){x + size[0], size[1]}, (t_v2i){x + size[0], size[1] + size[1]}, WHITE);
        put_line(data, (t_v2i){size[0], y + size[1]}, (t_v2i){size[0] + size[0], y + size[1]}, WHITE); 
        i++;
        y++;
    }
}



void draw_grid3(t_data *data, int lines, t_v2i pos, int x_offset, int y_offset)
{
    int spacing = pos[0] / (lines + 1);
    int x = x_offset;
    int y = y_offset;
    int i = 1;
    while (i <= lines) {
        put_line(data, (t_v2i){x + pos[0], pos[1]}, (t_v2i){x + pos[0], pos[1] + pos[1]}, WHITE);
        x += spacing;
        i++;
    }
    i = 1;
    while (i <= lines) {
        put_line(data, (t_v2i){pos[0], y + pos[1]}, (t_v2i){pos[0] + pos[0], y + pos[1]}, WHITE);
        y += spacing;
        i++;
    }
}





// void draw_grid(t_data *data, int lines, t_v2i pos)
// {
// 	int i;
// 	int x;
// 	int y;
// 	int spacing;
	
// 	spacing = SCREEN_WIDTH / (lines + 1);
// 	x = 0;
// 	y = 0;
// 	i = 1;
// 	while(i <= lines || y <= lines)
// 	{
// 		x = i * spacing;
// 		y = i * spacing;
// 		put_line(data, (t_v2i){x, 0}, (t_v2i){x, pos[1]}, WHITE);
// 		put_line(data, (t_v2i){0, y}, (t_v2i){pos[0], y}, WHITE);
// 		i++;
// 		y++;
// 	}
// }





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


	// map = create_tab(SIZE_X, SIZE_Y);
	// fill_tab(map, SIZE_X, SIZE_Y);

	//put_line(&img, (t_v2i){0, 80}, (t_v2i){500, 500}, WHITE);
	

	draw_grid(&img, 8, (t_v2i){300,300});
	//draw_grid2(&img, 10, (t_v2i){500, 500});
	//draw_grid3(&img, 10, (t_v2i){300, 300}, 100, 50);
	mlx_put_image_to_window(mlx, mlx_win, img.img, 0, 0);
	//usleep(100000);
	
	mlx_loop(mlx);
}




// cc main.c -Lmlx_linux -lmlx_Linux -L/usr/lib -Imlx_Linux -lXext -lX11 -lm && ./a.out