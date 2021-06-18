

# 第一层makefile，配置的HOME_DIR值，将传递到子目录的makefile里
export HOME_DIR = /mnt/hgfs/share/PRO/cwmp2/cpe
#定义 .o 文件存放的目录
OBJ_DIR = obj

#本地文件夹
LOCAL_DIR = cpe

#包含源文件的子目录（或外部目录）
SUBDIR = ../lib ../http ../cwmp ../libexter
OBJS = main cpe


all:
	@for x in $(OBJS);do \
		gcc -o ${LOCAL_DIR}-$${x}.o -c $${x}.c -I ${HOME_DIR} -lpthread ;\
	done
	@cp *.o ${HOME_DIR}/obj
	@rm -rf *.o
	@for x in $(SUBDIR);do \
		cd ${HOME_DIR}/$${x} && ${MAKE};\
	done
	@cd ${HOME_DIR}/${OBJ_DIR} && ${MAKE}

.PHONY:clear
clear:
	@for x in $(SUBDIR);do \
		cd ${HOME_DIR}/$${x} && ${MAKE} clear;\
	done
	@cd ${HOME_DIR}/${OBJ_DIR} && ${MAKE} clear


	
