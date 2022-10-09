FROM python:3.10-alpine

WORKDIR /chal

COPY ./requirements.txt /chal/
RUN pip3 install -r requirements.txt

COPY ./server.py /chal/
COPY ./templates/ /chal/templates/
COPY ./static/ /chal/static/
# Add other files here...

ENTRYPOINT [ "python3", "server.py" ]
