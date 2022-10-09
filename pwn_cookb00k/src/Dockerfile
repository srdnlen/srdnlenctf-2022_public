FROM ubuntu:22.04

WORKDIR /chal

COPY ./cookb00k /chal/
COPY ./banner_updated.txt /chal/
COPY ./flag.txt /chal/

RUN apt update -y && apt install -y socat && chmod +x /chal/cookb00k

ENTRYPOINT ["socat", "-t", "300", "-T", "30", "TCP4-LISTEN:5555,reuseaddr,fork", "EXEC:/chal/cookb00k"]