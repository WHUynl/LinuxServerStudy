#定义变量
src = $(wildcard ./*.c)
objs = $(patsubst %.c,%.o,$(src))
target = app5
$(target):$(src)
	$(CC) $(src) -o $(target)

%.o:%.c
	$(CC) -c $< -o $@

.PHONY:clean
clean:
	rm $(objs) -f