
#目标文件汇集处，将所有.o文件拼接成目标文件
TARGET_NAME = app
app:*.o
	@gcc -o ${TARGET_NAME} *.o -lpthread 
	cp ${TARGET_NAME} ${HOME_DIR}

.PHONY:clear
clear:
	@rm -rf *.o


	
