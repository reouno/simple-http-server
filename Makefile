CFLAGS := --std=c11
TARGET = TCPServer

$(TARGET):

.PHONY: clean
clean:
	$(RM) $(TARGET)
