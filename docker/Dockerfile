FROM gcc:8.3

RUN apt-get -y update && apt-get -y install supervisor valgrind cmake libz-dev emacs

ADD run.sh /run.sh
RUN chmod 755 /*.sh
CMD ["/run.sh"]
