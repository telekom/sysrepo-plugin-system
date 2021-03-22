# syntax=docker/dockerfile:1.0.0-experimental
FROM sysrepo/sysrepo-netopeer2:latest

MAINTAINER juraj.vijtiuk@sartura.hr

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y\
	systemd \
	ntp \
	ssh

WORKDIR /opt/dev

RUN \
	mkdir -m 0600 -p /root/.ssh && ssh-keyscan lab.sartura.hr >> ~/.ssh/known_hosts

RUN \
	--mount=type=secret,id=sshkey,dst=/root/.ssh/id_ed25519 git clone git@lab.sartura.hr:sysrepo/sysrepo-plugin-general.git

Run \
	cd sysrepo-plugin-general && mkdir build && cd build && \
	git checkout master && \
	cmake -DCMAKE_BUILD_TYPE="Debug" .. && \
	make -j2

RUN \
	sysrepoctl -i /opt/dev/sysrepo-plugin-general/yang/iana-crypt-hash\@2014-08-06.yang && \
	sysrepoctl -i /opt/dev/sysrepo-plugin-general/yang/ietf-system\@2014-08-06.yang && \
	sysrepoctl -c ietf-system -e ntp && \
	sysrepoctl -c ietf-system -e timezone-name

CMD ["/sbin/init"]

