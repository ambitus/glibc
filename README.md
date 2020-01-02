# Glibc for z/OS

## Description

This is a fork of Glibc that has been modified to run on z/OS.
Currently, only the Metal C calling convention and ELF object files are
supported.

### Notable changes

* A new target triple has been added to uniquely identify z/OS targets using
  Glibc, s390x-ibm-zos-gnu. It should be specified as the host when building a
  libc that should run on z/OS.

## Contributing

We welcome contributions from any and all contributors. However, since we plan
to merge this project into the original Glibc project, all contributions are
subject to the following conditions:

* All prospective contributors MUST abide by all legal requirements put
  on contributors to the original Glibc project. That includes, but its not
  limited to, the requirement to submit appropriate copyright assignment
  paperwork. Contact assign@gnu.org and mention your employment status and
  that you would like to contribute to Glibc. They should send the
  appropriate forms.
* All prospective contributors MUST explicitly agree to allow their
  contributions to be merged into the main Glibc project without notice, at
  any time.
