    CC=gcc  # 直接使用gcc代替cc
all:
	$(CC) -o anon anon.c
	$(CC) -o share share.c

run:
	cp page.bak page.txt
	./anon

clean:
	rm -f anon share page.txt
