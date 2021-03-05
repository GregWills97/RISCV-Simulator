CC=g++
CFLAGS=-Iincludes --std=c++11
ROOTDIR = .
OBJDIR = $(ROOTDIR)/obj
SRCDIR = $(ROOTDIR)/src
OBJS = $(OBJDIR)/system.o $(OBJDIR)/main.o $(OBJDIR)/ports.o $(OBJDIR)/cpu.o

TARGET = demo

.PHONY : clean run all

all : $(TARGET) run

$(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET) : $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

clean :
	rm -rf $(TARGET)
	rm -rf $(OBJDIR)/*.o
	rm output.txt

run :
	./$(TARGET) >> output.txt
