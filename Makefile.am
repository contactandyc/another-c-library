AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src examples

.PHONY: new-change bump-version

dist_doc_DATA = README.md AUTHORS NEWS.md CHANGELOG.md LICENSE

new-change:
	@changie new

bump-version:
	@changie bump
