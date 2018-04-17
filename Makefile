EXTRA_CFLAGS += $(shell rtai-config --module-cflags)

obj-m += rt_enc.o

rt_enc-objs := enc.o
