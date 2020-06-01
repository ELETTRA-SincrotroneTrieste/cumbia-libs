CONFIG += ordered
TEMPLATE = subdirs
SUBDIRS= quapps \
        qumbiaprojectwizard \
	qumbianewcontrolwizard \
	cuuimake \
        qumbia-reader \
	qumbia-client \
        cumbia \
        la-cumparsita \
        cumbia-component-config

cumbia-component-config.depends quapps
qumbia-client.depends quapps
qumbia-reader.depends quapps
la-cumparsita.depends quapps

