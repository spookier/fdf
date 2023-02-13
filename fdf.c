#include "fdf.h"
#include "mlx_int.h"

#define SCREEN_HEIGHT 1920
#define SCREEN_WIDTH 1080

#define SIZE_X 10
#define SIZE_Y 10

 #define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })


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


int	**create_tab(int size_x, int size_y)
{
	int **tab;
	int i;

	i = 0;
	tab = malloc(sizeof(int *) * size_x);
	while(i < size_x)
	{
		tab[i] = malloc(sizeof(int) * size_y);
		i++;
	}
	printf("[OK] MAP MALLOCED!\n");
	return (tab);
}

void fill_tab(int **tab, int size_x, int size_y)
{
	int i;
	int j;

	i = 0;
	while(i < size_x)
	{
		j = 0;
		while(j < size_y)
		{
			tab[i][j] = rand() % 10;
			j++;
		}
		i++;
	}
	printf("[OK] MAP FILLED!\n");
}

// void draw_grid(t_data *img, int **map, t_v2i pos, int space)
// {
// 	int i = 0;
// 	int tmp = space;
// 	space = 0;

// 	//DRAW COLUMNS
// 	while(i < SIZE_X)
// 	{
// 		//bresnahan_put_line(img, (t_v2i){300 + space, 200}, (t_v2i){200 + space, 380}, RED);
// 		bresnahan_put_line(img, (t_v2i){pos[0] + space, pos[1]}, (t_v2i){pos[0] + space, pos[1] + 180}, RED);
// 		i++;
// 		space += tmp;
// 	}

// 	space = 0;
// 	i = 0;

// 	//DRAW ROWS
// 	while(i < SIZE_Y)
// 	{
// 		//bresnahan_put_line(img, (t_v2i){200, 200 + space}, (t_v2i){380, 200 + space}, RED);
// 		bresnahan_put_line(img, (t_v2i){pos[0], pos[1] + space}, (t_v2i){pos[0] + 180, pos[1] + space}, RED);

// 		i++;
// 		space += tmp;
// 	}
// }

// void draw_grid2(t_data *img, int **map, int space)
// {
// 	int i = 0;
// 	int tmp = space;
// 	space = 0;
// 	double rad_30 = 30 * M_PI / 180;
// 	double rad_45 = 45 * M_PI / 180;


// void	put_line(t_data *data, t_v2i p1, t_v2i p2, int color)
// {
// 	t_v2f	diff;
// 	t_v2f	start;
// 	int		count;

// 	diff = (t_v2f){p2[0], p2[1]} - (t_v2f){p1[0], p1[1]};

// 	if (fabsf(diff[0]) > fabsf(diff [1]))
// 	{
// 		count = fabsf(diff[0]);
// 		diff = diff / fabsf(diff[0]);
// 	}
// 	else
// 	{
// 		count = fabsf(diff[1]);
// 		diff = diff / fabsf(diff[1]);
// 	}
// 	start = (t_v2f){p1[0], p1[1]};
// 	//p1[0] = [x] p1[1]=[y]
// 	while (count)
// 	{
// 		my_mlx_pixel_put(data, (t_v2i){start[0], start[1]}, color);
// 		start = start + diff;
// 		count--;
// 	}
// }

void	put_rect(t_data *data, t_v2i start, t_v2i dim, int color)
{
	t_v2i	pos;

	pos[1] = 0;
	while(pos[1] < dim[1])
	{
		pos[0] = 0;
		while(pos[0] < dim[0])
		{
			my_mlx_pixel_put(data, start + pos, color);
    		pos[0]++;
		}
    	pos[1]++;
	}
}

// void draw_grid(t_data *img, int **map, t_v2i pos, int space)
// {
// 	int i;
// 	int tmp;
// 	i = 0;
// 	tmp = space;
// 	space = 0;
// 	double rad_30 = 30 * M_PI / 180;
// 	double rad_45 = 45 * M_PI / 180;
// 	// DRAW COLUMNS
// 	while (i < SIZE_X)
// 	{
// 		int x1 = pos[0] + space;
// 		int y1 = pos[1];
// 		int x2 = pos[0] + space;
// 		int y2 = pos[1] + 180;
// 		int new_x1 = x1 * cos(rad_30) + y1 * sin(rad_30);
// 		int new_y1 = y1 * cos(rad_45) - x1 * sin(rad_45) * cos(rad_30);
// 		int new_x2 = x2 * cos(rad_30) + y2 * sin(rad_30);
// 		int new_y2 = y2 * cos(rad_45) - x2 * sin(rad_45) * cos(rad_30);
// 		bresnahan_put_line(img, (t_v2i){new_x1, new_y1}, (t_v2i){new_x2, new_y2}, WHITE);
// 		i++;
// 		space += tmp;
// 	}
// 	space = 0;
// 	i = 0;
// 	// DRAW ROWS
// 	while (i < SIZE_Y)
// 	{
// 		int x1 = pos[0];
// 		int y1 = pos[1] + space;
// 		int x2 = pos[0] + 180;
// 		int y2 = pos[1] + space;
// 		int new_x1 = x1 * cos(rad_30) + y1 * sin(rad_30);
// 		int new_y1 = y1 * cos(rad_45) - x1 * sin(rad_45) * cos(rad_30);
// 		int new_x2 = x2 * cos(rad_30) + y2 * sin(rad_30);
// 		int new_y2 = y2 * cos(rad_45) - x2 * sin(rad_45) * cos(rad_30);
// 		bresnahan_put_line(img, (t_v2i){new_x1, new_y1}, (t_v2i){new_x2, new_y2}, WHITE);
// 		i++;
// 		space += tmp;
// 	}
// }


void draw_grid(t_data *img, int **map, t_v2i pos, int space, double scale)
{
	int i;
	int tmp;

	i = 0;
	tmp = space;
	space = 0;
	double rad_30 = 55 * M_PI / 180;
	double rad_45 = 75 * M_PI / 180;

	// DRAW COLUMNS
	while (i < SIZE_X)
	{
		int x1 = pos[0] + space * scale;
		int y1 = pos[1];
		int x2 = pos[0] + space * scale;
		int y2 = pos[1] + 180 * scale;

		int new_x1 = x1 * cos(rad_30) + y1 * sin(rad_30);
		int new_y1 = y1 * cos(rad_45) - x1 * sin(rad_45) * cos(rad_30);
		int new_x2 = x2 * cos(rad_30) + y2 * sin(rad_30);
		int new_y2 = y2 * cos(rad_45) - x2 * sin(rad_45) * cos(rad_30);

		bresnahan_put_line(img, (t_v2i){new_x1, new_y1}, (t_v2i){new_x2, new_y2}, WHITE);
		i++;
		space += tmp;
		
	}

	space = 0;
	i = 0;

	// DRAW ROWS
	while (i < SIZE_Y)
	{
		int x1 = pos[0];
		int y1 = pos[1] + space * scale;	
		int x2 = pos[0] + 180 * scale;
		int y2 = pos[1] + space * scale;


		int new_x1 = x1 * cos(rad_30) + y1 * sin(rad_30);
		int new_y1 = y1 * cos(rad_45) - x1 * sin(rad_45) * cos(rad_30);
		int new_x2 = x2 * cos(rad_30) + y2 * sin(rad_30);
		int new_y2 = y2 * cos(rad_45) - x2 * sin(rad_45) * cos(rad_30);

		bresnahan_put_line(img, (t_v2i){new_x1, new_y1}, (t_v2i){new_x2, new_y2}, WHITE);
		i++;
		space += tmp;
	}

}









int    main(void)
{
	void	*mlx;
	void	*mlx_win;
	t_data	img;

	int		**map;

	mlx = mlx_init();
	mlx_win = mlx_new_window(mlx, 1080, 1080, "fdf ");
	img.img = mlx_new_image(mlx, 1080, 1080);
	img.addr = mlx_get_data_addr(img.img, &img.bits_per_pixel, &img.line_length,
								&img.endian);

	map = create_tab(SIZE_X, SIZE_Y);
	fill_tab(map, SIZE_X, SIZE_Y);

	//my_mlx_pixel_put(&img, (t_v2i){5, 5}, 0x00FF0000);

	//put_rect(&img, (t_v2i){200, 200},(t_v2i){500, 500} , RED);
	//bresnahan_put_line(&img, (t_v2i){200, 200}, (t_v2i){700, 500}, RED);

	
	//draw_grid(&img, map, (t_v2i){300, 300}, 20);

	
	//draw_grid(&img, map, (t_v2i){-300, 900}, 20, 2);
	int i = 0;
	int j = 0;
	while (i < 10)
	{
		j = 0;
		while(j < 10)
		{
			printf("map[%d][%d] = %d\n", i, j, map[i][j]);
			j++;
		}
		i++;
	}
	
	draw_grid(&img, map, (t_v2i){-800, 800}, 20, 3);
	
	
	mlx_put_image_to_window(mlx, mlx_win, img.img, 0, 0);
	//usleep(100000);
	
	mlx_loop(mlx);
}




// cc main.c -Lmlx_linux -lmlx_Linux -L/usr/lib -Imlx_Linux -lXext -lX11 -lm && ./a.out
