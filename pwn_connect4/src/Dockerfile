FROM ubuntu:22.04

RUN apt-get update
RUN apt-get install -y socat gcc-9 make libstdc++6

WORKDIR /chal

COPY ./flags.txt /chal/flag.txt
COPY ./connect4 /chal/connect4

RUN chmod +rx /chal/connect4

ENTRYPOINT ["socat", "-t", "300", "-T", "30", "TCP4-LISTEN:5555,reuseaddr,nodelay,fork", "EXEC:/chal/connect4"]
