FROM ubuntu:20.04

RUN apk update && apk upgrade && apk add bash
CMD ["bash", "./build.sh"]
