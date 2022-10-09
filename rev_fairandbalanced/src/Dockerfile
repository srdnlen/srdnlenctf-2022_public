FROM ubuntu:22.10

RUN apt-get update
RUN apt-get install -y socat gcc-9 make libstdc++6

WORKDIR /chal

# Not compiling on the image.
COPY ./server /chal/server

# Ensure execution permissions.
RUN chmod +rx /chal/server


ENTRYPOINT ["socat", "-t", "300", "-T", "30", "TCP4-LISTEN:5555,reuseaddr,fork", "EXEC:/chal/server"]