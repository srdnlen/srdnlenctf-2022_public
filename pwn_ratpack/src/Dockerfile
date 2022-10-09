FROM ubuntu:22.10

RUN apt-get update
RUN apt-get install -y socat gcc-9 make libstdc++6

WORKDIR /chal

COPY ./rats /chal/rats
# Add other files here...

RUN chmod +rx /chal/rats

ENTRYPOINT ["socat", "-t", "300", "-T", "30", "TCP4-LISTEN:5555,reuseaddr,nodelay,fork", "EXEC:/chal/rats"]