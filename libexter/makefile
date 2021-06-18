

#本地文件目录名
LOCAL_DIR = libexter
OBJS = digcacl md5
#用到的链接库： 


all:
	@for x in $(OBJS);do \
		gcc -o ${LOCAL_DIR}-$${x}.o -c $${x}.c -I ${HOME_DIR} ;\
	done
	@cp *.o ${HOME_DIR}/obj
	@rm -rf *.o
			
.PHONY:clear
clear:
	@rm -rf *.o *.s


