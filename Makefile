FILES := ip-locator.c cJSON/cJSON.c
CFLAGS := -lcurl -lm

ip-locator: ip-locator.c
	gcc ${FILES} -o ip-locator ${CFLAGS}

run: ip-locator
	./ip-locator

clean:
	rm ip-locator