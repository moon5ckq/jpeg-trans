# Author: mlckq <moon5ckq@gmail.com>
# File: Makefile
# Create Date: 2014-09-05 12:48

TARGET = example
all: $(TARGET)

INCLUDE = -I..
LIBS = 	-ljpeg
OPTFLAGS = -O2
CFLAGS = \
	-std=c++11 \
	$(INCLUDE) \
	$(LIBS) \
	$(OPTFLAGS)

$(TARGET): example.cpp ../jpeg_trans.cpp
	g++ $^ $(CFLAGS) -o $@

run: $(TARGET)
	@echo start running '$(TARGET)'
	@./$(TARGET) w.jpg

clean:
	rm -f $(TARGET) w.jpg

.PHONY: run clean


