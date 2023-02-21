#include <fcntl.h>
#include "get_next_line/get_next_line.h"
#include "stdio.h"
#include "fdf.h"


int main(void)
{
	int fd;
	fd = open("map.txt", O_RDONLY);
	
	printf("RESULT=%s\n", get_next_line(fd));

	close(fd);

}
	