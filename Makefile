OBJS=connection.o server.o main.o service.o 
TARGET=reverse 

OBJS2=client.o
TARGET2=client
	
CFLAGS=-Wall -I . -std=c99 -g

all: $(TARGET) $(TARGET2)

-include $(OBJS:.o=.d)

$(TARGET): $(OBJS)
	$(CC) $^ -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@
	$(CC) -MM $(CFLAGS) $< > $*.d

clean:
	rm -f $(TARGET) $(TARGET2) $(OBJS) $(OBJS2) $(OBJS:.o=.d) $(OBJS2:.o=.d)
