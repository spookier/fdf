#ifndef FT_PRINTF_H
# define FT_PRINTF_H

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "mlx_linux/mlx.h"

# define ESC		0xFF1B


#define M_PI       3.14159265358979323846


# define WHITE 		0xFFFFFF
# define RED 0xFF0000

typedef struct	s_data {
	void	*img;
	char	*addr;
	int		bits_per_pixel;
	int		line_length;
	int		endian;
}				t_data;

#endif
