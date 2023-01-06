CXX = gcc

main = final.c
files = sockop.c joystick.c
CFLAG = -lpthread
all: final

final:
	$(CXX) -o final $(main) $(files) $(CFLAG)

clean:
	rm -rf final

.PHONY: all clean #代表這些任務一定要執行
